#ifndef CONE_H
#define CONE_H

#include "GL/glew.h"
#include "scenegraph/OpenGLScene.h"
#include "Shape.h"

class Cone : public Shape
{
public:
    //Uninitialized Cone
    Cone();

    //Initialized Cone
    Cone(const int paramOne, const int paramTwo, const int paramThree, const GLuint vertexLocation, const GLuint normalLocation, NormalRenderer *normRender);

    virtual ~Cone();

    // Initialize Cone: tessellate triangles into base and sloped sides
    virtual void init(const GLuint vertexLocation, const GLuint normalLocation);

    static float intersect(glm::vec3 eye, glm::vec3 dir);
    static glm::vec3 normal(glm::vec3 eye, glm::vec3 dir, float t);
    static glm::vec2 textureCoordinates(glm::vec3 eye, glm::vec3 dir, float t);

private:
    // Parameters 2 and 1 respectively
    int m_numSlices;
    int m_numCuts;

};

#endif // CONE_H
