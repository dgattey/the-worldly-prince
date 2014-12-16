#ifndef TRANSFORMS_H
#define TRANSFORMS_H

#include "CS123Common.h"

class Transforms
{
public:
    Transforms();
    glm::mat4 getTransform() const;

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

#endif // TRANSFORMS_H
