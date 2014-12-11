#include "shape.h"
#include <iostream>
//#include "lib/CS123Common.h"
//#include "scenegraph/OpenGLScene.h"
//#include "ui/Settings.h"

Shape::Shape()
{
    m_isInitialized = false;
}

Shape::Shape(int type, int paramOne, int paramTwo, int paramThree, const GLuint vertexLocation, const GLuint normalLocation){
    m_paramOne = paramOne;
    m_paramTwo = paramTwo;
    m_paramThree = paramThree;
    m_type = type;
    m_vertexBufferData = NULL;
}

Shape::~Shape()
{
    glDeleteVertexArrays(1, &m_vaoID);
    glDeleteBuffers(1, &m_vbo);
    delete [] m_vertexBufferData;
}

// Graphically draws the shape
void Shape::draw() {
    if (!m_isInitialized){
        std::cout << "You must call init() before you can draw!" << std::endl;
    } else{
        // Rebind your vertex array and draw the triangles
        glBindVertexArray(m_vaoID);
        glDrawArrays(GL_TRIANGLES, 0, m_numVertices);
        glBindVertexArray(0);
    }
}

//// Tells whether input parameters match those of the shape
//bool Shape::paramsEqual(int type, int p1, int p2, int p3) {
//    bool isSame;
//    if (type == SHAPE_TORUS) {// Only a torus should care about paramThree
//        isSame = (m_type == type && m_paramOne == p1 && m_paramTwo == p2 && m_paramThree == p3);
//    } else {
//        if (type == SHAPE_CUBE) { // And a cube shouldn't care about paramTwo
//            isSame = (m_type == type && m_paramOne == p1);
//        } else {
//            isSame = (m_type == type && m_paramOne == p1 && m_paramTwo == p2);
//        }
//    }
//    return isSame;
//}

void Shape::bindData(const GLuint vertexLocation, const GLuint normalLocation){
    // Initialize the vertex array object.
    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);

    // Initialize the vertex buffer object.
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    // Pass vertex data to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*m_numVertices*2, m_vertexBufferData, GL_STATIC_DRAW);
//    delete [] m_vertexBufferData;
//    m_vertexBufferData = NULL;

    // Expose vertices to shader
    glEnableVertexAttribArray(vertexLocation);
    glVertexAttribPointer(
       vertexLocation,
       3,                    // num vertices per element (3 for triangle)
       GL_FLOAT,             // type
       GL_FALSE,             // normalized?
       2*sizeof(glm::vec3),  // stride
       (void*)0              // array buffer offset
    );

    glEnableVertexAttribArray(normalLocation);
    glVertexAttribPointer(
       normalLocation,
       3,                           // num vertices per element (3 for triangle)
       GL_FLOAT,                    // type
       GL_TRUE,                     // normalized?
       2*sizeof(glm::vec3),         // stride
       (void*) sizeof(glm::vec3)    // array buffer offset
    );

    //Clean up -- unbind buffer
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    // Initialize normals so they can be displayed with arrows. (This can be very helpful
    // for debugging!)
    // This object (m_normalRenderer) can be passed around to other classes,
    // make sure to include "OpenGLScene.h" in any class you want to use the NormalRenderer!
    // generateArrays will take care of any cleanup from the previous object state.
//    m_normRender->generateArrays(
//                (GLfloat *)(m_vertexBufferData),   // Pointer to vertex data
//                2 * sizeof(glm::vec3),             // Stride (distance between consecutive vertices/normals in BYTES
//                0,                                 // Offset of first position in BYTES
//                sizeof(glm::vec3),                 // Offset of first normal in BYTES
//                m_numVertices);                    // Number of vertices
}

// Creates a quadrilateral based on the given vertices and their normals
void Shape::quadTessellate(glm::vec3 v1, glm::vec3 n1, glm::vec3 v2, glm::vec3 n2, glm::vec3 v3, glm::vec3 n3, glm::vec3 v4, glm::vec3 n4, int idx) {
    m_vertexBufferData[idx] = v1;
    m_vertexBufferData[idx+1] = n1;
    m_vertexBufferData[idx+2] = v2;
    m_vertexBufferData[idx+3] = n2;
    m_vertexBufferData[idx+4] = v3;
    m_vertexBufferData[idx+5] = n3;

    m_vertexBufferData[idx+6] = v1;
    m_vertexBufferData[idx+7] = n1;
    m_vertexBufferData[idx+8] = v3;
    m_vertexBufferData[idx+9] = n3;
    m_vertexBufferData[idx+10] = v4;
    m_vertexBufferData[idx+11] = n4;
}
