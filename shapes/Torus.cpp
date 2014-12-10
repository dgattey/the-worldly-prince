#include "Torus.h"
#include <iostream>
#include "lib/CS123Common.h"
#include "scenegraph/OpenGLScene.h"
#include "ui/Settings.h"

#include <math.h>

Torus::Torus()
{
    m_isInitialized = false;
}

Torus::Torus(const int paramOne, const int paramTwo, const int paramThree, const GLuint vertexLocation, const GLuint normalLocation, NormalRenderer *normRender) :
    Shape(SHAPE_TORUS, paramOne, paramTwo, paramThree, vertexLocation, normalLocation, normRender)
{
    m_numCuts = paramOne;
    m_numSlices = paramTwo;
    m_innerRadius = paramThree;
    m_numVertices = paramOne*2*paramTwo*3; // Total number of vertices
    init(vertexLocation, normalLocation);
}

Torus::~Torus()
{
    glDeleteVertexArrays(1, &m_vaoID);
    glDeleteBuffers(1, &m_vbo);
}

void Torus::init(const GLuint vertexLocation, const GLuint normalLocation){
    // Initializations
    m_isInitialized = true;
    m_vertexBufferData = new glm::vec3[(m_numVertices*2)];

    // Calculate incremental changes in theta and phi
    float dTheta = 2.0*M_PI / m_numSlices;
    float dPhi = 2*M_PI / m_numCuts;
    float theta1 = 0;
    float theta2;
    // Calculate torus radius as well as distance to the center of the torus
    float smallR = (0.5f * (float)m_innerRadius / 100.0f);
    float bigR = 0.5f - smallR;

    int vps = (m_numVertices / m_numSlices) * 2; //vectors per slice
    int vpcps = 12;                    // vectors per cut, per slice

    // Tessellate each slice of the torus
    for (int slice=0; slice<m_numSlices; slice++) {
        theta2 = theta1 + dTheta;
        float phi1 = 0;
        float phi2 = phi1 + dPhi;
        // Tessellate each quadrilateral of each slice
        for (int cut=0; cut<m_numCuts; cut++) {
            phi2 = phi1 + dPhi;
            glm::vec3 v1 = glm::vec3((bigR + smallR*cos(phi1))*cos(theta1), smallR*sin(phi1), (bigR + smallR*cos(phi1))*sin(theta1));
            glm::vec3 v2 = glm::vec3((bigR + smallR*cos(phi2))*cos(theta1), smallR*sin(phi2), (bigR + smallR*cos(phi2))*sin(theta1));
            glm::vec3 v3 = glm::vec3((bigR + smallR*cos(phi2))*cos(theta2), smallR*sin(phi2), (bigR + smallR*cos(phi2))*sin(theta2));
            glm::vec3 v4 = glm::vec3((bigR + smallR*cos(phi1))*cos(theta2), smallR*sin(phi1), (bigR + smallR*cos(phi1))*sin(theta2));
            glm::vec3 n1 = glm::vec3(cos(theta1)*cos(phi1), sin(phi1), sin(theta1)*cos(phi1));
            glm::vec3 n2 = glm::vec3(cos(theta1)*cos(phi2), sin(phi2), sin(theta1)*cos(phi2));
            glm::vec3 n3 = glm::vec3(cos(theta2)*cos(phi2), sin(phi2), sin(theta2)*cos(phi2));
            glm::vec3 n4 = glm::vec3(cos(theta2)*cos(phi1), sin(phi1), sin(theta2)*cos(phi1));

            quadTessellate(v1, n1, v2, n2, v3, n3, v4, n4, vps*slice+vpcps*cut);

            phi1 = phi2;
        }
        // increment theta
        theta1 = theta2;
    }

    // Bind buffer data
    bindData(vertexLocation, normalLocation);

}

