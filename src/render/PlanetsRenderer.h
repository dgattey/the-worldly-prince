#ifndef PLANET_H
#define PLANET_H

#include "GLCommon.h"
#include "Renderer.h"
#include "PlanetDataParser.h"

class Transforms;
class Sphere;
class GLRenderWidget;

/**
 * @brief Class to support rendering of arbitrary numbers of
 * planets, using the Perlin noise shader to modulate and
 * color them. Relies on Sphere class to actually render
 */
class PlanetsRenderer : public Renderer {
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
    void parseData();
    void createSpheres();
    void deleteSpheres();
    glm::mat4x4 applyPlanetTrans(float speed, PlanetData trans);

    // For shaders
    float m_seed;

    // File used for xml data
    std::string m_file;

    // Objects
    QList<int> m_resolutions; // All possible resolutions
    QHash<QString,PlanetData> m_planetData; // Name to planet
    QHash<int, Sphere*> m_planets; // Spheres corresponding to resolutions

};

#endif // PLANET_H
