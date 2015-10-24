#include "Entity.h"

float lerp(float v0, float v1, float t) {
	return (1.0f - t) * v0 + t * v1;
}

Entity::Entity(SheetSprite sprite)
	: sprite(sprite), x(0), y(0), width(0), height(0),
	velocity_x(0), velocity_y(0), acceleration_x(0), acceleration_y(0),
	collidedTop(false), collidedBottom(false), collidedLeft(false), collidedRight(false) {
}

void Entity::Update(float elapsed) {
	collidedTop = false;
	collidedBottom = false;
	collidedLeft = false;
	collidedRight = false;

	velocity_x = lerp(velocity_x, 0.0f, elapsed * 2.8f);	//friction_x
	velocity_x += acceleration_x * elapsed;

	velocity_y = lerp(velocity_y, 0.0f, elapsed * 0.4f);	//friction_y
	velocity_y += acceleration_y * elapsed;
	velocity_y += -3.0f * elapsed;
}

bool CollidesWith(Entity *entity) {
	return true;
}

void Entity::Draw(ShaderProgram *program) {
	Matrix modelMatrix = Matrix();
	modelMatrix.Translate(x, y, 0);

	program->setModelMatrix(modelMatrix);

	float vertices[] = { -width / 2, -height / 2, width / 2, height / 2, -width / 2, height / 2,
		width / 2, height / 2, -width / 2, -height / 2, width / 2, -height / 2};
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);
	
	sprite.Draw(program);
	
	glDisableVertexAttribArray(program->positionAttribute);
}