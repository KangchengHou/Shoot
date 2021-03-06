#ifndef GAME_H
#define GAME_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "game_body_base.h"
#include <list>
// #include "box.h"
#include "shader.h"
#include <map>
#include <string>
#include <vector>
#include "light.h"
#include "objloader.h"
#include "particle_generator.h"
#include "physics/q3.h"
#include "model.h"
#include "cannon.h"
#include "resource_manager.h"

#include <irrKlang/irrKlang.h>
using namespace irrklang;
#pragma comment(lib, "irrKalng.lib")

  


// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

struct explosionParticle {
    ParticleGenerator* particleSystem;
    GLfloat life;
    glm::vec3 position;

    explosionParticle(Shader shader, Texture2D texture, GLuint amount, GLfloat life, glm::vec3 pos) 
        :particleSystem(new ParticleGenerator(shader, texture, amount)), life(life),position(pos)
    {

    }
};
// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // Game state
    GameState              State;
    bool             Keys[1024];
    bool                   leftMouse;
    bool                   rightMouse;
    GLuint                 Width, Height;
    GLuint shadowWidth;
    GLuint shadowHeight;
    ISoundEngine *SoundEngine;
    GLFWwindow* window;
    GLfloat rendernear;
    GLuint diffuseMap;
    GLuint normalMap;
    GLfloat renderfar;
    GLfloat groundSize; // 地图大小
    // FIXME: 删除rocket
    // GameBodyBase* rocket;
    // FIXME: delete paticles
    // Box player;
    
    GLuint depthMapFBO; // FIXME:
    GLuint depthCubemap; // FIXME: 同上
    GLuint cubeVAO; // 同上
    GLuint cubeVBO; // 同上
    glm::vec3 Gravity = glm::vec3(0.f, -9.8f, 0.f);
    std::list<explosionParticle*> explosionParticles;
    GameBodyBase* room = NULL;
    GameBodyBase* boss = NULL;
    GameBodyBase* player = NULL;
    GameBodyBase* rocket = NULL;
    std::list<GameBodyBase*> objects;
    CannonGroup cannongroup;

    // ParticleGenerator* particles;
    glm::vec3 bulletColor = glm::vec3(1.0f, 1.0f, 1.0f);
    Shader bulletShader;
    // std::vector<GameBodyBase*> lights;
    std::vector<Light> lights;
    glm::vec3 lightColor = glm::vec3(0.f, 0.5f, 0.5f);
    glm::mat4 lightSpaceMatrix;
    //------------skybox-------------
    Shader lightShader;
    Shader skyboxShader;
    GLuint skyboxVAO, skyboxVBO;
    GLuint skyboxTexture;
    //------------skybox-----------
    // 新加的两个shader
    Shader depthShader;
    Shader shader;
    // TODO: modify lightPos need to be refined
    glm::vec3 lightPos;
    // Constructor/Destructor
    Game(GLuint width, GLuint height);
    ~Game();
    // Initialize game state (load all shaders/textures/levels)
    void Init();
    // GameLoop
    void ProcessInput(GLfloat dt);
    void Update(GLfloat dt);
    void Render();
    GLuint loadTexture(GLchar const * path);
    void depthRender();
    void initDepthMap();
    void RenderScene(); // FIXME: 这个函数不应该放在Game类当中
    void renderEnvironment();
    void addObjectType(std::string name);
    void renderObject(const std::string& name, Shader& sh, GameBodyBase* object);
    void ProcessMouseMovement(double xoffset, double yoffset);
    void registerCollisionBody(GameBodyBase *obj, bool rest = false, float gravityScale = 1.0);
    GLuint loadCubemap(std::vector<std::string> faces);
    GameBodyBase* addObject(OBJECTTYPE type,
                 glm::vec3 position,
                 GLfloat yaw,
                 GLfloat pitch,
                 GLfloat roll,
                 GLfloat scale,
                 bool visible = true,
                 bool rest = false,
                 GLfloat gravityScale = 1.0) {
        GameBodyBase *p =  new GameBodyBase(type, position, yaw, pitch, roll, scale, visible);
        if(ResourceManager::LoadedModels.count(p->renderType)) {
            float* b = ResourceManager::LoadedModels[p->renderType]->boundary;
            for(int i = 0; i < 3; i++) {
                p->offset[i] = (b[i*2+1] + b[i*2]) / 2;
            }
        }
        
        objects.push_back(p);
        registerCollisionBody(p, rest, gravityScale);
        return p;
    }
private:
    const double sceneDt = 1.0 / 60.0;
    q3Scene scene = q3Scene( sceneDt );
};

#endif