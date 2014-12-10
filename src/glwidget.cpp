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

#include <QGLFramebufferObject>
#include <glm/gtx/rotate_vector.hpp>
//#include "shapes/Cylinder.h"

GLWidget::GLWidget( QWidget *parent )
    : QGLWidget( parent ), m_timer( this ), m_fps( 60.0f ), m_increment( 0 ),
      m_font( "Deja Vu Sans Mono", 12, 4 )
{

    setFocusPolicy( Qt::StrongFocus );
    setMouseTracking( true );

    // set up camera
    m_camera.center = glm::vec3( 0.f, 0.f, 0.f );
    m_camera.up = glm::vec3( 0.f, 1.f, 0.f );
    m_camera.zoom = 3.5f;
    m_camera.theta = M_PI * 1.5f, m_camera.phi = 0.2f;
    m_camera.fovy = 60.f;

    // Set up 60 FPS draw loop
    connect( &m_timer, SIGNAL( timeout() ), this, SLOT(tick() ) );

    // Start the timer for updating the screen
    m_timer.start( 1000.0f / m_fps );

    // set up light positions and intensities
    m_lightPositions[ 0 ] = glm::vec3( 0.f, 0.f,  0.f );
    m_lightPositions[ 1 ] = glm::vec3( 0.f, 0.f, -2.f );
    m_lightPositions[ 2 ] = glm::vec3( 0.f, 0.f,  2.f );
    m_lightPositions[ 3 ] = glm::vec3( 1.f, 1.f,  1.f );

    m_lightIntensities[ 0 ] = glm::vec3( 1.f, 0.f, 0.f );
    m_lightIntensities[ 1 ] = glm::vec3( 0.f, 1.f, 0.f );
    m_lightIntensities[ 2 ] = glm::vec3( 0.f, 0.f, 1.f );
    m_lightIntensities[ 3 ] = glm::vec3( 0.5, 0.5, 0.5 );

    // ambient and diffuse coefficients
    m_k_a = 0.2f;
    m_k_d = 0.8f;

    // set up coefficients and ambient intensity
    m_O_a = glm::vec3( 1.0,  1.0, 1.0 );    // ambient sphere color -- each channel in [0,1]
    m_O_d = glm::vec3( 1.0,  1.0, 1.0 );    // diffuse sphere color
    m_i_a = glm::vec3( 0.25, 0.25, 0.25 ); // ambient light intensity

    m_lastUpdate = QTime(0,0).msecsTo(QTime::currentTime());
    m_numFrames = 0;

    initializeParticles();

}

GLWidget::~GLWidget()
{

}

void GLWidget::initializeGL()
{
    fprintf( stdout, "Using OpenGL Version %s\n", glGetString( GL_VERSION ) );

    //initialize glew
    GLenum err = glewInit();
    if ( GLEW_OK != err )
    {
      /* Problem: glewInit failed, something is seriously wrong. */
      fprintf( stderr, "Error: %s\n", glewGetErrorString( err ) );
    }
    fprintf( stdout, "Status: Using GLEW %s\n", glewGetString( GLEW_VERSION ) );

    createShaderPrograms();
    createFramebufferObjects( width(), height() );

    // Enable depth testing, so that objects are occluded based on depth instead of drawing order
    glEnable( GL_DEPTH_TEST );

    // Enable back-face culling, meaning only the front side of every face is rendered
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

    // Specify that the front face is represented by vertices in counterclockwise order (this is the default)
    glFrontFace( GL_CCW );

    // Set up global (ambient) lighting
    // GLfloat global_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };

    // Set the screen color when the color buffer is cleared (in RGBA format)
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    // Load the initial settings
    updateCamera();
}

