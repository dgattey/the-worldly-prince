#ifndef PLANET_H
#define PLANET_H

#include "CS123Common.h"

class Transforms;
class Sphere;
class GLRenderer;

/**
 * @brief Class to support rendering of arbitrary numbers of
 * planets, using the Perlin noise shader to modulate and
 * color them. Relies on Sphere class to actually render
 */
class PlanetsRenderer {
public:
    PlanetsRenderer(GLRenderer *renderer);
    ~PlanetsRenderer();

    void createShaderProgram();
    void createFBO(glm::vec2 size);
    void render(glm::mat4x4 orbit);
    void refresh();

    int getTextureID();
    GLuint *getColorAttach();
    GLuint *getFBO();

private:
    void drawPlanets(glm::mat4x4 orbit);
    void randomizeSeed();

    GLRenderer *m_renderer;

    // GL needs
    GLuint m_FBO;
    GLuint m_shader;
    GLuint m_colorAttachment;

    // For shaders
    float m_seed;

    // Objects
    QList<Sphere*> m_planets;

};

#endif // PLANET_H
