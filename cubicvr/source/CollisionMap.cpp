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

#include "CubicVR/cvr_defines.h"
#include "CubicVR/CollisionMap.h"
#include <CubicVR/Face.h>

CollisionMap::CollisionMap(): mTriMeshShape(NULL), margin(0.0f)
{
		
};


void CollisionMap::setMargin(float margin_in)
{
	margin = margin_in;	
}


float CollisionMap::getMargin()
{
	return margin;
}


#ifndef ARCH_PSP
// collision visualizer
void CollisionMap::render()
{
#ifndef OPENGL_ES
	unsigned int i,t;
	float j;
	
//	for(i = 0; i < meshes.size(); i++)
//	{
//		cmapShader.render(*meshes[i]);
//	}
#ifndef ARCH_DC
	GLShader::clear();
#endif
	glDisable(GL_LIGHTING);
	glColor3f(0.0f,1.0f,0.0f);
	int sSet = activeSphereSet;


	// bounding box
//	if (bboxes.size()) for (i = 0; i < bboxes.size(); i++)
//	{
//		glBegin(GL_LINES);
//		
//		glColor3f(0.0,0.0,1.0);
//		
//		glVertex3f((*bboxes[i]).xmin,(*bboxes[i]).ymin,(*bboxes[i]).zmin);
//		glVertex3f((*bboxes[i]).xmax,(*bboxes[i]).ymin,(*bboxes[i]).zmin);
//
//		glVertex3f((*bboxes[i]).xmin,(*bboxes[i]).ymax,(*bboxes[i]).zmin);
//		glVertex3f((*bboxes[i]).xmax,(*bboxes[i]).ymax,(*bboxes[i]).zmin);
//
//		glVertex3f((*bboxes[i]).xmin,(*bboxes[i]).ymin,(*bboxes[i]).zmin);
//		glVertex3f((*bboxes[i]).xmin,(*bboxes[i]).ymax,(*bboxes[i]).zmin);
//
//		glVertex3f((*bboxes[i]).xmax,(*bboxes[i]).ymin,(*bboxes[i]).zmin);
//		glVertex3f((*bboxes[i]).xmax,(*bboxes[i]).ymax,(*bboxes[i]).zmin);
//
//
//		glVertex3f((*bboxes[i]).xmin,(*bboxes[i]).ymin,(*bboxes[i]).zmax);
//		glVertex3f((*bboxes[i]).xmax,(*bboxes[i]).ymin,(*bboxes[i]).zmax);
//
//		glVertex3f((*bboxes[i]).xmin,(*bboxes[i]).ymax,(*bboxes[i]).zmax);
//		glVertex3f((*bboxes[i]).xmax,(*bboxes[i]).ymax,(*bboxes[i]).zmax);
//
//		glVertex3f((*bboxes[i]).xmin,(*bboxes[i]).ymin,(*bboxes[i]).zmax);
//		glVertex3f((*bboxes[i]).xmin,(*bboxes[i]).ymax,(*bboxes[i]).zmax);
//
//		glVertex3f((*bboxes[i]).xmax,(*bboxes[i]).ymin,(*bboxes[i]).zmax);
//		glVertex3f((*bboxes[i]).xmax,(*bboxes[i]).ymax,(*bboxes[i]).zmax);
//
//
//		glVertex3f((*bboxes[i]).xmax,(*bboxes[i]).ymax,(*bboxes[i]).zmin);
//		glVertex3f((*bboxes[i]).xmax,(*bboxes[i]).ymax,(*bboxes[i]).zmax);
//
//		glVertex3f((*bboxes[i]).xmax,(*bboxes[i]).ymin,(*bboxes[i]).zmin);
//		glVertex3f((*bboxes[i]).xmax,(*bboxes[i]).ymin,(*bboxes[i]).zmax);
//
//		glVertex3f((*bboxes[i]).xmin,(*bboxes[i]).ymax,(*bboxes[i]).zmin);
//		glVertex3f((*bboxes[i]).xmin,(*bboxes[i]).ymax,(*bboxes[i]).zmax);
//
//		glVertex3f((*bboxes[i]).xmin,(*bboxes[i]).ymin,(*bboxes[i]).zmin);
//		glVertex3f((*bboxes[i]).xmin,(*bboxes[i]).ymin,(*bboxes[i]).zmax);
//
//		glEnd();
//	}

//	if (spheres.size()) for (i = 0; i < spheres[0]->size(); i++)
//	{
//		collision_sphere *tmp = (*spheres[0])[i];
//	
//		if (tmp->face >= 0)
//		{
//			glBegin(GL_LINES);
//			
//				XYZ pt;
//				
//				pt = tmp->center;
//				glVertex3f(pt.x,pt.y,pt.z);
//				pt += meshes[0]->faces[tmp->face]->face_normal;
//				glVertex3f(pt.x,pt.y,pt.z);
//			
//			glEnd();
//		}
//	}
	
	
//	glColor3f(0.0,0.6,0.0);
//
//	if (spheres.size()) for (i = 0; i < spheres[sSet]->size(); i++)
//	{
//		collision_sphere *tmp = (*spheres[sSet])[i];
//	
//		if (tmp->face >= 0)
//		{
//			glBegin(GL_LINES);
//			
//				XYZ pt;
//				Vector v;
//				
//				pt = (*spheres[0])[tmp->face]->center;
//				glVertex3f(pt.x,pt.y,pt.z);
//				
//				pt = tmp->center;
//				glVertex3f(pt.x,pt.y,pt.z);
//			
//			glEnd();
//		}
//	}
	
	
	// 1 dot per sphere
//	glColor3f(0.0,1.0,0.0);
//	glDepthFunc(GL_LEQUAL);
//	glPointSize(2.0);
//	glEnable(GL_POINT_SMOOTH);
//
//	if (spheres.size()) for (i = 0; i < spheres[sSet]->size(); i++)
//	{
//		glBegin(GL_POINTS);
//		
//			collision_sphere *tmp = (*spheres[sSet])[i];
//			glVertex3f(tmp->center.x,tmp->center.y,tmp->center.z);
//		
//		glEnd();
//	}


//  axis points with radius
//	glColor3f(0.0,1.0,0.0);
//
//	if (spheres.size()) for (i = 0; i < spheres[sSet]->size(); i++)
//	{
//		glBegin(GL_LINES);
//		
//			collision_sphere *tmp = (*spheres[sSet])[i];
//		
//			glVertex3f(tmp->center.x+tmp->radius,tmp->center.y,tmp->center.z);
//			glVertex3f(tmp->center.x-tmp->radius,tmp->center.y,tmp->center.z);
//
//			glVertex3f(tmp->center.x,tmp->center.y+tmp->radius,tmp->center.z);
//			glVertex3f(tmp->center.x,tmp->center.y-tmp->radius,tmp->center.z);
//
//			glVertex3f(tmp->center.x,tmp->center.y,tmp->center.z+tmp->radius);
//			glVertex3f(tmp->center.x,tmp->center.y,tmp->center.z-tmp->radius);
//		
//		glEnd();
//	}


	//		if ((*spheres[sSet])[i]->radius > CMAP_SPHERE_NOMINAL) continue;

/*		collision_sphere *tmp = (*spheres[sSet])[i];
		
		glPointSize(tmp->radius);
		glEnable(GL_POINT_SMOOTH);
		glBegin(GL_POINTS);
			glVertex3f(tmp->center.x,tmp->center.y,tmp->center.z);
		glEnd();*/


// sphereoid things		
	if (spheres.size()) for (i = 0; i < spheres[sSet]->size(); i++)
	{
		#define SP_RES 20.0f
		
		collision_sphere *tmp = (*spheres[sSet])[i];
		
		glColor4f(0.0f,0.0f,1.0f,0.5f);	
		glBegin(GL_LINE_STRIP);
			for (j = 0; j <= 2.0f*M_PI+SP_RES; j+= (M_PI*2.0f)/SP_RES)
			{
				glVertex3f(tmp->center.x+tmp->radius*sin(j),tmp->center.y,tmp->center.z+tmp->radius*cos(j));
			}
		glEnd();

		glColor4f(0.0f,1.0f,0.0f,0.5f);
		glBegin(GL_LINE_STRIP);
			t = 0;
			for (j = 0; j <= 2.0f*M_PI+SP_RES; j+= (M_PI*2.0f)/SP_RES)
			{
				glVertex3f(tmp->center.x,tmp->center.y+tmp->radius*sin(j),tmp->center.z+tmp->radius*cos(j));
			}
		glEnd();

		glColor4f(1.0f,0.0f,0.0f,0.5f);
		glBegin(GL_LINE_STRIP);
			for (j = 0; j <= 2.0f*M_PI+SP_RES; j+= (M_PI*2.0f)/SP_RES)
			{
				glVertex3f(tmp->center.x+tmp->radius*sin(j),tmp->center.y+tmp->radius*cos(j),tmp->center.z);
			}
		glEnd();
	}

#endif
}
#endif


