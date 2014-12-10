#include "Cube.h"
//#include "Shape.h"
#include <iostream>
#include "lib/CS123Common.h"
#include "scenegraph/OpenGLScene.h"
#include "ui/Settings.h"

Cube::Cube()
{
    m_isInitialized = false;
}

Cube::Cube(int paramOne, int paramTwo, const int paramThree, const GLuint vertexLocation, const GLuint normalLocation, NormalRenderer *normRender) :
    Shape(SHAPE_CUBE, paramOne, paramTwo, paramThree, vertexLocation, normalLocation, normRender)
{
    m_numSquares = paramOne;
    m_numVertices = (m_numSquares*m_numSquares)*36; // The total number of vertices to be stored
    init(vertexLocation, normalLocation);
}

Cube::~Cube()
{
    glDeleteVertexArrays(1, &m_vaoID);
    glDeleteBuffers(1, &m_vbo);
}

// RAY INTERSECTION
// Calculate the t-parameter for the intersection between a ray and a shape
float Cube::intersect(glm::vec3 eye, glm::vec3 dir) {
    float minT = -1.0f;

    glm::vec3 normals[] = {glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                           glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f) };

    for (glm::vec3 & n : normals){
        float t = ((n.x*n.x*0.5f + n.y*n.y*0.5f + n.z*n.z*0.5f) - (n.x*eye.x + n.y*eye.y + n.z*eye.z)) /
                (n.x*dir.x + n.y*dir.y + n.z*dir.z);
        glm::vec3 p = eye + dir*t;
        if (n.y != 0.0f && p.x <= 0.5 && p.x >= -0.5 && p.z <= 0.5 && p.z >= -0.5 && (minT < 0 || t < minT))
            minT = t;
        else if (n.x != 0.0f && p.y <= 0.5 && p.y >= -0.5 && p.z <= 0.5 && p.z >= -0.5 && (minT < 0 || t < minT))
            minT = t;
        else if (n.z != 0.0f && p.x <= 0.5 && p.x >= -0.5 && p.y <= 0.5 && p.y >= -0.5 && (minT < 0 || t < minT))
            minT = t;
    }

    return minT;
}

// Calculate the normal vector at parametrically defined intersection point
glm::vec3 Cube::normal(glm::vec3 eye, glm::vec3 dir, float t) {

    glm::vec3 normals[] = {glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                           glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f) };

    for (glm::vec3 & n : normals){
        float t1 = ((n.x*n.x*0.5f + n.y*n.y*0.5f + n.z*n.z*0.5f) - (n.x*eye.x + n.y*eye.y + n.z*eye.z)) /
                (n.x*dir.x + n.y*dir.y + n.z*dir.z);
        if (t == t1)
            return n;
    }
    return glm::vec3(0.0f);
}

// Calculate the texture coordinates for a given intersection point
glm::vec2 Cube::textureCoordinates(glm::vec3 eye, glm::vec3 dir, float t) {
    glm::vec3 p = eye + dir*t;

    glm::vec3 normals[] = {glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                           glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f) };

    int index = 0;
    for (glm::vec3 & n : normals){
        float t1 = ((n.x*n.x*0.5f + n.y*n.y*0.5f + n.z*n.z*0.5f) - (n.x*eye.x + n.y*eye.y + n.z*eye.z)) /
                (n.x*dir.x + n.y*dir.y + n.z*dir.z);
        if (t == t1) {
            if (index == 0)
                return glm::vec2(0.5f - p.z, 0.5f - p.y);
            if (index == 1)
                return glm::vec2(p.z + 0.5f, 0.5f - p.y);
            if (index == 2)
                return glm::vec2(p.x + 0.5f, p.z + 0.5f);
            if (index == 3)
                return glm::vec2(p.x + 0.5f, 0.5f - p.z);
            if (index == 4)
                return glm::vec2(p.x + 0.5f, 0.5f - p.y);
            if (index == 5)
                return glm::vec2(0.5f - p.x, 0.5f - p.y);
        }
        index++;
    }

    return glm::vec2(0.0f,0.0f);
}

