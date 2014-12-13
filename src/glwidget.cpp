#include "glwidget.h"
#include "settings.h"
#include <iostream>

#include <QFileDialog>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTime>
#include "newmath.h"
#include "cs123_lib/resourceloader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "common.h"

#define VERTSMOON 50
#define VERTSEARTH 120

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent), m_timer(this), m_fps(60.0f), m_increment(0),
      m_font("Deja Vu Sans Mono", 12, 4)
{

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    // set up camera
    m_camera.center = glm::vec3(0.f, 0.f, 0.f);
    m_camera.up = glm::vec3(0.f, 1.f, 0.f);
    m_camera.zoom = 3.0f;
    m_camera.theta = M_PI * 1.5f, m_camera.phi = 0.2f;
    m_camera.fovy = 60.f;

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

    m_shaderSeed = ((float)rand()) / ((float)RAND_MAX);

    m_lastUpdate = QTime(0,0).msecsTo(QTime::currentTime());
    m_numFrames = 0;
}

GLWidget::~GLWidget()
{
    m_flowers.clear();
    delete &m_flowerSphere;
    delete &m_flowerCylinder;
    delete[] m_particleData;
}

void GLWidget::initializeGL()
{
    fprintf(stdout, "Using OpenGL Version %s\n", glGetString(GL_VERSION));

    //initialize glew
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
      /* Problem: glewInit failed, something is seriously wrong. */
      fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    createShaderPrograms();
    createFramebufferObjects(width(), height());

    // Set up the time for orbit
    m_lastTime = QTime(0,0).msecsTo(QTime::currentTime());

    // Create data
    initializeParticles();
    generateFlowers();

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

void GLWidget::refresh() {
    delete[] m_particleData;
    m_flowers.clear();
    initializeGL();
}

void GLWidget::initializeParticles() {
    m_numParticles = 4000;
    m_particleData = new ParticleData[m_numParticles];
    for (int i = 0; i<m_numParticles; i++) {
        float x,y,z;
        float radius = 0.0f;
        while (radius < 100.0f) {
            x = urand(-300.0f, 300.0f);
            y = urand(-300.0f, 300.0f);
            z = urand(-300.0f, 300.0f);
            radius = sqrt(pow(x,2.0f) + pow(y,2.0f) + pow(z,2.0f));
        }
        m_particleData[i].life = urand(0.0f, 150.0f);//100.0f;
        m_particleData[i].dir = glm::vec3(0.0f,0.0f,0.0f);
        m_particleData[i].pos = glm::vec3(x,y,z);
        m_particleData[i].color = glm::vec3(0.9f, 0.7f, 0.8f);
        m_particleData[i].decay = -1.0f;
        if (urand(0.0f,1.0f) > 0.5f)
            m_particleData[i].decay = 1.0f;
        if (urand(0.0f,1.0f) > 0.95f) { // SHOOTING STAR!
            m_particleData[i].color = glm::vec3(0.8f, 0.5f, 0.4f);
            m_particleData[i].dir = glm::vec3(urand(-2.0f, 2.0f),urand(-2.0f, 2.0f),urand(-2.0f, 2.0f));
        }
    }
}


/**
  Create shader programs. Use the ResourceLoader new*ShaderProgram helper methods.
 **/
void GLWidget::createShaderPrograms()
{
    m_shaderPrograms["flower"] = ResourceLoader::loadShaders(":/shaders/flower.vert", ":/shaders/flower.frag");

    m_shaderPrograms["planet"] = ResourceLoader::loadShaders(":/shaders/noise.vert", ":/shaders/noise.frag");
    m_moon.init(VERTSMOON, VERTSMOON,
                  glGetAttribLocation(m_shaderPrograms["planet"], "position"),
                  glGetAttribLocation(m_shaderPrograms["planet"], "normal"));
    m_earth.init(VERTSEARTH, VERTSEARTH,
                  glGetAttribLocation(m_shaderPrograms["planet"], "position"),
                  glGetAttribLocation(m_shaderPrograms["planet"], "normal"));

    m_shaderPrograms["tex"] = ResourceLoader::loadShaders(":/shaders/tex.vert", ":/shaders/tex.frag");
    m_texquad.init(glGetAttribLocation(m_shaderPrograms["tex"], "position"),
                   glGetAttribLocation(m_shaderPrograms["tex"], "texCoords"));

    m_shaderPrograms["star"] = ResourceLoader::loadShaders(":/shaders/star.vert",":/shaders/star.frag");
    m_particle.init(glGetAttribLocation(m_shaderPrograms["star"], "position"),
                    glGetAttribLocation(m_shaderPrograms["star"], "texCoord"));
}

/**
  Allocate framebuffer objects.

  @param width: the viewport width
  @param height: the viewport height
 **/
void GLWidget::createFramebufferObjects(int width, int height)
{
    // Creates the star FBO and texture
    glGenFramebuffers(1, &m_starFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_starFBO);
    glActiveTexture(GL_TEXTURE0); // Texture 0 is for stars
    glGenTextures(1, &m_starColorAttachment);
    glBindTexture(GL_TEXTURE_2D, m_starColorAttachment);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_starColorAttachment, 0);

    // Creates the flower FBO and texture
    glGenFramebuffers(1, &m_planetFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_planetFBO);
    glActiveTexture(GL_TEXTURE1); // Texture 1 is for planet
    glGenTextures(1, &m_planetColorAttachment);
    glBindTexture(GL_TEXTURE_2D, m_planetColorAttachment);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_planetColorAttachment, 0);

    // Needed to do depth on planet
    GLuint planetDepth;
    glGenRenderbuffers(1, &planetDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, planetDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, planetDepth);

    // Clear
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLWidget::paintGL()
{
    // Get the time in seconds
    m_numFrames++;
    int time = QTime(0,0).msecsTo(QTime::currentTime());

    if (m_isOrbiting) {
        m_elapsedTime += (time - m_lastTime);
        printf("elapsed time = %f\n", m_elapsedTime);
        m_lastTime = time;
    }

    if (time - m_lastUpdate > 1000) {
        m_currentFPS = m_numFrames / (float)((time - m_lastUpdate)/1000.f);
        m_numFrames = 0;
        m_lastUpdate = time;
    }

    glm::mat4x4 localizedOrbit = glm::rotate(m_elapsedTime/m_fps, glm::vec3(3,3,1));

    renderStarPass();
    renderPlanetPass(localizedOrbit);
    renderFlowerPass(localizedOrbit);
    renderFinalPass();

    // TODO: Take out paint fps info
    paintText();

    updateCamera();
}


