#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
class Light{
public:
    glm::vec3 position;
    Light(glm::vec3 _position){
        position = _position;
    }
    // TODO: 加入 ambient之类的
    // TODO: 加入color
};
#endif