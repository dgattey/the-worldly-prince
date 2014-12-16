#ifndef STARSRENDERER_H
#define STARSRENDERER_H

#include "CS123Common.h"

#include "particle.h"

#define NUMPARTICLES 3000

class Transforms;
class TexQuad;
class Particle;
class ParticleData;
class GLRenderer;

/**
 * @brief Class to support rendering of arbitrary numbers of
 * planets, using the Perlin noise shader to modulate and
 * color them. Relies on Sphere class to actually render
 */
class StarsRenderer { 
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

    GLRenderer *m_renderer;

    // GL needs
    GLuint m_FBO;
    GLuint m_shader;
    GLuint m_colorAttachment;

    // Objects
    Particle m_particle;
    ParticleData *m_particleData;
};

#endif // STARSRENDERER_H
