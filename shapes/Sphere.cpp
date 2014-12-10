#include "Sphere.h"
#include <iostream>
#include "lib/CS123Common.h"
#include "scenegraph/OpenGLScene.h"
#include "ui/Settings.h"

#include <math.h>

Sphere::Sphere()
{
    m_isInitialized = false;
}

Sphere::Sphere(const int paramOne, const int paramTwo, const int paramThree, const GLuint vertexLocation, const GLuint normalLocation, NormalRenderer *normRender) :
    Shape(SHAPE_SPHERE, paramOne, paramTwo, paramThree, vertexLocation, normalLocation, normRender)
{
    m_numCuts = paramOne;
    m_numSlices = paramTwo;
    m_numVertices = ((paramOne-2)*2+2)*paramTwo*3;// Total number of vertices
    init(vertexLocation, normalLocation);
}

Sphere::~Sphere()
{
    glDeleteVertexArrays(1, &m_vaoID);
    glDeleteBuffers(1, &m_vbo);
}

// RAY INTERSECTION
// Calculate the t-parameter for the intersection between a ray and a shape
float Sphere::intersect(glm::vec3 eye, glm::vec3 dir) {
    float minT = -1;

    float a = dir.x*dir.x + dir.y*dir.y + dir.z*dir.z;
    float b = 2.0*eye.x*dir.x + 2.0*eye.y*dir.y + 2.0*eye.z*dir.z;
    float c = eye.x*eye.x + eye.y*eye.y + eye.z*eye.z - 0.25;

    // Check for intersection with the sphere (quadratic formula Part 1)
    float t1 = (-b + sqrt(b*b - 4*a*c)) / (2*a);
    if (t1 >= 0 && (t1 < minT || minT < 0)) {
        minT = t1;
    }
    // Check for intersection with the sphere (quadratic formula Part 2)
    float t2 = (-b - sqrt(b*b - 4*a*c)) / (2*a);
    if (t2 >= 0 && (t2 < minT || minT < 0)){
        minT = t2;
    }

    return minT;
}

// Calculate the normal vector at parametrically defined intersection point
glm::vec3 Sphere::normal(glm::vec3 eye, glm::vec3 dir, float t) {
    glm::vec3 p = eye + dir*t;
    // Return the sphere's normal vector
    return glm::normalize(glm::vec3(2.0f*p.x, 2.0f*p.y, 2.0f*p.z));
}

// Calculate the texture coordinates for a given intersection point
glm::vec2 Sphere::textureCoordinates(glm::vec3 eye, glm::vec3 dir, float t) {
    glm::vec3 p = eye + dir*t;
    float theta = std::atan2(p.z, p.x);
    float phi = std::asin(p.y/0.5);
    float u, v;
    v = 0.5f - phi/M_PI;
    if (v == 0 || v == 1) {
        u = 0.5f;
        return glm::vec2(u,v);
    }
    if (theta < 0) {
        u = -theta / (2*M_PI);
    } else {
        u = 1 - (theta / (2*M_PI));
    }
    return glm::vec2(u,v);
}

void Sphere::init(const GLuint vertexLocation, const GLuint normalLocation){
    // Initializations
    m_isInitialized = true;
    m_vertexBufferData = new glm::vec3[(m_numVertices*2)];

    // Calculate incremental changes in theta and phi
    float dTheta = 2.0*M_PI / m_numSlices;
    float dPhi = M_PI / m_numCuts;
    float theta1 = 0;
    float theta2;

    int vps = (m_numVertices / m_numSlices) * 2; //vectors per slice

    // Tessellate each slice of the sphere
    for (int slice=0; slice<m_numSlices; slice++) {
        theta2 = theta1 + dTheta;
        float phi1 = 0;
        float phi2 = phi1 + dPhi;
        //Adds the tip triangle for the slice
        glm::vec3 p0 = glm::vec3(0.0f, 0.5f, 0.0f);
        glm::vec3 p1 = glm::vec3(0.5*sin(phi2)*cos(theta2), 0.5*cos(phi2), 0.5*sin(phi2)*sin(theta2));
        glm::vec3 p2 = glm::vec3(0.5*sin(phi2)*cos(theta1), 0.5*cos(phi2), 0.5*sin(phi2)*sin(theta1));
        m_vertexBufferData[vps*slice] = p0;
        m_vertexBufferData[vps*slice+1] = glm::normalize(p0);
        m_vertexBufferData[vps*slice+2] = p1;
        m_vertexBufferData[vps*slice+3] = glm::normalize(p1);
        m_vertexBufferData[vps*slice+4] = p2;
        m_vertexBufferData[vps*slice+5] = glm::normalize(p2);
        phi1 = phi2;

        // Tessellate the middle of the slice out of quadrilaterals
        for (int cut=0; cut<m_numCuts-2; cut++) {
            phi2 = phi1 + dPhi;
            glm::vec3 v1 = glm::vec3(0.5*sin(phi1)*cos(theta1), 0.5*cos(phi1), 0.5*sin(phi1)*sin(theta1));
            glm::vec3 v2 = glm::vec3(0.5*sin(phi1)*cos(theta2), 0.5*cos(phi1), 0.5*sin(phi1)*sin(theta2));
            glm::vec3 v3 = glm::vec3(0.5*sin(phi2)*cos(theta2), 0.5*cos(phi2), 0.5*sin(phi2)*sin(theta2));
            glm::vec3 v4 = glm::vec3(0.5*sin(phi2)*cos(theta1), 0.5*cos(phi2), 0.5*sin(phi2)*sin(theta1));
            quadTessellate(v1, glm::normalize(v1), v2, glm::normalize(v2), v3, glm::normalize(v3), v4, glm::normalize(v4), vps*slice+6+12*cut);

            phi1 = phi2;
        }
        // Add the bottom tip triangle of the slice
        phi2 = phi1 + dPhi;
        p0 = glm::vec3(0.0f, -0.5f, 0.0f);
        p1 = glm::vec3(0.5*sin(phi1)*cos(theta2), 0.5*cos(phi1), 0.5*sin(phi1)*sin(theta2));
        p2 = glm::vec3(0.5*sin(phi1)*cos(theta1), 0.5*cos(phi1), 0.5*sin(phi1)*sin(theta1));
        m_vertexBufferData[vps*slice+6+12*(m_numCuts-2)] = p0; //Adds the tip triangle for the slice
        m_vertexBufferData[vps*slice+6+12*(m_numCuts-2)+1] = glm::normalize(p0);
        m_vertexBufferData[vps*slice+6+12*(m_numCuts-2)+2] = p2;
        m_vertexBufferData[vps*slice+6+12*(m_numCuts-2)+3] = glm::normalize(p2);
        m_vertexBufferData[vps*slice+6+12*(m_numCuts-2)+4] = p1;
        m_vertexBufferData[vps*slice+6+12*(m_numCuts-2)+5] = glm::normalize(p1);

        theta1 = theta2;
    }

    // Bind buffer data
    bindData(vertexLocation, normalLocation);

}
