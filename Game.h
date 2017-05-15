#ifdef _GAME_H_
#define _GAME_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <tuple>


enum GameState{
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN    
};


class Game
{
public:
    // Game state
    GameState              State;	
    GLboolean              Keys[1024];
    GLboolean              KeysProcessed[1024];
    GLuint                 Width, Height;
    std::vector<GameLevel> Levels;
    std::vector<PowerUp>   PowerUps;
    GLuint                 Level;
    GLuint                 Lives;
    // Constructor/Destructor
    Game(GLuint width, GLuint height);
    ~Game();
    // Initialize game state (load all shaders/textures/levels)
    void Init();
    // GameLoop
    void ProcessInput(GLfloat dt);
    void Update(GLfloat dt);
    void Render();
    void DoCollisions();
    // Reset
    void ResetLevel();
    void ResetPlayer();
    // Powerups
    void SpawnPowerUps(GameObject &block);
    void UpdatePowerUps(GLfloat dt);
};

#endif