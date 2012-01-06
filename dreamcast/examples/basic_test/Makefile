
BASE_NAME = basic_test
TARGET = $(BASE_NAME).elf
TARGET_BIN = $(BASE_NAME).bin
OBJS = main.o
RESOURCES = ../resources/images/crate.pcx

all: OBJS += romdisk.o 
all: LIBS += -lkglx 
all: rm-elf romdisk.o $(TARGET) dist

rc4: OBJS += romdisk.o 
rc4: LIBS += -lglx 
rc4: rm-elf romdisk.o $(TARGET) dist


include $(KOS_BASE)/Makefile.rules

sd: LIBS += -lkglx
sd: KOS_CFLAGS += -DSD_CARD
	
BUILDCMD=$(KOS_CC) $(KOS_CFLAGS) -I../../build/include/ -I./ -DARCH_DC -O2 -funroll-loops -c -o 

clean:
	-rm -f $(TARGET) $(OBJS) romdisk.o romdisk.img
	-rm -rf romdisk $(BASE_NAME)

rm-elf:
	-rm -f $(TARGET) romdisk.img romdisk.o

sd: rm-elf $(TARGET) dist
	rm -rf $(BASE_NAME)
	mkdir $(BASE_NAME)
	cp $(RESOURCES) $(BASE_NAME)/
	sh-elf-objcopy -R .stack -O binary $(TARGET) $(BASE_NAME)/$(BASE_NAME).bin

$(TARGET): $(OBJS)
	$(KOS_CC) $(KOS_CFLAGS) $(KOS_LDFLAGS) -o $(TARGET) $(KOS_START) \
		$(OBJS) $(OBJEXTRA)-L$(KOS_BASE)/lib -L../../ -lcubicvr -lstdc++ $(LIBS) -lpcx -lkosutils -lbulletdynamics -lbulletcollision -lbulletmath -lm $(KOS_LIBS) -L../../build/lib/

romdisk.img:
	mkdir romdisk || true
	cp $(RESOURCES) romdisk/
	$(KOS_GENROMFS) -f romdisk.img -d romdisk -v

romdisk.o: romdisk.img
	$(KOS_BASE)/utils/bin2o/bin2o romdisk.img romdisk romdisk.o

main.o: main.cpp
	$(BUILDCMD) main.o $^
		
run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

dist:
	rm -f $(OBJS)
	$(KOS_STRIP) $(TARGET)

