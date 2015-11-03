#include "Spaceship.h"

void Spaceship::DrawSpriteSheetSprite(int index, int spriteCountX, int spriteCountY) {
	float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX;
	float v = (float)(((int)index) / spriteCountX) / (float)spriteCountY;
	float spriteWidth = 1.0f / (float)spriteCountX;
	float spriteHeight = 1.0f / (float)spriteCountY;
	float texCoords[] = {
		u, v + spriteHeight,
		u + spriteWidth, v,
		u, v,
		u + spriteWidth, v,
		u, v + spriteHeight,
		u + spriteWidth, v + spriteHeight
	};
	float vertices[] = { -width / 2, -height * 3 / 4, width / 2, height * 3 / 4, -width / 2, height * 3 / 4,
		width / 2, height * 3 / 4, -width / 2, -height * 3 / 4, width / 2, -height * 3 / 4 };

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, spriteTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

Spaceship::Spaceship(ShaderProgram *program, float width, float height, GLuint spriteTexture)
	: program(program), width(width), height(height), use(false), spriteTexture(spriteTexture) {
	y = 1.9f;
	speed = 1.0f;
	direction_x = 1;
	use = false;
}

void Spaceship::Bootup() {
	if (!use) {
		if (rand() % 1000 < 500)
			direction_x *= -1;
		x = direction_x * -3.55f;
		use = true;
	}
}

void Spaceship::Update(float elapsed) {
	if (use) {
		x += direction_x * elapsed * speed;
		if (direction_x > 0 && x > 3.55f ||
			direction_x < 0 && x < -3.55f) {
			use = false;
		}
	}

	animationElapsed += elapsed * 0.3f;
	if (animationElapsed > 1.0 / framesPerSecond) {
		animationIndex++;
		animationElapsed = 0.0;
		if (animationIndex > numFrames - 1) {
			animationIndex = 0;
		}
	}
}

bool Spaceship::Collision(Entity *bullet) {
	if (bullet->use) {
		if (!((bullet->y - bullet->height / 2) > y + height / 2 ||
			(bullet->y + bullet->height / 2) < y - height / 2 ||
			(bullet->x - bullet->width / 2) > x + width / 2 ||
			(bullet->x + bullet->width / 2) < x - width / 2)) {
			use = false;
			bullet->use = false;
			return true;
		}
	}
	return false;
}

void Spaceship::Draw() {
	if (!use)
		return;

	Matrix modelMatrix = Matrix();
	modelMatrix.Translate(x, y, 0.0f);

	program->setModelMatrix(modelMatrix);

	DrawSpriteSheetSprite(animationIndex, 2, 1);
}