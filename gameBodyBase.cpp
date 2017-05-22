#include "gameBodyBase.h"

GameBodyBase::GameBodyBase(glm::vec3 position, glm::vec3 size)
    : position(position), size(size), acceleration(glm::vec3(0., 0., 0.)), speed(glm::vec3(0., 0., 0.))
{
    
}
GameBodyBase::~GameBodyBase()
{

}
void GameBodyBase::rotate(GLfloat a, glm::vec3 axis)
{
    glm::mat4 model;
    position = glm::vec3(glm::rotate(model, a, axis) 
                * glm::vec4(position,1.0f)); 
}
void GameBodyBase::updateSpeed(GLfloat dt)
{   
    glm::mat4 model;
    speed = glm::vec3(glm::translate(glm::vec3(glm::scale(model, glm::vec3(dt, dt, dt)) * glm::vec4(acceleration, 1.0f))) 
                * glm::vec4(speed, 1.0f));
}
void GameBodyBase::updatePos(GLfloat dt)
{
    glm::mat4 model;
    position = glm::vec3(glm::translate(glm::vec3(glm::scale(model, glm::vec3(dt, dt, dt)) * glm::vec4(speed, 1.0f))) 
                * glm::vec4(position, 1.0f));
}