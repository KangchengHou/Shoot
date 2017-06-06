#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstring>
#include <GL/glew.h> // Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
bool loadOBJ(
    const char * path,
    std::vector<glm::vec3> & out_vertices,  
        std::vector<glm::vec2> & out_uvs,  
        std::vector<glm::vec3> & out_normals
);

#endif