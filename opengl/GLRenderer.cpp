#include "CS123Common.h"
#include "GLRenderer.h"
#include "settings.h"
#include "newmath.h"
#include <iostream>

#include <QFileDialog>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTime>
#include <QDebug>
#include "resourceloader.h"

GLRenderer::GLRenderer(QGLFormat format, QWidget *parent)
    : QGLWidget(format, parent), m_timer(this), m_fps(60.0f), m_increment(0) {
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    // Set up camera
    m_camera.center = glm::vec3(0.f, 0.f, 0.f);
    m_camera.up = glm::vec3(0.f, 1.f, 0.f);
    m_camera.zoom = 3.0f;
    m_camera.theta = M_PI * 1.5f, m_camera.phi = 0.2f;
    m_camera.fovy = 60.f;

    // Set up 60 FPS draw loop
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));

    // Start the timer for updating the screen
    m_timer.start(1000.0f / m_fps);
    m_lastUpdate = QTime(0,0).msecsTo(QTime::currentTime());
    m_numFrames = 0;
    m_timeMultiplier = 1.0f; // Standard speed
    m_isOrbiting = true; // Rotates the scene

    // Set up renderers
    m_stars = new StarsRenderer(this);
    m_planets = new PlanetsRenderer(this);
    m_flowers  = new FlowersRenderer(m_planets, this);
}

GLRenderer::~GLRenderer() {
    delete m_stars;
    delete m_planets;
    delete m_flowers;
}

////////// INITIALIZATION //////////

void GLRenderer::initializeGL() {
    // Set up OpenGL with shaders and FBOs
    glewExperimental = GL_TRUE;
    fprintf(stdout, "Using OpenGL Version %s\n", glGetString(GL_VERSION));
    GLuint err = glewInit();
    if (err != GLEW_OK) {
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
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

    // Load the camera
    updateCamera();
}

void GLRenderer::refresh() {
    m_stars->refresh();
    m_planets->refresh();
    m_flowers->refresh();
}

void GLRenderer::createShaderPrograms() {
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
  Allocate framebuffer objects.

  @param width: the viewport width
  @param height: the viewport height
 **/
void GLRenderer::createFramebufferObjects(glm::vec2 size) {
    m_stars->createFBO(size);
    m_planets->createFBO(size);
    m_flowers->createFBO(size);

    // Clear
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLRenderer::createFBO(GLuint *fbo, GLuint *colorAttach, int texID, glm::vec2 size, bool depth) {
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

////////// PAINTING //////////

void GLRenderer::renderFinalPass() {
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

void GLRenderer::paintGL() {
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

    m_rotationalSpeed = m_elapsedTime/(1.0*m_fps);
    glm::vec3 rotateAxis = glm::vec3(3,3,1);
    glm::mat4x4 localizedOrbit = glm::rotate(m_rotationalSpeed/2.0f, rotateAxis);

    m_stars->render();
    m_planets->render(localizedOrbit);
    m_flowers->render(localizedOrbit);
    renderFinalPass();

    printFPS();

    updateCamera();
}

/**
  Draws a textured quad. The texture must be bound and unbound
  before and after calling this method - this method assumes that the texture
  has been bound beforehand using glBindTexture.

  @param w: the width of the quad to draw
  @param h: the height of the quad to draw
**/
void GLRenderer::renderTexturedQuad() {
    // Clamp value to edge of texture when texture index is out of bounds
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    m_texquad.draw();
}

////////// UPDATERS //////////

/**
  Called when the screen gets resized.
  The camera is updated when the screen resizes because the aspect ratio may change.
**/
void GLRenderer::resizeGL(int width, int height) {
    // Set the viewport to fill the screen
    glViewport(0, 0, width, height);

    // Update the camera
    updateCamera();

    // Resize all used textures
    createFramebufferObjects(glm::vec2(width, height));
}

/**
  Update the camera's specifications.
  It gets called in resizeGL which get called automatically on intialization
  and whenever the window is resized.
**/
void GLRenderer::updateCamera() {
    float w = width();
    float h = height();

    float ratio = 1.0f * w / h;
    glm::vec3 dir(-fromAnglesN(m_camera.theta, m_camera.phi));
    glm::vec3 eye(m_camera.center - dir * m_camera.zoom);

    m_transform.projection = glm::perspective(m_camera.fovy, ratio, 0.1f, 1000.f);
    m_transform.view = glm::lookAt(eye, m_camera.center, m_camera.up);

    m_camera.eye = eye;
}

/**
  Specifies to Qt what to do when the widget needs to be updated.
  We only want to repaint the onscreen objects.
**/
void GLRenderer::tick() {
    update();
}

////////// USER INTERACTION //////////

/**
  Handles any key press from the keyboard
 **/
void GLRenderer::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
    case Qt::Key_S: {
        QImage qi = grabFrameBuffer(false);
        QString filter;
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("PNG Image (*.png)"), &filter);
        qi.save(QFileInfo(fileName).absoluteDir().absolutePath() + "/" + QFileInfo(fileName).baseName() + ".png", "PNG", 100);
        break;
    } case Qt::Key_R: {
        refresh();
        break;
    } case Qt::Key_H: {
        break;
    } case Qt::Key_Right: {
        m_timeMultiplier *= 1.1f;
        if (m_timeMultiplier > 100.0f) m_timeMultiplier = 100.0f;
        break;
    } case Qt::Key_Left: {
        m_timeMultiplier *= 0.9f;
        if (m_timeMultiplier < 0.1f) m_timeMultiplier = 0.1f;
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
  Called when the mouse is dragged.  Rotates the camera based on mouse movement.
**/
void GLRenderer::mouseMoveEvent(QMouseEvent *event) {
    glm::vec2 pos(event->x(), event->y());
    if (event->buttons() & Qt::LeftButton || event->buttons() & Qt::RightButton)
    {
        m_camera.mouseMove(pos - m_prevMousePos);
    }
    m_prevMousePos = pos;
}

/**
  Record a mouse press position.
 **/
void GLRenderer::mousePressEvent(QMouseEvent *event) {
    m_prevMousePos.x = event->x();
    m_prevMousePos.y = event->y();
}

/**
  Called when the mouse wheel is turned.  Zooms the camera in and out.
**/
void GLRenderer::wheelEvent(QWheelEvent *event) {
    if (event->orientation() == Qt::Vertical)
    {
        m_camera.mouseWheel(event->delta());
    }
}

////////// HELPERS //////////

/**
  Draws text for the FPS and screenshot prompt
 **/
void GLRenderer::printFPS() {
    // Prints FPS and that's it
    fprintf(stdout, "FPS: %d\n", (int)(m_currentFPS + .5f));
    return;
}

////////// GETTERS //////////

Camera GLRenderer::getCamera() {
    return m_camera;
}

Transforms GLRenderer::getTransformation() {
    return m_transform;
}

float GLRenderer::getSimulationSpeed() {
    return m_timeMultiplier;
}

float GLRenderer::getRotationalSpeed() {
    return m_rotationalSpeed;
}

bool GLRenderer::getPaused() {
    return !m_isOrbiting;
}
