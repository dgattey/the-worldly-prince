#include "particle.h"
#include <iostream>

#define NUM_TRIS 2

Particle::Particle() {
    m_isInitialized = false;
}

Particle::Particle(const GLuint vertexLocation, const GLuint textureLocation){
    init(vertexLocation, textureLocation);
}

void Particle::init(const GLuint vertexLocation, const GLuint textureLocation){
    m_isInitialized = true;

    float v = 1.0f;
    GLfloat vertexBufferData[] = {
        -v, v, 0,
         0, 0,
         v, -v, 0,
         1, 1,
         v, v, 0,
         1, 0,
        -v, v, 0,
         0, 0,
        -v, -v, 0,
         0, 1,
         v, -v, 0,
         1, 1
    };

    // VAO init
    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);

    // Vertex buffer init
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*30, vertexBufferData, GL_STATIC_DRAW);

    // Expose vertices to shader
    glEnableVertexAttribArray(vertexLocation);
    glVertexAttribPointer(
       vertexLocation,
       3,                    // num vertices per element (3 for triangle)
       GL_FLOAT,             // type
       GL_FALSE,             // normalized?
       5*sizeof(GLfloat),  // stride
       (void*)0              // array buffer offset
    );

    glEnableVertexAttribArray(textureLocation);
    glVertexAttribPointer(
       textureLocation,
       2,                           // num vertices per element (3 for triangle)
       GL_FLOAT,                    // type
       GL_TRUE,                     // normalized?
       5*sizeof(GLfloat),         // stride
       (void*) (3*sizeof(GLfloat))    // array buffer offset
    );

    // Clean up
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
}

void Particle::draw(){
    if (!m_isInitialized){
        std::cout << "You must call init() before you can draw!" << std::endl;
        return;
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);

    glBindVertexArray(m_vaoID);
    glDrawArrays(GL_TRIANGLES, 0, NUM_TRIS*3);
    glBindVertexArray(0);

    glAccum(GL_MULT,0.5);
    glAccum(GL_ACCUM,1);
    glAccum(GL_RETURN,1);
}
