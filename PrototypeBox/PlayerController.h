#pragma once

#include <SDL.h>

class PlayerController {
public:
	PlayerController(SDL_Scancode UP, SDL_Scancode DOWN, SDL_Scancode LEFT, SDL_Scancode RIGHT, SDL_Scancode EXTEND);

	SDL_Scancode UP, DOWN, LEFT, RIGHT, EXTEND;
};