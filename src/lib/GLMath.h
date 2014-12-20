#ifndef NEWMATH_H
#define NEWMATH_H

#include <math.h>
#include <stdlib.h>
#include <iostream>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#define M_2PI (2 * M_PI)

/**
 * @brief Gives a random number scaled in 0 to 1
 * @return A float in [0,1]
 */
inline float frandN() {
    return (float)rand() / (float)RAND_MAX;
}

/**
 * @brief Gives the minimum of two numbers
 * @param a The first float
 * @param b The second float
 * @return The min of a and b
 */
inline float minN(float a, float b) {
    return a < b ? a : b;
}

/**
 * @brief Gives the maximum of two numbers
 * @param a The first float
 * @param b The second float
 * @return The max of a and b
 */
inline float maxN(float a, float b) {
    return a > b ? a : b;
}

/**
 * @brief Gives a vec3 representing the cos and sine angles for a given theta/phi
 * @param theta An angle (radians)
 * @param phi An angle (radians)
 * @return A vec3 of the angles
 */
static glm::vec3 fromAnglesN(float theta, float phi) {
    return glm::vec3(cosf(theta) * cosf(phi), sinf(phi), sinf(theta) * cosf(phi));
}

#endif // NEWMATH_H
