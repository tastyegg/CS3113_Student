#pragma once
#define LEVEL_HEIGHT 21
#define LEVEL_WIDTH 86
#define TILE_SIZE 0.3f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#include "Entity.h"
#include <vector>
#include "SheetSprite.h"
#include <time.h>

class ClassDemoApp {
public:
	ClassDemoApp();
	~ClassDemoApp();

	void Setup();

	void ProcessInput();
	void Render();
	void Update(float elapsed);

	void UpdateAndRender();

	bool isDone();

private:
	ShaderProgram *program;
	SDL_Event event;

	SDL_Joystick *joystick;
	SDL_Window *displayWindow;

	GLuint tileTexture;
	float lastElapsedTime;
	float timeLeftOver;

	Matrix modelMatrix;
	Matrix viewMatrix;
	Matrix projectionMatrix;

	unsigned char levelData[LEVEL_HEIGHT][LEVEL_WIDTH];
	std::vector<Entity*> entities;
	Entity *player;

	int baseTouchX;

	bool done;
};