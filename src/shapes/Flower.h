#ifndef FLOWER_H
#define FLOWER_H

#include "GLCommon.h"
#include "Cylinder.h"
#include "Sphere.h"

#define STEMCOLOR glm::vec3(0, 0.5, 0)

class Flower {
public:
    Flower();

    // copy constructor-ish
    Flower(Flower *around);
    virtual ~Flower();
    bool isVisible(glm::vec3 cameraEye);

    glm::mat4x4 cylModel;

    glm::mat4x4 centerModel;
    glm::vec3 centerColor;

    int petalCount;
    glm::mat4x4 *petalModels;
    glm::vec3 petalColor;

protected:
    bool m_isInitialized;
};

#endif // FLOWER_H
