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

class SpriteSheet {
public:
	SpriteSheet();
	~SpriteSheet();

	void DrawSpriteSheetSprite(int index, int spriteCountX, int spriteCountY);
};