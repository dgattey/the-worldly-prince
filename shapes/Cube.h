#ifndef CUBE_H
#define CUBE_H

#include "GL/glew.h"
#include "scenegraph/OpenGLScene.h"
#include "Shape.h"

class Cube : public Shape
{
public:
    //Uninitialized Sphere
    Cube();

    //Initialized Sphere
    Cube(int paramOne, int paramTwo, const int paramThree, const GLuint vertexLocation, const GLuint normalLocation, NormalRenderer *normRender);

    virtual ~Cube();

    // Initialize Sphere: generate VAO, vertex data, and buffer it on GPU
    virtual void init(const GLuint vertexLocation, const GLuint normalLocation);

    static float intersect(glm::vec3 eye, glm::vec3 dir);
    static glm::vec3 normal(glm::vec3 eye, glm::vec3 dir, float t);
    static glm::vec2 textureCoordinates(glm::vec3 eye, glm::vec3 dir, float t);

private:
    // Tessellation parameter that keeps track of squares along one side of one face
    int m_numSquares;

};

#endif // CUBE_H
