#ifndef BOX_H
#define BOX_H

#include "game_body_base.h"
#include "shader.h"
// #include "game.h"

class Box : public GameBodyBase{
public:
    GLuint containerVAO;
    
    Box(glm::vec3 position, glm::vec3 size, glm::vec3 color = glm::vec3(1, 1, 1));
    ~Box();
    virtual void initRenderData();
    virtual void render( glm::vec3 color, glm::vec3 lightPos, GLuint gameWidth, GLuint gameHeight,const GameBodyBase& player, Shader shader);
};

#endif