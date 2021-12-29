
using AudioCallback = void (*)(void* buf, unsigned int frames, void* pdata);

struct ButtonsPressed {
	bool X = false;
};

void setup(AudioCallback);

ButtonsPressed update();