void Cube::init(const GLuint vertexLocation, const GLuint normalLocation){
    // Initialize the vertex buffer data
    m_isInitialized = true;
    m_vertexBufferData = new glm::vec3[m_numVertices*2]; // The *2 is to account for normal vectors

    // Calculate the incremental step (i.e. the sidelength for an individual sub-square)
    float delta = 1.0f / (float)m_numSquares;
    int idx = 0;

    // Loop through each sub-square and add them to the six faces
    for (int row=0; row<m_numSquares; row++) {
        for (int col=0; col<m_numSquares; col++) {
            // Add to the FRONT XY face
            glm::vec3 v1 = glm::vec3(0.5f - (delta * col), 0.5f - (delta * row), 0.5f);
            glm::vec3 v2 = glm::vec3(0.5f - (delta * col), 0.5f - (delta * (row+1)), 0.5f);
            glm::vec3 v3 = glm::vec3(0.5f - (delta * (col+1)), 0.5f - (delta * (row+1)), 0.5f);
            glm::vec3 v4 = glm::vec3(0.5f - (delta * (col+1)), 0.5f - (delta * row), 0.5f);
            glm::vec3 norm = glm::vec3(0.0f, 0.0f, 1.0f);
            quadTessellate(v4, norm, v3, norm, v2, norm, v1, norm, 12*idx); idx++;
            // Add to the BACK XY face
            v1 = glm::vec3(0.5f - (delta * col), 0.5f - (delta * row), -0.5f);
            v2 = glm::vec3(0.5f - (delta * col), 0.5f - (delta * (row+1)), -0.5f);
            v3 = glm::vec3(0.5f - (delta * (col+1)), 0.5f - (delta * (row+1)), -0.5f);
            v4 = glm::vec3(0.5f - (delta * (col+1)), 0.5f - (delta * row), -0.5f);
            norm = glm::vec3(0.0f, 0.0f, -1.0f);
            quadTessellate(v1, norm, v2, norm, v3, norm, v4, norm, 12*idx); idx++;
            // Add to the BACK ZY face
            v1 = glm::vec3(-0.5f, 0.5f - (delta * row), 0.5f - (delta * col));
            v2 = glm::vec3(-0.5f, 0.5f - (delta * (row+1)), 0.5f - (delta * col));
            v3 = glm::vec3(-0.5f, 0.5f - (delta * (row+1)), 0.5f - (delta * (col+1)));
            v4 = glm::vec3(-0.5f, 0.5f - (delta * row), 0.5f - (delta * (col+1)));
            norm = glm::vec3(-1.0f, 0.0f, 0.0f);
            quadTessellate(v4, norm, v3, norm, v2, norm, v1, norm, 12*idx); idx++;
            // Add to the FRONT ZY face
            v1 = glm::vec3(0.5f, 0.5f - (delta * row), 0.5f - (delta * col));
            v2 = glm::vec3(0.5f, 0.5f - (delta * (row+1)), 0.5f - (delta * col));
            v3 = glm::vec3(0.5f, 0.5f - (delta * (row+1)), 0.5f - (delta * (col+1)));
            v4 = glm::vec3(0.5f, 0.5f - (delta * row), 0.5f - (delta * (col+1)));
            norm = glm::vec3(1.0f, 0.0f, 0.0f);
            quadTessellate(v1, norm, v2, norm, v3, norm, v4, norm, 12*idx); idx++;
            // Add to the BACK XZ face
            v1 = glm::vec3(0.5f - (delta * col), -0.5f, 0.5f - (delta * row));
            v2 = glm::vec3(0.5f - (delta * col), -0.5f, 0.5f - (delta * (row+1)));
            v3 = glm::vec3(0.5f - (delta * (col+1)), -0.5f, 0.5f - (delta * (row+1)));
            v4 = glm::vec3(0.5f - (delta * (col+1)), -0.5f, 0.5f - (delta * row));
            norm = glm::vec3(0.0f, -1.0f, 0.0f);
            quadTessellate(v4, norm, v3, norm, v2, norm, v1, norm, 12*idx); idx++;
            // Add to the FRONT XZ face
            v1 = glm::vec3(0.5f - (delta * col), 0.5f, 0.5f - (delta * row));
            v2 = glm::vec3(0.5f - (delta * col), 0.5f, 0.5f - (delta * (row+1)));
            v3 = glm::vec3(0.5f - (delta * (col+1)), 0.5f, 0.5f - (delta * (row+1)));
            v4 = glm::vec3(0.5f - (delta * (col+1)), 0.5f, 0.5f - (delta * row));
            norm = glm::vec3(0.0f, 1.0f, 0.0f);
            quadTessellate(v1, norm, v2, norm, v3, norm, v4, norm, 12*idx); idx++;
        }
    }

    // OpenGL commands to bind the vertex data
    bindData(vertexLocation, normalLocation);

}


