#include "Shape.h"

#define MIN_P 1
#define MAX_P 100

/**
 * @brief Empty constructor used by subclasses
 */
Shape::Shape() {
    // For subclasses that need a default constructor
}

/**
 * @brief Constructor for a shape - just initializes the shape and GL
 * @param shader The shader for this shape
 * @param param1 The horizontal tesselation
 * @param param2 The vertical tesselation
 */
Shape::Shape(GLuint shader, int param1, int param2) {
    initShape(shader, param1, param2);
}

/**
 * @brief Makes sure it cleans up GL before finishing
 */
Shape::~Shape() {
    cleanupGL();
}

/**
 * @brief Intersects a ray with a cap (top or bottom by y)
 * @param p The point to start from
 * @param d The direction to move by t
 * @param y The y value for the cap
 * @param part The object intersected
 * @param data An object encapsulating relevant information for casting
 */
void Shape::rayCapIntersectT(glm::vec3 p, glm::vec3 d, float y, ShapePart part, RayData *data) {
    float testT = (y - p.y)/d.y;
    glm::vec3 result = p + testT*d;

    // If the value is positive and less than the current T and makes a valid circle, return it
    bool inBounds = result.x*result.x + result.z*result.z <= RADIUS_SQ;
    if (testT > 0 && testT < data->t && inBounds) {
        data->t = testT;
        data->part = part;
    }
}

/**
 * @brief Gives back the normal for the cap at y
 * @param y The position of the cap vertically
 * @param data An object encapsulating relevant information for casting
 */
void Shape::rayCapIntersectNorm(float y, RayData *data) {
    data->norm = glm::vec4(0, y, 0, 0);
}

/**
 * @brief Computes the two possible t-vals and saves closer ones
 * Works on infinite cone/cylinder
 * @param p The point to start from
 * @param d The direction to move by t
 * @param a A from Pythag
 * @param b B from Pythag
 * @param c C from Pythag
 * @param bound Upper and lower bounds on circle
 * @param part The shape it came from
 * @param data An object encapsulating relevant information for casting
 */
void Shape::rayCircleBoundsCheckT(glm::vec3 p, glm::vec3 d, float a, float b, float c, float bound, ShapePart part, RayData *data) {
    // Discriminant, checking for equality
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
    for (int idx=0; idx<2; idx++) {
        float t = idx==0 ? t1 : t2;
        glm::vec3 res = p + t*d;
        bool inBounds = res.y < bound && res.y > -bound;

        // T is in bounds and valid?
        if (t > 0 && t < data->t && inBounds) {
            data->t = t;
            data->part = part;
        }
    }
}

/**
 * @brief Computes the normal for a ray intersecting with the side of a circle
 * @param eye The position to start from
 * @param dir The direction to move in
 * @param useY If y should be used (for a cone) or not (cylinder)
 * @param data An object encapsulating ray data
 */
void Shape::rayCircleBoundsCheckNorm(glm::vec3 eye, glm::vec3 dir, bool useY, RayData *data) {
    glm::vec3 transformEye = glm::vec3(data->transform*glm::vec4(eye, 1));
    glm::vec3 transformDir = glm::vec3(data->transform*glm::vec4(dir, 0));
    glm::vec3 res = transformEye+data->t*transformDir;

    float y = !useY ? 0 : -0.5*res.y+0.25;

    // If y should be used (cone), include the y term rather than 0 (cylinder)
    data->norm = glm::vec4(2*res.x, y, 2*res.z, 0);
}

/**
 * @brief Figures out a cylindrical texture for the ray and texture data specified
 * @param rayData The ray data
 * @param texData The texture data
 */
