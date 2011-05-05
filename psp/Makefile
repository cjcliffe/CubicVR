INCDIR = ../cubicvr/include/ ../cubicvr/external/bullet-2.78/src/ ./ {$PSPSDK}/psp/include
CFLAGS = -O3 
#CFLAGS = -g -Wall
CXXFLAGS = $(CFLAGS) -fexceptions -fno-rtti -DARCH_PSP 
ASFLAGS = $(CFLAGS)

LIBDIR = 
LDFLAGS =
PSPSDK=$(shell psp-config --pspsdk-path)

ifeq ($(PSPSDK),)
$(error $$(PSPSDK) is undefined.  Use "PSPSDK := $$(shell psp-config --pspsdk-path)" in your Makefile)
endif

CC       = psp-gcc
CXX      = psp-g++
AS       = psp-gcc
LD       = psp-gcc
AR       = psp-ar
RANLIB   = psp-ranlib
STRIP    = psp-strip
MKSFO    = mksfo
PACK_PBP = pack-pbp
FIXUP    = psp-fixup-imports

# Add in PSPSDK includes and libraries.
INCDIR   := $(INCDIR) . $(PSPSDK)/include
LIBDIR   := $(LIBDIR) . $(PSPSDK)/lib

CFLAGS   := $(addprefix -I,$(INCDIR)) $(CFLAGS)
CXXFLAGS := $(CFLAGS) $(CXXFLAGS)
ASFLAGS  := $(CFLAGS) $(ASFLAGS)

ifeq ($(PSP_FW_VERSION),)
PSP_FW_VERSION=150
endif

CFLAGS += -D_PSP_FW_VERSION=$(PSP_FW_VERSION)

ifeq ($(BUILD_PRX),1)
LDFLAGS  := $(addprefix -L,$(LIBDIR)) -specs=$(PSPSDK)/lib/prxspecs -Wl,-q,-T$(PSPSDK)/lib/linkfile.prx $(LDFLAGS)
EXTRA_CLEAN += $(TARGET).elf
# Setup default exports if needed
ifdef PRX_EXPORTS
EXPORT_OBJ=$(patsubst %.exp,%.o,$(PRX_EXPORTS))
EXTRA_CLEAN += $(EXPORT_OBJ)
else 
EXPORT_OBJ=$(PSPSDK)/lib/prxexports.o
endif
else
LDFLAGS  := $(addprefix -L,$(LIBDIR)) $(LDFLAGS)
endif


#PSPSDK_LIB = -lpsplibc -lstd -lpspgum -lpspgu -lm -lpspdebug -lpspdisplay -lpspge -lpspctrl -lpspsdk -lc -lpspnet -lpspnet_inet -lpspnet_apctl -lpspnet_resolver -lpsputility -lpspuser #-lpspkernel



# Link with following default libraries.  Other libraries should be specified in the $(LIBS) variable.
# TODO: This library list needs to be generated at configure time.
#
PSPSDK_LIBS = -lpspdebug -lpspdisplay_driver -lpspctrl_driver -lpspsdk -lbulletcollision -lbulletdynamics -lbulletmath -lbulletsoftbody
LIBS     := $(LIBS) $(PSPSDK_LIBS) $(PSPSDK_LIBC_LIB) -lpspkernel

BUILDCMD=$(CC) $(CFLAGS) $(CXXFLAGS) -c -o 	

BULLET_PREFIX=build/include/

CUBICVR_OBJS = Resource.o Gamepad.o Envelope.o Face.o IFFReader.o lodepng.o Matrix.o Motion.o RGB.o RGBA.o Timer.o Transform.o Vector.o XYZ.o XYZRGB.o SectorMap.o BITSET.o MATRIX4X4.o PLANE.o VECTOR3D.o systemfont.o tinyxml.o tinystr.o tinyxmlerror.o tinyxmlparser.o Resource.o ResourceManager.o ResourcePool.o Logger.o StringUtil.o 

# TGALoader.o Font.o Bitmap.o VECTOR2D.o VECTOR4D.o  RigidSceneDebug.o BSPSceneObject.o BSP.o

CUBICVR_GL_OBJS = Camera.o  GLExt.o  Object.o objectLWO.o UV.o UVMapper.o SceneObject.o ObjectController.o Scene.o sceneLWS.o Shader.o CollisionMap.o Texture.o Light.o Material.o \
 Bone.o DataTree.o FRUSTUM.o sceneLWS.o SceneObject.o TGALoader.o 

 #  Pencil.o

CUBICVR_DYNAMICS_OBJS = RigidScene.o RigidScene.o RigidSceneObject.o Landscape.o Vehicle.o RigidBox.o RigidSphere.o SoftScene.o SoftSceneObject.o Particle.o RigidCapsule.o

#   Particle.o ParticleEmitter.o ParticleSystem.o systemfont.o   CollisionMap.o Bone.o 

# not used: Bitmap.o Font.o

