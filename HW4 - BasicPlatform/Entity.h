#pragma once

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#include "SheetSprite.h"
#include <math.h>

enum EntityType {ENTITY_PLAYER, ENTITY_ENEMY, ENTITY_PLATFORM};

class Entity {
public:
	Entity(SheetSprite sprite);
	
	void Update(float elapsed);
	void Draw(ShaderProgram *program);
	bool CollidesWith(Entity *entity);

	SheetSprite sprite;

	float x, y;
	float width, height;
	
	float velocity_x, velocity_y;
	float acceleration_x, acceleration_y;

	EntityType entityType;
	bool isStatic;

	bool collidedTop, collidedBottom, collidedLeft, collidedRight;
};