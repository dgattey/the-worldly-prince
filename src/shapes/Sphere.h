#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"

/**
 * @brief Sphere - A Shape
 * A class that creates a sphere by tesselating triangles
 * over the parameters passed into the constructor
 */
class Sphere : public Shape {
public:
    Sphere(GLuint shader, int param1, int param2);
    virtual ~Sphere();

    // Required geometry functions
    void createGeometry();
    void renderGeometry();
    void updateGeometry(int p1, int p2);

    void boundParams();

    void computeT(glm::vec3 p, glm::vec3 d, RayData *data);
    void computeNorm(glm::vec3 eye, glm::vec3 d, RayData *data);
    void computeTexture(RayData *rayData, TexturePointData *texData);
};

#endif // SPHERE_H
