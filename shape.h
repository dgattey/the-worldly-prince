#ifndef SHAPE_H
#define SHAPE_H

#include "GL/glew.h"
#include <glm/glm.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

//#include "scenegraph/OpenGLScene.h"

class Shape
{
public:
    //Uninitialized Shape
    Shape();

    //Initialized Shape
    Shape(int type, int paramOne, int paramTwo, int paramThree, const GLuint vertexLocation, const GLuint normalLocation);

    virtual ~Shape();

    // Every shape must be able to tessellate itself (upon construction)
    virtual void init(const GLuint vertexLocation, const GLuint normalLocation)=0;

    // Graphically draws the shape using OpenGL
    virtual void draw();

    // Determines whether input parameters match those of the shape
    bool paramsEqual(int type, int p1, int p2, int p3);

private:
    // Distinct parameters for the shape's tessellation
    int m_paramOne;
    int m_paramTwo;
    int m_paramThree;
    int m_type;

protected:
    // OpenGL calls to buffer vertex data
    void bindData(const GLuint vertexLocation, const GLuint normalLocation);

    // Tessellates a basic quadrilateral (a common method used by all shapes)
    void quadTessellate(glm::vec3 v1, glm::vec3 n1, glm::vec3 v2, glm::vec3 n2, glm::vec3 v3, glm::vec3 n3, glm::vec3 v4, glm::vec3 n4, int idx);

    // Attributes:
    bool m_isInitialized;
    GLuint m_vaoID;
    GLuint m_vbo;
//    NormalRenderer *m_normRender;
    int m_numVertices;
    glm::vec3 *m_vertexBufferData;

};

#endif // SHAPE_H
