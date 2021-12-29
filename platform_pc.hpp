#pragma message "Compile for PC"

#include <stdio.h>
#include <thread>
#include "./platform_shared.hpp"

const char* ROOT_PATH = "./";

ButtonsPressed update() {
	ButtonsPressed ret = {};
	return ret;
}

void setup(AudioCallback callback, void* context) {
	std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(16));
	printf("\e[1;1H\e[2J");
}

#define VAE_PRINT(...) printf(__VA_ARGS__);
#define PRINT_SCREEN(...) printf(__VA_ARGS__);