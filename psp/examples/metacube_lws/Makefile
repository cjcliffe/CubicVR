TARGET = CubicVR_MetaCube
OBJS = main.o Gamepad.o

INCDIR = ./ ../../build/include/

CFLAGS = -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti -DARCH_PSP
ASFLAGS = $(CFLAGS)


LIBDIR = ../../build/lib
LIBS = -lcubicvr -lbulletdynamics -lbulletcollision -lbulletmath -lpspgum -lpspgu -lpspaudiolib -lpspaudio -lpsppower -lpsprtc -lm -lstdc++
LDFLAGS =
# -lmad


EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = CubicVR - Metacube w Lighting
PSP_EBOOT_ICON = ICON0.PNG
PSP_EBOOT_PIC1 = PIC1.PNG

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

delete:
	rm -Rf $(TARGET)
	rm -Rf $(TARGET)%
	rm -Rf ./content
	
upload:
	cp -R $(TARGET) $(PSPGAMEDIR)
	cp -R $(TARGET)% $(PSPGAMEDIR) 

content:
	@mkdir ./content || true
	@cp ../resources/models/metacube* ./content/
	@cp ../resources/images/DiffMap.tga ./content/
	@cp ../resources/images/spot.tga ./content/

	
uploadcontent: content
	cp -R ./content/* $(PSPGAMEDIR)$(TARGET)
	
doeverything: delete clean kxploit upload uploadcontent

cubicvr:
	./make_cvr

deplus: cubicvr doeverything

