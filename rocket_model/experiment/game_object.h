#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>


class GameObject {
public:
	GameObject(): position(0, 0, 0), size(1, 1, 1), velocity(0.0f), color(1.0f) { };
	GameObject(glm::vec3 pos, glm::vec3 size, glm::vec4 color, glm::vec3 velocity): position(pos), size(size), velocity(velocity), color(color) { }
	~GameObject();

	glm::vec3 position;
	glm::vec3 size;
	glm::vec3 velocity;
	glm::vec4 color;
};

#endif