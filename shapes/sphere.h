#ifndef SPHERE_H
#define SPHERE_H

#include "shape.h"
#include "GL/glu.h"
#include "glm.hpp"

class Sphere : public Shape
{
public:
    // uninitialized sphere
    Sphere();

    // initialized sphere
    Sphere(GLuint position, GLuint normal);

    ~Sphere();

    void init(GLuint position, GLuint normal);

    void render();

    bool needsUpdate();

private:

    GLfloat *m_vertexBufferData;
    GLuint m_vaoID;
    int m_p1 = 8;
    int m_p2 = 8;
    bool m_isInitialized;
    void addVertexNormal(glm::vec3 vertex, glm::vec3 normal, int *startIndex);
    glm::vec3 cartesianFromDegs(double theta, double phi, double radians);

};

#endif // SPHERE_H
