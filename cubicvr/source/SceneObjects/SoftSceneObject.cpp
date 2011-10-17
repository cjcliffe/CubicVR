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


#include <CubicVR/SceneObjects/SoftSceneObject.h>



SoftSceneObject::SoftSceneObject(): bind_obj(NULL)
{
	
}


SoftSceneObject::SoftSceneObject(Mesh &obj_in): bind_obj(NULL)
{
	bind(obj_in);
}

void SoftSceneObject::bind(Mesh &obj_in)
{
	obj = &soft_obj;
	cmap_obj = &soft_obj;
	bind_obj = &obj_in;
};


void SoftSceneObject::initialize(btSoftRigidDynamicsWorld &dynamicsWorld_in, btSoftBodyWorldInfo &softBodyWorldInfo_in)
{
	softBodyWorldInfo = &softBodyWorldInfo_in;	
	
	dynamicsWorld = &dynamicsWorld_in;

	startTransform.setIdentity();
	
	btVector3 localInertia(0,0,0);
	
	btQuaternion rot;
	
	XYZ rot_src = getRotation();
	
	rot.setEuler(DEGTORAD(rot_src.y),DEGTORAD(rot_src.x),DEGTORAD(rot_src.z));
	
	startTransform.setRotation(rot);
	
	if (!rigid_parent) 
	{
		startTransform.setOrigin(getPosition().cast());
	}
	else
	{
		startTransform.setOrigin(getPosition().cast());
		startTransform = rigid_parent->getRigidBody().getWorldTransform() * startTransform;
	}

	mSoftBody = NULL;
}


void SoftSceneObject::evaluate()
{
	unsigned int i;
	
	if (!obj) return;
	
	obj->openBuffer(true);


	for(int i=0;i<mSoftBody->m_nodes.size();++i)
	{
		const btSoftBody::Node&	n=mSoftBody->m_nodes[i];
		
		*obj->points[i] = XYZ(n.m_x.getX(),n.m_x.getY(),n.m_x.getZ());
	}
		
//	for(i=0;i<mSoftBody->m_faces.size();++i)
//	{
//		const btSoftBody::Face&	f=mSoftBody->m_faces[i];
//
//		obj->faces[i]->face_normal = XYZ(f.m_normal.getX(),f.m_normal.getY(),f.m_normal.getZ());
//	}	


//		glColor3f(0,0,1);
//		glBegin(GL_LINE_STRIP);
//			glVertex3f(f.m_n[0]->m_x.getX(),f.m_n[0]->m_x.getY(),f.m_n[0]->m_x.getZ());
//			glVertex3f(f.m_n[1]->m_x.getX(),f.m_n[1]->m_x.getY(),f.m_n[1]->m_x.getZ());
//			glVertex3f(f.m_n[2]->m_x.getX(),f.m_n[2]->m_x.getY(),f.m_n[2]->m_x.getZ());
//			glVertex3f(f.m_n[0]->m_x.getX(),f.m_n[0]->m_x.getY(),f.m_n[0]->m_x.getZ());
//		glEnd();


#ifdef ARCH_PSP
	obj->calcNormals(); 
#else
	obj->reCalcNormals(); 
#endif
	
	std::vector<int> subCount;

	subCount.resize(obj->points.size());

	for (i = 0; i < obj->faces.size(); i++)
	{
		obj->updatePointNormalCache(obj->faces[i]->pointref[0],subCount[obj->faces[i]->pointref[0]]++,obj->faces[i]->point_normals[0]);
		obj->updatePointNormalCache(obj->faces[i]->pointref[1],subCount[obj->faces[i]->pointref[1]]++,obj->faces[i]->point_normals[1]);
		obj->updatePointNormalCache(obj->faces[i]->pointref[2],subCount[obj->faces[i]->pointref[2]]++,obj->faces[i]->point_normals[2]);
	}
	
	for (i = 0; i < obj->points.size(); i++)
	{
		obj->updatePointCache(i);
	}
	
	obj->closeBuffer(true);

/*
	if (mSoftBody && mSoftBody->getMotionState())
	{
		btTransform objTrans;
		
		btDefaultMotionState* myMotionState = (btDefaultMotionState*)mRigidBody->getMotionState();

//		float m[16];
		
		myMotionState->getWorldTransform(objTrans);
//		objTrans.getOpenGLMatrix(m);
//		controller().setMatrixLock(true);
//		controller().setMatrix(m);
//		controller().position = XYZ(objTrans.getOrigin().getX(),objTrans.getOrigin().getY(),objTrans.getOrigin().getZ());


		XYZ a = controller().position;
		XYZ newpos = XYZ(objTrans.getOrigin().getX(),objTrans.getOrigin().getY(),objTrans.getOrigin().getZ());
		a -= newpos;
		
		XYZ newrot; 
		
		QuaternionToEulerXYZ(objTrans.getRotation(),newrot);
		
		XYZ b = controller().rotation;
		b -= newrot;
	
		// attempt to prevent unnecessary tiny jitters from updating position/rotation to smooth out movements and the gl matrix cache
		double trans_jitter_threshold = 0.0025;
		double rot_jitter_threshold = 0.02;
	
		if ((fabs(a.x) > trans_jitter_threshold || fabs(a.y) > trans_jitter_threshold || fabs(a.z) > trans_jitter_threshold)||
			(fabs(b.x) > rot_jitter_threshold || fabs(b.y) > rot_jitter_threshold || fabs(b.z) > rot_jitter_threshold))
		{
			controller().position = newpos;
			controller().rotation = newrot;
			
//				printf("jitter = %f,%f,%f  r: %f,%f,%f \n",a.x,a.y,a.z,b.x,b.y,b.z);
		}				
//			printf("rot = %f,%f,%f\n",controller().rotation.x,controller().rotation.y,controller().rotation.z);
	}


	std::vector<RigidSceneObject *>::iterator children_i;
	
	for (children_i = rigid_children.begin(); children_i != rigid_children.end(); children_i++)
	{
		(*children_i)->evaluate();
	}
	
	*/
	
}


