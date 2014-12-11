#include "sphere.h"
#include <iostream>

Sphere::Sphere()
{
    m_isInitialized = false;
}

Sphere::Sphere(const GLuint vertexLocation, const GLuint normalLocation)
{
    // The sphere can't have fewer than 3 sides
    m_p1 = 8;
    m_p2 = 8;
    init(vertexLocation, normalLocation);
}

Sphere::~Sphere()
{
    if (m_vertexBufferData != NULL) {
        free(m_vertexBufferData);
    }
}

void Sphere::init(const GLuint vertexLocation, const GLuint normalLocation)
{
    m_isInitialized = true;
    // The current index into the buffer data array
    int i = 0;

    // delta theta per m_p2 slice
    double theta_step = (2.0 * M_PI) / m_p2;

    // delta phi per m_p1 slice
    double phi_step = (M_PI) / m_p1;

    // The size of the vertex buffer data array
    int size = 2 * (m_p1) * m_p2 * 6 * 3;
    m_vertexBufferData = (GLfloat *)malloc(sizeof(GLfloat) * size);

    for (int slice = 0; slice < m_p2; slice++) {

        for (int row = 0; row < m_p1; row++) {
            // Lower left
            glm::vec3 point1 = cartesianFromDegs(theta_step * slice, phi_step * row, 0.5);

            // Upper right
            glm::vec3 point2 = cartesianFromDegs(theta_step * (slice + 1), phi_step * (row + 1), 0.5);

            // Upper left
            glm::vec3 point3 = cartesianFromDegs(theta_step * slice, phi_step * (row + 1), 0.5);

            // Lower right
            glm::vec3 point5 = cartesianFromDegs(theta_step * (slice + 1), phi_step * row, 0.5);

            // point4 = point1, point6 = point2
            addVertexNormal(point1, glm::normalize(point1), &i);
            addVertexNormal(point2, glm::normalize(point2), &i);
            addVertexNormal(point3, glm::normalize(point3), &i);
            addVertexNormal(point1, glm::normalize(point1), &i);
            addVertexNormal(point5, glm::normalize(point5), &i);
            addVertexNormal(point2, glm::normalize(point2), &i);
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

glm::vec3 Sphere::cartesianFromDegs(double theta, double phi, double radius)
{
    double x = radius * sin(phi) * cos(theta);
    double y = radius * cos(phi);
    double z = radius * sin(phi) * sin(theta);
    return glm::vec3(x, y, z);
}

void Sphere::render()
{
    if (m_isInitialized) {
        glBindVertexArray(m_vaoID);
        int size = 2 * (m_p1 - 2) * m_p2 * 6 * 3;
        glDrawArrays(GL_TRIANGLES, 0, size);
        glBindVertexArray(0);
    } else {
        printf("Must initialized sphere before drawing it!\n");
    }
}

void Sphere::addVertexNormal(glm::vec3 vertex, glm::vec3 normal, int *startIndex)
{
    m_vertexBufferData[(*startIndex)++] = vertex.x;  //X
    m_vertexBufferData[(*startIndex)++] = vertex.y;  //Y
    m_vertexBufferData[(*startIndex)++] = vertex.z;  //Z

    m_vertexBufferData[(*startIndex)++] = normal.x;  //X Normal
    m_vertexBufferData[(*startIndex)++] = normal.y;  //Y Normal
    m_vertexBufferData[(*startIndex)++] = normal.z;  //Z Normal
}

