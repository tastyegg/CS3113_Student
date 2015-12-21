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

void ClassDemoApp::LoadLevel(std::string mapName) {
	std::ifstream infile(mapName);
	std::string line;
	while (std::getline(infile, line)) {
		if (line == "[layer]") {
			while (std::getline(infile, line)) {
				if (line == "") break;
				std::istringstream sStream(line);
				std::string key, value;
				std::getline(sStream, key, '=');
				std::getline(sStream, value);
				if (key == "data") {
					for (int y = 0; y < LEVEL_HEIGHT; y++) {
						std::getline(infile, line);
						std::istringstream lineStream(line);
						std::string tile;

						for (int x = 0; x < LEVEL_WIDTH; x++) {
							std::getline(lineStream, tile, ',');
							unsigned char val = (unsigned char)atoi(tile.c_str());
							if (val > 0) {
								levelData[y][x] = val - 1;
							}
							else {
								levelData[y][x] = NO_TILE;
							}
						}
					}
				}
			}
			break;
		}
	}
}

void ClassDemoApp::Setup() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Box Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, (int)WINDOW_WIDTH, (int)WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef _WINDOWS
	glewInit();
#endif
	done = false;

	srand((unsigned int)time(NULL));

	program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured_lighting.glsl");
	programInfo = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured_colored.glsl");
	tileTexture = LoadTexture("Tileset.png");
	infoBox = LoadTexture("InfoBox.png");

	projectionMatrix.setOrthoProjection(-PROJECTION_WIDTH, PROJECTION_WIDTH, -PROJECTION_HEIGHT, PROJECTION_HEIGHT, -1.0f, 1.0f);

	state = PlayingLevel;
	LoadLevel("map1.txt");

	SheetSprite playerSprite = SheetSprite(LoadTexture("player.png"), 1, 1);
	player = new Entity(playerSprite);
	entities.push_back(player);
	player->entityType = Entity::ENTITY_PLAYER;
	player->width = BOX_SIZE;
	player->height = BOX_SIZE;
	player->spawnX = 2;
	player->spawnY = -2;
	player->y = TILE_SIZE * player->spawnY + player->height / 2;
	player->x = TILE_SIZE * player->spawnX + player->width / 2;
	player->isStatic = false;

	includePlayer2 = false;

	player2 = new Entity(playerSprite);
	if (includePlayer2) {
		entities.push_back(player2);
	}
	player2->entityType = Entity::ENTITY_PLAYER;
	player2->width = BOX_SIZE;
	player2->height = BOX_SIZE;
	player2->spawnX = 2;
	player2->spawnY = -9;
	player2->y = TILE_SIZE * player2->spawnY + player->height / 2;
	player2->x = TILE_SIZE * (player2->spawnX + 2) + player->width / 2;
	player2->isStatic = false;

	Entity* box = new Entity(playerSprite);
	box->entityType = Entity::ENTITY_ENEMY;
	box->width = BOX_SIZE;
	box->height = BOX_SIZE;
	box->spawnX = 21;
	box->spawnY = -21;
	box->y = TILE_SIZE * box->spawnY + box->height / 2;
	box->x = TILE_SIZE * (box->spawnX + 2) + box->width / 2;
	box->isStatic = false;
	entities.push_back(box);


	players[0] = player;
	players[1] = player2;

	//Set Controls Here
	controls[0] = new PlayerController(SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_K);
	controls[1] = new PlayerController(SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_RCTRL);

	infoBoxTimer = 8;
}

