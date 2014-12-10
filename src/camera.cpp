#include "camera.h"
#include <qgl.h>
#include "newmath.h"

void LabCamera::mouseMove(const glm::vec2 &delta)
{
    // Rotate the eye vector around the origin
    theta += delta.x * 0.01f;
    phi += delta.y * 0.01f;

    // Keep theta in [0, 2pi] and phi in [-pi/2, pi/2]
    theta -= floorf(theta / M_2PI) * M_2PI;
    phi = maxN(0.01f - M_PI / 2, minN(M_PI / 2 - 0.01f, phi));
}

void LabCamera::mouseWheel(float delta)
{
    zoom *= powf(0.999f, delta);
}

///**
//  Called to switch to an orthogonal OpenGL camera.
//  Useful for rending a textured quad across the whole screen.

//  @param width: the viewport width
//  @param height: the viewport height
//**/
//void GLWidget::applyOrthogonalCamera(float width, float height)
//{
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluOrtho2D(0.f, width, 0.f, height);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//}
