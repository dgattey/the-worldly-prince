// "near" and "far" used to be pointer modifiers in 16-bit DOS, and
// apparently are still #defined in Windows headers somewhere.
// #undef-ing them allows the lab to compile in Windows.
#undef near
#undef far

#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "glm/glm.hpp"

#define SAFE_DELETE(x) if((x)) { delete (x); (x) = NULL; }
#define MAX(x, y) (x) > (y) ? (x) : (y)
#define MIN(x, y) (x) < (y) ? (x) : (y)
#define PI 3.141592653589f

/**
  * Returns a uniformly distributed random number on the given interval.
  * ex. urand(-1, 1)  would return a random number between -1 and 1.
  */
static inline float urand(float lower = 0.0f, float upper = 1.0f)
{
    return (rand() % 1000) / 1000.0f * (upper - lower) + lower;
}

struct Camera
{
    glm::vec3 eye, center, up;
    float angle, near, far;
};

#endif // COMMON_H
