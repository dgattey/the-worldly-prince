#ifndef FLOWERRENDERER_H
#define FLOWERRENDERER_H

#include "CS123Common.h"

class Transforms;
class Sphere;

/**
 * @brief Class to support rendering of arbitrary numbers of
 * planets, using the Perlin noise shader to modulate and
 * color them. Relies on Sphere class to actually render
 */
class FlowersRenderer {
public:
    FlowersRenderer();
    ~FlowersRenderer();

    void createShaderProgram();
    void createFBO(glm::vec2 size);
    void render(Transforms trans, glm::mat4x4 localizedOrbit, float rSpeed);
    void refresh();

    int getTextureID();
    GLuint *getColorAttach();
    GLuint *getFBO();

private:
    void drawPlanets(Transforms trans, glm::mat4x4 localizedOrbit, float rSpeed);
    void randomizeSeed();

    // GL needs
    GLuint m_FBO;
    GLuint m_shader;
    GLuint m_colorAttachment;

    // For shaders
    float m_seed;

    // Objects
    QList<Sphere*> m_planets;

};

#endif // FLOWERRENDERER_H
