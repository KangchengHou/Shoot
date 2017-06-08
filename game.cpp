#include "game.h"
#include "resource_manager.h"
// #include "box_renderer.h"
#include <vector>
#include <algorithm>
#include <SOIL/SOIL.h>
#include "physics/q3.h"


void Game::registerCollisionBody(GameBodyBase *obj, bool rest) {
    q3BodyDef bodyDef;
    bodyDef.position = q3Vec3(obj->position.x, obj->position.y, obj->position.z);
    if(!rest) {
        bodyDef.bodyType = q3BodyType::eDynamicBody;
    }
    

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
Game::Game(GLuint width, GLuint height)
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{
    // gravity = rp3d::Vector3(0.0, -9.81, 0.0);
    // world.setGravity(gravity);
    // // Change the number of iterations of the velocity solver
    // world.setNbIterationsVelocitySolver(15);
    // // Change the number of iterations of the position solver
    // world.setNbIterationsPositionSolver(8);


    rendernear = 0.1f;
    renderfar = 5000.0f;
    
    player = new GameBodyBase(PLAYER, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.3f, 0.5f, 0.3f));
    // objects.push_back(player);
    // registerCollisionBody(player);
    players.push_back(player);
    boss = player;

    GLfloat size = 1000.0f;
    room = new GameBodyBase(OTHER, glm::vec3(0.0f, size / 2, 0.0f), glm::vec3(size));
    // objects.push_back(room);

    GameBodyBase* ground = new GameBodyBase(OTHER, glm::vec3(0.0f, -100/2, 0.0f), glm::vec3(100));
    objects.push_back(ground);
    bullets.push_back(ground);
    registerCollisionBody(ground, true);
}
void Game::ProcessMouseMovement(double xoffset, double yoffset)
{
    if (this->State == GAME_ACTIVE)
    {
        this->boss->ProcessMouseMovement(xoffset, yoffset);
    }
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
bool Game::aabbTest(GameBodyBase *a, GameBodyBase *b)
{
    for (int i = 0; i < 3; i++)
    {
        // printf("%.0lf %.0lf\n", a->position[i] + a->size[i]/2 , b->position[i] - b->size[i]/2);
        // printf("%.0lf %.0lf\n", b->position[i] + b->size[i]/2 , a->position[i] - a->size[i]/2);
        if (a->position[i] + a->size[i] / 2 < b->position[i] - b->size[i] / 2 ||
                b->position[i] + b->size[i] / 2 < a->position[i] - a->size[i] / 2)
            return false;
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
    if (fabs(fcos) >= fabs(rcos) && fabs(fcos) >= fabs(ucos))
    {
        a->stuck[0 + (fcos > 0)] = GL_TRUE;
    }
    if (fabs(ucos) >= fabs(rcos) && fabs(ucos) >= fabs(fcos))
    {
        a->stuck[2 + (ucos > 0)] = GL_TRUE;
    }
    if (fabs(rcos) >= fabs(fcos) && fabs(rcos) >= fabs(ucos))
    {
        a->stuck[4 + (rcos > 0)] = GL_TRUE;
    }
    return true;
}
bool Game::obbTest(GameBodyBase *a, GameBodyBase *b)
{
    glm::vec3 av[8];
    glm::vec3 bv[8];
    glm::vec3 axis[6];
    int cnt = 0;
    for (int i = 1; i < 2; i += 2)
        for (int j = -1; j < 2; j += 2)
            for (int k = -1; k < 2; k += 2)
            {
                av[cnt] = glm::vec3(a->position[0] + i * a->size[0] / 2, a->position[1] + j * a->size[1] / 2, a->position[2] + k * a->size[2] / 2);
                bv[cnt++] = glm::vec3(b->position[0] + i * b->size[0] / 2, b->position[1] + j * b->size[1] / 2, b->position[2] + k * b->size[2] / 2);
            }

    glm::mat4 model;
    // axis[0] = glm::vec3(glm::rotate(model, a->angle[0], axis)
    //             * glm::vec4(position,1.0f));
    for (int i = 0; i < 6; i += 2)
    {
        axis[i] = av[i] - av[i + 1];
        axis[i + 1] = bv[i] - bv[i + 1];
    }
    double minA, maxA, minB, maxB, ap, bp;
    for (int i = 0; i < 6; i++)
    {
        minA = minB = 1e9;
        maxA = maxB = -1e9;
        for (int j = 0; j < 8; j++)
        {
            ap = glm::dot(av[j], axis[i]);
            bp = glm::dot(av[j], axis[i]);
            minA = std::min(minA, ap);
            maxA = std::max(maxA, ap);
            minB = std::min(minB, bp);
            maxB = std::max(maxB, bp);
        }
        if (minA > maxB || minB > maxA)
            return false;
    }
    return true;
}

void Game::Update(GLfloat dt)
{
    // FIXME: add sth. for rocket
    // puts("update");
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
    // printf("%d\n", acc);
    if (rocket != NULL) {
        particles->update(dt, *rocket, 20, glm::vec3(0.0f));
    } else {

    }

    glm::mat4 model;
    // this->lights[0].position = glm::vec3(glm::rotate(model, dt, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(this->lights[0].position,1.0f));
    // lights[0]->rotate(dt, glm::vec3(0.0f, 1.0f, 0.0f));
    // for (auto a : objects)
    // {
    //     for (int j = 0; j < 6; j++)
    //         a->stuck[j] = false;
    // }
    lights[0].position = this->boss->camera.position;
    bool flag = false;

    int cnt = 0;
    for (auto iter = objects.cbegin(); iter != objects.cend(); iter++)
    {
        cnt++;
        q3Transform trans = (*iter)->body->GetTransform();
        q3Vec3 pos = trans.position;
        q3Mat3 rot = trans.rotation;
        // printf("%d ", cnt);
        // (*iter)->body->prtflags();
        // printf("q3  %.2lf %.2lf %.2lf\n", pos.x, pos.y, pos.z);
        // printf("it  %.2lf %.2lf %.2lf\n", (*iter)->position.x, (*iter)->position.y, (*iter)->position.z);
        (*iter)->setPos(pos.x, pos.y, pos.z);
        // if ((*iter)->type == ROCKET) std::cout << (*iter)->position.x << " " <<  (*iter)->position.y << std::endl;
    }
    // printf("%d\n", cnt);
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
        // now in the game state
        // process input for player
        if (this->Keys[GLFW_KEY_TAB] == 1 )
        {
            if (rocket == NULL) {
                std::cout << "no rocket" << std::endl;
            } else if (boss->type == PLAYER) {
                boss = rocket;
            } else if (boss->type == ROCKET) {
                boss = player;
            }
            this->Keys[GLFW_KEY_TAB]++;
        }
        if (this->Keys[GLFW_KEY_ENTER] == 1 || this->Keys[GLFW_KEY_ENTER] == 3)
        {
            if (rocket == NULL) {
                std::cout << "fuck" << std::endl;
                glm::vec3 rocketPos = player->position + glm::vec3(0.0f, 2.0f, 0.0f);
                rocket = new GameBodyBase(ROCKET, rocketPos, glm::vec3(1.0f), glm::vec4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), -89.0f, 89.0f);
                rocket->speed = glm::vec3(0.0f, 1.0f, 0.0f);
                objects.push_back(rocket);
                registerCollisionBody(rocket);
                rockets.push_back(rocket);
                particles = new ParticleGenerator(shader, ResourceManager::GetTexture("fire"), GLuint(500));

            }
        }
        boss->speed = boss->worldUp * glm::dot(boss->speed, boss->worldUp);
        if (this->Keys[GLFW_KEY_W] == 1 || this->Keys[GLFW_KEY_W] == 3)
        {
            // printf("W %d\n", this->Keys[GLFW_KEY_W]);
            boss->ProcessKeyboard(FORWARD, dt, this->Keys[GLFW_KEY_W]);
            // this->Keys[GLFW_KEY_W] ++;
        }
        if (this->Keys[GLFW_KEY_S] == 1 || this->Keys[GLFW_KEY_S] == 3)
        {
            boss->ProcessKeyboard(BACKWARD, dt, this->Keys[GLFW_KEY_S]);
            // this->Keys[GLFW_KEY_S] ++;
        }
        if (this->Keys[GLFW_KEY_A] == 1 || this->Keys[GLFW_KEY_A] == 3)
        {
            boss->ProcessKeyboard(LEFT, dt, this->Keys[GLFW_KEY_A]);
            // this->Keys[GLFW_KEY_A] ++;
        }
        if (this->Keys[GLFW_KEY_D] == 1 || this->Keys[GLFW_KEY_D] == 3)
        {
            boss->ProcessKeyboard(RIGHT, dt, this->Keys[GLFW_KEY_D]);
            // this->Keys[GLFW_KEY_D] ++;
        }

        if (this->Keys[GLFW_KEY_SPACE] == 1)
        {
            boss->addPos(boss->up * 0.1f);
            boss->addSpeed(glm::vec3(5.0f * boss->up));
            boss->setAcceleration(1, 0);
            boss->addAcceleration(this->Gravity);
            this->Keys[GLFW_KEY_SPACE]++;
        }

        if (this->mouse[0] == 1)
        {
            // GameBodyBase *bullet = new GameBodyBase(OTHER, boss->camera.position + boss->camera.front, glm::vec3(0.5, 0.5, 0.5));
            GameBodyBase *bullet = new GameBodyBase(OTHER, player->camera.position + player->camera.front, glm::vec3(0.5, 0.5, 0.5));
            registerCollisionBody(bullet);
            // bullet->init();
            bullet->setSpeed(glm::vec3(5.0f * player->camera.front));
            // bullet->acceleration = player->camera.front;
            // bullet->addAcceleration(this->Gravity);
            objects.push_back(bullet);
            bullets.push_back(bullet);
            this->mouse[0] = 2;
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

    // for (auto iter = bullets.begin(); iter != bullets.end(); iter++)
    // {
    //     renderObject("cube", depthShader, (*iter)->position, glm::vec3(1.5f));
    // }

    for (auto iter = players.begin(); iter != players.end(); iter++)
    {
        renderObject("player", depthShader, (*iter));
    }

    // FIXME: 换位置
    if (rocket != NULL) {
        renderObject("rocket", depthShader, rocket);
        // particles->draw();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Game::Render()
{
    // GLfloat near = 0.1f; //FIXME: 这两个其实重复了，改了一个别忘记该另外一个，在depthRender里面
    // GLfloat far = 5000.0f;
    depthRender();
    // 渲染普通的场景
    #ifdef _APPLE_
    glViewport(0, 0, this->Width * 2, this->Height * 2);
    #else 
    glViewport(0, 0, this->Width, this->Height);
    #endif
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.Use();
    glm::mat4 projection = glm::perspective(boss->camera.zoom, (float)this->Width / (float)this->Height, rendernear, renderfar);
    glm::mat4 view = boss->camera.GetViewMatrix();
    shader.SetMatrix4("projection", projection);
    shader.SetMatrix4("view", view);
    shader.SetVector3f("lightPos", this->lights[0].position.x, this->lights[0].position.y, this->lights[0].position.z);
    shader.SetVector3f("viewPos", this->boss->camera.position.x, this->boss->camera.position.y, this->boss->camera.position.z);
    // std::cout << "camera position : " << this->boss->camera.position.x << " " <<  this->boss->camera.position.y <<" "<< this->boss->camera.position.z << std::endl; 
    shader.SetInteger("shadows", 1);
    shader.SetFloat("far_plane", renderfar); // FIXME: 这个float非常重要
    glActiveTexture(GL_TEXTURE0);
    ResourceManager::GetTexture("woodTexture").Bind();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->depthCubemap);
    RenderScene(shader);
    int rencnt = 0;
    for (auto iter = bullets.begin(); iter != bullets.end(); iter++)
    {
        renderObject("cube", shader, (*iter));
        rencnt ++;

    }
    // printf("render bullets: %d\n", rencnt);

    for (auto iter = players.begin(); iter != players.end(); iter++)
    {
        renderObject("player", shader, (*iter));
    }

    glActiveTexture(GL_TEXTURE0);
    ResourceManager::GetTexture("rocket").Bind();
    for (auto iter = rockets.begin(); iter != rockets.end(); iter++) {
        // std::cout << "have huojian" << std::endl;
        renderObject("rocket", shader, rocket);
    }
    if (rocket != NULL) {
        glActiveTexture(GL_TEXTURE0);
        ResourceManager::GetTexture("fire").Bind();
        // particles->draw();
    }

}

void Game::RenderScene(Shader &sh)
{
    // 房间的cube
    GLfloat size = 1000.0f;

    glDisable(GL_CULL_FACE); // Note that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling methods.
    sh.SetInteger("reverse_normals", 1);
    // 这是一种特殊的cube，最大的那个cube，特殊处理
    // renderObject("cube", sh, room);
    sh.SetInteger("reverse_normals", 0);
    glEnable(GL_CULL_FACE);

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
    if(name == "rocket"){
        model = glm::translate(model, object->position);
        model = glm::scale(model, object->size);
        model = glm::rotate(model, glm::radians(object->selfpitch - 90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(object->selfyaw), glm::vec3(0.0f, 1.0f, 0.0f));
    }else{
        model = glm::translate(model, object->position);
        model = glm::scale(model, object->size);
        model = glm::rotate(model, glm::radians(object->selfpitch), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(object->selfyaw), glm::vec3(0.0f, 1.0f, 0.0f));   
    }



    sh.SetMatrix4("model", model);
    GLuint VAO = VAOmap[name];


    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, modelSizeMap[name]);
    glBindVertexArray(0);
}
