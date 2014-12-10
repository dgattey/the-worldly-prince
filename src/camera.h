#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

#include <QMouseEvent>
//#include "vector.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

/**
    A basic camera
*/
// "near" and "far" used to be pointer modifiers in 16-bit DOS, and
// apparently are still #defined in <windows.h>
// #undef-ing them allows the lab to compile in Windows. Hooray for Windows!
#undef near
#undef far

struct LabCamera {
    glm::vec3 eye, center, up;
    float fovy, near, far;

    float theta, phi;
    float zoom;

    void mouseMove(const glm::vec2 &delta);
    void mouseWheel(float delta);
};

#endif // CAMERA_H
