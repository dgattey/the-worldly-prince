#ifndef TRANSFORMS_H
#define TRANSFORMS_H

#include <glm/glm.hpp>

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
