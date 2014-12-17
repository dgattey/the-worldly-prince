#ifndef CAMERA_H
#define CAMERA_H

#include "CS123Common.h"
#include <QMouseEvent>

/**
    A basic camera
*/
// "near" and "far" used to be pointer modifiers in 16-bit DOS, and
// apparently are still #defined in <windows.h>
// #undef-ing them allows the project to compile in Windows. Hooray for Windows!
#undef near
#undef far

struct Camera {
    glm::vec3 eye, center, up;
    float fovy, near, far;

    float theta, phi;
    float zoom;
    float zoomLimitHigh;
    float zoomLimitLow;

    void mouseMove(const glm::vec2 &delta);
    void mouseWheel(float delta);
};

#endif // CAMERA_H
