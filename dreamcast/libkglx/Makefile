# KallistiOS 1.1.8
#
# addons/libgl Makefile
# (c)2001 Dan Potter
#
# $Id: Makefile,v 1.4 2002/06/28 04:41:52 axlen Exp $

OBJS  = \
	src/glapi.o \
	src/gldlist.o \
	src/gldepth.o \
	src/gldraw.o \
	src/glkos.o \
	src/gllight.o \
	src/glmisc.o\
	src/gltex.o \
	src/gltrans.o \
	src/glvars.o \
	src/glblend.o \
	src/glfog.o \
	src/todo.o

SUBDIRS =

# Note: We need Chui KGLX includes, and KOS_CFLAGS reads the kos-ports include directory by default.
# Instead, redefine KOS_CFLAGS without the kos-ports include (and just use the Chui's includes).
KOS_CFLAGS = -O2 -I${KOS_BASE}/include -I${KOS_BASE}/kernel/arch/${KOS_ARCH}/include -I${KOS_BASE}/addons/include \
-D_arch_${KOS_ARCH} -D_arch_sub_${KOS_SUBARCH} -Wall -g -fno-builtin -fno-strict-aliasing -ml -m4-single-only -fno-crossjumping \
-Isrc -Iinclude 

KOS_CFLAGS+= -DREDUCED_MEMORY
#KOS_CFLAGS+= -DTODO_USE_ASSERT
#KOS_CFLAGS+= -DTODO_WARN


TARGET = libkglx.a 

include $(KOS_BASE)/addons/Makefile.prefab 


gldraw.o: gldraw.c gloparray.c
