#
# KallistiGL test program
# (c)2001 Benoit Miller
#   

TARGET = basic_test.elf
OBJS = main.o

all: OBJS += romdisk.o 
all: rm-elf romdisk.o $(TARGET) dist


include $(KOS_BASE)/Makefile.rules


sd: KOS_CFLAGS += -DSDCARD_BUILD
	
BUILDCMD=$(KOS_CC) $(KOS_CFLAGS) -I../../build/include/ -I./ -DARCH_DC -O2 -funroll-loops -c -o 

clean:
	-rm -f $(TARGET) $(OBJS) romdisk.o romdisk.img
	-rm -rf romdisk

rm-elf:
	-rm -f $(TARGET) romdisk.img romdisk.o

sd: rm-elf $(TARGET) dist
	sh-elf-objcopy -R .stack -O binary $(TARGET) basic_test.bin

$(TARGET): $(OBJS)
	$(KOS_CC) $(KOS_CFLAGS) $(KOS_LDFLAGS) -o $(TARGET) $(KOS_START) \
		$(OBJS) $(OBJEXTRA)-L$(KOS_BASE)/lib -L../../ -lcubicvr -lstdc++ -lkglx -lpcx -lkosutils -lbulletdynamics -lbulletcollision -lbulletmath -lm $(KOS_LIBS) -L../../build/lib/

romdisk.img:
	mkdir romdisk || true
	cp ../resources/images/crate.pcx romdisk/
	cp ../resources/images/grass.pcx romdisk/
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

