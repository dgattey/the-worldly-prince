#ifndef PLANET_H
#define PLANET_H

#include "CS123Common.h"
#include "Renderer.h"

class Transforms;
class Sphere;
class GLRenderer;

/**
 * @brief Class to support rendering of arbitrary numbers of
 * planets, using the Perlin noise shader to modulate and
 * color them. Relies on Sphere class to actually render
 */
class PlanetsRenderer : public Renderer {

    // Represents the color applied to a planet along with its index in a shape list
    struct PlanetColor {
        PlanetColor() {}
        PlanetColor(glm::vec4 l, glm::vec4 h, float t, int i) :
            low(l), high(h), threshold(t), shapeIndex(i) {}

        glm::vec4 low;
        glm::vec4 high;
        float threshold;
        int shapeIndex;
    };

    // Represents the constant transformation for a planet
    struct PlanetData {
        PlanetData(float s, glm::vec3 t, float d, float y, glm::vec3 p, PlanetColor col) :
            size(s), tilt(t), day(d), year(y), place(p), c(col) {}

        float size;
        glm::vec3 tilt;
        float day;
        float year;
        glm::vec3 place;
        PlanetColor c;
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
    glm::mat4x4 applyPlanetTrans(float speed, PlanetData trans);

    // For shaders
    float m_seed;

    // Objects
    QList<Sphere*> m_planets;
    QList<PlanetData> m_planetData;

};

#endif // PLANET_H
