#ifndef FLOWERSRENDERER_H
#define FLOWERSRENDERER_H

#include "CS123Common.h"

class GLRenderer;
class PlanetsRenderer;
class Flower;
class Sphere;
class Cylinder;

/**
 * @brief Class to support rendering of arbitrary numbers of
 * flowers, using the flowers shader to actually draw them
 */
class FlowersRenderer {
public:
    FlowersRenderer(PlanetsRenderer *planets, GLRenderer *renderer);
    ~FlowersRenderer();

    void createShaderProgram();
    void createFBO(glm::vec2 size);
    void render();
    void refresh();

    int getTextureID();
    GLuint *getColorAttach();
    GLuint *getFBO();

private:
    void drawFlowers();

    GLRenderer *m_renderer;
    PlanetsRenderer *m_planets;

    // GL needs
    GLuint m_FBO;
    GLuint m_shader;
    GLuint m_colorAttachment;

    // Objects
    QList<Flower *> m_flowers;
    Sphere *m_flowerSphere;
    Cylinder *m_flowerCylinder;
};

#endif // FLOWERSRENDERER_H
