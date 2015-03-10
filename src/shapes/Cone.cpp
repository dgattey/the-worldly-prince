#include "Cone.h"
#include "Util.h"

#define MAX_P 100
#define NUM_VERTS 3

/**
 * @brief Blank constructor
 */
Cone::Cone() {}

/**
 * @brief Sets up cone to use the given shader and params
 * Creates the geometry too, by way of Shape methods.
 * @param shader The GLuint for the shader
 * @param param1 The resolution horizontally
 * @param param2 The resolution vertically
 */
Cone::Cone(GLuint shader, int param1, int param2)
    : Shape(shader, param1, param2) {
    boundParams();
    createGeometry();
}

/**
 * @brief Nothing to delete
 */
Cone::~Cone() {}

/**
 * @brief Computes a t-value for intersection given a point, dist, and data
 * @param p The point to start from
 * @param d The direction to move by t
 * @param data An object encapsulting relevant information for casting
 */
void Cone::computeT(glm::vec3 p, glm::vec3 d, RayData *data) {
    Cone::rayCircleIntersect(p, d, data); // Infinite cone
    Shape::rayCapIntersectT(p, d, -RADIUS, CONE_CAP, data); // Bottom cap
}

/**
 * @brief Given an eye, dir, and calculated data, gives back the normal
 * Assumes computeT has already been called and the data object has a
 * t value as well as a shape part for the intersection
 * @param eye The eye point of the camera
 * @param dir The direction to move in
 * @param data An object encapsulating relevant information for computation
 */
void Cone::computeNorm(glm::vec3 eye, glm::vec3 dir, RayData *data) {
    switch(data->part) {
    case CONE_CAP:
        Shape::rayCapIntersectNorm(-RADIUS, data);
        break;
    case CONE_SIDE:
        Shape::rayCircleBoundsCheckNorm(eye, dir, true, data);
        break;
    default:
        break;
    }
}

/**
 * @brief Computes a texture for the cone given rayData and a pointer to tex
 * @param rayData An object with relevant information for texture finding
 * @param texData The location to save the texture data
 */
void Cone::computeTexture(RayData *rayData, TexturePointData *texData) {
    computeCylindricalTexture(rayData, texData);
}

/**
 * @brief Intersect rays with the sides of an infinite cone (and bounds)
 * @param p The point to start from
 * @param d The direction to move in
 * @param data An object with information used to help compute the intersection
 */
void Cone::rayCircleIntersect(glm::vec3 p, glm::vec3 d, RayData *data) {
    float a = d.x*d.x + d.z*d.z - 0.25*d.y*d.y;
    float b = 2.0*p.x*d.x + 2.0*p.z*d.z - 0.5*p.y*d.y + 0.25*d.y;
    float c = p.x*p.x + p.z*p.z - 0.25*p.y*p.y + 0.25*p.y - 0.0625;

    // Make sure in bounds
    Shape::rayCircleBoundsCheckT(p, d, a, b, c, RADIUS, CONE_SIDE, data);
}

/**
 * @brief Given new p1 and p2 values, update the geometry
 * @param p1 A new horizontal resolution value
 * @param p2 A new vertical resolution value
 */
