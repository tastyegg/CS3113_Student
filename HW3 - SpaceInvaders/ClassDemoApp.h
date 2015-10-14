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
#include "Invaders.h"
#include "Tank.h"
#include "Spaceship.h"
#include <vector>

class ClassDemoApp {
public:
	ClassDemoApp();
	~ClassDemoApp();

	void Setup();

	void ProcessInput();
	void MainMenuRender();
	void GameLevelRender();
	void GameOverRender();
	void GameLevelUpdate(float elapsed);

	void UpdateAndRender();

	bool isDone();

private:
	ShaderProgram *program;
	SDL_Event event;

	SDL_Joystick *joystick;
	SDL_Window *displayWindow;

	GLuint font;
	GLuint spritetexture;
	float lastElapsedTime;

	Matrix modelMatrix;
	Matrix viewMatrix;
	Matrix projectionMatrix;

	int state;
	int highscore;

	Spaceship *spaceship;
	Invaders *invaders;
	Tank *tank;

	bool done;
};