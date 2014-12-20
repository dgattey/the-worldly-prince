#include "GLCommon.h"
#include "GLRenderWidget.h"
#include "Settings.h"
#include "GLMath.h"
#include "ResourceLoader.h"

#include <iostream>
#include <QFileDialog>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTime>
#include <QDebug>

#define MAXMULT 100.0f
#define MINMULT 0.1f

/**
 * @brief Sets up the widget for use
 * Creates a timer that ticks at 60 FPS to draw. Creates renderers
 * for all objects in the scene (stars, planets, and flowers). Also
 * sets values like mouse tracking that we need.
 * @param format
 * @param parent
 */
GLRenderWidget::GLRenderWidget(QGLFormat format, QWidget *parent)
    : QGLWidget(format, parent), m_timer(this), m_fps(60.0f), m_increment(0) {
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    // Set up 60 FPS draw loop
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));

    // Start the timer for updating the screen
    m_lastUpdate = QTime(0,0).msecsTo(QTime::currentTime());
    m_numFrames = 0;
    m_timeMultiplier = 1.0f; // Standard speed
    m_isOrbiting = true; // Rotates the scene
    m_timer.start(1000.0f / m_fps);
}

/**
 * @brief Deletes the other renderers
 */
GLRenderWidget::~GLRenderWidget() {
    delete m_stars;
    delete m_planets;
    delete m_flowers;
}

/**
 * @brief Creates a GLEW instance, all data, the camera, and relevant GL info/calls
 */
void GLRenderWidget::initializeGL() {
    // Set up OpenGL
    glewExperimental = GL_TRUE;
    fprintf(stdout, "Using OpenGL Version %s\n", glGetString(GL_VERSION));
    GLuint err = glewInit();
    if (err != GLEW_OK) {
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    // Set up renderers
    m_stars = new StarsRenderer(this);
    m_planets = new PlanetsRenderer(this);
    m_flowers  = new FlowersRenderer(m_planets, this);

    // Set up the shader programs and FBOs
    createShaderPrograms();
    createFramebufferObjects(glm::vec2(width(), height()));

    // Set up the time for orbit
    m_lastTime = QTime(0,0).msecsTo(QTime::currentTime());

    // Create data
    refresh();

    // Occlusion based on depth, back-face culling, black when cleared
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Set up camera
    CameraData data = CAMERA_DATA_INITIALIZER;
    data.zoom = M_PI * 2.0f;
    data.zoomMin = 1.5f;
    data.zoomMax = 300.0f;
    data.theta = M_PI * 1.5f;
    data.fovy = M_PI * 0.25f;
    data.near = 0.1f;
    data.far = 1000.0f;
    m_camera.init(data);
    updateCamera(); // sets eye
}

/**
 * @brief Refreshes all renders
 */
void GLRenderWidget::refresh() {
    m_stars->refresh();
    m_planets->refresh();
    m_flowers->refresh();
}

/**
 * @brief Creates all renderers' shader programs plus the composition one
 */
void GLRenderWidget::createShaderPrograms() {
    fprintf(stdout, "\nCompiling all shaders...\n");
    m_stars->createShaderProgram();
    m_planets->createShaderProgram();
    m_flowers->createShaderProgram();

    m_shaderTex = ResourceLoader::loadShaders(":/shaders/tex.vert", ":/shaders/tex.frag");
    m_texquad.init(glGetAttribLocation(m_shaderTex, "position"),
                   glGetAttribLocation(m_shaderTex, "texCoords"));
    fprintf(stdout, "\n");
}

/**
 * Allocates framebuffer objects for all renderers
 * @param width The viewport width
 * @param height The viewport height
 **/
void GLRenderWidget::createFramebufferObjects(glm::vec2 size) {
    m_stars->createFBO(size);
    m_planets->createFBO(size);
    m_flowers->createFBO(size);

    // Clear
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief Allows a subclass or renderer to create an FBO based on passed in data
 * @param fbo The pointer to create the FBO at
 * @param colorAttach The pointer to create the color attachment at
 * @param texID The ID of the texture, 0 to GL_TEXTURE_MAX_AMOUNT
 * @param size The size of the FBO
 * @param depth If depth should also be generated
 */
void GLRenderWidget::createFBO(GLuint *fbo, GLuint *colorAttach, int texID, glm::vec2 size, bool depth) {
    int width = size.x;
    int height = size.y;

    glGenFramebuffers(1, fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, *fbo);
    glActiveTexture(GL_TEXTURE0+texID); // Texture 1 is for planet
    glGenTextures(1, colorAttach);
    glBindTexture(GL_TEXTURE_2D, *colorAttach);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *colorAttach, 0);

    // Setup depth if necessary
    if (!depth) return;
    GLuint depthI;
    glGenRenderbuffers(1, &depthI);
    glBindRenderbuffer(GL_RENDERBUFFER, depthI);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthI);
}

/**
 * @brief Assumes rendering of prepasses and blends together renders
 * Uses output of other renderers to textures to pass those textures to
 * a shader to blend together as a final output
 */
