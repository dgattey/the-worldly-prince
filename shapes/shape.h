#ifndef SHAPE_H
#define SHAPE_H

#include "GL/glew.h"
#include "glm/glm.hpp"

class Shape
{
public:
    Shape();
    Shape(GLuint position, GLuint normal);
    virtual ~Shape();

    virtual void init();

    virtual void render();

protected:
    GLuint m_vaoID;
    GLuint m_vertexLocation;
    GLuint m_normalLocation;
};

#endif // SHAPE_H