/*
enum SceneObjectArrayParamShadow
{
	OBJ_ARRAY_RIGHT = 0,
	OBJ_ARRAY_UP,
	OBJ_ARRAY_BACK,
	OBJ_ARRAY_LEFT,
	OBJ_ARRAY_DOWN,
	OBJ_ARRAY_FRONT
};
*/

//btCollisionShape *CollisionMap::makeCollisionShape(const XYZ &scale, vector<int> *arrayParam, XYZ *spacing)
//{
//	cvrIndex i;
//	
//	if (!meshes.size()) return NULL;
//	
//	if (mTriMeshShape) return mTriMeshShape;
//	
//	Object *obj = meshes[0];
//
//	btTriangleMesh *mTriMesh = new btTriangleMesh();
//	
//	for (i = 0; i < obj->faces.size(); i++)
//	{
//		if (obj->faces[i]->pointref.size() != 3) continue;
//		
//		btVector3 v0(obj->points[obj->faces[i]->pointref[0]]->x*scale.x,obj->points[obj->faces[i]->pointref[0]]->y*scale.y,obj->points[obj->faces[i]->pointref[0]]->z*scale.z); 
//		btVector3 v1(obj->points[obj->faces[i]->pointref[1]]->x*scale.x,obj->points[obj->faces[i]->pointref[1]]->y*scale.y,obj->points[obj->faces[i]->pointref[1]]->z*scale.z); 
//		btVector3 v2(obj->points[obj->faces[i]->pointref[2]]->x*scale.x,obj->points[obj->faces[i]->pointref[2]]->y*scale.y,obj->points[obj->faces[i]->pointref[2]]->z*scale.z); 
//		
//		mTriMesh->addTriangle(v0,v1,v2); 
//	}
//	
//	 mTriMeshShape = new btConvexTriangleMeshShape(mTriMesh);
//	 
//	 return mTriMeshShape;
//}


