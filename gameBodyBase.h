#ifndef GAMEBODYBASE_H
#define GAMEBODYBASE_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GameBodyBase {

public:
    glm::vec3 acceleration;
    glm::vec3 speed;
    glm::vec3 position;
    glm::vec3 size;

    GameBodyBase(glm::vec3 position, glm::vec3 size);
    ~GameBodyBase();
    
    void rotate(GLfloat a, glm::vec3 axis);
    void updateSpeed(GLfloat dt);
    void updatePos(GLfloat dt);
    
};


#endif
