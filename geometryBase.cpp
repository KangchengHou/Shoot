#include "geometryBase.h"
#include <iostream>


GeometryBase::GeometryBase(std::vector<glm::vec3> vertices, std::vector<glm::vec3> faces)
    : vertices(vertices), faces(faces)
{
    std::cout << "succeed in constructing geometryBase" << std::endl;
}
GeometryBase::~GeometryBase()
{
    std::cout << "succeed in deconstructing geometryBase" << std::endl;
}

// void GeometryBase::applyMatrix(double matrix[4][4]) 
// {
//     for (vector<glm::vec3>::const_iterator iter = vertices.cbegin(); iter != vertices.cend(); iter++)
//     {
//         // 做变换
//     }
//     for (vector<glm::vec3>::const_iterator iter = faces.cbegin(); iter != faces.cend(); iter++)
//     {
//         // 法向量做变化
//         // 每个顶点做变化
//     }
//     // 重新计算当前geometry对象的立方体界线
//     // 重新计算当前geometry对象的球体界线
// }
glm::vec3 GeometryBase::center() {
    // 先计算立方体界线
    
}