void GLWidget::initializeParticles() {
    m_numParticles = 2000;
    m_particleData = new ParticleData[m_numParticles];
    for (int i = 0; i<m_numParticles; i++) {
        float x = urand(-400.0f, 400.0f);
        float y = urand(-400.0f, 400.0f);
        float z = urand(-400.0f, 400.0f);
        m_particleData[i].life = urand(0.0f, 200.0f);//100.0f;
        m_particleData[i].dir = glm::vec3(0.0f,0.0f,0.0f);
        m_particleData[i].pos = glm::vec3(x,y,z);
        m_particleData[i].color = glm::vec3(0.8f, 0.6f, 0.8f);
        m_particleData[i].decay = -1.0f;
        if (urand(0.0f,1.0f) > 0.5f)
            m_particleData[i].decay = 1.0f;
        if (urand(0.0f,1.0f) > 0.87f) { // SHOOTING STAR!
            m_particleData[i].color = glm::vec3(0.8f, 0.5f, 0.4f);
            m_particleData[i].dir = glm::vec3(urand(-2.0f, 2.0f),urand(-2.0f, 2.0f),urand(-2.0f, 2.0f));
        }
    }
}

GLuint GLWidget::loadTexture(const QString &path)
{
    QImage texture;
    QFile file(path);
    if(!file.exists()) return -1;
    texture.load(file.fileName());
    texture = QGLWidget::convertToGLFormat(texture);

    // Put your code here

    // Generate a new OpenGL texture ID to put our image into
    GLuint id = 0;
    glGenTextures(1, &id);

    // Make the texture we just created the new active texture
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, texture.width(), texture.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.bits());
    // Copy the image data into the OpenGL texture

    // Set filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//    // Set coordinate wrapping options
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glBindTexture(GL_TEXTURE_2D, 0);

    return id; // Return something meaningful
}

/**
  Create shader programs. Use the ResourceLoader new*ShaderProgram helper methods.
 **/
void GLWidget::createShaderPrograms()
{
    m_shaderPrograms[ "phong" ] = ResourceLoader::loadShaders( ":/shaders/phong.vert", ":/shaders/phong.frag" );
    m_sphere.init( glGetAttribLocation( m_shaderPrograms[ "phong" ], "position" ), glGetAttribLocation( m_shaderPrograms[ "phong" ], "normal" ) );
    m_cylinder = new Cylinder(50,50,50,glGetAttribLocation( m_shaderPrograms[ "phong" ], "position" ),
            glGetAttribLocation( m_shaderPrograms[ "phong" ], "normal" ));

    m_shaderPrograms[ "lights" ] = ResourceLoader::loadShaders( ":/shaders/lights.vert",":/shaders/lights.frag" );
    m_shaderPrograms[ "brightpass" ] = ResourceLoader::loadShaders( ":/shaders/tex.vert", ":/shaders/brightpass.frag" );
    m_shaderPrograms[ "lightblur" ] = ResourceLoader::loadShaders( ":/shaders/tex.vert", ":/shaders/lightblur.frag" );
    m_shaderPrograms[ "tex" ] = ResourceLoader::loadShaders( ":/shaders/tex.vert", ":/shaders/tex.frag" );
    m_texquad.init(glGetAttribLocation(m_shaderPrograms[ "tex" ], "position"),glGetAttribLocation(m_shaderPrograms[ "tex" ], "texCoords" ) );

    m_shaderPrograms[ "star" ] = ResourceLoader::loadShaders( ":/shaders/star.vert",":/shaders/star.frag" );
    m_particle.init(glGetAttribLocation(m_shaderPrograms[ "star" ], "position"),glGetAttribLocation(m_shaderPrograms[ "star" ], "texCoord" ));

    m_starTexture = loadTexture(":/textures/particle2.bmp");//3.jpg");
}

/**
  Allocate framebuffer objects.

  @param width: the viewport width
  @param height: the viewport height
 **/
void GLWidget::createFramebufferObjects( int width, int height )
{
    // Allocate the main framebuffer object for rendering the scene to

    // Creates the phong FBO
    glGenFramebuffers( 1, &m_phongFBO );
    glBindFramebuffer( GL_FRAMEBUFFER, m_phongFBO );

    glActiveTexture( GL_TEXTURE0 );
    glGenTextures( 1, &m_phongColorAttachment );
    glBindTexture( GL_TEXTURE_2D, m_phongColorAttachment );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_phongColorAttachment, 0);

    glGenRenderbuffers( 1, &m_phongDepthAttachment );
    glBindRenderbuffer( GL_RENDERBUFFER, m_phongDepthAttachment );
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height );
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_phongDepthAttachment );

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    // TODO - Step 2.1: Create an FBO for brightPass. You don't need a depth attachment for this one.
    glGenFramebuffers( 1, &m_brightPassFBO );
    glBindFramebuffer( GL_FRAMEBUFFER, m_brightPassFBO );

