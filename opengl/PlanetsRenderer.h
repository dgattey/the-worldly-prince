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
    // Represents the constant transformation for a planet
    struct PlanetTransformation {
        PlanetTransformation(float s, glm::vec3 t, float d, float y, glm::vec3 p) :
            size(s), tilt(t), day(d), year(y), place(p) {}

        float size;
        glm::vec3 tilt;
        float day;
        float year;
        glm::vec3 place;
    };

public:
    PlanetsRenderer(GLRenderer *renderer);
    ~PlanetsRenderer();

    void createShaderProgram();
    void createFBO(glm::vec2 size);
    void render();
    void refresh();

    int getTextureID();
    GLuint *getColorAttach();
    GLuint *getFBO();
    glm::mat4x4 getMoonTransformation(float speed);

private:
    void drawPlanets();
    void randomizeSeed();
    glm::mat4x4 applyPlanetTrans(float speed, PlanetTransformation trans);

    GLRenderer *m_renderer;

    // GL needs
    GLuint m_FBO;
    GLuint m_shader;
    GLuint m_colorAttachment;

    // For shaders
    float m_seed;

    // Objects
    QList<Sphere*> m_planets;
    QList<PlanetTransformation> m_planetTransformations;

};

#endif // PLANET_H