void ClassDemoApp::UpdateAndRender() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
		else if (false && event.type == SDL_MOUSEBUTTONDOWN && state == PlayingLevel) {	//debugging - moves player(s) to cursor
			if (event.button.button == SDL_BUTTON_LEFT) {
				if (!includePlayer2) {
					player->x += (event.button.x / WINDOW_WIDTH - 0.5f) * PROJECTION_WIDTH * 2;
					player->y -= (event.button.y / WINDOW_HEIGHT - 0.5f) * PROJECTION_HEIGHT * 2;
					player->velocity_y = 0;
				}
				else {
					player->x = (player->x + player2->x) / 2 + (event.button.x / WINDOW_WIDTH - 0.5f) * PROJECTION_WIDTH * 2;
					player->y = (player->y + player2->y) / 2 - (event.button.y / WINDOW_HEIGHT - 0.5f) * PROJECTION_HEIGHT * 2;
					player->velocity_y = 0;
				}
			}
			else if (event.button.button == SDL_BUTTON_RIGHT) {
				if (includePlayer2) {
					player2->x = (player->x + player2->x) / 2 + (event.button.x / WINDOW_WIDTH - 0.5f) * PROJECTION_WIDTH * 2;
					player2->y = (player->y + player2->y) / 2 - (event.button.y / WINDOW_HEIGHT - 0.5f) * PROJECTION_HEIGHT * 2;
					player2->velocity_y = 0;
				}
			}
		}
		else if (event.type == SDL_KEYDOWN) {
			infoBoxTimer *= 0.05f;
			if (!includePlayer2 && (event.key.keysym.scancode == controls[1]->UP ||
				event.key.keysym.scancode == controls[1]->DOWN ||
				event.key.keysym.scancode == controls[1]->LEFT ||
				event.key.keysym.scancode == controls[1]->RIGHT ||
				event.key.keysym.scancode == controls[1]->EXTEND)) {
				includePlayer2 = true;
				entities.push_back(player2);
			}
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

	infoBoxTimer -= elapsed;
	movingColor += elapsed * 0.3f;
	Render();
}

