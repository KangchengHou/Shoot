#ifndef GameBodyBase_H
#define GameBodyBase_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include "shader.h"

// Defines several possible options for player movement. Used as abstraction to stay away from window-system specific input methods
enum GameBodyBase_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

enum OBJECTTYPE
{
    PLAYER,
    ROCKET,
    GOD
};

// Default player values
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 3.0f;
const GLfloat SENSITIVTY = 0.25f;
const GLfloat ZOOM = 45.0f;

class Camera
{
  public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    GLfloat zoom;
    void updatePos(glm::vec3 position, glm::vec3 front, glm::vec3 up)
    {
        this->position = position - front * 2.0f + up * 1.0f;
    }
    void updateCameraVectors(GLfloat yaw, GLfloat pitch, glm::vec3 worldUp)
    {
        // Calculate the new front vector
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        this->front = glm::normalize(front);
        // Also re-calculate the right and up vector
        this->right = glm::normalize(glm::cross(this->front, worldUp)); // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        this->up = glm::normalize(glm::cross(this->right, this->front));
    }
    void ProcessMouseScroll(GLfloat yoffset)
    {
        if (this->zoom >= 1.0f && this->zoom <= 45.0f)
            this->zoom -= yoffset;
        if (this->zoom <= 1.0f)
            this->zoom = 1.0f;
        if (this->zoom >= 45.0f)
            this->zoom = 45.0f;
    }
    glm::mat4 GetViewMatrix() const
    {
        return glm::lookAt(this->position, this->position + this->front, this->up);
    }
};
// An abstract player class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class GameBodyBase
{
  public:
    // GameBodyBase Attributes
    OBJECTTYPE type;
    glm::vec3 acceleration;
    glm::vec3 speed;
    glm::vec3 position;
    glm::vec3 size;
    glm::vec3 color;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    Camera camera;
    // Eular Angles
    GLfloat yaw;
    GLfloat pitch;
    GLboolean stuck[6];
    // GameBodyBase options
    GLfloat MovementSpeed;
    GLfloat MouseSensitivity;
    GLfloat zoom;
    GameBodyBase(OBJECTTYPE type,
                 glm::vec3 position,
                 glm::vec3 size,
                 glm::vec3 color = glm::vec3(1, 1, 1),
                 glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                 GLfloat yaw = YAW,
                 GLfloat pitch = PITCH)
        : type(type),
          position(position),
          size(size),
          acceleration(glm::vec3(0., 0., 0.)),
          speed(glm::vec3(0., 0., 0.)),
          color(color),
          front(glm::vec3(0.0f, 0.0f, -1.0f)),
          MovementSpeed(SPEED),
          MouseSensitivity(SENSITIVTY),
          zoom(ZOOM)
    {
        this->position = position;
        this->worldUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        this->camera.updatePos(position, this->front, this->up);
        this->camera.zoom = ZOOM;
        this->updateGameBodyBaseVectors();
    }
    ~GameBodyBase();

    void rotate(GLfloat a, glm::vec3 axis);
    void setSpeed(glm::vec3 newSpeed);
    void setSpeed(int d, GLfloat v);
    void addSpeed(glm::vec3 dltSpeed);
    void setAcceleration(glm::vec3 newAcceleration);
    void setAcceleration(int d, GLfloat a);
    void addAcceleration(glm::vec3 dltAcceleration);
    void updateSpeed(GLfloat dt);
    void updatePos(GLfloat dt);
    void addPos(glm::vec3 dltPos);
    void initRenderData();
    void render(glm::vec3 color, glm::vec3 lightPos, GLuint gameWidth, GLuint gameHeight, const GameBodyBase &player, Shader shader);
    void updateVectors();
    // Constructor with vectors

    // Constructor with scalar values
    GameBodyBase(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), zoom(ZOOM)
    {
        this->position = glm::vec3(posX, posY, posZ);
        this->worldUp = glm::vec3(upX, upY, upZ);
        this->yaw = yaw;
        this->pitch = pitch;
        this->updateGameBodyBaseVectors();
        for (int i = 0; i < 6; i++)
            this->stuck[i] = GL_FALSE;
    }

    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of player defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(GameBodyBase_Movement direction, GLfloat deltaTime, GLbyte mode)
    {
        // printf("mode %d\n", mode);
        if (type == PLAYER)
        {
            GLfloat velocity = this->MovementSpeed * deltaTime;
            if (mode == 1)
            {
                // printf("speed  %.2lf %.2lf %.2lf\n", speed[0], speed[1], speed[2]);
                // this->speed[0] = this->speed[2] = 0.f;
                GLfloat y = glm::dot(this->speed, this->up);
                this->speed -= y * this->up;
                if (direction == FORWARD)
                    this->speed += this->front * this->MovementSpeed;
                if (direction == BACKWARD)
                    this->speed -= this->front * this->MovementSpeed;
                if (direction == LEFT)
                    this->speed -= this->right * this->MovementSpeed;
                if (direction == RIGHT)
                    this->speed += this->right * this->MovementSpeed;
                if (glm::length(this->speed) > 1e-5)
                    this->speed = glm::normalize(this->speed) * this->MovementSpeed;
                this->speed += y * this->up;
                // printf("speed  %.2lf %.2lf %.2lf\n", speed[0], speed[1], speed[2]);
            }
            else if (mode == 3)
            {
                // this->speed[0] = this->speed[2] = 0.f;
                if (direction == FORWARD)
                    this->speed -= this->front * glm::dot(this->speed, this->front);
                if (direction == BACKWARD && glm::dot(this->speed, this->front) < 0)
                    this->speed -= this->front * glm::dot(this->speed, this->front);
                if (direction == RIGHT && glm::dot(this->speed, this->right) > 0)
                    this->speed -= this->right * glm::dot(this->speed, this->right);
                if (direction == LEFT && glm::dot(this->speed, this->right) < 0)
                    this->speed -= this->right * glm::dot(this->speed, this->right);
                GLfloat y = glm::dot(this->speed, this->up);
                this->speed -= y * this->up;
                if (glm::length(this->speed) > 1e-5)
                    this->speed = glm::normalize(this->speed) * this->MovementSpeed;
                this->speed += y * this->up;
                // printf("speed  %.2lf %.2lf %.2lf\n", speed[0], speed[1], speed[2]);
            }
        }
        else if (type == ROCKET)
        {
            if (direction == FORWARD)
                this->speed += this->up * 0.1f;
            if (direction == BACKWARD)
                this->speed -= this->up * 0.1f;
            if (direction == RIGHT)
                this->speed += this->right * 0.1f;
            if (direction == LEFT)
                this->speed -= this->right * 0.1f;
            GLfloat y = glm::dot(this->speed, this->up);
            if (glm::length(this->speed) > 1e-5)
                this->speed = glm::normalize(this->speed) * this->MovementSpeed;
            this->front = this->speed / this->MovementSpeed;
            this->right = glm::cross(this->front, this->up);
            this->up = glm::cross(this->right, this->front);
        }
    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= this->MouseSensitivity;
        yoffset *= this->MouseSensitivity;

        this->yaw += xoffset;
        this->pitch += yoffset;
        std::cout << "yaw : " << this->yaw << " pitch : " << this->pitch << std::endl;
        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (this->pitch > 89.0f)
                this->pitch = 89.0f;
            if (this->pitch < -89.0f)
                this->pitch = -89.0f;
        }
        
        // Update front, right and up Vectors using the updated Eular angles
        this->updateGameBodyBaseVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis

  private:
    // Calculates the front vector from the GameBodyBase's (updated) Eular Angles
    void updateGameBodyBaseVectors()
    {
        // Calculate the new front vector
        camera.updateCameraVectors(this->yaw, this->pitch, this->worldUp);
        glm::vec3 front;
        front.x = cos(glm::radians(this->yaw));
        front.y = sin(glm::radians(0.f));
        front.z = sin(glm::radians(this->yaw));
        this->front = glm::normalize(front);
        // Also re-calculate the right and up vector
        this->right = glm::normalize(glm::cross(this->front, this->worldUp)); // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        this->up = glm::normalize(glm::cross(this->right, this->front));
    }
};

#endif