#include "Mixer.h"

DemoApp::DemoApp() {
	deviceSpec.freq = 44100;
	deviceSpec.format = AUDIO_F32;
	deviceSpec.channels = 1;
	deviceSpec.samples = 512;
	deviceSpec.callback = appAudioCallBack;
}

int DemoApp::loadSound(const char *soundFile) {
	Uint8 *buffer;
	SDL_AudioSpec spec;
	Uint32 bufferSize;

	if (SDL_LoadWAV(soundFile, &spec, &buffer, &bufferSize) == NULL) {
		return -1;
	}

	SDL_AudioCVT cvt;
	SDL_BuildAudioCVT(&cvt, spec.format, spec.channels, spec.freq, AUDIO_F32, 1, 44100);
	cvt.len = bufferSize;
	cvt.buf = new Uint8[bufferSize * cvt.len_mult];
	memcpy(cvt.buf, buffer, bufferSize);

	SDL_ConvertAudio(&cvt);
	SDL_FreeWAV(buffer);

	MixerSound sound;
	sound.buffer = cvt.buf;
	sound.length = cvt.len_cvt;
	sound.loaded = true;
	sound.offset = 0;
	sound.format = AUDIO_F32;
	sound.volume = 1.0;
	sound.playing = false;
	mixSounds.push_back(sound);

	return mixSounds.size() - 1;
}

float mixSamples(float A, float B) {
	if (A < 0 && B < 0) {
		return (A + B) - (A * B) / -1.0f;
	}
	else if (A > 0 && B>0) {
		return (A + B) - (A * B) / 1.0f;
	}
	else {
		return A + B;
	}
}

void DemoApp::appAudioCallBack(void *userdata, Uint8 *stream, int len) {
	DemoApp *app = (DemoApp*)userdata;
	memset(stream, 0, len);

	for (int i = 0; i < app->mixSounds.size(); i++) {
		MixerSound *sound = &app->mixSounds[i];
		if (sound->loaded && sound->playing) {
			for (int s = 0; s < len / 4; s++) {
				float *sourceBuffer = (float*)(sound->buffer + sound->offset);
				((float*)stream)[s] = mixSamples(((float*)stream)[s], (sourceBuffer[s] * sound->volume));
			}
			sound->offset += len;
			if (sound->offset >= sound->length - len) {
				if (sound->loop) {
					sound->offset = 0;
				}
				else {
					sound->playing = false;
				}
			}
		}
	}
}