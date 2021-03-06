#pragma message "Compile for PSP"

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspaudiolib.h>
#include <pspaudio.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <psppower.h>
#include <stdio.h>
#include <cassert>
#include "./platform_shared.hpp"

extern "C" void __cxa_pure_virtual() { while (1); } // needed until tklb::MemoryPool is removed

extern "C"
{
	/* Disable FPU exceptions */
	void _DisableFPUExceptions();
}

const char* ROOT_PATH = "ms0:/PSP/GAME/test/";

/* Define the module info section */
PSP_MODULE_INFO("VAEPSPTEST", 0, 1, 1);
/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

#define VAE_PRINT(...) fprintf(stdout, __VA_ARGS__);
#define PRINT_SCREEN(...) pspDebugScreenPrintf(__VA_ARGS__);

void MyExceptionHandler(PspDebugRegBlock *regs) {
	// never called on PPSSPP
	printf("\n\n");
	pspDebugDumpException(regs);
}

void handleAssert() {
	/* Cause a break exception */
	asm(
		"break\r\n"
	);
	const int maxElem = 32;
	unsigned int results[maxElem];
	int result = pspDebugGetStackTrace(results, maxElem);
	VAE_PRINT("Try getting trace\n");
	for (int i = 0; i < result; i++) {
		VAE_PRINT("%i\t%i\n", i, results[i]);
	}
	VAE_PRINT("trace end\n");
	// assert(false);
}

#define TKLB_ASSERT(condition) if (!(condition)) { fprintf(stdout, "Assert failed at " __FILE__ ":%i\n", __LINE__); handleAssert(); }

/* Exit callback */
int exitCallback(int arg1, int arg2, void *common) {
	sceKernelExitGame();
	return 0;
}

/* Callback thread */
int callbackThread(SceSize args, void *argp) {
	int cbid;
	cbid = sceKernelCreateCallback("Exit Callback", exitCallback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();
	return 0;
}

/* Sets up the callback thread and returns its thread id */
int setupCallbacks(void) {
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", callbackThread, 0x11, 0xFA0, 0, 0);
	if (thid >= 0) {
		sceKernelStartThread(thid, 0, 0);
	}
	return thid;
}

void setup(AudioCallback callback, void* context) {
	VAE_PRINT("Install exception handler\n");
	int ret = pspDebugInstallErrorHandler(MyExceptionHandler);
	// Not implemented on PPSSPP
	VAE_PRINT("Exceptionshandler returned %i\n", ret);
	_DisableFPUExceptions();
	scePowerSetClockFrequency(333, 333, 166);
	setupCallbacks();

	pspAudioInit();
	pspAudioSetChannelCallback(0, callback, context);

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

	pspDebugScreenInit();
}

ButtonsPressed update() {
	ButtonsPressed pressed = { };
	sceDisplayWaitVblankStart();
	pspDebugScreenSetXY(0, 4);
	SceCtrlData pad;
	sceCtrlReadBufferPositive(&pad, 1);
	pressed.X = pad.Buttons & PSP_CTRL_CROSS;
	return pressed;
}

#define VAE_NO_STDIO

void* vae_file_open(const char* path, const char* mode) {
	int handle = sceIoOpen(path, PSP_O_RDONLY, 0777);
	if (handle < 0) { return nullptr; }
	size_t ptr = handle;
	return reinterpret_cast<void*>(ptr);
}

size_t vae_file_seek(void* file, size_t offset, int seek) {
	size_t ptr = reinterpret_cast<size_t>(file);
	size_t ret = sceIoLseek(ptr, offset, seek);
	return ret;
}

size_t vae_file_read(char* dest, size_t size, int, void* file) {
	size_t ptr = reinterpret_cast<size_t>(file);
	int result = sceIoRead(ptr, dest, size);
	return size == result;
}

size_t vae_file_close(void* file) {
	size_t ptr = reinterpret_cast<size_t>(file);
	return sceIoClose(ptr);
}