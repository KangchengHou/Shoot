#include "game_body_base.h"
// GameBodyBase::GameBodyBase(glm::vec3 position, glm::vec3 size, glm::vec3 color)
//     : position(position), size(size), acceleration(glm::vec3(0., 0., 0.)), speed(glm::vec3(0., 0., 0.)), color(color)
// {}
GameBodyBase::~GameBodyBase()
{
}
void GameBodyBase::updateVectors()
{
    // Calculate the new front vector
    glm::vec3 tmp;
    tmp.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    tmp.y = sin(glm::radians(this->pitch));
    tmp.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    this->front = glm::normalize(front);
    // Also re-calculate the right and up vector
    this->right = glm::normalize(glm::cross(this->front, this->worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    this->up    = glm::normalize(glm::cross(this->right, this->front));
}
void GameBodyBase::rotate(GLfloat a, glm::vec3 axis)
{
    glm::mat4 model;
    position = glm::vec3(glm::rotate(model, a, axis) 
                * glm::vec4(position,1.0f)); 
}
void GameBodyBase::addSpeed(glm::vec3 dltSpeed) {
    speed += dltSpeed;
}
void GameBodyBase::setSpeed(glm::vec3 newSpeed) {
    speed = newSpeed;
}
void GameBodyBase::setSpeed(int d, GLfloat v) {
    speed[d] = v;
}
void GameBodyBase::setAcceleration(glm::vec3 newAcceleration) {
    acceleration = newAcceleration;
}
void GameBodyBase::setAcceleration(int d, GLfloat a) {
    acceleration[d] = a;
}
void GameBodyBase::addAcceleration(glm::vec3 dltAcceleration) {
    acceleration += dltAcceleration;
}
void GameBodyBase::updateSpeed(GLfloat dt)
{   
    GLfloat a[3];
    // printf("before  %.2lf %.2lf %.2lf\n", acceleration[0], acceleration[1], acceleration[2]);
    a[0] = glm::dot(front, acceleration);
    a[1] = glm::dot(up, acceleration);
    a[2] = glm::dot(right, acceleration);
    GLfloat v[3];
    v[0] = glm::dot(front, speed);
    v[1] = glm::dot(up, speed);
    v[2] = glm::dot(right, speed);
    // if(fabs(glm::dot(up, speed)) > 1e-5) {
    //     printf("a  %.2lf %.2lf %.2lf\n", a[0], a[1], a[2]);
    //     printf("v  %.2lf %.2lf %.2lf\n", v[0], v[1], v[2]);
    // }
    for(int j = 0; j < 3; j++)
        v[j] += a[j] * dt;

    for(int j = 0; j < 6; j++){
        if(stuck[j]) {
            if ( (v[j/2]>0) ^ (j ^ 1) )
                v[j/2] = 0;
            // setAcceleration(j/2, 0);
        }
    }
    speed = v[0] * front + v[1] * up + v[2] * right;
    // if(fabs(glm::dot(up, speed)) > 1e-5 && fabs(v[1] < 1e-5)) {
    //     printf("after  %.2lf %.2lf %.2lf\n", acceleration[0], acceleration[1], acceleration[2]);
    //     printf("speed  %.2lf %.2lf %.2lf\n", speed[0], speed[1], speed[2]);
    // }
    // printf("after  %.2lf %.2lf %.2lf\n", acceleration[0], acceleration[1], acceleration[2]);
    // printf("speed  %.2lf %.2lf %.2lf\n", speed[0], speed[1], speed[2]);
    
}
void GameBodyBase::updatePos(GLfloat dt)
{
    glm::mat4 model;
    // printf("uppos  %.2lf %.2lf %.2lf\n", speed[0], speed[1], speed[2]);
    position = glm::vec3(glm::translate(glm::vec3(glm::scale(model, glm::vec3(dt, dt, dt)) * glm::vec4(speed, 1.0f))) 
                * glm::vec4(position, 1.0f));
    this->camera.updatePos(this->position, this->front, this->up);
}
void GameBodyBase::addPos(glm::vec3 dltPos)
{
    position += dltPos;
}
void GameBodyBase::initRenderData() {}
void GameBodyBase::render( glm::vec3 color, glm::vec3 lightPos, GLuint gameWidth, GLuint gameHeight, const GameBodyBase& player, Shader shader) {}