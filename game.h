/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef GAME_H
#define GAME_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "GameBodyBase.h"
#include "box.h"
#include "shader.h"

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
    GLFWwindow* window;
    GameBodyBase player;
    glm::vec3 Gravity = glm::vec3(0.f, -9.8f, 0.f);

    std::vector<GameBodyBase*> bullets;
    glm::vec3 bulletColor = glm::vec3(1.0f, 1.0f, 1.0f);
    Shader bulletShader;
    std::vector<GameBodyBase*> lights;
    glm::vec3 lightColor = glm::vec3(0.f, 0.5f, 0.5f);
    Shader lightShader;
    
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
};

#endif