void GLWidget::renderFlowers(glm::mat4x4 localizedOrbit)
{
    Transforms sphereTransform = m_transform;


    // iterate through each of the flowers and render the components
    for (std::list<Flower *>::const_iterator iterator = m_flowers.begin(), end = m_flowers.end(); iterator != end; ++iterator) {
        Flower *f = *iterator;

        sphereTransform.model = localizedOrbit * f->cylModel;

        glUniform3fv(glGetUniformLocation(m_shaderPrograms["flower"], "color"), 1, glm::value_ptr(glm::vec3(0, 0.5, 0)));
        glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["flower"], "mvp"), 1, GL_FALSE, &sphereTransform.getTransform()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["flower"], "m"), 1, GL_FALSE, &sphereTransform.model[0][0]);

        m_flowerCylinder.render();

        sphereTransform.model = localizedOrbit * f->centerModel;

        glUniform3fv(glGetUniformLocation(m_shaderPrograms["flower"], "color"), 1, glm::value_ptr(f->centerColor));
        glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["flower"], "mvp"), 1, GL_FALSE, &sphereTransform.getTransform()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["flower"], "m"), 1, GL_FALSE, &sphereTransform.model[0][0]);

        m_flowerSphere.render();

        for (int i = 0; i < f->petalCount; i++) {

            sphereTransform.model = localizedOrbit * f->petalModels[i];

            // hard code some of the colors
            glUniform3fv(glGetUniformLocation(m_shaderPrograms["flower"], "color"), 1, glm::value_ptr(f->petalColor));
            glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["flower"], "mvp"), 1, GL_FALSE, &sphereTransform.getTransform()[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["flower"], "m"), 1, GL_FALSE, &sphereTransform.model[0][0]);

            m_flowerSphere.render();
        }
    }
}

