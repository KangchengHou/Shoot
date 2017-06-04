# Shoot
Game implemented with OpenGL
## TODO
* [ ] 多光源、多阴影 

## References 
1. [learnopengl](www.learnopengl.com)
2. [nehe physcis engine tutorial](http://nehe.gamedev.net/tutorial/introduction_to_physical_simulations/18005/)
3. [openspades 很好的游戏](https://github.com/yvt/openspades)
4. [一个简单的物理引擎　可以借鉴一下](https://github.com/rools/engine)
5. [另外一个游戏引擎](https://github.com/Shervanator/Engine)
6. [又是另外一个](https://github.com/dimi309/small3d)

## Design
1. program.cpp 是主程序所在位置。其中设置了glfw 和 glEnable 的一些属性。
2. Game　里就是整个游戏，Init 函数负责初始化
3. ResouceManager 负责文件加载，Shader, Texture等。
4. 每一个object(箱子，灯光)有一个对应的renderer,每个renderer一一对应一个shader。shader中可以设置各种属性，renderer表示一种渲染的方式。每个object中储存一个renderer,渲染每个object的时候讲他的坐标传到对应的renderer里面.
5. 环境贴图 environment maps 当一个火箭射过来了，可以从那边看到我们的脸。
6.  

## Requirements
* [ ] [10 分]基于 OpenGL，具有基本体素（立方体、球、圆柱、圆锥、多面棱柱、多面棱台）的建模表达能力；   这个我们后期加入就可以了，现在不需要放进去。
* [ ] [10 分]具有基本三维网格导入导出功能（建议 OBJ 格式)；  导入火箭OBJ就可以了
* [ ] [15 分]具有基本材质、纹理的显示和编辑能力；  可以铺一个瓷砖等，所以材质还是需要的，要把这个加回去。
* [X] ~~*[10 分]具有基本几何变换功能（旋转、平移、缩放等）；*~~
* [X] ~~*[15 分]基本光照明模型要求，并实现基本的光源编辑（如调整光源的位置，光强等参数）；*~~
* [ ] [15 分]能对建模后场景进行漫游如 Zoom In/Out， Pan, Orbit, Zoom To Fit等观察功能。这个具体是什么不是很清楚。
* [ ] [15 分]Awesomeness 指数：展示项目本身所独有的炫酷特点，包括但不限于
有感染力的视觉特效。

## Refactor Direction
1. 我们会有多个光源的阴影，但是也不能太多，因为三个效率上比较困难了，可以试一下，但是多个光源还是要做的。
3. 加入火箭的粒子效果，火箭爆炸以后的碎片带来的阴影效果
4. 加入一个AI，基本的AI即可，不用太强，会移动就可以了。
5. 碰撞检测的实现，可以做一个两个人打篮球的demo，和Move or Die是一样的那种。
6. 一个全局的碰撞，特殊判定就可以了，和整个大正方体的判定。
7. 粒子系统