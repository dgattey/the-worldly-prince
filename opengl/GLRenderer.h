#ifndef GLWIDGET_H
#define GLWIDGET_H
#include "CS123Common.h"
#include <QGLWidget>
#include <QTimer>
#include "camera.h"
#include "transforms.h"
#include "texquad.h"

#include "PlanetsRenderer.h"
#include "FlowersRenderer.h"
#include "StarsRenderer.h"

#include "flower.h"
#include "cylinder.h"
#include "sphere.h"

#define NUM_LIGHTS 4

// Forward declared classes
class QGLFramebufferObject;
class StarsRenderer;
class PlanetsRenderer;
class FlowersRenderer;

/**
 * @brief The GLRenderer class
 * Renders all objects to screen and initializes OpenGL as needed.
 * Draws stars as particles in background, three planets on top, and
 * flowers on the moon (gray planet). Uses two FBOS, one without
 * depth for stars, and another with depth for flowers + planets.
 * Each of the *Renderers has matching methods to be called from
 * this class to draw into the correct textures
 */
class GLRenderer : public QGLWidget {
Q_OBJECT
public:
    GLRenderer(QGLFormat format, QWidget *parent = 0);
    ~GLRenderer();

    // Set up an FBO the right way
    static void createFBO(GLuint *fbo, GLuint *colorAttach, int texId, glm::vec2 size, bool depth);

    // Getters for other renderers
    Camera getCamera();
    Transforms getTransformation();
    float getSimulationSpeed();
    float getRotationalSpeed();
    bool getPaused();

protected:
    // Inheirited methods
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    // Updates camera based on current width/height
    void updateCamera();

protected slots:
    void tick();

private:
    // Recreates all data, doesn't move camera or reset time
    void refresh();

    // OpenGL creation, rendering
    void createShaderPrograms();
    void createFramebufferObjects(glm::vec2 size);
    void renderTexturedQuad();
    void renderFinalPass();

    // Prints FPS to console
    void printFPS();

    // TODO: MOVE TO OWN CLASS
    void renderFlowers(glm::mat4x4 localizedOrbit);
    void generateFlowers();
    void renderFlowersPass(glm::mat4x4 localizedOrbit);
    std::list<Flower *> m_flowers;
    Sphere *m_flowerSphere;
    Cylinder *m_flowerCylinder;

    // Scene variables
    Camera m_camera; // Camera of scene
    Transforms m_transform; // Current scene transform
    TexQuad m_texquad; // Global texQuad used when drawing to screen
    glm::vec2 m_prevMousePos; // Mouse pos before
    bool m_isOrbiting; // If paused or not

    // Other renderers
    StarsRenderer *m_stars; // Renders all stars
    PlanetsRenderer *m_planets; // Renders all planets

    // Time
    QTimer m_timer;
    float m_lastTime;
    float m_elapsedTime;
    float m_fps;
    float m_increment;
    int m_lastUpdate;
    int m_numFrames;
    float m_currentFPS;
    float m_timeMultiplier; // Global speed multiplier
    float m_rotationalSpeed; // Current rotational speed

    // Resources
    QHash<QString, GLuint> m_shaderPrograms; // hash table for easy access to shaders

    // Lights
    glm::vec3 m_lightPositions[NUM_LIGHTS];
    glm::vec3 m_lightIntensities[NUM_LIGHTS];
    float m_k_a;
    float m_k_d;
    glm::vec3 m_O_a;
    glm::vec3 m_O_d;
    glm::vec3 m_i_a;
};

#endif // GLWIDGET_H
