#include "game.h"
#include "resource_manager.h"
#include "box_renderer.h"


BoxRenderer * Renderer;
BoxRenderer * Renderer2; // for demo add lighting square

Game::Game(GLuint width, GLuint height) 
	: State(GAME_ACTIVE), Keys(), Width(width), Height(height), camera(glm::vec3(0.0f, 0.0f, 3.0f))
{ 

}

Game::~Game()
{
    delete Renderer;
    delete Renderer2;
}

void Game::Init()
{ 
    // Load shaders
    // ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");
    ResourceManager::LoadShader("shaders/lighting.vs", "shaders/lighting.frag", nullptr, "lighting");
    ResourceManager::LoadShader("shaders/lamp.vs", "shaders/lamp.frag", nullptr, "lamp");
    // Configure shaders
    // TODO: configure shaders : camera etc. 
    // glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
    // ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    // ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // Load textures
    // no alpha value
    
    // ResourceManager::LoadTexture("textures/container2.png", GL_FALSE, "container2");
    // ResourceManager::LoadTexture("textures/container2_specular.png", GL_FALSE, "container2_specular");
    // ResourceManager::LoadTexture("textures/awesomeface.png", GL_TRUE, "face");
    // // Set render-specific controls
    Renderer = new BoxRenderer(ResourceManager::GetShader("lighting"));
    Renderer2 = new BoxRenderer(ResourceManager::GetShader("lamp"));
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
        if (this->Keys[GLFW_KEY_W])
            camera.ProcessKeyboard(FORWARD, dt);
        if (this->Keys[GLFW_KEY_S])
            camera.ProcessKeyboard(BACKWARD, dt);
        if (this->Keys[GLFW_KEY_A])
            camera.ProcessKeyboard(LEFT, dt);
        if (this->Keys[GLFW_KEY_D])
            camera.ProcessKeyboard(RIGHT, dt);
        // TODO: add space 
    }
}

void Game::Render()
{
    // TODO: render here
    Renderer2->DrawBox(ResourceManager::GetTexture("container2"), ResourceManager::GetTexture("container2_specular"), glm::vec3(2, 2, 2), glm::vec3(0.5, 1, 1), glm::vec3(1.0f, 1.0f, 1.0f), *this);
    Renderer->DrawBox(ResourceManager::GetTexture("container2"), ResourceManager::GetTexture("container2_specular"), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::vec3(1.0f, 1.0f, 1.0f), *this);
    // Renderer->DrawBox(nullptr, nullptr, glm::vec3(1.2f, 1.0f, 2.0f),glm::vec3(0.2f,0.2f,0.2f),glm::vec3(0.0f, 0.0f, 0.0f), *this);
}