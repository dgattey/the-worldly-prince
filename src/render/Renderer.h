#ifndef RENDERER
#define RENDERER

#include "GLCommon.h"

class GLRenderWidget;

/**
 * @brief The Renderer interface
 * Renderer interface to support everything a Renderer should have
 * the ability to do. Includes variables like the FBO, shader, and
 * color attachment. Also includes a refresh data method.
 */
class Renderer {
public:
    virtual void createShaderProgram() = 0;
    virtual void createFBO(glm::vec2 size) = 0;
    virtual void render() = 0;
    virtual void refresh() = 0;

    virtual int getTextureID() = 0;
    virtual GLuint *getColorAttach() = 0;
    virtual GLuint *getFBO() = 0;

protected:
    GLRenderWidget *m_renderer;

    // GL needs
    GLuint m_FBO;
    GLuint m_shader;
    GLuint m_colorAttachment;
    int m_textureID;
};

#endif // RENDERER

