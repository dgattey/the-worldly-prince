#include "camera.h"
#include "GLMath.h"

/**
 * @brief Constructor that just initializes the camera with default data
 */
Camera::Camera() {
    init(CAMERA_DATA_INITIALIZER);
}

Camera::~Camera() {}

/**
 * @brief Initializes the camera with data
 * @param data The CameraData object that holds relevant information
 */
void Camera::init(CameraData data) {
    m_center = data.center;
    m_up = data.up;
    m_zoom = data.zoom;
    m_zoomMin = data.zoomMin;
    m_zoomMax = data.zoomMax;
    m_theta = data.theta;
    m_phi = data.phi;
    m_fovy = data.fovy;
}

/**
 * @brief Rotates the camera around the current center
 * @param delta The amount to rotate by in x and y
 */
void Camera::rotateAroundCenter(const glm::vec2 &delta) {
    // Rotate the eye vector around the origin
    m_theta += delta.x * 0.01f;
    m_phi += delta.y * 0.01f;

    // Keep theta in [0, 2pi] and phi in [-pi/2, pi/2]
    m_theta -= floorf(m_theta / M_2PI) * M_2PI;
    m_phi = maxN(0.01f - M_PI / 2, minN(M_PI / 2 - 0.01f, m_phi));
}

/**
 * @brief Zooms into the current center of the camera
 * @param delta the amount to zoom (positive/negative)
 */
void Camera::zoom(float delta) {
    m_zoom *= powf(0.9995f, delta);
    if (m_zoom > m_zoomMax) m_zoom = m_zoomMax;
    else if (m_zoom < m_zoomMin) m_zoom = m_zoomMin;
}

/**
 * @brief Gets the current camera data
 * @return A CameraData object for the current camera settings
 */
CameraData Camera::getData() {
    CameraData ret;
    ret.center = m_center;
    ret.eye = m_eye;
    ret.far = m_far;
    ret.fovy = m_fovy;
    ret.near = m_near;
    ret.phi = m_phi;
    ret.theta = m_theta;
    ret.up = m_up;
    ret.zoom = m_zoom;
    ret.zoomMax = m_zoomMax;
    ret.zoomMin = m_zoomMin;
    return ret;
}

/**
 * @brief Sets all data a user can set (currently just the eye)
 * @param eye A new eye point to set for the camera
 */
void Camera::setData(glm::vec3 eye) {
    m_eye = eye;
}