//    glActiveTexture( GL_TEXTURE1 );
    glGenTextures( 1, &m_brightPassColorAttachment );
    glBindTexture( GL_TEXTURE_2D, m_brightPassColorAttachment );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_brightPassColorAttachment, 0);

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    // TODO - Step 2.1: Create an FBO for blur. You don't need a depth attachment for this one either.
    glGenFramebuffers( 1, &m_blurFBO );
    glBindFramebuffer( GL_FRAMEBUFFER, m_blurFBO );

//    glActiveTexture( GL_TEXTURE2 );
    glGenTextures( 1, &m_blurColorAttachment );
    glBindTexture( GL_TEXTURE_2D, m_blurColorAttachment );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_blurColorAttachment, 0);

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    // STAR FBO ?????????????????????????????????????
    glGenFramebuffers( 1, &m_starFBO );
    glBindFramebuffer( GL_FRAMEBUFFER, m_starFBO );

//    glActiveTexture( GL_TEXTURE2 );
    glGenTextures( 1, &m_starColorAttachment );
    glBindTexture( GL_TEXTURE_2D, m_starColorAttachment );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_starColorAttachment, 0);

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void GLWidget::paintGL()
{
    // Get the time in seconds
    m_numFrames++;
    int time = QTime(0,0).msecsTo(QTime::currentTime());

    if (time - m_lastUpdate > 1000) {
        m_currentFPS = m_numFrames / (float)((time - m_lastUpdate)/1000.f);
        m_numFrames = 0;
        m_lastUpdate = time;
    }

    renderStarPass();

    // Draw scene
    renderGeometryPass();

    // Process brightpass
    renderBrightPass();

    // Process blur
    renderBlur( width(), height() );

    // Process final pass
    renderFinalPass();

    // paint fps info
    paintText();

    updateCamera();
}