void GLWidget::renderStars() {
    for(int i =0; i<m_numParticles; i++) {

        Transforms particleTransform = m_transform;

        float x1 = m_particleData[i].pos.x;
        float y1 = m_particleData[i].pos.y;
        float z1 = m_particleData[i].pos.z;

        glm::vec3 n = glm::vec3(0.0f,0.0f,1.0f);
        glm::vec3 np = glm::normalize(glm::vec3(-x1,-y1,-z1));

        glm::vec3 view = glm::normalize(m_camera.eye);
        if (glm::dot(view, np) > 0) { // Backface Culling!!!!!!

            glm::vec3 axis = glm::cross(n, np);
            float angle = glm::acos(glm::dot(n, np) / (glm::length(glm::vec4(n,0.0f)) * glm::length(glm::vec4(np,0.0f))));

            particleTransform.model = glm::translate(glm::vec3(x1,y1,z1)) * glm::scale(glm::vec3(1.0f, 1.0f, 1.0f)) *
                    glm::rotate(angle*360.0f/6.28f, axis) * particleTransform.model;

            float pulse = 1.0f;// cos((int)m_particleData[i].life % 6);

            glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["star"], "mvp"), 1, GL_FALSE, &particleTransform.getTransform()[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["star"], "m"), 1, GL_FALSE, &particleTransform.model[0][0]);
            glUniform4f(glGetUniformLocation(m_shaderPrograms["star"], "color"),
                    pulse*m_particleData[i].color.x, pulse*m_particleData[i].color.y, pulse*m_particleData[i].color.z, m_particleData[i].life / 150.0f);

            m_particle.draw();

            // Give shooting stars their tail
            if (glm::length(glm::vec4(m_particleData[i].dir,0.0f)) > 0) {
                for (int dt = 0; dt <= 8; dt++) {
//                    for (int p = 0; p < 1; p++) {
                        float coeff = 0.5f;
                        glm::vec3 newPos = glm::vec3(m_particleData[i].pos.x - coeff*dt*m_particleData[i].dir.x /*- urand(-2.5f,2.5f)*/,
                                                     m_particleData[i].pos.y - coeff*dt*m_particleData[i].dir.y /*- urand(-2.5f,2.5f)*/,
                                                     m_particleData[i].pos.z - coeff*dt*m_particleData[i].dir.z /*- urand(-2.5f,2.5f)*/);
                        Transforms temp = m_transform;
                        temp.model = glm::translate(newPos) * glm::rotate(angle*360.0f/6.28f, axis) * glm::scale(glm::vec3(1.0f + 1.0f/((float)dt))) * temp.model;
                        glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["star"], "mvp"), 1, GL_FALSE, &temp.getTransform()[0][0]);
                        glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["star"], "m"), 1, GL_FALSE, &temp.model[0][0]);
                        glUniform4f(glGetUniformLocation(m_shaderPrograms["star"], "color"),
                                (1.0f/((float)dt))*m_particleData[i].color.x, (1.0f/((float)dt))*m_particleData[i].color.y, (1.0f/((float)dt))*m_particleData[i].color.z, m_particleData[i].life / 150.0f);
                        m_particle.draw();
//                    }
                }
            }
        }

        m_particleData[i].pos = m_particleData[i].pos + m_particleData[i].dir;
        m_particleData[i].life += 1.0f*m_particleData[i].decay;

        if (m_particleData[i].life <= 0 || m_particleData[i].life >= 150) {
            m_particleData[i].decay *= -1.0f;
            if (glm::length(glm::vec4(m_particleData[i].dir,0.0f)) > 0 && m_particleData[i].life <= 0) {
                m_particleData[i].pos.x = urand(-200.0f, 200.0f);
                m_particleData[i].pos.y = urand(-200.0f, 200.0f);
                m_particleData[i].pos.z = urand(-200.0f, 200.0f);
            }
        }
    }

}

void GLWidget::renderPlanet(glm::mat4x4 localizedOrbit) {
    Transforms sphereTransform = m_transform;
    float time = QTime(0,0).msecsTo(QTime::currentTime());
    glUniform1f(glGetUniformLocation(m_shaderPrograms["planet"], "seed"), m_shaderSeed);
    GLuint mvp = glGetUniformLocation(m_shaderPrograms["planet"], "mvp");
    GLuint colorLow = glGetUniformLocation(m_shaderPrograms["planet"], "colorLow");
    GLuint colorHigh = glGetUniformLocation(m_shaderPrograms["planet"], "colorHigh");
    GLuint threshold = glGetUniformLocation(m_shaderPrograms["planet"], "threshold");

    // Transform and render the moon - local orbit only, and all gray
    glm::vec4 gray = glm::vec4(0.48, 0.48, 0.5, 0.4);
    glUniform4fv(colorHigh, 1, &gray[0]);
    glUniform1f(threshold, -999.0f); // Only show colorHigh
    sphereTransform.model = localizedOrbit *
                            m_transform.model;
    glUniformMatrix4fv(mvp, 1, GL_FALSE, &sphereTransform.getTransform()[0][0]);
    m_moon.render();


    // Transform and render the earth - scale, local orbit, orbit around point
    glm::vec4 blue = glm::vec4(0.1, 0.1, 0.7, 0.6);
    glUniform4fv(colorLow, 1, &blue[0]);
    glm::vec4 green = glm::vec4(0.08, 0.55, 0.25, 0.15);
    glUniform4fv(colorHigh, 1, &green[0]);
    glUniform1f(threshold, 1.72f);
    sphereTransform.model = glm::rotate(time/(2*m_fps), glm::vec3(0,0,1)) *
                            glm::translate(glm::vec3(6, -4, 17)) *
                            glm::rotate(-time/m_fps, glm::vec3(1,2,4)) *
                            glm::scale(glm::vec3(3.3f)) *
                            m_transform.model;
    glUniformMatrix4fv(mvp, 1, GL_FALSE, &sphereTransform.getTransform()[0][0]);
    m_earth.render();
}

