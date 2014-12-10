#include "particleemitter.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <stdlib.h>

ParticleEmitter::ParticleEmitter(glm::vec3 color, glm::vec3 velocity,
                                 glm::vec3 force, float scale, float fuzziness, float speed,
                                 unsigned maxParticles) :
                    m_maxParticles(maxParticles), m_speed(speed),
                    m_fuzziness(fuzziness), m_scale(scale), m_color(color), m_velocity(velocity),
                    m_force(force)
{
    m_particles = new Particle[maxParticles];
    resetParticles();
}

ParticleEmitter::~ParticleEmitter()
{
    if (m_particles)
    {
        delete[] m_particles;
        m_particles = 0;
    }
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

/**
  * You need to fill this in.
  *
  * Resets the particle at the given index to its initial state. Should reset the
  * position, direction, force, color, life, and decay of the particle.
  */
void ParticleEmitter::resetParticle(unsigned i)
{
    Particle &particle = m_particles[i];

    float x = rand() % 700 - 350;
    float y = rand() % 700 - 350;
    float z = rand() % 700 - 350;
//    std::cout << x << " " << y << " " << z << std::endl;
    particle.pos = glm::vec3(x,y,z);//glm::vec3(0.0f);
    particle.life = 1.0f;
    particle.decay = urand(0.00025f, 0.015f); // I multiplied these by 0.1
    particle.color = glm::vec3(0.9f, 0.8f, 0.95f);//m_color;

    float fx = urand(-m_fuzziness*0.01f, m_fuzziness*0.01f) + m_force.x;
    float fy = urand(-m_fuzziness*0.01f, m_fuzziness*0.01f) + m_force.y;
    float fz = urand(-m_fuzziness*0.01f, m_fuzziness*0.01f) + m_force.z;
    particle.force = glm::vec3(0.0f);//glm::vec3(fx, fy, fz);
//    std::cout << urand(0.0f, 1.0f) << std::endl;
    int shooting_star = 0;
    if (urand(0.0f, 1.0f) > 0.9) {
        shooting_star = 1;
        particle.color = glm::vec3(0.9f, 0.6f, 0.6f);//m_color;
    }
    float vx = urand(-m_fuzziness, m_fuzziness)* shooting_star + m_velocity.x * shooting_star;
    float vy = urand(-m_fuzziness, m_fuzziness)* shooting_star + m_velocity.y * shooting_star;
    float vz = urand(-m_fuzziness, m_fuzziness)* shooting_star + m_velocity.z * shooting_star;
    particle.dir = glm::vec3(vx, vy, vz);


    // Continue resetting the particle here...
}

/**
  * Resets all particles in this emitter to their initial states
  */
void ParticleEmitter::resetParticles()
{
    for (int i = 0; i < m_maxParticles; i++)
        resetParticle(i);
}

/**
  * You need to fill this in.
  *
  * Performs one step of the particle simulation. Should perform all physics
  * calculations and maintain the life property of each particle.
  */
void ParticleEmitter::updateParticles()
{
    for(int i = 0; i < m_maxParticles; ++i)
    {
        Particle &particle = m_particles[i];

        // Update particle here.
        particle.pos = particle.pos + (particle.dir * m_speed); // Life may not be this easy ...
        particle.dir += particle.force;
        particle.life -= particle.decay;

        if (particle.life < 0) // If its life is up
            resetParticle(i);

    }
}


/**
 * You need to fill this in!
 *
 * Draws each of the particles by making a call to glDrawArrays()
 * for each particle with a different ModelViewProjection matrix.
 */
void ParticleEmitter::drawParticlesVAO(Transforms transform, GLuint mvpLocation, GLuint colorLocation){
    if (!m_isInitialized){
        std::cout << "You must call initGL() before you can draw!" << std::endl;
    } else{
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);

        // Bind the VAO
        glBindVertexArray(m_vao);

        // @TODO: Render each particle
        for(int i = 0; i < m_maxParticles; ++i)
        {
            Particle &particle = m_particles[i];

            glm::mat4 targetModelMat = glm::mat4(1.0f); // Transform this matrix!
            targetModelMat = glm::translate(glm::mat4(1.0f), particle.pos) * targetModelMat;

            transform.model = targetModelMat;
            glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(transform.getTransform()));
            float r = particle.color.x;
            float g = particle.color.y;
            float b = particle.color.z;
            float a = 2.0f*sqrt(particle.life);

            glUniform4fv(colorLocation, 1, glm::value_ptr(glm::vec4(r,g,b,a)));

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        // Unbind the VAO
        glBindVertexArray(0);
        glAccum(GL_MULT,0.5);
        glAccum(GL_ACCUM,1);
        glAccum(GL_RETURN,1);
    }
}

/**
 * You need to fill this in!
 *
 * Sets up a single square that will be drawn (number of Particles) times in different positions
 */
void ParticleEmitter::initGL(int vertexLocation, int textureLocation){

    glClear(GL_ACCUM_BUFFER_BIT);

    // Generate and bind VAO and VBO.
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    // @TODO: Fill in the buffer with data for the vertex positions and texture coordinates.
    float v = m_scale / 2.0f;
    GLfloat vertexBufferData[] = {
        -v, v, 0,
         0, 0,
         v, -v, 0,
         1, 1,
         v, v, 0,
         1, 0,
        -v, v, 0,
         0, 0,
        -v, -v, 0,
         0, 1,
         v, -v, 0,
         1, 1
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*30, vertexBufferData, GL_STATIC_DRAW);

    // Expose vertices to shader
    glEnableVertexAttribArray(vertexLocation);
    glVertexAttribPointer(
       vertexLocation,
       3,                    // num vertices per element (3 for triangle)
       GL_FLOAT,             // type
       GL_FALSE,             // normalized?
       5*sizeof(GLfloat),  // stride
       (void*)0              // array buffer offset
    );

    glEnableVertexAttribArray(textureLocation);
    glVertexAttribPointer(
       textureLocation,
       2,                           // num vertices per element (3 for triangle)
       GL_FLOAT,                    // type
       GL_TRUE,                     // normalized?
       5*sizeof(GLfloat),         // stride
       (void*) (3*sizeof(GLfloat))    // array buffer offset
    );

    // Clean up -- unbind things.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_isInitialized = true;
}
