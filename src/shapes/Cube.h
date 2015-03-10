#ifndef CUBE_H
#define CUBE_H

#include "Shape.h"
#include "Util.h"

/**
 * @brief Cube - A Shape
 * A class that creates a cube by tesselating triangles
 * over the parameter passed into the constructor
 */
class Cube : public Shape {
public:
    Cube(GLuint shader, int param1);
    virtual ~Cube();

    // Required geometry functions
    void createGeometry();
    void renderGeometry();
    void updateGeometry(int p1, int p2);

    void boundParams();

    void computeT(glm::vec3 p, glm::vec3 d, RayData *data);
    void computeNorm(glm::vec3 eye, glm::vec3 d, RayData *data);
    void computeTexture(RayData *rayData, TexturePointData *texData);

    // Computes the t value for a specified face based on the parameters given - explained in code
    static void rayFaceIntersect(glm::vec3 p, glm::vec3 d, int face, int opposite, int component, ShapePart part, RayData *data);

private:
    // Helper to switch vals of vec based on face
    glm::vec3 swizzle(glm::vec3 v, int face);
};

#endif // CUBE_H