/**
  Renders the spheres in our scene. We'll bind the Phong shader in here.
**/
void GLWidget::renderShapes()
{
    // TODO - Step 1.1:
    //   - Bind the "phong" shader program
    glUseProgram(m_shaderPrograms["phong"]);

//    for (int i = 0; i < 6; i++ )
//    {
//        float sign = i % 2 ? 1.f : -1.f;
//        float x = i % 3 ? 0.f : sign;
//        float y = ( i + 1) % 3 ? 0.f : sign;
//        float z = ( i + 2) % 3 ? 0.f : sign;
//        Transforms sphereTransform = m_transform;

//        sphereTransform.model =  glm::translate( glm::vec3( x, y, z ) )  * glm::scale( glm::vec3( 0.5f, 0.5f, 0.5f) ) * sphereTransform.model;

//        // TODO - Step 1.3,1.4:
//        //   - Set the necessary uniforms (can be found in phong.vert and phong.frag)
//        glUniform1f(glGetUniformLocation(m_shaderPrograms["phong"], "k_a"), m_k_a);
//        glUniform1f(glGetUniformLocation(m_shaderPrograms["phong"], "k_d"), m_k_d);

//        glUniform3fv(glGetUniformLocation(m_shaderPrograms["phong"], "O_a"), 1, glm::value_ptr(m_O_a));
//        glUniform3f(glGetUniformLocation(m_shaderPrograms["phong"], "O_d"), m_O_d.x, m_O_d.y, m_O_d.z);
//        glUniform3f(glGetUniformLocation(m_shaderPrograms["phong"], "i_a"), m_i_a.x, m_i_a.y, m_i_a.z);

//        glUniform3fv(glGetUniformLocation(m_shaderPrograms["phong"], "lightIntensities"), NUM_LIGHTS, glm::value_ptr(m_lightIntensities[0]));
//        glUniform3fv(glGetUniformLocation(m_shaderPrograms["phong"], "lightPositions"), NUM_LIGHTS, glm::value_ptr(m_lightPositions[0]));

//        glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["phong"], "mvp"), 1, GL_FALSE, &sphereTransform.getTransform()[0][0]);
//        glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["phong"], "m"), 1, GL_FALSE, &sphereTransform.model[0][0]);

//        m_sphere.draw();
//    }

    // TEST OUT BEING ABLE TO DRAW ANOTHER SPHERE
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    Transforms sphereTransform = m_transform;

    sphereTransform.model =  glm::translate( glm::vec3( x, y, z ) )  * glm::scale( glm::vec3( 1.5f, 1.5f, 1.5f) ) * sphereTransform.model;

    // TODO - Step 1.3,1.4:
    //   - Set the necessary uniforms (can be found in phong.vert and phong.frag)
    glUniform1f(glGetUniformLocation(m_shaderPrograms["phong"], "k_a"), m_k_a);
    glUniform1f(glGetUniformLocation(m_shaderPrograms["phong"], "k_d"), m_k_d);

    glUniform3fv(glGetUniformLocation(m_shaderPrograms["phong"], "O_a"), 1, glm::value_ptr(m_O_a));
    glUniform3f(glGetUniformLocation(m_shaderPrograms["phong"], "O_d"), m_O_d.x, m_O_d.y, m_O_d.z);
    glUniform3f(glGetUniformLocation(m_shaderPrograms["phong"], "i_a"), m_i_a.x, m_i_a.y, m_i_a.z);

    glUniform3fv(glGetUniformLocation(m_shaderPrograms["phong"], "lightIntensities"), NUM_LIGHTS, glm::value_ptr(m_lightIntensities[0]));
    glUniform3fv(glGetUniformLocation(m_shaderPrograms["phong"], "lightPositions"), NUM_LIGHTS, glm::value_ptr(m_lightPositions[0]));

    glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["phong"], "mvp"), 1, GL_FALSE, &sphereTransform.getTransform()[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["phong"], "m"), 1, GL_FALSE, &sphereTransform.model[0][0]);

    m_sphere.draw();

    sphereTransform = m_transform;

    x = urand(-.75f, 0.75f);
    z = sqrt((0.75f*0.75f) - (x*x)) - urand(0.0f,0.75f);
    if (urand(0.0f,1.0f) > 0.5f)
        z = z * -1;
    y = sqrt((0.75f*0.75f) - (x*x) - (z*z));
    if (urand(0.0f,1.0f) > 0.5f)
        y = y * -1;
    sphereTransform.model = glm::translate( glm::vec3( x, y, z ) ) * glm::rotate(45.0f, glm::vec3(x, y, z)) * glm::scale(glm::vec3(0.05f, 0.5f, 0.05f)) * sphereTransform.model;

    // TODO - Step 1.3,1.4:
    //   - Set the necessary uniforms (can be found in phong.vert and phong.frag)
    glUniform1f(glGetUniformLocation(m_shaderPrograms["phong"], "k_a"), m_k_a);
    glUniform1f(glGetUniformLocation(m_shaderPrograms["phong"], "k_d"), m_k_d);

    glUniform3fv(glGetUniformLocation(m_shaderPrograms["phong"], "O_a"), 1, glm::value_ptr(m_O_a));
    glUniform3f(glGetUniformLocation(m_shaderPrograms["phong"], "O_d"), m_O_d.x, m_O_d.y, m_O_d.z);
    glUniform3f(glGetUniformLocation(m_shaderPrograms["phong"], "i_a"), m_i_a.x, m_i_a.y, m_i_a.z);

    glUniform3fv(glGetUniformLocation(m_shaderPrograms["phong"], "lightIntensities"), NUM_LIGHTS, glm::value_ptr(m_lightIntensities[0]));
    glUniform3fv(glGetUniformLocation(m_shaderPrograms["phong"], "lightPositions"), NUM_LIGHTS, glm::value_ptr(m_lightPositions[0]));

    glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["phong"], "mvp"), 1, GL_FALSE, &sphereTransform.getTransform()[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["phong"], "m"), 1, GL_FALSE, &sphereTransform.model[0][0]);

    m_cylinder->draw();

    // TODO - Step 1.1:
    //   - Unbind the "phong" shader program
    glUseProgram(0);
}

