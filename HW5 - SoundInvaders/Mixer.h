#pragma once

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
#include "ClassDemoApp.h"
#include <vector>

class MixerSound {
public:
	Uint32 offset;
	Uint32 length;
	Uint8 *buffer;
	float volume;
	SDL_AudioFormat format;
	bool loaded;
	bool playing;
	bool loop;
};

class DemoApp {
public:
	DemoApp();
	~DemoApp();
	int loadSound(const char *soundFile);
	void appAudioCallBack(void *userdata, Uint8 *stream, int len);
private:
	SDL_AudioSpec deviceSpec;
	std::vector<MixerSound> mixSounds;
};