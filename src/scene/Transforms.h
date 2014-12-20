#ifndef TRANSFORMS_H
#define TRANSFORMS_H

#include "GLCommon.h"

/**
 * @brief Class that supports model, view, projection transforms
 * Ability to get any of them, or the overall transform - like a struct
 */
class Transforms {
public:
    Transforms();
    glm::mat4 getTransform() const;

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

#endif // TRANSFORMS_H
