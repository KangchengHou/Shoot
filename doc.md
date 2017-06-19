# CG Project Design Documentation

​													by 王书琪  侯康成  魏天昊

### 一、游戏概述

我们设计了一款第一人称射击类游戏，游戏中有五门大炮不间断的向玩家发射炮弹，玩家需要控制着角色发射光波击中炮弹或进行躲避。

### 二、游戏说明

玩家在游戏中使用鼠标控制视角方向，WSAD控制角色移动（W键是往视角方向前进），空格键跳起。滚轮键控制场景缩放。

按下回车键时可以发射火箭进行空中巡游。发射火箭后可以使用Tab键在角色与火箭之间自由切换视角。

当切换到火箭视角时，可以使用WSAD控制火箭的朝向，鼠标控制环绕视角。火箭速度固定。

### 三、运行环境和配置

支持的操作系统为Mac OS与Linux

所需要预先安装的库依赖有：GLFW、SOIL、ASSIMP、irrklang

### 四、游戏运行流程



![1](1.png)



### 五、设计要点说明

* 基本体素建模表达

> 使用三角形拼接而成的方法表达体素。
> 
> 如游戏中现代枪发出的“射线”。

* 网格导入导出

> 由于obj文件非常规整，如v开头代表定点，f开头代表面等，便可按照其语法写一个很简单的“解析器”把文件读进来，再进行相应的“配对”。
> 
> 有独立的loadOBJ函数可导入.obj模型。在游戏中炮弹等都是由该函数导入的。

* 材质纹理的显示与编辑



* 几何变换

> 每个物体的类中都保存着该物体的位置、旋转角、缩放等信息。在render的时候会将用这些信息生成变换矩阵传给shader，由shader进行绘制。
> 
> 游戏中处处体现着几何变换，如火箭形态的改变，导弹空中飞行等。

* 光照渲染与光源编辑



* 场景漫游与碰撞检测

> 游戏中使用了轻量级物理引擎，将所有物体都使用长方体包起来进行碰撞检测与受力分析。碰撞检测采用obb算法，在两个物体六个平行轴上进行投影，投影全不重叠则为不碰撞，否则即为碰撞。
> 
> 如炮弹和枪的射线之间的碰撞检测，炮弹和地面的碰撞检测，人和大炮之间的碰撞检测等。

### 六、程序文件结构



### 七、主要代码文件说明

各文件说明如下：

#### program.cpp

1. 负责opengl的初始化，窗口设置，
2. 鼠标，键盘的回调
3. 程序主体循环调用Game::ProcessInput, Game::Update, Game::Render

#### game.cpp

game控制着游戏的主体循环。

1. 加载纹理、模型、染色器
2. 添加玩家、敌人、建筑物，会生成一个
3. 将输入转化为游戏状态
4. 利用物理引擎更新物体的位置
5. 渲染场景与物体

```c++
	Game(GLuint width, GLuint height);
    ~Game();	
	void Init();
    // GameLoop
    void ProcessInput(GLfloat dt);
    void Update(GLfloat dt);
    void Render();
    GLuint loadTexture(GLchar const * path);
    void depthRender();
    void initDepthMap();
    void RenderScene();
    void renderEnvironment();
    void addObjectType(std::string name);
    void renderObject(const std::string& name, Shader& sh, GameBodyBase* object);
    void ProcessMouseMovement(double xoffset, double yoffset);
    void registerCollisionBody(GameBodyBase *obj, bool rest = false, float gravityScale = 1.0);
    GameBodyBase* addObject(OBJECTTYPE type,
                 glm::vec3 position,
                 GLfloat yaw,
                 GLfloat pitch,
                 GLfloat roll,
                 GLfloat scale,
                 bool visible = true,
                 bool rest = false,
                 GLfloat gravityScale = 1.0);
```

#### game_body_base.cpp

定义了Camera类和GameBodyBase类。

Camera类中记录着照相机属性，包括照相机相对于物体的位置、角度、缩放等值与更新函数。

GameBodyBase是游戏中任何物体的基类，记录着该物体的照相机位属性、物理属性和更新函数。还记录着物体对应的模型信息，包括使用模型的名称、缩放比例等。

