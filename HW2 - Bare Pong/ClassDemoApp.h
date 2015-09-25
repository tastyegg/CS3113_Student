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

	GLuint spritetexture;
	float lastElapsedTime;

	Matrix modelMatrix;
	Matrix viewMatrix;
	Matrix projectionMatrix;

	Entity *paddle1;
	Entity *paddle2;
	Entity *ball;

	bool done;
};