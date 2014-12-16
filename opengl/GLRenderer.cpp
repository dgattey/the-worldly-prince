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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

GLRenderer::GLRenderer(QGLFormat format, QWidget *parent)
    : QGLWidget(format, parent), m_timer(this), m_fps(60.0f), m_increment(0) {

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    // set up camera
    m_camera.center = glm::vec3(0.f, 0.f, 0.f);
    m_camera.up = glm::vec3(0.f, 1.f, 0.f);
    m_camera.zoom = 3.0f;
    m_camera.theta = M_PI * 1.5f, m_camera.phi = 0.2f;
    m_camera.fovy = 60.f;
    m_isOrbiting = true; // rotations

    // Set up 60 FPS draw loop
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));

    // Start the timer for updating the screen
    m_timer.start(1000.0f / m_fps);

    // set up light positions and intensities
    m_lightPositions[0] = glm::vec3(0.f, 0.f,  0.f);
    m_lightPositions[1] = glm::vec3(0.f, 0.f, -2.f);
    m_lightPositions[2] = glm::vec3(0.f, 0.f,  2.f);
    m_lightPositions[3] = glm::vec3(1.f, 1.f,  1.f);

    m_lightIntensities[0] = glm::vec3(1.f, 0.f, 0.f);
    m_lightIntensities[1] = glm::vec3(0.f, 1.f, 0.f);
    m_lightIntensities[2] = glm::vec3(0.f, 0.f, 1.f);
    m_lightIntensities[3] = glm::vec3(0.5, 0.5, 0.5);

    // ambient and diffuse coefficients
    m_k_a = 0.2f;
    m_k_d = 0.8f;

    // set up coefficients and ambient intensity
    m_O_a = glm::vec3(1.0,  1.0, 1.0);    // ambient sphere color -- each channel in [0,1]
    m_O_d = glm::vec3(1.0,  1.0, 1.0);    // diffuse sphere color
    m_i_a = glm::vec3(0.25, 0.25, 0.25); // ambient light intensity

    m_lastUpdate = QTime(0,0).msecsTo(QTime::currentTime());
    m_numFrames = 0;
    m_timeMultiplier = 1.0f;

    m_stars = new StarsRenderer(this);
    m_planets = new PlanetsRenderer(this);
}

GLRenderer::~GLRenderer() {
    m_flowers.clear();
    delete m_flowerSphere;
    delete m_flowerCylinder;

    delete m_stars;
    delete m_planets;
}

void GLRenderer::initializeGL() {
    fprintf(stdout, "Using OpenGL Version %s\n", glGetString(GL_VERSION));

    //initialize glew
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
      /* Problem: glewInit failed, something is seriously wrong. */
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    createShaderPrograms();
    createFramebufferObjects(glm::vec2(width(), height()));

    // Set up the time for orbit
    m_lastTime = QTime(0,0).msecsTo(QTime::currentTime());

    // Create data
    refresh();

    // Enable depth testing, so that objects are occluded based on depth instead of drawing order
    glEnable(GL_DEPTH_TEST);

    // Enable back-face culling, meaning only the front side of every face is rendered
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Specify that the front face is represented by vertices in counterclockwise order (this is the default)
    glFrontFace(GL_CCW);

    // Set the screen color when the color buffer is cleared (in RGBA format)
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Load the initial settings
    updateCamera();
}

void GLRenderer::refresh() {
    m_flowers.clear();
    generateFlowers();

    m_stars->refresh();
    m_planets->refresh();
}


/**
  Create shader programs. Use the ResourceLoader new*ShaderProgram helper methods.
 **/
void GLRenderer::createShaderPrograms() {
    fprintf(stdout, "\nCompiling all shaders...\n");
    m_stars->createShaderProgram();
    m_planets->createShaderProgram();
    m_shaderPrograms["flower"] = ResourceLoader::loadShaders(":/shaders/flower.vert", ":/shaders/flower.frag");

    m_shaderPrograms["tex"] = ResourceLoader::loadShaders(":/shaders/tex.vert", ":/shaders/tex.frag");
    m_texquad.init(glGetAttribLocation(m_shaderPrograms["tex"], "position"),
                   glGetAttribLocation(m_shaderPrograms["tex"], "texCoords"));
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
    renderFlowersPass(localizedOrbit);
    renderFinalPass();

    printFPS();

    updateCamera();
}


