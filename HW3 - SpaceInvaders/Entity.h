#pragma once

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"

class Entity {
public:
	Entity(ShaderProgram *program, float width, float height, GLuint spriteTexture);
	~Entity();

	void Draw();

	ShaderProgram *program;
	GLuint spriteTexture;
	float x, y, rotation;
	int textureID;
	float width, height;
	float speed;
	float direction_x, direction_y;
	bool use;
};