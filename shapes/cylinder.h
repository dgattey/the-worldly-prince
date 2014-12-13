#ifndef CYLINDER_H
#define CYLINDER_H

#include "GL/glew.h"
#include "shape.h"

class Cylinder : public Shape
{
public:
    // uninitialized cylinder
    Cylinder();

    ~Cylinder();

    void init(const GLuint position, const GLuint normal);

    void render();

    bool needsUpdate();

private:

    GLfloat *m_vertexBufferData;
    GLuint m_vaoID;
    bool m_isInitialized;
    int m_p1 = 10;
    int m_p2 = 10;

    void addVertexNormal(glm::vec3 vertex, glm::vec3 normal, int
            *startIndex);
};

#endif // CYLINDER_H
