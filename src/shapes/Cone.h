#ifndef CONE_H
#define CONE_H

#include "Shape.h"

/**
 * @brief Cone - A Shape
 * A class that creates a cube by tesselating triangles
 * over the parameter passed into the constructor
 */
class Cone : public Shape {
public:
    Cone();
    Cone(GLuint shader, int param1, int param2);
    virtual ~Cone();

    // Required geometry functions
    virtual void createGeometry();
    virtual void renderGeometry();
    virtual void updateGeometry(int p1, int p2);

    virtual void boundParams();

    void computeT(glm::vec3 p, glm::vec3 d, RayData *data);
    void computeNorm(glm::vec3 eye, glm::vec3 dir, RayData *data);
    void computeTexture(RayData *rayData, TexturePointData *texData);

    // Computes all ray circle intersections for the cone itself
    static void rayCircleIntersect(glm::vec3 p, glm::vec3 d, RayData *data);

protected:
    // Helpers for sides and bottom
    virtual void createCapGeometry(int *arrayPos, const float st, const float theta, int dir);
    virtual void createSideGeometry(int *arrayPos, const float st, const float theta);

    // Number of triangles in one ring on the bottom
    int m_oneRingTri;

    // All the rest of the triangles
    int m_restRingTri;
};

#endif // CONE_H
