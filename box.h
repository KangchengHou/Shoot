#ifndef BOX_H
#define BOX_H

#include "gameBodyBase.h"
#include "shader.h"
#include "Camera.h"
// #include "game.h"

class Box : public GameBodyBase{
public:
    GLuint containerVAO;
    
    Box(glm::vec3 position, glm::vec3 size, glm::vec3 color = glm::vec3(1, 1, 1));
    ~Box();
<<<<<<< HEAD
    virtual void initRenderData();
    virtual void render( glm::vec3 color, glm::vec3 lightPos, GLuint gameWidth, GLuint gameHeight, Camera& camera, Shader shader);
=======
    void initRenderData();
    void render( glm::vec3 color, glm::vec3 lightPos, GLuint gameWidth, GLuint gameHeight, Camera& camera, Shader shader);
>>>>>>> 6b8a28ebb9b34317e5f7904634122836c7b1f208
};

#endif