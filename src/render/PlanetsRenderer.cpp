#include "PlanetsRenderer.h"
#include "ResourceLoader.h"
#include "PlanetDataParser.h"
#include "Sphere.h"
#include "GLMath.h"
#include "GLRenderWidget.h"
#include "Settings.h"

#define DATA_STATIC ":/xml/planetData.xml"

/**
 * @brief Creates the planet data for rendering later
 * @param renderer The GLRenderWidget owning this renderer
 */
PlanetsRenderer::PlanetsRenderer(GLRenderWidget *renderer) {
    m_textureID = -1;
    m_renderer = renderer;
    m_shader = 0;

    // Parse the XML and save the data it creates (after copying to app local data)
    m_file = ResourceLoader::copyFileToLocalData(DATA_STATIC).toStdString();
    parseData();
}

/**
 * @brief Deletes all planet shape data
 */
PlanetsRenderer::~PlanetsRenderer() {
    deleteSpheres();
}

/**
 * @brief Assuming m_file is setup, parses all data in and creates resolutions as needed
 */
void PlanetsRenderer::parseData() {
    deleteSpheres();

    PlanetDataParser parser = PlanetDataParser(m_file.c_str());
    m_resolutions = parser.getResolutions();
    m_planetData = parser.getPlanets();

    createSpheres();
}

/**
 * @brief Adds a new sphere for every resolution in m_resolutions to map to from PlanetData
 */
void PlanetsRenderer::createSpheres() {
    if (m_shader == 0) return;
    for (int i=0; i<m_resolutions.size(); i++) {
        int res = m_resolutions.at(i);
        m_planets.insert(res, new Sphere(m_shader, res, res));
    }
}

/**
 * @brief Deletes all memory used in m_planets and clears the list
 */
void PlanetsRenderer::deleteSpheres() {
    for (int i=0; i<m_planets.size(); i++) {
        delete m_planets.values().at(i);
    }
    m_planets.clear();
}

/**
 * @brief Loads the noise vertex and fragment shaders and creates two planet shapes
 * The first has low vertices, and the second has high vertices, making them suited
 * to different tasks as a result
 */
void PlanetsRenderer::createShaderProgram() {
    m_shader = ResourceLoader::loadShaders(":/shaders/noise.vert", ":/shaders/noise.frag");
    createSpheres();
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
    parseData();
}

/**
 * @brief Returns a unique texture ID to associate with the planets
 * @return an int to add to GL_TEXTURE0 to get a unique texture
 */
int PlanetsRenderer::getTextureID() {
    if (m_textureID < 0) m_textureID = settings.getAndIncrementTextureIndex();
    return m_textureID;
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
    return applyPlanetTrans(speed, m_planetData.value("Moon"));
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
        PlanetData data = m_planetData.values().at(i);
        PlanetColor c = data.color;
        trans.model = applyPlanetTrans(speed, data);
        glUniform4fv(colorLow, 1, &c.low[0]);
        glUniform4fv(colorHigh, 1, &c.high[0]);
        glUniform1f(threshold, c.threshold);
        glUniformMatrix4fv(mvp, 1, GL_FALSE, &trans.getTransform()[0][0]);
        m_planets.value(data.resolution)->renderGeometry();
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
           glm::translate(trans.position) *
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

