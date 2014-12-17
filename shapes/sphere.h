#ifndef SPHERE_H
#define SPHERE_H

#include "CS123Common.h"
#include "shape.h"

class Sphere : public Shape
{
public:
    // uninitialized sphere
    Sphere();

    ~Sphere();

    // Creates initialized sphere and returns it
    static Sphere *generate(int p, GLuint position, GLuint normal);

    void init(int p1, int p2, GLuint position, GLuint normal);
    void init(int p, GLuint position, GLuint normal);

    void render();

    bool needsUpdate();

private:

    GLfloat *m_vertexBufferData;
    GLuint m_vaoID;
    int m_p1;
    int m_p2;
    bool m_isInitialized;
    void addVertexNormal(glm::vec3 vertex, glm::vec3 normal, int *startIndex);
    glm::vec3 cartesianFromDegs(double theta, double phi, double radians);

};

#endif // SPHERE_H