void GLRenderWidget::renderFinalPass() {
    // Draw to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Get shader ready
    int starID = m_stars->getTextureID();
    int planetID = m_planets->getTextureID();
    glUseProgram(m_shaderTex);
    glUniform1i(glGetUniformLocation(m_shaderTex, "starTex"), starID);
    glUniform1i(glGetUniformLocation(m_shaderTex, "planetTex"), planetID);

    // Bind to the rendered stars texture
    glActiveTexture(GL_TEXTURE0+starID);
    glBindTexture(GL_TEXTURE_2D, *m_stars->getColorAttach());

    // Bind to the rendered planet + flowers texture
    glActiveTexture(GL_TEXTURE0+planetID);
    glBindTexture(GL_TEXTURE_2D, *m_planets->getColorAttach());

    // Draw
    renderTexturedQuad();

    // Clear
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief Updates the FPS/time and camera placement, and renders all renderers
 */
void GLRenderWidget::paintGL() {
    // Get the time in seconds
    m_numFrames++;
    int time = QTime(0,0).msecsTo(QTime::currentTime());

    if (m_isOrbiting) {
        m_elapsedTime += m_timeMultiplier*(time - m_lastTime);
        m_lastTime = time;
    }

    if (time - m_lastUpdate > 1000) {
        m_currentFPS = m_numFrames / (float)((time - m_lastUpdate)/1000.f);
        m_numFrames = 0;
        m_lastUpdate = time;
    }

    m_rotationalSpeed = m_elapsedTime/((M_PI)*m_fps);

    m_stars->render();
    m_planets->render();
    m_flowers->render();
    renderFinalPass();

    printFPS();

    updateCamera();
}

/**
 * Draws a textured quad. The texture must be bound and unbound
 * before and after calling this method - this method assumes that the texture
 * has been bound beforehand using glBindTexture.
 *
 * @param w The width of the quad to draw
 * @param h The height of the quad to draw
 **/
void GLRenderWidget::renderTexturedQuad() {
    // Clamp value to edge of texture when texture index is out of bounds
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    m_texquad.draw();
}

/**
 * @brief Resizes all buffers and viewport itself
 * The camera is updated when the screen resizes because the aspect ratio may change.
 * @param width The new width
 * @param height The new height
 */
void GLRenderWidget::resizeGL(int width, int height) {
    // Set the viewport to fill the screen
    glViewport(0, 0, width, height);

    // Update the camera
    updateCamera();

    // Resize all used textures
    createFramebufferObjects(glm::vec2(width, height));
}

/**
 * @brief Based on width and height, sets eye of camera to correct vector
 * Also sets the projection of view of the scene based on the data
 */
void GLRenderWidget::updateCamera() {
    float w = width();
    float h = height();
    CameraData cData = m_camera.getData();

    float ratio = 1.0f * w / h;
    glm::vec3 dir(-fromAnglesN(cData.theta, cData.phi));
    glm::vec3 eye(cData.center - dir * cData.zoom);

    m_transform.projection = glm::perspective(cData.fovy, ratio, cData.near, cData.far);
    m_transform.view = glm::lookAt(eye, cData.center, cData.up);

    m_camera.setData(eye);
}

/**
 * @brief When ticking, simply update the onscreen objects
 */
void GLRenderWidget::tick() {
    update();
}

/**
 * @brief Based on keypress, does something special
 * R will refresh, right arrow makes time faster, left makes time slower,
 * and space pauses time. H should eventually hide/show text
 * @param event The keypress event
 */
void GLRenderWidget::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
    case Qt::Key_R: {
        refresh();
        break;
    } case Qt::Key_Right: {
        m_timeMultiplier *= 1.1f;
        if (m_timeMultiplier > MAXMULT) m_timeMultiplier = MAXMULT;
        break;
    } case Qt::Key_Left: {
        m_timeMultiplier *= 0.9f;
        if (m_timeMultiplier < MINMULT) m_timeMultiplier = MINMULT;
        break;
    } case Qt::Key_Space: {
        if (!m_isOrbiting) {
            m_lastTime = QTime(0,0).msecsTo(QTime::currentTime());
        }
        m_isOrbiting = !m_isOrbiting;
        break;
        }
    }
}

/**
 * @brief Based on mouse movement, does something
 * If a button was down, rotates the camera around the center of its scene.
 * If not, just saves the mouse position for later.
 * @param event The mouse event that triggered this
 */
void GLRenderWidget::mouseMoveEvent(QMouseEvent *event) {
    glm::vec2 pos(event->x(), event->y());
    if (event->buttons() & Qt::LeftButton || event->buttons() & Qt::RightButton) {
        m_camera.rotateAroundCenter(pos - m_prevMousePos);
    }
    m_prevMousePos = pos;
}

/**
 * @brief Register that a key was clicked by saving the previous positions
 * @param event The mouse event that triggered this
 */
void GLRenderWidget::mousePressEvent(QMouseEvent *event) {
    m_prevMousePos.x = event->x();
    m_prevMousePos.y = event->y();
}

/**
 * @brief Called when the mouse wheel scrolls - zooms camera
 * Zooms camera flat toward land when close enough
 * @param event The wheel event that triggered this
 */
void GLRenderWidget::wheelEvent(QWheelEvent *event) {
    if (event->orientation() == Qt::Vertical) {
        m_camera.zoom(event->delta());
    }
}

/**
 * @brief Currently just prints the FPS to console
 */
void GLRenderWidget::printFPS() {
    // Prints FPS and that's it
    fprintf(stdout, "FPS: %d\n", (int)(m_currentFPS + .5f));
    return;
}

/**
 * @brief Returns the current camera
 * @return m_camera
 */
Camera GLRenderWidget::getCamera() {
    return m_camera;
}

/**
 * @brief Returns the current camera transformation
 * @return m_transform
 */
Transforms GLRenderWidget::getTransformation() {
    return m_transform;
}

/**
 * @brief Returns the speed multiplier for the simulation
 * @return m_timeMultiplier
 */
float GLRenderWidget::getSimulationSpeed() {
    return m_timeMultiplier;
}

/**
 * @brief Returns the rotational speed for simulation
 * @return m_rotationalSpeed
 */
float GLRenderWidget::getRotationalSpeed() {
    return m_rotationalSpeed;
}

/**
 * @brief Returns if the simulation is paused
 * @return !m_isOrbiting
 */
bool GLRenderWidget::getPaused() {
    return !m_isOrbiting;
}
