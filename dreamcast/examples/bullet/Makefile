#
# KallistiGL test program
# (c)2001 Benoit Miller
#   

TARGET = bullet.elf
OBJS = main.o romdisk.o

all: rm-elf $(TARGET) dist

include $(KOS_BASE)/Makefile.rules

clean:
	-rm -f $(TARGET) $(OBJS) romdisk.img
	-rm -rf romdisk

rm-elf:
	-rm -f $(TARGET) romdisk.img romdisk.o

INCLUDES=-I../../build/include/ -I./
BUILDCMD=$(KOS_CC) $(KOS_CFLAGS) $(KOS_LDFLAGS) $(INCLUDES) -DARCH_DC -c -o
		
# BUILDCMD=$(KOS_CC) $(KOS_CFLAGS) $(KOS_LDFLAGS) -o $(TARGET) $(KOS_START) \
#	$(OBJS) Timer.o -L$(KOS_BASE)/lib -lgl -lpcx -lkosutils $(KOS_LIBS) -L../../../dependencies/product/lib/bullet/dreamcast/ -I../../../dependencies/product/include/bullet/ -I./ -I../../../cubicvr/include/ -c

$(TARGET): $(OBJS) romdisk.o
	$(KOS_CC) $(KOS_CFLAGS) $(KOS_LDFLAGS) -o $(TARGET) $(KOS_START) \
		$(OBJS) $(OBJEXTRA)-L$(KOS_BASE)/lib -L../../ -lcubicvr -lgcc -lstdc++ -lglx -lpcx -lkosutils -lbulletdynamics -lbulletcollision -lbulletmath -lm $(KOS_LIBS) -L../../build/lib/


# -L../../dcbullet/

#  -L../../../dependencies/product/lib/bullet/dreamcast/

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