void ClassDemoApp::Update(float elapsed) {
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	for (int i = 0; i < 2; i++) {
		//Player control
		if (keys[controls[i]->EXTEND]) {
			if (keys[controls[i]->UP]) {
				if (!players[i]->collidedTop) {
					float oldHeight = players[i]->height;
					players[i]->height = lerp(players[i]->height, 5 * BOX_SIZE, 0.03f);
					players[i]->y += (players[i]->height - oldHeight) / 2;
				}
				float oldWidth = players[i]->width;
				players[i]->width = lerp(players[i]->width, BOX_SIZE, 0.04f);
				players[i]->x += players[i]->lastDirectionX * (players[i]->width - oldWidth) / 2;
			}
			else if (keys[controls[i]->DOWN]) {
				if (!players[i]->collidedBottom) {
					float oldHeight = players[i]->height;
					players[i]->height = lerp(players[i]->height, 5 * BOX_SIZE, 0.03f);
					players[i]->y -= (players[i]->height - oldHeight) / 2;
				}
				float oldWidth = players[i]->width;
				players[i]->width = lerp(players[i]->width, BOX_SIZE, 0.04f);
				players[i]->x += players[i]->lastDirectionX * (players[i]->width - oldWidth) / 2;
			}
			else if (keys[controls[i]->LEFT]) {
				if (!players[i]->collidedLeft) {
					float oldWidth = players[i]->width;
					players[i]->width = lerp(players[i]->width, 5 * BOX_SIZE, 0.03f);
					players[i]->x -= (players[i]->width - oldWidth) / 2;
					players[i]->lastDirectionX = 1;
				}
				float oldHeight = players[i]->height;
				players[i]->height = lerp(players[i]->height, BOX_SIZE, 0.08f);
				players[i]->y += (oldHeight - players[i]->height) / 2;
				players[i]->velocity_y += (oldHeight - players[i]->height) * 3.4f;
			}
			else if (keys[controls[i]->RIGHT]) {
				if (!players[i]->collidedRight) {
					float oldWidth = players[i]->width;
					players[i]->width = lerp(players[i]->width, 5 * BOX_SIZE, 0.03f);
					players[i]->x += (players[i]->width - oldWidth) / 2;
					players[i]->lastDirectionX = -1;
				}
				float oldHeight = players[i]->height;
				players[i]->height = lerp(players[i]->height, BOX_SIZE, 0.08f);
				players[i]->y += (oldHeight - players[i]->height) / 2;
				players[i]->velocity_y += (oldHeight - players[i]->height) * 3.4f;
			}
			else {
				float oldWidth = players[i]->width;
				players[i]->width = lerp(players[i]->width, BOX_SIZE, 0.003f);
				players[i]->x += players[i]->lastDirectionX * (players[i]->width - oldWidth) / 2;
				float oldHeight = players[i]->height;
				players[i]->height = lerp(players[i]->height, BOX_SIZE, 0.006f);
				players[i]->y += (oldHeight - players[i]->height) / 2;
				players[i]->velocity_y += (oldHeight - players[i]->height) * 3.4f;
			}
			players[i]->acceleration_x = 0;
		}
		else {
			if (keys[controls[i]->UP]) {
				float oldHeight = players[i]->height;
				players[i]->height = lerp(players[i]->height, BOX_SIZE, 0.08f);
				players[i]->y += (oldHeight - players[i]->height) / 2;
				players[i]->velocity_y += (oldHeight - players[i]->height) * 3.6f;
			}
			else if (keys[controls[i]->DOWN]) {
				float oldHeight = players[i]->height;
				players[i]->height = lerp(players[i]->height, BOX_SIZE, 0.06f);
				players[i]->y -= (oldHeight - players[i]->height) / 2;
			}
			else {
				float oldHeight = players[i]->height;
				players[i]->height = lerp(players[i]->height, BOX_SIZE, 0.04f);
				players[i]->y += (oldHeight - players[i]->height) / 2;
				players[i]->velocity_y += (oldHeight - players[i]->height);
			}
			if (keys[controls[i]->LEFT]) {
				players[i]->acceleration_x = -0.2f;
				float oldWidth = players[i]->width;
				players[i]->width = lerp(players[i]->width, BOX_SIZE, 0.09f);
				players[i]->x -= (oldWidth - players[i]->width) / 2;
				players[i]->velocity_x -= (oldWidth - players[i]->width) * 1.4f;
			}
			else if (keys[controls[i]->RIGHT]) {
				players[i]->acceleration_x = 0.2f;
				float oldWidth = players[i]->width;
				players[i]->width = lerp(players[i]->width, BOX_SIZE, 0.09f);
				players[i]->x += (oldWidth - players[i]->width) / 2;
				players[i]->velocity_x += (oldWidth - players[i]->width) * 1.4f;
			}
			else {
				float oldWidth = players[i]->width;
				players[i]->width = lerp(players[i]->width, BOX_SIZE, 0.04f);
				players[i]->x += players[i]->lastDirectionX * (players[i]->width - oldWidth) / 2;
				players[i]->acceleration_x = 0;
			}
		}
	}

	//Global entities update
	for (size_t i = 0; i < entities.size(); i++) {
		if (!entities[i]->isStatic) {
			//Update acceleration and velocities of entities
			entities[i]->Update(elapsed);

			for (size_t j = 0; j < entities.size(); j++) {
				if (j != i && entities[i]->CollidesWith(entities[j])) {
					if (entities[i]->entityType == Entity::ENTITY_PLAYER) {
						float xDiff = entities[i]->x - entities[j]->x;
						float yDiff = entities[i]->y - entities[j]->y;
						float topPen = (entities[j]->y + entities[j]->height / 2) - (entities[i]->y - entities[i]->height / 2);
						float bottomPen = (entities[i]->y + entities[i]->height / 2) - (entities[j]->y - entities[j]->height / 2);
						float leftPen = (entities[j]->x + entities[j]->width / 2) - (entities[i]->x - entities[i]->width / 2);
						float rightPen = (entities[i]->x + entities[i]->width / 2) - (entities[j]->x - entities[j]->width / 2);

						if (std::max(leftPen, rightPen) / (entities[i]->width + entities[j]->width) > std::max(topPen, bottomPen) / (entities[i]->height + entities[j]->height)) {
							//Horizontial collision : this condition indicates that entityI is to a side of entityJ
							entities[i]->velocity_x = (entities[i]->velocity_x + entities[j]->velocity_x) / 2;
							entities[j]->velocity_x = entities[i]->velocity_x;
							bool collisionLR = entities[i]->entityType == Entity::ENTITY_PLAYER &&
								((entities[i] == players[0] && keys[controls[0]->EXTEND] && (keys[controls[0]->LEFT] || keys[controls[0]->RIGHT])) ||
								(entities[i] == players[1] && keys[controls[1]->EXTEND] && (keys[controls[1]->LEFT] || keys[controls[1]->RIGHT])));

							if (xDiff > 0) {	//J <- I
								if (!entities[j]->collidedLeft && collisionLR) {
									entities[j]->x = entities[i]->x - entities[i]->width / 2 - entities[j]->width / 2;
								}
								else {
									entities[i]->x = entities[j]->x + entities[j]->width / 2 + entities[i]->width / 2;
									entities[i]->collidedLeft = true; entities[j]->collidedRight = true;
								}
							}
							else if (xDiff < 0) {
								if (!entities[j]->collidedRight && collisionLR) {
									entities[j]->x = entities[i]->x + entities[i]->width / 2 + entities[j]->width / 2;
								}
								else {
									entities[i]->x = entities[j]->x - entities[j]->width / 2 - entities[i]->width / 2;
									entities[i]->collidedRight = true; entities[j]->collidedLeft = true;
								}
							}
						}
						else {
							if (yDiff > 0) {	//I is on top
								entities[i]->y = entities[j]->y + entities[j]->height / 2 + entities[i]->height / 2;
								entities[j]->velocity_y = (entities[j]->velocity_y + entities[j]->velocity_y) / 2;
								entities[i]->collidedBottom = true; entities[j]->collidedTop = true;
								entities[i]->velocity_y = 0;
							}
							else if (yDiff < 0) {	//J is on top
								entities[j]->y = entities[i]->y + entities[i]->height / 2 + entities[j]->height / 2;
								if (!(entities[i]->entityType == Entity::ENTITY_PLAYER && !entities[j]->collidedTop &&
									((entities[i] == players[0] && keys[controls[0]->EXTEND] && keys[controls[0]->UP]) ||
									(entities[i] == players[1] && keys[controls[1]->EXTEND] && keys[controls[1]->UP])))) {
									entities[i]->collidedTop = true; entities[j]->collidedBottom = true;
								}
								entities[j]->velocity_y = 0;
							}

						}
					}
				}
			}

			int gridX = 0, gridY = 0;

			//Perform Y calculations and collision with world map
			entities[i]->y += entities[i]->velocity_y * elapsed;
			for (int w = 0; w < 5; w++) {
				worldToTileCoordinates(entities[i]->x + ((float)w - 2) / 5.0f * entities[i]->width, entities[i]->y - entities[i]->height / 2, gridX, gridY);
				if (levelData[-gridY][gridX] != NO_TILE) {
					entities[i]->y = gridY * TILE_SIZE + entities[i]->height / 2 + 0.000001f;
					entities[i]->velocity_y = 0;
					entities[i]->collidedBottom = true;
					entities[i]->spawnX = gridX;
					entities[i]->spawnY = gridY;
				}
				worldToTileCoordinates(entities[i]->x + ((float)w - 2) / 5.0f * entities[i]->width, entities[i]->y + entities[i]->height / 2, gridX, gridY);
				if (levelData[-gridY][gridX] != NO_TILE) {
					entities[i]->y = (gridY - 1) * TILE_SIZE - entities[i]->height / 2 - 0.000001f;
					if (entities[i]->velocity_y > 0)
						entities[i]->velocity_y = 0;
					entities[i]->collidedTop = true;
				}
			}

			//Perform X calculations and collision with world map
			entities[i]->x += entities[i]->velocity_x * elapsed;
			for (int h = 0; h < 5; h++) {
				worldToTileCoordinates(entities[i]->x - entities[i]->width / 2, entities[i]->y + ((float)h - 2) / 5.0f * entities[i]->height, gridX, gridY);
				if (levelData[-gridY][gridX] != NO_TILE) {
					entities[i]->x = (gridX + 1) * TILE_SIZE + entities[i]->width / 2 - 0.000001f;
					entities[i]->velocity_x = 0;
					entities[i]->collidedLeft = true;
				}
				worldToTileCoordinates(entities[i]->x + entities[i]->width / 2, entities[i]->y + ((float)h - 2) / 5.0f * entities[i]->height, gridX, gridY);
				if (levelData[-gridY][gridX] != NO_TILE) {
					entities[i]->x = gridX * TILE_SIZE - entities[i]->width / 2 + 0.0000001f;
					entities[i]->velocity_x = 0;
					entities[i]->collidedRight = true;
				}
			}

			//Downward Bound Reset for player
			if (entities[i]->y - entities[i]->height / 2 < TILE_SIZE * (-LEVEL_HEIGHT + 0.5f)) {
				//if (entities[i]->entityType == entities[i]->ENTITY_PLAYER) {
					entities[i]->width = TILE_SIZE;
					entities[i]->height = TILE_SIZE;
					entities[i]->y = TILE_SIZE * entities[i]->spawnY + entities[i]->height / 2;
					entities[i]->x = TILE_SIZE * entities[i]->spawnX + entities[i]->width / 2;
					entities[i]->velocity_y = 0;
				//}
			}
		}
	}

	GLint lightPositionsUniform = glGetUniformLocation(program->programID, "lightPositions");
	GLfloat lightPositions[2 * 2 * 4];

	float xDiff = player->x - player2->x;
	float yDiff = player->y - player2->y;
	float scale = 1.0f;
	if (fabs(xDiff) * 0.8f > fabs(yDiff) * 1.4f) {
		if (fabs(xDiff) > PROJECTION_WIDTH * 1.4f) {
			scale = PROJECTION_WIDTH * 1.4f / fabs(xDiff);
		}
	}
	else if (fabs(yDiff) > PROJECTION_WIDTH * 0.8f) {
		scale = PROJECTION_WIDTH * 0.8f / fabs(yDiff);
	}

	for (int i = 0; i < 2; i++) {
		if (!includePlayer2) {
			lightPositions[i * 8] = -players[i]->width / 2;
			lightPositions[i * 8 + 1] = -players[i]->height / 2;
			lightPositions[i * 8 + 2] = players[i]->width / 2;
			lightPositions[i * 8 + 3] = -players[i]->height / 2;
			lightPositions[i * 8 + 4] = -players[i]->width / 2;
			lightPositions[i * 8 + 5] = players[i]->height / 2;
			lightPositions[i * 8 + 6] = players[i]->width / 2;
			lightPositions[i * 8 + 7] = players[i]->height / 2;
			break;
		}
		else {
			lightPositions[i * 8] = (-players[i]->width / 2 - (i - 0.5f) * (xDiff)) * scale;
			lightPositions[i * 8 + 1] = (-players[i]->height / 2 - (i - 0.5f) * (yDiff)) * scale;
			lightPositions[i * 8 + 2] = (players[i]->width / 2 - (i - 0.5f) * (xDiff)) * scale;
			lightPositions[i * 8 + 3] = (-players[i]->height / 2 - (i - 0.5f) * (yDiff)) * scale;
			lightPositions[i * 8 + 4] = (-players[i]->width / 2 - (i - 0.5f) * (xDiff)) * scale;
			lightPositions[i * 8 + 5] = (players[i]->height / 2 - (i - 0.5f) * (yDiff)) * scale;
			lightPositions[i * 8 + 6] = (players[i]->width / 2 - (i - 0.5f) * (xDiff)) * scale;
			lightPositions[i * 8 + 7] = (players[i]->height / 2 - (i - 0.5f) * (yDiff)) * scale;
		}
	}

	glUniform2fv(lightPositionsUniform, 8, lightPositions);
}

