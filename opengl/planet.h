#ifndef PLANET_H
#define PLANET_H

#include "CS123Common.h"
#include "sphere.h"

class Transforms;

class Planet {
public:
    Planet();
    ~Planet();

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

#endif // PLANET_H
