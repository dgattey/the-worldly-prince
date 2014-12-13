#include "particle.h"
#include <iostream>

#define NUM_TRIS 2

Particle::Particle()
{
    m_isInitialized = false;
}

Particle::Particle(const GLuint vertexLocation, const GLuint textureLocation){
    init(vertexLocation, textureLocation);
}

void Particle::init(const GLuint vertexLocation, const GLuint textureLocation){
    m_isInitialized = true;

    float v = 1.0f;//m_scale / 2.0f;
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

    // VAO init
    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);

    // Vertex buffer init
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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


//    // Give our vertices to OpenGL.
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

//    // Expose vertices to shader
//    glEnableVertexAttribArray(vertexLocation);
//    glVertexAttribPointer(
//       vertexLocation,
//       3,                  // num vertices per element (3 for triangle)
//       GL_FLOAT,           // type
//       GL_FALSE,           // normalized?
//       0,                  // stride
//       (void*)0            // array buffer offset
//    );

//    glEnableVertexAttribArray(normalLocation);
//    glVertexAttribPointer(
//       normalLocation,
//       3,                  // num vertices per element (3 for triangle)
//       GL_FLOAT,           // type
//       GL_TRUE,           // normalized?
//       0,                  // stride
//       (void*)0            // array buffer offset
//    );

    //Clean up -- unbind things
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
}

void Particle::draw(){
    if (!m_isInitialized){
        std::cout << "You must call init() before you can draw!" << std::endl;
    } else{
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
//        glDepthMask(GL_FALSE);
//        glEnable(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        // Rebind your vertex array and draw the triangles
        glBindVertexArray(m_vaoID);
        glDrawArrays(GL_TRIANGLES, 0, NUM_TRIS*3);
        glBindVertexArray(0);

        glAccum(GL_MULT,0.5);
        glAccum(GL_ACCUM,1);
        glAccum(GL_RETURN,1);
    }

//    else{
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
//        glDepthMask(GL_FALSE);
//        glEnable(GL_BLEND);

//        // Bind the VAO
//        glBindVertexArray(m_vaoID);

//        // @TODO: Render each particle
//        for(int i = 0; i < m_maxParticles; ++i)
//        {
//            Particle &particle = m_particles[i];

//            glm::mat4 targetModelMat = glm::mat4(1.0f); // Transform this matrix!
//            targetModelMat = glm::translate(glm::mat4(1.0f), particle.pos) * targetModelMat;

//            transform.model = targetModelMat;
//            glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(transform.getTransform()));
//            float r = particle.color.x;
//            float g = particle.color.y;
//            float b = particle.color.z;
//            float a = 2.0f*sqrt(particle.life);

//            glUniform4fv(colorLocation, 1, glm::value_ptr(glm::vec4(r,g,b,a)));

//            glDrawArrays(GL_TRIANGLES, 0, 6);
//        }
//        // Unbind the VAO
//        glBindVertexArray(0);
//        glAccum(GL_MULT,0.5);
//        glAccum(GL_ACCUM,1);
//        glAccum(GL_RETURN,1);
//    }
}