btCollisionShape *CollisionMap::makeCollisionShape(float mass, const XYZ &scale, vector<int> *arrayParam, XYZ *spacing)
{
	cvrIndex i = 0;
	
	if (!meshes.size()) return NULL;
	
	if (mTriMeshShape) return mTriMeshShape;
	
	Mesh *obj = meshes[0];

	btTriangleMesh *mTriMesh = new btTriangleMesh();
	
	for (i = 0; i < obj->faces.size(); i++)
	{
		if (obj->faces[i]->pointref.size() != 3) continue;
		
		btVector3 v0(obj->points[obj->faces[i]->pointref[0]]->x*scale.x,obj->points[obj->faces[i]->pointref[0]]->y*scale.y,obj->points[obj->faces[i]->pointref[0]]->z*scale.z); 
		btVector3 v1(obj->points[obj->faces[i]->pointref[1]]->x*scale.x,obj->points[obj->faces[i]->pointref[1]]->y*scale.y,obj->points[obj->faces[i]->pointref[1]]->z*scale.z); 
		btVector3 v2(obj->points[obj->faces[i]->pointref[2]]->x*scale.x,obj->points[obj->faces[i]->pointref[2]]->y*scale.y,obj->points[obj->faces[i]->pointref[2]]->z*scale.z); 
		
		mTriMesh->addTriangle(v0,v1,v2); 
	}
	
	if (mass == 0.0)
	{
		mTriMeshShape = new btBvhTriangleMeshShape(mTriMesh,true);
	}
	else
	{
		mTriMeshShape = new btConvexTriangleMeshShape(mTriMesh);
	}
	 
	
	 if (arrayParam)
	 {
		 XYZ arraySpacing = spacing?(*spacing):XYZ(0,0,0);
		 
		 if (arrayParam->size())
		 {
//			 obj->calcBB();
//			 
//			 XYZ bbMin = obj->bbMin;
//			 XYZ bbMax = obj->bbMax;
			 
			 
			 
//			 XYZ startpos = XYZ(0,0,0);
//			 startpos -= XYZ(
//							 bbMin.x*(float)(*arrayParam)[OBJ_ARRAY_LEFT]-(arraySpacing.x*(float)(*arrayParam)[OBJ_ARRAY_LEFT]),
//							 bbMin.y*(float)(*arrayParam)[OBJ_ARRAY_DOWN]-(arraySpacing.y*(float)(*arrayParam)[OBJ_ARRAY_DOWN]),
//							 bbMin.z*(float)(*arrayParam)[OBJ_ARRAY_BACK]-(arraySpacing.z*(float)(*arrayParam)[OBJ_ARRAY_BACK]));
			 
			 btCompoundShape *arrayShape = new btCompoundShape();
			 
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
				glLoadIdentity();

//				 glTranslatef(startpos.x,startpos.y,startpos.z);

			 XYZ bb1,bb2,bb;
			 
			 obj->calcBB();
			 obj->getBoundingBox(bb1,bb2);

			 bb1.x*=scale.x;
			 bb1.y*=scale.y;
			 bb1.z*=scale.z;
			 bb2.x*=scale.x;
			 bb2.y*=scale.y;
			 bb2.z*=scale.z;
			 
			 bb = bb2-bb1;
			 
			 int end[3];
			 
			 end[0] = (*arrayParam)[0]+(*arrayParam)[3];
			 end[1] = (*arrayParam)[1]+(*arrayParam)[4];
			 end[2] = (*arrayParam)[2]+(*arrayParam)[5];
			 
			 
			 glTranslatef((-bb.x-arraySpacing.x*scale.x)*(float)(*arrayParam)[3],(-bb.y-arraySpacing.y*scale.y)*(float)(*arrayParam)[4],(-bb.z-arraySpacing.z*scale.z)*(float)(*arrayParam)[5]);
			 
			 for (int a = 0; a <= end[0]; a++)
			 {
				 if (a) glTranslatef(bb.x+arraySpacing.x*scale.x,0,0);
				 
				 for (int b = 0; b <= end[1]; b++)
				 {
					 if (b) glTranslatef(0,bb.y+arraySpacing.y*scale.y,0);
					 
					 for (int c = 0; c <= end[2]; c++)
					 {
						 if (c) glTranslatef(0,0,bb.z+arraySpacing.z*scale.z);

						 float t[16];
						 
						 btTransform trans;
						 
						 glGetFloatv(GL_MODELVIEW_MATRIX,t);
						 trans.setFromOpenGLMatrix(t);
						 
						 arrayShape->addChildShape(trans,mTriMeshShape);						 
					 }
					 
					 if (end[2]) glTranslatef(0,0,(-bb.z-arraySpacing.z*scale.z)*(float)end[2]);
				 }
				 
				 if (end[1]) glTranslatef(0,(-bb.y-arraySpacing.y*scale.y)*(float)end[1],0);				
			 }
					
			glPopMatrix();
			 
			 mTriMeshShape = arrayShape;
		 }
	 }
	
	 return mTriMeshShape;
}