```c++

class Camera
{
public:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;

    GLfloat zoom = 1;
    GLfloat MouseSensitivity;

    GLfloat cameraposyaw;
    GLfloat camerapospitch;
    GLfloat cameraposradius = 5.0f;

    Camera(glm::vec3 targetpos, glm::vec3 targetfront, GLfloat targetyaw, GLfloat targetpitch);
    void updateSelfAnglesAccordingToTargetAngles(GLfloat targetyaw, GLfloat targetpitch);
    void updateCameraPosition();
    void updateTargetPosition(glm::vec3 targetpos);
    void updateBaseVectorsAccordingToSelfAngles();
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset);
    glm::mat4 GetViewMatrix();
};

class GameBodyBase
{
public:
    std::string renderType;
    q3Body* body = NULL;
    // GameBodyBase Attributes
    OBJECTTYPE type; 
    GLfloat scale;
    glm::vec3 color;
    glm::vec3 offset = glm::vec3(0, 0, 0);
    double life = INITIAL_LIFE;
    double rest_life = INITIAL_LIFE;

    glm::vec3 acceleration;
    glm::vec3 speed;
    glm::vec3 position;

    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    bool visible;

    Camera camera;
    ParticleGenerator* particleGenerator = NULL;
    // Eular Angles
    GLfloat pitch; // x
    GLfloat yaw;   // y
    GLfloat roll;  // z
    glm::mat3 rotationMatrix;


    GLfloat yawspeed;
    GLfloat pitchspeed;
    const GLfloat yawacc = YAWACC;
    const GLfloat pitchacc = PITCHACC;

    const GLfloat MovementSpeed;

    GLfloat MouseSensitivity;
	GameBodyBase(OBJECTTYPE type,
                 glm::vec3 position,
                 GLfloat yaw,
                 GLfloat pitch,
                 GLfloat roll,

                 // GLfloat renderyaw,
                 // GLfloat renderpitch,
                 // GLboolean synchro,
                 GLfloat scale,
                 bool visible = true,
                 glm::vec3 color = glm::vec3(1, 1, 1)
                                   // ,GLfloat zoom = 1
                );
    ~GameBodyBase();

    void setSpeed(glm::vec3 newSpeed);

    void setAcceleration(glm::vec3 newAcceleration);
    glm::mat3 euler2matrix(double pitch, double yaw, double roll);
  	void setPos(double x, double y, double z);
  	void ProcessKeyboard(GameBodyBase_Movement direction, GLfloat deltaTime, GLboolean pressed);
  	void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true);
  	void updateBaseVectorsAccordingToSelfAngles();
  
```

#### resource_manager.cpp

负责各种素材的加载与管理工作。是静态类，全局只能声明一次。管理着染色器、纹理、导入模型、顶点绑定器等。

使用模型时需先用ResourceManager加载之后才可以使用。

```c++
class ResourceManager
{
public:
    // Resource storage
    static std::map<std::string, Shader>    Shaders;
    static std::map<std::string, Texture2D> Textures;
    static std::map<std::string, Model*> LoadedModels;
    static std::map<std::string, GLuint> VAOmap;
    static std::map<std::string, int> VAOSizeMap;
    static std::map<std::string, glm::vec3> modelSizeMap;
    
    // Loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader
    static Shader   LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, std::string name);
    // Retrieves a stored sader
    static Shader   GetShader(std::string name);
    // Properly de-allocates all loaded resources
    static void      Clear();

    static Texture2D LoadTexture(const GLchar *file, GLboolean alpha, std::string name);
    // Retrieves a stored texture
    static Texture2D GetTexture(std::string name);
    static void addObjectType(std::string name);
    static void loadModel(std::string name);
private:
    // Private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
    ResourceManager() { }
    // Loads and generates a shader from file
    static Shader    loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile = nullptr);
    static Texture2D loadTextureFromFile(const GLchar *file, GLboolean alpha);
};

```

#### objloader.cpp

负责加载模型，传入从.obj文件中读取的vertices, uvs, normals数组即可将模型加载到内存中。

```c++
bool loadOBJ(
    const char * path,
    std::vector<glm::vec3> & out_vertices,
    std::vector<glm::vec2> & out_uvs,
    std::vector<glm::vec3> & out_normals
);
```
#### 材质纹理的显示与编辑
读入texture时的步骤，大致就是。
1. 读入图片，存为`unsigned int `
2. 生成纹理，设置一些纹理的参数，比如如果超出界限纹理该怎么显示以及mipmap的形式，
```C
glBindTexture(GL_TEXTURE_2D, this->ID);
glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
// Set Texture wrap and filter modes
glGenerateMipmap(GL_TEXTURE_2D);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
// Unbind texture
glBindTexture(GL_TEXTURE_2D, 0);
```