void Shape::computeCylindricalTexture(RayData *rayData, TexturePointData *texData) {
    glm::vec4 pt = rayData->intersectTransform;

    if (rayData->part == CONE_CAP || rayData->part == CYLINDER_BOTTOM_CAP || rayData->part == CYLINDER_TOP_CAP) {
        texData->u = pt.x+RADIUS;
        texData->v = pt.z+RADIUS;
        if (rayData->part == CYLINDER_BOTTOM_CAP || rayData->part == CONE_CAP) texData->v *= -1;
        return;
    }

    texData->v = -(pt.y+RADIUS);

    // Figure out where u is
    float theta = atan2(pt.z, pt.x);
    if (theta < 0) texData->u = -theta/(2.0*M_PI);
    else texData->u = 1 - (theta/(2.0*M_PI));
}

/**
 * @brief Save data for the class
 * @param shader The shader GLuint
 * @param param1 The horizontal tesselation parameter
 * @param param2 The vertical tesselation parameter
 */
void Shape::initShape(GLuint shader, int param1, int param2) {
    m_shader = shader;
    m_p1 = param1;
    m_p2 = param2;
    m_vertexData = NULL;

    // Initialize the vao and vbo and create vertex array
    setupGL();
}

/**
 * @brief Bounds the parameters to reasonable values
 */
void Shape::boundParams() {
    // Bounds to 1 and 100 by default
    m_p1 = m_p1 < MIN_P ? MIN_P : m_p1 > MAX_P ? MAX_P : m_p1;
    m_p2 = m_p2 < MIN_P ? MIN_P : m_p2 > MAX_P ? MAX_P : m_p2;
}

/**
 * @brief Deletes vertex data and buffers/arrays
 */
void Shape::cleanupGL() {
    // Delete vertex data
    if (m_vertexData != NULL) {
        delete[] m_vertexData;
        m_vertexData = NULL;
    }
    // Delete ID data
    if (m_vboID != 0) {
        glDeleteBuffers(1, &m_vboID);
        m_vboID = 0;
    }
    if (m_vaoID != 0) {
        glDeleteVertexArrays(1, &m_vaoID);
        m_vaoID = 0;
    }
}

/**
 * @brief Creates VAO and VBO for vertices
 */
void Shape::setupGL() {
    // Initialize the vertex array and buffer
    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);
    glGenBuffers(1, &m_vboID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
}

/**
 * @brief Loads current data for vertices and shader into GL
 * @param bufDataSize The size of the vertex buffer
 */
void Shape::passVerticesToGL(int bufDataSize) {
    // Pass vertex data to OpenGL.
    float stride = sizeof(GLfloat)*6;
    glBufferData(GL_ARRAY_BUFFER, bufDataSize, m_vertexData, GL_STATIC_DRAW);
    glEnableVertexAttribArray(glGetAttribLocation(m_shader, "position"));
    glEnableVertexAttribArray(glGetAttribLocation(m_shader, "normal"));
    glVertexAttribPointer(
        glGetAttribLocation(m_shader, "position"),
        3,                   // Num coordinates per position
        GL_FLOAT,            // Type
        GL_FALSE,            // Normalized
        stride, // Stride
        (void*) 0            // Array buffer offset
    );
    glVertexAttribPointer(
        glGetAttribLocation(m_shader, "normal"),
        3,           // Num coordinates per normal
        GL_FLOAT,    // Type
        GL_TRUE,     // Normalized
        stride,           // Stride
        (void*) (sizeof(GLfloat) * 3)    // Array buffer offset
    );

    // Unbind buffers.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

/**
 * @brief Helper to save vec and norm at the next positions in vertdata
 * @param vec The vector to save
 * @param norm The normal to save
 * @param pos The current index (pointer because passing)
 */
void Shape::storeVectors(glm::vec3 vec, glm::vec3 norm, int* pos) {
    // Deconstruct what was passed in
    m_vertexData[(*pos)++] = vec.x;
    m_vertexData[(*pos)++] = vec.y;
    m_vertexData[(*pos)++] = vec.z;
    m_vertexData[(*pos)++] = norm.x;
    m_vertexData[(*pos)++] = norm.y;
    m_vertexData[(*pos)++] = norm.z;
}

