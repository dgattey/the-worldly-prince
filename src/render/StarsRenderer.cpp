#include "StarsRenderer.h"
#include "ResourceLoader.h"
#include "GLRenderWidget.h"

#define NUMPARTICLES 4000
#define MAXLIFE 150.0f
#define SPREAD 450.0f
#define MINRADIUS 125.0f
#define TAILLENGTH 6
#define STARCOLOR glm::vec3(0.9f, 0.7f, 0.8f)
#define SHOOTINGSTARCOLOR glm::vec3(0.8f, 0.5f, 0.4f)

StarsRenderer::StarsRenderer(GLRenderWidget *renderer) {
    m_renderer = renderer;
    m_particleData = new ParticleData[NUMPARTICLES];
}

StarsRenderer::~StarsRenderer() {
    delete[] m_particleData;
}

void StarsRenderer::createShaderProgram() {
    m_shader = ResourceLoader::loadShaders(":/shaders/star.vert",":/shaders/star.frag");
    GLuint pos = glGetAttribLocation(m_shader, "position");
    GLuint texCoord = glGetAttribLocation(m_shader, "texCoord");

    // Create a particle
    m_particle.init(pos, texCoord);
}

void StarsRenderer::createFBO(glm::vec2 size) {
    GLRenderWidget::createFBO(&m_FBO, &m_colorAttachment, getTextureID(), size, false);
}

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

void StarsRenderer::refresh() {
    for (int i = 0; i<NUMPARTICLES; i++) {
        setupStar(i);
    }
}

// GETTERS

int StarsRenderer::getTextureID() {
    return 0;
}

GLuint *StarsRenderer::getColorAttach() {
    return &m_colorAttachment;
}

GLuint *StarsRenderer::getFBO() {
    return &m_FBO;
}

// START OF PRIVATE METHODS

void StarsRenderer::drawStars() {
    glm::vec3 eye = m_renderer->getCamera().getData().eye;
    Transforms origTrans = m_renderer->getTransformation();
    float globalSpeed = m_renderer->getSimulationSpeed();
    float speed = m_renderer->getRotationalSpeed();
    bool isPaused = m_renderer->getPaused();
    glm::mat4x4 atmosphericRotation = glm::rotate(speed/700.0f, glm::vec3(0,1,-0.75f));

    for(int i =0; i<NUMPARTICLES; i++) {
        Transforms trans = origTrans;
        float x1 = m_particleData[i].pos.x;
        float y1 = m_particleData[i].pos.y;
        float z1 = m_particleData[i].pos.z;

        glm::vec3 n = glm::vec3(0.0f,0.0f,1.0f);
        glm::vec3 np = glm::normalize(glm::vec3(-x1,-y1,-z1));

        glm::vec3 view = glm::normalize(eye);
        if (glm::dot(view, glm::vec3(atmosphericRotation * glm::vec4(np, 1.f))) > 0) { // Backface Culling!!!!!!
            glm::vec3 axis = glm::cross(n, np);
            float angle = glm::acos(glm::dot(n, np) / (glm::length(glm::vec4(n,0.0f)) * glm::length(glm::vec4(np,0.0f))));
            trans.model =
                atmosphericRotation *
                glm::translate(glm::vec3(x1,y1,z1)) *
                glm::rotate(angle, axis) *
                trans.model;

            float alph = m_particleData[i].life / MAXLIFE;

            glUniformMatrix4fv(glGetUniformLocation(m_shader, "mvp"), 1, GL_FALSE, &trans.getTransform()[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(m_shader, "m"), 1, GL_FALSE, &trans.model[0][0]);
            glUniform4f(glGetUniformLocation(m_shader, "color"),
                    m_particleData[i].color.x,
                    m_particleData[i].color.y,
                    m_particleData[i].color.z,
                    alph);

            m_particle.draw();

            // Give shooting stars their tail
            float coeff = 0.5f;
            if (glm::length(m_particleData[i].dir) > 0) {
                for (int dt = 0; dt <= TAILLENGTH; dt++) {
                    glm::vec3 newPos = glm::vec3(m_particleData[i].pos - coeff*dt*m_particleData[i].dir);
                    Transforms temp = origTrans;
                    temp.model =
                            atmosphericRotation *
                            glm::translate(newPos) *
                            glm::rotate(angle, axis) *
                            glm::scale(glm::vec3(1.0f + 1.0f/((float)dt))) *
                            temp.model;
                    glUniformMatrix4fv(glGetUniformLocation(m_shader, "mvp"), 1, GL_FALSE, &temp.getTransform()[0][0]);
                    glUniformMatrix4fv(glGetUniformLocation(m_shader, "m"), 1, GL_FALSE, &temp.model[0][0]);
                    glUniform4f(glGetUniformLocation(m_shader, "color"),
                            (1.0f/((float)dt))*m_particleData[i].color.x,
                            (1.0f/((float)dt))*m_particleData[i].color.y,
                            (1.0f/((float)dt))*m_particleData[i].color.z,
                            alph);
                    m_particle.draw();
                }
            }
        }

        // Only calculate new data if the simulation isn't paused
        if (isPaused) continue;

        m_particleData[i].pos = m_particleData[i].pos + globalSpeed*m_particleData[i].dir;
        m_particleData[i].life += globalSpeed*m_particleData[i].decay;

        if (m_particleData[i].life <= 0 || m_particleData[i].life >= MAXLIFE) {
            m_particleData[i].decay *= -1.0f;
            if (glm::length(glm::vec4(m_particleData[i].dir,0.0f)) > 0 && m_particleData[i].life <= 0) {
                m_particleData[i].pos.x = urand(-SPREAD, SPREAD);
                m_particleData[i].pos.y = urand(-SPREAD, SPREAD);
                m_particleData[i].pos.z = urand(-SPREAD, SPREAD);
            }
        }
    }
}

void StarsRenderer::setupStar(int i) {
    float x,y,z;
    float radius = 0.0f;
    while (radius < MINRADIUS) {
        x = urand(-SPREAD, SPREAD);
        y = urand(-SPREAD, SPREAD);
        z = urand(-SPREAD, SPREAD);
        radius = sqrt(pow(x,2.0f) + pow(y,2.0f) + pow(z,2.0f));
    }
    m_particleData[i].life = urand(0, MAXLIFE);
    m_particleData[i].dir = glm::vec3(0);
    m_particleData[i].pos = glm::vec3(x,y,z);
    m_particleData[i].color = STARCOLOR;
    m_particleData[i].decay = -1;

    // Shooting star
    if (urand(0.0f,1.0f) > 0.97f) {
        m_particleData[i].color = SHOOTINGSTARCOLOR;
        m_particleData[i].dir = glm::vec3(urand(-M_PI, M_PI),urand(-M_PI, M_PI),urand(-M_PI, M_PI));
    }

    // Twinkling (fading out/in)
    else if (urand(0.0f,1.0f) > 0.5f)
        m_particleData[i].decay = 1;
}

