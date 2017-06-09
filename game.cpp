#include "game.h"
#include "resource_manager.h"
// #include "box_renderer.h"
#include <vector>
#include <algorithm>
#include <SOIL/SOIL.h>
#include "physics/q3.h"
Game::Game(GLuint width, GLuint height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{
    for (int i = 0; i < 1024; i++) {
        Keys[i] = false;
    }
    leftMouse = false;
    rightMouse = false;

    rendernear = 0.1f;
    renderfar = 5000.0f;

    player = new GameBodyBase(PLAYER, glm::vec3(0.0f, 10.0f, 0.0f), 90.0f, 0.0f, 0.0f, glm::vec3(0.3f, 0.5f, 0.3f));
    // player = new GameBodyBase(PLAYER, glm::vec3(0.0f, 10.0f, 0.0f), 90.0f, 0.0f, 0.0f, 90.0f, 0.0f, false, glm::vec3(0.3f, 0.5f, 0.3f));
    objects.push_back(player);
    players.push_back(player);
    registerCollisionBody(player);

    boss = player;

    // GLfloat size = 100.0f;
    // room = new GameBodyBase(OTHER, glm::vec3(0.0f, size / 2, 0.0f), 90.0f, 0.0f, 0.0f, 90.0f, 0.0f, false, glm::vec3(size));
    // objects.push_back(room);

    GameBodyBase* ground = new GameBodyBase(OTHER, glm::vec3(0.0f, -100 / 2, 0.0f), 90.0f, 0.0f, 0.0f, glm::vec3(100));
    // GameBodyBase* ground = new GameBodyBase(OTHER, glm::vec3(0.0f, -100 / 2, 0.0f), 90.0f, 0.0f, 0.0f, 90.0f, 0.0f, false, glm::vec3(100));
    objects.push_back(ground);
    bullets.push_back(ground);
    registerCollisionBody(ground, true);

}
void Game::ProcessMouseMovement(double xoffset, double yoffset)
{
    // if (this->State == GAME_ACTIVE)
    // {
    //     this->boss->ProcessMouseMovement(xoffset, yoffset);
    // }
}
Game::~Game()
{
}

void Game::Init()
{

    this->cubeVAO = 0;
    this->cubeVBO = 0;
    ResourceManager::LoadTexture("./wood.png", false, "woodTexture");
    ResourceManager::LoadTexture("./fire.png", GL_TRUE, "fire");
    ResourceManager::LoadTexture("./booster.png", GL_TRUE, "rocket");

    // FIXME: add appropriate position

    ResourceManager::LoadShader("shaders/lamp.vs", "shaders/lamp.frag", nullptr, "lamp");
    ResourceManager::LoadShader("shaders/point_shadows.vs", "shaders/point_shadows.frag", nullptr, "point_shadows");

    lightShader = ResourceManager::GetShader("lamp");
    // TODO: new added shader
    shader = ResourceManager::GetShader("point_shadows");
    depthShader = ResourceManager::GetShader("point_shadows_depth");
    this->initDepthMap();
    this->lights.push_back(Light(glm::vec3(0.0f)));


    addObjectType("cube");
    addObjectType("rocket");
    addObjectType("player");
}


void Game::Update(GLfloat dt)
{
    static f32 accumulator = 0;
    accumulator += dt;
    accumulator = q3Clamp01( accumulator );
    static int acc = 0;
    while ( accumulator >= sceneDt )
    {
        scene.Step( );
        accumulator -= sceneDt;
        acc++;
    }
    if (rocket != NULL) {
        particles->update(dt, *rocket, 20, glm::vec3(0.0f));
    } else {

    }
    glm::mat4 model;
    lights[0].position = this->boss->camera.position;
    bool flag = false;

    // std::cout << "107m" << std::endl;


    int cnt = 0;
    for (auto iter = objects.cbegin(); iter != objects.cend(); iter++)
    {
        // std::cout << (*iter)->type << std::endl;
        if ((*iter)->type == ROCKET) {
            (*iter)->yaw += (*iter)->yawspeed * dt;
            (*iter)->pitch += (*iter)->pitchspeed * dt;
            // (*iter)->yaw = (*iter)->renderyaw;
            // (*iter)->pitch = (*iter)->renderpitch;
            std::cout << "----" << std::endl;
            std::cout << (*iter)->yaw << " " << (*iter)->pitch << " " << std::endl;

            (*iter)->updateBaseVectorsAccordingToSelfAngles();
            std::cout << (*iter)->front.x << " " << (*iter)->front.y << " " << (*iter)->front.z << " "  << std::endl;
            // // std::cout << (*iter)->front.x << " " << (*iter)->front.y << " " << (*iter)->front.z << " "  << std::endl;
            // // std::cout << (*iter)->front.x << " " << (*iter)->front.y << " " << (*iter)->front.z << " "  << std::endl;

            std::cout << "----" << std::endl;

            (*iter)->setSpeed((*iter)->front * (*iter)->MovementSpeed);


        }
        cnt++;
        q3Transform trans = (*iter)->body->GetTransform();
        q3Vec3 pos = trans.position;
        q3Mat3 rot = trans.rotation;

        // std::cout << "115" << std::endl;

        (*iter)->setPos(pos.x, pos.y, pos.z);
        // std::cout << (*iter)->position.x << " " << (*iter)->position.y << " " << (*iter)->position.z << " " << std::endl;
    }

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
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}
void Game::ProcessInput(GLfloat dt)
{

    if (this->State == GAME_ACTIVE)
    {
        //     // now in the game state
        //     // process input for player
        if (this->Keys[GLFW_KEY_TAB] == 1 )
        {
            this->Keys[GLFW_KEY_TAB] = false;
            if (rocket == NULL) {
                std::cout << "no rocket" << std::endl;
            } else if (boss->type == PLAYER) {
                boss = rocket;
                std::cout << "change boss to rocket" << std::endl;
            } else if (boss->type == ROCKET) {
                boss = player;
            }
            // this->Keys[GLFW_KEY_TAB]++;
        }
        if (this->Keys[GLFW_KEY_ENTER] == 1 )
        {
            // if (rocket == NULL) {
            // std::cout << "202" << std::endl;

            Keys[GLFW_KEY_ENTER] = false;
            // std::cout << "204" << std::endl;

            glm::vec3 rocketPos = player->position + WORLDUP * 2.0f;
            rocket = new GameBodyBase(ROCKET, rocketPos, 90.0f, 90.0f, 0.0f, glm::vec3(1.0f),  glm::vec4(1.0f));
            // rocket = new GameBodyBase(ROCKET, rocketPos, 90.0f, 90.0f, 0.0f, 90.0f, 90.0f, false, glm::vec3(1.0f),  glm::vec4(1.0f));
            // std::cout << "209" << std::endl;
            // std::cout << "211" << std::endl;

            objects.push_back(rocket);
            rockets.push_back(rocket);
            registerCollisionBody(rocket, false, 0);
            rocket->setSpeed(glm::vec3(0.0f, 1.0f, 0.0f));

            // std::cout << "198" << std::endl;

            particles = new ParticleGenerator(shader, ResourceManager::GetTexture("fire"), GLuint(5));

        }

        if (this->Keys[GLFW_KEY_W] == true)
            boss->ProcessKeyboard(FORWARD, dt, this->Keys[GLFW_KEY_W]);
        else if (this->Keys[GLFW_KEY_S] == true)
            boss->ProcessKeyboard(BACKWARD, dt, this->Keys[GLFW_KEY_S]);
        else if (this->Keys[GLFW_KEY_A] == true)
            boss->ProcessKeyboard(LEFT, dt, this->Keys[GLFW_KEY_A]);
        else if (this->Keys[GLFW_KEY_D] == true)
            boss->ProcessKeyboard(RIGHT, dt, this->Keys[GLFW_KEY_D]);
        else {
            if (boss->type == PLAYER) {

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
            GameBodyBase *bullet = new GameBodyBase(OTHER, player->position + player->front,  player->pitch, player->yaw, player->roll,  glm::vec3(0.5, 0.5, 0.5));
            // GameBodyBase *bullet = new GameBodyBase(OTHER, player->position + player->front,  player->pitch, player->yaw, player->roll, player->pitch, player->yaw,  true,  glm::vec3(0.5, 0.5, 0.5));
            registerCollisionBody(bullet);
            // bullet->init();
            bullet->setSpeed(glm::vec3(5.0f * player->front));
            // bullet->acceleration = player->camera.front;
            // bullet->setAcceleration(this->Gravity);
            objects.push_back(bullet);
            bullets.push_back(bullet);

        }
    }
}

void Game::depthRender()
{
    GLfloat aspect = (GLfloat)this->shadowWidth / (GLfloat)this->shadowHeight;
    // GLfloat near = 0.1f;
    // GLfloat far = 100.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, rendernear, renderfar); // 太远的东西也会被剪裁掉
    std::vector<glm::mat4> shadowTransforms;
    // 现在先创建一个光源的，这个貌似就比较复杂了。。
    // TODO: 增加多光源
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
    RenderScene(depthShader);

    for (auto iter = bullets.begin(); iter != bullets.end(); iter++)
    {
        renderObject("cube", depthShader, (*iter));
    }

    for (auto iter = players.begin(); iter != players.end(); iter++)
    {
        renderObject("player", depthShader, (*iter));
    }

    // FIXME: 换位置
    if (rocket != NULL) {
        renderObject("rocket", depthShader, rocket);
        particles->draw();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Game::Render()
{
    // GLfloat near = 0.1f; //FIXME: 这两个其实重复了，改了一个别忘记该另外一个，在depthRender里面
    // GLfloat far = 5000.0f;
    depthRender();
    // 渲染普通的场景
    glViewport(0, 0, this->Width * 2, this->Height * 2);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.Use();
    glm::mat4 projection = glm::perspective(boss->camera.zoom, (float)this->Width / (float)this->Height, rendernear, renderfar);
    glm::mat4 view = boss->camera.GetViewMatrix();
    shader.SetMatrix4("projection", projection);
    shader.SetMatrix4("view", view);
    shader.SetVector3f("lightPos", this->lights[0].position.x, this->lights[0].position.y, this->lights[0].position.z);
    shader.SetVector3f("viewPos", this->boss->camera.position.x, this->boss->camera.position.y, this->boss->camera.position.z);

    shader.SetInteger("shadows", 1);
    shader.SetFloat("far_plane", renderfar); // FIXME: 这个float非常重要
    glActiveTexture(GL_TEXTURE0);
    ResourceManager::GetTexture("woodTexture").Bind();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->depthCubemap);
    RenderScene(shader);

    for (auto iter = bullets.begin(); iter != bullets.end(); iter++)
    {
        renderObject("cube", shader, (*iter));
    }

    for (auto iter = players.begin(); iter != players.end(); iter++)
    {
        renderObject("player", shader, (*iter));
        // std::cout << "317" << std::endl;

    }

    glActiveTexture(GL_TEXTURE0);
    ResourceManager::GetTexture("rocket").Bind();
    for (auto iter = rockets.begin(); iter != rockets.end(); iter++) {
        // std::cout << "have huojian" << std::endl;
        renderObject("rocket", shader, (*iter));
    }
    // std::cout << rockets.size() << std::endl;
    if (rocket != NULL) {
        glActiveTexture(GL_TEXTURE0);
        ResourceManager::GetTexture("fire").Bind();
        particles->draw();
    }

}

void Game::RenderScene(Shader &sh)
{
    // 房间的cube
    // GLfloat size = 1000.0f;

    // glDisable(GL_CULL_FACE); // Note that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling methods.
    // sh.SetInteger("reverse_normals", 1);
    // // 这是一种特殊的cube，最大的那个cube，特殊处理
    // renderObject("cube", sh, room);
    // sh.SetInteger("reverse_normals", 0);
    // glEnable(GL_CULL_FACE);

}

void Game::addObjectType(std::string name)
{
    GLuint VAO = 0;
    GLuint VBO = 0;
    if (name == "cube")
    {
        GLfloat vertices[] = {
            // Back face
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
            // Front face
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            // Left face
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
            // Right face
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-left
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-right
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-left
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-left
            // Bottom face
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            // Top face
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
        };
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        // Fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // Link vertex attributes
        glBindVertexArray(VAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        VAOmap[name] = VAO;
        modelSizeMap[name] = 36;
    }
    if (name == "rocket")
    {
        std::vector<glm::vec3> verts;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;
        loadOBJ("./rocket3.obj", verts, uvs, normals);
        int modelSize = verts.size();
        // std::cout << "vert : " << verts.size() << "normals : " << normals.size() << "uvs: " <<uvs.size()<<std::endl;
        GLfloat *vertices = new GLfloat[modelSize * 8];
        for (int i = 0; i < modelSize; i++)
        {
            vertices[i * 8 + 0] = verts[i].x;
            vertices[i * 8 + 1] = verts[i].y;
            vertices[i * 8 + 2] = verts[i].z;
            vertices[i * 8 + 3] = normals[i].x;
            vertices[i * 8 + 4] = normals[i].y;
            vertices[i * 8 + 5] = normals[i].z;
            vertices[i * 8 + 6] = uvs[i].x;
            vertices[i * 8 + 7] = uvs[i].y;
        }
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        // Fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * modelSize * 8, vertices, GL_STATIC_DRAW);
        // Link vertex attributes
        glBindVertexArray(VAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        VAOmap[name] = VAO;
        modelSizeMap[name] = modelSize;
    }
    if (name == "player")
    {
        std::vector<glm::vec3> verts;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;
        loadOBJ("./face.obj", verts, uvs, normals);
        int modelSize = verts.size();
        // std::cout << "vert : " << verts.size() << "normals : " << normals.size() << "uvs: " <<uvs.size()<<std::endl;
        GLfloat *vertices = new GLfloat[modelSize * 8];
        for (int i = 0; i < modelSize; i++)
        {
            vertices[i * 8 + 0] = verts[i].x;
            vertices[i * 8 + 1] = verts[i].y;
            vertices[i * 8 + 2] = verts[i].z;
            vertices[i * 8 + 3] = normals[i].x;
            vertices[i * 8 + 4] = normals[i].y;
            vertices[i * 8 + 5] = normals[i].z;
            vertices[i * 8 + 6] = uvs[i].x;
            vertices[i * 8 + 7] = uvs[i].y;
        }
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        // Fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * modelSize * 8, vertices, GL_STATIC_DRAW);
        // Link vertex attributes
        glBindVertexArray(VAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        VAOmap[name] = VAO;
        modelSizeMap[name] = modelSize;
    }
}
void Game::renderObject(const std::string &name, Shader &sh, GameBodyBase* object)
{
    // TODO: add ambient light etc.
    glm::mat4 model = glm::mat4();
    model = glm::translate(model, object->position);
    model = glm::scale(model, object->size);

    model = glm::rotate(model, object->type == ROCKET ? glm::radians(object->pitch - 90.0f) : glm::radians(object->pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, (object->type == ROCKET ? glm::radians(object->yaw) : glm::radians(object->yaw + 90.0f)), glm::vec3(0.0f, 1.0f, 0.0f));


    sh.SetMatrix4("model", model);
    GLuint VAO = VAOmap[name];



    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, modelSizeMap[name]);
    glBindVertexArray(0);
}

void Game::registerCollisionBody(GameBodyBase *obj, bool rest, float gravityScale) {
    q3BodyDef bodyDef;
    bodyDef.position = q3Vec3(obj->position.x, obj->position.y, obj->position.z);
    if (!rest) {
        bodyDef.bodyType = q3BodyType::eDynamicBody;
    }
    bodyDef.gravityScale = gravityScale;

    q3Body* body = scene.CreateBody( bodyDef );
    q3BoxDef boxDef; // See q3Box.h for settings details
    q3Transform trans; // Contains position and orientation, see q3Transform.h for details
    // q3Identity( localSpace ); // Specify the origin, and identity orientation
    trans.position = q3Vec3(0.f, 0.f, 0.f);
    double v[9] = {0.0};
    const float *pSource = (const float*)glm::value_ptr(obj->rotationMatrix);
    for (int i = 0; i < 9; ++i)
        v[i] = pSource[i];
    trans.rotation = q3Mat3(v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8]);

    // Create a box at the origin with width, height, depth = (1.0, 1.0, 1.0)
    // and add it to a rigid body. The transform is defined relative to the owning body
    boxDef.Set( trans, q3Vec3( obj->size[0], obj->size[1], obj->size[2] ) );
    body->AddBox( boxDef );
    obj->body = body;
}