void Cone::updateGeometry(int p1, int p2) {
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
void Cone::boundParams() {
    m_p1 = m_p1 < 1 ? 1 : m_p1 > MAX_P ? MAX_P : m_p1; // Top can just be one round of triangles
    m_p2 = m_p2 < 3 ? 3 : m_p2 > MAX_P ? MAX_P : m_p2; // At least 3 sides to cone
}

/**
 * @brief Actually create the triangular geometry and pass all data to GL
 */
void Cone::createGeometry() {
    int arrayPos = 0;
    const float st = 0.5;

    // Numbers of triangles
    m_oneRingTri = m_p2;                        // How many in first ring
    m_restRingTri = 2*m_oneRingTri*(m_p1-1);    // Rest of rings have this many in total
    m_numTriangles = ((m_oneRingTri+2)+NUM_VERTS*(m_oneRingTri+m_restRingTri+2*(m_p1-1)*m_p2)); // Count of all verts
    m_vertexData = new GLfloat[6*m_numTriangles];

    // Delta for angle
    float theta = (2*M_PI)/(1.0*m_oneRingTri);

    createCapGeometry(&arrayPos, st, theta, 1);
    createSideGeometry(&arrayPos, st, theta);

    // Pass all vertices to GL and setup attrs and normals
    passVerticesToGL(sizeof(GLfloat)*6*m_numTriangles);
}

/**
 * @brief Create the bottom of the cone (the cap)
 * @param arrayPos Current position in vertex array
 * @param st Width of ring
 * @param theta Delta for angle with origin
 * @param dir The direction to move in
 */
void Cone::createCapGeometry(int *arrayPos, const float st, const float theta, int dir) {
    float x1,x2,x3,x4,y,z1,z2,z3,z4,cos1,cos2,sin1,sin2;

    /* BOTTOM - Create bottom of cone
     * 1. Store center point and first ring triangle fan
     * 2. Create basic triangles around rest of rings
     */
    glm::vec3 norm = glm::vec3(0, -2*st, 0);    // Current normal
    float ringW = st/(1.0*m_p1);                // Width of one bottom ring
    storeVectors(glm::vec3(0,-st,0), norm, arrayPos);

    // Change in theta to use if the direction is negative when p2 is odd
    float delta = dir > 0 || m_p2%2==0 ? 0 : theta/2.0;

    // Iterates around first ring
    for (int i=0; i<=m_oneRingTri; i++) {
        x1 = ringW*cos(dir*i*theta+delta);
        y = -st;
        z1 = ringW*sin(dir*i*theta+delta);
        storeVectors(glm::vec3(x1, y, z1), norm, arrayPos);
    }

    // Rest of rings - loops over rings and for each, around circle
    for (int j=1; j<=(m_p1-1); j++) {
        float crw = ringW*j;        // Current ring width
        float drw = ringW*(j+1);    // Width of outer points

        // Go around unit circle
        for (int i=0; i<m_oneRingTri; i++) {
            cos1 = cos(dir*i*theta+delta);
            cos2 = cos(dir*(i+1)*theta+delta);
            sin1 = sin(dir*i*theta+delta);
            sin2 = sin(dir*(i+1)*theta+delta);

            // All possible points for this step
            x1 = crw*cos1;
            x2 = crw*cos2;
            x3 = drw*cos1;
            x4 = drw*cos2;
            y = -st;
            z1 = crw*sin1;
            z2 = crw*sin2;
            z3 = drw*sin1;
            z4 = drw*sin2;

            // Tri 1
            storeVectors(glm::vec3(x1, y, z1), norm, arrayPos);
            storeVectors(glm::vec3(x3, y, z3), norm, arrayPos);
            storeVectors(glm::vec3(x4, y, z4), norm, arrayPos);

            // Tri 2
            storeVectors(glm::vec3(x1, y, z1), norm, arrayPos);
            storeVectors(glm::vec3(x4, y, z4), norm, arrayPos);
            storeVectors(glm::vec3(x2, y, z2), norm, arrayPos);
        }
    }
}

/**
 * @brief Create the side of the cone (the infinite cone part)
 * @param arrayPos Current position in vertex array
 * @param st Width of ring
 * @param theta Delta for angle with origin
 */
void Cone::createSideGeometry(int *arrayPos, const float st, const float theta) {
    float x1,x2,x3,x4,y1, y2,z1,z2,z3,z4,cos1,cos2,sin1,sin2, nextScalar;
    /* SIDES - create sides
     * 1. Create triangles for top level
     * 2. Create rest of triangles
     */
    glm::vec3 conicPt = glm::vec3(0, st, 0);    // Point at top of cone
    float segmentHeight = 1/(1.0*m_p1);         // The height of one level of the cone
    float circScalar = st*segmentHeight/tan(M_PI/4.0);  // The scalar for all points x and z
    float yNorm = st*cos(M_PI/3.0)/(1.0*m_p1);        // Altitude of tri made by conic section by Pythag

    // 1.
    for (int i=0; i<m_oneRingTri; i++) {
        x1 = circScalar*cos(-i*theta);
        x2 = circScalar*cos(-(i+1)*theta);
        y1 = st-segmentHeight;                   // Just top segment y val
        z1 = circScalar*sin(-i*theta);
        z2 = circScalar*sin(-(i+1)*theta);

        // Vectors to use below
        glm::vec3 v1 = glm::vec3(x1, y1, z1);
        glm::vec3 v2 = glm::vec3(x2, y1, z2);
        glm::vec3 v3 = glm::normalize(glm::vec3((x1+x2)/2, yNorm, (z1+z2)/2));

        // Save points
        storeVectors(conicPt, v3, arrayPos);
        storeVectors(v1, glm::normalize(glm::vec3(v1.x, yNorm, v1.z)), arrayPos);
        storeVectors(v2, glm::normalize(glm::vec3(v2.x, yNorm, v2.z)), arrayPos);
    }

    // 2.
    for (int j=1; j<m_p1; j++) {
        circScalar = st*j*segmentHeight/tan(M_PI/4.0);
        nextScalar = st*(j+1)*segmentHeight/tan(M_PI/4.0);
        y1 = st-(j*segmentHeight);
        y2 = st-((j+1)*segmentHeight);
        for (int i=0; i<m_oneRingTri; i++) {
            cos1 = cos(-i*theta);
            cos2 = cos(-(i+1)*theta);
            sin1 = sin(-i*theta);
            sin2 = sin(-(i+1)*theta);
            x1 = circScalar*cos1;
            x2 = circScalar*cos2;
            x3 = nextScalar*cos1;
            x4 = nextScalar*cos2;
            z1 = circScalar*sin1;
            z2 = circScalar*sin2;
            z3 = nextScalar*sin1;
            z4 = nextScalar*sin2;

            // Vectors to use below
            glm::vec3 v1 = glm::vec3(x1, y1, z1);
            glm::vec3 v2 = glm::vec3(x2, y1, z2);
            glm::vec3 v3 = glm::vec3(x3, y2, z3);
            glm::vec3 v4 = glm::vec3(x4, y2, z4);

            // First tri
            storeVectors(v1, glm::normalize(glm::vec3(v1.x, yNorm*j, v1.z)), arrayPos);
            storeVectors(v3, glm::normalize(glm::vec3(v3.x, yNorm*(j+1), v3.z)), arrayPos);
            storeVectors(v2, glm::normalize(glm::vec3(v2.x, yNorm*j, v2.z)), arrayPos);

            // Second tri
            storeVectors(v2, glm::normalize(glm::vec3(v2.x, yNorm*j, v2.z)), arrayPos);
            storeVectors(v3, glm::normalize(glm::vec3(v3.x, yNorm*(j+1), v3.z)), arrayPos);
            storeVectors(v4, glm::normalize(glm::vec3(v4.x, yNorm*(j+1), v4.z)), arrayPos);
        }
    }
}

/**
 * @brief Simply binds and draws the triangle fan at the bottom plus triangles on side
 */
void Cone::renderGeometry() {
    glBindVertexArray(m_vaoID);
    glDrawArrays(GL_TRIANGLE_FAN, 0, m_oneRingTri+2);
    glDrawArrays(GL_TRIANGLES, m_oneRingTri+2, NUM_VERTS*(m_restRingTri+m_oneRingTri+2*(m_p1-1)*m_p2));
    glBindVertexArray(0);
}
