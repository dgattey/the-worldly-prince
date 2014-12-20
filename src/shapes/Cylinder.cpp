#include "Cylinder.h"
#include "Shape.h"
#include "Util.h"

#define FACES 6
#define NUM_VERTS 3

/**
 * @brief Constructor for a cylinder - bounds the params and creates geometry
 * @param shader The shader for this shape
 * @param param1 The horizontal tesselation
 * @param param2 The vertical tesselation
 */
Cylinder::Cylinder(GLuint shader, int param1, int param2) {
    Shape::initShape(shader, param1, param2);
    boundParams();
    createGeometry();
}

/**
 * @brief Empty deconstructor
 */
Cylinder::~Cylinder() {}

/**
 * @brief Intersects a ray with the sides of the circle that is the infinite cylinder
 * @param p The point to start from
 * @param d The direction to move by t
 * @param data An object encapsulating relevant information for casting
 */
void Cylinder::rayCircleIntersect(glm::vec3 p, glm::vec3 d, RayData *data) {
    float a = d.x*d.x + d.z*d.z;
    float b = 2.0*p.x*d.x + 2.0*p.z*d.z;
    float c = p.x*p.x + p.z*p.z - RADIUS_SQ;

    // Make sure in bounds
    Shape::rayCircleBoundsCheckT(p, d, a, b, c, RADIUS, CYLINDER_SIDE, data);
}

/**
 * @brief Computes a t-value for intersection given a point, dist, and data
 * @param p The point to start from
 * @param d The direction to move by t
 * @param data An object encapsulting relevant information for casting
 */
void Cylinder::computeT(glm::vec3 p, glm::vec3 d, RayData *data) {
    Cylinder::rayCircleIntersect(p, d, data); // Infinite cylinder
    Shape::rayCapIntersectT(p, d, RADIUS, CYLINDER_TOP_CAP, data); // Top cap
    Shape::rayCapIntersectT(p, d, -RADIUS, CYLINDER_BOTTOM_CAP, data); // Bottom cap
}

/**
 * @brief Given an eye, dir, and calculated data, gives back the normal
 * Assumes computeT has already been called and the data object has a
 * t value as well as a shape part for the intersection
 * @param eye The eye point of the camera
 * @param dir The direction to move in
 * @param data An object encapsulating relevant information for computation
 */
void Cylinder::computeNorm(glm::vec3 eye, glm::vec3 dir, RayData *data) {
    switch(data->part) {
    case CYLINDER_BOTTOM_CAP:
        Shape::rayCapIntersectNorm(-RADIUS, data);
        break;
    case CYLINDER_SIDE:
        Shape::rayCircleBoundsCheckNorm(eye, dir, false, data);
        break;
    case CYLINDER_TOP_CAP:
        Shape::rayCapIntersectNorm(RADIUS, data);
        break;
    default:
        break;
    }
}

/**
 * @brief Computes a texture for the cylinder given rayData and a pointer to tex
 * @param rayData An object with relevant information for texture finding
 * @param texData The location to save the texture data
 */
void Cylinder::computeTexture(RayData *data, TexturePointData *texData){
    computeCylindricalTexture(data, texData);
}

/**
 * @brief Given new p1 and p2 values, update the geometry
 * @param p1 A new horizontal resolution value
 * @param p2 A new vertical resolution value
 */
void Cylinder::updateGeometry(int p1, int p2) {
    m_p1 = p1;
    m_p2 = p2;
    boundParams();
    cleanupGL();
    setupGL();
    createGeometry();
}

/**
 * @brief Bound the parameters to something sensible
 */
void Cylinder::boundParams() {
    Cone::boundParams();
}

/**
 * @brief Actually create the triangular geometry and pass all data to GL
 */