void CollisionMap::addMesh(Mesh &mesh_obj)
{
	Mesh *obj_temp;
	
	obj_temp = new Mesh();
	
	obj_temp->cloneStructure(mesh_obj);
#ifndef ARCH_PSP
#ifndef OPENGL_ES
#ifndef ARCH_DC
	obj_temp->triangulate();
#endif
#endif
#endif
	obj_temp->calcNormals();
	
	meshes.push_back(obj_temp);
};


void CollisionMap::compileSphereMap(float nominal_rad)
{
	vector < Mesh * >::iterator meshes_i;
	vector < Face * >::iterator faces_i;
	vector < XYZ * >::iterator points_i;
	
	cvrIndex faceNum = 0;
//	unsigned int numDivs;
	bool bb_init = false;

	Logger::log("Compiling sphere map for collision mesh nominal radius %f\n\n",nominal_rad);
	
	collision_box bbox;
	bbox.xmin = bbox.xmax = bbox.ymin = bbox.ymax = bbox.zmin = bbox.zmax = 0;
	
	for (meshes_i = meshes.begin(); meshes_i != meshes.end(); meshes_i++)
	{
		for (faces_i = (*meshes_i)->faces.begin(); faces_i != (*meshes_i)->faces.end(); faces_i++)
		{
			// Find center of face (point average, triangles only)
			Vector centerpoint = Vector(0,0,0);
			
			if ((*faces_i)->points.size() == 0) continue;
			
			for (points_i = (*faces_i)->points.begin(); points_i != (*faces_i)->points.end(); points_i++)
			{
				if (!bb_init)
				{
					bbox.xmin = (**points_i).x;
					bbox.xmax = (**points_i).x;
					bbox.ymin = (**points_i).y;
					bbox.ymax = (**points_i).y;
					bbox.zmin = (**points_i).z;
					bbox.zmax = (**points_i).z;
					bb_init = true;
				}
				centerpoint += (XYZ)(**points_i);
				
				// calculate bounding regions
				bbox.xmin = (bbox.xmin < (**points_i).x) ? bbox.xmin : (**points_i).x;
				bbox.ymin = (bbox.ymin < (**points_i).y) ? bbox.ymin : (**points_i).y;
				bbox.zmin = (bbox.zmin < (**points_i).z) ? bbox.zmin : (**points_i).z;

				bbox.xmax = (bbox.xmax > (**points_i).x) ? bbox.xmax : (**points_i).x;
				bbox.ymax = (bbox.ymax > (**points_i).y) ? bbox.ymax : (**points_i).y;
				bbox.zmax = (bbox.zmax > (**points_i).z) ? bbox.zmax : (**points_i).z;
			}
			
			centerpoint /= (cvrFloat)(*faces_i)->points.size();
			
			// face bounding sphere radius
			Vector rad;
			
			rad=XYZ(0,0,0);
			
			// Determine maximum extent (radius) of any rotated position of this face from it's centerpoint
			for (points_i = (*faces_i)->points.begin(); points_i != (*faces_i)->points.end(); points_i++)
			{
				Vector dist;
				
				dist = *(*points_i);
				dist -= centerpoint;

				if (dist.magnitude() > rad.magnitude()) rad = dist;
			}
			
			
			// check for thin triangles that aren't going to have enough surface area
//			double rfail = 0;
//			Vector rtest[3];
//			
//			rtest[0] = *(*faces_i).points[0];
//			rtest[1] = *(*faces_i).points[1];
//			rtest[2] = *(*faces_i).points[2];
//
//			rtest[0] -= *(*faces_i).points[2];
//			rtest[1] -= *(*faces_i).points[0];
//			rtest[2] -= *(*faces_i).points[1];
//			
//			if (rtest[0].magnitude() < nominal_rad) rfail = rtest[0].magnitude();
//			if (rtest[1].magnitude() < nominal_rad) rfail = rtest[1].magnitude();
//			if (rtest[2].magnitude() < nominal_rad) rfail = rtest[2].magnitude();
			
			// if the face is smaller than the nominal rad just add it as-is
			if (rad.magnitude() <= nominal_rad*2.0)
			{
				addSphere(centerpoint,rad.magnitude(),faceNum); //rad.magnitude()
				continue;
			};

						
//			addSphere(centerpoint,rad.magnitude(),faceNum); //rad.magnitude()
//			addSphere(*(*faces_i).points[0],nominal_rad,faceNum); //rad.magnitude()
//			addSphere(*(*faces_i).points[1],nominal_rad,faceNum); //rad.magnitude()
//			addSphere(*(*faces_i).points[2],nominal_rad,faceNum); //rad.magnitude()
			
			
			// create a 'grid' on the current face (two vectors on the plane 90 deg from each other)
			// and 'scatter' the spheres across the surface at the nominal radius within the surface range
			
			Vector norm;
			Vector lv,bv,cp,rp,gv[2];
			
			(*faces_i)->calcNormal(norm);
			norm.makeUnit();
			
			lv = centerpoint;
//			lv -= *(*faces_i).points[0];
			
			rp = *(*faces_i)->points[1];
			rp -= *(*faces_i)->points[0];
			rp /= 2.0f;
			rp += *(*faces_i)->points[0];
			lv -= rp;
			
			gv[0] = lv;
			gv[1] = Vector::crossProduct(gv[0],norm);

			gv[0].makeUnit();
			gv[1].makeUnit();

			float i,j;
			float r;
			
			r = rad.magnitude();
						
			for (i = -r; i <= r; i+= nominal_rad)
			{	
				for (j = -r; j <= r; j+= nominal_rad)
				{
					cp = centerpoint;
					
					lv = gv[0];
					lv *= i;

					bv = gv[1];
					bv *= j;
					
					cp += lv;
					cp += bv;
					
					// check if this sphere's centerpoint falls within the face, add it if it does
					if ((*faces_i)->onTriangle(cp)) addSphere(cp,nominal_rad,faceNum);
				}
				
			}
			

			faceNum++;
		}
	}
	
	Logger::log("Generated bounding box.\n");

//	collision_box *bbox_ptr = new collision_box;
	
//	*bbox_ptr = bbox;
	
//	bboxes.push_back(bbox_ptr);
	
	
	collision_sphere *tmp;
	unsigned int currentSphereSet = activeSphereSet;
	unsigned int t;

	activeSphereSet++;

	if (spheres.size()-1 < activeSphereSet)
	{
	 vector<collision_sphere*> *tmpVect = new vector<collision_sphere*>;
	 spheres.push_back(tmpVect);
	}

	
	unsigned int xdivs, ydivs, zdivs;
	cvrFloat block_size;
	
	block_size=1.0f;
	
	xdivs = (int)floor(fabs(bbox.xmax - bbox.xmin)/block_size);
	ydivs = (int)floor(fabs(bbox.ymax - bbox.ymin)/block_size);
	zdivs = (int)floor(fabs(bbox.zmax - bbox.zmin)/block_size);
	
	if (xdivs < 1) xdivs = 1;
	if (ydivs < 1) ydivs = 1;
	if (zdivs < 1) zdivs = 1;

	cvrIndex i,j,k,l;

	map<int, map<int, map<int, vector<int>, ltulong>, ltulong>, ltulong> quant_map;

	cvrFloat xs,ys,zs;
	
	xs = (fabs(bbox.xmax - bbox.xmin) / ((cvrFloat) xdivs) );
	ys = (fabs(bbox.ymax - bbox.ymin) / ((cvrFloat) ydivs) );
	zs = (fabs(bbox.zmax - bbox.zmin) / ((cvrFloat) zdivs) );
	
	for (t = 0; t < spheres[currentSphereSet]->size(); t++)
	{
		tmp = (*spheres[currentSphereSet])[t];		
		
//		if (tmp->radius > 2.0*sqrt(xs*xs + ys*ys + zs*zs)) 
//		{
//			(*spheres[activeSphereSet]).push_back(tmp);
//			
//			continue;
//		}
		
		i = (unsigned int)floor((tmp->center.x-bbox.xmin) / xs);
		j = (unsigned int)floor((tmp->center.y-bbox.ymin) / ys);
		k = (unsigned int)floor((tmp->center.z-bbox.zmin) / zs);
		
		i = (i < 0)?0:i;
		j = (j < 0)?0:j;
		k = (k < 0)?0:k;

		i = (i > xdivs-1)?(xdivs-1):i;
		j = (j > ydivs-1)?(ydivs-1):j;
		k = (k > zdivs-1)?(zdivs-1):k;

//		if (i < 0 | j < 0 | k < 0 | i > STEP_DIVS-1 | j > STEP_DIVS-1 | k > STEP_DIVS-1) 
//		{ 
//			(*spheres[activeSphereSet]).push_back(tmp);
//	
//			printf("skipped? %d,%d,%d xyz: %f,%f,%f\n",i,j,k,tmp->center.x,tmp->center.y,tmp->center.z); 
//			
//			continue; 
//		};

//		printf("Sorted point into sector i,j,k: %d, %d, %d\n",i,j,k);
		
		quant_map[i][j][k].push_back(t);
	}
	
	for (i = 0; i < xdivs; i++)
	{
		for (j = 0; j < ydivs; j++)
		{
			for (k = 0; k < zdivs; k++)
			{
				unsigned int numPts = quant_map[i][j][k].size();
				
				if (numPts)
				{
					Vector quant_center;
					Vector sect_center;
					Vector half_sect;
					
					Vector quant_rad;
					cvrFloat dquant_rad;
					cvrFloat max_rad;
					
					collision_box *bb_sect;
					

					quant_center = XYZ(0,0,0);
					sect_center = XYZ(bbox.xmin+((cvrFloat)i)*xs,bbox.ymin+((cvrFloat)j)*ys,bbox.zmin+((cvrFloat)k)*zs);

					bb_sect = new collision_box;
					
					bb_sect->xmin = sect_center.x;
					bb_sect->ymin = sect_center.y;
					bb_sect->zmin = sect_center.z;

					bb_sect->xmax = sect_center.x+xs;
					bb_sect->ymax = sect_center.y+ys;
					bb_sect->zmax = sect_center.z+zs;
					
					bboxes.push_back(bb_sect);

					half_sect = XYZ(xs/2.0f,ys/2.0f,zs/2.0f);
					sect_center += half_sect;
					
					max_rad = 0.0f;


					for (l = 0; l < numPts; l++)
					{
						tmp = (*spheres[currentSphereSet])[(quant_map[i][j][k])[l]];
						quant_center += tmp->center;
					}

					quant_center /= (cvrFloat)numPts;
					
					//printf("Sector i,j,k: %d, %d, %d\n",i,j,k);
					for (l = 0; l < numPts; l++)
					{
						tmp = (*spheres[currentSphereSet])[(quant_map[i][j][k])[l]];
						
					//	printf("Found sphere: %d\n",(*quant_map[i][j][k])[l]);

						quant_rad = quant_center;
						quant_rad -= tmp->center;
						
						dquant_rad = quant_rad.magnitude();
						
						if (dquant_rad > max_rad) max_rad = dquant_rad;
					}
					
										
					addSphere(quant_center,max_rad,tmp->face);
				}
			}
		}
	}
	
	
	if (spheres.size()) 
	{
		Logger::log("Generated %d spheres.\n",(*spheres[activeSphereSet]).size());
	}
	else
	{
		Logger::log("No spheres generated.");
	}
//	processSpheres(nominal_rad);
};


