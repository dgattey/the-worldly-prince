#include "PlanetsRenderer.h"
#include "ResourceLoader.h"
#include "Sphere.h"
#include "GLMath.h"
#include "GLRenderWidget.h"

#define VERTS_HIGH 48
#define VERTS_LOW 36
#define PLANET1 0
#define PLANET2 1
#define MOON 0
#define EARTH 1
#define MARS 2
#define SUN 3

/**
 * @brief Creates the planet data for rendering later
 * @param renderer The GLRenderWidget owning this renderer
 */
PlanetsRenderer::PlanetsRenderer(GLRenderWidget *renderer) {
    m_renderer = renderer;
    PlanetColor c;

    // Moon
    c = PlanetColor(glm::vec4(),
                    glm::vec4(0.48, 0.48, 0.5, 0.4), // high - gray
                    -999.f, PLANET1);
    m_planetData += PlanetData(1.0f, glm::vec3(1, 1, 0), 20.f, 500.f, glm::vec3(0), c);

    // Earth
    c = PlanetColor(glm::vec4(0.2, 0.3, 0.8, 0.45), // water - blue
                    glm::vec4(0.08, 0.55, 0.25, 0.15), // earth - green
                    1.72, PLANET2);
    m_planetData += PlanetData(4.3f, glm::vec3(0,3,1), 25.f, 75.f, glm::vec3(10, 0, 15), c);

    // Mars
    c = PlanetColor(glm::vec4(0.6, 0.25, 0.15, 0.3), // red
                    glm::vec4(0.4, 0.2, 0.2, 0.3), // maroon
                    1.32, PLANET2);
    m_planetData += PlanetData(3.6f, glm::vec3(0,3,1), 20.f, 70.f, glm::vec3(-30, 0, 30), c);

    // Sun
    c = PlanetColor(glm::vec4(),
                    glm::vec4(0.7, 0.5, 0, 0.8), // yellow
                    -999.f, PLANET2);
    m_planetData += PlanetData(20.f, glm::vec3(3,0,1), 40.f, 140.f, glm::vec3(-80, 0, 100), c);
}

/**
 * @brief Deletes all planet shape data
 */
PlanetsRenderer::~PlanetsRenderer() {
    for (int i=0; i<m_planets.size(); i++) {
        delete m_planets.at(i);
    }
}

/**
 * @brief Loads the noise vertex and fragment shaders and creates two planet shapes
 * The first has low vertices, and the second has high vertices, making them suited
 * to different tasks as a result
 */
void PlanetsRenderer::createShaderProgram() {
    m_shader = ResourceLoader::loadShaders(":/shaders/noise.vert", ":/shaders/noise.frag");

    // Order is moon, earth/mars/sun
    m_planets += new Sphere(m_shader, VERTS_LOW, VERTS_LOW);
    m_planets += new Sphere(m_shader, VERTS_HIGH, VERTS_HIGH);
}

/**
 * @brief Calls on GLRenderWidget to create an FBO
 * @param size the size of the FBO
 */
void PlanetsRenderer::createFBO(glm::vec2 size) {
    GLRenderWidget::createFBO(&m_FBO, &m_colorAttachment, getTextureID(), size, true);
}

/**
 * @brief Binds to the appropriate variables and renders the scene
 */
void PlanetsRenderer::render() {
    glUseProgram(m_shader);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glActiveTexture(GL_TEXTURE0+getTextureID());
    glBindTexture(GL_TEXTURE_2D, m_colorAttachment);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw the planet with depth and no blending
    drawPlanets();

    // Clear
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
}

/**
 * @brief Changes seed that the shader uses
 */
void PlanetsRenderer::refresh() {
    randomizeSeed();
}

/**
 * @brief Returns a unique texture ID to associate with the planets
 * @return an int to add to GL_TEXTURE0 to get a unique texture
 */
int PlanetsRenderer::getTextureID() {
    return 1;
}

/**
 * @brief Returns the current color attachment
 * @return a GLuint used to attach colors to
 */
GLuint *PlanetsRenderer::getColorAttach() {
    return &m_colorAttachment;
}

/**
 * @brief Returns the FBO associated with this renderer
 * @return a GLuint used as the setup FBO for this renderer
 */
GLuint *PlanetsRenderer::getFBO() {
    return &m_FBO;
}

/**
 * @brief Gets transformation of the moon
 * @param speed The speed of the simulation
 * @return The transformation of the moon as a glm::mat4x4 at speed
 */
glm::mat4x4 PlanetsRenderer::getMoonTransformation(float speed) {
    return applyPlanetTrans(speed, m_planetData.at(MOON));
}

/**
 * @brief Actually draws the planets
 * Passes the correct info to the shaders for everything in planetData.
 * Also calls render() on the shapes themselves
 */
void PlanetsRenderer::drawPlanets() {
    Transforms trans = m_renderer->getTransformation();
    float speed = m_renderer->getRotationalSpeed();

    // Pass shared info to shader first
    glUniform1f(glGetUniformLocation(m_shader, "seed"), m_seed);

    GLuint mvp = glGetUniformLocation(m_shader, "mvp");
    GLuint colorLow = glGetUniformLocation(m_shader, "colorLow");
    GLuint colorHigh = glGetUniformLocation(m_shader, "colorHigh");
    GLuint threshold = glGetUniformLocation(m_shader, "threshold");

    // Render all planets based off their size
    for (int i = 0; i<m_planetData.size(); i++) {
        PlanetColor c = m_planetData.at(i).c;
        trans.model = applyPlanetTrans(speed, m_planetData.at(i));
        glUniform4fv(colorLow, 1, &c.low[0]);
        glUniform4fv(colorHigh, 1, &c.high[0]);
        glUniform1f(threshold, c.threshold);
        glUniformMatrix4fv(mvp, 1, GL_FALSE, &trans.getTransform()[0][0]);
        m_planets.at(c.shapeIndex)->renderGeometry();
    }
}

/**
 * @brief Based on a data object and speed, returns transformation for planet
 * Scales to make bigger/smaller, then rotates around a local axis (day rotation)
 * then translates to starting position and rotates again to represent the year
 * rotation. All data except for year rotational axis comes from the PlanetData
 * @param speed The current simulation speed
 * @param trans The saved data to apply to the planet
 * @return A glm::mat4x4 representing transformations for this planetData at the given speed
 */
glm::mat4x4 PlanetsRenderer::applyPlanetTrans(float speed, PlanetData trans) {
    return glm::rotate(speed/trans.year, glm::vec3(0,1,0)) *
           glm::translate(trans.place) *
           glm::rotate(speed/trans.day, trans.tilt) *
           glm::scale(glm::vec3(trans.size)) *
           m_renderer->getTransformation().model;
}

/**
 * @brief Makes the seed a new random number in [0,1]
 */
void PlanetsRenderer::randomizeSeed() {
    m_seed = frandN();
}

