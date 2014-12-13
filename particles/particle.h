#ifndef PARTICLE_H
#define PARTICLE_H

#include "GL/glew.h"

class Particle
{
public:
    //Uninitialized Sphere
    Particle();
    //Initialized Sphere
    Particle(const GLuint vertexLocation, const GLuint normalLocation);
    // Initialize Sphere: generate VAO, vertex data, and buffer it on GPU
    void init(const GLuint vertexLocation, const GLuint normalLocation);
    void draw();
private:
    bool m_isInitialized;
    GLuint m_vaoID;
};

#endif // PARTICLE_H
