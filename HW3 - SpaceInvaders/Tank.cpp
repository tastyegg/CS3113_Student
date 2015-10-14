#include "Tank.h"

void Tank::DrawSpriteSheetSprite(int index, int spriteCountX, int spriteCountY) {
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
	float vertices[] = { -width * 3 / 2, -height * 3 / 2, width * 3 / 2, height * 3 / 2, -width * 3 / 2, height * 3 / 2,
		width * 3 / 2, height * 3 / 2, -width * 3 / 2, -height * 3 / 2, width * 3 / 2, -height * 3 / 2 };

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, spriteTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

Tank::Tank(ShaderProgram *program, float width, float height, GLuint spriteTexture, GLuint whiteTexture)
	: program(program), width(width), height(height), spriteTexture(spriteTexture) {
	bullet = new Entity(program, width, height * (44.0f / 13.0f), whiteTexture);

	y = -1.6f;
	speed = 2.6f;
	bullet->speed = 2.6f;
	bullet->use = false;
}

Tank::~Tank() {
	//delete bullet;
}

void Tank::CreateBullet() {
	if (!bullet->use) {
		bullet->use = true;
		bullet->x = x;
		bullet->y = y;
	}
}

void Tank::Update(float elapsed) {
	if (bullet->use) {
		bullet->y += elapsed * bullet->speed;
		if (bullet->y > 2.0f) {
			bullet->use = false;
		}
	}
	if (x < -3.55f + width / 2)
		x = -3.55f + width / 2;
	if (x > 3.55f - width / 2)
		x = 3.55f - width / 2;
}

void Tank::Draw() {
	Matrix modelMatrix = Matrix();
	modelMatrix.Translate(x, y, 0.0f);

	program->setModelMatrix(modelMatrix);


	if (bullet->use) {
		DrawSpriteSheetSprite(1, 1, 2);
		bullet->Draw();
	} else
		DrawSpriteSheetSprite(0, 1, 2);
}