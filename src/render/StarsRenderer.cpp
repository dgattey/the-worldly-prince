#include "StarsRenderer.h"
#include "ResourceLoader.h"
#include "GLRenderWidget.h"
#include "Settings.h"

#define NUMPARTICLES 4000
#define MAXLIFE 150.0f
#define SPREAD 450.0f
#define MINRADIUS 125.0f
#define TAILLENGTH 6
#define TAILCONTRIB 0.5f
#define STARCOLOR glm::vec3(0.9f, 0.7f, 0.8f)
#define SHOOTINGCOLOR glm::vec3(0.8f, 0.5f, 0.4f)
#define SHOOTINGTHRESHOLD 0.97f
#define TWINKLINGTHRESHOLD 0.5f

/**
 * @brief Saves GLRenderWidget and makes a new particle data array
 * @param renderer The GLRenderWidget running everything
 */
StarsRenderer::StarsRenderer(GLRenderWidget *renderer) {
    m_textureID = -1;
    m_renderer = renderer;
    m_starData = new ParticleData[NUMPARTICLES];
}

/**
 * @brief Simply deletes all particle data
 */
StarsRenderer::~StarsRenderer() {
    delete[] m_starData;
}

/**
 * @brief Creates a particle and loads the star shaders, passing the right attribs in
 */
void StarsRenderer::createShaderProgram() {
    m_shader = ResourceLoader::loadShaders(":/shaders/star.vert",":/shaders/star.frag");
    GLuint pos = glGetAttribLocation(m_shader, "position");
    GLuint texCoord = glGetAttribLocation(m_shader, "texCoord");

    // Create a particle
    m_particle.init(pos, texCoord);
}

/**
 * @brief Creates a new FBO for this renderer
 * @param size The FBO size
 */
void StarsRenderer::createFBO(glm::vec2 size) {
    GLRenderWidget::createFBO(&m_FBO, &m_colorAttachment, getTextureID(), size, false);
}

/**
 * @brief Binds to the right stuff and enables blend/no depth and draws stars
 */
void StarsRenderer::render() {
    glUseProgram(m_shader);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glActiveTexture(GL_TEXTURE0+getTextureID());
    glBindTexture(GL_TEXTURE_2D, m_colorAttachment);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);

    // Draws stars without depth and with blending
    drawStars();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    // Clear
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
}

/**
 * @brief Create entirely different stars for all of the particles
 */
void StarsRenderer::refresh() {
    for (int i = 0; i<NUMPARTICLES; i++) {
        setupStar(i);
    }
}

/**
 * @brief Gets the texture ID associated with this renderer
 * @return An int used to add to GL_TEXTURE0 to get a unique texture
 */
int StarsRenderer::getTextureID() {
    if (m_textureID < 0) m_textureID = settings.getAndIncrementTextureIndex();
    return m_textureID;
}

/**
 * @brief Returns the color attachment for this renderer
 * @return A GLuint representing where to attach to to get colors
 */
GLuint *StarsRenderer::getColorAttach() {
    return &m_colorAttachment;
}

/**
 * @brief Returns the FBO for this renderer
 * @return A GLuint for the frame buffer object
 */
GLuint *StarsRenderer::getFBO() {
    return &m_FBO;
}

/**
 * @brief Renders all stars with some optimizations
 * Gets saved data, then loops through all particle data and checks for
 * viewability. If outside viewport, culls the drawing of them. Passes
 * the right shader variables, and draws the particle. If a shooting
 * star, renders the tail too.
 */
void StarsRenderer::drawStars() {
    glm::vec3 eye = glm::normalize(m_renderer->getCamera().getData().eye);
    bool isPaused = m_renderer->getPaused();
    glm::mat4x4 atmosphericRotation = getAtmosphericRotation();

    for(int i =0; i<NUMPARTICLES; i++) {
        glm::vec3 np = glm::normalize(-m_starData[i].pos);

        // Manual backface culling
        if (glm::dot(eye, glm::vec3(atmosphericRotation * glm::vec4(np, 1.f))) > 0) {
            glm::vec3 n = glm::vec3(0.0f,0.0f,1.0f);
            glm::vec3 axis = glm::cross(n, np);
            float angle = glm::acos(glm::dot(n, np) / (glm::length(glm::vec4(n,0.0f)) * glm::length(glm::vec4(np,0.0f))));

            // Actual drawing
            drawBody(i, angle, axis);
            if (isShootingStar(i)) drawTail(i, angle, axis);
        }

        // Only calculate new data if the simulation isn't paused
        if (!isPaused) calculateData(i);
    }
}

/**
 * @brief Draws one celestial body given star index, angle, and axis
 * @param i The index of the star
 * @param angle The angle to rotate by to face user
 * @param axis The axis of rotation to face user
 */
