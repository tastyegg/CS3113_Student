#include "ClassDemoApp.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_GAME_OVER };

GLuint LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	SDL_FreeSurface(surface);

	return textureID;
}

void DrawText(ShaderProgram* program, int fontTexture, std::string text, float size, float spacing) {
	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int i = 0; i < (int)text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
			((size + spacing) * i) + (-0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size,
		});
	}
	glUseProgram(program->programID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
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
	displayWindow = SDL_CreateWindow("SoundInvaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef _WINDOWS
	glewInit();
#endif
	done = false;

	program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
//	program = new ShaderProgram(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
	spritetexture = LoadTexture("p1_front.png");
	font = LoadTexture("Capword.png");
	hitSound = Mix_LoadWAV("Get_Rupee.wav");
	shootSound = Mix_LoadWAV("Trident.wav");
	death1Sound = Mix_LoadWAV("Boss_Hit.wav");
	death2Sound = Mix_LoadWAV("Boss_Death.wav");
	music = Mix_LoadMUS("Dark_World.ogg");

	projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);

	lives = 10;
	spaceship = new Spaceship(program, 0.21f, 0.21f, LoadTexture("Zelda.png"));
	invaders = new Invaders(program, 0.21f, 0.21f, LoadTexture("Links.png"), LoadTexture("Arrow.png"));
	tank = new Tank(program, 0.19f, 0.19f, LoadTexture("Ganon.png"), LoadTexture("Trident.png"));
	highscore = 0;
	state = STATE_MAIN_MENU;

	musicTicks = 16;
	Mix_PlayMusic(music, -1);
}

void ClassDemoApp::UpdateAndRender() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastElapsedTime;
	lastElapsedTime = ticks;

	if (musicTicks < 96)
		musicTicks += elapsed;
	Mix_VolumeMusic((int)musicTicks);

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
		else if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.scancode == SDL_SCANCODE_RETURN) {
				if (state != STATE_GAME_LEVEL) {
					state = STATE_GAME_LEVEL;
					invaders->Restart();
					invaders->score = 0;
				}
			}
		}
	}

	switch (state) {
	case STATE_MAIN_MENU:
		MainMenuRender();
		break;
	case STATE_GAME_LEVEL:
		GameLevelUpdate(elapsed);
		GameLevelRender();
		break;
	case STATE_GAME_OVER:
		GameOverRender();
		break;
	}
}

void ClassDemoApp::GameLevelUpdate(float elapsed) {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]) {
		tank->x -= elapsed * tank->speed;
	}
	else if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) {
		tank->x += elapsed * tank->speed;
	}
	if (keys[SDL_SCANCODE_SPACE]) {
		if (!tank->bullet->use) {
			Mix_PlayChannel(-1, shootSound, 0);
		}
		tank->CreateBullet();
	}

	tank->Update(elapsed);
	invaders->Update(elapsed);
	spaceship->Update(elapsed);

	if (tank->bullet->use) {
		glClear(GL_COLOR_BUFFER_BIT);
		invaders->Collision(tank->bullet);
		if (!tank->bullet->use) {
			Mix_PlayChannel(-1, hitSound, 0);
			if (rand() % 10 < 3)
				spaceship->Bootup();
		} else if (spaceship->use) {
			spaceship->Collision(tank->bullet);
			if (!tank->bullet->use) {
				invaders->score += (invaders->Count()) * 100;
				Mix_PlayChannel(-1, hitSound, 0);
			}
		}
	}
	if (invaders->TankCollision(tank)) {
		Mix_PlayChannel(-1, death1Sound, 0);
		lives--;
	}
	if (lives == 0 || invaders->y < -10) {
		Mix_PlayChannel(-1, death2Sound, 0);
		state = STATE_GAME_OVER;
		if (highscore < invaders->score)
			highscore = invaders->score;
		invaders->score = 0;
		tank->x = 0;
		tank->bullet->use = false;
		spaceship->use = false;
		lives = 10;
		invaders->ascension = 1;
	}
}

void ClassDemoApp::GameLevelRender() {
	glClear(GL_COLOR_BUFFER_BIT);

	program->setModelMatrix(modelMatrix);
	program->setViewMatrix(viewMatrix);
	program->setProjectionMatrix(projectionMatrix);

	glUseProgram(program->programID);

	modelMatrix.identity();
	modelMatrix.Translate(-3.35f, 1.778f, 0);
	program->setModelMatrix(modelMatrix);
	DrawText(program, font, "$" + std::to_string(invaders->score), 0.2f, -0.05f);

	invaders->Draw();
	tank->Draw();
	spaceship->Draw();

	modelMatrix.identity();
	modelMatrix.Translate(-3.45f, -1.778f, 0);
	program->setModelMatrix(modelMatrix);
	std::string lifeString = "";
	for (int i = 0; i < lives; i++)
		lifeString += '}';
	for (int i = lives; i < 10; i++)
		lifeString += '~';
	DrawText(program, font, lifeString, 0.14f, 0.01f);

	SDL_GL_SwapWindow(displayWindow);
}

void ClassDemoApp::MainMenuRender() {
	glClear(GL_COLOR_BUFFER_BIT);

	program->setModelMatrix(modelMatrix);
	program->setViewMatrix(viewMatrix);
	program->setProjectionMatrix(projectionMatrix);

	glUseProgram(program->programID);

	modelMatrix.identity();
	modelMatrix.Translate(-2.42f, 1.13f, 0);
	program->setModelMatrix(modelMatrix);
	DrawText(program, font, "Invaders w/ Sound", 0.6f, -0.29f);

	modelMatrix.identity();
	modelMatrix.Translate(-1.55f, 0, 0);
	program->setModelMatrix(modelMatrix);
	DrawText(program, font, "Press RETURN to start", 0.2f, -0.05f);

	SDL_GL_SwapWindow(displayWindow);
}

void ClassDemoApp::GameOverRender() {
	glClear(GL_COLOR_BUFFER_BIT);

	program->setModelMatrix(modelMatrix);
	program->setViewMatrix(viewMatrix);
	program->setProjectionMatrix(projectionMatrix);

	glUseProgram(program->programID);

	modelMatrix.identity();
	modelMatrix.Translate(-1.32f, 1.08f, 0);
	program->setModelMatrix(modelMatrix);
	DrawText(program, font, "Game Over", 0.4f, -0.05f);

	modelMatrix.identity();
	modelMatrix.Translate(-0.82f, 0.28f, 0);
	program->setModelMatrix(modelMatrix);
	DrawText(program, font, "TOP $" + std::to_string(highscore), 0.2f, -0.05f);

	modelMatrix.identity();
	modelMatrix.Translate(-1.52f, -0.4f, 0);
	program->setModelMatrix(modelMatrix);
	DrawText(program, font, "Press RETURN to restart", 0.2f, -0.05f);

	SDL_GL_SwapWindow(displayWindow);
}

bool ClassDemoApp::isDone() {
	return done;
}