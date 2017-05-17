#include "game.h"
#include "resource_manager.h"
#include "box_renderer.h"




Game::Game(GLuint width, GLuint height) 
	: State(GAME_ACTIVE), Keys(), Width(width), Height(height),\
    mouse_xoffset(0),mouse_yoffset(0),scroll_yoffset(0),camera(glm::vec3(0.0f, 0.0f, 3.0f))
{ 

}

Game::~Game()
{
    delete Renderer;
}

void Game::Init()
{ 
    // Load shaders
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");
    ResourceManager::LoadShader("shaders/lighting.vs", "shaders/lighting.frag", nullptr, "lighting");
    ResourceManager::LoadShader("shaders/lamp.vs", "shaders/lamp.frag", nullptr, "lamp");
    // Configure shaders
    // TODO: configure shaders : camera etc. 
    // glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
    // ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    // ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // Load textures
    // no alpha value
    ResourceManager::LoadTexture("textures/container2.png", GL_FALSE, "container2");
    ResourceManager::LoadTexture("textures/container2_specular.png", GL_FALSE, "container2_specular");
    // ResourceManager::LoadTexture("textures/awesomeface.png", GL_TRUE, "face");
    // // Set render-specific controls
    // Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
}

void Game::Update(GLfloat dt)
{
    // TODO: 
    // in Game::Update
    // set shader's projection, view, clip matrix 
    // set ResourceManager::GetShader("...").Use() 
    // set each shaders' attribute. 
    

}


void Game::ProcessInput(GLfloat dt)
{
    if(this->State == GAME_ACTIVE){
        // now in the game state
        // process input for camera 
        if (keys[GLFW_KEY_W])
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (keys[GLFW_KEY_S])
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (keys[GLFW_KEY_A])
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (keys[GLFW_KEY_D])
            camera.ProcessKeyboard(RIGHT, deltaTime);
        camera.ProcessMouseMovement(this->mouse_xoffset, this->mouse_yoffset);
        camera.ProcessMouseScroll(this->scroll_yoffset);
    }
}

void Game::Render()
{
    // TODO: render here
    
    Renderer->DrawSprite(ResourceManager::GetTexture("face"), glm::vec2(200, 200), glm::vec2(300, 400), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f), this->camera);
}