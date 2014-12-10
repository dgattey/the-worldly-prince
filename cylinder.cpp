#include "cylinder.h"
#include <iostream>
//#include "lib/CS123Common.h"
//#include "scenegraph/OpenGLScene.h"
//#include "ui/Settings.h"
#include "shape.h"

#include <math.h>

Cylinder::Cylinder()
{
    m_isInitialized = false;
}

Cylinder::Cylinder(const int paramOne, const int paramTwo, const int paramThree, const GLuint vertexLocation, const GLuint normalLocation) :
    Shape(1, paramOne, paramTwo, paramThree, vertexLocation, normalLocation)
{
    m_numCuts = paramOne;
    m_numSlices = paramTwo;
    m_numVertices = ((2*m_numCuts+2*(2*m_numCuts - 1))*m_numSlices)*3;//12*m_numSlices;
    init(vertexLocation, normalLocation);
}

Cylinder::~Cylinder()
{
    glDeleteVertexArrays(1, &m_vaoID);
    glDeleteBuffers(1, &m_vbo);
}

// RAY INTERSECTION
// Calculate the t-parameter for the intersection between a ray and a shape
float Cylinder::intersect(glm::vec3 eye, glm::vec3 dir) {
    float minT = -1;

    float a = dir.x*dir.x + dir.z*dir.z;
    float b = 2.0*eye.x*dir.x + 2.0*eye.z*dir.z;
    float c = eye.x*eye.x + eye.z*eye.z - 0.25;

    // Check for intersection with cylinder body (quadratic formula Part 1)
    float t1 = (-b + sqrt(b*b - 4*a*c)) / (2*a);
    glm::vec3 p1 = eye + t1*dir;
    if (p1.y > -0.5 && p1.y < 0.5 && (t1 < minT || minT < 0)) {
        minT = t1;
    }
    // Check for intersection with cylinder body (quadratic formula Part 2)
    float t2 = (-b - sqrt(b*b - 4*a*c)) / (2*a);
    glm::vec3 p2 = eye + t2*dir;
    if (p2.y > -0.5 && p2.y < 0.5 && (t2 < minT || minT < 0)){
        minT = t2;
    }
    // Check for intersection with cylinder's top cap
    float t3 = (0.5 - eye.y) / dir.y;
    glm::vec3 p3 = eye + t3*dir;
    if ((p3.x*p3.x + p3.z*p3.z <= 0.25) && (t3 < minT || minT < 0)) {
        minT = t3;
    }
    // Check for intersection with cylinder's bottom cap
    float t4 = (-0.5 - eye.y) / dir.y;
    glm::vec3 p4 = eye + t4*dir;
    if ((p4.x*p4.x + p4.z*p4.z <= 0.25) && (t4 < minT || minT < 0)) {
        minT = t4;
    }

    return minT;
}

// Calculate the normal vector at parametrically defined intersection point
glm::vec3 Cylinder::normal(glm::vec3 eye, glm::vec3 dir, float t) {

    // Check to see if the intersection point is on the top plane...
    float t3 = (0.5 - eye.y) / dir.y;
    if(t == t3)
        return glm::vec3(0.0f, 1.0f, 0.0f);
    // ...or on the bottom plane
    float t4 = (-0.5 - eye.y) / dir.y;
    if(t == t4)
        return glm::vec3(0.0f, -1.0f, 0.0f);

    // Otherwise, return the cylinder body's normal
    glm::vec3 p = eye + dir*t;
    return glm::normalize(glm::vec3(2.0f*p.x, 0.0f, 2.0f*p.z));

}

// Calculate the texture coordinates for a given intersection point
glm::vec2 Cylinder::textureCoordinates(glm::vec3 eye, glm::vec3 dir, float t) {
    glm::vec3 p = eye + dir*t;
    // Check to see if the intersection point is on the top plane...
    float t3 = (0.5 - eye.y) / dir.y;
    if(t == t3)
        return glm::vec2(p.x + 0.5f, p.z + 0.5f);
    // ...or on the bottom plane
    float t4 = (-0.5 - eye.y) / dir.y;
    if(t == t4)
        return glm::vec2(p.x + 0.5f, 0.5f - p.z);

    float theta = std::atan2(p.z, p.x);
    float u, v;
    v = 0.5 - p.y;
    if (theta < 0) {
        u = -theta / (2*M_PI);
    } else {
        u = 1 - (theta / (2*M_PI));
    }
    return glm::vec2(u,v);
}

