#ifndef GLWIDGET_H
#define GLWIDGET_H
#include "GLCommon.h"
#include <QGLWidget>
#include <QTimer>
#include "Camera.h"
#include "Transforms.h"
#include "TexturedQuad.h"

#include "PlanetsRenderer.h"
#include "FlowersRenderer.h"
#include "StarsRenderer.h"

#define NUM_LIGHTS 4

// Forward declared classes
class QGLFramebufferObject;
class StarsRenderer;
class PlanetsRenderer;
class FlowersRenderer;

/**
 * @brief The GLRenderWidget class
 * Renders all objects to screen and initializes OpenGL as needed.
 * Draws stars as particles in background, three planets on top, and
 * flowers on the moon (gray planet). Uses two FBOS, one without
 * depth for stars, and another with depth for flowers + planets.
 * Each of the *Renderers has matching methods to be called from
 * this class to draw into the correct textures
 */
class GLRenderWidget : public QGLWidget {
Q_OBJECT
public:
    GLRenderWidget(QGLFormat format, QWidget *parent = 0);
    ~GLRenderWidget();

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

    // Scene variables
    Camera m_camera; // Camera of scene
    Transforms m_transform; // Current scene transform
    TexturedQuad m_texquad; // Global texQuad used when drawing to screen
    glm::vec2 m_prevMousePos; // Mouse pos before
    bool m_isOrbiting; // If paused or not

    // Renderers
    GLuint m_shaderTex;
    StarsRenderer *m_stars; // Renders all stars
    PlanetsRenderer *m_planets; // Renders all planets
    FlowersRenderer *m_flowers; // Renders all flowers

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
};

#endif // GLWIDGET_H
