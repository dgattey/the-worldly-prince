#ifndef FLOWER_H
#define FLOWER_H

#include "GL/glew.h"
#include "shape.h"
#include <list>

class Flower : public Shape
{
public:
    Flower(GLuint shader);
    ~Flower();

    void init();

    void render();

    bool needsUpdate();

private:

    GLfloat *m_vertexBufferData;
    GLuint m_vaoID;
    GLuint m_shader;
    int m_shapeCount;
    std::list<Shape *> m_shapes;
    glm::mat4x4 *m_transforms;

};
glm::mat4x4 translationMatrix(glm::vec3 vec);
glm::mat4x4 scaleMatrix(glm::vec3 vec);
glm::mat4x4 rotateMatrix(glm::vec3 vec, float angle);

#endif // FLOWER_H
