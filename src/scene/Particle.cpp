#include "Particle.h"
#include <iostream>

#define NUM_TRIS 2
#define SIZE 1.0f

/**
 * @brief Sets up particle as uninitialized
 */
Particle::Particle() {
    m_isInitialized = false;
}

/**
 * @brief Initializes particle object - assumes shader already setup
 * @param vertexLocation The vertex GLuint
 * @param textureLocation The texture GLuint
 */
void Particle::init(const GLuint vertexLocation, const GLuint textureLocation){
    m_isInitialized = true;

    // All corners
    GLfloat vertexBufferData[] = {
        -SIZE, SIZE, 0, 0, 0,
         SIZE, -SIZE, 0, 1, 1,
         SIZE, SIZE, 0, 1, 0,
        -SIZE, SIZE, 0, 0, 0,
        -SIZE, -SIZE, 0, 0, 1,
         SIZE, -SIZE, 0, 1, 1
    };

    // VAO and vertex buffer init
    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
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

/**
 * @brief Draws a particle by binding to the VAO and drawing 2 triangles
 * Fails if init hasn't been called
 */
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
