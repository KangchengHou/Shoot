#ifndef BOX_H
#define BOX_H

#include "gameBodyBase.h"
#include "shader.h"
#include "Camera.h"
// #include "game.h"

class Box : public GameBodyBase{
public:
    GLuint containerVAO;
    
    Box(glm::vec3 position, glm::vec3 size);
    ~Box();
    void initRenderData();
    void render( glm::vec3 color, glm::vec3 lightPos, GLuint gameWidth, GLuint gameHeight, Camera& camera, Shader shader);
};

#endif