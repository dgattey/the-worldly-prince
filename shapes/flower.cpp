#include "flower.h"
#include "shape.h"
#include "sphere.h"
#include "cylinder.h"
#include "common.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/ext.hpp>
#include "gtc/type_ptr.hpp"
#include <iostream>
/* 
 * Sets up the properties of a flower
 * Note that this does not actually initialize the subshapes since it wasn't
 * working when I tried that for some reason.
 *
 * This flower is randomized and not dependent on any other flower.
 */
Flower::Flower()
{

    // get some randomized rotation from the initial point
    glm::vec3 dims = glm::vec3(rand() % 2, rand() % 2, rand() % 2);
    while (!(dims.x || dims.y || dims.z)) {
        dims = glm::vec3(rand() % 2, rand() % 2, rand() % 2);
    }
    glm::mat4x4 arbitraryRotation = glm::rotate((float)(rand() % 360), dims);

    // lots of hard coded transformations
    cylModel = arbitraryRotation * glm::translate( glm::vec3(0.0f, -0.72f, 0.0f ) ) * glm::scale(glm::vec3(0.003f, 0.03f, 0.003f)) * glm::mat4(1.0f);
    petalCount = rand() % 4 + 5;
    petals = new Sphere[petalCount];
    petalModels = new glm::mat4x4[petalCount];
    petalColor = glm::vec3(rand() % 150, rand() % 150, rand() % 150);

    // general petal translation
    glm::mat4x4 petalTransform = glm::translate(glm::vec3(0.f, -0.735f, 0.0f)) ;

    // transform each of the pedals radially
    for (int j = 0; j < petalCount; j++) {
        petalModels[j] = arbitraryRotation * petalTransform * glm::rotate(glm::floor(360.f / (float)petalCount * j), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::vec3(0.02f / ((float)petalCount), 0.003f, 0.03f)) * glm::mat4(1.f);
    }
}

/* 
 * Sets up the properties of a flower based on another flower
 * Note that this also does not actually initialize the subshapes since 
 * it wasn't working when I tried that for some reason.
 *
 * This flower copies its properties from the parameter and has a rotation
 * near to the passed in flower. This is the way that clumping is done.
 */
Flower::Flower(Flower *around)
{
    petalCount = around->petalCount;
    float angle = rand() % 50;
    glm::vec3 dims =  glm::vec3(((float) rand() / (RAND_MAX)), ((float) rand() / (RAND_MAX)), ((float) rand() / (RAND_MAX)));
    glm::mat4x4 arbitraryRotation = glm::rotate(angle, dims);
    cylModel = arbitraryRotation * around->cylModel;
    petals = new Sphere[petalCount];
    petalModels = new glm::mat4x4[petalCount];
    petalColor = around->petalColor;
    for (int j = 0; j < petalCount; j++) {
        petalModels[j] = arbitraryRotation * around->petalModels[j];
    }
}

Flower::~Flower()
{
    delete[] petals;
    delete[] petalModels;
}

bool Flower::isVisible(glm::vec3 cameraEye) {
    glm::vec4 flowerPos = cylModel * glm::vec4(0.f, 0.f, 0.f, 1.f);
    float dot = glm::dot(glm::normalize(cameraEye), glm::normalize(glm::vec3(flowerPos)));
    if (dot > 0) {
        return true;
    }
    return false;
}
