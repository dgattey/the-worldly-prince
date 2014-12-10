#ifndef SHADER_H
#define SHADER_H
#include <GL/glew.h>

class ResourceLoader
{
public:
    ResourceLoader();
    static GLuint loadShaders(const char * vertex_file_path,const char * fragment_file_path);
    static GLuint loadFragShader(const char * fragment_file_path);
};

#endif // SHADER_H