#ifdef OPENGL_ES
//	#define SOFTBODY_CLUSTERS 1
//	#define SOFTBODY_CLUSTERDEFAULT 128
#else
	#define SOFTBODY_CLUSTERS 1
	#define SOFTBODY_CLUSTERDEFAULT 128
#endif


btSoftBody &SoftSceneObject::createSoftBody()
{
	btVector3 localInertia(0,0,0);

	/*if (!cmap && !colShape && (cmap_obj || obj) && !mRigidBody)
	{
		cmap = new CollisionMap();
		cmap->addMesh(cmap_obj?*cmap_obj:*obj);
	}*/


/*	if (!cmap_obj) 
	{
		cmap_obj = obj;
#ifndef ARCH_PSP
		cmap_obj->triangulate();
#endif
	}
*/
	if (bind_obj)
	{
		obj->cloneObject(*bind_obj);
	}

#ifndef ARCH_PSP
#ifndef OPENGL_ES
#ifndef ARCH_DC
	obj->triangulate();
#endif
#endif
#endif
	
	obj->cache(true,true);

	
	int numFaces = obj->faces.size();
	int numVerts = obj->points.size();
	
	indicies = new int[numFaces*3];
	verticies = new float[numVerts*3];

	int i,j;

	j = 0;
	
	for (i = 0; i < numVerts*3; i+=3)
	{
		verticies[i] = obj->points[j]->x;
		verticies[i+1] = obj->points[j]->y;
		verticies[i+2] = obj->points[j]->z;

		j++;
	}

	j = 0;
	for (i = 0; i < numFaces*3; i+=3)
	{
		indicies[i] = obj->faces[j]->pointref[0];
		indicies[i+1] = obj->faces[j]->pointref[1];
		indicies[i+2] = obj->faces[j]->pointref[2];
		j++;
	}
	
	mSoftBody = btSoftBodyHelpers::CreateFromTriMesh(*softBodyWorldInfo,	
													verticies,
													indicies,
													numFaces);

#ifdef SOFTBODY_CLUSTERS
	// clusters
	btSoftBody::Material*	pm=mSoftBody->appendMaterial();
	pm->m_kLST				=	0.1f;
//	pm->m_flags				-=	btSoftBody::fMaterial::DebugDraw;			
	mSoftBody->generateBendingConstraints(2,pm);
#endif
	
	
													
/*	mSoftBody = btSoftBodyHelpers::CreateFromTriMesh(*softBodyWorldInfo,	
														gVerticesBunny,
														&gIndicesBunny[0][0],
														BUNNY_NUM_TRIANGLES);																					
*/
													
	mSoftBody->m_cfg.kDF				=	0.5f;
	mSoftBody->m_materials[0]->m_kLST	=	0.1f;
	mSoftBody->m_cfg.kMT				=	0.05f;	
#ifdef OPENGL_ES
	mSoftBody->m_cfg.piterations	=	2;
#else
	mSoftBody->m_cfg.piterations	=	2;
#endif
	
#ifdef SOFTBODY_CLUSTERS
	mSoftBody->m_cfg.collisions	= btSoftBody::fCollision::CL_SS + btSoftBody::fCollision::CL_RS;
#else
	mSoftBody->m_cfg.collisions	|=	btSoftBody::fCollision::VF_SS;	
#endif
	
	
//	mSoftBody->scale(controller().scale.cast());
	mSoftBody->randomizeConstraints();

	btMatrix3x3	m;

	XYZ rot_src = getRotation();
	
	m.setEulerZYX(DEGTORAD(rot_src.x),DEGTORAD(rot_src.y),DEGTORAD(rot_src.z));
	mSoftBody->transform(btTransform(m,getPosition().cast()));
	mSoftBody->scale(btVector3(getScale().cast()));
	
	setScale(XYZ(1,1,1));
	setPosition(XYZ(0,0,0));
	setRotation(XYZ(0,0,0));

	mSoftBody->setTotalMass(mass,true);

	
	mSoftBody->setPose(true,true);

	
#ifdef SOFTBODY_CLUSTERS
	mSoftBody->generateClusters(SOFTBODY_CLUSTERDEFAULT);
#else
	
#endif


	return *mSoftBody;
/*
	if (cmap && !colShape && !mRigidBody)
	{
		//softbody is dynamic if and only if mass is non zero, otherwise static
		if (mass == 0.0)
		{
			colShape = cmap->makeStaticCollisionShape();
		}
		else
		{
			colShape = cmap->makeCollisionShape();
		}
	}


	if (mass != 0.0)
	{
		colShape->calculateLocalInertia(mass,localInertia);
	}
	
	if (!mRigidBody)
	{
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

		btSoftBody::btSoftBodyConstructionInfo cInfo(mass,myMotionState,colShape,localInertia);

		mSoftBody = new btSoftBody(cInfo);
	}
	
	return *mSoftBody;
*/
}



