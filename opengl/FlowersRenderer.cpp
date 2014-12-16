#include "StarsRenderer.h"
#include "resourceloader.h"
#include "GLRenderer.h"
#include "PlanetsRenderer.h"

#include "flower.h"
#include "cylinder.h"
#include "sphere.h"

#define VARIETY 10 // Types of flowers
#define GARDENSIZE 15 // Num similar flowers per garden
#define RESOLUTION 5 // How many vertices per sphere dimension

FlowersRenderer::FlowersRenderer(PlanetsRenderer *planets, GLRenderer *renderer) {
    m_renderer = renderer;
    m_planets = planets;
}

FlowersRenderer::~FlowersRenderer() {
    for (int i=0; i<m_flowers.size(); i++) {
        delete m_flowers.at(i);
    }
    delete m_flowerSphere;
    delete m_flowerCylinder;
}

void FlowersRenderer::createShaderProgram() {
    m_shader = ResourceLoader::loadShaders(":/shaders/flower.vert", ":/shaders/flower.frag");
}

void FlowersRenderer::createFBO(glm::vec2 size) {
    m_FBO = *m_planets->getFBO();
    m_colorAttachment = *m_planets->getColorAttach();
}

void FlowersRenderer::render(glm::mat4x4 orbit) {
    glUseProgram(m_shader);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glActiveTexture(GL_TEXTURE0+getTextureID());
    glBindTexture(GL_TEXTURE_2D, m_colorAttachment);

    // Draws stars without depth and with blending
    drawFlowers(orbit);

    // Clear
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
}

void FlowersRenderer::refresh() {
    m_flowers.clear();

    m_flowerCylinder = new Cylinder();
    m_flowerCylinder->init(glGetAttribLocation(m_shader, "position"),
                 glGetAttribLocation(m_shader, "normal"));
    m_flowerSphere = new Sphere();
    m_flowerSphere->init(RESOLUTION, RESOLUTION,
                   glGetAttribLocation(m_shader, "position"),
                   glGetAttribLocation(m_shader, "normal"));

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

// GETTERS

int FlowersRenderer::getTextureID() {
    return m_planets->getTextureID();
}

GLuint *FlowersRenderer::getColorAttach() {
    m_colorAttachment = *m_planets->getColorAttach();
    return &m_colorAttachment;
}

GLuint *FlowersRenderer::getFBO() {
    m_FBO = *m_planets->getFBO();
    return &m_FBO;
}

// START OF PRIVATE METHODS

void FlowersRenderer::drawFlowers(glm::mat4x4 orbit) {
    Transforms origTrans = m_renderer->getTransformation();
    Transforms trans = origTrans;

    // iterate through each of the flowers and render the components
    for (int i=0; i<m_flowers.size(); i++) {
        Flower *f = m_flowers.at(i);

        // Stem
        trans.model = orbit * f->cylModel;
        glUniform3fv(glGetUniformLocation(m_shader, "color"), 1, glm::value_ptr(glm::vec3(0, 0.5, 0)));
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "mvp"), 1, GL_FALSE, &trans.getTransform()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "m"), 1, GL_FALSE, &trans.model[0][0]);
        m_flowerCylinder->render();

        // Center sphere
        trans.model = orbit * f->centerModel;
        glUniform3fv(glGetUniformLocation(m_shader, "color"), 1, glm::value_ptr(f->centerColor));
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "mvp"), 1, GL_FALSE, &trans.getTransform()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "m"), 1, GL_FALSE, &trans.model[0][0]);
        m_flowerSphere->render();

        // All petals
        for (int i = 0; i < f->petalCount; i++) {
            trans.model = orbit * f->petalModels[i];
            glUniform3fv(glGetUniformLocation(m_shader, "color"), 1, glm::value_ptr(f->petalColor));
            glUniformMatrix4fv(glGetUniformLocation(m_shader, "mvp"), 1, GL_FALSE, &trans.getTransform()[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(m_shader, "m"), 1, GL_FALSE, &trans.model[0][0]);
            m_flowerSphere->render();
        }
    }
}

