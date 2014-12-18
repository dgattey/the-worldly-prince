#include "TexturedQuad.h"

TexturedQuad::TexturedQuad() {
    m_isInitialized = false;
}

TexturedQuad::TexturedQuad(const GLuint vertexLocation, const GLuint normalLocation) {
    init(vertexLocation, normalLocation);
}

void TexturedQuad::init(const GLuint vertexLocation, const GLuint texLocation) {
    m_isInitialized = true;

    m_posLocation = vertexLocation;
    m_texLocation = texLocation;

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

    GLuint vertexBuffer;
    glGenBuffers( 1, &vertexBuffer );
    glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(GLfloat) * dataSize, &data[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray( m_posLocation );
    glVertexAttribPointer( m_posLocation, 3, GL_FLOAT, GL_FALSE, stride, 0); //Position

    glEnableVertexAttribArray( m_texLocation );
    glVertexAttribPointer( m_texLocation, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3*sizeof(GLfloat)) ); //Texture coordinate

    // Clean up
    glBindVertexArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

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

