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

PlanetsRenderer::~PlanetsRenderer() {
    for (int i=0; i<m_planets.size(); i++) {
        delete m_planets.at(i);
    }
}

void PlanetsRenderer::createShaderProgram() {
    m_shader = ResourceLoader::loadShaders(":/shaders/noise.vert", ":/shaders/noise.frag");
    GLuint pos = glGetAttribLocation(m_shader, "position");
    GLuint norm = glGetAttribLocation(m_shader, "normal");

    // Order is moon, earth/mars/sun
    m_planets += Sphere::generate(VERTS_LOW,pos,norm);
    m_planets += Sphere::generate(VERTS_HIGH,pos,norm);
}

void PlanetsRenderer::createFBO(glm::vec2 size) {
    GLRenderWidget::createFBO(&m_FBO, &m_colorAttachment, getTextureID(), size, true);
}

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

void PlanetsRenderer::refresh() {
    randomizeSeed();
}

// GETTERS

int PlanetsRenderer::getTextureID() {
    return 1;
}

GLuint *PlanetsRenderer::getColorAttach() {
    return &m_colorAttachment;
}

GLuint *PlanetsRenderer::getFBO() {
    return &m_FBO;
}

glm::mat4x4 PlanetsRenderer::getMoonTransformation(float speed) {
    return applyPlanetTrans(speed, m_planetData.at(MOON));
}

// START OF PRIVATE METHODS

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
        m_planets.at(c.shapeIndex)->render();
    }
}

glm::mat4x4 PlanetsRenderer::applyPlanetTrans(float speed, PlanetData trans) {
    return glm::rotate(speed/trans.year, glm::vec3(0,1,0)) *
           glm::translate(trans.place) *
           glm::rotate(speed/trans.day, trans.tilt) *
           glm::scale(glm::vec3(trans.size)) *
           m_renderer->getTransformation().model;
}

void PlanetsRenderer::randomizeSeed() {
    m_seed = frandN();
}

