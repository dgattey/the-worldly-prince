#ifndef NEWMATH_H
#define NEWMATH_H

#include <math.h>
#include <stdlib.h>
#include <iostream>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#define M_2PI (2 * M_PI)

inline float frandN() { return (float)rand() / (float)RAND_MAX; }
inline float minN(float a, float b) { return a < b ? a : b; }
inline float maxN(float a, float b) { return a > b ? a : b; }

static glm::vec3 fromAnglesN(float theta, float phi) { return glm::vec3(cosf(theta) * cosf(phi), sinf(phi), sinf(theta) * cosf(phi)); }

#endif // NEWMATH_H
