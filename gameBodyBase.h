#ifndef GAMEBODYBASE_H
#define GAMEBODYBASE_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "Camera.h"
// Default camera values
class GameBodyBase {

public:
    glm::vec3 acceleration;
    glm::vec3 speed;
    glm::vec3 position;
    glm::vec3 size;
    glm::vec3 color;
    glm::vec3 angle;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    GLfloat yaw;
    GLfloat pitch;
    glm::vec3 worldUp;
    GameBodyBase(glm::vec3 position, glm::vec3 size, glm::vec3 color = glm::vec3(1, 1, 1));
    ~GameBodyBase();
    
    virtual void rotate(GLfloat a, glm::vec3 axis);
    virtual void setSpeed(glm::vec3 newSpeed);
    virtual void setAcceleration(glm::vec3 newAcceleration);
    virtual void addAcceleration(glm::vec3 dltAcceleration);
    virtual void updateSpeed(GLfloat dt);
    virtual void updatePos(GLfloat dt);
    virtual void initRenderData();
    virtual void render( glm::vec3 color, glm::vec3 lightPos, GLuint gameWidth, GLuint gameHeight, Camera& camera, Shader shader);
    virtual void updateVectors();
};


#endif
