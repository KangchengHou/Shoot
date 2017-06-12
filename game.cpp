#include "game.h"

// #include "box_renderer.h"
#include <vector>
#include <algorithm>
#include <SOIL/SOIL.h>
#include "physics/q3.h"

GLfloat time_gun;
GLfloat shootTime;

Game::Game(GLuint width, GLuint height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{
    for (int i = 0; i < 1024; i++)
    {
        Keys[i] = false;
    }
    leftMouse = false;
    rightMouse = false;

    rendernear = 0.1f;
    renderfar = 5000.0f;
}

Game::~Game()
{
}

void Game::Init()
{

    this->cubeVAO = 0;
    this->cubeVBO = 0;
    ResourceManager::LoadTexture("./wood.png", false, "woodTexture");
    ResourceManager::LoadTexture("./flame.png", GL_TRUE, "flame");
    ResourceManager::LoadTexture("./particle.png", GL_TRUE, "laser");
    ResourceManager::LoadTexture("./fire.png", GL_TRUE, "fire");
    ResourceManager::LoadTexture("./booster.png", GL_TRUE, "rocket");
    ResourceManager::LoadTexture("./brickwall.jpg", GL_FALSE, "ground");
    ResourceManager::LoadTexture("./brickwall_normal.jpg", GL_FALSE, "ground_normal");
    ResourceManager::LoadTexture("./wall.png", GL_FALSE, "wall");
    ResourceManager::LoadTexture("./wall_normal.png", GL_FALSE, "wall_normal");

    // ResourceManager::LoadTexture("./brickwall.jpg", GL_FALSE, "brickwall");
    // ResourceManager::LoadTexture("./brickwall_normal.jpg", GL_FALSE, "brickwall_normal");
    // FIXME: add appropriate position

    ResourceManager::LoadShader("shaders/point_shadows.vs", "shaders/point_shadows.frag", nullptr, "point_shadows");
    ResourceManager::LoadShader("shaders/point_shadows_depth.vs", "shaders/point_shadows_depth.frag", "shaders/point_shadows_depth.gs", "point_shadows_depth");
    ResourceManager::LoadShader("shaders/normal_mapping.vs", "shaders/normal_mapping.frag", nullptr, "normal_mapping");
    ResourceManager::LoadShader("shaders/particle.vs", "shaders/particle.frag", nullptr, "particle");

    shader = ResourceManager::GetShader("point_shadows");
    depthShader = ResourceManager::GetShader("point_shadows_depth");
    this->initDepthMap();
    this->lights.push_back(Light(glm::vec3(0.0f)));

    ResourceManager::addObjectType("cube");
    ResourceManager::addObjectType("rocket");
    // ResourceManager::addObjectType("player");
    ResourceManager::addObjectType("plane");
    ResourceManager::loadModel("cannon");
    ResourceManager::loadModel("player");
    ResourceManager::loadModel("sphere");
    ResourceManager::loadModel("gun");
    ResourceManager::loadModel("building");
    player = addObject(PLAYER, glm::vec3(0.0f, 15.0f, 100.0f), 90.0f, 0.0f, 0.0f, 10.0f, false);
    boss = player;
    time_gun = 0.0f;
    shootTime = 0.0f;
    // players.push_back(player);

    this->groundSize = 10000.0f;
    addObject(OTHER, glm::vec3(0.0f, -groundSize / 2, 0.0f), 90.0f, 0.0f, 0.0f, this->groundSize, false, true);
    // puts("fuck");
    // addObject(CANNON, glm::vec3(5.0f, 50.0f, -5.0f), 90.0f, 0.0f, 0.0f, 0.1f);

    for (int i = 0; i < 5; i++)
    {
        cannongroup.cannons.push_back(addObject(CANNON, glm::vec3(i * 40 - 80, 10, -20), 90, 0, 0, 0.1));
        cannongroup.frequency.push_back(((rand() % 200) + 1) / 5.f);
        cannongroup.timer.push_back(0);
    }

    addObject(BUILDING, glm::vec3(0, 500, -1600), 90, 90, 0, 0.004, true, true);
    // puts("");
}
#define sqr(x) ((x) * (x))
void Game::Update(GLfloat dt)
{
    cannongroup.orientPlayer(player->position);
    for (int i = 0; i < cannongroup.cannons.size(); i++)
    {
        // cannongroup.orientPlayer(player->position);
        cannongroup.timer[i] += dt;
        if (cannongroup.timer[i] > cannongroup.frequency[i])
        {
            cannongroup.timer[i] -= cannongroup.frequency[i];
            // puts("bong");
            glm::vec3 shootposition = cannongroup.cannons[i]->position + cannongroup.cannons[i]->front * 5.0f + cannongroup.cannons[i]->up * 2.0f;
            auto *p = addObject(CANNONBULLET, shootposition, 90, 0, 0, 0.3);
            auto dis = player->position - shootposition;
            float speed = 10;
            // float t = sqrt(sqr(dis.x) + sqr(dis.z)) / speed;
            float s = sqrt(sqr(dis.x) + sqr(dis.z));
            float y = -dis.y;
            float t = sqrt(2 / 9.8 * (sqrt(3) / 3 * s + y));
            float v = s / t;
            p->setSpeed(glm::vec3(v * dis.x / s, sqrt(3) / 3 * v, v * dis.z / s));
            p->life = p->rest_life = 10;
        }
    }

    static f32 accumulator = 0;
    accumulator += dt;
    accumulator = q3Clamp01(accumulator);
    static int acc = 0;
    while (accumulator >= sceneDt)
    {
        scene.Step();
        accumulator -= sceneDt;
        acc++;
    }
    for (auto &object : objects)
    {
        if (object->particleGenerator)
            object->particleGenerator->update(dt, object->position, object->speed, 2, -object->scale * object->offset);
    }

    glm::mat4 model;
    lights[0].position = glm::vec3(this->player->position.x, 10.0f, this->player->position.z);
    bool flag = false;

    // std::cout << "107m" << std::endl;

    int cnt = 0;
    auto iter = objects.cbegin();
    while (iter != objects.cend())
    {
        // printf("before: %.5f\n", (*iter)->rest_life);
        (*iter)->rest_life -= dt;
        // cout << "dt: " << dt << endl;
        // printf("after: %.5f\n", (*iter)->rest_life);
        auto next = ++iter;
        iter--;
        if ((*iter)->rest_life <= 0)
        {
            this->scene.RemoveBody((*iter)->body);
            delete (*iter);
            objects.erase(iter);
            iter = next;
            continue;
        }

        if (iter == objects.cend())
            break;
        if ((*iter)->type == ROCKET)
        {
            (*iter)->yaw += (*iter)->yawspeed * dt;
            (*iter)->pitch += (*iter)->pitchspeed * dt;
            // (*iter)->yaw = (*iter)->renderyaw;
            // (*iter)->pitch = (*iter)->renderpitch;
            // std::cout << "----" << std::endl;
            // std::cout << (*iter)->yaw << " " << (*iter)->pitch << " " << std::endl;

            (*iter)->updateBaseVectorsAccordingToSelfAngles();
            // std::cout << (*iter)->front.x << " " << (*iter)->front.y << " " << (*iter)->front.z << " " << std::endl;
            // // std::cout << (*iter)->front.x << " " << (*iter)->front.y << " " << (*iter)->front.z << " "  << std::endl;
            // // std::cout << (*iter)->front.x << " " << (*iter)->front.y << " " << (*iter)->front.z << " "  << std::endl;

            // std::cout << "----" << std::endl;

            (*iter)->setSpeed((*iter)->front * (*iter)->MovementSpeed);
        }
        cnt++;
        q3Transform trans = (*iter)->body->GetTransform();
        q3Vec3 pos = trans.position;
        q3Mat3 rot = trans.rotation;

        // std::cout << "115" << std::endl;
        glm::vec3 off = (*iter)->offset * (*iter)->scale;
        (*iter)->setPos(pos.x - off.x, pos.y - off.y, pos.z - off.z);
        
        
        // cout << (*iter)->renderType << endl;
        // cout << (*iter)->rest_life << endl;
        // cout << (*iter)->body->GetFlag() << endl;
        // std::cout << (*iter)->position.x << " " << (*iter)->position.y << " " << (*iter)->position.z << " " << std::endl;
        // std::cout << (*iter)->body->GetTransform().position.x << " " << (*iter)->body->GetTransform().position.y << " " << (*iter)->body->GetTransform().position.z << " " << std::endl;
        // puts("before");
        if((*iter)->type == CANNONBULLET || (*iter)->type == LASER){
            // puts("fuck");
            if( (*iter)->body->IsColliding() && (*iter)->life - (*iter)->rest_life > 1) {
                explosionParticles.push_back(new explosionParticle(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("flame"), 50, 0.5f, (*iter)->position));

                (*iter)->rest_life = 0;
            }
            else {
                // puts("no");
            }
        }
        // puts("after");

        iter++;
    }
    for (auto &es : explosionParticles)
    {
        es->life -= dt;
        if (es->life > 0)
            es->particleSystem->update(dt, es->position,glm::vec3(0.0f), 2, glm::vec3(((rand() % 100) - 50) / 50.0f,((rand() % 100) - 50) / 50.0f,((rand() % 100) - 50) / 50.0f) );
    }
    time_gun += dt * 200.0f;
    if (time_gun > 360.0f) time_gun = 0.0f;
    if (shootTime > 0) shootTime -= 1.0f;
}
GLuint Game::loadTexture(GLchar const *path)
{
    // Generate texture ID and load texture data
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width, height;
    unsigned char *image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);
    return textureID;
}
void Game::initDepthMap()
{
    shader.Use();
    shader.SetInteger("diffuseTexture", 0);
    shader.SetInteger("depthMap", 1);
    this->shadowWidth = 1024;
    this->shadowHeight = 1024;
    glGenFramebuffers(1, &this->depthMapFBO);
    glGenTextures(1, &this->depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->depthCubemap);
    for (GLuint i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, this->shadowWidth, this->shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, this->depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}
void Game::ProcessInput(GLfloat dt)
{

    if (this->State == GAME_ACTIVE)
    {
        //     // now in the game state
        //     // process input for player
        if (this->Keys[GLFW_KEY_TAB] == 1)
        {
            this->Keys[GLFW_KEY_TAB] = false;
            if (rocket == NULL)
            {
                std::cout << "no rocket" << std::endl;
            }
            else if (boss->type == PLAYER)
            {
                boss = rocket;
                std::cout << "change boss to rocket" << std::endl;
            }
            else if (boss->type == ROCKET)
            {
                boss = player;
            }
            // this->Keys[GLFW_KEY_TAB]++;
        }
        if (this->Keys[GLFW_KEY_ENTER] == 1)
        {
            // if (rocket == NULL) {
            // std::cout << "202" << std::endl;

            Keys[GLFW_KEY_ENTER] = false;
            // std::cout << "204" << std::endl;

            glm::vec3 rocketPos = player->position + WORLDUP * 2.0f;
            rocket = addObject(ROCKET, rocketPos, 90.0f, 90.0f, 0.0f, 1.0f, true, false, 0);
            rocket->setSpeed(glm::vec3(0.0f, 1.0f, 0.0f));

            // std::cout << "198" << std::endl;

            // particles = new ParticleGenerator(shader, ResourceManager::GetTexture("fire"), GLuint(5));
        }

        if (this->Keys[GLFW_KEY_W] == true)
            boss->ProcessKeyboard(FORWARD, dt, this->Keys[GLFW_KEY_W]);
        else if (this->Keys[GLFW_KEY_S] == true)
            boss->ProcessKeyboard(BACKWARD, dt, this->Keys[GLFW_KEY_S]);
        else if (this->Keys[GLFW_KEY_A] == true)
            boss->ProcessKeyboard(LEFT, dt, this->Keys[GLFW_KEY_A]);
        else if (this->Keys[GLFW_KEY_D] == true)
            boss->ProcessKeyboard(RIGHT, dt, this->Keys[GLFW_KEY_D]);
        else
        {
            if (boss->type == PLAYER)
            {

                q3Vec3 v = boss->body->GetLinearVelocity();
                boss->setSpeed(glm::vec3(0.0f, v.y, 0.0f));
            }
        }

        if (this->Keys[GLFW_KEY_SPACE] == 1)
        {
            boss->setSpeed(glm::vec3(boss->MovementSpeed * boss->up));
        }

        if (leftMouse)
        {
            leftMouse = false;
            GameBodyBase *bullet = addObject(LASER, player->position + 5.0f * player->camera.front, player->pitch, player->yaw, player->roll, 0.1f, false, false, 0);
            bullet->life = bullet->rest_life = 10;
            bullet->setSpeed(glm::vec3(20.0f * player->camera.front));
            shootTime = 6.0f;
        }
    }
}

void Game::depthRender()
{
    GLfloat aspect = (GLfloat)this->shadowWidth / (GLfloat)this->shadowHeight;

    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, rendernear, renderfar); // 太远的东西也会被剪裁掉
    std::vector<glm::mat4> shadowTransforms;

    shadowTransforms.push_back(shadowProj * glm::lookAt(lights[0].position, lights[0].position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lights[0].position, lights[0].position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lights[0].position, lights[0].position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lights[0].position, lights[0].position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lights[0].position, lights[0].position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lights[0].position, lights[0].position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
    // 这里是先把场景搞到depth cubemap上面
    glViewport(0, 0, this->shadowWidth, this->shadowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, this->depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    depthShader.Use();
    // std::cout << "far : " << far << std::endl;
    for (GLuint i = 0; i < 6; i++)
    {
        depthShader.SetMatrix4(("shadowTransforms[" + std::to_string(i) + "]").c_str(), shadowTransforms[i]);
    }
    // far 是用来控制裁剪空间远近的
    depthShader.SetFloat("far_plane", renderfar);
    depthShader.SetVector3f("lightPos", this->lights[0].position.x, this->lights[0].position.y, this->lights[0].position.z);
    // 然后用这个shader渲染所有东西

    for (auto iter = objects.begin(); iter != objects.end(); iter++){
        if ((*iter)->visible)
        {
            renderObject((*iter)->renderType, depthShader, (*iter));
        }
    }
    
    
    // 渲染地面
    glm::mat4 model;
    model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(10.0f));
    depthShader.SetMatrix4("model", model);

    glBindVertexArray(ResourceManager::VAOmap["plane"]);
    glDrawArrays(GL_TRIANGLES, 0, ResourceManager::VAOSizeMap["plane"]);
    glBindVertexArray(0);


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Game::Render()
{

    depthRender(); // 将所有物体都渲染一遍
    // 渲染普通的场景
    glViewport(0, 0, this->Width * 2 , this->Height * 2);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderEnvironment(); // 将墙壁和地面都渲染一遍 这些是带有阴影和法线的

    // renderGameObjects(); // 将其他所有物体都渲染一遍， 带有阴影的
    shader.Use();
    glm::mat4 projection = glm::perspective(boss->camera.zoom, (float)this->Width / (float)this->Height, rendernear, renderfar);
    glm::mat4 view = boss->camera.GetViewMatrix();
    shader.SetMatrix4("projection", projection);
    shader.SetMatrix4("view", view);
    shader.SetVector3f("lightPos", this->lights[0].position.x, this->lights[0].position.y, this->lights[0].position.z);
    shader.SetVector3f("viewPos", this->boss->camera.position.x, this->boss->camera.position.y, this->boss->camera.position.z);

    shader.SetFloat("far_plane", renderfar); // FIXME: 这个float非常重要
    glActiveTexture(GL_TEXTURE0);
    ResourceManager::GetTexture("woodTexture").Bind();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->depthCubemap);

    // render gun
    glm::vec3 gun_pos = this->player->camera.position +
                        this->player->camera.front * (1.5f + 0.2f * sin(glm::radians(30.0f * shootTime))) + this->player->camera.right * 1.5f + this->player->camera.up * (-1.5f + 0.05f * sin(glm::radians(time_gun)));
    glm::mat4 model;
    model = glm::translate(model, gun_pos);
    
    model = glm::rotate(model, glm::radians(-player->camera.camerapospitch ), player->camera.right);
    model = glm::rotate(model, glm::radians(player->camera.cameraposyaw + 5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.SetMatrix4("model", model);
    ResourceManager::LoadedModels["gun"]->Draw(shader);

    for (auto iter = objects.begin(); iter != objects.end(); iter++)
        if ((*iter)->visible)
        {
            renderObject((*iter)->renderType, shader, (*iter));
        }

    glActiveTexture(GL_TEXTURE0);

    
    for (auto &object : objects)
    {
        if (object->particleGenerator)
        {
            if(object->type == CANNONBULLET)
                ResourceManager::GetTexture("flame").Bind(); //待会人发出的子弹要和cannon bullet 区别
            if(object->type == LASER)
                ResourceManager::GetTexture("laser").Bind(); //待会人发出的子弹要和cannon bullet 区别
            if(object->type == LASER){
                object->particleGenerator->draw(projection, view, this->boss->camera.front, glm::vec3(5.0f));
            }
            if(object->type == CANNONBULLET){
                object->particleGenerator->draw(projection, view, this->boss->camera.front);
            }
            
        }
    }

    for (auto &es : explosionParticles)
    {
        if (es->life > 0) 
        {
            es->particleSystem->draw(projection, view, this->boss->camera.front, glm::vec3(40.0f));
        }
    }
}

void Game::renderEnvironment()
{
    // 渲染房间的底部 以及 周围
    // 首先渲染房间的下面
    Shader normalShader = ResourceManager::GetShader("normal_mapping");
    normalShader.Use();
    normalShader.SetInteger("diffuseMap", 0);
    normalShader.SetInteger("normalMap", 1);
    normalShader.SetInteger("depthMap", 2);
    normalShader.SetFloat("far_plane",this->renderfar);
    glm::mat4 projection = glm::perspective(boss->camera.zoom, (float)this->Width / (float)this->Height, rendernear, renderfar);
    glm::mat4 view = boss->camera.GetViewMatrix();
    normalShader.SetMatrix4("projection", projection);
    normalShader.SetMatrix4("view", view);
    normalShader.SetVector3f("lightPos", this->lights[0].position.x, this->lights[0].position.y, this->lights[0].position.z);
    normalShader.SetVector3f("viewPos", this->boss->camera.position.x, this->boss->camera.position.y, this->boss->camera.position.z);

    normalShader.Use();
    glActiveTexture(GL_TEXTURE0);
    ResourceManager::GetTexture("ground").Bind();
    // glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glActiveTexture(GL_TEXTURE1);
    ResourceManager::GetTexture("ground_normal").Bind();
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->depthCubemap);


    glm::mat4 model;
    model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(10.0f));
    normalShader.Use();
    normalShader.SetMatrix4("model", model);

    glBindVertexArray(ResourceManager::VAOmap["plane"]);
    glDrawArrays(GL_TRIANGLES, 0, ResourceManager::VAOSizeMap["plane"]);
    glBindVertexArray(0);
}

void Game::renderObject(const std::string &name, Shader &sh, GameBodyBase *object)
{
    // TODO: add ambient light etc.
    glm::mat4 model = glm::mat4();
    model = glm::translate(model, object->position);
    model = glm::scale(model, glm::vec3(object->scale));

    model = glm::rotate(model, object->type == ROCKET ? glm::radians(object->pitch - 90.0f) : glm::radians(object->pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, (object->type == ROCKET ? glm::radians(object->yaw) : glm::radians(object->yaw + 90.0f)), glm::vec3(0.0f, 1.0f, 0.0f));

    sh.SetMatrix4("model", model);

    if (name == "cannon" || name == "player" || name == "sphere" || name == "building")
    {
        if(name == "building") {
            puts("drawing building");
        }
        ResourceManager::LoadedModels[name]->Draw(sh);
    }
    else
    {
        GLuint VAO = ResourceManager::VAOmap[name];
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, ResourceManager::VAOSizeMap[name]);
        glBindVertexArray(0);
    }
}

void Game::registerCollisionBody(GameBodyBase *obj, bool rest, float gravityScale)
{
    q3BodyDef bodyDef;
    glm::vec3 pos = obj->position + obj->offset * obj->scale;
    bodyDef.position = q3Vec3(pos.x, pos.y, pos.z);
    if (!rest)
    {
        bodyDef.bodyType = q3BodyType::eDynamicBody;
    }
    bodyDef.gravityScale = gravityScale;

    q3Body *body = scene.CreateBody(bodyDef);
    q3BoxDef boxDef;   // See q3Box.h for settings details
    q3Transform trans; // Contains position and orientation, see q3Transform.h for details
    // q3Identity( localSpace ); // Specify the origin, and identity orientation
    trans.position = q3Vec3(0.f, 0.f, 0.f);
    double v[9] = {0.0};
    const float *pSource = (const float *)glm::value_ptr(obj->rotationMatrix);
    for (int i = 0; i < 9; ++i)
        v[i] = pSource[i];
    trans.rotation = q3Mat3(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8]);

    // Create a box at the origin with width, height, depth = (1.0, 1.0, 1.0)
    // and add it to a rigid body. The transform is defined relative to the owning body

    glm::vec3 size = ResourceManager::modelSizeMap[obj->renderType] * obj->scale;
    boxDef.Set(trans, q3Vec3(size[0], size[1], size[2]));
    body->AddBox(boxDef);
    obj->body = body;
}
