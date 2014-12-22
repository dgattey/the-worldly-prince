#include "StarsRenderer.h"
#include "ResourceLoader.h"
#include "GLRenderWidget.h"
#include "PlanetsRenderer.h"

#include "Flower.h"
#include "Cylinder.h"
#include "Sphere.h"

#define VARIETY 10 // Types of flowers
#define GARDENSIZE 15 // Num similar flowers per garden
#define RESOLUTION 5 // How many vertices per sphere dimension

/**
 * @brief Just sets up the renderers
 * @param planets The planet renderer
 * @param renderer The GLRenderWidget
 */
FlowersRenderer::FlowersRenderer(PlanetsRenderer *planets, GLRenderWidget *renderer) {
    m_textureID = -1;
    m_renderer = renderer;
    m_planets = planets;
}

/**
 * @brief Deletes all flower data and the sphere and cylinder used to draw it
 */
FlowersRenderer::~FlowersRenderer() {
    for (int i=0; i<m_flowers.size(); i++) {
        delete m_flowers.at(i);
    }
    delete m_flowerSphere;
    delete m_flowerCylinder;
}

/**
 * @brief Loads flower shaders (vert and frag)
 */
void FlowersRenderer::createShaderProgram() {
    m_shader = ResourceLoader::loadShaders(":/shaders/flower.vert", ":/shaders/flower.frag");
    m_flowerCylinder = new Cylinder(m_shader, RESOLUTION, RESOLUTION);
    m_flowerSphere = new Sphere(m_shader, RESOLUTION, RESOLUTION);
}

/**
 * @brief Uses the planet renderer's FBO and colorAttachment
 * @param size Unused
 */
void FlowersRenderer::createFBO(glm::vec2 size) {
    m_FBO = *m_planets->getFBO();
    m_colorAttachment = *m_planets->getColorAttach();
}

/**
 * @brief Binds to the right FBO and texture and draws flowers
 */
void FlowersRenderer::render() {
    glUseProgram(m_shader);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glActiveTexture(GL_TEXTURE0+getTextureID());
    glBindTexture(GL_TEXTURE_2D, m_colorAttachment);

    // Draws stars without depth and with blending
    drawFlowers();

    // Clear
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
}

/**
 * @brief Creates new shapes and recreates all flowers using gardens
 */
void FlowersRenderer::refresh() {
    m_flowers.clear();

    for (int i = 0; i < VARIETY; i++) {
        // our template flower
        Flower *f = new Flower();
        m_flowers += f;

        // other similar flowers
        for (int j = 0; j < GARDENSIZE; j++) {
            m_flowers += new Flower(f);
        }
    }
}

/**
 * @brief Returns the texture ID of this renderer
 * @return An int telling the texture ID (from planet renderer)
 */
int FlowersRenderer::getTextureID() {
    return m_planets->getTextureID();
}

/**
 * @brief Returns the color attachment of this renderer
 * @return A GLuint telling the attachment (from planet renderer)
 */
GLuint *FlowersRenderer::getColorAttach() {
    m_colorAttachment = *m_planets->getColorAttach();
    return &m_colorAttachment;
}

/**
 * @brief Returns the FBO of this renderer
 * @return A GLuint telling the FBO (from planet renderer)
 */
GLuint *FlowersRenderer::getFBO() {
    m_FBO = *m_planets->getFBO();
    return &m_FBO;
}

/**
 * @brief Draws all flowers using same shapes, but different colors and mvp/models
 */
void FlowersRenderer::drawFlowers() {
    float speed = m_renderer->getRotationalSpeed();
    glm::mat4x4 orbit = m_planets->getMoonTransformation(speed);
    Transforms trans = m_renderer->getTransformation();

    // iterate through each of the flowers and render the components
    for (int i=0; i<m_flowers.size(); i++) {
        Flower *f = m_flowers.at(i);

        // Stem
        trans.model = orbit * f->cylModel;
        glUniform3fv(glGetUniformLocation(m_shader, "color"), 1, glm::value_ptr(STEMCOLOR));
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "mvp"), 1, GL_FALSE, &trans.getTransform()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "m"), 1, GL_FALSE, &trans.model[0][0]);
        m_flowerCylinder->renderGeometry();

        // Center sphere
        trans.model = orbit * f->centerModel;
        glUniform3fv(glGetUniformLocation(m_shader, "color"), 1, glm::value_ptr(f->centerColor));
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "mvp"), 1, GL_FALSE, &trans.getTransform()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "m"), 1, GL_FALSE, &trans.model[0][0]);
        m_flowerSphere->renderGeometry();

        // All petals
        for (int i = 0; i < f->petalCount; i++) {
            trans.model = orbit * f->petalModels[i];
            glUniform3fv(glGetUniformLocation(m_shader, "color"), 1, glm::value_ptr(f->petalColor));
            glUniformMatrix4fv(glGetUniformLocation(m_shader, "mvp"), 1, GL_FALSE, &trans.getTransform()[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(m_shader, "m"), 1, GL_FALSE, &trans.model[0][0]);
            m_flowerSphere->renderGeometry();
        }
    }
}

