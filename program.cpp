#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "game.h"
#include "resource_manager.h"
#include "GameBodyBase.h"
// GLFW function declerations
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// The Width of the screen
const GLuint SCREEN_WIDTH = 800;
// The height of the screen
const GLuint SCREEN_HEIGHT = 600;


GLfloat lastX  =  SCREEN_WIDTH  / 2.0;
GLfloat lastY  =  SCREEN_HEIGHT / 2.0;


Game Shoot(SCREEN_WIDTH, SCREEN_HEIGHT);


int main(int argc, char *argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Shoot", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    // bind window to the game
    Shoot.window = window;
    glewExperimental = GL_TRUE;
    
    glewInit();
    glGetError(); // Call it once to catch glewInit() bug, all other errors are now from our application.

    // glfw callback functions 
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // TODO: these functions has some use
    // GLFW Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // OpenGL configuration
    glViewport(0, 0, SCREEN_WIDTH * 2, SCREEN_HEIGHT * 2) ;
    // glEnable(GL_CULL_FACE);
    // glEnable(GL_BLEND); 
    glEnable(GL_DEPTH_TEST);

    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize game
    Shoot.Init();

    // DeltaTime variables
    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;

    // Start Game within Menu State
    Shoot.State = GAME_ACTIVE;

    while (!glfwWindowShouldClose(window))
    {
        // Calculate delta time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();

        //deltaTime = 0.001f;
        // Manage user input
        Shoot.ProcessInput(deltaTime);

        // Update Game state
        Shoot.Update(deltaTime);

        // Render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Shoot.Render();

        glfwSwapBuffers(window);
    }

    // Delete all resources as loaded using the resource manager
    ResourceManager::Clear();

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // When a user presses the escape key, we set the WindowShouldClose property to true, closing the application
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS && Shoot.Keys[key] > 2)
            Shoot.Keys[key] = 1;
        else if (action == GLFW_RELEASE && Shoot.Keys[key] < 3)
            Shoot.Keys[key] = 3;
    }
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // TODO: add these to shoot class
    // quite ugly here
    // looking for a better solution
    // to integrate into the Game Class
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

    lastX = xpos;
    lastY = ypos;
    
    Shoot.player.ProcessMouseMovement(xoffset, yoffset);
    // std::cout << "xoffset: " << xoffset << " yoffset: " << yoffset << std::endl; 

}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT){
        if (action == GLFW_PRESS && Shoot.mouse[0] == 0)
            Shoot.mouse[0] = 1;
        else if(action == GLFW_RELEASE)
            Shoot.mouse[0] = 0;
        // puts("fuck");
    }
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Shoot.player.camera.ProcessMouseScroll(yoffset);
}