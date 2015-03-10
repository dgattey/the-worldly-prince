#include "Transforms.h"

/**
 * @brief Initializes everything to all 1s
 */
Transforms::Transforms() :
    model(glm::mat4(1.0f)),
    view(glm::mat4(1.0f)),
    projection(glm::mat4(1.0f)) {}

/**
 * @brief Gets current transform
 * @return projection * view * model as glm::mat4
 */
glm::mat4 Transforms::getTransform() const{
    return projection * view * model;
}