void GLWidget::renderStars() {
    // TODO : Use particles to render stars in the sky!
//    glUseProgram(m_shaderPrograms[ "star" ]);
//    glBindTexture(GL_TEXTURE_2D, m_starTexture);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the color & depth buffers.
    glActiveTexture(GL_TEXTURE0); // Set the active texture to texture 0.
    glUniform1i(glGetUniformLocation(m_shaderPrograms["star"],"textureSampler"), 0); // Tell the shader to use texture 0.

    glUseProgram(m_shaderPrograms[ "star" ]);
    glBindTexture(GL_TEXTURE_2D, m_starTexture);

    for(int i =0; i<m_numParticles; i++) {

        Transforms particleTransform = m_transform;

        float x1 = m_particleData[i].pos.x;
        float y1 = m_particleData[i].pos.y;
        float z1 = m_particleData[i].pos.z;

        glm::vec3 n = glm::vec3(0.0f,0.0f,1.0f);
        glm::vec3 np = glm::normalize(glm::vec3(-x1,-y1,-z1));
        glm::vec3 axis = glm::cross(n, np);
        float angle = glm::acos(glm::dot(n, np) / (glm::length(glm::vec4(n,0.0f)) * glm::length(glm::vec4(np,0.0f))));

        particleTransform.model = glm::translate(glm::vec3(x1,y1,z1)) * glm::scale(glm::vec3(1.0f, 1.0f, 1.0f)) *
                glm::rotate(angle*360.0f/6.28f, axis) * particleTransform.model;

        float pulse = 1.0f;// cos((int)m_particleData[i].life % 6);

        glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["star"], "mvp"), 1, GL_FALSE, &particleTransform.getTransform()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shaderPrograms["star"], "m"), 1, GL_FALSE, &particleTransform.model[0][0]);
        glUniform4f(glGetUniformLocation(m_shaderPrograms["star"], "color"),
                pulse*m_particleData[i].color.x, pulse*m_particleData[i].color.y, pulse*m_particleData[i].color.z, m_particleData[i].life / 200.0f);

        m_particle.draw();
        m_particleData[i].pos = m_particleData[i].pos + m_particleData[i].dir;
        m_particleData[i].life += 1.0f*m_particleData[i].decay;

        if (m_particleData[i].life <= 0 || m_particleData[i].life >= 200) {
            m_particleData[i].decay *= -1.0f;
            if (glm::length(glm::vec4(m_particleData[i].dir,0.0f)) > 0) {
                m_particleData[i].pos.x = urand(-400.0f, 400.0f);
                m_particleData[i].pos.y = urand(-400.0f, 400.0f);
                m_particleData[i].pos.z = urand(-400.0f, 400.0f);
            }
        }
    }

    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);

//    glFlush();
//    swapBuffers();
}


/**
  Run a gaussian blur on the texture stored in fbo 2 and
  put the result in fbo 1.  The blur should have a radius of 8.

  @param width: the viewport width
  @param height: the viewport height
**/
void GLWidget::renderBlur( int width, int height )
{
    int radius = 3;
    int dim = radius * 2 + 1;
    GLint arraySize = dim * dim;
    GLfloat kernel[ arraySize ];
    GLfloat offsets[ arraySize * 2 ];
    createBlurKernel ( radius, width, height, &kernel[ 0 ], &offsets[ 0 ] );

    // TODO - Step 2.4:
    //   - Bind and clear the appropriate frame buffer
    //   - Bind the "lightblur" shader program
    //   - Set the necessary uniforms
    //   - Render the appropriate textured quad
    //   - Unbind the shader program
    glBindFramebuffer( GL_FRAMEBUFFER, m_blurFBO );
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_shaderPrograms["lightblur"]);
    glUniform1i(glGetUniformLocation(m_shaderPrograms["lightblur"], "tex"), 0);
    glUniform1i(glGetUniformLocation(m_shaderPrograms["lightblur"], "arraySize"), arraySize);
    glUniform2fv(glGetUniformLocation(m_shaderPrograms["lightblur"], "offsets"), arraySize, &offsets[0]);
    glUniform1fv(glGetUniformLocation(m_shaderPrograms["lightblur"], "kernel"), arraySize, &kernel[0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_brightPassColorAttachment);
    renderTexturedQuad();
    glBindTexture(GL_TEXTURE_2D, 0);

    glUseProgram(0);
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );


}