void StarsRenderer::drawBody(int i, float angle, glm::vec3 axis) {
    // Transformation computation
    Transforms trans = m_renderer->getTransformation();
    trans.model =
            getAtmosphericRotation() *
            glm::translate(m_starData[i].pos) *
            glm::rotate(angle, axis) *
            trans.model;

    // Pass shader info
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "mvp"), 1, GL_FALSE, &trans.getTransform()[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "m"), 1, GL_FALSE, &trans.model[0][0]);
    glUniform4f(glGetUniformLocation(m_shader, "color"),
            m_starData[i].color.x,
            m_starData[i].color.y,
            m_starData[i].color.z,
            m_starData[i].life / MAXLIFE);

    // Draw it!
    m_particle.draw();
}

/**
 * @brief Draws the tail of a shooting star
 * @param i The index of the star to draw the tail for
 * @param angle The angle to move in so the particle is facing the user
 * @param axis The axis about which to rotate the particle
 */
void StarsRenderer::drawTail(int i, float angle, glm::vec3 axis) {
    glm::mat4x4 atmosphericRotation = getAtmosphericRotation();
    float alpha = m_starData[i].life / MAXLIFE;

    // For length of tail, calculate new offset and scale
    for (int dt = 0; dt <= TAILLENGTH; dt++) {
        float contrib = 1.0f/((float)dt);
        glm::vec3 newPos = glm::vec3(m_starData[i].pos - TAILCONTRIB*dt*m_starData[i].dir);

        // Transformation
        Transforms temp = m_renderer->getTransformation();
        temp.model = atmosphericRotation *
                     glm::translate(newPos) *
                     glm::rotate(angle, axis) *
                     glm::scale(glm::vec3(1.0f + contrib)) *
                     temp.model;

        // Shader info
        glm::vec3 color = contrib*m_starData[i].color;
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "mvp"), 1, GL_FALSE, &temp.getTransform()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "m"), 1, GL_FALSE, &temp.model[0][0]);
        glUniform4f(glGetUniformLocation(m_shader, "color"), color.x, color.y, color.z, alpha);

        // Draw it!
        m_particle.draw();
    }
}

/**
 * @brief Calculates new position/life of star i
 * @param i The index into the particle data
 */
void StarsRenderer::calculateData(int i) {
    float globalSpeed = m_renderer->getSimulationSpeed();
    m_starData[i].pos = m_starData[i].pos + globalSpeed*m_starData[i].dir;
    m_starData[i].life += globalSpeed*m_starData[i].decay;

    // If the life is below min or above max, reset position
    if (m_starData[i].life <= 0 || m_starData[i].life >= MAXLIFE) {
        m_starData[i].decay *= -1.0f;
        if (isShootingStar(i) && m_starData[i].life <= 0) {
            m_starData[i].pos.x = urand(-SPREAD, SPREAD);
            m_starData[i].pos.y = urand(-SPREAD, SPREAD);
            m_starData[i].pos.z = urand(-SPREAD, SPREAD);
        }
    }
}

/**
 * @brief Creates a star from scratch
 * @param The index into the particleData array
 */
void StarsRenderer::setupStar(int i) {
    float x,y,z;
    float radius = 0.0f;
    while (radius < MINRADIUS) {
        x = urand(-SPREAD, SPREAD);
        y = urand(-SPREAD, SPREAD);
        z = urand(-SPREAD, SPREAD);
        radius = sqrt(pow(x,2.0f) + pow(y,2.0f) + pow(z,2.0f));
    }
    m_starData[i].life = urand(0, MAXLIFE);
    m_starData[i].dir = glm::vec3(0);
    m_starData[i].pos = glm::vec3(x,y,z);
    m_starData[i].color = STARCOLOR;
    m_starData[i].decay = -1;

    // Shooting star
    if (urand(0.0f,1.0f) > SHOOTINGTHRESHOLD) {
        m_starData[i].color = SHOOTINGCOLOR;
        m_starData[i].dir = glm::vec3(urand(-M_PI, M_PI),urand(-M_PI, M_PI),urand(-M_PI, M_PI));
    }

    // Twinkling (fading out/in)
    else if (urand(0.0f,1.0f) > TWINKLINGTHRESHOLD)
        m_starData[i].decay = 1;
}

/**
 * @brief Returns the atmospheric rotation of the stars based on speed
 * @return The glm::mat4x4 representing the rotation for the current rotational speed
 */
glm::mat4x4 StarsRenderer::getAtmosphericRotation() {
    return glm::rotate(m_renderer->getRotationalSpeed()/700.0f,
                       glm::vec3(0,1,-0.75f));
}

/**
 * @brief Checks whether a given star is a shooting star
 * @param i The index of the star
 * @return If the direction is something other than static
 */
bool StarsRenderer::isShootingStar(int i) {
    return glm::length(m_starData[i].dir) > 0;
}

