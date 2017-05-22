#include "game.h"
#include "resource_manager.h"
// #include "box_renderer.h"
#include <vector>
#include <algorithm>

// std::vector<BoxRenderer*> renderers;

Game::Game(GLuint width, GLuint height) 
	: State(GAME_ACTIVE), Keys(), Width(width), Height(height), camera(glm::vec3(0.0f, 0.0f, 10.0f))
{ 
    lightPos = glm::vec3(0.0f, 0.0f, 0.0f);
}

Game::~Game()
{
    // int size = renderers.size();
    // for(int i = 0; i < size; i++){
    //     delete renderers[i];
    // }
}

void Game::Init()
{ 
    ResourceManager::LoadShader("shaders/lighting.vs", "shaders/lighting.frag", nullptr, "lighting");
    ResourceManager::LoadShader("shaders/lamp.vs", "shaders/lamp.frag", nullptr, "lamp");
    // renderers.push_back(new BoxRenderer(ResourceManager::GetShader("lighting")));
    // renderers.push_back(new BoxRenderer(ResourceManager::GetShader("lamp")));
    // lights.push_back(new Box(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.5, 0.5, 0.5)));
    bulletShader = ResourceManager::GetShader("lighting");
    lightShader = ResourceManager::GetShader("lamp");
<<<<<<< HEAD
    lights.push_back(new Box(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1, 1, 1)));

    Box* bullet = new Box(glm::vec3(0.0f, -7.0f, 0.0f), glm::vec3(100, 10, 100), glm::vec3(1.0f, 0.0f, 0.0f));
    bullets.push_back(bullet);
    for (auto iter = bullets.cbegin(); iter != bullets.cend(); iter++)
    {
        std::cout << (*iter)->color[0] << (*iter)->color[1] << (*iter)->color[2] << std::endl;
    }
}
bool Game::aabbTest(GameBodyBase *a, GameBodyBase *b) {
    for(int i = 0; i < 3; i++) {
        // printf("%.0lf %.0lf\n", a->position[i] + a->size[i]/2 , b->position[i] - b->size[i]/2);
        // printf("%.0lf %.0lf\n", b->position[i] + b->size[i]/2 , a->position[i] - a->size[i]/2);
        if (a->position[i] + a->size[i]/2 < b->position[i] - b->size[i]/2 ||
            b->position[i] + b->size[i]/2 < a->position[i] - a->size[i]/2) return false;
        puts("live");
    }
    return true;
=======
    lights.push_back(new Box(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.5, 0.5, 0.5)));
