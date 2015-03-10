#ifndef TEXQUAD_H
#define TEXQUAD_H

#include "GLCommon.h"

/**
 * @brief Class to support rendering of a quad textured by some buffer
 */
class TexturedQuad {
public:
    TexturedQuad();
    void init(const GLuint vertexLocation, const GLuint normalLocation);
    void draw();

private:
    bool m_isInitialized;
    GLuint m_vaoID;
};

#endif // TEXQUAD_H
