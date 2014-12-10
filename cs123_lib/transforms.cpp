#include "transforms.h"

Transforms::Transforms() : projection(glm::mat4(1.0f)), model(glm::mat4(1.0f)), view(glm::mat4(1.0f))
{
}

glm::mat4 Transforms::getTransform() const{
    return projection * view * model;
}