3.使用纹理的时候，只需要绑定某个特定的纹理到opengl的上下文，同时配合好着色器，在着色器当中配置好哪个顶点对应纹理当中的哪个目标。过程当中的关键代码如下。
```C
glBindTexture(GL_TEXTURE_2D, TextureID);
```
```C
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse;

void main()
{
    FragColor = texture(texture_diffuse, TexCoords);
}
```
如上，我们就可以将材质运用到物体上面了。
####光源渲染与光源编辑
对于光源有两个方面需要考虑，一个是如何使一个被光射到的物体真实地反映出他被光射到了，另外一个就是如何将阴影渲染出来。
对于物体如何被光射到，我们使用了Blinn-Phong光照模型，这是一个比较简单，但是也能比较真实反映现实光照条件的模型，在这个模型当中材质有四个与光照相关的属性，ambient, diffuse, specular, shininess, 灯光有三个与光照相关的属性，ambient，diffuse，specular。计算光照的时候，对于每个像素，只要计算这个像素点和光源的位置关系，接着计算ambient，diffuse，specular三个分量就可以了。
```C
#version 330 core
// 材质的属性
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular; 
	float shininess;
}; 

// 灯光的属性
struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 FragPos; 
in vec3 Normal; 

out vec4 color;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
	// Ambient
	vec3 ambient = light.ambient * material.ambient;

	// Diffuse 
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	// Specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir); 
	spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);	
	vec3 specular = light.specular * (spec * material.specular);

	vec3 result = ambient + diffuse + specular;
	color = vec4(result, 1.0f);
} 
```
对于阴影来说，我们使用的是一个cubeMap，首先从光源的位置渲染整个场景，从中得知哪些地方是能被光源所看到的，哪些不能，从这里我们就可以知道哪些地方是处于阴影之中的，得到这个信息之后，我们再进行一遍正常的渲染，这个过程中，我们是可以知道哪些地方是有阴影的，这时候就可以渲染出阴影来了。


```C
 GLfloat aspect = (GLfloat)this->shadowWidth / (GLfloat)this->shadowHeight;
// 设置几何着色器，将场景渲染到cubeMap的六个面上
glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, rendernear, renderfar); 
std::vector<glm::mat4> shadowTransforms;

shadowTransforms.push_back(shadowProj * glm::lookAt(lights[0].position, lights[0].position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
shadowTransforms.push_back(shadowProj * glm::lookAt(lights[0].position, lights[0].position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
shadowTransforms.push_back(shadowProj * glm::lookAt(lights[0].position, lights[0].position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
shadowTransforms.push_back(shadowProj * glm::lookAt(lights[0].position, lights[0].position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
shadowTransforms.push_back(shadowProj * glm::lookAt(lights[0].position, lights[0].position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
shadowTransforms.push_back(shadowProj * glm::lookAt(lights[0].position, lights[0].position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

glViewport(0, 0, this->shadowWidth, this->shadowHeight);
glBindFramebuffer(GL_FRAMEBUFFER, this->depthMapFBO);
glClear(GL_DEPTH_BUFFER_BIT);
depthShader.Use();

for (GLuint i = 0; i < 6; i++)
{
	depthShader.SetMatrix4(("shadowTransforms[" + std::to_string(i) + "]").c_str(), shadowTransforms[i]);
}

depthShader.SetFloat("far_plane", renderfar);
depthShader.SetVector3f("lightPos", this->lights[0].position.x, this->lights[0].position.y, this->lights[0].position.z);

RenderAllObjects();
glBindFramebuffer(GL_FRAMEBUFFER, 0);
```
现在我们就获得了场景当中哪些地方是处于阴影之中的，接下来只要在渲染的过程当中加入阴影就可以了。
渲染的过程当中，首先将之前获得的深度frame绑定到材质当中，接着根据得到的深度图知道当前点是否，选择是否当前这个fragment是否在阴影之中，如果当前这个fragment在阴影之中，那么这个fragment就表现的像是只有ambient分量，否则就按正常的光照模型来计算。着色器的代码大致如下。
```C
#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseTexture;
uniform samplerCube depthMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;
uniform bool shadows;


float ShadowCalculation(vec3 fragPos)
{

    vec3 fragToLight = fragPos - lightPos;

    float closestDepth = texture(depthMap, fragToLight).r;

    closestDepth *= far_plane;
    float currentDepth = length(fragToLight);
    float bias = 0.05;
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
// 如果对于光源，当前点在最前面，那么就不在阴影中，否则就处于阴影之中
    return shadow;
}

void main()
{ 
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    // Ambient
    vec3 ambient = 0.3 * color;
    // Diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // 如果shadows是true，那么只渲染ambient，否则diffuse和specular分量都存在
    float shadow = shadows ? ShadowCalculation(fs_in.FragPos) : 0.0;
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0f);
}
```
如上，我们就可以渲染出带有阴影的材质来了。
#### 参考资料