>>>>>>> 6b8a28ebb9b34317e5f7904634122836c7b1f208
}
bool Game::obbTest(GameBodyBase *a, GameBodyBase *b) {
    glm::vec3 av[8];
    glm::vec3 bv[8];
    glm::vec3 axis[6];
    int cnt = 0;
    for(int i = 1; i < 2; i += 2)
        for(int j = -1; j < 2; j += 2)
            for(int k = -1; k < 2; k += 2) {
                av[cnt  ] = glm::vec3(a->position[0] + i * a->size[0] / 2, a->position[1] + j * a->size[1] / 2, a->position[2] + k * a->size[2] / 2);
                bv[cnt++] = glm::vec3(b->position[0] + i * b->size[0] / 2, b->position[1] + j * b->size[1] / 2, b->position[2] + k * b->size[2] / 2);
            }

    glm::mat4 model;
    // axis[0] = glm::vec3(glm::rotate(model, a->angle[0], axis) 
    //             * glm::vec4(position,1.0f)); 
    for(int i = 0; i < 6; i += 2) {
        axis[i  ] = av[i] - av[i+1];
        axis[i+1] = bv[i] - bv[i+1];
    }
    double minA, maxA, minB, maxB, ap, bp;
    for(int i = 0; i < 6; i++) {
        minA = minB =  1e9;
        maxA = maxB = -1e9;
        for(int j = 0; j < 8; j++) {
            ap = glm::dot(av[j], axis[i]);
            bp = glm::dot(av[j], axis[i]);
            minA = std::min(minA, ap);
            maxA = std::max(maxA, ap);
            minB = std::min(minB, bp);
            maxB = std::max(maxB, bp);
        }
        if(minA > maxB || minB > maxA) return false;
    }
    return true;
}
void Game::Update(GLfloat dt)
{
    // TODO: 
    // in Game::Update
    // set shader's projection, view, clip matrix 
    // set ResourceManager::GetShader("...").Use() 
    // set each shaders' attribute. 
    glm::mat4 model;
<<<<<<< HEAD
    // this->lightPos = glm::vec3(glm::rotate(model, dt, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(this->lightPos,1.0f)); 
    // lights[0]->rotate(dt, glm::vec3(0.0f, 1.0f, 0.0f));
=======
    this->lightPos = glm::vec3(glm::rotate(model, dt, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(this->lightPos,1.0f)); 
    lights[0]->rotate(dt, glm::vec3(0.0f, 1.0f, 0.0f));
    
>>>>>>> 6b8a28ebb9b34317e5f7904634122836c7b1f208
    for (auto iter = bullets.cbegin(); iter != bullets.cend(); iter++)
    {
        (*iter)->updateSpeed(dt);
        (*iter)->updatePos(dt);
        if(bullets.size() > 1) {
            printf("ind %d\n", iter - bullets.cbegin());
            printf("%.0lf %.0lf %.0lf\n", (*iter)->position[0], (*iter)->position[1], (*iter)->position[2]);
        }
    }
<<<<<<< HEAD
    for (auto a = bullets.cbegin(); a != bullets.cend(); a++)
        for (auto b = a+1; b != bullets.cend(); b++) 
            if(a != b) {
            if(aabbTest(*a, *b)) {
                puts("fuck");
                printf("ind %d\n", a - bullets.cbegin());
                printf("ind %d\n", b - bullets.cbegin());
                (*a)->setSpeed(glm::vec3(0, 0, 0));
                (*b)->setSpeed(glm::vec3(0, 0, 0));
                (*a)->setAcceleration(glm::vec3(0, 0, 0));
                (*b)->setAcceleration(glm::vec3(0, 0, 0));
                auto itera = bullets.cbegin();
                auto iterb = bullets.cbegin();
                printf("%d\n", itera == iterb);
                printf("%.0lf %.0lf %.0lf\n", (*a)->position[0], (*a)->position[1], (*a)->position[2]);
                printf("%.0lf %.0lf %.0lf\n", (*b)->position[0], (*b)->position[1], (*b)->position[2]);
                // exit(0);
            }
        }
=======
>>>>>>> 6b8a28ebb9b34317e5f7904634122836c7b1f208

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
        if (this->Keys[GLFW_KEY_SPACE] && !this->LastKeys[GLFW_KEY_SPACE]){
            
            Box* bullet = new Box(camera.Position + camera.Front, glm::vec3(0.5, 0.5, 0.5));
            bullet->setSpeed(glm::vec3(20 * camera.Front.x, 20 * camera.Front.y, 20 * camera.Front.z));
            bullet->acceleration = camera.Front;
<<<<<<< HEAD
            bullet->addAcceleration(this->Gravity);
=======
>>>>>>> 6b8a28ebb9b34317e5f7904634122836c7b1f208
            bullets.push_back(bullet);
            
        }
        // TODO: add space 
    }
    for(int i = 0; i < 1024; i++) {
        this->LastKeys[i] = this->Keys[i];
    }
}

void Game::Render()
{
    // TODO: render here
    for (auto iter = bullets.cbegin(); iter != bullets.cend(); iter++)
    {
        // renderers[0]->DrawBox((*iter)->position, (*iter)->size, bulletColor, *this);
<<<<<<< HEAD
        (*iter)->render((*iter)->color, this->lightPos, this->Width, this->Height, this->camera,  bulletShader);
=======
        (*iter)->render(bulletColor, this->lightPos, this->Width, this->Height, this->camera,  bulletShader);
>>>>>>> 6b8a28ebb9b34317e5f7904634122836c7b1f208
    }
    for (auto iter = lights.cbegin(); iter != lights.cend(); iter++)
    {
        (*iter)->render(lightColor, this->lightPos, this->Width, this->Height, this->camera, lightShader);
        // renderers[1]->DrawBox((*iter)->position, (*iter)->size, lightColor, *this);
    }
    // renderers[0]->DrawBox(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::vec3(1.0f, 1.0f, 1.0f), *this);
    // renderers[1]->DrawBox(this->lightPos, glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.f, 0.5f, 0.5f), *this);
}