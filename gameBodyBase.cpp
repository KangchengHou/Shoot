#include "gameBodyBase.h"
#include "iostream"

GameBodyBase::GameBodyBase(GeometryBase* geometry) 
    : geometry(geometry)
{
    std::cout << "succeed in constructing gamebodybase" << std::endl;
}
GameBodyBase::~GameBodyBase() 
{
    std::cout << "succeed in deconstructing gamebodybase" << std::endl;
}

void GameBodyBase::setAcceleration(glm::vec3 a)
{
    acceleration = a;
}
glm::vec3 GameBodyBase::getAcceleration() 
{
    return acceleration;
}
void GameBodyBase::setSpeed(GLfloat dt)
{
    speed[0] += acceleration[0] * dt;
    speed[1] += acceleration[1] * dt;
    speed[2] += acceleration[2] * dt;
}
glm::vec3 GameBodyBase::getSpeed() 
{
    return speed;
}
void GameBodyBase::setPos(GLfloat dt)
{
    position[0] += speed[0] * dt;
    position[1] += speed[1] * dt;
    position[2] += speed[2] * dt;
}
glm::vec3 GameBodyBase::getPos() 
{
    return position;
}
