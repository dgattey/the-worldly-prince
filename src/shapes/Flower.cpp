#include "Flower.h"
#include "Shape.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "GLCommon.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/ext.hpp>
#include <iostream>

/**
 * @brief Sets up the properties of a flower
 * This flower is randomized and not dependent on any other flower.
 */
Flower::Flower() {

    // get some randomized rotation from the initial point
    glm::vec3 dims = glm::vec3(rand() % 2, rand() % 2, rand() % 2);
    while (!(dims.x || dims.y || dims.z)) {
        dims = glm::vec3(rand() % 2, rand() % 2, rand() % 2);
    }
    glm::mat4x4 arbitraryRotation = glm::rotate((float)(rand() % 360), dims);

    // lots of hard coded transformations
    float sphereRadius = 0.65f;
    float stemScale = 0.08f;

    cylModel = arbitraryRotation
            * glm::translate(glm::vec3(0.0f, sphereRadius + (stemScale/2), 0.0f))
            * glm::scale(glm::vec3(stemScale/30.f, stemScale, stemScale/30.f))
            * glm::mat4(1.0f);

    centerModel = arbitraryRotation
            * glm::translate(glm::vec3(0.0f, sphereRadius + stemScale, 0.0f))
            * glm::scale(glm::vec3(stemScale/15.f))
            * glm::mat4(1.0f);

    centerColor = glm::vec3(1.f, 0.5f + ((float)rand()) / (2.f * RAND_MAX), 0.f);
    petalCount = rand() % 4 + 5;
    petalModels = new glm::mat4x4[petalCount];
    petalColor = glm::vec3((float)rand() / ((float)RAND_MAX),
                           (float)rand() / ((float)RAND_MAX),
                           (float)rand() / ((float)RAND_MAX));

    // general petal translation
    glm::mat4x4 petalTransform = glm::translate(glm::vec3(0.f, sphereRadius + stemScale, 0.0f)) ;

    // transform each of the pedals radially
    for (int j = 0; j < petalCount; j++) {
        petalModels[j] = arbitraryRotation
                * petalTransform
                * glm::rotate(glm::floor(360.f / (float)petalCount * j), glm::vec3(0.0f, 1.0f, 0.0f))
                * glm::scale(glm::vec3(0.02f / ((float)petalCount), 0.003f, 0.03f))
                * glm::mat4(1.f);
    }
}

/**
 * @brief Sets up the properties of a flower
 * This flower copies its properties from the parameter and has a rotation
 * near to the passed in flower. This is the way that clumping is done.
 * @param around The flower it should cluster around
 */
Flower::Flower(Flower *around) {
    petalCount = around->petalCount;
    float angle = pow(rand() % ((int)pow(360.f, (1.f/2.f))), 2.f);
    glm::vec3 dims = glm::vec3((float)rand() / ((float)RAND_MAX),
                               (float)rand() / ((float)RAND_MAX),
                               (float)rand() / ((float)RAND_MAX));
    glm::mat4x4 arbitraryRotation = glm::rotate(angle, dims);
    cylModel = arbitraryRotation * around->cylModel;
    centerModel = arbitraryRotation * around->centerModel;
    centerColor = around->centerColor;
    petalModels = new glm::mat4x4[petalCount];
    petalColor = around->petalColor;
    for (int j = 0; j < petalCount; j++) {
        petalModels[j] = arbitraryRotation * around->petalModels[j];
    }
}

/**
 * @brief Just delete the petal models
 */
Flower::~Flower() {
    delete[] petalModels;
}

/**
 * @brief Culling based on eye
 * @param cameraEye The camera's eye
 * @return If the flower is currently visible or not
 */
bool Flower::isVisible(glm::vec3 cameraEye) {
    glm::vec4 flowerPos = cylModel * glm::vec4(0.f, 0.f, 0.f, 1.f);
    float dot = glm::dot(glm::normalize(cameraEye), glm::normalize(glm::vec3(flowerPos)));
    if (dot > -0.1) {
        return true;
    }
    return false;
}
