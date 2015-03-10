#include "TexturedQuad.h"

/**
 * @brief Sets up initialized to false
 */
TexturedQuad::TexturedQuad() {
    m_isInitialized = false;
}

/**
 * @brief Generates a full screen quad and binds all data for later
 * @param vertexLocation The shader's GLuint for vertex location
 * @param texLocation The shader's GLuint for texture location
 */
void TexturedQuad::init(const GLuint vertexLoc, const GLuint textureLoc) {
    m_isInitialized = true;

    // Generate a full screen quad
    GLfloat rad = 1.0f;    // -1 to +1 in clip space
    const GLuint dataSize = ( 3 + 2 ) * 4;
    GLfloat data[dataSize] = { -rad, -rad, 0,    0, 0,        // BL
                                rad, -rad, 0,    1, 0,        // BR
                               -rad,  rad, 0,    0, 1,        // TL
                                rad,  rad, 0,    1, 1};       // TR

    size_t stride = sizeof( GLfloat ) * 3 + sizeof( GLfloat ) * 2;
    glGenVertexArrays( 1, &m_vaoID );
    glBindVertexArray( m_vaoID );

    // VBO
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * dataSize, &data[0], GL_STATIC_DRAW);

    // Attributes
    glEnableVertexAttribArray(vertexLoc);
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(textureLoc);
    glVertexAttribPointer(textureLoc, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3*sizeof(GLfloat)));

    // Clean up
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**
 * @brief If initialized, draw the two triangles
 */
void TexturedQuad::draw(){
    if (!m_isInitialized){
        fprintf(stderr, "You must call init() before you can draw a TexturedQuad!");
        return;
    }

    // Rebind vertex array and draw the triangles
    glBindVertexArray(m_vaoID);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

