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
const GLfloat YAW = 90.0f;
const GLfloat PITCH = -45.0f;
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

    // 照相机在以被拍摄物体为中心的球上
    GLfloat cameraposyaw = -90.0f;
    GLfloat camerapospitch = 45.0f;
    GLfloat cameraposradius = 5.0f;

    Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f)) {
        cameraposyaw = -90.0f;
        camerapospitch = 45.0f;
        cameraposradius = 5.0f;
        front = glm::vec3(0.0f, 0.7f * cameraposradius, 0.7f * cameraposradius);
        right = glm::normalize(glm::cross(worldUp, this->front));
        up = glm::normalize(glm::cross( this->front, this->right));
        position = pos + front;
        zoom = 1;
        std::cout << "in cfun " << this->front.x << " " << this->front.y << " " << this->front.z << " " << std::endl;
    }
    void updatePos(glm::vec3 position, glm::vec3 front, glm::vec3 up)
    {
        // this->position = position - front * 2.0f + up * 1.0f;
        this->position = position + glm::vec3(0.0f, 0.0f, 3.0f) + glm::vec3(0.0f, 3.0f, 0.0f);
    }
    // 物体更新位置时会更新照相机位置 会根据是否按p进入其中一个函数
    void updatePos_point(glm::vec3 pos, glm::vec3 front, glm::vec3 up)
    {
        // this->position = position - front * 2.0f + up * 1.0f;
        this->position = pos + this->front;
    }
    void updatePos_track(glm::vec3 pos, glm::vec3 front, glm::vec3 up)
    {
        this->position = pos + glm::vec3(0.0f, 0.7f, 0.7f) * cameraposradius;
    }

    // 没按p时会进入这里
    void updateCameraVectors(GLfloat yaw, GLfloat pitch, glm::vec3 worldUp)
    {
        // Calculate the new front vector
        glm::vec3 f;
        f.x = cameraposradius * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        f.y = cameraposradius * sin(glm::radians(pitch));
        f.z = -cameraposradius * sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        // this->front = glm::normalize(f);
        this->front = (-f);
        // Also re-calculate the right and up vector
        this->right = glm::normalize(glm::cross(worldUp, this->front)); // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        this->up = glm::normalize(glm::cross( this->front, this->right));
    }

    glm::mat4 GetViewMatrix() const
    {
        std::cout << "in getviewmatrix front " << this->front.x << " " << this->front.y << " " << this->front.z << " " << std::endl;
        std::cout << "in getviewmatrix position " << this->position.x << " " << this->position.y << " " << this->position.z << " " << std::endl;

        return glm::lookAt(this->position, this->position - this->front, this->up);
    }

};
// An abstract player class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class GameBodyBase
{
public:
    q3Body* body = NULL;
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
    // 画火箭用的 画人的时候应该可以无视掉 其他的正常使用
    GLfloat selfyaw;
    GLfloat selfpitch;
    GLfloat selfyawspeed;
    GLfloat selfpitchspeed;
    const GLfloat selfyawacc = 0.5f;
    const GLfloat selfpitchacc = 0.5f;

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
                 GLfloat selfpitch = 0.0f
                )
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
          zoom(ZOOM),
          camera(Camera(position))
    {
        this->position = position;
        // this->worldUp = up;
        this->worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        this->yaw = yaw;
        this->roll = roll;
        this->pitch = pitch;
        this->rotationMatrix = euler2matrix(pitch, yaw, roll);

        this->selfyawspeed = 0.0f;
        // this->selfyawacc = 0.0f;

        this->selfpitchspeed = 0.0f;
        // this->selfpitchacc = 0.0f;
        // this->camera.updatePos(position, this->front, this->up);
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
    void addPos(glm::vec3 dltPos);
    void initRenderData();
    void render(glm::vec3 color, glm::vec3 lightPos, GLuint gameWidth, GLuint gameHeight, const GameBodyBase &player, Shader shader);
    void updateVectors();
    // Constructor with vectors
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
    void updateAngleAndAngleSpeed(GLfloat dt, GLboolean p_pressed) {
        updateAngleSpeed(dt);
        updateAngle(dt);
    }

    void updatePosAndPosSpeed(GLfloat dt, GLboolean p_pressed) {
        updatePosSpeed(dt);
        updatePos(dt, p_pressed);
    }
    void setPos(double x, double y, double z, GLboolean p_pressed)
    {
        position.x = x;
        position.y = y;
        position.z = z;
        // this->camera.updatePos(this->position, this->front, this->up);

        std::cout << "p pressed? : " << p_pressed << std::endl;
        if (p_pressed) {
            std::cout << this->position.x << " " << this->position.y << " " << this->position.z << std::endl;
            this->camera.updatePos_point(this->position, this->front, this->up);
        } else
            this->camera.updatePos_track(this->position, this->front, this->up);
    }
    // Constructor with scalar values
    GameBodyBase(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), zoom(ZOOM), camera(Camera(position))
    {
        this->position = glm::vec3(posX, posY, posZ);
        this->worldUp = glm::vec3(upX, upY, upZ);
        this->yaw = yaw;
        this->pitch = pitch;
        // this->updateGameBodyBaseVectors();
        for (int i = 0; i < 6; i++)
            this->stuck[i] = GL_FALSE;
    }

    // Returns the view matrix calculated using Eular Angles and the LookAt Matrix

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of player defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(GameBodyBase_Movement direction, GLfloat deltaTime)
    {
        // printf("type %d\n", type);
        if (type == PLAYER)
        {
            // GLfloat velocity = this->MovementSpeed * deltaTime;

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
            this->position += this->speed * deltaTime;

        }
        else if (type == ROCKET)
        {
            if (direction == FORWARD) {
                // std::cout << "forwardpitch: " << " ";
                this->selfpitchspeed -= this->selfpitchacc;
                // std::cout << this->selfpitchacc << std::endl;
            }
            if (direction == BACKWARD) {
                // std::cout << "backwardpitch: " << " ";
                this->selfpitchspeed += this->selfpitchacc;
                // std::cout << this->selfpitchacc << std::endl;
            }
            if (direction == RIGHT) {
                // std::cout << "rightyaw: " << " ";
                this->selfyawspeed += this->selfyawacc;
                // std::cout << this->selfyawspeed << std::endl;
            }
            if (direction == LEFT) {
                // std::cout << "leftyaw: " << " ";
                this->selfyawspeed -= this->selfyawacc;
                // std::cout << this->selfyawacc << std::endl;
            }

        }

    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    // 没按p时动鼠标进入这里
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= this->MouseSensitivity;
        yoffset *= this->MouseSensitivity;

        this->yaw -= xoffset;
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

    void ProcessMouseScroll(GLfloat yoffset)
    {
        // if (this->zoom >= 1.0f && this->zoom <= 45.0f)
        //     this->zoom -= yoffset;
        // if (this->zoom <= 1.0f)
        //     this->zoom = 1.0f;
        // if (this->zoom >= 45.0f)
        //     this->zoom = 45.0f;
        if (camera.cameraposradius >= 2.0f) {
            camera.cameraposradius -= yoffset;
            cameraUpdateRelativePosition2Object();
        }
    }

    // 按p时动鼠标进入这里
    void updateCameraPosition(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true) {
        xoffset *= this->MouseSensitivity;
        yoffset *= this->MouseSensitivity;

        camera.cameraposyaw += xoffset;
        camera.camerapospitch += yoffset;

        cameraUpdateRelativePosition2Object();
    }

    void cameraUpdateRelativePosition2Object() {
        camera.position.x = position.x + camera.cameraposradius * cos(glm::radians(camera.camerapospitch)) * cos(glm::radians(camera.cameraposyaw));
        camera.position.z = position.z - camera.cameraposradius * cos(glm::radians(camera.camerapospitch)) * sin(glm::radians(camera.cameraposyaw));
        camera.position.y = position.y + camera.cameraposradius * sin(glm::radians(camera.camerapospitch));

        camera.front = -(position - camera.position);
        camera.right = glm::normalize(glm::cross(worldUp, camera.front));
        camera.up = glm::normalize(glm::cross(camera.front, camera.right));
        // std::cout << "after set front " << camera.front.x << " " << camera.front.y << " " << camera.front.z << " " << std::endl;
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
    void updatePosSpeed(GLfloat dt);
    void updatePos(GLfloat dt,  GLboolean p_pressed);
    void updateAngleSpeed(GLfloat dt) {
        // 在处理input的时候已经做了

    }

    void updateAngle(GLfloat dt) {
        this->selfyaw += this->selfyawspeed * dt;
        this->selfpitch += this->selfpitchspeed * dt;

    }




};

#endif