void Cylinder::createGeometry() {
    int arrayPos = 0;
    const float st = 0.5;

    // Numbers of triangles
    m_oneRingTri = m_p2;                        // How many in first ring
    m_restRingTri = 2*m_oneRingTri*(m_p1-1);    // Rest of rings have this many in total
    m_numTriangles = 2*(m_oneRingTri+2)+NUM_VERTS*(2*m_restRingTri+2*m_p1*m_p2); // Count of all verts
    m_vertexData = new GLfloat[6*m_numTriangles];

    // Delta for angle
    float theta = (2*M_PI)/(1.0*m_oneRingTri);

    createCapGeometry(&arrayPos, st, theta, 1);
    createCapGeometry(&arrayPos, -st, theta, -1);
    createSideGeometry(&arrayPos, st, theta);

    // Pass all vertices to GL and setup attrs and normals
    passVerticesToGL(sizeof(GLfloat)*6*m_numTriangles);
}

/**
 * @brief Create the side of the cylinder (the infinite cylinder part)
 * @param arrayPos Current position in vertex array
 * @param st Width of ring
 * @param theta Delta for angle with origin
 */
void Cylinder::createSideGeometry(int *arrayPos, const float st, const float theta) {
    float x1,x2,y1,y2,z1,z2,cos1,cos2,sin1,sin2;
    /* SIDES - create sides
     * 1. Create triangles for top level
     * 2. Create rest of triangles
     */
    float segmentHeight = 1/(1.0*m_p1);         // The height of one level of the cone
    float yNorm = 0;

    // 2.
    for (int j=0; j<m_p1; j++) {
        y1 = st-(j*segmentHeight);
        y2 = st-((j+1)*segmentHeight);
        for (int i=0; i<m_oneRingTri; i++) {
            cos1 = cos(-i*theta);
            cos2 = cos(-(i+1)*theta);
            sin1 = sin(-i*theta);
            sin2 = sin(-(i+1)*theta);
            x1 = st*cos1;
            x2 = st*cos2;
            z1 = st*sin1;
            z2 = st*sin2;

            // Vectors to use below
            glm::vec3 v1 = glm::vec3(x1, y1, z1);
            glm::vec3 v2 = glm::vec3(x2, y1, z2);
            glm::vec3 v3 = glm::vec3(x1, y2, z1);
            glm::vec3 v4 = glm::vec3(x2, y2, z2);

            // First tri
            storeVectors(v1, glm::normalize(glm::vec3(v1.x, yNorm, v1.z)), arrayPos);
            storeVectors(v3, glm::normalize(glm::vec3(v3.x, yNorm, v3.z)), arrayPos);
            storeVectors(v2, glm::normalize(glm::vec3(v2.x, yNorm, v2.z)), arrayPos);

            // Second tri
            storeVectors(v2, glm::normalize(glm::vec3(v2.x, yNorm, v2.z)), arrayPos);
            storeVectors(v3, glm::normalize(glm::vec3(v3.x, yNorm, v3.z)), arrayPos);
            storeVectors(v4, glm::normalize(glm::vec3(v4.x, yNorm, v4.z)), arrayPos);
        }
    }
}

/**
 * @brief Simply binds and draws the triangle fan at the top and bottom plus triangles on side
 */
void Cylinder::renderGeometry() {
    // For convenience, have static sizes here
    const int pos1 = m_oneRingTri+2;
    const int pos2 = pos1+NUM_VERTS*m_restRingTri;
    const int pos3 = pos2+m_oneRingTri+2;
    glBindVertexArray(m_vaoID);
    // Draw bottom
    glDrawArrays(GL_TRIANGLE_FAN, 0, m_oneRingTri+2);
    glDrawArrays(GL_TRIANGLES, pos1, NUM_VERTS*m_restRingTri);

    // Draw top + sides
    glDrawArrays(GL_TRIANGLE_FAN, pos2, m_oneRingTri+2);
    glDrawArrays(GL_TRIANGLES, pos3, NUM_VERTS*(m_restRingTri+2*m_p1*m_p2));
    glBindVertexArray(0);
}
