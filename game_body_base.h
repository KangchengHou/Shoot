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
#include "physics/q3.h"
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
    OTHER
};

// Default player values
const GLfloat PITCH = 0.0f;
const GLfloat YAW = -90.0f;
const GLfloat ROLL = 0.0f;
const GLfloat SPEED = 5.0f;
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
        // this->position = position - front * 2.0f + up * 1.0f;
        this->position = position + glm::vec3(0.0f, 0.0f, 3.0f) + glm::vec3(0.0f, 3.0f, 0.0f);
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
    q3Body* body;
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
    GLfloat pitch; // x
    GLfloat yaw;   // y
    GLfloat roll;  // z
    glm::mat3 rotationMatrix;
    GLfloat selfyaw;
    GLfloat selfpitch;
    GLboolean stuck[6];
    // GameBodyBase options
    GLfloat MovementSpeed;
    GLfloat MouseSensitivity;
    GLfloat zoom;
    GameBodyBase(OBJECTTYPE type,
                 glm::vec3 position,
                 glm::vec3 size,
                 GLfloat pitch = PITCH,
                 GLfloat yaw = YAW,
                 GLfloat roll = ROLL,
                 glm::vec3 color = glm::vec3(1, 1, 1),
                 glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                 glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f),
                 GLfloat selfyaw = 89.0f,
                 GLfloat selfpitch = 0.0f)
        : type(type),
          position(position),
          size(size),
          acceleration(glm::vec3(0., 0., 0.)),
          speed(glm::vec3(0., 0., 0.)),
          color(color),
          front(front),
          up(up),
          selfyaw(selfyaw),
          selfpitch(selfpitch),
          MovementSpeed(SPEED),
          MouseSensitivity(SENSITIVTY),
          zoom(ZOOM)
    {
        this->position = position;
        // this->worldUp = up;
        this->worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        this->yaw = yaw;
        this->roll = roll;
        this->pitch = pitch;
        this->rotationMatrix = euler2matrix(pitch, yaw, roll);
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
    glm::mat3 euler2matrix(double pitch, double yaw, double roll)
    {
        // Calculate rotation about x axis
        glm::mat3 R_x(
                1,       0,              0,
                0,       cos(pitch),   -sin(pitch),
                0,       sin(pitch),   cos(pitch)
                );
        // Calculate rotation about y axis
        glm::mat3  R_y(
                cos(yaw),    0,      sin(yaw),
                0,            1,      0,
                -sin(yaw),   0,      cos(yaw)
                );
        // Calculate rotation about z axis
        glm::mat3  R_z (
                cos(roll),    -sin(roll),      0,
                sin(roll),    cos(roll),       0,
                0,             0,                1);
        
        // Combined rotation matrix
        glm::mat3 R = R_z * R_y * R_x;
        return R;
    }
    // Constructor with vectors

    // Constructor with scalar values
    GameBodyBase(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), zoom(ZOOM)
    {
        this->position = glm::vec3(posX, posY, posZ);
        this->worldUp = glm::vec3(upX, upY, upZ);
        this->yaw = yaw;
        this->roll = 0;
        this->pitch = pitch;
        this->rotationMatrix = euler2matrix(pitch, yaw, roll);
        this->updateGameBodyBaseVectors();
        for (int i = 0; i < 6; i++)
            this->stuck[i] = GL_FALSE;
    }

    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of player defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(GameBodyBase_Movement direction, GLfloat deltaTime, GLbyte mode)
    {
        // printf("type %d\n", type);
        if (type == PLAYER)
        {
            GLfloat velocity = this->MovementSpeed * deltaTime;
            glm::vec3 frontProject = this->front - glm::dot(this->front, this->worldUp) * this->worldUp;
            glm::vec3 rightProject = this->right - glm::dot(this->right, this->worldUp) * this->worldUp;
            if (mode == 1)
            {
                GLfloat y = glm::dot(this->speed, this->worldUp);
                this->speed -= y * this->worldUp;
                if (direction == FORWARD)
                    this->speed += frontProject * this->MovementSpeed;
                if (direction == BACKWARD)
                    this->speed -= frontProject * this->MovementSpeed;
                if (direction == LEFT)
                    this->speed -= rightProject * this->MovementSpeed;
                if (direction == RIGHT)
                    this->speed += rightProject * this->MovementSpeed;
                if (glm::length(this->speed) > 1e-5)
                    this->speed = glm::normalize(this->speed) * this->MovementSpeed;
                this->speed += y * this->worldUp;
                // this->position += this->speed * deltaTime;
                // printf("position  %.2lf %.2lf %.2lf\n", position[0], position[1], position[2]);
            }
            else if (mode == 3)
            {
                // this->speed[0] = this->speed[2] = 0.f;
                if (direction == FORWARD  && glm::dot(this->speed, frontProject) > 0)
                    this->speed -= frontProject * glm::dot(this->speed, frontProject);
                if (direction == BACKWARD && glm::dot(this->speed, frontProject) < 0)
                    this->speed -= frontProject * glm::dot(this->speed, frontProject);
                if (direction == RIGHT && glm::dot(this->speed, rightProject) > 0)
                    this->speed -= rightProject * glm::dot(this->speed, rightProject);
                if (direction == LEFT  && glm::dot(this->speed, rightProject) < 0)
                    this->speed -= rightProject * glm::dot(this->speed, rightProject);
                GLfloat y = glm::dot(this->speed, this->up);
                this->speed -= y * this->up;
                if (glm::length(this->speed) > 1e-5)
                    this->speed = glm::normalize(this->speed) * this->MovementSpeed;
                this->speed += y * this->up;
                // printf("speed  %.2lf %.2lf %.2lf\n", speed[0], speed[1], speed[2]);
            }
        }
        else if (type == ROCKET && mode == 1)
        {
            if (direction == FORWARD) {
                // W
                this->selfpitch += 5.0f;
            }
            if (direction == BACKWARD) {
                this->selfpitch -= 5.0f;
            }
            if (direction == RIGHT) {
                this->selfyaw -= 5.0f;
            }
            if (direction == LEFT) {
                this->selfyaw += 5.0f;
            }

            this->front.x = cos(glm::radians(selfpitch)) * cos(glm::radians(selfyaw));
            this->front.z = cos(glm::radians(selfpitch)) * sin(glm::radians(selfyaw));
            this->front.y = sin(glm::radians(selfpitch));
            std::cout << "speed : " << this->speed.y << std::endl;
            // GLfloat y = glm::dot(this->speed, this->up);
            if (glm::length(this->speed) > 1e-5){
                this->speed = glm::normalize(this->front) * this->MovementSpeed;

            }
            // this->front = this->speed / this->MovementSpeed;
            this->right = glm::normalize(glm::cross(this->front, this->up));
            this->up = glm::normalize(glm::cross(this->right, this->front));
        }
    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= this->MouseSensitivity;
        yoffset *= this->MouseSensitivity;

        this->yaw += xoffset;
        this->pitch += yoffset;
        // std::cout << "yaw : " << this->yaw << " pitch : " << this->pitch << std::endl;
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
    void setPos(double x, double y, double z)
    {
        position.x = x;
        position.y = y;
        position.z = z;
        this->camera.updatePos(this->position, this->front, this->up);
    }
    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis

private:
    // Calculates the front vector from the GameBodyBase's (updated) Eular Angles
    void updateGameBodyBaseVectors()
    {
        // Calculate the new front vector
        // printf("front %.2lf %.2lf %.2lf\n", this->front.x, this->front.y, this->front.z);
        this->rotationMatrix = euler2matrix(pitch, yaw, roll);
        camera.updateCameraVectors(this->yaw, this->pitch, this->worldUp);
        // glm::vec3 front;
        // front.x = cos(glm::radians(this->yaw));
        // front.y = sin(glm::radians(0.f));
        // front.z = sin(glm::radians(this->yaw));
        this->front = glm::vec3(0, 0, -1) * this->rotationMatrix;
        this->right = glm::vec3(1, 0,  0) * this->rotationMatrix;
        this->up    = glm::vec3(0, 1,  0) * this->rotationMatrix;
        // this->front = glm::normalize(front);
        // // Also re-calculate the right and up vector
        // this->right = glm::normalize(glm::cross(this->front, this->worldUp)); // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        // this->up = glm::normalize(glm::cross(this->right, this->front));
    }
};

#endif