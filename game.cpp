#include "game.h"
#include "resource_manager.h"
// #include "box_renderer.h"
#include <vector>
#include <algorithm>
#include <SOIL.h>
// std::vector<BoxRenderer*> renderers;

Game::Game(GLuint width, GLuint height) 
	: State(GAME_ACTIVE), Keys(), Width(width), Height(height), player(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.3f, 0.5f, 0.3f))
{ 

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
    this->cubeVAO = 0;
    this->cubeVBO = 0;
    ResourceManager::LoadTexture("./wood.png", false, "woodTexture");
    // FIXME: add appropriate position
    // ResourceManager::LoadShader("shaders/lighting.vs", "shaders/lighting.frag", nullptr, "lighting");
    ResourceManager::LoadShader("shaders/lamp.vs", "shaders/lamp.frag", nullptr, "lamp");
    ResourceManager::LoadShader("shaders/point_shadows.vs", "shaders/point_shadows.frag", nullptr, "point_shadows");
    ResourceManager::LoadShader("shaders/point_shadows_depth.vs", "shaders/point_shadows_depth.frag", "shaders/point_shadows_depth.gs","point_shadows_depth");
    // renderers.push_back(new BoxRenderer(ResourceManager::GetShader("lighting")));
    // renderers.push_back(new BoxRenderer(ResourceManager::GetShader("lamp")));
    // lights.push_back(new Box(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.5, 0.5, 0.5)));
    // bulletShader = ResourceManager::GetShader("lighting");
    lightShader = ResourceManager::GetShader("lamp");
    // TODO: new added shader
    shader = ResourceManager::GetShader("point_shadows");
    depthShader = ResourceManager::GetShader("point_shadows_depth");
    this->initDepthMap();
    this->lights.push_back(Light(glm::vec3(0.0f)));

    Box* bullet = new Box(glm::vec3(0.0f, -7.0f, 0.0f), glm::vec3(100, 10, 100), glm::vec3(1.0f, 0.0f, 0.0f));
    // bullets[0] is the ground
    bullet->init();
    bullets.push_back(bullet); 
    
    // bullets[1] is the player
    // player.addAcceleration(this->Gravity);
    bullets.push_back(&player);
    Box * newBullet = new Box(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(10, 10, 10), glm::vec3(1.0f, 0.0f, 0.0f));
    newBullet->init();
    bullets.push_back(newBullet);
    addObjectType("cube");
    addObjectType("rocket");
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

void Game::Update(GLfloat dt)
{
    // TODO: 
    // in Game::Update
    // set shader's projection, view, clip matrix 
    // set ResourceManager::GetShader("...").Use() 
    // set each shaders' attribute. 
    glm::mat4 model;
    // this->lights[0].position = glm::vec3(glm::rotate(model, dt, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(this->lights[0].position,1.0f)); 
    // lights[0]->rotate(dt, glm::vec3(0.0f, 1.0f, 0.0f));
    for (auto a: bullets) {
        for(int j = 0; j < 6; j++)
            a->stuck[j] = false;
    }
    lights[0].position = this->player.position;
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

void Game::initDepthMap(){
    shader.Use();
    shader.SetInteger("diffuseTexture",0);
    shader.SetInteger("depthMap", 1);
    this->shadowWidth = 1024;
    this->shadowHeight = 1024;
    glGenFramebuffers(1, &this->depthMapFBO);
    glGenTextures(1, &this->depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->depthCubemap);
    for (GLuint i = 0; i < 6; i++){
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
    }
}
void Game::depthRender(){
    GLfloat aspect = (GLfloat)this->shadowWidth / (GLfloat)this->shadowHeight;
    GLfloat near = 1.0f;
    GLfloat far = 100.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f),aspect, near, far); // 太远的东西也会被剪裁掉
    std::vector<glm::mat4> shadowTransforms;
    // 现在先创建一个光源的，这个貌似就比较复杂了。。
    // TODO: 增加多光源
    shadowTransforms.push_back(shadowProj * glm::lookAt(lights[0].position, lights[0].position + glm::vec3( 1.0,  0.0,  0.0), glm::vec3(0.0, -1.0,  0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lights[0].position, lights[0].position + glm::vec3(-1.0,  0.0,  0.0), glm::vec3(0.0, -1.0,  0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lights[0].position, lights[0].position + glm::vec3( 0.0,  1.0,  0.0), glm::vec3(0.0,  0.0,  1.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lights[0].position, lights[0].position + glm::vec3( 0.0, -1.0,  0.0), glm::vec3(0.0,  0.0, -1.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lights[0].position, lights[0].position + glm::vec3( 0.0,  0.0,  1.0), glm::vec3(0.0, -1.0,  0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lights[0].position, lights[0].position + glm::vec3( 0.0,  0.0, -1.0), glm::vec3(0.0, -1.0,  0.0)));
    // 这里是先把场景搞到depth cubemap上面
    glViewport(0,0,this->shadowWidth, this->shadowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, this->depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        depthShader.Use();
        std::cout << "far : " << far << std::endl;
        for(GLuint i = 0; i < 6; i++){
            depthShader.SetMatrix4(("shadowTransforms[" + std::to_string(i) + "]").c_str(), shadowTransforms[i]);
        }
        // far 是用来控制裁剪空间远近的
        depthShader.SetFloat("far_plane", far);
        depthShader.SetVector3f("lightPos", this->lights[0].position.x, this->lights[0].position.y, this->lights[0].position.z);
        // 然后用这个shader渲染所有东西 
        RenderScene(depthShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
}
void Game::Render()
{
    GLfloat near = 1.0f; //FIXME: 这两个其实重复了，改了一个别忘记该另外一个，在depthRender里面
    GLfloat far = 200.0f;
    depthRender();
    // 渲染普通的场景
    glViewport(0, 0, this->Width, this->Height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.Use();
    glm::mat4 projection = glm::perspective(player.camera.zoom, (float)this->Width / (float)this->Height, 0.1f, 100.0f);
    glm::mat4 view = player.camera.GetViewMatrix();
    shader.SetMatrix4("projection", projection);
    shader.SetMatrix4("view", view);
    shader.SetVector3f("lightPos",this->lights[0].position.x, this->lights[0].position.y, this->lights[0].position.z);
    shader.SetVector3f("viewPos", this->player.camera.position.x, this->player.camera.position.y, this->player.camera.position.z);

    shader.SetInteger("shadows", 1); 
    shader.SetFloat("far_plane",far);// FIXME: 这个float非常重要
    glActiveTexture(GL_TEXTURE0);
    ResourceManager::GetTexture("woodTexture").Bind();
    // glBindTexture(GL_TEXTURE_2D, this->woodTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->depthCubemap);
    RenderScene(shader);


}

void Game::RenderScene(Shader &sh){
    // 房间的cube
    GLfloat size = 100.0f;

    glDisable(GL_CULL_FACE); // Note that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling methods.
    sh.SetInteger("reverse_normals",1);
    // 这是一种特殊的cube，最大的那个cube，特殊处理
    renderObject("cube", sh, glm::vec3(0.0f, size / 2, 0.0f), glm::vec3(100.0f));
    sh.SetInteger("reverse_normals",0);
    glEnable(GL_CULL_FACE);
    // 具体的小cube
    for (auto iter = bullets.begin(); iter != bullets.end(); iter++)
    {
        renderObject("rocket", sh, (*iter)->position, glm::vec3(1.5f));
    }
    
}

void Game::addObjectType(std::string name){
    GLuint VAO = 0;
    GLuint VBO = 0;
    if(name == "cube"){
        GLfloat vertices[] = {
            // Back face
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
            0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
            -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
            // Front face
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
            0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
            // Left face
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
            -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            // Right face
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right         
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left     
            // Bottom face
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
            0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
            -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            // Top face
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right     
            0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left        
        };
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        // Fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // Link vertex attributes
        glBindVertexArray(VAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        VAOmap[name] = VAO;
        modelSizeMap[name] = 36;
    }
    if(name == "rocket"){
        std::vector<glm::vec3> verts;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;
        loadOBJ("./face.obj",verts,uvs, normals);
        int modelSize = verts.size();
        // std::cout << "vert : " << verts.size() << "normals : " << normals.size() << "uvs: " <<uvs.size()<<std::endl;
        GLfloat* vertices = new GLfloat[modelSize * 8];
        for(int i = 0; i < modelSize; i++){
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
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        VAOmap[name] = VAO;
        modelSizeMap[name] = modelSize;
    }
    

}
void Game::renderObject(const std::string& name, Shader& sh, const glm::vec3& position, glm::vec3 scale,  glm::vec3 color){
    // TODO: add ambient light etc.
    glm::mat4 model = glm::mat4();
    model = glm::translate(model, position);
    model = glm::scale(model, scale);
    sh.SetMatrix4("model", model);
    GLuint VAO = VAOmap[name];
    if(name == "cube"){
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, modelSizeMap["cube"]);
        glBindVertexArray(0);
    }
    if(name == "rocket"){
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, modelSizeMap["rocket"]);
        glBindVertexArray(0);
    }
}
