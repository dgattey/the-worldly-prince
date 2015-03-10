#include "Sphere.h"
#include "Util.h"

#define MAX_P 100

/**
 * @brief Sets up sphere to use the given shader and params
 * Creates the geometry too, by way of Shape methods.
 * @param shader The GLuint for the shader
 * @param param1 The resolution horizontally
 * @param param2 The resolution vertically
 */
Sphere::Sphere(GLuint shader, int param1, int param2)
    : Shape(shader, param1, param2) {
    boundParams();
    createGeometry();
}

/**
 * @brief Nothing to delete
 */
Sphere::~Sphere() {}

/**
 * @brief Computes a t-value for intersection given a point, dist, and data
 * @param p The point to start from
 * @param d The direction to move by t
 * @param data An object encapsulting relevant information for casting
 */
void Sphere::computeT(glm::vec3 p, glm::vec3 d, RayData *data) {
    float a = d.x*d.x + d.y*d.y + d.z*d.z;
    float b = 2.0*(p.x*d.x + p.z*d.z + p.y*d.y);
    float c = p.x*p.x + p.z*p.z + p.y*p.y - 0.25;

    // Make sure in bounds
    float disc = b*b - 4.0*a*c;
    if (disc < 0) {
        data->t = INT_MAX;
        return;
    }

    // Compute the two possible t values
    float sqrtd = sqrt(disc);
    float t1 = (-b + sqrtd)/(2.0*a);
    float t2 = (-b - sqrtd)/(2.0*a);

    // Find the minimum one greater than 0
    if (t1 > 0 && t1 < data->t) {
        data->t = t1;
        data->part = SPHERE_P;
    }
    if (t2 > 0 && t2 < data->t) {
        data->t = t2;
        data->part = SPHERE_P;
    }
}

/**
 * @brief Given an eye, dir, and calculated data, gives back the normal
 * Assumes computeT has already been called and the data object has a
 * t value as well as a shape part for the intersection
 * @param eye The eye point of the camera
 * @param dir The direction to move in
 * @param data An object encapsulating relevant information for computation
 */
void Sphere::computeNorm(glm::vec3 eye, glm::vec3 d, RayData *data) {
    glm::vec3 transformEye = glm::vec3(data->transform*glm::vec4(eye, 1));
    glm::vec3 transformDir = glm::vec3(data->transform*glm::vec4(d, 0));
    glm::vec3 res = transformEye+data->t*transformDir;
    data->norm = glm::vec4(res+res, 0);
}

/**
 * @brief Computes a texture for the sphere given rayData and a pointer to tex
 * @param rayData An object with relevant information for texture finding
 * @param texData The location to save the texture data
 */
void Sphere::computeTexture(RayData *rayData, TexturePointData *texData) {
    glm::vec4 pt = rayData->intersectTransform;
    texData->v = 1-(asin(pt.y/RADIUS)/M_PI+RADIUS);

    // THE SINGULARITY!
    if (texData->v == 0 || texData->v == 1) {
        texData->u = RADIUS;
        return;
    }

    // Figure out where u is
    float theta = atan2(pt.z, pt.x);
    if (theta < 0) texData->u = -theta/(2.0*M_PI);
    else texData->u = 1 - (theta/(2.0*M_PI));
}

/**
 * @brief Given new p1 and p2 values, update the geometry
 * @param p1 A new horizontal resolution value
 * @param p2 A new vertical resolution value
 */
void Sphere::updateGeometry(int p1, int p2) {
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
void Sphere::boundParams() {
    m_p1 = m_p1 < 2 ? 2 : m_p1 > MAX_P ? MAX_P : m_p1; // Two rows at least
    m_p2 = m_p2 < 3 ? 3 : m_p2 > MAX_P ? MAX_P : m_p2; // At least 3 sides to "sphere"
}

/**
 * @brief Actually create the triangular geometry and pass all data to GL
 */
void Sphere::createGeometry() {
    float x1,x2,x3,x4,y1,y2,z1,z2,z3,z4,cosTheta1,cosTheta2,sinTheta1,sinTheta2,sinPhi1,sinPhi2;
    int arrayPos = 0;
    const float r = 0.5;

    // Numbers of triangles
    m_numTriangles = NUM_VERTS*(m_p2*2*(m_p1)); // Count of all verts
    m_vertexData = new GLfloat[6*m_numTriangles];

    // Theta, or p2 (horizontal)
    float phi = (2*M_PI)/(1.0*(2*m_p1));
    float theta = (2*M_PI)/(1.0*m_p2);
    for (int i=0; i<m_p2; i++) {
        cosTheta1 = cos(-i*theta);
        cosTheta2 = cos(-(i-1)*theta);
        sinTheta1 = sin(-i*theta);
        sinTheta2 = sin(-(i-1)*theta);

        // Phi, or p1 (vertical)
        for (int j=1; j<=m_p1; j++) {
            sinPhi1 = sin(-j*phi);
            sinPhi2 = sin(-(j-1)*phi);
            y1 = r*cos(-j*phi);
            y2 = r*cos(-(j-1)*phi);
            x1 = r*sinPhi1*cosTheta1;
            x2 = r*sinPhi1*cosTheta2;
            x3 = r*sinPhi2*cosTheta1;
            x4 = r*sinPhi2*cosTheta2;
            z1 = r*sinPhi1*sinTheta1;
            z2 = r*sinPhi1*sinTheta2;
            z3 = r*sinPhi2*sinTheta1;
            z4 = r*sinPhi2*sinTheta2;

            // Vectors to use below
            glm::vec3 v1 = glm::vec3(x1, y1, z1);
            glm::vec3 v2 = glm::vec3(x2, y1, z2);
            glm::vec3 v3 = glm::vec3(x3, y2, z3);
            glm::vec3 v4 = glm::vec3(x4, y2, z4);

            // First tri
            storeVectors(v1, glm::normalize(v1), &arrayPos);
            storeVectors(v3, glm::normalize(v3), &arrayPos);
            storeVectors(v2, glm::normalize(v2), &arrayPos);

            // Second tri
            storeVectors(v2, glm::normalize(v2), &arrayPos);
            storeVectors(v3, glm::normalize(v3), &arrayPos);
            storeVectors(v4, glm::normalize(v4), &arrayPos);
        }
    }

    // Pass all vertices to GL and setup attrs and normals
    passVerticesToGL(sizeof(GLfloat)*6*m_numTriangles);
}

/**
 * @brief Simply binds and draws the triangles
 */
void Sphere::renderGeometry() {
    glBindVertexArray(m_vaoID);
    glDrawArrays(GL_TRIANGLES, 0, m_numTriangles);
    glBindVertexArray(0);
}
