#include "Cone.h"
#include <iostream>
#include "lib/CS123Common.h"
#include "scenegraph/OpenGLScene.h"
#include "ui/Settings.h"

#include <math.h>

Cone::Cone()
{
    m_isInitialized = false;
}

Cone::Cone(const int paramOne, const int paramTwo, const int paramThree, const GLuint vertexLocation, const GLuint normalLocation, NormalRenderer *normRender) :
    Shape(SHAPE_CONE, paramOne, paramTwo, paramThree, vertexLocation, normalLocation, normRender)
{
    m_numCuts = paramOne;
    m_numSlices = paramTwo;
    m_numVertices = (m_numSlices*(2*(2*m_numCuts-1)))*3; // Total number of vertices
    init(vertexLocation, normalLocation);
}

Cone::~Cone()
{
    glDeleteVertexArrays(1, &m_vaoID);
    glDeleteBuffers(1, &m_vbo);
}

// RAY INTERSECTION
// Calculate the t-parameter for the intersection between a ray and a shape
float Cone::intersect(glm::vec3 eye, glm::vec3 dir) {
    float minT = -1;

    float a = dir.x*dir.x + dir.z*dir.z - 0.25*dir.y*dir.y;
    float b = 2.0*eye.x*dir.x + 2.0*eye.z*dir.z - 0.5*eye.y*dir.y + 0.25*dir.y;
    float c = eye.x*eye.x + eye.z*eye.z - 0.25*eye.y*eye.y + 0.25*eye.y - 0.0625;

    // Check intersection with cone body (quadratic formula Part 1)
    float t1 = (-b + sqrt(b*b - 4*a*c)) / (2*a);
    glm::vec3 p1 = eye + t1*dir;
    if (p1.y > -0.5 && p1.y < 0.5 && (t1 < minT || minT < 0)) {
        minT = t1;
    }
    // Check intersection with cone body (quadratic formula Part 2)
    float t2 = (-b - sqrt(b*b - 4*a*c)) / (2*a);
    glm::vec3 p2 = eye + t2*dir;
    if (p2.y > -0.5 && p2.y < 0.5 && (t2 < minT || minT < 0)){
        minT = t2;
    }
    // Check for intersection with cone's bottom cap
    float t3 = (-0.5 - eye.y) / dir.y;
    glm::vec3 p3 = eye + t3*dir;
    if ((p3.x*p3.x + p3.z*p3.z <= 0.25) && (t3 < minT || minT < 0)) {
        minT = t3;
    }

    return minT;
}

// Calculate the normal vector at parametrically defined intersection point
glm::vec3 Cone::normal(glm::vec3 eye, glm::vec3 dir, float t) {

    // Check to see if the intersection point is on the bottom cap
    float t3 = (-0.5 - eye.y) / dir.y;
    if (t == t3) {
        return glm::vec3(0.0f, -1.0f, 0.0f); // bottom cap normal
    } else {
        glm::vec3 p = eye + dir*t;
        return glm::normalize(glm::vec3(2.0f*p.x, -0.5f*p.y + 0.25, 2.0f*p.z)); // cone body normal
    }

}