void GLWidget::renderStarPass()
{
    glUseProgram(m_shaderPrograms["star"]);
    glBindFramebuffer(GL_FRAMEBUFFER, m_starFBO);
    glBindTexture(GL_TEXTURE_2D, m_starColorAttachment);
    glActiveTexture(GL_TEXTURE0);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draws stars without depth and with blending and reset
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);

    renderStars();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    // Clear
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
}

void GLWidget::renderFlowerPass(glm::mat4x4 localizedOrbit)
{
    glUseProgram(m_shaderPrograms["flower"]);
    glBindFramebuffer(GL_FRAMEBUFFER, m_planetFBO);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_planetColorAttachment);

    // Draw shapes with depth and no blending
    renderFlowers(localizedOrbit);

    // Clear
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
}

void GLWidget::renderPlanetPass(glm::mat4x4 localizedOrbit)
{
    glUseProgram(m_shaderPrograms["planet"]);
    glBindFramebuffer(GL_FRAMEBUFFER, m_planetFBO);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_planetColorAttachment);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw the planet with depth and no blending
    renderPlanet(localizedOrbit);

    // Clear
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(0);
}

void GLWidget::renderFinalPass()
{
    // Draw to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_shaderPrograms["tex"]);
    glUniform1i(glGetUniformLocation(m_shaderPrograms["tex"], "starTex"), 0);
    glUniform1i(glGetUniformLocation(m_shaderPrograms["tex"], "planetTex"), 1);

    // Draw composed stars
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_starColorAttachment);

    // Draw composed planet and flowers
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_planetColorAttachment);
    renderTexturedQuad();

    // Clear
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// make flower gardens!
void GLWidget::generateFlowers()
{
    // the number of different types of flowers to generate
    int flowerVariety = 10;
    
    // how many similar flowers we should surround each flower with
    int gardenSize = 15;

    m_flowerCylinder.init(glGetAttribLocation(m_shaderPrograms["flower"], "position"),
                 glGetAttribLocation(m_shaderPrograms["flower"], "normal"));
    m_flowerSphere.init(5, 5,
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

////////// HELPER CODE ///////////

/**
  Draws a textured quad. The texture must be bound and unbound
  before and after calling this method - this method assumes that the texture
  has been bound beforehand using glBindTexture.

  @param w: the width of the quad to draw
  @param h: the height of the quad to draw
**/
void GLWidget::renderTexturedQuad()
{
    // Clamp value to edge of texture when texture index is out of bounds
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    m_texquad.draw();
}

/**
  Called when the screen gets resized.
  The camera is updated when the screen resizes because the aspect ratio may change.
**/
void GLWidget::resizeGL(int width, int height)
{
    // Set the viewport to fill the screen
    glViewport(0, 0, width, height);

    // Update the camera
    updateCamera();

    // Resize all used textures
    createFramebufferObjects(width, height);
}

/**
  Update the camera's specifications.
  It gets called in resizeGL which get called automatically on intialization
  and whenever the window is resized.
**/
void GLWidget::updateCamera()
{
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
void GLWidget::paintText()
{
    glColor3f(1.f, 1.f, 1.f);

    // QGLWidget's renderText takes xy coordinates, a string, and a font
    renderText(10, 20, "FPS: " + QString::number((int) (m_currentFPS + .5f)), m_font);
    renderText(10, 40, "(Space): Pause", m_font);
    renderText(10, 60, "R: Refresh", m_font);
}


/**
  Specifies to Qt what to do when the widget needs to be updated.
  We only want to repaint the onscreen objects.
**/
void GLWidget::tick()
{
    update();
}


//handle events from user
/**
  Handles any key press from the keyboard
 **/
void GLWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_S:
            {
            QImage qi = grabFrameBuffer(false);
            QString filter;
            QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("PNG Image (*.png)"), &filter);
            qi.save(QFileInfo(fileName).absoluteDir().absolutePath() + "/" + QFileInfo(fileName).baseName() + ".png", "PNG", 100);
            break;
            }
        case Qt::Key_R:
            {
            refresh();
            break;
            }
        case Qt::Key_Space:
            {
            if (!m_isOrbiting) {
                printf("wasn't orbiting, starting now\n");
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
void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
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
void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_prevMousePos.x = event->x();
    m_prevMousePos.y = event->y();
}

/**
  Called when the mouse wheel is turned.  Zooms the camera in and out.
**/
void GLWidget::wheelEvent(QWheelEvent *event)
{
    if (event->orientation() == Qt::Vertical)
    {
        m_camera.mouseWheel(event->delta());
    }
}
