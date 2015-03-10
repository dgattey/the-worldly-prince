#ifndef CYLINDER_H
#define CYLINDER_H

#include "Cone.h"

/**
 * @brief Cylinder - A Shape
 * A class that creates a cylinder by tesselating triangles
 * over the parameter passed into the constructor.
 * Inherits from the cone since it does very similar things
 */
class Cylinder : public Cone {
public:
    Cylinder(GLuint shader, int param1, int param2);
    virtual ~Cylinder();

    // Required geometry functions
    void createGeometry();
    void renderGeometry();
    void updateGeometry(int p1, int p2);

    void boundParams();

    void computeT(glm::vec3 p, glm::vec3 d, RayData *data);
    void computeNorm(glm::vec3 eye, glm::vec3 dir, RayData *data);
    void computeTexture(RayData *data, TexturePointData *texData);

    // Computes all ray circle intersections for the cylinder itself
    static void rayCircleIntersect(glm::vec3 p, glm::vec3 d, RayData *data);

    // Helper to create sides
    void createSideGeometry(int *arrayPos, const float st, const float theta);

};

#endif // CYLINDER_H
