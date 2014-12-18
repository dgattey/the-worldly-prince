#ifndef PARTICLE_H
#define PARTICLE_H

#include "GLCommon.h"

struct __attribute__ ((aligned (16))) ParticleData {
    float life;
    float decay;
    glm::vec3 color;
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 force;
};

class Particle {
public:
    Particle();
    Particle(const GLuint vertexLocation, const GLuint normalLocation);

    void init(const GLuint vertexLocation, const GLuint normalLocation);
    void draw();

private:
    bool m_isInitialized;
    GLuint m_vaoID;
};

#endif // PARTICLE_H