void GLWidget::renderBrightPass()
{
    // TODO - Step 2.3:
    //   - Bind and clear the appropriate frame buffer
    //   - Bind the "brightpass" shader program
    //   - Set the necessary uniforms
    //   - Render the appropriate textured quad
    glBindFramebuffer( GL_FRAMEBUFFER, m_brightPassFBO );
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glClearColor(0,0,0,0);

    glUseProgram(m_shaderPrograms["brightpass"]);
    glUniform1i(glGetUniformLocation(m_shaderPrograms["brightpass"], "tex"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_phongColorAttachment); //use mbrightpass??????????????????????
    renderTexturedQuad();
    glBindTexture(GL_TEXTURE_2D, 0);

    glUseProgram(0);
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void GLWidget::renderFinalPass()
{
    // TODO - Step 2.5:
    //   - Bind and clear the appropriate frame buffer
    //   - Bind the "tex" shader program
    //   - Set the necessary uniforms
    //   - Render the appropriate textured quad
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_shaderPrograms["tex"]);
    glUniform1i(glGetUniformLocation(m_shaderPrograms["tex"], "originalTex"), 0);
    glUniform1i(glGetUniformLocation(m_shaderPrograms["tex"], "blurTex"), 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_phongColorAttachment);
//    renderTexturedQuad();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_blurColorAttachment);
    renderTexturedQuad();
//    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glUseProgram(0);
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

}

void GLWidget::renderGeometryPass()
{
    // TODO - Step 2.2:
    //   - Bind the appropriate FBO and clear it
    glBindFramebuffer( GL_FRAMEBUFFER, m_phongFBO );
    glUseProgram(m_shaderPrograms["phong"]);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_starColorAttachment);

    renderStars();
    renderShapes();
//    renderLights();

    glBindTexture(GL_TEXTURE_2D, 0);

    // TODO - Step 2.2:
    //  - Unbind the FBO
    glUseProgram(0);
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void GLWidget::renderStarPass()
{
    glBindFramebuffer( GL_FRAMEBUFFER, m_starFBO );
    glUseProgram(m_shaderPrograms["star"]);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderStars();

    glUseProgram(0);
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}


////////// HELPER CODE ///////////

/**
 * Draw small white spheres to tell us where our lights are.
 */
void GLWidget::renderLights()
{
    glUseProgram( m_shaderPrograms[ "lights" ] );
    for( int i = 0; i < NUM_LIGHTS; i++ )
    {
        Transforms sphereTransform = m_transform;
        sphereTransform.model =
                glm::translate( glm::vec3( m_lightPositions[ i ].x, m_lightPositions[ i ].y, m_lightPositions[ i ].z ) )
                * glm::scale( glm::vec3( 0.05f, 0.05f, 0.05f ) )
                * sphereTransform.model;
        glUniform3f( glGetUniformLocation( m_shaderPrograms[ "lights" ], "color" ), 1.0f, 1.0f, 1.0f );
        glUniformMatrix4fv( glGetUniformLocation( m_shaderPrograms[ "lights" ], "mvp" ), 1, GL_FALSE, &sphereTransform.getTransform()[ 0 ][ 0 ] );
        m_sphere.draw();
    }
    glUseProgram( 0 );
}

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
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    m_texquad.draw();
}

