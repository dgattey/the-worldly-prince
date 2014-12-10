#ifndef TORUS_H
#define TORUS_H

#include "GL/glew.h"
#include "scenegraph/OpenGLScene.h"
#include "Shape.h"

class Torus : public Shape
{
public:
    //Uninitialized Torus
    Torus();

    //Initialized Torus
    Torus(const int paramOne, const int paramTwo, const int paramThree, const GLuint vertexLocation, const GLuint normalLocation, NormalRenderer *normRender);

    virtual ~Torus();

    // Initialize Torus: tessellate triangles to approximate a Torus
    virtual void init(const GLuint vertexLocation, const GLuint normalLocation);

private:
    // Tessellation parameters (2 and 1 and 3 respectively)
    int m_numSlices;
    int m_numCuts;
    int m_innerRadius;

};

#endif // TORUS_H
