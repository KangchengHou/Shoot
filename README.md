# Shoot
Game implemented with OpenGL
## TODO
* [ ] 骨骼动画
* [ ] API设计,也就是main.cpp那边要重写
* [ ] 地形设计
* [ ] 物理引擎,重力系统
* [ ] 载入obj 模型
* [ ] 现在有一个人在二维平面上跑了，现在要有他能把一个物体给扔出去，先做这个，要用比较合适的写的方法，以及物体之间碰撞检测的比较好的方法，还有物体的弹跳。
* [ ] 修改了一下代码 Shader& 原来是引用，我改成直接赋值了，如果性能有问题可以检查一下这个有没有被频繁的调用，现在感觉只有初始化的时候才用了一下。

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

## overall design 
1. program

## TODO
1. 物理引擎
2. 两个renderer怎么改不来
3. 