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

class Spaceship {
public:
	Spaceship(ShaderProgram *program, float width, float height, GLuint spriteTexture);
	~Spaceship();

	void DrawSpriteSheetSprite(int index, int spriteCountX, int spriteCountY);
	void Bootup();
	void Update(float elapsed);
	void Draw();
	bool Spaceship::Collision(Entity *bullet);

	ShaderProgram *program;
	GLuint spriteTexture;
	float x, y, rotation;
	int textureID;
	float width, height;
	float speed;
	float direction_x, direction_y;
	bool use;

	int runAnimation[2];
	int numFrames = 2;
	float animationElapsed = 0.0f;
	float framesPerSecond = 30.0f;
	int animationIndex = 0;
};