#ifndef SPHERE_H
#define SPHERE_H

#include "GL/glew.h"
#include "scenegraph/OpenGLScene.h"
#include "Shape.h"

class Sphere : public Shape
{
public:
    //Uninitialized Sphere
    Sphere();

    //Initialized Sphere
    Sphere(const int paramOne, const int paramTwo, const int paramThree, const GLuint vertexLocation, const GLuint normalLocation, NormalRenderer *normRender);

    virtual ~Sphere();

    // Initialize Sphere: tessellate triangles to approximate a sphere
    virtual void init(const GLuint vertexLocation, const GLuint normalLocation);

    static float intersect(glm::vec3 eye, glm::vec3 dir);
    static glm::vec3 normal(glm::vec3 eye, glm::vec3 dir, float t);
    static glm::vec2 textureCoordinates(glm::vec3 eye, glm::vec3 dir, float t);

private:
    // Tessellation parameters (2 and 1 respectively)
    int m_numSlices;
    int m_numCuts;

};

#endif // SPHERE_H