btSoftBody &SoftSceneObject::getSoftBody()
{
	if (!mSoftBody) createSoftBody();
	return *mSoftBody;
}


void SoftSceneObject::setSoftBody(btSoftBody &mSoftBody_in)
{
//		if (mSoftBody)
//		{
//			//has existing, cleanup?
//		}
	mSoftBody = &mSoftBody_in;
}



void SoftSceneObject::render(Renderer &renderer, int stage, bool initShadowMatrix, bool no_pivot, bool no_scale)
{	
//	int i,j,numFaces;
//	glColor3f(0,0,1);
//	glBegin(GL_TRIANGLES);
//		j = 0;
//		for (i = 0; i < numFaces*3; i+=3)
//		{
//			glVertex3f(verticies[(indicies[i]*3)],verticies[(indicies[i]*3)+1],verticies[(indicies[i]*3)+2]);
//			glVertex3f(verticies[(indicies[i+1]*3)],verticies[(indicies[i+1]*3)+1],verticies[(indicies[i+1]*3)+2]);
//			glVertex3f(verticies[(indicies[i+2]*3)],verticies[(indicies[i+2]*3)+1],verticies[(indicies[i+2]*3)+2]);
//		}		
//	glEnd();

	
	SceneObject::render(renderer,stage,initShadowMatrix,true,true);

/*
	std::vector<SoftSceneObject *>::iterator children_i;

	SceneObject::render(renderer,stage,true);

	for (children_i = soft_children.begin(); children_i != soft_children.end(); children_i++)
	{
		(*children_i)->render(renderer,stage);
	}
*/
};