#include "game_body_base.h"
// GameBodyBase::GameBodyBase(glm::vec3 position, glm::vec3 size, glm::vec3 color)
//     : position(position), size(size), acceleration(glm::vec3(0., 0., 0.)), speed(glm::vec3(0., 0., 0.)), color(color)
// {}
GameBodyBase::~GameBodyBase()
{
    //!!!!!!!!!!!!!!!!!❤️️️️️️
    this->body->RemoveAllBoxes();
}
// void GameBodyBase::updateVectors()
// {
//     // Calculate the new front vector
//     glm::vec3 tmp;
//     tmp.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
//     tmp.y = sin(glm::radians(this->pitch));
//     tmp.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
//     this->front = glm::normalize(front);
//     // Also re-calculate the right and up vector
//     this->right = glm::normalize(glm::cross(this->front, this->worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
//     this->up    = glm::normalize(glm::cross(this->right, this->front));
// }
// void GameBodyBase::rotate(GLfloat a, glm::vec3 axis)
// {
//     glm::mat4 model;
//     position = glm::vec3(glm::rotate(model, a, axis)
//                          * glm::vec4(position, 1.0f));
// }
// void GameBodyBase::addSpeed(glm::vec3 dltSpeed) {
//     speed += dltSpeed;
// }
// // void GameBodyBase::setSpeed(glm::vec3 newSpeed) {
// //     speed = newSpeed;
// // }
void GameBodyBase::setSpeed(glm::vec3 newSpeed) {
    speed = newSpeed;
    // std::cout << "?" << std::endl;

    body->SetLinearVelocity(q3Vec3(speed.x, speed.y, speed.z));
    // std::cout << "??" << std::endl;

    // puts("fuck");
}
void GameBodyBase::setAcceleration(glm::vec3 newAcceleration) {
    acceleration = newAcceleration;
}
// void GameBodyBase::setAcceleration(int d, GLfloat a) {
//     acceleration[d] = a;
// }
// void GameBodyBase::addAcceleration(glm::vec3 dltAcceleration) {
//     acceleration += dltAcceleration;
// }
// void GameBodyBase::updatePosSpeed(GLfloat dt)
// {


//     if (type == ROCKET) {
//         GLfloat x = cos(glm::radians(selfpitch)) * cos(glm::radians(selfyaw));
//         GLfloat y = sin(glm::radians(selfpitch));
//         GLfloat z = cos(glm::radians(selfpitch)) * sin(glm::radians(selfyaw));

//         this->speed = glm::vec3(x, y, z) * SPEED;
//     } else {
//         speed.x += acceleration.x * dt;
//         speed.y += acceleration.y * dt;
//         speed.z += acceleration.z * dt;

//     }

// }
// void GameBodyBase::updatePos(GLfloat dt,  GLboolean p_pressed)
// {
//     // std::cout << "before" << position.y << std::endl;
//     // std::cout << "speed" << speed.y << std::endl;
//     position.x += speed.x * dt;
//     position.y += speed.y * dt;
//     position.z += speed.z * dt;
//     // std::cout << "after" << position.y << std::endl;
//     if (p_pressed)
//         this->camera.updatePos_point(this->position, this->front, this->up);
//     else
//         this->camera.updatePos_track(this->position, this->front, this->up);


// }
// void GameBodyBase::addPos(glm::vec3 dltPos)
// {
//     position += dltPos;
// }
// void GameBodyBase::initRenderData() {}
// void GameBodyBase::render( glm::vec3 color, glm::vec3 lightPos, GLuint gameWidth, GLuint gameHeight, const GameBodyBase& player, Shader shader) {}