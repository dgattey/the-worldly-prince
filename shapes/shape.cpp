#include "shape.h"
#include <iostream>

Shape::Shape(){}

Shape::Shape(GLuint position, GLuint normal)
{
    m_vertexLocation = position;
    m_normalLocation = normal;
    init();
}

Shape::~Shape()
{ }

/*
 * This shape subclass models a triangle. It was used as a default for testing
 */
void Shape::init()
{
    GLfloat vertexBufferData[] = {
        -1, -1, 0, // Position 1
        0,  0, 1, // Normal 1
        1, -1, 0, // Position 2
        0,  0, 1, // Normal 2
        -1, 1, 0,  // Position 3
        0, 0, 1,  // Normal 3
   };

    // Step 2: initialize and bind a Vertex Array Object -- see glGenVertexArrays and glBindVertexArray
    glGenVertexArrays(1, &m_vaoID);	//	Create	1	VAO
    glBindVertexArray(m_vaoID);

    // Step 3: initialize and bind a buffer for your vertex data -- see glGenBuffers and glBindBuffer
    GLuint vboID;
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);

    // Step 4: Send your vertex data to the GPU -- see glBufferData
    glBufferData(GL_ARRAY_BUFFER, 3 * 6 * sizeof(GLfloat), vertexBufferData, GL_STATIC_DRAW);

    // Step 5: Expose the vertices to other OpenGL components (namely, shaders)
    //         -- see glEnableVertexAttribArray and glVertexAttribPointer
    glEnableVertexAttribArray(m_vertexLocation);
    glEnableVertexAttribArray(m_normalLocation);
    glVertexAttribPointer(
        m_vertexLocation,
        3,                   // Num coordinates per position
        GL_FLOAT,            // Type
        GL_FALSE,            // Normalized
        sizeof(GLfloat) * 6, // Stride
        (void*) 0            // Array buffer offset
    );
    glVertexAttribPointer(
        m_normalLocation,
        3,           // Num coordinates per normal
        GL_FLOAT,    // Type
        GL_TRUE,     // Normalized
        sizeof(GLfloat) * 6,           // Stride
        (void*) (sizeof(GLfloat) * 3)    // Array buffer offset
   );
    // Step 6: Clean up -- unbind the buffer and vertex array.
    //         It is a good habit to leave the state of OpenGL the way you found it
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Shape::render()
{
    glBindVertexArray(m_vaoID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}
