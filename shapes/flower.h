#ifndef FLOWER_H
#define FLOWER_H

#include "GL/glew.h"
#include "glm/glm.hpp"
#include "cylinder.h"
#include "sphere.h"

class Flower
{
public:
    Flower();
    Flower(Flower *around);
    virtual ~Flower();

    void init(GLuint position, GLuint normal);

    Cylinder stem;
    glm::mat4x4 cylModel;

    int petalCount;
    Sphere *petals;
    glm::mat4x4 *petalModels;
    glm::vec3 petalColor;

protected:
    bool m_isInitialized;
};

#endif // FLOWER_H
