#include "Entity.h"

Entity::Entity(ShaderProgram *program, float width, float height, GLuint spriteTexture)
	: program(program), width(width), height(height), use(false), spriteTexture(spriteTexture) {

}

void Entity::Draw() {
	Matrix modelMatrix = Matrix();
	modelMatrix.Translate(x, y, 0.0f);
	modelMatrix.Rotate(rotation);

	program->setModelMatrix(modelMatrix);

	float vertices[] = { -width / 2, -height / 2, width / 2, height / 2, -width / 2, height / 2,
		width / 2, height / 2, -width / 2, -height / 2, width / 2, -height / 2};
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);
	
	float texCoords[] = { 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, spriteTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);

}