void ClassDemoApp::Render() {
	glClear(GL_COLOR_BUFFER_BIT);
	//glClearColor(0, 0.3f, 0.7f, 1.0f);

	modelMatrix.identity();
	viewMatrix.identity();

	if (includePlayer2) {
		float xDiff = fabs(player->x - player2->x);
		float yDiff = fabs(player->y - player2->y);
		if (xDiff * 0.8f > yDiff * 1.4f) {
			if (xDiff > PROJECTION_WIDTH * 1.4f) {
				viewMatrix.Scale(PROJECTION_WIDTH * 1.4f / xDiff, PROJECTION_WIDTH * 1.4f / xDiff, 1);
			}
		}
		else if (yDiff > PROJECTION_WIDTH * 0.8f) {
			viewMatrix.Scale(PROJECTION_WIDTH * 0.8f / yDiff, PROJECTION_WIDTH * 0.8f / yDiff, 1);
		}
		viewMatrix.Translate(-(player->x + player2->x) / 2, -(player->y + player2->y) / 2, 0);
	}
	else {
		viewMatrix.Translate(-player->x, -player->y, 0);
	}

	program->setModelMatrix(modelMatrix);
	program->setViewMatrix(viewMatrix);
	program->setProjectionMatrix(projectionMatrix);

	glUseProgram(program->programID);

	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	std::vector<float> colors;

	int noDraws = 0;
	for (int y = 0; y < LEVEL_HEIGHT; y++) {
		for (int x = 0; x < LEVEL_WIDTH; x++) {
			if (levelData[y][x] != NO_TILE) {
				float u = (float)(((int)levelData[y][x]) % 5) / (float)5;
				float v = (float)(((int)levelData[y][x]) / 5) / (float)3;

				float spriteWidth = 1.0f / (float)5;
				float spriteHeight = 1.0f / (float)3;

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
				float colorM = movingColor + (x + y) * 0.08f;
				float colorR = powf(sinf(colorM), 2) * 0.8f;
				float colorG = powf(sinf(colorM + 2.0f), 2) * 0.8f;
				float colorB = powf(sinf(colorM + 4.0f), 2) * 0.8f;
				for (int i = 0; i < 6; i++) {
					colors.push_back(colorR);
					colors.push_back(colorG);
					colors.push_back(colorB);
					colors.push_back(1.0f);
				}
			}
			else
				noDraws++;
		}
	}

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, &vertexData[0]);
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, &texCoordData[0]);
	glEnableVertexAttribArray(program->texCoordAttribute);

	GLuint colorAttribute = glGetAttribLocation(program->programID, "color");
	glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, false, 0, &colors[0]);
	glEnableVertexAttribArray(colorAttribute);

	glBindTexture(GL_TEXTURE_2D, tileTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6 * (LEVEL_HEIGHT * LEVEL_WIDTH - noDraws));

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
	glDisableVertexAttribArray(colorAttribute);

	for (size_t i = 0; i < entities.size(); i++)
		entities[i]->Draw(program);

	if (infoBoxTimer > 0) {
		modelMatrix.identity();
		viewMatrix.identity();
		programInfo->setModelMatrix(modelMatrix);
		programInfo->setViewMatrix(viewMatrix);
		programInfo->setProjectionMatrix(projectionMatrix);

		glUseProgram(programInfo->programID);

		vertexData.clear();
		texCoordData.clear();
		colors.clear();

		vertexData.insert(vertexData.end(), {
			-2.1f, -2.1f,
			-2.1f,  2.1f,
			 2.1f,  2.1f,

			-2.1f, -2.1f,
			 2.1f,  2.1f,
			 2.1f, -2.1f
		});
		texCoordData.insert(texCoordData.end(), {
			0,1,
			0,0,
			1,0,
			0,1,
			1,0,
			1,1
		});

		float boxTransparency = std::min(infoBoxTimer / 1.0f, 0.9f);
		colors.insert(colors.end(), {
			1.0f, 1.0f, 1.0f, boxTransparency,
			1.0f, 1.0f, 1.0f, boxTransparency,
			1.0f, 1.0f, 1.0f, boxTransparency,

			1.0f, 1.0f, 1.0f, boxTransparency,
			1.0f, 1.0f, 1.0f, boxTransparency,
			1.0f, 1.0f, 1.0f, boxTransparency,
		});

		glVertexAttribPointer(programInfo->positionAttribute, 2, GL_FLOAT, false, 0, &vertexData[0]);
		glEnableVertexAttribArray(programInfo->positionAttribute);
		glVertexAttribPointer(programInfo->texCoordAttribute, 2, GL_FLOAT, false, 0, &texCoordData[0]);
		glEnableVertexAttribArray(programInfo->texCoordAttribute);

		GLuint colorAttribute = glGetAttribLocation(programInfo->programID, "color");
		glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, false, 0, &colors[0]);
		glEnableVertexAttribArray(colorAttribute);

		glBindTexture(GL_TEXTURE_2D, infoBox);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(programInfo->positionAttribute);
		glDisableVertexAttribArray(programInfo->texCoordAttribute);
		glDisableVertexAttribArray(colorAttribute);
	}

	SDL_GL_SwapWindow(displayWindow);
}

bool ClassDemoApp::isDone() {
	return done;
}