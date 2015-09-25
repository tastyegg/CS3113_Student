#include "ClassDemoApp.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

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

ClassDemoApp::ClassDemoApp() {
	Setup();
}

ClassDemoApp::~ClassDemoApp() {
	delete program;

	SDL_Quit();
}

void ClassDemoApp::Setup() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	done = false;

//	program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	program = new ShaderProgram(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
	spritetexture = LoadTexture("p1_front.png");

	projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);

	paddle1 = new Entity(program, modelMatrix, 0.2f, 0.8f);
	paddle2 = new Entity(program, modelMatrix, 0.2f, 0.8f);
	ball = new Entity(program, modelMatrix, 0.2f, 0.2f);

	paddle1->x = -3.25f;
	paddle2->x = 3.25f;
	paddle1->speed = 4.4f;
	paddle2->speed = 4.4f;
	ball->speed = 3.0f;
	ball->direction_x = 1.0f;
	ball->direction_y = 1.0f;
}

void ClassDemoApp::UpdateAndRender() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
	}

	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastElapsedTime;
	lastElapsedTime = ticks;

	Update(elapsed);
	Render();
}

void ClassDemoApp::Update(float elapsed) {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_W]) {
		paddle1->y += elapsed * paddle2->speed;
	}
	if (keys[SDL_SCANCODE_S]) {
		paddle1->y -= elapsed * paddle2->speed;
	}
	if (keys[SDL_SCANCODE_UP]) {
		paddle2->y += elapsed * paddle2->speed;
	}
	if (keys[SDL_SCANCODE_DOWN]) {
		paddle2->y -= elapsed * paddle2->speed;
	}

	ball->x += elapsed * ball->direction_x * ball->speed;
	ball->y += elapsed * ball->direction_y * ball->speed;

	// Paddle -> Screen Collision
	if (paddle1->y > 2.0f - paddle1->height / 2) {
		paddle1->y = 2.0f - paddle1->height / 2;
	}
	else if (paddle1->y < -2.0f + paddle1->height / 2) {
		paddle1->y = -2.0f + paddle1->height / 2;
	}
	if (paddle2->y > 2.0f - paddle2->height / 2) {
		paddle2->y = 2.0f - paddle2->height / 2;
	}
	else if (paddle2->y < -2.0f + paddle2->height / 2) {
		paddle2->y = -2.0f + paddle2->height / 2;
	}

	// Ball -> Screen Collision
	if (ball->x > 3.55f - ball->width / 2) {
		ball->direction_x = -1;
		ball->x = 0;
	} else if (ball->x < -3.55f + ball->width / 2) {
		ball->direction_x = 1;
		ball->x = 0;
	}

	if (ball->y > 2.0f - ball->height / 2) {
		ball->direction_y = -1;
		//ball->y = 2.0f - ball->height / 2;
	} else if (ball->y < -2.0f + ball->height / 2) {
		ball->direction_y = 1;
		//ball->y = -2.0f + ball->height / 2;
	}

	// Ball -> Paddle Collision
	if (!(paddle1->y - paddle1->height / 2 > ball->y + ball->height / 2 ||
		paddle1->y + paddle1->height / 2 < ball->y - ball->height / 2 ||
		paddle1->x - paddle1->width / 2 > ball->x + ball->width / 2 ||
		paddle1->x + paddle1->width / 2 < ball->x - ball->width / 2)) {
		ball->direction_x = 1.0f;
	}
	if (!(paddle2->y - paddle2->height / 2 > ball->y + ball->height / 2 ||
		paddle2->y + paddle2->height / 2 < ball->y - ball->height / 2 ||
		paddle2->x - paddle2->width / 2 > ball->x + ball->width / 2 ||
		paddle2->x + paddle2->width / 2 < ball->x - ball->width / 2)) {
		ball->direction_x = -1.0f;
	}
}

void ClassDemoApp::Render() {
	glClear(GL_COLOR_BUFFER_BIT);

	program->setModelMatrix(modelMatrix);
	program->setViewMatrix(viewMatrix);
	program->setProjectionMatrix(projectionMatrix);

	glUseProgram(program->programID);

	paddle1->Draw();
	paddle2->Draw();
	ball->Draw();
	
	SDL_GL_SwapWindow(displayWindow);
}

bool ClassDemoApp::isDone() {
	return done;
}