#pragma once

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#include <math.h>
#include "Entity.h"
#include "Tank.h"

class Invaders {
public:
	Invaders(ShaderProgram *program, float width, float height, GLuint spriteTexture, GLuint whiteTexture);
	~Invaders();

	void Invaders::Restart();
	void Update(float elapsed);
	void Collision(Entity *bullet);
	bool TankCollision(Tank *tank);
	void Draw();
	void CreateBullet();
	int Count();
	void Invaders::DrawSpriteSheetSprite(int index, int spriteCountX, int spriteCountY);

	bool pawns[11][5];

	ShaderProgram *program;
	float x, y, rotation;
	int textureID;
	float width, height, spacing;
	float speed;
	float direction_x, direction_y;
	Entity *bullets[4];

	int score;

	GLuint spriteTexture;
	int runAnimation[7];
	int numFrames = 7;
	float animationElapsed = 0.0f;
	float framesPerSecond = 30.0f;
	int animationIndex = 0;
};