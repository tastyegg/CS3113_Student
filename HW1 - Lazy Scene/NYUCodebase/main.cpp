#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#include <math.h>

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

GLuint LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);

	return textureID;
}

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif

	/// SETUP

	glViewport(0, 0, 640, 360);

	ShaderProgram program = ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	GLuint playerSprite = LoadTexture("p1_front.png");
	GLuint cloudSprite = LoadTexture("cloud1.png");
	GLuint rockSprite = LoadTexture("rock.png");

	Matrix modelMatrix;
	Matrix modelMatrix2;
	Matrix modelMatrix3;
	Matrix viewMatrix;
	Matrix projectionMatrix;

	projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);

	float lastFrameticks = 0.0f;
	float playerSpritePosition = 0.0f;
	float cloudSpritePosition = 0.0f;
	float rockSpritePositionX = 0.0f;
	float rockSpritePositionY = 0.0f;
	/// GAME LOOP

	SDL_Event event;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}

		// Logic

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameticks;
		lastFrameticks = ticks;

		playerSpritePosition += elapsed * 4;
		cloudSpritePosition += elapsed;
		rockSpritePositionY += elapsed * 3;
		if (rockSpritePositionY > 2.8f) {
			rockSpritePositionX = ((int)(sin(cloudSpritePosition) * 8)) / 3.0f;
			rockSpritePositionY -= 2.8f;
		}
		modelMatrix.identity();
		modelMatrix.Translate(((int)(sin(playerSpritePosition) * 8)) / 3.0f, -1.4f, 0.0f);
		//modelMatrix.Scale(1.0f, 1.0f, 1.0f);
		//modelMatrix.Rotate(0.0f * (3.1415926f / 180.0f));
		modelMatrix2.identity();
		modelMatrix2.Translate((sin(cloudSpritePosition) * 8) / 3.0f, 1.4f, 0.0f);
		modelMatrix3.identity();
		modelMatrix3.Translate(rockSpritePositionX, 1.4f - rockSpritePositionY, 0.0f);

		program.setModelMatrix(modelMatrix);
		program.setViewMatrix(viewMatrix);
		program.setProjectionMatrix(projectionMatrix);

		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(program.programID);

		//Drawing Phase

		//Create a textured sprite (player)
		//Create two triangles (1 unit squared)
		float vertices[] = { -0.4f, -0.5f, 0.4f, 0.5f, -0.4f, 0.5f, 0.4f, 0.5f, -0.4f, -0.5f, 0.4f, -0.5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);

		//Create two triangles for texture
		float texCoords[] = { 0.0, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0 };
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glBindTexture(GL_TEXTURE_2D, playerSprite);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		program.setModelMatrix(modelMatrix3);
		//Create a textured sprite (rock)
		//Create two triangles
		float vertices2[] = { -0.5f, -0.3f, 0.5f, 0.3f, -0.5f, 0.3f, 0.5f, 0.3f, -0.5f, -0.3f, 0.5f, -0.3f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
		glEnableVertexAttribArray(program.positionAttribute);

		//Create two triangles for texture
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glBindTexture(GL_TEXTURE_2D, rockSprite);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		program.setModelMatrix(modelMatrix2);
		//Create a textured sprite (cloud)
		//Create two triangles
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
		glEnableVertexAttribArray(program.positionAttribute);

		//Create two triangles for texture
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glBindTexture(GL_TEXTURE_2D, cloudSprite);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
		
		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
