/*
 This file is part of CubicVR.
 
	Copyright (C) 2003 by Charles J. Cliffe

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
*/

#ifndef CUBICVR_COLLISIONMAP
#define CUBICVR_COLLISIONMAP

#include <CubicVR/Object.h>
#include <CubicVR/Transform.h>
#include <CubicVR/Shader.h>
//#include <CubicVR/SceneObject.h>
#include <vector>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btOptimizedBvh.h>

#define CMAP_SPHERE_NOMINAL 0.3f


struct collision_sphere
{
	XYZ center;
	cvrFloat radius;
	unsigned long face;
	std::vector<collision_sphere *> parents;
	std::vector<collision_sphere *> children;	
};


struct collision_box
{
	cvrFloat xmin, xmax, ymin, ymax, zmin, zmax;
};

typedef struct std::map< Face *, std::vector< Face * >, ltulong > CollisionList;

class CollisionMap
{
public:
	btCollisionShape *mTriMeshShape;
	
	std::vector<Object *> meshes;
	std::vector<std::vector<collision_sphere *>* > spheres;
	std::vector<collision_box *> bboxes;
	std::map< cvrIndex, std::map< cvrIndex, cvrIndex, ltindex >, ltindex > sphere_map;
	Transform *trans;
#ifndef ARCH_PSP
#ifndef OPENGL_ES
	LineShader cmapShader;
#endif
#endif
	unsigned int activeSphereSet;
	float margin;
	
	CollisionMap();
	
	void compileSphereMap(float nominal_rad = CMAP_SPHERE_NOMINAL);
	void bindTransformation(Transform *trans_in);
#ifndef ARCH_PSP
	void render();
#endif

	void addMesh(Object &mesh_obj);
	void addBoundingBox(cvrFloat xmin, cvrFloat xmax, cvrFloat ymin, cvrFloat ymax, cvrFloat zmin, cvrFloat zmax);
	void addSphere(XYZ &center, cvrFloat radius, cvrIndex faceNum = -1);
	void processSpheres(float nominal_rad = CMAP_SPHERE_NOMINAL);
	void setMargin(float margin_in);
	float getMargin();
	
	static void sphereTest(CollisionMap &a, CollisionMap &b, CollisionList &alist, CollisionList &blist);
	
	void getBoundingBox(XYZ &aabbMin, XYZ &aabbMax);
//	btCollisionShape *makeCollisionShape(const XYZ &scale=XYZ(1,1,1), vector<int> *arrayParam=NULL, XYZ *spacing=NULL);
	btCollisionShape *makeCollisionShape(float mass, const XYZ &scale=XYZ(1,1,1), vector<int> *arrayParam=NULL, XYZ *spacing=NULL);
};


#endif
