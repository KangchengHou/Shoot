#include "game.h"
#include "resource_manager.h"
// #include "box_renderer.h"
#include <vector>
#include <algorithm>

// std::vector<BoxRenderer*> renderers;

Game::Game(GLuint width, GLuint height) 
	: State(GAME_ACTIVE), Keys(), Width(width), Height(height), player(glm::vec3(0.0f, 1.0f, 10.0f), glm::vec3(0.3f, 0.5f, 0.3f))
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
    player.init();
    ResourceManager::LoadShader("shaders/lighting.vs", "shaders/lighting.frag", nullptr, "lighting");
    ResourceManager::LoadShader("shaders/lamp.vs", "shaders/lamp.frag", nullptr, "lamp");
    // renderers.push_back(new BoxRenderer(ResourceManager::GetShader("lighting")));
    // renderers.push_back(new BoxRenderer(ResourceManager::GetShader("lamp")));
    // lights.push_back(new Box(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.5, 0.5, 0.5)));
    bulletShader = ResourceManager::GetShader("lighting");
    lightShader = ResourceManager::GetShader("lamp");
    Box* light = new Box(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1, 1, 1));
    light->init();
    lights.push_back(light);

    Box* bullet = new Box(glm::vec3(0.0f, -7.0f, 0.0f), glm::vec3(100, 10, 100), glm::vec3(1.0f, 0.0f, 0.0f));
    // bullets[0] is the ground
    bullet->init();
    bullets.push_back(bullet); 

    // bullets[1] is the player
    // player.addAcceleration(this->Gravity);
    bullets.push_back(&player);
    // for (auto iter = bullets.cbegin(); iter != bullets.cend(); iter++)
    // {
    //     std::cout << (*iter)->color[0] << (*iter)->color[1] << (*iter)->color[2] << std::endl;
    // }
}
bool Game::aabbTest(GameBodyBase *a, GameBodyBase *b) {
    for(int i = 0; i < 3; i++) {
        // printf("%.0lf %.0lf\n", a->position[i] + a->size[i]/2 , b->position[i] - b->size[i]/2);
        // printf("%.0lf %.0lf\n", b->position[i] + b->size[i]/2 , a->position[i] - a->size[i]/2);
        if (a->position[i] + a->size[i]/2 < b->position[i] - b->size[i]/2 ||
            b->position[i] + b->size[i]/2 < a->position[i] - a->size[i]/2) return false;
        // puts("live");
    }
    glm::vec3 dif = glm::normalize(b->position - a->position);
    GLfloat fcos = glm::dot(a->front, dif) / a->size[2] / b->size[2];
    GLfloat ucos = glm::dot(a->up, dif) / a->size[1] / b->size[1];
    GLfloat rcos = glm::dot(a->right, dif) / a->size[0] / b->size[0];
    // printf("fro  %.1lf %.1lf %.1lf\n", a->front.x, a->front.y, a->front.z);
    // printf("upp  %.1lf %.1lf %.1lf\n", a->up.x, a->up.y, a->up.z);
    // printf("rig  %.1lf %.1lf %.1lf\n", a->right.x, a->right.y, a->right.z);
    // printf("dif  %.1lf %.1lf %.1lf\n", dif.x, dif.y, dif.z);
    // printf("cos  %.1lf %.1lf %.1lf\n", fcos, rcos, ucos);
    if(fabs(fcos) >= fabs(rcos) && fabs(fcos) >= fabs(ucos)) {
        a->stuck[0 + (fcos > 0)] = GL_TRUE;
    }
    if(fabs(ucos) >= fabs(rcos) && fabs(ucos) >= fabs(fcos)) {
        a->stuck[2 + (ucos > 0)] = GL_TRUE;
    }
    if(fabs(rcos) >= fabs(fcos) && fabs(rcos) >= fabs(ucos)) {
        a->stuck[4 + (rcos > 0)] = GL_TRUE;
    }
    return true;
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
// bool Game::directTest(GameBodyBase *a, GameBodyBase *b, int j) {
//     if(j == 0 || j == 1) {
//         a->position += (-1 + j + j) * a->front / 5;
//     }

// }
void Game::Update(GLfloat dt)
{
    // TODO: 
    // in Game::Update
    // set shader's projection, view, clip matrix 
    // set ResourceManager::GetShader("...").Use() 
    // set each shaders' attribute. 
    glm::mat4 model;
    // this->lightPos = glm::vec3(glm::rotate(model, dt, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(this->lightPos,1.0f)); 
    // lights[0]->rotate(dt, glm::vec3(0.0f, 1.0f, 0.0f));
    for (auto a: bullets) {
        for(int j = 0; j < 6; j++)
            a->stuck[j] = false;
    }
    bool flag = false;
    for (auto a = bullets.cbegin(); a != bullets.cend(); a++)
        for (auto b = bullets.cbegin(); b != bullets.cend(); b++)  if(a != b) {
            if(aabbTest(*a, *b)) {
                if((*a) != &player){
                    (*a)->setSpeed(glm::vec3(0.f, 0.f, 0.f));
                    (*a)->setAcceleration(glm::vec3(0, 0, 0));
                }
                // puts("fuck");
                
                // (*a)->setAcceleration(glm::vec3(0, 0, 0));
                // (*b)->setAcceleration(glm::vec3(0, 0, 0));
                // printf("%d\n", itera == iterb);
                // printf("%.0lf %.0lf %.0lf\n", (*a)->position[0], (*a)->position[1], (*a)->position[2]);
                // printf("%.0lf %.0lf %.0lf\n", (*b)->position[0], (*b)->position[1], (*b)->position[2]);
                // exit(0);
            }
        }
    // printf("player  %.2lf %.2lf %.2lf\n", player.speed[0], player.speed[1], player.speed[2]);
    for (auto iter = bullets.cbegin(); iter != bullets.cend(); iter++)
    {
        // printf("before  %.2lf %.2lf %.2lf\n", (*iter)->speed[0], (*iter)->speed[1], (*iter)->speed[2]);
        // for(int j = 0; j < 6; j++)
        //     printf("%d ", (*iter)->stuck[j]);
        // puts("");
        (*iter)->updateSpeed(dt);
        (*iter)->updatePos(dt);
        // printf("after  %.2lf %.2lf %.2lf\n", (*iter)->speed[0], (*iter)->speed[1], (*iter)->speed[2]);
        // if(bullets.size() > 1) {
            // printf("ind %d\n", iter - bullets.cbegin());
            // printf("%.0lf %.0lf %.0lf\n", (*iter)->position[0], (*iter)->position[1], (*iter)->position[2]);
        // }
    }
    // printf("player  %.2lf %.2lf %.2lf\n", player.speed[0], player.speed[1], player.speed[2]);
    // if(flag) exit(0);
    // puts("+================+");
}


void Game::ProcessInput(GLfloat dt)
{
    if(this->State == GAME_ACTIVE){
        // now in the game state
        // process input for player 
        player.speed = player.up * glm::dot(player.speed, player.up);
        if (this->Keys[GLFW_KEY_W] == 1 || this->Keys[GLFW_KEY_W] == 3) {
            // printf("W %d\n", this->Keys[GLFW_KEY_W]);
            player.ProcessKeyboard(FORWARD, dt, this->Keys[GLFW_KEY_W]);
            // this->Keys[GLFW_KEY_W] ++;
        }
        if (this->Keys[GLFW_KEY_S] == 1 || this->Keys[GLFW_KEY_S] == 3) {
            player.ProcessKeyboard(BACKWARD, dt, this->Keys[GLFW_KEY_S]);
            // this->Keys[GLFW_KEY_S] ++;
        }
        if (this->Keys[GLFW_KEY_A] == 1 || this->Keys[GLFW_KEY_A] == 3) {
            player.ProcessKeyboard(LEFT, dt, this->Keys[GLFW_KEY_A]);
            // this->Keys[GLFW_KEY_A] ++;
        }
        if (this->Keys[GLFW_KEY_D] == 1 || this->Keys[GLFW_KEY_D] == 3) {
            player.ProcessKeyboard(RIGHT, dt, this->Keys[GLFW_KEY_D]);
            // this->Keys[GLFW_KEY_D] ++;
        }
        if (this->Keys[GLFW_KEY_SPACE] == 1){
            player.addPos(player.up * 0.1f);
            player.addSpeed(glm::vec3(5.0f * player.up));
            player.setAcceleration(1, 0);
            player.addAcceleration(this->Gravity);
            this->Keys[GLFW_KEY_SPACE] ++;
        }
        if (this->mouse[0] == 1){
            Box* bullet = new Box(player.position + player.front, glm::vec3(0.5, 0.5, 0.5));
            bullet->init();
            bullet->setSpeed(glm::vec3(5.0f * player.camera.front + 5.0f * player.camera.up));
            // bullet->acceleration = player.camera.front;
            bullet->addAcceleration(this->Gravity);
            bullets.push_back(bullet);
            this->mouse[0] = 2;
            
        }
        // TODO: add space 
    }
}

void Game::Render()
{
    // TODO: render here
    for (auto iter = bullets.cbegin(); iter != bullets.cend(); iter++)
    {
        // renderers[0]->DrawBox((*iter)->position, (*iter)->size, bulletColor, *this);
        // puts("fuck");
        (*iter)->render((*iter)->color, this->lightPos, this->Width, this->Height, this->player,  bulletShader);
    }
    for (auto iter = lights.cbegin(); iter != lights.cend(); iter++)
    {
        (*iter)->render(lightColor, this->lightPos, this->Width, this->Height, this->player, lightShader);
        // renderers[1]->DrawBox((*iter)->position, (*iter)->size, lightColor, *this);
    }
    // renderers[0]->DrawBox(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::vec3(1.0f, 1.0f, 1.0f), *this);
    // renderers[1]->DrawBox(this->lightPos, glm::vec3(0.5, 0.5, 0.5), glm::vec3(0.f, 0.5f, 0.5f), *this);
}