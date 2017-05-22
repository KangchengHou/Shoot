

#ifndef GAMEBODYBASE_H
#define GAMEBODYBASE_H

#include "geometryBase.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class GameBodyBase {
public:
    
    GameBodyBase(GeometryBase* geometry);
	virtual ~GameBodyBase();

protected:
    // variables

    glm::vec3 position;
    glm::vec3 speed;
    glm::vec3 acceleration;
    double mass;
    GeometryBase* geometry;
    

    void render();

    // 获取 改变位置
    void setPos(GLfloat dt);
    glm::vec3 getPos();

    // 获取 改变速度
    void setSpeed(GLfloat dt);
    glm::vec3 getSpeed();

    // 获取 改变加速度
    void setAcceleration(glm::vec3 a);
    glm::vec3 getAcceleration();


    
    // 克隆对象
    // object: 接收克隆的对象  
    // recursive: 是否对子对象一一进行克隆
    void clone(GameBodyBase& object, bool recursive);
    // raycast 用于碰撞检测  判断一个射线与该对象的交点
    // updateMorphTargets用于对象变形
    // http://blog.csdn.net/omni360/article/details/42896863
    void raycast();
    void updateMorphTargets();
    
};

#endif