#pragma once

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#include "Entity.h"

class Tank {
public:
	Tank(ShaderProgram *program, float width, float height, GLuint spriteTexture, GLuint whiteTexture);
	~Tank();

	void DrawSpriteSheetSprite(int index, int spriteCountX, int spriteCountY);
	void CreateBullet();
	void Update(float elapsed);
	void Draw();

	int pierce;
	Entity *bullet;

	ShaderProgram *program;
	GLuint spriteTexture;
	float x, y, rotation;
	int textureID;
	float width, height;
	float speed;
	float direction_x, direction_y;
};