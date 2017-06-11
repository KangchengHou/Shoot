#include "game_body_base.h"
// GameBodyBase::GameBodyBase(glm::vec3 position, glm::vec3 size, glm::vec3 color)
//     : position(position), size(size), acceleration(glm::vec3(0., 0., 0.)), speed(glm::vec3(0., 0., 0.)), color(color)
// {}
GameBodyBase::~GameBodyBase()
{
    this->body->RemoveAllBoxes();
    if (particleGenerator) {
        delete particleGenerator;
    }
}

void GameBodyBase::setSpeed(glm::vec3 newSpeed) {
    speed = newSpeed;
    body->SetLinearVelocity(q3Vec3(speed.x, speed.y, speed.z));

}
void GameBodyBase::setAcceleration(glm::vec3 newAcceleration) {
    acceleration = newAcceleration;
}
