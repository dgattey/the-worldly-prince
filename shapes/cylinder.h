#ifndef CYLINDER_H
#define CYLINDER_H

#include "GL/glew.h"
//#include "scenegraph/OpenGLScene.h"
#include "shape.h"

class Cylinder : public Shape
{
public:
    //Uninitialized Cylinder
    Cylinder();

    //Initialized Cylinder
    Cylinder(const int paramOne, const int paramTwo, const int paramThree, const GLuint vertexLocation, const GLuint normalLocation);

    virtual ~Cylinder();

    // Initialize Cylinder: Tessellate triangles for caps and side
    virtual void init(const GLuint vertexLocation, const GLuint normalLocation);

    static float intersect(glm::vec3 eye, glm::vec3 dir);
    static glm::vec3 normal(glm::vec3 eye, glm::vec3 dir, float t);
    static glm::vec2 textureCoordinates(glm::vec3 eye, glm::vec3 dir, float t);

private:
    // Tessellation parameters
    int m_numSlices; // paramTwo
    int m_numCuts; // paramOne
};


#endif // CYLINDER_H
