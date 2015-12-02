#pragma once
#define WINDOW_HEIGHT 360.0f
#define WINDOW_WIDTH 640.0f
#define PROJECTION_WIDTH 3.55f
#define PROJECTION_HEIGHT 2.0f
#define LEVEL_HEIGHT 20
#define LEVEL_WIDTH 100
#define TILE_SIZE 0.3f
#define BOX_SIZE TILE_SIZE - 0.001f

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
#include "global.h"
#include "PlayerController.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

enum GameState {
	PlayingLevel = 0,
	GameOver = 1
};

class ClassDemoApp {
public:
	ClassDemoApp();
	~ClassDemoApp();

	void ClassDemoApp::LoadLevel(std::string mapName);
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
	GLuint infoBox;
	float lastElapsedTime;

	Matrix modelMatrix;
	Matrix viewMatrix;
	Matrix projectionMatrix;

	unsigned char levelData[LEVEL_HEIGHT][LEVEL_WIDTH];
	std::vector<Entity*> entities;
	bool includePlayer2;
	Entity *player;
	Entity *player2;
	Entity *players[2];
	PlayerController *controls[2];
	float movingColor;
	float infoBoxTimer;
	GameState state;

	bool done;
};