#ifndef PARTICLE_H
#define PARTICLE_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "shader.h"
#include "game_body_base.h"
#include "texture.h"

struct Particle {
	glm::vec3 position, velocity;
	glm::vec4 color;
	GLfloat life;

	Particle(): position(0.0f), velocity(0.0f), color(1.0f), life(0.0f) {}
};

class ParticleGenerator {
public:
	ParticleGenerator(Shader shader, Texture2D texture, GLuint amount);
	~ParticleGenerator();
	void init();
	void draw();
	void update(GLfloat dt, GameBodyBase &object, GLuint newParticles, glm::vec3 offset);
	GLuint firstUnusedParticle();
	void respawnParticle(Particle &particle, GameBodyBase &object, glm::vec3 offset);

	Shader shader;
	Texture2D texture;
	GLuint amount;
	GLuint VAO;
	std::vector<Particle> particles;
	GLuint amout;
};

#endif