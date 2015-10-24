#include "ClassDemoApp.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEP 6

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

void worldToTileCoordinates(float worldX, float worldY, int &gridX, int &gridY) {
	gridX = (int)(worldX / TILE_SIZE);
	gridY = (int)(worldY / TILE_SIZE);
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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef _WINDOWS
	glewInit();
#endif
	done = false;

	srand((unsigned int)time(NULL));

	program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
//	program = new ShaderProgram(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
	tileTexture = LoadTexture("sheet_4.png");

	projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);

	//SheetSprite sheet = SheetSprite(tileTexture, 14, 8);
	for (int i = 0; i < LEVEL_HEIGHT; i++) {
		for (int j = 0; j < LEVEL_WIDTH; j++) {
			if (i == 0 || j == 0 || j == LEVEL_WIDTH - 1)
				levelData[i][j] = 13;
			else
				levelData[i][j] = 254;
		}
	}

	for (int k = 2; k < LEVEL_WIDTH - 5; ) {
		for (int r = rand() % 3; r < 3; r++) {
			int j = rand() % (LEVEL_HEIGHT - 4) + 3;
			for (int i = rand() % 4; i < 6; i++) {
				levelData[j][k + i] = 13;
				if (i == 4 && (k + i) > LEVEL_WIDTH - 11) {
					levelData[j - 1][k + i] = 64;
					r = 3;
				}
			}
		}
		k += 7 + rand() % 3;
	}

	SheetSprite playerSprite = SheetSprite(LoadTexture("p2_spritesheet.png"), 1, 1);
	player = new Entity(playerSprite);
	entities.push_back(player);
	player->width = 0.225f;
	player->height = 0.3f;
	player->x = TILE_SIZE * 6 + player->width / 2;
	player->y = -TILE_SIZE * 2;
	player->isStatic = false;
	baseTouchX = 6;
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

	float fixedElapsed = elapsed;
	if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEP)
		fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEP;
	while (fixedElapsed > FIXED_TIMESTEP) {
		fixedElapsed -= FIXED_TIMESTEP;
		Update(FIXED_TIMESTEP);
	}
	Update(fixedElapsed);

	Render();
}

void ClassDemoApp::Update(float elapsed) {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	//Player commands
	if (keys[SDL_SCANCODE_UP] && player->collidedBottom) {
		player->velocity_y = 5.8f;
		player->collidedBottom = false;
	}
	if (keys[SDL_SCANCODE_LEFT]) {
		player->acceleration_x = -5.8f;
	}
	else if (keys[SDL_SCANCODE_RIGHT]) {
		player->acceleration_x = 5.8f;
	}
	else {
		player->acceleration_x = 0;
	}

	//Global entities update
	for (size_t i = 0; i < entities.size(); i++)
		if (!entities[i]->isStatic) {

			//Update acceleration and velocities of entities
			entities[i]->Update(elapsed);

			int gridX = 0, gridY = 0;

			//Perform Y calculations and collision with world map
			entities[i]->y += entities[i]->velocity_y * elapsed;
			worldToTileCoordinates(entities[i]->x, entities[i]->y - entities[i]->height / 2, gridX, gridY);
			if (gridX >= 0 && gridY <= 0 && levelData[-gridY][gridX] == 13) {
				entities[i]->y = gridY * TILE_SIZE + entities[i]->height / 2 + 0.000001f;
				entities[i]->velocity_y = 0;
				entities[i]->collidedBottom = true;
				baseTouchX = gridX;
			}
			worldToTileCoordinates(entities[i]->x, entities[i]->y + entities[i]->height / 2, gridX, gridY);
			if (gridX >= 0 && gridY <= 0 && levelData[-gridY][gridX] == 13) {
				entities[i]->y = (gridY - 1) * TILE_SIZE - entities[i]->height / 2 - 0.000001f;
				entities[i]->velocity_y = 0;
				entities[i]->collidedTop = true;
			}

			//Perform X calculations and collision with world map
			entities[i]->x += entities[i]->velocity_x * elapsed;
			worldToTileCoordinates(entities[i]->x - entities[i]->width / 2, entities[i]->y, gridX, gridY);
			if (gridX >= 0 && gridY <= 0 && levelData[-gridY][gridX] == 13) {
				entities[i]->x = (gridX + 1) * TILE_SIZE + entities[i]->width / 2 + 0.000001f;
				entities[i]->velocity_x = 0;
				entities[i]->collidedLeft = true;
			}
			worldToTileCoordinates(entities[i]->x + entities[i]->width / 2, entities[i]->y, gridX, gridY);
			if (gridX >= 0 && gridY <= 0 && levelData[-gridY][gridX] == 13) {
				entities[i]->x = gridX * TILE_SIZE - entities[i]->width / 2 - 0.000001f;
				entities[i]->velocity_x = 0;
				entities[i]->collidedRight = true;
			}

			if (entities[i]->y < TILE_SIZE * -LEVEL_HEIGHT) {
				entities[i]->y = TILE_SIZE * -2;
				entities[i]->x = TILE_SIZE * baseTouchX + entities[i]->width / 2;
				entities[i]->velocity_y = 0;
			}
		}
}

void ClassDemoApp::Render() {
	glClear(GL_COLOR_BUFFER_BIT);

	viewMatrix.identity();
	viewMatrix.Translate(-player->x, -player->y, 0);

	program->setModelMatrix(modelMatrix);
	program->setViewMatrix(viewMatrix);
	program->setProjectionMatrix(projectionMatrix);

	glUseProgram(program->programID);

	std::vector<float> vertexData;
	std::vector<float> texCoordData;

	int noDraws = 0;
	for (int y = 0; y < LEVEL_HEIGHT; y++) {
		for (int x = 0; x < LEVEL_WIDTH; x++) {
			if (levelData[y][x] != 254) {
				float u = (float)(((int)levelData[y][x]) % 14) / (float)14;
				float v = (float)(((int)levelData[y][x]) / 14) / (float)8;

				float spriteWidth = 1.0f / (float)14;
				float spriteHeight = 1.0f / (float)8;

				vertexData.insert(vertexData.end(), {
					TILE_SIZE * x, -TILE_SIZE * y,
					TILE_SIZE * x, -TILE_SIZE * (y + 1),
					TILE_SIZE * (x + 1), -TILE_SIZE * (y + 1),

					TILE_SIZE * x, -TILE_SIZE * y,
					TILE_SIZE * (x + 1), -TILE_SIZE * (y + 1),
					TILE_SIZE * (x + 1), -TILE_SIZE * y
				});

				texCoordData.insert(texCoordData.end(), {
					u, v,
					u, v + spriteHeight,
					u + spriteWidth, v + spriteHeight,

					u, v,
					u + spriteWidth, v + spriteHeight,
					u + spriteWidth, v
				});
			}
			else
				noDraws++;
		}
	}

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, &vertexData[0]);
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, &texCoordData[0]);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, tileTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6 * (LEVEL_HEIGHT * LEVEL_WIDTH - noDraws));

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);

	for (size_t i = 0; i < entities.size(); i++)
		entities[i]->Draw(program);

	SDL_GL_SwapWindow(displayWindow);
}

bool ClassDemoApp::isDone() {
	return done;
}