// Calculate the texture coordinates for a given intersection point
glm::vec2 Cone::textureCoordinates(glm::vec3 eye, glm::vec3 dir, float t) {
    glm::vec3 p = eye + dir*t;
    // Check to see if the intersection point is on the bottom plane
    float t3 = (-0.5 - eye.y) / dir.y;
    if(t == t3)
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

void Cone::init(const GLuint vertexLocation, const GLuint normalLocation){
    // Initialize vertex buffer data
    m_isInitialized = true;
    m_vertexBufferData = new glm::vec3[m_numVertices*2];

    // Calculate the incremental rotation around the y-axis
    float dTheta = 2.0f*M_PI / m_numSlices;
    float theta1 = 0;
    float theta2;

    int vps = (m_numVertices / m_numSlices) *2; // number of Vectors Per Slice (includes normals)
    int vpcps = 12;                     // number of Vectors Per Cut Per Slice

    // Tessellate triangles for each side of every slice
    for (int slice=0; slice<m_numSlices; slice++) {
        theta2 = theta1 + dTheta;
        int cut_ID = 0;

        // Calculate the change of radius between triangles along the base
        float dR = 0.5f / m_numCuts;
        float radius1 = 0;
        float radius2 = dR;

        //Build the bottom circle, starting with the interior triangle fan
        glm::vec3 p0 = glm::vec3(0.0f, -0.5f, 0.0f);
        glm::vec3 p1 = glm::vec3(radius2*cos(theta1), -0.5f, radius2*sin(theta1));
        glm::vec3 p2 = glm::vec3(radius2*cos(theta2), -0.5f, radius2*sin(theta2));
        m_vertexBufferData[slice*vps+cut_ID*vpcps] = p0;
        m_vertexBufferData[slice*vps+cut_ID*vpcps+1] = glm::vec3(0.0f, -1.0f, 0.0f);
        m_vertexBufferData[slice*vps+cut_ID*vpcps+2] = p1;
        m_vertexBufferData[slice*vps+cut_ID*vpcps+3] = glm::vec3(0.0f, -1.0f, 0.0f);
        m_vertexBufferData[slice*vps+cut_ID*vpcps+4] = p2;
        m_vertexBufferData[slice*vps+cut_ID*vpcps+5] = glm::vec3(0.0f, -1.0f, 0.0f);
        radius1 = radius2;

        // Finish building the base out of quadrilaterals
        for (int cut=0; cut<m_numCuts-1; cut++) {
            radius2 = radius1 + dR;
            // Tessellate quadrilaterals along the base
            glm::vec3 v1 = glm::vec3(radius1*cos(theta2), -0.5f, radius1*sin(theta2));
            glm::vec3 v2 = glm::vec3(radius1*cos(theta1), -0.5f, radius1*sin(theta1));
            glm::vec3 v3 = glm::vec3(radius2*cos(theta1), -0.5f, radius2*sin(theta1));
            glm::vec3 v4 = glm::vec3(radius2*cos(theta2), -0.5f, radius2*sin(theta2));
            glm::vec3 norm = glm::vec3(0.0f, -1.0f, 0.0f);
            quadTessellate(v1, norm, v2, norm, v3, norm, v4, norm, slice*vps+6+cut_ID*vpcps);

            radius1 = radius2;
            cut_ID++;
        }

        radius1 = 0.5f;
        float dHeight = 1.0f / m_numCuts;
        float h1 = -0.5f;
        float h2;

        // Build quadrilaterals up the side of the cone
        for (int cut=0; cut<m_numCuts-1; cut++) {
            h2 = h1 + dHeight;
            radius2 = radius1 - dR;
            // Tessellate quadrilaterals along the side
            glm::vec3 v1 = glm::vec3(radius1*cos(theta2), h1, radius1*sin(theta2));
            glm::vec3 v2 = glm::vec3(radius1*cos(theta1), h1, radius1*sin(theta1));
            glm::vec3 v3 = glm::vec3(radius2*cos(theta1), h2, radius2*sin(theta1));
            glm::vec3 v4 = glm::vec3(radius2*cos(theta2), h2, radius2*sin(theta2));
            glm::vec3 n1 = glm::normalize(glm::vec3(cos(theta1), 0.5f, sin(theta1)));
            glm::vec3 n2 = glm::normalize(glm::vec3(cos(theta2), 0.5f, sin(theta2)));
            quadTessellate(v1, n2, v2, n1, v3, n1, v4, n2, slice*vps+6+cut_ID*vpcps);

            radius1 = radius2;
            h1 = h2;
            cut_ID++;
        }

        //Build the tip of the cone
        p0 = glm::vec3(0.0f, 0.5f, 0.0f);
        p1 = glm::vec3(radius2*cos(theta1), h1, radius2*sin(theta1));
        p2 = glm::vec3(radius2*cos(theta2), h1, radius2*sin(theta2));
        m_vertexBufferData[slice*vps+cut_ID*vpcps+6] = p0;
        m_vertexBufferData[slice*vps+cut_ID*vpcps+7] = glm::normalize(glm::vec3(cos(theta2 - (dTheta/2.0)), 0.5f, sin(theta2 - (dTheta/2.0))));
        m_vertexBufferData[slice*vps+cut_ID*vpcps+8] = p2;
        m_vertexBufferData[slice*vps+cut_ID*vpcps+9] = glm::normalize(glm::vec3(cos(theta2), 0.5f, sin(theta2)));
        m_vertexBufferData[slice*vps+cut_ID*vpcps+10] = p1;
        m_vertexBufferData[slice*vps+cut_ID*vpcps+11] = glm::normalize(glm::vec3(cos(theta1), 0.5f, sin(theta1)));

        theta1 = theta2;
    }

    // Buffer vertex data
    bindData(vertexLocation, normalLocation);
}
