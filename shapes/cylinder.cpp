#include "cylinder.h"
#include <iostream>

Cylinder::Cylinder()
{
    m_isInitialized = false;
}

Cylinder::Cylinder(const GLuint position, const GLuint normal)
{
    init(position, normal);
}

Cylinder::~Cylinder()
{
    free(m_vertexBufferData);
}

void Cylinder::init(const GLuint vertexLocation, const GLuint normalLocation)
{
    m_isInitialized = true;
    // The current index into the buffer data array
    int i = 0;

    // The amount each slice rotates by
    double step = (2.0 * M_PI) / m_p2;

    // The size of the vertex buffer data array
    int size = 6 * 3 * m_p2 * (6 * m_p1 - 2);
    m_vertexBufferData = (GLfloat *)malloc(sizeof(GLfloat) * size);

    // For each of the m_p2 slices
    for (int slice = 0; slice < m_p2; slice++) {
        float oldX = 0.5 * cos((slice - 1) * step);
        float oldZ = 0.5 * sin((slice - 1) * step);
        float newX = 0.5 * cos(slice * step);
        float newZ = 0.5 * sin(slice * step);

        // First we do the side
        // We can use the same len since they're both a radius away from the center.
        glm::vec3 oldNormal = glm::vec3(oldX, 0, oldZ);
        glm::vec3 newNormal = glm::vec3(newX, 0, newZ);
        for (int row = 0; row < m_p1; row++) {
            // Lower left
            glm::vec3 point1 = glm::vec3(
                    newX,
                    -0.5 + row * (1.f / m_p1),
                    newZ);

            // Upper right
            glm::vec3 point2 = glm::vec3(
                    oldX,
                    -0.5 + (row + 1) * (1.f / m_p1),
                    oldZ);

            // Upper left
            glm::vec3 point3 = glm::vec3(
                    newX,
                    -0.5 + (row + 1) * (1.f / m_p1),
                    newZ);

            // Lower right
            glm::vec3 point5 = glm::vec3(
                    oldX,
                    -0.5 + row * (1.f / m_p1),
                    oldZ);

            addVertexNormal(point1, newNormal, &i);
            addVertexNormal(point2, oldNormal, &i);
            addVertexNormal(point3, newNormal, &i);
            addVertexNormal(point1, newNormal, &i);
            addVertexNormal(point5, oldNormal, &i);
            addVertexNormal(point2, oldNormal, &i);
        }

        // Now we'll do the top
        glm::vec3 normal = glm::vec3(0, 1, 0);
        for (int row = 0; row < m_p1; row++) {
            // Lower left
            glm::vec3 point1 = glm::vec3(
                    newX * (1 - (float)row / m_p1),
                    0.5,
                    newZ * (1 - (float)row / m_p1));

            // Upper right
            glm::vec3 point2 = glm::vec3(
                    oldX * (1 - (float)(row + 1) / m_p1),
                    0.5,
                    oldZ * (1 - (float)(row + 1) / m_p1));

            // Upper left
            glm::vec3 point3 = glm::vec3(
                    newX * (1 - (float)(row + 1) / m_p1),
                    0.5,
                    newZ * (1 - (float)(row + 1) / m_p1));

            // Lower right
            glm::vec3 point5 = glm::vec3(
                    oldX * (1 - (float)row / m_p1),
                    0.5,
                    oldZ * (1 - (float)row / m_p1));

            // And now the tip (special case the inner triangle)
            if (row == (m_p1 - 1)) {
                addVertexNormal(point1, normal, &i);
                addVertexNormal(point5, normal, &i);
                addVertexNormal(point2, normal, &i);
                break;
            }

            // point4 = point1, point6 = point2
            addVertexNormal(point1, normal, &i);
            addVertexNormal(point2, normal, &i);
            addVertexNormal(point3, normal, &i);
            addVertexNormal(point1, normal, &i);
            addVertexNormal(point5, normal, &i);
            addVertexNormal(point2, normal, &i);
        }

        // Now we'll do the bottom
        normal = glm::vec3(0, -1, 0);
        for (int row = 0; row < m_p1; row++) {
            // Lower left
            glm::vec3 point1 = glm::vec3(
                    oldX * (1 - (float)row / m_p1),
                    -0.5,
                    oldZ * (1 - (float)row / m_p1));

            // Upper right
            glm::vec3 point2 = glm::vec3(
                    newX * (1 - (float)(row + 1) / m_p1),
                    -0.5,
                    newZ * (1 - (float)(row + 1) / m_p1));

            // Upper left
            glm::vec3 point3 = glm::vec3(
                    oldX * (1 - (float)(row + 1) / m_p1),
                    -0.5,
                    oldZ * (1 - (float)(row + 1) / m_p1));

            // Lower right
            glm::vec3 point5 = glm::vec3(
                    newX * (1 - (float)row / m_p1),
                    -0.5,
                    newZ * (1 - (float)row / m_p1));

            // And now the tip (special case the inner triangle)
            if (row == (m_p1 - 1)) {
                addVertexNormal(point1, normal, &i);
                addVertexNormal(point5, normal, &i);
                addVertexNormal(point2, normal, &i);
                break;
            }

            addVertexNormal(point1, normal, &i);
            addVertexNormal(point2, normal, &i);
            addVertexNormal(point3, normal, &i);
            addVertexNormal(point1, normal, &i);
            addVertexNormal(point5, normal, &i);
            addVertexNormal(point2, normal, &i);
        }

    }

    // Step 2: initialize and bind a Vertex Array Object -- see glGenVertexArrays and glBindVertexArray
    glGenVertexArrays(1, &m_vaoID);	//	Create	1	VAO
    glBindVertexArray(m_vaoID);

    // Step 3: initialize and bind a buffer for your vertex data -- see glGenBuffers and glBindBuffer
    GLuint vboID;
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);

    // Step 4: Send your vertex data to the GPU -- see glBufferData
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(GLfloat), m_vertexBufferData, GL_STATIC_DRAW);

    // Step 5: Expose the vertices to other OpenGL components (namely, shaders)
    //         -- see glEnableVertexAttribArray and glVertexAttribPointer
    glEnableVertexAttribArray(vertexLocation);
    glEnableVertexAttribArray(normalLocation);
    glVertexAttribPointer(
        vertexLocation,
        3,                   // Num coordinates per position
        GL_FLOAT,            // Type
        GL_FALSE,            // Normalized
        sizeof(GLfloat) * 6, // Stride
        (void*) 0            // Array buffer offset
    );
    glVertexAttribPointer(
        normalLocation,
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

void Cylinder::render()
{
    if (m_isInitialized) {
        glBindVertexArray(m_vaoID);
        int size = 6 * 3 * m_p2 * (6 * m_p1 - 2);
        glDrawArrays(GL_TRIANGLES, 0, size);
        glBindVertexArray(0);
    } else {
        printf("Must initialized cylinder before rendering!\n");
    }
}

void Cylinder::addVertexNormal(glm::vec3 vertex, glm::vec3 normal, int *startIndex)
{
    m_vertexBufferData[(*startIndex)++] = vertex.x;  //X
    m_vertexBufferData[(*startIndex)++] = vertex.y;  //Y
    m_vertexBufferData[(*startIndex)++] = vertex.z;  //Z

    // Make sure this is a normal vector
    normal = glm::normalize(normal);

    m_vertexBufferData[(*startIndex)++] = normal.x;  //X Normal
    m_vertexBufferData[(*startIndex)++] = normal.y;  //Y Normal
    m_vertexBufferData[(*startIndex)++] = normal.z;  //Z Normal
}
