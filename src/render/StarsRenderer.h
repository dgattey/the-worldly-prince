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
    void setupStar(int i);

    // Objects
    Particle m_particle;
    ParticleData *m_particleData;
};

#endif // STARSRENDERER_H
