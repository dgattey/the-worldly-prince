#ifndef FLOWERSRENDERER_H
#define FLOWERSRENDERER_H

#include "CS123Common.h"
#include "Renderer.h"

class GLRenderer;
class PlanetsRenderer;
class Flower;
class Sphere;
class Cylinder;

/**
 * @brief Class to support rendering of arbitrary numbers of
 * flowers, using the flowers shader to actually draw them
 */
class FlowersRenderer : public Renderer {
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

    PlanetsRenderer *m_planets;

    // Objects
    QList<Flower *> m_flowers;
    Sphere *m_flowerSphere;
    Cylinder *m_flowerCylinder;
};

#endif // FLOWERSRENDERER_H
