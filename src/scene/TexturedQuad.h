#ifndef TEXQUAD_H
#define TEXQUAD_H

#include "GLCommon.h"

class TexturedQuad
{
public:
    //Uninitialized quad
    TexturedQuad();
    //Initialized quad
    TexturedQuad(const GLuint vertexLocation, const GLuint normalLocation);
    void init(const GLuint vertexLocation, const GLuint normalLocation);
    void draw();

private:
    bool m_isInitialized;
    GLuint m_vaoID;

    GLuint m_posLocation;
    GLuint m_texLocation;
};

#endif // TEXQUAD_H
