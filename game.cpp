#include "game.h"
#include "resource_manager.h"
#include "box_renderer.h"
#include <vector>

std::vector<BoxRenderer*> renderers;
Game::Game(GLuint width, GLuint height) 
	: State(GAME_ACTIVE), Keys(), Width(width), Height(height), camera(glm::vec3(0.0f, 0.0f, 3.0f))
{ 
    lightPos = glm::vec3(0.0f, 0.0f, 3.0f);
}

Game::~Game()
{
    int size = renderers.size();
    for(int i = 0; i < size; i++){
        delete renderers[i];
    }
}

void Game::Init()
{ 
    ResourceManager::LoadShader("shaders/lighting.vs", "shaders/lighting.frag", nullptr, "lighting");
    ResourceManager::LoadShader("shaders/lamp.vs", "shaders/lamp.frag", nullptr, "lamp");
    renderers.push_back(new BoxRenderer(ResourceManager::GetShader("lighting")));
    renderers.push_back(new BoxRenderer(ResourceManager::GetShader("lamp")));

}

void Game::Update(GLfloat dt)
{
    // TODO: 
    // in Game::Update
    // set shader's projection, view, clip matrix 
    // set ResourceManager::GetShader("...").Use() 
    // set each shaders' attribute. 
    glm::mat4 model;
    this->lightPos = glm::vec3(glm::rotate(model, dt, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(this->lightPos,1.0f)); 
    // this->lightPos = this->camera.Position;
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
        if (this->Keys[GLFW_KEY_SPACE]){
            camera.ProcessKeyboard(SPACE, dt);
        }
        // TODO: add space 
    }
}

void Game::Render()
{
    // TODO: render here
    renderers[0]->DrawBox(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::vec3(1.0f, 1.0f, 1.0f), *this);
    renderers[1]->DrawBox(this->lightPos, glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.f, 0.5f, 0.5f), *this);
    renderers[0]->DrawBox(glm::vec3(2,2,2),glm::vec3(1,1,1),glm::vec3(1.0f,1.0f,1.0f),*this);
    renderers[0]->DrawBox(glm::vec3(-2,-2,-2),glm::vec3(1,1,1),glm::vec3(1.0f,1.0f,1.0f),*this);
}