void GLRenderer::renderFlowers(glm::mat4x4 localizedOrbit)
{
    Transforms sphereTransform = m_transform;

    // iterate through each of the flowers and render the components
    for (std::list<Flower *>::const_iterator iterator = m_flowers.begin(), end = m_flowers.end(); iterator != end; ++iterator) {
        Flower *f = *iterator;

        sphereTransform.model = localizedOrbit * f->cylModel;

        glUniform3fv(glGetUniformLocation(m_shaderPrograms["flower"], "color"), 1, glm::value_ptr(glm::vec3(0, 0.5, 0)));
        glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["flower"], "mvp"), 1, GL_FALSE, &sphereTransform.getTransform()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["flower"], "m"), 1, GL_FALSE, &sphereTransform.model[0][0]);

        m_flowerCylinder->render();

        sphereTransform.model = localizedOrbit * f->centerModel;

        glUniform3fv(glGetUniformLocation(m_shaderPrograms["flower"], "color"), 1, glm::value_ptr(f->centerColor));
        glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["flower"], "mvp"), 1, GL_FALSE, &sphereTransform.getTransform()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["flower"], "m"), 1, GL_FALSE, &sphereTransform.model[0][0]);

        m_flowerSphere->render();

        for (int i = 0; i < f->petalCount; i++) {

            sphereTransform.model = localizedOrbit * f->petalModels[i];

            // hard code some of the colors
            glUniform3fv(glGetUniformLocation(m_shaderPrograms["flower"], "color"), 1, glm::value_ptr(f->petalColor));
            glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["flower"], "mvp"), 1, GL_FALSE, &sphereTransform.getTransform()[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["flower"], "m"), 1, GL_FALSE, &sphereTransform.model[0][0]);

            m_flowerSphere->render();
        }
    }
}

void GLRenderer::renderFlowersPass(glm::mat4x4 localizedOrbit)
{
    glUseProgram(m_shaderPrograms["flower"]);
    glBindFramebuffer(GL_FRAMEBUFFER, *m_planets->getFBO());
    glActiveTexture(GL_TEXTURE0+m_planets->getTextureID());
    glBindTexture(GL_TEXTURE_2D, *m_planets->getColorAttach());

    // Draw shapes with depth and no blending
    renderFlowers(localizedOrbit);

    // Clear
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
}

void GLRenderer::renderFinalPass() {
    // Draw to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int starID = m_stars->getTextureID();
    int planetID = m_planets->getTextureID();

    // Get shader ready
    glUseProgram(m_shaderPrograms["tex"]);
    glUniform1i(glGetUniformLocation(m_shaderPrograms["tex"], "starTex"), starID);
    glUniform1i(glGetUniformLocation(m_shaderPrograms["tex"], "planetTex"), planetID);

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

// make flower gardens!
void GLRenderer::generateFlowers()
{
    // the number of different types of flowers to generate
    int flowerVariety = 10;
    
    // how many similar flowers we should surround each flower with
    int gardenSize = 15;

    m_flowerCylinder = new Cylinder();
    m_flowerCylinder->init(glGetAttribLocation(m_shaderPrograms["flower"], "position"),
                 glGetAttribLocation(m_shaderPrograms["flower"], "normal"));
    m_flowerSphere = new Sphere();
    m_flowerSphere->init(5, 5,
                   glGetAttribLocation(m_shaderPrograms["flower"], "position"),
                   glGetAttribLocation(m_shaderPrograms["flower"], "normal"));

    for (int i = 0; i < flowerVariety; i++) {
    	// our template flower
        Flower *f = new Flower();
        m_flowers.push_back(f);

        // other similar flowers
        for (int j = 0; j < gardenSize; j++) {
            m_flowers.push_back(new Flower(f));
        }
    }
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

////////// HELPER CODE ///////////

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
  Draws text for the FPS and screenshot prompt
 **/
void GLRenderer::printFPS() {
    // Prints FPS and that's it
    fprintf(stdout, "FPS: %d\n", (int)(m_currentFPS + .5f));
    return;
}

/**
  Specifies to Qt what to do when the widget needs to be updated.
  We only want to repaint the onscreen objects.
**/
void GLRenderer::tick() {
    update();
}

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
