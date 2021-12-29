#ifdef _PSP_FW_VERSION
	#include "./platform_psp.hpp"
#else
	#include "./platform_pc.hpp"
#endif

#include <limits.h>

#define VAE_LOG_VOICES
#define VAE_LOG_EVENTS
#define VAE_NO_EXCEPT
#define VAE_NO_SIMD
#define VAE_NO_AUDIO_THREAD
#define TKLB_RESAMPLER_LINEAR
#define VAE_NO_AUDIO_DEVICE
#define VAE_NO_WAV
#include "./VAE/src/core/vae_engine.hpp"



/* This function gets called by pspaudiolib every time the
   audio buffer needs to be filled. The sample format is
   16-bit, stereo. */
void audioCallback(void* buf, unsigned int length, void* userdata) {
	auto& engine = *reinterpret_cast<vae::core::Engine*>(userdata);
	short* ubuf = (short*) buf;
	engine.process(length, ubuf, 2);
	engine.update();
}


int main() {
	bool success = false;
	vae::core::Engine engine;
	vae::EmitterHandle emitter;
	setup(audioCallback, &engine);

	vae::EngineConfig settings;
	settings.internalSampleRate = 44100;
	settings.rootPath = ROOT_PATH;
	settings.hrtfVoices = 0;
	settings.voices = 8;
	settings.preAllocatedEmitters = 64;
	settings.virtualVoices = 32;
	settings.finishedVoiceQueueSize = 16;
	engine.init(settings);
	success = engine.loadBank("smallbank") == vae::Result::Success;
	emitter = engine.createEmitter();
	engine.fireEvent(0, 1, emitter);
	unsigned int ref = 0;
	bool released = true;
	while(1) {
		auto pressed = update();
		if (success) {
			PRINT_SCREEN("Bank loaded\n");
		}
		if (pressed.X) {
			PRINT_SCREEN("\n X PRESSED");
			if (released) {
				engine.fireEvent(0, 0, emitter);
				released = false;
			}
		} else {
			released = true;
			PRINT_SCREEN("\n NOT X PRESSED");
		}
		// engine.setSpeed(emitter, sin(ref * 0.3) * 0.2 + 1.0);
		PRINT_SCREEN("\nTime %u", engine.getStreamTime());
		PRINT_SCREEN("\nVoices %u", engine.getActiveVoiceCount());
		PRINT_SCREEN("\nReference %u", ref);
		ref++;
	}
	return 0;
}
