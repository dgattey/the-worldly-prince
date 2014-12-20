#include "Cube.h"
#include "Util.h"

#define FACES 6
#define NUM_VERTS 3

// Faces
enum faces_t {
    BOTTOM = 5,
    LEFT = 4,
    BACK = 3,
    TOP = 2,
    RIGHT = 1,
    FRONT = 0
};

/**
 * @brief Constructor for a cube
 * @param shader The GLuint representing the shader for this shape
 * @param param1 The tesselation parameter
 */
Cube::Cube(GLuint shader, int param1)
    : Shape(shader, param1, 1) {
    boundParams();
    createGeometry();
}

/**
 * @brief Empty deconstructor
 */
Cube::~Cube() {}

/**
 * @brief Intersects a ray with a face
 * @param p The point
 * @param d The direction
 * @param face Which face it is (see enum above)
 * @param opposite If it should be an opposite (may be same as face or the opposite)
 * @param component The component (x,y,z) of the p and d to use
 * @param part The part of the shape we came from
 * @param data The data object to save to
 */
void Cube::rayFaceIntersect(glm::vec3 p, glm::vec3 d, int face, int opposite, int component, ShapePart part, RayData *data) {
    // Get value for comparing component: negative or positive depending on if it matches face
    float val = face==opposite ? -RADIUS : RADIUS;
    float testT = (val - p[component])/d[component]; // getting a possible t value, using component
    glm::vec3 result = p + testT*d;

    // If the value is positive and less than the current T and makes a valid square, return it
    bool inBounds = true;
    for (int j=0; j<3; j++) {
        if (j==component) continue; // Don't check this result component because we know what the value is
        inBounds &= result[j] <= RADIUS && result[j] >= -RADIUS;
    }

    // If everything worked, change t and norm
    if (testT > 0 && testT < data->t && inBounds) {
        data->t = testT;
        data->part = part;
    }
}

/**
 * @brief Computes a t-value for intersection given a point, dist, and data
 * @param p The point to start from
 * @param d The direction to move by t
 * @param data An object encapsulting relevant information for casting
 */