/**
  Called when the screen gets resized.
  The camera is updated when the screen resizes because the aspect ratio may change.
**/
void GLWidget::resizeGL( int width, int height )
{
    // Set the viewport to fill the screen
    glViewport( 0, 0, width, height );

    // Update the camera
    updateCamera();
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
    float aspectRatio = 1.0f * w / h;

    float ratio = 1.0f * w / h;
    glm::vec3 dir( -fromAnglesN( m_camera.theta, m_camera.phi ) );
    glm::vec3 eye( m_camera.center - dir * m_camera.zoom );

    m_transform.projection = glm::perspective( m_camera.fovy, ratio, 0.1f, 1000.f );
    m_transform.view = glm::lookAt( eye, m_camera.center, m_camera.up );

    m_camera.eye = eye;
}


/**
  Creates a gaussian blur kernel with the specified radius.  The kernel values
  and offsets are stored.

  @param radius: The radius of the kernel to create.
  @param width: The width of the image.
  @param height: The height of the image.
  @param kernel: The array to write the kernel values to.
  @param offsets: The array to write the offset values to.
**/
void GLWidget::createBlurKernel( int radius, int width, int height,
                                                    GLfloat* kernel, GLfloat* offsets )
{
    int size = radius * 2 + 1;
    float sigma = radius / 3.0f;
    float twoSigmaSigma = 2.0f * sigma * sigma;
    float rootSigma = sqrt( twoSigmaSigma * M_PI );
    float total = 0.0f;
    float xOff = 1.0f / width, yOff = 1.0f / height;
    int offsetIndex = 0;
    for ( int y = -radius, idx = 0; y <= radius; ++y )
    {
        for ( int x = -radius; x <= radius; ++x, ++idx )
        {
            float d = x * x + y * y;
            kernel[ idx ] = exp( -d / twoSigmaSigma ) / rootSigma;
            total += kernel[ idx ];
            offsets[ offsetIndex++ ] = x * xOff;
            offsets[ offsetIndex++ ] = y * yOff;
        }
    }
    for ( int i = 0; i < size * size; ++i )
    {
        kernel[ i ] /= total;
    }
}

/**
  Draws text for the FPS and screenshot prompt
 **/
void GLWidget::paintText()
{
    glColor3f( 1.f, 1.f, 1.f );

    // QGLWidget's renderText takes xy coordinates, a string, and a font
    renderText( 10, 20, "FPS: " + QString::number( ( int ) ( m_currentFPS + .5f ) ), m_font );
    renderText( 10, 35, "S: Save screenshot", m_font );
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
void GLWidget::keyPressEvent( QKeyEvent *event )
{
    switch( event->key() )
    {
        case Qt::Key_S:
        QImage qi = grabFrameBuffer( false );
        QString filter;
        QString fileName = QFileDialog::getSaveFileName( this, tr( "Save Image" ), "", tr( "PNG Image (*.png)" ), &filter );
        qi.save( QFileInfo( fileName ).absoluteDir().absolutePath() + "/" + QFileInfo( fileName ).baseName() + ".png", "PNG", 100 );
        break;
    }
}


/**
  Called when the mouse is dragged.  Rotates the camera based on mouse movement.
**/
void GLWidget::mouseMoveEvent( QMouseEvent *event )
{
    glm::vec2 pos( event->x(), event->y() );
    if ( event->buttons() & Qt::LeftButton || event->buttons() & Qt::RightButton )
    {
        m_camera.mouseMove( pos - m_prevMousePos );
    }
    m_prevMousePos = pos;
}

/**
  Record a mouse press position.
 **/
void GLWidget::mousePressEvent( QMouseEvent *event )
{
    m_prevMousePos.x = event->x();
    m_prevMousePos.y = event->y();
}

/**
  Called when the mouse wheel is turned.  Zooms the camera in and out.
**/
void GLWidget::wheelEvent( QWheelEvent *event )
{
    if ( event->orientation() == Qt::Vertical )
    {
        m_camera.mouseWheel( event->delta() );
    }
}
