#include "camera.h"
#include <qgl.h>
#include "newmath.h"

void Camera::mouseMove(const glm::vec2 &delta) {
    // Rotate the eye vector around the origin
    theta += delta.x * 0.01f;
    phi += delta.y * 0.01f;

    // Keep theta in [0, 2pi] and phi in [-pi/2, pi/2]
    theta -= floorf(theta / M_2PI) * M_2PI;
    phi = maxN(0.01f - M_PI / 2, minN(M_PI / 2 - 0.01f, phi));
}

void Camera::mouseWheel(float delta) {
    zoom *= powf(0.9995f, delta);
    if (zoom > zoomLimitHigh) zoom = zoomLimitHigh;
    else if (zoom < zoomLimitLow) zoom = zoomLimitLow;
}
