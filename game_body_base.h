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
#include "particle_generator.h"
#include "resource_manager.h"
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
    OTHER,
    CANNON,
    CANNONBULLET,
    LASER,
    BUILDING,
};

// Default player values
const GLfloat YAW = 90.0f;
const GLfloat PITCH = -45.0f;
const GLfloat YAWACC = 0.5f;
const GLfloat PITCHACC = 0.5f;
const GLfloat ROLL = 0.0f;
const GLfloat SPEED = 5.0f;
const GLfloat SENSITIVTY = 0.25f;
const GLfloat ZOOM = 45.0f;
const GLfloat CAMERARADIUS = 10.0f;
const glm::vec3 WORLDUP = glm::vec3(0.0f, 1.0f, 0.0f);

const double INITIAL_LIFE = 10000000.0;

class Camera
{
public:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;

    GLfloat zoom = 1;
    GLfloat MouseSensitivity;

    // 照相机在以被拍摄物体为中心的球上
    GLfloat cameraposyaw;
    GLfloat camerapospitch;
    GLfloat cameraposradius = 5.0f;

    Camera(glm::vec3 targetpos, glm::vec3 targetfront, GLfloat targetyaw, GLfloat targetpitch) {

        MouseSensitivity = SENSITIVTY;
        cameraposradius = CAMERARADIUS;
        updateSelfAnglesAccordingToTargetAngles(targetyaw, targetpitch);
        updateBaseVectorsAccordingToSelfAngles();
        updateTargetPosition(targetpos);
        updateCameraPosition();
        // zoom = 1;
        // std::cout << "in cfun " << this->front.x << " " << this->front.y << " " << this->front.z << " " << std::endl;
    }
    void updateSelfAnglesAccordingToTargetAngles(GLfloat targetyaw, GLfloat targetpitch) {
        cameraposyaw = targetyaw - 180.0f;
        camerapospitch = -targetpitch;
    }
    void updateCameraPosition() {
        position = target - front * cameraposradius;
    }
    void updateTargetPosition(glm::vec3 targetpos) {
        target = targetpos;
    }
    void updateBaseVectorsAccordingToSelfAngles() {
        glm::vec3 to;
        to.x = cos(glm::radians(camerapospitch)) * cos(glm::radians(cameraposyaw));
        to.y = sin(glm::radians(camerapospitch));
        to.z = -cos(glm::radians(camerapospitch)) * sin(glm::radians(cameraposyaw));

        this->front = glm::normalize(-to);
        this->right = glm::normalize(glm::cross(this->front, WORLDUP)); // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        this->up = glm::normalize(glm::cross(this->right, this->front));
    }

    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) {
        xoffset *= this->MouseSensitivity;
        yoffset *= this->MouseSensitivity;

        this->cameraposyaw += xoffset;
        this->camerapospitch += yoffset;


        this->updateBaseVectorsAccordingToSelfAngles();
        this->updateCameraPosition();
    }

    glm::mat4 GetViewMatrix() const
    {
        // std::cout << "in getviewmatrix front " << this->front.x << " " << this->front.y << " " << this->front.z << " " << std::endl;
        // std::cout << "in getviewmatrix position " << this->position.x << " " << this->position.y << " " << this->position.z << " " << std::endl;

        return glm::lookAt(this->position, this->target, this->up);
    }

};
// An abstract player class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class GameBodyBase
{
public:
    std::string renderType;
    q3Body* body = NULL;
    // GameBodyBase Attributes
    OBJECTTYPE type; 
    GLfloat scale;
    glm::vec3 color;
    glm::vec3 offset = glm::vec3(0, 0, 0);
    double life = INITIAL_LIFE;
    double rest_life = INITIAL_LIFE;

    glm::vec3 acceleration;
    glm::vec3 speed;
    glm::vec3 position;

    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    bool visible;

    Camera camera;
    ParticleGenerator* particleGenerator = NULL;
    // Eular Angles
    GLfloat pitch; // x
    GLfloat yaw;   // y
    GLfloat roll;  // z
    glm::mat3 rotationMatrix;

    // GLfloat renderyaw;
    // GLfloat renderpitch;
    // GLboolean synchro;

    // 用以产生火箭各种姿态renderyaw
    GLfloat yawspeed;
    GLfloat pitchspeed;
    const GLfloat yawacc = YAWACC;
    const GLfloat pitchacc = PITCHACC;

    const GLfloat MovementSpeed;

    GLfloat MouseSensitivity;

    GameBodyBase(OBJECTTYPE type,
                 glm::vec3 position,
                 GLfloat yaw,
                 GLfloat pitch,
                 GLfloat roll,

                 // GLfloat renderyaw,
                 // GLfloat renderpitch,
                 // GLboolean synchro,
                 GLfloat scale,
                 bool visible = true,
                 glm::vec3 color = glm::vec3(1, 1, 1)
                                   // ,GLfloat zoom = 1
                )
        : type(type),
          position(position),
          acceleration(glm::vec3(0., 0., 0.)),
          speed(glm::vec3(0., 0., 0.)),
          yaw(yaw),
          pitch(pitch),
          roll(roll),
          visible(visible),

          // renderyaw(renderyaw),
          // renderpitch(renderpitch),
          // synchro(synchro),

          scale(scale),
          color(color),
          MovementSpeed(SPEED),
          MouseSensitivity(SENSITIVTY),
          yawspeed(0.0f),
          yawacc(YAWACC),
          pitchspeed(0.0f),
          pitchacc(PITCHACC),

          // 下面会覆盖掉的 只是因为没有Camera()的构造函数
          camera(Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), 90.0f, 0.0f)),
          particleGenerator(NULL)
    {

        // camera.zoom = ZOOM;
        if(type == PLAYER)  renderType = "player";
        if(type == ROCKET)  renderType = "rocket";
        if(type == OTHER)   renderType = "cube";
        if(type == CANNON)  renderType = "cannon";
        if(type == CANNONBULLET) renderType = "sphere";
        if(type == LASER) renderType = "sphere";
        if(type == BUILDING) renderType = "building";
        this->rotationMatrix = euler2matrix(pitch, yaw, roll);
        this->updateBaseVectorsAccordingToSelfAngles();
        this->camera = Camera(position, front, yaw, pitch);
        if (type == LASER) {
            particleGenerator = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("laser"), 50 );
        }
        if (type == CANNONBULLET || type == ROCKET) {
            particleGenerator = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("flame"), 50 );
        }
        if(type == PLAYER) {
            this->camera.cameraposradius = 0.5f;
        }
    }
    ~GameBodyBase();

    // void rotate(GLfloat a, glm::vec3 axis);
    void setSpeed(glm::vec3 newSpeed);
    // void setSpeed(int d, GLfloat v);
    // void addSpeed(glm::vec3 dltSpeed);
    void setAcceleration(glm::vec3 newAcceleration);
    // void setAcceleration(int d, GLfloat a);
    // void addAcceleration(glm::vec3 dltAcceleration);
    // void addPos(glm::vec3 dltPos);
    // void initRenderData();
    // void render(glm::vec3 color, glm::vec3 lightPos, GLuint gameWidth, GLuint gameHeight, const GameBodyBase &player, Shader shader);
    // void updateVectors();
    // // Constructor with vectors
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

    void setPos(double x, double y, double z)
    {
        position.x = x;
        position.y = y;
        position.z = z;


        this->camera.updateTargetPosition(position);

        this->camera.updateCameraPosition();

    }

    void ProcessKeyboard(GameBodyBase_Movement direction, GLfloat deltaTime, GLboolean pressed)
    {
        // printf("type %d\n", type);
        if (type == PLAYER)
        {


            if (direction == FORWARD)
                setSpeed(this->speed + glm::vec3(front.x, 0, front.z) * this->MovementSpeed);
            if (direction == BACKWARD)
                setSpeed(this->speed - glm::vec3(front.x, 0, front.z) * this->MovementSpeed);
            if (direction == LEFT)
                setSpeed(this->speed - glm::vec3(right.x, 0, right.z) * this->MovementSpeed);
            if (direction == RIGHT)
                setSpeed(this->speed + glm::vec3(right.x, 0, right.z) * this->MovementSpeed);


        }
        else if (type == ROCKET)
        {
            if (direction == FORWARD) {
                this->pitchspeed -= this->pitchacc;
            }
            if (direction == BACKWARD) {
                this->pitchspeed += this->pitchacc;
            }
            if (direction == RIGHT) {
                this->yawspeed += this->yawacc;
            }
            if (direction == LEFT) {
                this->yawspeed -= this->yawacc;
            }

        }

    }

    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= this->MouseSensitivity;
        yoffset *= this->MouseSensitivity;

        this->yaw -= xoffset;
        this->pitch += yoffset;

        if (constrainPitch) {
            if (pitch > 89.0f ) pitch = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;
        }

        // Update front, right and up Vectors using the updated Eular angles
        this->updateBaseVectorsAccordingToSelfAngles();
        this->camera.updateSelfAnglesAccordingToTargetAngles(this->yaw, this->pitch);
        this->camera.updateBaseVectorsAccordingToSelfAngles();
        // this->camera.updateCameraPosition();
        this->camera.updateTargetPosition(this->camera.position + this->camera.front);
    }

// private:
    // Calculates the front vector from the GameBodyBase's (updated) Eular Angles
    void updateBaseVectorsAccordingToSelfAngles()
    {
        // Calculate the new front vector
        // camera.updateCameraVectors(this->yaw, this->pitch, this->worldUp);
        glm::vec3 f;
        f.x = cos(glm::radians(this->pitch)) * cos(glm::radians(this->yaw));
        f.y = sin(glm::radians(this->pitch));
        f.z = -cos(glm::radians(this->pitch)) * sin(glm::radians(this->yaw));
        this->front = glm::normalize(f);
        // Also re-calculate the right and up vector
        this->right = glm::normalize(glm::cross(this->front, WORLDUP)); // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        this->up = glm::normalize(glm::cross(this->right, this->front));
    }




};

#endif