void CollisionMap::bindTransformation(Transform *trans_in)
{
	trans = trans_in;
};


void CollisionMap::sphereTest(CollisionMap &a, CollisionMap &b, CollisionList &alist, CollisionList &blist)
{
	
}



void CollisionMap::addBoundingBox(cvrFloat xmin, cvrFloat xmax, cvrFloat ymin, cvrFloat ymax, cvrFloat zmin, cvrFloat zmax)
{
	collision_box *bbox_temp;
	
	bbox_temp = new collision_box;
	
	bbox_temp->xmin = xmin;
	bbox_temp->xmax = xmax;
	bbox_temp->ymin = ymin;
	bbox_temp->ymax = ymax;
	bbox_temp->zmin = zmin;
	bbox_temp->zmax = zmax;
	
	bboxes.push_back(bbox_temp);
};


void CollisionMap::processSpheres(float nominal_rad)
{
	int sSet = activeSphereSet;
	unsigned int i,j;
	collision_sphere *tmp;


	bool bProcNeeded = false;

	for (i = 0; i < spheres[sSet]->size(); i++)
	{
		tmp = (*spheres[sSet])[i];

		if (tmp->radius > nominal_rad)
		{
			bProcNeeded = true;
			break;
		}
	}
	
	if (!bProcNeeded) return;


	activeSphereSet++;
	
	if (spheres.size()-1 < activeSphereSet)
	{
	 vector<collision_sphere*> *tmpVect = new vector<collision_sphere*>;
	 spheres.push_back(tmpVect);
	}
	
	
	for (i = 0; i < spheres[sSet]->size(); i++)
	{
		tmp = (*spheres[sSet])[i];

		if (tmp->radius > nominal_rad)
		{
			cvrFloat q_rad = tmp->radius/2.0f;
//			cvrFloat pdist;
			
			XYZ tmpPoint;
			
			XYZ subSphereOfs[15] = 
			{
				XYZ(0,0,0),
				XYZ(q_rad,0,0),
				XYZ(-q_rad,0,0),
				XYZ(0,q_rad,0),
				XYZ(0,-q_rad,0),
				XYZ(0,0,q_rad),
				XYZ(0,0,-q_rad),
				XYZ(-q_rad,-q_rad,-q_rad),
				XYZ(-q_rad,-q_rad,q_rad),
				XYZ(q_rad,-q_rad,-q_rad),
				XYZ(q_rad,-q_rad,q_rad),
				XYZ(-q_rad,q_rad,-q_rad),
				XYZ(-q_rad,q_rad,q_rad),
				XYZ(-q_rad,q_rad,-q_rad),
				XYZ(q_rad,q_rad,q_rad)
			};
			

			for (j = 0; j < 15; j++)
			{
				tmpPoint = tmp->center;
				tmpPoint += subSphereOfs[j];

				if (tmp->face >= 0)
				{
					float dist = meshes[0]->faces[tmp->face]->planeDistanceTo(tmpPoint);
					
					if (dist < q_rad && dist >= 0)
					{
						addSphere(tmpPoint,q_rad,tmp->face);
					}
					// else discard
				}

			}
		}
		else
		{
			(*spheres[activeSphereSet]).push_back(tmp);
		}

	}

	Logger::log("Generated %d sub spheres.\n",(*spheres[activeSphereSet]).size());

	processSpheres(nominal_rad);
}

void CollisionMap::addSphere(XYZ &center, cvrFloat radius, cvrIndex faceNum)
{
	collision_sphere *sphere_temp;
	
	sphere_temp = new collision_sphere;
	
	sphere_temp->center = center;
	sphere_temp->radius = radius;
	sphere_temp->face = faceNum;
	
	
	if (!spheres.size())
	{
		vector<collision_sphere*> *tmpVect = new vector<collision_sphere*>;
		spheres.push_back(tmpVect);
		activeSphereSet = 0;
	}
	
	(*spheres[activeSphereSet]).push_back(sphere_temp);
	
	
	
//	printf("Collision Map: added sphere centerpoint: %f, %f, %f,  radius: %f,  face: %d\n",center.x,center.y,center.z,radius,faceNum);
};
