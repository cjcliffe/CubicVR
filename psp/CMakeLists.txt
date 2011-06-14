# Project name
cmake_minimum_required (VERSION 2.6) 
project (cubicvr)

add_definitions(-DARCH_PSP -O3)

include_directories (
	../cubicvr/include
	../cubicvr/external/bullet-2.78/src
	${PSPSDK_PATH}/include)

set (CUBICVR_LIST
    ../cubicvr/source/Resource.cpp
    ../cubicvr/source/Gamepad.cpp
    ../cubicvr/source/Envelope.cpp
    ../cubicvr/source/Face.cpp
    ../cubicvr/source/IFFReader.cpp
    ../cubicvr/source/lodepng.cpp
    ../cubicvr/source/Matrix.cpp
    ../cubicvr/source/Motion.cpp
    ../cubicvr/source/RGB.cpp
    ../cubicvr/source/RGBA.cpp
    ../cubicvr/source/Timer.cpp
    ../cubicvr/source/Transform.cpp
    ../cubicvr/source/Vector.cpp
    ../cubicvr/source/XYZ.cpp
    ../cubicvr/source/XYZRGB.cpp
    ../cubicvr/source/SectorMap.cpp
    ../cubicvr/source/BITSET.cpp
    ../cubicvr/source/math/MATRIX4X4.cpp
    ../cubicvr/source/math/PLANE.cpp
    ../cubicvr/source/math/VECTOR3D.cpp
    ../cubicvr/source/systemfont.cpp
    ../cubicvr/source/tinyxml/tinyxml.cpp
    ../cubicvr/source/tinyxml/tinystr.cpp
    ../cubicvr/source/tinyxml/tinyxmlerror.cpp
    ../cubicvr/source/tinyxml/tinyxmlparser.cpp
    ../cubicvr/source/ResourceManager.cpp
    ../cubicvr/source/ResourcePool.cpp
    ../cubicvr/source/Logger.cpp
    ../cubicvr/source/StringUtil.cpp)

set (CUBICVR_GL_LIST
    ../cubicvr/source/Camera.cpp
    ../cubicvr/source/GLExt.cpp
    ../cubicvr/source/Object.cpp
    ../cubicvr/source/objectLWO.cpp
    ../cubicvr/source/UV.cpp
    ../cubicvr/source/UVMapper.cpp
    ../cubicvr/source/SceneObject.cpp
    ../cubicvr/source/ObjectController.cpp
    ../cubicvr/source/Scene.cpp
    ../cubicvr/source/sceneLWS.cpp
    ../cubicvr/source/Shader.cpp
    ../cubicvr/source/CollisionMap.cpp
    ../cubicvr/source/Texture.cpp
    ../cubicvr/source/Light.cpp
    ../cubicvr/source/Material.cpp
    ../cubicvr/source/Bone.cpp
    ../cubicvr/source/datatree/DataTree.cpp
    ../cubicvr/source/FRUSTUM.cpp
    ../cubicvr/source/sceneLWS.cpp
    ../cubicvr/source/SceneObject.cpp
    ../cubicvr/source/psp/TGALoader.cpp)

set (CUBICVR_DYNAMICOBJECTS_LIST
    ../cubicvr/source/RigidScene.cpp
    ../cubicvr/source/RigidScene.cpp
    ../cubicvr/source/RigidSceneObject.cpp
    ../cubicvr/source/Landscape.cpp
    ../cubicvr/source/Vehicle.cpp
    ../cubicvr/source/RigidBox.cpp
    ../cubicvr/source/RigidSphere.cpp
    ../cubicvr/source/SoftScene.cpp
    ../cubicvr/source/SoftSceneObject.cpp
    ../cubicvr/source/Particle.cpp
    ../cubicvr/source/RigidCapsule.cpp)

add_library(cubicvr
	${CUBICVR_LIST}
	${CUBICVR_GL_LIST}
	${CUBICVR_DYNAMICS_LIST})

