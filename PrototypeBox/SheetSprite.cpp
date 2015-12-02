#include "SheetSprite.h"

SheetSprite::SheetSprite(GLuint loadedSheet, int spriteCountX, int spriteCountY)
	: index(0), loadedSheet(loadedSheet), spriteCountX(spriteCountX), spriteCountY(spriteCountY)
{}

void SheetSprite::Draw(ShaderProgram *program) {
	float u = (float)(((int)index) % spriteCountX) / (float)spriteCountX;
	float v = (float)(((int)index) / spriteCountY) / (float)spriteCountY;

	float spriteWidth = 1.0f / (float)spriteCountX;
	float spriteHeight = 1.0f / (float)spriteCountY;

	GLfloat spriteUVs[] = {
		u, v + spriteHeight,
		u + spriteWidth, v,
		u, v,
		u + spriteWidth, v,
		u, v + spriteHeight,
		u + spriteWidth, v + spriteHeight
	};

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, spriteUVs);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, loadedSheet);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->texCoordAttribute);
}