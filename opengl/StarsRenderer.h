#ifndef STARSRENDERER_H
#define STARSRENDERER_H

#include "CS123Common.h"
#include "Renderer.h"
#include "particle.h" // Must be included here

class ParticleData;
class GLRenderer;

/**
 * @brief Class to support rendering of arbitrary numbers of
 * stars, using particles to actually draw them
 */
class StarsRenderer : public Renderer {
public:
    StarsRenderer(GLRenderer *renderer);
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
