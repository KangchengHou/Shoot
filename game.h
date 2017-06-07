#ifndef GAME_H
#define GAME_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "game_body_base.h"
// #include "box.h"
#include "shader.h"
#include <map>
#include "light.h"
#include "objloader.h"
#include "particle_generator.h"
// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // Game state
    GameState              State;
    GLbyte              Keys[1024];
    GLbyte              mouse[5];
    GLuint                 Width, Height;
    GLuint shadowWidth;
    GLuint shadowHeight;
    GLFWwindow* window;
    GLfloat rendernear;
    GLfloat renderfar;

    // FIXME: 删除rocket
    // GameBodyBase* rocket;
    // FIXME: delete paticles
    // Box player;
    std::map<std::string, GLuint> VAOmap;
    std::map<std::string, int> modelSizeMap;
    GLuint depthMapFBO; // FIXME:
    GLuint depthCubemap; // FIXME: 同上
    GLuint cubeVAO; // 同上
    GLuint cubeVBO; // 同上
    glm::vec3 Gravity = glm::vec3(0.f, -9.8f, 0.f);

    GameBodyBase* room = NULL;
    GameBodyBase* boss = NULL;
    GameBodyBase* player = NULL;
    GameBodyBase* rocket = NULL;
    std::vector<GameBodyBase*> objects;
    std::vector<GameBodyBase*> bullets;
    std::vector<GameBodyBase*> players;
    std::vector<GameBodyBase*> rockets;
    ParticleGenerator* particles;
    glm::vec3 bulletColor = glm::vec3(1.0f, 1.0f, 1.0f);
    Shader bulletShader;
    // std::vector<GameBodyBase*> lights;
    std::vector<Light> lights;
    glm::vec3 lightColor = glm::vec3(0.f, 0.5f, 0.5f);
    glm::mat4 lightSpaceMatrix;
    Shader lightShader;
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
    bool aabbTest(GameBodyBase *a, GameBodyBase *b);
    bool obbTest(GameBodyBase *a, GameBodyBase *b);
    void ProcessInput(GLfloat dt);
    void Update(GLfloat dt);
    void Render();
    void depthRender();
    void initDepthMap();
    void RenderScene(Shader& shader); // FIXME: 这个函数不应该放在Game类当中
    void addObjectType(std::string name);
    void renderObject(const std::string& name, Shader& sh, GameBodyBase* object);
    void ProcessMouseMovement(double xoffset, double yoffset);
};

#endif