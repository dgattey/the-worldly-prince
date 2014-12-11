#include "flower.h"
#include "shape.h"
#include "sphere.h"
#include "cylinder.h"
#include "gtc/type_ptr.hpp"
#include <iostream>

Flower::Flower(GLuint shader)
{
    m_shader = shader;

    init();
}

Flower::~Flower()
{
    if (m_vertexBufferData != NULL) {
        free(m_vertexBufferData);
    }
    for (std::list<Shape *>::iterator iterator = m_shapes.begin(), end = m_shapes.end(); iterator != end; ++iterator) {
        Shape *s = *iterator;
        delete s;
    }
}

void Flower::init()
{
    int petals = glm::min(5.0, floor(8 * rand()));
    m_shapeCount = petals + 2;
    m_transforms = new glm::mat4x4[m_shapeCount];
    glm::mat4x4 petalTransformation = translationMatrix(glm::vec3{0.75, 2, 0}) * scaleMatrix(glm::vec3{1.5, 0.01, 0.5});
    int i;
    for (i = 0; i < petals; i++) {
        m_shapes.push_back(new Sphere(glGetAttribLocation(m_shader, "position"), glGetAttribLocation(m_shader, "normal")));
        m_transforms[i] = petalTransformation * rotateMatrix(glm::vec3{0, 1, 0}, i * (360.f / (float)petals));
    }
    m_shapes.push_back(new Sphere(glGetAttribLocation(m_shader, "position"), glGetAttribLocation(m_shader, "normal")));
    m_transforms[i++] = translationMatrix(glm::vec3{0, 2, 0}) * scaleMatrix(glm::vec3{0.5, 0.01, 0.5});

    m_shapes.push_back(new Cylinder(glGetAttribLocation(m_shader, "position"), glGetAttribLocation(m_shader, "normal")));
    m_transforms[i++] = translationMatrix(glm::vec3{0, 0.5, 0}) * scaleMatrix(glm::vec3{0.2, 2.0, 0.2});
}

void Flower::render()
{
    int i = 0;
    for (std::list<Shape *>::const_iterator iterator = m_shapes.begin(), end = m_shapes.end(); iterator != end; ++iterator) {
        Shape *s = *iterator;
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "m"), 1, GL_FALSE, glm::value_ptr(glm::transpose(m_transforms[i++])));
        s->render();
    }
}

glm::mat4x4 translationMatrix(glm::vec3 trans)
{
    return glm::mat4x4{
        1.f, 0.f, 0.f, trans.x,
        0.f, 1.f, 0.f, trans.y,
        0.f, 0.f, 1.f, trans.z,
        0.f, 0.f, 0.f, 1
    };
}

glm::mat4x4 scaleMatrix(glm::vec3 scale)
{
    return glm::mat4x4{
        scale.x, 0.f, 0.f, 0.f,
        0.f, scale.y, 0.f, 0.f,
        0.f, 0.f, scale.z, 0.f,
        0.f, 0.f, 0.f, 1.f,
    };
}

glm::mat4x4 rotateMatrix(glm::vec3 rotate, float angle)
{
    glm::mat4x4 rotateMatrix = glm::mat4();
    bool isSet = false;
    if (rotate.x) {
        rotateMatrix = glm::mat4x4{
            1.f, 0.f, 0.f, 0.f,
            0.f, glm::cos(angle * rotate.x), -glm::sin(angle * rotate.x), 0.f,
            0.f, glm::sin(angle * rotate.x), glm::cos(angle * rotate.x), 0.f,
            0.f, 0.f, 0.f, 1.f,
        };
    }
    if (rotate.y) {
        glm::mat4x4 ymat = glm::mat4x4{
                glm::cos(angle * rotate.y), 0.f, glm::sin(angle * rotate.y), 0.f,
                0.f, 1.f, 0.f, 0.f,
                -glm::sin(angle * rotate.y), 0.f, glm::cos(angle * rotate.y), 0.f,
                0.f, 0.f, 0.f, 1.f,
            };
        if (isSet) {
            rotateMatrix = ymat * rotateMatrix;
        } else {
            rotateMatrix = ymat;
        }
    }
    if (rotate.z) {
        glm::mat4x4 zmat = glm::mat4x4{
            glm::cos(angle * rotate.z), -glm::sin(angle * rotate.z), 0.f, 0.f,
            glm::sin(angle * rotate.z), glm::cos(angle * rotate.z), 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f,
        };
        if (isSet) {
            rotateMatrix = zmat * rotateMatrix;
        } else {
            rotateMatrix = zmat;
        }
    }
    return rotateMatrix;
}