void Cube::computeT(glm::vec3 p, glm::vec3 d, RayData *data) {
    // Go through each face and compute intersections with each plane
    for (int i=0; i<FACES; i++) {
        switch(i) {
        case FRONT:
            rayFaceIntersect(p,d,i, BACK, 2 /*corresponds to z*/, CUBE_FRONT, data);
            break;
        case BACK:
            rayFaceIntersect(p,d,i, BACK, 2 /*corresponds to z*/, CUBE_BACK, data);
            break;
        case LEFT:
            rayFaceIntersect(p,d,i, LEFT, 0 /*corresponds to x*/, CUBE_LEFT, data);
            break;
        case RIGHT:
            rayFaceIntersect(p,d,i, LEFT, 0 /*corresponds to x*/, CUBE_RIGHT, data);
            break;
        case TOP:
            rayFaceIntersect(p,d,i, BOTTOM, 1 /*corresponds to y*/, CUBE_TOP, data);
            break;
        case BOTTOM:
            rayFaceIntersect(p,d,i, BOTTOM, 1 /*corresponds to y*/, CUBE_BOTTOM, data);
            break;
        default:
            break;
        }
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
void Cube::computeNorm(glm::vec3 eye, glm::vec3 d, RayData *data) {
    data->norm = glm::vec4();
    switch(data->part) {
    case CUBE_FRONT:
        data->norm[2] = 1;
        break;
    case CUBE_BACK:
        data->norm[2] = -1;
        break;
    case CUBE_LEFT:
        data->norm[0] = -1;
        break;
    case CUBE_RIGHT:
        data->norm[0] = 1;
        break;
    case CUBE_TOP:
        data->norm[1] = 1;
        break;
    case CUBE_BOTTOM:
        data->norm[1] = -1;
        break;
    default:
        break;
    }

}

/**
 * @brief Computes a texture for the cube given rayData and a pointer to tex
 * @param rayData An object with relevant information for texture finding
 * @param texData The location to save the texture data
 */
void Cube::computeTexture(RayData *rayData, TexturePointData *texData) {
    glm::vec4 pt = rayData->intersectTransform;
    glm::vec2 twoDPt;
    switch(rayData->part) {
    case CUBE_FRONT:
        twoDPt = glm::vec2(pt.x, -pt.y);
        break;
    case CUBE_BACK:
        twoDPt = glm::vec2(-pt.x, -pt.y);
        break;
    case CUBE_LEFT:
        twoDPt = glm::vec2(pt.z, -pt.y);
        break;
    case CUBE_RIGHT:
        twoDPt = glm::vec2(-pt.z, -pt.y);
        break;
    case CUBE_TOP:
        twoDPt = glm::vec2(pt.x, pt.z);
        break;
    case CUBE_BOTTOM:
        twoDPt = glm::vec2(pt.x, -pt.z);
        break;
    default:
        break;
    }
    texData->u = twoDPt.x + RADIUS;
    texData->v = twoDPt.y + RADIUS;
}

/**
 * @brief For a change in the tesselation parameter, saves changes and creates new geometry
 * @param p1 New value
 * @param p2 Unused
 */
void Cube::updateGeometry(int p1, int p2) {
    m_p1 = p1;
    boundParams();
    cleanupGL();
    setupGL();
    createGeometry();
}

/**
 * @brief Bound the params to scaled values
 */
void Cube::boundParams() {
    Shape::boundParams(); // Use defaults of 1 and 100
}

/**
 * @brief Actually create the geometry based on triangles and tesselation param
 */
void Cube::createGeometry() {

    // Calculate number of items we need
    // Num triangles, mult by num vertices per, mult by 3 for num floats in vector
    int numTriOnFace = (m_p1+1)*2*(m_p1+1);
    int oneFaceNumVertices = NUM_VERTS*6*numTriOnFace;
    m_vertexData = new GLfloat[oneFaceNumVertices*FACES];

    // Current normal (will change from face to face)
    glm::vec3 norm = glm::vec3(0, 0, 1);

    float start = -0.5;
    float z = 0.5;
    float p1float = m_p1*1.0;
    float delta = 1/p1float;
    int arrayPos = 0;

    // Create three faces
    for (int k=0; k<FACES; k++) { //face
        glm::vec3 newNorm = swizzle(norm, k);
        for (int i=0; i<m_p1; i++) { //y
            float y1 = start + i/p1float;
            float y2 = y1+delta;
            for (int j=0; j<m_p1; j++) { //x
                float x1 = start + j/p1float;
                float x2 = x1+delta;

                // Bottom left, right, and upper left
                storeVectors(swizzle(glm::vec3(x1, y1, z), k), newNorm, &arrayPos);
                storeVectors(swizzle(glm::vec3(x2, y1, z), k), newNorm, &arrayPos);
                storeVectors(swizzle(glm::vec3(x1, y2, z), k), newNorm, &arrayPos);

                // Upper left, bottom right, and upper right
                storeVectors(swizzle(glm::vec3(x1, y2, z), k), newNorm, &arrayPos);
                storeVectors(swizzle(glm::vec3(x2, y1, z), k), newNorm, &arrayPos);
                storeVectors(swizzle(glm::vec3(x2, y2, z), k), newNorm, &arrayPos);

            }
        }
    }

    // Pass all vertices to GL and setup attrs and normals
    m_numTriangles = m_p1*m_p1*2*NUM_VERTS*FACES;
    passVerticesToGL(m_numTriangles*sizeof(GLfloat)*6);
}

/**
 * @brief Render the cube by just binding and drawing
 */
void Cube::renderGeometry() {
    glBindVertexArray(m_vaoID);
    glDrawArrays(GL_TRIANGLES, 0, m_numTriangles);
    glBindVertexArray(0);
}

/**
 * @brief Switch x and z or y and x or y and z, etc based on which face it is
 * @param v The input vector to swizzle
 * @param face The face we're looking at
 * @return The v vector, swizzled to be useful in textures, etc.
 */
glm::vec3 Cube::swizzle(glm::vec3 v, int face) {
    // In given v, x and y will change while z stays static
    switch(face) {

    // y, z change, x static
    case LEFT:
        return glm::vec3(-v.z, v.y, v.x);
    case RIGHT:
        return glm::vec3(v.z, v.y, -v.x);

    // x, z change, y static
    case TOP:
        return glm::vec3(v.x, v.z, -v.y);
    case BOTTOM:
        return glm::vec3(v.x, -v.z, v.y);

    // x, y change, z static
    case BACK:
        return glm::vec3(-v.x, v.y, -v.z);
    case FRONT:
    default:
        return v;
    }
}
