TARGET = alsample
OBJS = main.o disablefpu.o

INCDIR = 
CFLAGS = -O0 -g
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti -fno-threadsafe-statics
ASFLAGS = $(CFLAGS)

LIBDIR = 
LDFLAGS = 
LIBS = -lpspaudiolib -lpspaudio -lpsppower

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = VAE PSP TEST

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak