#ifndef SHAPE_H
#define SHAPE_H

#include "GLCommon.h"
#include "ShapeData.h"

class RayData;


#define RADIUS 0.5
#define RADIUS_SQ RADIUS*RADIUS
#define NUM_VERTS 3

class Shape {
public:
    Shape();
    Shape(GLuint shader, int param1, int param2);
    virtual ~Shape();

    // Renders a given shape (assumes GL is setup with correct vertices)
    virtual void renderGeometry() = 0;

    // Creates vertex array and readies GL for drawing
    virtual void createGeometry() = 0;

    // Recreates vertex array and readies GL because of a change in p1 or p2
    virtual void updateGeometry(int p1, int p2) = 0;

    // Computes the T value for this shape given an eye point and direction
    virtual void computeT(glm::vec3 eye, glm::vec3 d, RayData *data) = 0;
    virtual void computeNorm(glm::vec3 eye, glm::vec3 dir, RayData *data) = 0;

    // Computes the texture for a given point for this shape
    virtual void computeTexture(RayData *rayData, TexturePointData *texData) = 0;

    // Computes texture for cones and cylinders (since it's the same)
    static void computeCylindricalTexture(RayData *rayData, TexturePointData *texData);

    // Intersects a ray with a plane - finding norm and t
    static void rayCapIntersectT(glm::vec3 p, glm::vec3 d, float y, ShapePart part, RayData *data);
    static void rayCapIntersectNorm(float y, RayData *data);

    // Checks bounds for a, b, c values computed for a ray circle intersection - used in cone and cylinder
    static void rayCircleBoundsCheckT(glm::vec3 p, glm::vec3 d, float a, float b, float c, float bound, ShapePart part, RayData *data);
    static void rayCircleBoundsCheckNorm(glm::vec3 eye, glm::vec3 dir, bool useY, RayData *data);

    // Bounds the parameters to sane values - default bounds to 1 and 100
    void boundParams();

protected:
    GLuint m_vaoID;
    GLuint m_vboID;
    GLuint m_shader;

    // Creates the shape (constructor just calls this)
    void initShape(GLuint shader, int param1, int param2);

    // Helper function for storing vectors in vertex data array
    void storeVectors(glm::vec3 vec, glm::vec3 norm, int* pos);

    // Deletes GL data
    void cleanupGL();

    // Sets up GL data (buffers)
    void setupGL();

    // Gives the vertices to GL to create and do stuff with
    void passVerticesToGL(int bufDataSize);

    // Main array used for vertices and normals
    GLfloat* m_vertexData;

    // Current parameters
    int m_p1;
    int m_p2;
    int m_numTriangles; // Total num of triangles on face currently

};

#endif // SHAPE_H
