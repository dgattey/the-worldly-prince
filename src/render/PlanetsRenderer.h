#ifndef PLANET_H
#define PLANET_H

#include "GLCommon.h"
#include "Renderer.h"

class Transforms;
class Sphere;
class GLRenderWidget;

/**
 * @brief Class to support rendering of arbitrary numbers of
 * planets, using the Perlin noise shader to modulate and
 * color them. Relies on Sphere class to actually render
 */
class PlanetsRenderer : public Renderer {

    // Represents the color applied to a planet along with its index in a shape list
    struct PlanetColor {
        PlanetColor() {}
        PlanetColor(glm::vec4 l, glm::vec4 h, float t) :
            low(l), high(h), threshold(t) {}

        glm::vec4 low;
        glm::vec4 high;
        float threshold;
    };

    // Represents the constant transformation for a planet
    struct PlanetData {
        PlanetData(float s, glm::vec3 t, float d, float y, glm::vec3 p, PlanetColor col, int r) :
            size(s), tilt(t), day(d), year(y), place(p), c(col), resolution(r) {}

        float size;
        glm::vec3 tilt;
        float day;
        float year;
        glm::vec3 place;
        PlanetColor c;
        int resolution;
    };

public:
    PlanetsRenderer(GLRenderWidget *renderer);
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
    QList<int> m_resolutions;
    QList<PlanetData> m_planetData;
    QHash<int, Sphere*> m_planets;

};

#endif // PLANET_H
