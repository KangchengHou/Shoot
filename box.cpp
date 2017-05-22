#include "box.h"

Box::Box(glm::vec3 position, glm::vec3 size)
    : position(position), size(size), acceleration(glm::vec3(0., 0., 0.)), speed(glm::vec3(0., 0., 0.))
{
    
}
Box::~Box()
{

}
void Box::rotate(GLfloat a, glm::vec3 axis)
{
    glm::mat4 model;
    position = glm::vec3(glm::rotate(model, a, axis) 
                * glm::vec4(position,1.0f)); 
}
void Box::updateSpeed(GLfloat dt)
{   
    speed[0] += dt * acceleration[0];
    speed[1] += dt * acceleration[1];
    speed[2] += dt * acceleration[2];
}
void Box::updatePos(GLfloat dt)
{
    position[0] += dt * speed[0];
    position[1] += dt * speed[1];
    position[2] += dt * speed[2];
}