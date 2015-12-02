#pragma once

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "ShaderProgram.h"

class SheetSprite {
public:
	SheetSprite(GLuint loadedSheet, int spriteCountX, int spriteCountY);

	void Draw(ShaderProgram *program);
	
	int index;
	GLuint loadedSheet;
	int spriteCountX, spriteCountY;
};