all: bullet cubicvr
	@mkdir build/lib || true
	@mkdir build/include || true
	@mkdir build/include/CubicVR || true
	cp -R ../cubicvr/include/* build/include/
	cp libcubicvr.a build/lib/

bullet:
	cd bullet && make

Bitmap.o: ../cubicvr/source/Bitmap.cpp
	$(BUILDCMD) Bitmap.o $^
Camera.o: ../cubicvr/source/Camera.cpp
	$(BUILDCMD) Camera.o $^ 
CollisionMap.o: ../cubicvr/source/CollisionMap.cpp
	$(BUILDCMD) CollisionMap.o $^
Envelope.o: ../cubicvr/source/Envelope.cpp
	$(BUILDCMD) Envelope.o $^
Face.o: ../cubicvr/source/Face.cpp
	$(BUILDCMD) Face.o $^
Font.o: ../cubicvr/source/Font.cpp
	$(BUILDCMD) Font.o $^
Gamepad.o: ../cubicvr/source/Gamepad.cpp
	$(BUILDCMD) Gamepad.o $^
GLExt.o: ../cubicvr/source/GLExt.cpp
	$(BUILDCMD) GLExt.o $^
IFFReader.o: ../cubicvr/source/IFFReader.cpp
	$(BUILDCMD) IFFReader.o $^
Light.o: ../cubicvr/source/Light.cpp
	$(BUILDCMD) Light.o $^
Logger.o: ../cubicvr/source/Logger.cpp
	$(BUILDCMD) Logger.o $^
lodepng.o: ../cubicvr/source/lodepng.cpp
	$(BUILDCMD) lodepng.o $^
Material.o: ../cubicvr/source/Material.cpp
	$(BUILDCMD) Material.o $^
Matrix.o: ../cubicvr/source/Matrix.cpp
	$(BUILDCMD) Matrix.o $^
Motion.o: ../cubicvr/source/Motion.cpp
	$(BUILDCMD) Motion.o $^
Object.o: ../cubicvr/source/Object.cpp
	$(BUILDCMD) Object.o $^
ObjectController.o: ../cubicvr/source/ObjectController.cpp
	$(BUILDCMD) ObjectController.o $^
objectLWO.o: ../cubicvr/source/objectLWO.cpp
	$(BUILDCMD) objectLWO.o $^
Particle.o: ../cubicvr/source/Particle.cpp
	$(BUILDCMD) Particle.o $^
ParticleEmitter.o: ../cubicvr/source/ParticleEmitter.cpp
	$(BUILDCMD) ParticleEmitter.o $^
ParticleSystem.o: ../cubicvr/source/ParticleSystem.cpp
	$(BUILDCMD) ParticleSystem.o $^
Pencil.o: ../cubicvr/source/Pencil.cpp
	$(BUILDCMD) Pencil.o $^
Resource.o: ../cubicvr/source/Resource.cpp
	$(BUILDCMD) Resource.o $^
ResourceManager.o: ../cubicvr/source/ResourceManager.cpp
	$(BUILDCMD) ResourceManager.o $^
ResourcePool.o: ../cubicvr/source/ResourcePool.cpp
	$(BUILDCMD) ResourcePool.o $^
RGB.o: ../cubicvr/source/RGB.cpp
	$(BUILDCMD) RGB.o $^
RGBA.o: ../cubicvr/source/RGBA.cpp
	$(BUILDCMD) RGBA.o $^
Scene.o: ../cubicvr/source/Scene.cpp
	$(BUILDCMD) Scene.o $^
sceneLWS.o: ../cubicvr/source/sceneLWS.cpp
	$(BUILDCMD) sceneLWS.o $^
SceneObject.o: ../cubicvr/source/SceneObject.cpp
	$(BUILDCMD) SceneObject.o $^
Shader.o: ../cubicvr/source/Shader.cpp
	$(BUILDCMD) Shader.o $^
StringUtil.o: ../cubicvr/source/StringUtil.cpp
	$(BUILDCMD) StringUtil.o $^
systemfont.o: ../cubicvr/source/systemfont.cpp
	$(BUILDCMD) systemfont.o $^
Texture.o: ../cubicvr/source/Texture.cpp
	$(BUILDCMD) Texture.o $^
Timer.o: ../cubicvr/source/Timer.cpp
	$(BUILDCMD) Timer.o $^
Transform.o: ../cubicvr/source/Transform.cpp
	$(BUILDCMD) Transform.o $^
UV.o: ../cubicvr/source/UV.cpp
	$(BUILDCMD) UV.o $^
UVMapper.o: ../cubicvr/source/UVMapper.cpp
	$(BUILDCMD) UVMapper.o $^
Vector.o: ../cubicvr/source/Vector.cpp
	$(BUILDCMD) Vector.o $^
XYZ.o: ../cubicvr/source/XYZ.cpp
	$(BUILDCMD) XYZ.o $^
XYZRGB.o: ../cubicvr/source/XYZRGB.cpp
	$(BUILDCMD) XYZRGB.o $^
TGALoader.o: ../cubicvr/source/psp/TGALoader.cpp
	$(BUILDCMD) TGALoader.o $^
SectorMap.o: ../cubicvr/source/SectorMap.cpp
	$(BUILDCMD) SectorMap.o $^



#new

Bitmap.o: ../cubicvr/source/Bitmap.cpp
	$(BUILDCMD) Bitmap.o $^

BITSET.o: ../cubicvr/source/BITSET.cpp
	$(BUILDCMD) BITSET.o $^


Bone.o: ../cubicvr/source/Bone.cpp
	$(BUILDCMD) Bone.o $^


BSP.o: ../cubicvr/source/BSP.cpp
	$(BUILDCMD) BSP.o $^


DataTree.o: ../cubicvr/source/datatree/DataTree.cpp
	$(BUILDCMD) DataTree.o $^


FRUSTUM.o: ../cubicvr/source/FRUSTUM.cpp
	$(BUILDCMD) FRUSTUM.o $^

IMAGE.o: ../cubicvr/source/IMAGE.cpp
	$(BUILDCMD) IMAGE.o $^


MATRIX4X4.o: ../cubicvr/source/math/MATRIX4X4.cpp
	$(BUILDCMD) MATRIX4X4.o $^

PLANE.o: ../cubicvr/source/math/PLANE.cpp
	$(BUILDCMD) PLANE.o $^

#VECTOR2D.o: ../cubicvr/source/math/VECTOR2D.cpp
#	$(BUILDCMD) VECTOR2D.o $^

VECTOR3D.o: ../cubicvr/source/math/VECTOR3D.cpp
	$(BUILDCMD) VECTOR3D.o $^

#VECTOR4D.o: ../cubicvr/source/math/VECTOR4D.cpp
#	$(BUILDCMD) VECTOR4D.o $^

#RigidSceneDebug.o: ../cubicvr/source/RigidSceneDebug.cpp
#	$(BUILDCMD) RigidSceneDebug.o $^

textureTGA.o: ../cubicvr/source/textureTGA.cpp
	$(BUILDCMD) textureTGA.o $^







# dynamics
RigidScene.o: ../cubicvr/source/RigidScene.cpp
	$(BUILDCMD) RigidScene.o $^
RigidSceneObject.o: ../cubicvr/source/SceneObjects/RigidSceneObject.cpp
	$(BUILDCMD) RigidSceneObject.o $^
Landscape.o: ../cubicvr/source/SceneObjects/Landscape.cpp
	$(BUILDCMD) Landscape.o $^
Vehicle.o: ../cubicvr/source/SceneObjects/Vehicle.cpp
	$(BUILDCMD) Vehicle.o $^
RigidBox.o: ../cubicvr/source/SceneObjects/RigidBox.cpp
	$(BUILDCMD) RigidBox.o $^
RigidSphere.o: ../cubicvr/source/SceneObjects/RigidSphere.cpp
	$(BUILDCMD) RigidSphere.o $^
SoftSceneObject.o: ../cubicvr/source/SceneObjects/SoftSceneObject.cpp
	$(BUILDCMD) SoftSceneObject.o $^
SoftScene.o: ../cubicvr/source/SoftScene.cpp
	$(BUILDCMD) SoftScene.o $^


RigidCapsule.o: ../cubicvr/source/SceneObjects/RigidCapsule.cpp
	$(BUILDCMD) RigidCapsule.o $^

BSPSceneObject.o: ../cubicvr/source/SceneObjects/BSPSceneObject.cpp
	$(BUILDCMD) BSPSceneObject.o $^

BSPSceneObject.o: ../cubicvr/source/SceneObjects/BSPSceneObject.cpp
	$(BUILDCMD) BSPSceneObject.o $^


tinyxml.o: ../cubicvr/source/tinyxml/tinyxml.cpp
	$(BUILDCMD) tinyxml.o $^

tinystr.o: ../cubicvr/source/tinyxml/tinystr.cpp
	$(BUILDCMD) tinystr.o $^

tinyxmlerror.o: ../cubicvr/source/tinyxml/tinyxmlerror.cpp
	$(BUILDCMD) tinyxmlerror.o $^

tinyxmlparser.o: ../cubicvr/source/tinyxml/tinyxmlparser.cpp
	$(BUILDCMD) tinyxmlparser.o $^


Resource.o: ../cubicvr/source/Resource.cpp
	$(BUILDCMD) Resource.o $^

textureTGA.o: ../cubicvr/src/textureTGA.cpp
	$(CVRBUILD) textureTGA.o $^




	
cubicvr: libcubicvr.a

libcubicvr.a: $(CUBICVR_OBJS) $(CUBICVR_GL_OBJS) $(CUBICVR_DYNAMICS_OBJS)
	$(AR) cru $@ $^ 
	$(RANLIB) $@

install: libcubicvr.a
	cp libcubicvr.a $(PSPDEV)/psp/lib/
	test -d $(PSPDEV)/psp/include/CubicVR || mkdir $(PSPDEV)/psp/include/CubicVR/
	cp -R ../cubicvr/include/CubicVR/* $(PSPDEV)/psp/include/CubicVR/ || true
	cd pspbullet && make install
	cp lua/include/* $(PSPDEV)/psp/include/
	cp lua/lib/* $(PSPDEV)/psp/lib/

clean: 
	rm *.o || true
	rm libcubicvr.a || true

rebuild: clean all
