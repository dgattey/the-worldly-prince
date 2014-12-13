#ifndef GLWIDGET_H
#define GLWIDGET_H
#include <GL/glew.h>
#include <QGLWidget>
#include <QTimer>
#include "camera.h"
#include <GL/glu.h>
#include "cs123_lib/transforms.h"
#include "texquad.h"
#include "particle.h"
#include "flower.h"
#include "cylinder.h"
#include "sphere.h"


#define NUM_LIGHTS 4

class QGLFramebufferObject;

class GLWidget : public QGLWidget
{

    /**
      * Basic definition for a particle. You should not need to modify this.
      */
    struct __attribute__ ((aligned (16))) ParticleData
    {
        /**
          * The amount of life remaining. When the particle is first born, it should
          * have a life of 1.0. Each step this should decrease by decay (i.e.
          * life -= decay). When this value reaches zero, the particle should reset.
          */
        float life;
        /**
          * The amount the life of this particle decreases per step. See the documentation
          * for Particle.life.
          */
        float decay;
        /**
          * The particle's color. Currently the same for all particles; however, it would be
          * possible to have randomly generated colors or colors based on time. Used when
          * drawing the particle.
          */
        glm::vec3 color;
        /**
          * The particle's current position in 3D space. Updated every step based on
          * the particle's velocity.
          */
        glm::vec3 pos;
        /**
          * The direction this particle is currently moving. The velocity of any particle
          * in the system is: velocity = ParticleEmitter.m_speed * Particle.dir
          */
        glm::vec3 dir;
        /**
          * The force acting on this particle (e.g. from gravity). At each update step,
          * Particle.dir += Particle.force.
          */
        glm::vec3 force;
    };

    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();

    void updateCamera();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    void createShaderPrograms();
    void createFramebufferObjects(int width, int height);

    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    void renderStars();
    void renderFlowers(glm::mat4x4 localizedOrbit);
    void renderPlanet(glm::mat4x4 localizedOrbit);
    void renderTexturedQuad();
    void generateFlowers();
    void initializeParticles();

    void renderFinalPass();
    void renderFlowerPass(glm::mat4x4 localizedOrbit);
    void renderPlanetPass(glm::mat4x4 localizedOrbit);
    void renderStarPass();

    void paintText();

protected slots:
    void tick();

private:
    LabCamera m_camera;
    Transforms m_transform;
    Sphere m_moon;
    Sphere m_earth;
    TexQuad m_texquad;
    Particle m_particle;
    int m_numParticles;
    ParticleData *m_particleData;
    std::list<Flower *> m_flowers;

    QTimer m_timer;
    float m_fps;
    float m_increment;
    glm::vec2 m_prevMousePos;
    int m_lastUpdate;
    int m_numFrames;
    float m_currentFPS;

    QFont m_font; // font for rendering text

    // Resources
    QHash<QString, GLuint> m_shaderPrograms; // hash table for easy access to shaders

    glm::vec3 m_lightPositions[NUM_LIGHTS];
    glm::vec3 m_lightIntensities[NUM_LIGHTS];

    float m_k_a;
    float m_k_d;

    glm::vec3 m_O_a;
    glm::vec3 m_O_d;
    glm::vec3 m_i_a;

    GLuint m_planetFBO;
    GLuint m_planetColorAttachment;

    GLuint m_starFBO;
    GLuint m_starColorAttachment;

};

#endif // GLWIDGET_H
