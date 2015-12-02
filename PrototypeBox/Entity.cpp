#include "Entity.h"

Entity::Entity(SheetSprite sprite)
	: sprite(sprite), x(0.0f), y(0.0f), width(0.0f), height(0.0f),
	velocity_x(0.0f), velocity_y(0.0f), acceleration_x(0.0f), acceleration_y(0.0f),
	spawnX(0), spawnY(0), lastDirectionX(0),
	collidedTop(false), collidedBottom(false), collidedLeft(false), collidedRight(false) {
	for (int i = 0; i < 6; i++) {
		colors[i * 4] = 1.0f;
		colors[i * 4 + 1] = 1.0f;
		colors[i * 4 + 2] = 1.0f;
		colors[i * 4 + 3] = 1.0f;
	}
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
	velocity_y += -7.0f * elapsed;	//gravity
}

bool Entity::CollidesWith(Entity *entity) {
	if (y - height / 2 > entity->y + entity->height / 2 ||
		y + height / 2 < entity->y - entity->height / 2 ||
		x - width / 2 > entity->x + entity->width / 2 ||
		x + width / 2 < entity->x - entity->width / 2) {
		return false;
	}
	return true;
}

void Entity::Draw(ShaderProgram *program) {
	Matrix modelMatrix = Matrix();
	modelMatrix.Translate(x, y, 0.0f);

	program->setModelMatrix(modelMatrix);

	float vertices[] = { -width / 2, -height / 2, width / 2, height / 2, -width / 2, height / 2,
		width / 2, height / 2, -width / 2, -height / 2, width / 2, -height / 2};
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);

	GLuint colorAttribute = glGetAttribLocation(program->programID, "color");
	glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, false, 0, colors);
	glEnableVertexAttribArray(colorAttribute);

	sprite.Draw(program);
	
	glDisableVertexAttribArray(program->positionAttribute);
}