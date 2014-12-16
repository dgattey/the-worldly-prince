#include "StarsRenderer.h"
#include "resourceloader.h"
#include "GLRenderer.h"

StarsRenderer::StarsRenderer(GLRenderer *renderer) {
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
    GLRenderer::createFBO(&m_FBO, &m_colorAttachment, getTextureID(), size, false);
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
        float x,y,z;
        float radius = 0.0f;
        while (radius < 100.0f) {
            x = urand(-300.0f, 300.0f);
            y = urand(-300.0f, 300.0f);
            z = urand(-300.0f, 300.0f);
            radius = sqrt(pow(x,2.0f) + pow(y,2.0f) + pow(z,2.0f));
        }
        m_particleData[i].life = urand(0.0f, 150.0f);//100.0f;
        m_particleData[i].dir = glm::vec3(0.0f,0.0f,0.0f);
        m_particleData[i].pos = glm::vec3(x,y,z);
        m_particleData[i].color = glm::vec3(0.9f, 0.7f, 0.8f);
        m_particleData[i].decay = -1.0f;

        // Shooting star
        if (urand(0.0f,1.0f) > 0.97f) {
            m_particleData[i].color = glm::vec3(0.8f, 0.5f, 0.4f);
            m_particleData[i].dir = glm::vec3(urand(-2.0f, 2.0f),urand(-2.0f, 2.0f),urand(-2.0f, 2.0f));
        }

        // Twinkling (fading out/in)
        else if (urand(0.0f,1.0f) > 0.5f)
            m_particleData[i].decay = 1.0f;
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
    Camera camera = m_renderer->getCamera();
    Transforms origTrans = m_renderer->getTransformation();
    float globalSpeed = m_renderer->getSimulationSpeed();
    float speed = m_renderer->getRotationalSpeed();
    bool isPaused = m_renderer->getPaused();
    glm::mat4x4 atmosphericRotation = glm::rotate(speed/25.0f, glm::vec3(7,1,8));

    for(int i =0; i<NUMPARTICLES; i++) {

        Transforms trans = origTrans;

        float x1 = m_particleData[i].pos.x;
        float y1 = m_particleData[i].pos.y;
        float z1 = m_particleData[i].pos.z;

        glm::vec3 n = glm::vec3(0.0f,0.0f,1.0f);
        glm::vec3 np = glm::normalize(glm::vec3(-x1,-y1,-z1));

        glm::vec3 view = glm::normalize(camera.eye);
        if (glm::dot(view, glm::vec3(atmosphericRotation * glm::vec4(np, 1.f))) > 0) { // Backface Culling!!!!!!

            glm::vec3 axis = glm::cross(n, np);
            float angle = glm::acos(glm::dot(n, np) / (glm::length(glm::vec4(n,0.0f)) * glm::length(glm::vec4(np,0.0f))));

            trans.model =
                atmosphericRotation *
                glm::translate(glm::vec3(x1,y1,z1)) *
                glm::rotate(angle*360.0f/6.28f, axis) *
                trans.model;

            float alph = m_particleData[i].life / 150.0f;

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
            if (glm::length(glm::vec4(m_particleData[i].dir,0.0f)) > 0) {
                for (int dt = 0; dt <= 8; dt++) {
                    glm::vec3 newPos = glm::vec3(m_particleData[i].pos - coeff*dt*m_particleData[i].dir);
                    Transforms temp = origTrans;
                    temp.model =
                            atmosphericRotation *
                            glm::translate(newPos) *
                            glm::rotate(angle*360.0f/6.28f, axis) *
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

        if (m_particleData[i].life <= 0 || m_particleData[i].life >= 150) {
            m_particleData[i].decay *= -1.0f;
            if (glm::length(glm::vec4(m_particleData[i].dir,0.0f)) > 0 && m_particleData[i].life <= 0) {
                m_particleData[i].pos.x = urand(-200.0f, 200.0f);
                m_particleData[i].pos.y = urand(-200.0f, 200.0f);
                m_particleData[i].pos.z = urand(-200.0f, 200.0f);
            }
        }
    }
}

