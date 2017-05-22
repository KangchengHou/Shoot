#include "gameBodyBase.h"
GameBodyBase::GameBodyBase(glm::vec3 position, glm::vec3 size, glm::vec3 color)
    : position(position), size(size), acceleration(glm::vec3(0., 0., 0.)), speed(glm::vec3(0., 0., 0.)), color(color)
{}
GameBodyBase::~GameBodyBase()
{
}
void GameBodyBase::updateVectors()
{
    // Calculate the new Front vector
    glm::vec3 tmp;
    tmp.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    tmp.y = sin(glm::radians(this->pitch));
    tmp.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    this->front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    this->right = glm::normalize(glm::cross(this->front, this->worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    this->up    = glm::normalize(glm::cross(this->right, this->front));
}
void GameBodyBase::rotate(GLfloat a, glm::vec3 axis)
{
    glm::mat4 model;
    position = glm::vec3(glm::rotate(model, a, axis) 
                * glm::vec4(position,1.0f)); 
}
void GameBodyBase::setSpeed(glm::vec3 newSpeed) {
    speed = newSpeed;
}
void GameBodyBase::setAcceleration(glm::vec3 newAcceleration) {
    acceleration = newAcceleration;
}
void GameBodyBase::addAcceleration(glm::vec3 dltAcceleration) {
    acceleration += dltAcceleration;
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
void GameBodyBase::initRenderData() {}
void GameBodyBase::render( glm::vec3 color, glm::vec3 lightPos, GLuint gameWidth, GLuint gameHeight, Camera& camera, Shader shader) {}