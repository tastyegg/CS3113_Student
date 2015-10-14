
#include "Invaders.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

void Invaders::DrawSpriteSheetSprite(int index, int spriteCountX, int spriteCountY) {
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

Invaders::Invaders(ShaderProgram *program, float width, float height, GLuint spriteTexture, GLuint whiteTexture)
	: program(program), width(width), height(height), spriteTexture(spriteTexture) {
	for (int i = 0; i < 4; i++) {
		bullets[i] = new Entity(program, width * 5 / 16, height * 15 / 16, whiteTexture);
		bullets[i]->speed = 2.1f;
		bullets[i]->use = false;
	}
	spacing = width / 2;
	direction_x = 1;
	Restart();
}

void Invaders::Restart() {
	for (int i = 0; i < 4; i++)
		bullets[i]->use = false;
	speed = 0.2f;
	x = 0;
	y = 0;
	for (int i = 0; i < 11; i++) {
		for (int j = 0; j < 5; j++) {
			pawns[i][j] = true;
		}
	}
}

void Invaders::Update(float elapsed) {
	x += elapsed * direction_x * speed;
	int leftmostInvader = 0;
	bool leftmost = false;
	//Check left most invader && right most invader when moving to a border
	if (direction_x < 0) {
		for (int i = 0; i < 11; i++) {
			for (int j = 0; j < 5; j++){
				if (pawns[i][j]) {
					leftmost = true;
					break;
				}
			}
			if (leftmost) {
				leftmostInvader = i;
				break;
			}
		}
		if (x - fmod(x, width / 2) - width - (5 - leftmostInvader)*(width + spacing) < -3.55f) {
			x = -3.55f + width + (5 - leftmostInvader)*(width + spacing);
			direction_x = 1;
			y--;
			if (y <= -6) {
				speed = 3;
			}
			else if (y <= -4) {
				speed = 2;
			}
		}
	} else if (direction_x > 0) {
		for (int i = 10; i >= 0; i--) {
			for (int j = 0; j < 5; j++){
				if (pawns[i][j]) {
					leftmost = true;
					break;
				}
			}
			if (leftmost) {
				leftmostInvader = 10 - i;
				break;
			}
		}
		if (x - fmod(x, width / 2) + (5 - leftmostInvader)*(width + spacing) > 3.55f) {
			x = 3.55f - (5 - leftmostInvader)*(width + spacing);
			direction_x = -1;
			y--;
			if (y <= -6) {
				speed = 3;
			}
			else if (y <= -4) {
				speed = 2;
			}
		}
	}

	if (rand() % 1000 < (55.0f / (Count() + 1)) * 2 )
		CreateBullet();
	for (int b = 0; b < 4; b++) {
		if (bullets[b]->use) {
			bullets[b]->y -= elapsed * bullets[b]->speed;
			if (bullets[b]->y < -2.0f) {
				bullets[b]->use = false;
			}
		}
	}

	animationElapsed += elapsed;
	if (animationElapsed > 1.0 / framesPerSecond) {
		animationIndex++;
		animationElapsed = 0.0;
		if (animationIndex > numFrames - 1) {
			animationIndex = 0;
		}
	}
}

void Invaders::Collision(Entity *bullet) {
	for (int j = 0; j < 5; j++) {
		for (int i = 0; i < 11; i++) {
			if (pawns[i][j]) {
				if (!(bullet->y > (height + spacing) * (5 + y - j) + height ||
					bullet->y < (height + spacing) * (5 + y - j) ||
					bullet->x > x - fmod(x, width / (8 * speed)) - (5 - i)*(width + spacing) ||
					bullet->x < x - fmod(x, width / (8 * speed)) - (5 - i)*(width + spacing) - width)) {
					pawns[i][j] = false;
					if (j == 0)
						score += 300;
					else if (j < 3)
						score += 200;
					else if (j < 5)
						score += 100;
					i = 11; j = 5;
					bullet->use = false;
				}
			}
		}
	}
	if (Count() == 0)
		Restart();
}

bool Invaders::TankCollision(Tank *tank) {
	for (int b = 0; b < 4; b++) {
		if (bullets[b]->use) {
			if (!(bullets[b]->y > tank->y + tank->height / 2 ||
				bullets[b]->y < tank->y - tank->height / 2 ||
				bullets[b]->x > tank->x + tank->width / 2 ||
				bullets[b]->x < tank->x - tank->width / 2)) {
				return true;
			}
		}
	}
	return false;
}

void Invaders::CreateBullet() {
	for (int b = 0; b < 4; b++) {
		if (!bullets[b]->use) {
			int count = Count();
			int randPawn = rand() % count;
			for (int i = 0; i < 11; i++) {
				for (int j = 0; j < 5; j++) {
					if (pawns[i][j]) {
						randPawn--;
						if (randPawn < 0) {
							bullets[b]->use = true;
							bullets[b]->x = x - fmod(x, width / (8 * speed)) - (5 - i)*(width + spacing) - width / 2;
							bullets[b]->y = (height + spacing) * (5 + y - j) + height / 2;
							i = 11; j = 5;
						}
					}
				}
			}
			b = 4;
		}
	}
}

int Invaders::Count() {
	int count = 0;
	for (int i = 0; i < 11; i++)
		for (int j = 0; j < 5; j++)
			if (pawns[i][j])
				count++;
	return count;
}

void Invaders::Draw() {
	Matrix modelMatrix = Matrix();
	modelMatrix.Translate(x - fmod(x, width/ (8 * speed)) - width / 2 - 5 * (width + spacing),
		(height + spacing) * (5 + y) + height / 2, 0.0f);

	/*
	float vertices[] = { -width / 2, -height / 2, width / 2, height / 2, -width / 2, height / 2,
		width / 2, height / 2, -width / 2, -height / 2, width / 2, -height / 2 };
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);

	float texCoords[] = { 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, spriteTexture);
	*/

	for (int j = 0; j < 5; j++) {
		for (int i = 0; i < 11; i++) {
			if (pawns[i][j]) {
				program->setModelMatrix(modelMatrix);
				if (j == 0)
					DrawSpriteSheetSprite(animationIndex + 14, 7, 3);
				else if (j < 3)
					DrawSpriteSheetSprite(animationIndex + 7, 7, 3);
				else if (j < 5)
					DrawSpriteSheetSprite(animationIndex, 7, 3);
				//glDrawArrays(GL_TRIANGLES, 0, 6);
			}
			modelMatrix.Translate(width + spacing, 0, 0);
		}
		modelMatrix.Translate((width + spacing) * -11, -height - spacing, 0);
	}

	for (int b = 0; b < 4; b++) {
		if (bullets[b]->use) {
			bullets[b]->Draw();
		}
	}

	/*
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
	*/
}