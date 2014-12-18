#ifndef CAMERA_H
#define CAMERA_H

#include "GLCommon.h"
#include <QMouseEvent>

/**
 * @brief Holds camera data floats and vec3s
 */
struct CameraData {
    glm::vec3 eye, center, up;
    float fovy, near, far;
    float theta, phi;
    float zoom, zoomMax, zoomMin;
};

// Default initializer for camera data
const struct CameraData CAMERA_DATA_INITIALIZER = {
    glm::vec3(), glm::vec3(), glm::vec3(0.0f, 1.0f, 0.0f),
    1.0f, 0.0f, 0.0f,
    0.0f, 0.0f,
    10.0f, 100.0f, 1.0f
};

/**
 * @brief Basic scene camera
 * Supports rotation around center, zooming into point, and that's it
 */
class Camera {
public:
    Camera();
    ~Camera();

    void init(CameraData data);
    void rotateAroundCenter(const glm::vec2 &delta);
    void zoom(float delta);

    CameraData getData();
    void setData(glm::vec3 eye);

private:
    glm::vec3 m_eye, m_center, m_up;
    float m_fovy, m_near, m_far;
    float m_theta, m_phi;
    float m_zoom, m_zoomMax, m_zoomMin;
};

#endif // CAMERA_H
