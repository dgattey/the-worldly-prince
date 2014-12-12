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
Flower::Flower()
{
    glm::vec3 dims = glm::vec3(rand() % 2, rand() % 2, rand() % 2);
    while (!(dims.x || dims.y || dims.z)) {
        dims = glm::vec3(rand() % 2, rand() % 2, rand() % 2);
    }
    glm::mat4x4 arbitraryRotation = glm::rotate((float)(rand() % 360), dims);
    cylModel = arbitraryRotation * glm::translate( glm::vec3(0.0f, -0.72f, 0.0f ) ) * glm::scale(glm::vec3(0.003f, 0.03f, 0.003f)) * glm::mat4(1.0f);
    petalCount = rand() % 4 + 5;
    petals = new Sphere[petalCount];
    petalModels = new glm::mat4x4[petalCount];
    petalColor = glm::vec3(rand() % 150, rand() % 150, rand() % 150);
    std::cout << glm::to_string(petalColor) << std::endl;
    glm::mat4x4 petalTransform = glm::translate(glm::vec3(0.f, -0.735f, 0.0f)) ;
    for (int j = 0; j < petalCount; j++) {
        petalModels[j] = arbitraryRotation * petalTransform * glm::rotate(glm::floor(360.f / (float)petalCount * j), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::vec3(0.02f / ((float)petalCount), 0.003f, 0.03f)) * glm::mat4(1.f);
    }
}

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