void Cylinder::init(const GLuint vertexLocation, const GLuint normalLocation){
    // Initializations
    m_isInitialized = true;
    m_vertexBufferData = new glm::vec3[(m_numVertices)*2];

    // Calculate incrementations of theta
    float dTheta = 2.0f*M_PI / m_numSlices;
    float theta1 = 0;
    float theta2;

    int vps = (m_numVertices / m_numSlices) *2; // vectors per slice
    int vpcps = 12;                     // vectors per cut per slice

    // Tessellate each slice of the cylinder
    for (int slice=0; slice<m_numSlices; slice++) {
        theta2 = theta1 + dTheta;
        int cut_ID = 0;

        float dR = 0.5f / m_numCuts;
        float radius1 = 0;
        float radius2 = dR;

        //Build the bottom and top circle caps, starting with the interior triangle fan
        glm::vec3 p0 = glm::vec3(0.0f, -0.5f, 0.0f);
        glm::vec3 p1 = glm::vec3(radius2*cos(theta1), -0.5f, radius2*sin(theta1));
        glm::vec3 p2 = glm::vec3(radius2*cos(theta2), -0.5f, radius2*sin(theta2));
        m_vertexBufferData[slice*vps+cut_ID*vpcps] = p0;
        m_vertexBufferData[slice*vps+cut_ID*vpcps+1] = glm::vec3(0.0f, -1.0f, 0.0f);
        m_vertexBufferData[slice*vps+cut_ID*vpcps+2] = p1;
        m_vertexBufferData[slice*vps+cut_ID*vpcps+3] = glm::vec3(0.0f, -1.0f, 0.0f);
        m_vertexBufferData[slice*vps+cut_ID*vpcps+4] = p2;
        m_vertexBufferData[slice*vps+cut_ID*vpcps+5] = glm::vec3(0.0f, -1.0f, 0.0f);

        p0 = glm::vec3(0.0f, 0.5f, 0.0f);
        p1 = glm::vec3(radius2*cos(theta1), 0.5f, radius2*sin(theta1));
        p2 = glm::vec3(radius2*cos(theta2), 0.5f, radius2*sin(theta2));
        m_vertexBufferData[slice*vps+cut_ID*vpcps+6] = p0;
        m_vertexBufferData[slice*vps+cut_ID*vpcps+7] = glm::vec3(0.0f, 1.0f, 0.0f);
        m_vertexBufferData[slice*vps+cut_ID*vpcps+8] = p2;
        m_vertexBufferData[slice*vps+cut_ID*vpcps+9] = glm::vec3(0.0f, 1.0f, 0.0f);
        m_vertexBufferData[slice*vps+cut_ID*vpcps+10] = p1;
        m_vertexBufferData[slice*vps+cut_ID*vpcps+11] = glm::vec3(0.0f, 1.0f, 0.0f);

        cut_ID++;
        radius1 = radius2;
        // Finish building the bottom cap with quadrilaterals
        for (int cut=0; cut<m_numCuts-1; cut++) {
                radius2 = radius1 + dR;
                glm::vec3 v1 = glm::vec3(radius1*cos(theta2), -0.5f, radius1*sin(theta2));
                glm::vec3 v2 = glm::vec3(radius1*cos(theta1), -0.5f, radius1*sin(theta1));
                glm::vec3 v3 = glm::vec3(radius2*cos(theta1), -0.5f, radius2*sin(theta1));
                glm::vec3 v4 = glm::vec3(radius2*cos(theta2), -0.5f, radius2*sin(theta2));
                glm::vec3 norm = glm::vec3(0.0f, -1.0f, 0.0f);
                quadTessellate(v1, norm, v2, norm, v3, norm, v4, norm, slice*vps+cut_ID*vpcps);

                radius1 = radius2;
                cut_ID++;
        }
        // Finish building the top cap with quadrilaterals
        radius1 = dR;
        for (int cut=0; cut<m_numCuts-1; cut++) {
                radius2 = radius1 + dR;
                glm::vec3 v1 = glm::vec3(radius1*cos(theta2), 0.5f, radius1*sin(theta2));
                glm::vec3 v2 = glm::vec3(radius1*cos(theta1), 0.5f, radius1*sin(theta1));
                glm::vec3 v3 = glm::vec3(radius2*cos(theta1), 0.5f, radius2*sin(theta1));
                glm::vec3 v4 = glm::vec3(radius2*cos(theta2), 0.5f, radius2*sin(theta2));
                glm::vec3 norm = glm::vec3(0.0f, 1.0f, 0.0f);
                quadTessellate(v4, norm, v3, norm, v2, norm, v1, norm, slice*vps+cut_ID*vpcps);

                radius1 = radius2;
                cut_ID++;
        }

        // Build the sides entirely out of quadrilaterals
        float h1 = -0.5f;
        float h2;
        float dHeight = 1.0f / m_numCuts;
        for (int cut=0; cut<m_numCuts; cut++) {
            h2 = h1 + dHeight;
            glm::vec3 v1 = glm::vec3(0.5f*cos(theta2), h1, 0.5f*sin(theta2));
            glm::vec3 v2 = glm::vec3(0.5f*cos(theta1), h1, 0.5f*sin(theta1));
            glm::vec3 v3 = glm::vec3(0.5f*cos(theta1), h2, 0.5f*sin(theta1));
            glm::vec3 v4 = glm::vec3(0.5f*cos(theta2), h2, 0.5f*sin(theta2));
            glm::vec3 n1 = glm::vec3(cos(theta1), 0.0f, sin(theta1));
            glm::vec3 n2 = glm::vec3(cos(theta2), 0.0f, sin(theta2));
            quadTessellate(v1, n2, v2, n1, v3, n1, v4, n2, slice*vps+cut_ID*vpcps);

            h1 = h2;
            cut_ID++;
        }
        theta1 = theta2;
    }

    // Bind buffer data
    bindData(vertexLocation, normalLocation);

}


