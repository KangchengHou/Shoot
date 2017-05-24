#ifndef GEOMETRYBASE_H
#define GEOMETRYBASE_H

#include <iostream>
#include <vector>
#include <glm/glm.hpp>


class GeometryBase {
public:
    // TODO
    GeometryBase(std::vector<glm::vec3> vertices, std::vector<glm::vec3> faces);
    virtual ~GeometryBase();

    // 不知道为什么要有id
    int id;
    int uuid;

    // 更新系列
    bool verticesNeedUpdate;
    bool elementsNeedUpdate;


    // variables
    // 更新需要将verticesNeedUpdate设为true
    std::vector<glm::vec3> vertices;
    // 如[[0,1,2]]表示vertices[0] vertices[1] vertices[2]构成一个三角面
    // 更新需要将elementsNeedUpdate设为true
    std::vector<glm::vec3> faces;
    // normals和faces一一对映
    std::vector<glm::vec3> normals;
    

    // 还没有matrix的类
    // 几何体变换 移动等
    void applyMatrix(double matrix[4][4]);
    // 返回中心坐标
    glm::vec3 center();
    // 计算每个面的法向量
    void computeFaceNorms();

};

// http://blog.csdn.net/omni360/article/details/41485339
#endif