#ifndef STARSRENDERER_H
#define STARSRENDERER_H

#include "GLCommon.h"
#include "Renderer.h"
#include "Particle.h" // Must be included here

class ParticleData;
class GLRenderWidget;

/**
 * @brief Class to support rendering of arbitrary numbers of
 * stars, using particles to actually draw them
 */
class StarsRenderer : public Renderer {
public:
    StarsRenderer(GLRenderWidget *renderer);
    ~StarsRenderer();

    void createShaderProgram();
    void createFBO(glm::vec2 size);
    void render();
    void refresh();

    int getTextureID();
    GLuint *getColorAttach();
    GLuint *getFBO();

private:
    void drawStars();
    void drawBody(int i, float angle, glm::vec3 axis);
    void drawTail(int i, float angle, glm::vec3 axis);
    void calculateData(int i);
    void setupStar(int i);
    bool isShootingStar(int i);
    glm::mat4x4 getAtmosphericRotation();

    // Objects
    Particle m_particle;
    ParticleData *m_starData;
};

#endif // STARSRENDERER_H
