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

#include <CubicVR/SectorMap.h>


SectorMap::SectorMap(const XYZ &bb_pt1, const XYZ &bb_pt2, double sector_size_in) 
{
	map.empty();

	if (bb_pt1.x < bb_pt2.x && bb_pt1.y < bb_pt2.y && bb_pt1.z < bb_pt2.z)
	{	
		bb1 = bb_pt1;
		bb2 = bb_pt2;
	}
	else
	{
		bb1 = bb_pt2;
		bb2 = bb_pt1;
	}

	bounds = bb2-bb1;
	
	divx = (long)(bounds.x/sector_size_in);
	divy = (long)(bounds.y/sector_size_in);
	divz = (long)(bounds.z/sector_size_in);
	
	bbsec = XYZ(bounds.x/(float)divx,bounds.y/(float)divy,bounds.z/(float)divz);
}

SectorLoc SectorMap::getSector(const XYZ &pt)
{
	SectorLoc tmpSector;
	
	XYZ cpos = pt;
	
	cpos -= bb1;
	
	long x = (long)(cpos.x/((float)bounds.x/(float)divx));
	long y = (long)(cpos.y/((float)bounds.y/(float)divy));
	long z = (long)(cpos.z/((float)bounds.z/(float)divz));
	
	tmpSector.x = x;
	tmpSector.y = y;
	tmpSector.z = z;
	
//	printf("%d, %d, %d\n",x,y,z);
	
	return tmpSector;
}



void SectorMap::disposeStaticCast(std::vector<SectorLoc *> &staticCast)
{
	std::vector<SectorLoc *>::iterator i;
	
	for (i = staticCast.begin(); i != staticCast.end(); i++)
	{
		delete (*i);
	}
	
	staticCast.clear();
}


void SectorMap::castSphere(std::vector<SectorLoc *> &staticCast, XYZ &position, float radius)
{
	XYZ aabb1, aabb2;
	Vector dist;
	
	aabb1 = position-radius;
	aabb2 = position+radius;
	
	SectorLoc loc_start, loc_end;
	cvrIndex a,b,c;
//	int i;
	
	loc_start = getSector(aabb1);
	loc_end = getSector(aabb2);
		
	float secSpan = bbsec.x;
	if (bbsec.y > secSpan) secSpan = bbsec.y;
	if (bbsec.z > secSpan) secSpan = bbsec.z;	
	secSpan = -secSpan;
	
//	bool testBB;
	
	for (a = loc_start.x; a <= loc_end.x; a++)
	{
		if (a > divx || a < 0) continue;
		
		for (b = loc_start.y; b <= loc_end.y; b++)
		{
			if (b > divy || b < 0) continue;
			
			for (c = loc_start.z; c <= loc_end.z; c++)
			{
				if (c > divz || c < 0) continue;						
				
				XYZ bbTestSize = bbsec * 0.5f;
				XYZ bbTestPos = bb1+(XYZ(((float)a)*bbsec.x,((float)b)*bbsec.y,((float)c)*bbsec.z))+bbTestSize;
				
//				XYZ testpt[8];
//				testpt[0] = aabb1;
//				testpt[1] = XYZ(aabb2.x,aabb1.y,aabb1.z);
//				testpt[2] = XYZ(aabb1.x,aabb2.y,aabb1.z);				
//				testpt[3] = XYZ(aabb1.x,aabb1.y,aabb2.z);				
//				testpt[4] = XYZ(aabb1.x,aabb2.y,aabb2.z);
//				testpt[5] = XYZ(aabb2.x,aabb1.y,aabb2.z);
//				testpt[6] = XYZ(aabb2.x,aabb2.y,aabb1.z);
//				testpt[7] = aabb2;
				
//				testBB = false;
//				
//				for (i = 0; i < 8; i++)
//				{
//					dist = testpt[i]-position;
//					if (dist.magnitude()<=radius)
//					{
//						testBB=true;
//						break;
//					}
//				}
//				
//				if (!testBB) continue;
				
				dist = bbTestPos-position;
				if (dist.magnitude() > radius - secSpan/2.0) continue;
				
				SectorLoc *loc;
				
				loc = new SectorLoc;
				loc->x=a;
				loc->y=b;
				loc->z=c;
				
				staticCast.push_back(loc);
			}
		}
	}
	
}	

void SectorMap::castFrustum(std::vector<SectorLoc *> &staticCast, XYZ &position, float modelview[16], float fov, float nearclip, float farclip, float aspect)
{
	Vector forward,nearpt,farpt;
	
	right.x = modelview[0];
	right.y = modelview[4];
	right.z = modelview[8];
	
	up.x = modelview[1];
	up.y = modelview[5];
	up.z = modelview[9];
	
	forward = up;
	forward *= right;
	forward.makeUnit();
	
	nearpt = forward;
	nearpt *= nearclip;
	
	farpt = forward;
	farpt *= (farclip);
	
	float wNear,hNear,wFar,hFar;
	
	hNear = 2.0f * tan(DEGTORAD((fov)) / 2.0f) * nearclip;
	wNear = hNear * aspect;
	hFar = 2.0f * tan(DEGTORAD((fov)) / 2.0f) * farclip;
	wFar = hFar * aspect;
	
	
	Vector far_right = right * (wFar/2.0f);
	Vector far_up = up * (hFar/2.0f);
	
	Vector near_right = right * (wNear/2.0f);
	Vector near_up = up * (hNear/2.0f);
	
	Vector far_top_left = far_up - far_right + farpt;
	Vector far_top_right = far_up + far_right + farpt;
	Vector far_bottom_left = Vector(0,0,0) - far_up - far_right + farpt;
	Vector far_bottom_right = Vector(0,0,0) - far_up + far_right + farpt;
	
	Vector near_top_left = near_up - near_right + nearpt;
	Vector near_top_right = near_up + near_right + nearpt;
	Vector near_bottom_left = Vector(0,0,0) - near_up - near_right + nearpt;
	Vector near_bottom_right = Vector(0,0,0) - near_up + near_right + nearpt;		
	
	far_top_left += position;
	far_top_right += position;
	far_bottom_left += position;
	far_bottom_right += position;
	
	near_top_left += position;
	near_top_right += position;
	near_bottom_left += position;
	near_bottom_right += position;
	
	
	XYZ aabb1, aabb2;
	XYZ f_bounds[8];
	
	f_bounds[0] = far_top_left;
	f_bounds[1] = far_top_right;
	f_bounds[2] = far_bottom_left;
	f_bounds[3] = far_bottom_right;
	f_bounds[4] = near_top_left;
	f_bounds[5] = near_top_right;
	f_bounds[6] = near_bottom_left;
	f_bounds[7] = near_bottom_right;
	
	aabb1 = f_bounds[0];
	aabb2 = f_bounds[0];
	
	for (int i = 1; i < 8; i++)
	{
		if (aabb1.x > f_bounds[i].x) aabb1.x = f_bounds[i].x;
		if (aabb1.y > f_bounds[i].y) aabb1.y = f_bounds[i].y;
		if (aabb1.z > f_bounds[i].z) aabb1.z = f_bounds[i].z;
		
		if (aabb2.x < f_bounds[i].x) aabb2.x = f_bounds[i].x;
		if (aabb2.y < f_bounds[i].y) aabb2.y = f_bounds[i].y;
		if (aabb2.z < f_bounds[i].z) aabb2.z = f_bounds[i].z;
	}
	
	//	querySectorAABB(objList,aabb1,aabb2);
	
	Face face_top,face_left,face_bottom,face_right,face_near,face_far;
	
	face_near.points.resize(3);
	face_near.points[0] = &f_bounds[6];
	face_near.points[1] = &f_bounds[4];
	face_near.points[2] = &f_bounds[5];
	face_near.calcFaceNormal();
	
	face_far.points.resize(3);
	face_far.points[0] = &f_bounds[2];
	face_far.points[1] = &f_bounds[1];
	face_far.points[2] = &f_bounds[0];
	face_far.calcFaceNormal();	
	
	face_left.points.resize(3);
	face_left.points[0] = &f_bounds[6];
	face_left.points[1] = &f_bounds[2];
	face_left.points[2] = &f_bounds[0];
	face_left.calcFaceNormal();
	
	face_right.points.resize(3);
	face_right.points[0] = &f_bounds[7];
	face_right.points[1] = &f_bounds[5];
	face_right.points[2] = &f_bounds[1];
	face_right.calcFaceNormal();
	
	face_bottom.points.resize(3);
	face_bottom.points[0] = &f_bounds[6];
	face_bottom.points[1] = &f_bounds[7];
	face_bottom.points[2] = &f_bounds[3];
	face_bottom.calcFaceNormal();
	
	face_top.points.resize(3);
	face_top.points[0] = &f_bounds[5];
	face_top.points[1] = &f_bounds[4];
	face_top.points[2] = &f_bounds[0];
	face_top.calcFaceNormal();
	
	
	SectorLoc loc_start, loc_end;
	cvrIndex a,b,c;
	
	loc_start = getSector(aabb1);
	loc_end = getSector(aabb2);
	
	std::set<SceneObject *>::iterator i;
	
	
//	Vector bbsecv;
//	bbsecv = bbsec;
	float secSpan = bbsec.x;
//
	if (bbsec.y > secSpan) secSpan = bbsec.y;
	if (bbsec.z > secSpan) secSpan = bbsec.z;	
	secSpan = -secSpan;
//	float secSpan = -bbsecv.magnitude();
	
	for (a = loc_start.x; a <= loc_end.x; a++)
	{
		if (a > divx || a < 0) continue;
		
		for (b = loc_start.y; b <= loc_end.y; b++)
		{
			if (b > divy || b < 0) continue;
			
			for (c = loc_start.z; c <= loc_end.z; c++)
			{
				if (c > divz || c < 0) continue;						
				
				XYZ bbTestMin = bb1+(XYZ(((float)a)*bbsec.x,((float)b)*bbsec.y,((float)c)*bbsec.z));		
				XYZ bbTestMax = bbTestMin + bbsec;
				
				if (face_near.planeDistanceTo(bbTestMin) < secSpan && face_near.planeDistanceTo(bbTestMax) < secSpan) continue;
				if (face_far.planeDistanceTo(bbTestMin) < secSpan && face_far.planeDistanceTo(bbTestMax) < secSpan) continue;
				if (face_left.planeDistanceTo(bbTestMin) < secSpan && face_left.planeDistanceTo(bbTestMax) < secSpan) continue;
				if (face_right.planeDistanceTo(bbTestMin) < secSpan && face_right.planeDistanceTo(bbTestMax) < secSpan) continue;
				if (face_top.planeDistanceTo(bbTestMin) < secSpan && face_top.planeDistanceTo(bbTestMax) < secSpan) continue;
				if (face_bottom.planeDistanceTo(bbTestMin) < secSpan && face_bottom.planeDistanceTo(bbTestMax) < secSpan) continue;
				
				SectorLoc *loc;
				
				loc = new SectorLoc;
				loc->x=a;
				loc->y=b;
				loc->z=c;
				
				staticCast.push_back(loc);
				
//				for (i = map[a][b][c].sceneobj.begin(); i != map[a][b][c].sceneobj.end(); i++)
//				{
//					objList.insert((*i));
//				}
			}
		}
	}
	
	
//	std::set<SceneObject *> removals;
//	
//	for (i = objList.begin(); i != objList.end(); i++)
//	{
//		XYZ aabb1_obj, aabb2_obj;
//		
//		(*i)->getAABB(aabb1_obj,aabb2_obj);
//		
//		Vector objsec;
//		objsec = aabb2_obj-aabb1_obj;
//		
//		float objSpan = -objsec.magnitude();
//		
//		if (face_near.planeDistanceTo(aabb1_obj) < objSpan && face_near.planeDistanceTo(aabb2_obj) < objSpan) { removals.insert(*i);  continue; };
//		if (face_far.planeDistanceTo(aabb1_obj) < objSpan && face_far.planeDistanceTo(aabb2_obj) < objSpan) { removals.insert(*i);  continue; };
//		if (face_left.planeDistanceTo(aabb1_obj) < objSpan && face_left.planeDistanceTo(aabb2_obj) < objSpan) { removals.insert(*i);  continue; };
//		if (face_right.planeDistanceTo(aabb1_obj) < objSpan && face_right.planeDistanceTo(aabb2_obj) < objSpan) { removals.insert(*i);  continue; };
//		if (face_top.planeDistanceTo(aabb1_obj) < objSpan && face_top.planeDistanceTo(aabb2_obj) < objSpan) { removals.insert(*i);  continue; };
//		if (face_bottom.planeDistanceTo(aabb1_obj) < objSpan && face_bottom.planeDistanceTo(aabb2_obj) < objSpan) { removals.insert(*i);  continue; };
//	}
//	
//	for (i = removals.begin(); i != removals.end(); i++)
//	{
//		objList.erase(*i);
//	}
		
}	

#if !defined(ARCH_PSP) && !defined(OPENGL_ES)
void SectorMap::renderFrustum(MATRIX4X4 projMatrix, MATRIX4X4 modelMatrix, float nearClip, float farClip, const int viewport[4])
{
	FRUSTUM frust;
	
	frust.update(projMatrix,modelMatrix);
	
	XYZ aabb1, aabb2;
//	XYZ f_bounds[8];
	
	
	XYZ far_top_left,far_top_right,far_bottom_left,far_bottom_right,near_top_left,near_top_right,near_bottom_left,near_bottom_right;
	
	
	
	near_top_left = frust.unProject(XYZ(viewport[0],				viewport[1],			nearClip),projMatrix,modelMatrix,viewport);
	near_top_right = frust.unProject(XYZ(viewport[0]+viewport[2],	viewport[1],			nearClip),projMatrix,modelMatrix,viewport);
	near_bottom_left = frust.unProject(XYZ(viewport[0],				viewport[1]+viewport[3],nearClip),projMatrix,modelMatrix,viewport);
	near_bottom_right = frust.unProject(XYZ(viewport[0]+viewport[2],	viewport[1]+viewport[3],nearClip),projMatrix,modelMatrix,viewport);
	
	far_top_left = frust.unProject(XYZ(viewport[0],				viewport[1],			farClip),projMatrix,modelMatrix,viewport);
	far_top_right = frust.unProject(XYZ(viewport[0]+viewport[2],	viewport[1],			farClip),projMatrix,modelMatrix,viewport);
	far_bottom_left = frust.unProject(XYZ(viewport[0],				viewport[1]+viewport[3],farClip),projMatrix,modelMatrix,viewport);
	far_bottom_right = frust.unProject(XYZ(viewport[0]+viewport[2],	viewport[1]+viewport[3],farClip),projMatrix,modelMatrix,viewport);
	
//	aabb1 = f_bounds[0];
//	aabb2 = f_bounds[0];
//	
//	for (int i = 1; i < 8; i++)
//	{
//		if (aabb1.x > f_bounds[i].x) aabb1.x = f_bounds[i].x;
//		if (aabb1.y > f_bounds[i].y) aabb1.y = f_bounds[i].y;
//		if (aabb1.z > f_bounds[i].z) aabb1.z = f_bounds[i].z;
//		
//		if (aabb2.x < f_bounds[i].x) aabb2.x = f_bounds[i].x;
//		if (aabb2.y < f_bounds[i].y) aabb2.y = f_bounds[i].y;
//		if (aabb2.z < f_bounds[i].z) aabb2.z = f_bounds[i].z;
//	}
//	
//	SectorLoc loc_start, loc_end;
//	cvrIndex a,b,c;
//	
//	loc_start = getSector(aabb1);
//	loc_end = getSector(aabb2);
#ifndef ARCH_DC	
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(4.0f);
#endif
	glColor3f(1,0,0);
	glBegin(GL_LINE_STRIP);

//	for (int i = 1; i < 9; i++)
//	{	
//		glVertex3f(f_bounds[i%8].x,f_bounds[i%8].y,f_bounds[i%8].z);
//	}
//		
//	glEnd();
//	glDisable(GL_LINE_SMOOTH);
	
	glVertex3f(far_top_left.x,far_top_left.y,far_top_left.z);
	glVertex3f(near_top_left.x,near_top_left.y,near_top_left.z);
	
	glVertex3f(far_top_right.x,far_top_right.y,far_top_right.z);
	glVertex3f(near_top_right.x,near_top_right.y,near_top_right.z);
	
	glVertex3f(far_bottom_left.x,far_bottom_left.y,far_bottom_left.z);
	glVertex3f(near_bottom_left.x,near_bottom_left.y,near_bottom_left.z);
	
	glVertex3f(far_bottom_right.x,far_bottom_right.y,far_bottom_right.z);
	glVertex3f(near_bottom_right.x,near_bottom_right.y,near_bottom_right.z);
	
	glEnd();
	
	
	glColor3f(1,0,0);
	glBegin(GL_LINE_STRIP);
	
	glVertex3f(far_bottom_left.x,far_bottom_left.y,far_bottom_left.z);
	glVertex3f(far_bottom_right.x,far_bottom_right.y,far_bottom_right.z);
	glVertex3f(far_top_right.x,far_top_right.y,far_top_right.z);
	glVertex3f(far_top_left.x,far_top_left.y,far_top_left.z);
	glVertex3f(far_bottom_left.x,far_bottom_left.y,far_bottom_left.z);
	
	glEnd();
	
	
	glColor3f(0,1,0);
	glBegin(GL_LINE_STRIP);
	
	glVertex3f(near_bottom_left.x,near_bottom_left.y,near_bottom_left.z);
	glVertex3f(near_bottom_right.x,near_bottom_right.y,near_bottom_right.z);
	glVertex3f(near_top_right.x,near_top_right.y,near_top_right.z);
	glVertex3f(near_top_left.x,near_top_left.y,near_top_left.z);
	glVertex3f(near_bottom_left.x,near_bottom_left.y,near_bottom_left.z);
	
	glEnd();
	
}	
#endif


void SectorMap::queryFrustum(std::set<SceneObject*> &objList, MATRIX4X4 projMatrix, MATRIX4X4 modelMatrix, float nearClip, float farClip, const int viewport[4], std::set<Light *> *lightList)
{
	frust.update(projMatrix,modelMatrix);
	
	/*
	 viewport[0]=x
	 viewport[1]=y
	 viewport[2]=width
	 viewport[3]=height 	 
	 */
	
	XYZ aabb1, aabb2;
	XYZ f_bounds[8];
	
	f_bounds[0] = frust.unProject(XYZ(viewport[0],				viewport[1],			nearClip),projMatrix,modelMatrix,viewport);
	f_bounds[1] = frust.unProject(XYZ(viewport[0]+viewport[2],	viewport[1],			nearClip),projMatrix,modelMatrix,viewport);
	f_bounds[2] = frust.unProject(XYZ(viewport[0],				viewport[1]+viewport[3],nearClip),projMatrix,modelMatrix,viewport);
	f_bounds[3] = frust.unProject(XYZ(viewport[0]+viewport[2],	viewport[1]+viewport[3],nearClip),projMatrix,modelMatrix,viewport);

	f_bounds[4] = frust.unProject(XYZ(viewport[0],				viewport[1],			farClip),projMatrix,modelMatrix,viewport);
	f_bounds[5] = frust.unProject(XYZ(viewport[0]+viewport[2],	viewport[1],			farClip),projMatrix,modelMatrix,viewport);
	f_bounds[6] = frust.unProject(XYZ(viewport[0],				viewport[1]+viewport[3],farClip),projMatrix,modelMatrix,viewport);
	f_bounds[7] = frust.unProject(XYZ(viewport[0]+viewport[2],	viewport[1]+viewport[3],farClip),projMatrix,modelMatrix,viewport);
	
	aabb1 = f_bounds[0];
	aabb2 = f_bounds[0];
	
	for (int i = 1; i < 8; i++)
	{
		if (aabb1.x > f_bounds[i].x) aabb1.x = f_bounds[i].x;
		if (aabb1.y > f_bounds[i].y) aabb1.y = f_bounds[i].y;
		if (aabb1.z > f_bounds[i].z) aabb1.z = f_bounds[i].z;
		
		if (aabb2.x < f_bounds[i].x) aabb2.x = f_bounds[i].x;
		if (aabb2.y < f_bounds[i].y) aabb2.y = f_bounds[i].y;
		if (aabb2.z < f_bounds[i].z) aabb2.z = f_bounds[i].z;
	}
	
	SectorLoc loc_start, loc_end;
	cvrIndex a,b,c;
	
	loc_start = getSector(aabb1);
	loc_end = getSector(aabb2);
	
	std::set<SceneObject *>::iterator i;
	SectorNode *tmpNode;
	
	//	last_cast.clear();
	
	for (a = loc_start.x; a <= loc_end.x; a++)
	{
		if (a > divx || a < 0) continue;
		if (map.find(a) == map.end()) continue;
		
		for (b = loc_start.y; b <= loc_end.y; b++)
		{
			if (b > divy || b < 0) continue;
			if (map[a].find(b) == map[a].end()) continue;
			
			for (c = loc_start.z; c <= loc_end.z; c++)
			{
				if (c > divz || c < 0) continue;						
				if (map[a][b].find(c) == map[a][b].end()) continue;
				
				XYZ bbTestMin = bb1+(XYZ((float)a*bbsec.x,(float)b*bbsec.y,(float)c*bbsec.z));		
				XYZ bbTestMax = bbTestMin + bbsec;
				
				if (frust.isBoundingBoxInside(bbTestMin, bbTestMax))
				{
					tmpNode = map[a][b][c];
					
					///				last_cast.insert(tmpNode);
					
					if (lightList != NULL) for (light_i = tmpNode->light.begin(); light_i != tmpNode->light.end(); light_i++)
					{					
						lightList->insert(*light_i);
					}
					
					for (i = map[a][b][c]->sceneobj.begin(); i != map[a][b][c]->sceneobj.end(); i++)
					{
						objList.insert((*i));
					}
				}				
			}
		}
	}
	
	
//	std::set<SceneObject *> removals;
//	
//	for (i = objList.begin(); i != objList.end(); i++)
//	{
//		if (!frust.IsBoundingBoxInside((*i)->aabbMin, (*i)->aabbMax))
//		{
//			removals.insert(*i);
//		}
//	}
//	
//	for (i = removals.begin(); i != removals.end(); i++)
//	{
//		objList.erase(*i);
//	}
	
	if (lightList != NULL) for (light_i = global_light.begin(); light_i != global_light.end(); light_i++)
	{					
		lightList->insert(*light_i);
	}
	
	
	//	printf("%f, %f, %f / %f, %f, %f, fov:%f num_in_view:%d\n",aabb1.x,aabb1.y,aabb1.z,aabb2.x,aabb2.y,aabb2.z,cam->fov,objList.size());
}	


void SectorMap::queryOrthoFrustum(std::set<SceneObject*> &objList, XYZ position, float ortho_width, float ortho_height, MATRIX4X4 projMatrix, MATRIX4X4 modelMatrix, float nearClip, float farClip, const int viewport[4], std::set<Light *> *lightList)
{
//	frust.Update(projMatrix,modelMatrix);

	Vector vright, vup, vforward;
	
	vright.x = modelMatrix.entries[0];
	vright.y = modelMatrix.entries[4];
	vright.z = modelMatrix.entries[8];
	
	vup.x = modelMatrix.entries[1];
	vup.y = modelMatrix.entries[5];
	vup.z = modelMatrix.entries[9];
	
	vup.makeUnit();
	vright.makeUnit();
	
	vforward = vup*vright;
	vforward.makeUnit();
	
	
	XYZ far_top_left,far_top_right,far_bottom_left,far_bottom_right,near_top_left,near_top_right,near_bottom_left,near_bottom_right;
	
	float hw, hh;
	XYZ right=vright,up=vup,forward=vforward;
	
	hw = ortho_width/2.0;
	hh = ortho_height/2.0;
	
	near_top_left = position - right*hw + up*hh;
	near_top_right = position + right*hw + up*hh;
	near_bottom_left = position - right*hw - up*hh;
	near_bottom_right = position + right*hw - up*hh;
	
	far_top_left = near_top_left;
	far_top_right = near_top_right;
	far_bottom_left = near_bottom_left;
	far_bottom_right = near_bottom_right;
	
	near_top_left += forward*nearClip;
	near_top_right += forward*nearClip;
	near_bottom_left += forward*nearClip;
	near_bottom_right += forward*nearClip;
	
	far_top_left += forward*farClip;
	far_top_right += forward*farClip;
	far_bottom_left += forward*farClip;
	far_bottom_right += forward*farClip;

	XYZ aabb1, aabb2;
	XYZ f_bounds[8];

	f_bounds[0] = far_top_left;
	f_bounds[1] = far_top_right;
	f_bounds[2] = far_bottom_left;
	f_bounds[3] = far_bottom_right;
	f_bounds[4] = near_top_left;
	f_bounds[5] = near_top_right;
	f_bounds[6] = near_bottom_left;
	f_bounds[7] = near_bottom_right;
	
	
	frust.plane(0).SetFromPoints(far_bottom_left,far_bottom_right,far_top_right);	// Far Plane
	frust.plane(1).SetFromPoints(near_bottom_right,near_bottom_left,near_top_left);	// Near Plane
	frust.plane(2).SetFromPoints(near_bottom_left,near_bottom_right,far_bottom_right); // Bottom Plane
	frust.plane(3).SetFromPoints(near_top_right,near_top_left,far_top_left); // Top Plane
	frust.plane(4).SetFromPoints(near_top_left,near_bottom_left,far_bottom_left); // Left Plane
	frust.plane(5).SetFromPoints(near_bottom_right, near_top_right, far_top_right); // Right Plane
	
	aabb1 = f_bounds[0];
	aabb2 = f_bounds[0];
	
	for (int i = 1; i < 8; i++)
	{
		if (aabb1.x > f_bounds[i].x) aabb1.x = f_bounds[i].x;
		if (aabb1.y > f_bounds[i].y) aabb1.y = f_bounds[i].y;
		if (aabb1.z > f_bounds[i].z) aabb1.z = f_bounds[i].z;
		
		if (aabb2.x < f_bounds[i].x) aabb2.x = f_bounds[i].x;
		if (aabb2.y < f_bounds[i].y) aabb2.y = f_bounds[i].y;
		if (aabb2.z < f_bounds[i].z) aabb2.z = f_bounds[i].z;
	}
	
	SectorLoc loc_start, loc_end;
	cvrIndex a,b,c;
	
	loc_start = getSector(aabb1);
	loc_end = getSector(aabb2);
	
	std::set<SceneObject *>::iterator i;
	SectorNode *tmpNode;
	
	//	last_cast.clear();
	
	for (a = loc_start.x; a <= loc_end.x; a++)
	{
		if (a > divx || a < 0) continue;
		if (map.find(a) == map.end()) continue;
		
		for (b = loc_start.y; b <= loc_end.y; b++)
		{
			if (b > divy || b < 0) continue;
			if (map[a].find(b) == map[a].end()) continue;
			
			for (c = loc_start.z; c <= loc_end.z; c++)
			{
				if (c > divz || c < 0) continue;						
				if (map[a][b].find(c) == map[a][b].end()) continue;
				
				XYZ bbTestMin = bb1+(XYZ((float)a*bbsec.x,(float)b*bbsec.y,(float)c*bbsec.z));		
				XYZ bbTestMax = bbTestMin + bbsec;
				
				if (frust.isBoundingBoxInside(bbTestMin, bbTestMax))
				{
					tmpNode = map[a][b][c];
					
					///				last_cast.insert(tmpNode);
					
					if (lightList != NULL) for (light_i = tmpNode->light.begin(); light_i != tmpNode->light.end(); light_i++)
					{					
						lightList->insert(*light_i);
					}
					
					for (i = map[a][b][c]->sceneobj.begin(); i != map[a][b][c]->sceneobj.end(); i++)
					{
						objList.insert((*i));
					}
				}				
			}
		}
	}
	
	
//	std::set<SceneObject *> removals;
//	
//	for (i = objList.begin(); i != objList.end(); i++)
//	{
//		if (!frust.IsBoundingBoxInside((*i)->aabbMin, (*i)->aabbMax))
//		{
//			removals.insert(*i);
//		}
//	}
	
//	for (i = removals.begin(); i != removals.end(); i++)
//	{
//		objList.erase(*i);
//	}
	
	if (lightList != NULL) for (light_i = global_light.begin(); light_i != global_light.end(); light_i++)
	{					
		lightList->insert(*light_i);
	}
	
	
	//	printf("%f, %f, %f / %f, %f, %f, fov:%f num_in_view:%d\n",aabb1.x,aabb1.y,aabb1.z,aabb2.x,aabb2.y,aabb2.z,cam->fov,objList.size());
}	

#if !defined(OPENGL_ES) && !defined(ARCH_PSP)
void SectorMap::renderOrthoFrustum(XYZ position, float ortho_width, float ortho_height, MATRIX4X4 projMatrix, MATRIX4X4 modelMatrix, float nearClip, float farClip, const int viewport[4])
{
	Vector vright, vup, vforward;
	
	vright.x = modelMatrix.entries[0];
	vright.y = modelMatrix.entries[4];
	vright.z = modelMatrix.entries[8];
	
	vup.x = modelMatrix.entries[1];
	vup.y = modelMatrix.entries[5];
	vup.z = modelMatrix.entries[9];

	vup.makeUnit();
	vright.makeUnit();

	vforward = vup*vright;
	vforward.makeUnit();
	
	
	XYZ far_top_left,far_top_right,far_bottom_left,far_bottom_right,near_top_left,near_top_right,near_bottom_left,near_bottom_right;
		
	float hw, hh;
	XYZ right=vright,up=vup,forward=vforward;
	
	hw = ortho_width/2.0;
	hh = ortho_height/2.0;
	
	near_top_left = position - right*hw + up*hh;
	near_top_right = position + right*hw + up*hh;
	near_bottom_left = position - right*hw - up*hh;
	near_bottom_right = position + right*hw - up*hh;
	
	far_top_left = near_top_left;
	far_top_right = near_top_right;
	far_bottom_left = near_bottom_left;
	far_bottom_right = near_bottom_right;
	
	near_top_left += forward*nearClip;
	near_top_right += forward*nearClip;
	near_bottom_left += forward*nearClip;
	near_bottom_right += forward*nearClip;
	far_top_left += forward*farClip;
	far_top_right += forward*farClip;
	far_bottom_left += forward*farClip;
	far_bottom_right += forward*farClip;
	
#ifndef ARCH_DC
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(4.0f);
#endif
	glColor3f(1,0,0);
	glBegin(GL_LINES);
		
	glVertex3f(far_top_left.x,far_top_left.y,far_top_left.z);
	glVertex3f(near_top_left.x,near_top_left.y,near_top_left.z);
	
	glVertex3f(far_top_right.x,far_top_right.y,far_top_right.z);
	glVertex3f(near_top_right.x,near_top_right.y,near_top_right.z);
	
	glVertex3f(far_bottom_left.x,far_bottom_left.y,far_bottom_left.z);
	glVertex3f(near_bottom_left.x,near_bottom_left.y,near_bottom_left.z);
	
	glVertex3f(far_bottom_right.x,far_bottom_right.y,far_bottom_right.z);
	glVertex3f(near_bottom_right.x,near_bottom_right.y,near_bottom_right.z);
	
	glEnd();
	
	
	glColor3f(1,0,0);
	glBegin(GL_LINE_STRIP);
	
	glVertex3f(far_bottom_left.x,far_bottom_left.y,far_bottom_left.z);
	glVertex3f(far_bottom_right.x,far_bottom_right.y,far_bottom_right.z);
	glVertex3f(far_top_right.x,far_top_right.y,far_top_right.z);
	glVertex3f(far_top_left.x,far_top_left.y,far_top_left.z);
	glVertex3f(far_bottom_left.x,far_bottom_left.y,far_bottom_left.z);
	
	glEnd();
	
	
	glColor3f(0,1,0);
	glBegin(GL_LINE_STRIP);
	
	glVertex3f(near_bottom_left.x,near_bottom_left.y,near_bottom_left.z);
	glVertex3f(near_bottom_right.x,near_bottom_right.y,near_bottom_right.z);
	glVertex3f(near_top_right.x,near_top_right.y,near_top_right.z);
	glVertex3f(near_top_left.x,near_top_left.y,near_top_left.z);
	glVertex3f(near_bottom_left.x,near_bottom_left.y,near_bottom_left.z);
	
	glEnd();	


	
	//	printf("%f, %f, %f / %f, %f, %f, fov:%f num_in_view:%d\n",aabb1.x,aabb1.y,aabb1.z,aabb2.x,aabb2.y,aabb2.z,cam->fov,objList.size());
}	
#endif


//void SectorMap::queryFrustum(std::set<SceneObject*> &objList, Camera *cam, std::set<Light *> *lightList)
//void SectorMap::queryFrustum(std::set<SceneObject*> &objList, XYZ &position, float modelview[16], float fov, float nearclip, float farclip, float aspect, std::set<Light *> *lightList)
//{
//#ifdef ARCH_PSP
//	#warning queryFrustum broken on PSP, need matrix pull for modelview
//#endif
//#ifndef ARCH_PSP	
////	float modelview[16];
//
//	Vector forward,nearpt,farpt;
//	
/////	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
//
//	right.x = modelview[0];
//	right.y = modelview[4];
//	right.z = modelview[8];
//	
//	up.x = modelview[1];
//	up.y = modelview[5];
//	up.z = modelview[9];
//	
//	forward = up;
//	forward *= right;
//	forward.makeUnit();
//	
//	nearpt = forward;
//	nearpt *= nearclip;
//	
//	farpt = forward;
//	farpt *= (farclip);
//	
//	float wNear,hNear,wFar,hFar;
//	
//	hNear = 2.0f * tan(DEGTORAD((fov)) / 2.0f) * nearclip;
//	wNear = hNear * aspect;
//	hFar = 2.0f * tan(DEGTORAD((fov)) / 2.0f) * farclip;
//	wFar = hFar * aspect;
//	
//	
//	Vector far_right = right * (wFar/2.0f);
//	Vector far_up = up * (hFar/2.0f);
//	
//	Vector near_right = right * (wNear/2.0f);
//	Vector near_up = up * (hNear/2.0f);
//	
//	Vector far_top_left = far_up - far_right + farpt;
//	Vector far_top_right = far_up + far_right + farpt;
//	Vector far_bottom_left = Vector(0,0,0) - far_up - far_right + farpt;
//	Vector far_bottom_right = Vector(0,0,0) - far_up + far_right + farpt;
//	
//	Vector near_top_left = near_up - near_right + nearpt;
//	Vector near_top_right = near_up + near_right + nearpt;
//	Vector near_bottom_left = Vector(0,0,0) - near_up - near_right + nearpt;
//	Vector near_bottom_right = Vector(0,0,0) - near_up + near_right + nearpt;		
//	
//	far_top_left += position;
//	far_top_right += position;
//	far_bottom_left += position;
//	far_bottom_right += position;
//	
//	near_top_left += position;
//	near_top_right += position;
//	near_bottom_left += position;
//	near_bottom_right += position;
//
//	
//	XYZ aabb1, aabb2;
//	XYZ f_bounds[8];
//	
//	f_bounds[0] = far_top_left;
//	f_bounds[1] = far_top_right;
//	f_bounds[2] = far_bottom_left;
//	f_bounds[3] = far_bottom_right;
//	f_bounds[4] = near_top_left;
//	f_bounds[5] = near_top_right;
//	f_bounds[6] = near_bottom_left;
//	f_bounds[7] = near_bottom_right;
//	
//	aabb1 = f_bounds[0];
//	aabb2 = f_bounds[0];
//
//	for (int i = 1; i < 8; i++)
//	{
//		if (aabb1.x > f_bounds[i].x) aabb1.x = f_bounds[i].x;
//		if (aabb1.y > f_bounds[i].y) aabb1.y = f_bounds[i].y;
//		if (aabb1.z > f_bounds[i].z) aabb1.z = f_bounds[i].z;
//	
//		if (aabb2.x < f_bounds[i].x) aabb2.x = f_bounds[i].x;
//		if (aabb2.y < f_bounds[i].y) aabb2.y = f_bounds[i].y;
//		if (aabb2.z < f_bounds[i].z) aabb2.z = f_bounds[i].z;
//	}
//	
////	querySectorAABB(objList,aabb1,aabb2);
//
//	Face face_top,face_left,face_bottom,face_right,face_near,face_far;
//	
//	face_near.points.resize(3);
//	face_near.points[0] = &f_bounds[6];
//	face_near.points[1] = &f_bounds[4];
//	face_near.points[2] = &f_bounds[5];
//	face_near.calcFaceNormal();
//
//	face_far.points.resize(3);
//	face_far.points[0] = &f_bounds[2];
//	face_far.points[1] = &f_bounds[1];
//	face_far.points[2] = &f_bounds[0];
//	face_far.calcFaceNormal();	
//	
//	face_left.points.resize(3);
//	face_left.points[0] = &f_bounds[6];
//	face_left.points[1] = &f_bounds[2];
//	face_left.points[2] = &f_bounds[0];
//	face_left.calcFaceNormal();
//
//	face_right.points.resize(3);
//	face_right.points[0] = &f_bounds[7];
//	face_right.points[1] = &f_bounds[5];
//	face_right.points[2] = &f_bounds[1];
//	face_right.calcFaceNormal();
//	
//	face_bottom.points.resize(3);
//	face_bottom.points[0] = &f_bounds[6];
//	face_bottom.points[1] = &f_bounds[7];
//	face_bottom.points[2] = &f_bounds[3];
//	face_bottom.calcFaceNormal();
//	
//	face_top.points.resize(3);
//	face_top.points[0] = &f_bounds[5];
//	face_top.points[1] = &f_bounds[4];
//	face_top.points[2] = &f_bounds[0];
//	face_top.calcFaceNormal();
//	
//	
//	SectorLoc loc_start, loc_end;
//	cvrIndex a,b,c;
//	
//	loc_start = getSector(aabb1);
//	loc_end = getSector(aabb2);
//	
//	std::set<SceneObject *>::iterator i;
//	SectorNode *tmpNode;
//	
//	Vector bbsecv;
//	bbsecv = bbsec;
//	float secSpan;
////	float secSpan = bbsec.x;
////	if (bbsec.y > secSpan) secSpan = bbsec.y;
////	if (bbsec.z > secSpan) secSpan = bbsec.z;	
////	secSpan = secSpan;
//	secSpan = -bbsecv.magnitude();
//	
////	last_cast.clear();
//	
//	for (a = loc_start.x; a <= loc_end.x; a++)
//	{
//		if (a > divx || a < 0) continue;
//		if (map.find(a) == map.end()) continue;
//		
//		for (b = loc_start.y; b <= loc_end.y; b++)
//		{
//			if (b > divy || b < 0) continue;
//			if (map[a].find(b) == map[a].end()) continue;
//			
//			for (c = loc_start.z; c <= loc_end.z; c++)
//			{
//				if (c > divz || c < 0) continue;						
//				if (map[a][b].find(c) == map[a][b].end()) continue;
//				
//				XYZ bbTestMin = bb1+(XYZ((float)a*bbsec.x,(float)b*bbsec.y,(float)c*bbsec.z));		
//				XYZ bbTestMax = bbTestMin + bbsec;
//				
//				if (face_near.planeDistanceTo(bbTestMin) < secSpan && face_near.planeDistanceTo(bbTestMax) < secSpan) continue;
//				if (face_far.planeDistanceTo(bbTestMin) < secSpan && face_far.planeDistanceTo(bbTestMax) < secSpan) continue;
//				if (face_left.planeDistanceTo(bbTestMin) < secSpan && face_left.planeDistanceTo(bbTestMax) < secSpan) continue;
//				if (face_right.planeDistanceTo(bbTestMin) < secSpan && face_right.planeDistanceTo(bbTestMax) < secSpan) continue;
//				if (face_top.planeDistanceTo(bbTestMin) < secSpan && face_top.planeDistanceTo(bbTestMax) < secSpan) continue;
//				if (face_bottom.planeDistanceTo(bbTestMin) < secSpan && face_bottom.planeDistanceTo(bbTestMax) < secSpan) continue;
//				
//
//				tmpNode = map[a][b][c];
//				
/////				last_cast.insert(tmpNode);
//
//				
//				if (lightList != NULL) for (light_i = tmpNode->light.begin(); light_i != tmpNode->light.end(); light_i++)
//				{					
//					lightList->insert(*light_i);
//				}
//				
//				for (i = map[a][b][c]->sceneobj.begin(); i != map[a][b][c]->sceneobj.end(); i++)
//				{
//					objList.insert((*i));
//				}
//			}
//		}
//	}
//	
//	
//	std::set<SceneObject *> removals;
//
//	for (i = objList.begin(); i != objList.end(); i++)
//	{
//		XYZ aabb1_obj, aabb2_obj;
//				
//		aabb1_obj = (*i)->aabbMin;
//		aabb2_obj = (*i)->aabbMax;
////		(*i)->getAABB(aabb1_obj,aabb2_obj);
//		
//		Vector objsec;
//		objsec = aabb2_obj-aabb1_obj;
//		
//		float objSpan = -objsec.magnitude();
//		
//		if (face_near.planeDistanceTo(aabb1_obj) < objSpan && face_near.planeDistanceTo(aabb2_obj) < objSpan) { removals.insert(*i);  continue; };
//		if (face_far.planeDistanceTo(aabb1_obj) < objSpan && face_far.planeDistanceTo(aabb2_obj) < objSpan) { removals.insert(*i);  continue; };
//		if (face_left.planeDistanceTo(aabb1_obj) < objSpan && face_left.planeDistanceTo(aabb2_obj) < objSpan) { removals.insert(*i);  continue; };
//		if (face_right.planeDistanceTo(aabb1_obj) < objSpan && face_right.planeDistanceTo(aabb2_obj) < objSpan) { removals.insert(*i);  continue; };
//		if (face_top.planeDistanceTo(aabb1_obj) < objSpan && face_top.planeDistanceTo(aabb2_obj) < objSpan) { removals.insert(*i);  continue; };
//		if (face_bottom.planeDistanceTo(aabb1_obj) < objSpan && face_bottom.planeDistanceTo(aabb2_obj) < objSpan) { removals.insert(*i);  continue; };
//	}
//	
//	for (i = removals.begin(); i != removals.end(); i++)
//	{
//		objList.erase(*i);
//	}
//	
//	if (lightList != NULL) for (light_i = global_light.begin(); light_i != global_light.end(); light_i++)
//	{					
//		lightList->insert(*light_i);
//	}
//	
//	
////	printf("%f, %f, %f / %f, %f, %f, fov:%f num_in_view:%d\n",aabb1.x,aabb1.y,aabb1.z,aabb2.x,aabb2.y,aabb2.z,cam->fov,objList.size());
//#endif	
//
//}	
	

void SectorMap::renderFrustum(Camera *cam)
{
#ifndef ARCH_PSP
#ifndef OPENGL_ES
	Vector forward,nearpt,farpt;
	
	forward = up;
	forward *= right;
	forward.makeUnit();
	
	nearpt = forward;
	nearpt *= cam->nearclip;
	
	farpt = forward;
	farpt *= (cam->farclip);
	
	float wNear,hNear,wFar,hFar;
	
	hNear = 2.0f * tan(DEGTORAD(cam->fov) / 2.0f) * cam->nearclip;
	wNear = hNear * cam->aspect;
	hFar = 2.0f * tan(DEGTORAD(cam->fov) / 2.0f) * cam->farclip;
	wFar = hFar * cam->aspect;
	
	
	Vector far_right = right * (wFar/2.0f);
	Vector far_up = up * (hFar/2.0f);
	
	Vector near_right = right * (wNear/2.0f);
	Vector near_up = up * (hNear/2.0f);
	
	Vector far_top_left = far_up - far_right + farpt;
	Vector far_top_right = far_up + far_right + farpt;
	Vector far_bottom_left = Vector(0,0,0) - far_up - far_right + farpt;
	Vector far_bottom_right = Vector(0,0,0) - far_up + far_right + farpt;
	
	Vector near_top_left = near_up - near_right + nearpt;
	Vector near_top_right = near_up + near_right + nearpt;
	Vector near_bottom_left = Vector(0,0,0) - near_up - near_right + nearpt;
	Vector near_bottom_right = Vector(0,0,0) - near_up + near_right + nearpt;		
	
	far_top_left += cam->position;
	far_top_right += cam->position;
	far_bottom_left += cam->position;
	far_bottom_right += cam->position;
	
	near_top_left += cam->position;
	near_top_right += cam->position;
	near_bottom_left += cam->position;
	near_bottom_right += cam->position;
	
	
	XYZ aabb1, aabb2;
	XYZ f_bounds[8];
	
	f_bounds[0] = far_top_left;
	f_bounds[1] = far_top_right;
	f_bounds[2] = far_bottom_left;
	f_bounds[3] = far_bottom_right;
	f_bounds[4] = near_top_left;
	f_bounds[5] = near_top_right;
	f_bounds[6] = near_bottom_left;
	f_bounds[7] = near_bottom_right;
	
	aabb1 = f_bounds[0];
	aabb2 = f_bounds[0];
	
	for (int i = 1; i < 8; i++)
	{
		if (aabb1.x > f_bounds[i].x) aabb1.x = f_bounds[i].x;
		if (aabb1.y > f_bounds[i].y) aabb1.y = f_bounds[i].y;
		if (aabb1.z > f_bounds[i].z) aabb1.z = f_bounds[i].z;
		
		if (aabb2.x < f_bounds[i].x) aabb2.x = f_bounds[i].x;
		if (aabb2.y < f_bounds[i].y) aabb2.y = f_bounds[i].y;
		if (aabb2.z < f_bounds[i].z) aabb2.z = f_bounds[i].z;
	}
	
#ifndef ARCH_DC	
	GLShader::clear();
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(4);
#endif
	glDisable(GL_LIGHTING);

	drawBB(aabb1,aabb2);

	glBegin(GL_LINES);
	glColor3f(1,0,1);
	//				glVertex3f(cam->position.x+nearpt.x,cam->position.y+nearpt.y,cam->position.z+nearpt.z);
	//				glVertex3f(cam->position.x+farpt.x,cam->position.y+farpt.y,cam->position.z+farpt.z);
	//		glVertex3f(cam->position.x+nearpt.x,cam->position.y+nearpt.y,cam->position.z+nearpt.z);
	//		glVertex3f(cam->position.x+farpt.x,cam->position.y+farpt.y,cam->position.z+farpt.z);
	
	farpt += cam->position;
	far_right += cam->position;
	far_up += cam->position;
	
	nearpt += cam->position;
	near_right += cam->position;
	near_up += cam->position;
	
	
	glColor3f(0,0,1);
	
//	glVertex3f(farpt.x,farpt.y,farpt.z);		
//	glVertex3f(far_right.x,far_right.y,far_right.z);
//	
//	glVertex3f(farpt.x,farpt.y,farpt.z);		
//	glVertex3f(far_up.x,far_up.y,far_up.z);
	
//	glVertex3f(farpt.x,farpt.y,farpt.z);		
//	glVertex3f(far_top_left.x,far_top_left.y,far_top_left.z);
//	
//	glVertex3f(farpt.x,farpt.y,farpt.z);		
//	glVertex3f(far_top_right.x,far_top_right.y,far_top_right.z);
//	
//	glVertex3f(farpt.x,farpt.y,farpt.z);		
//	glVertex3f(far_bottom_left.x,far_bottom_left.y,far_bottom_left.z);
//	
//	glVertex3f(farpt.x,farpt.y,farpt.z);		
//	glVertex3f(far_bottom_right.x,far_bottom_right.y,far_bottom_right.z);
	
	
//	glColor3f(0.0,1.0,0.0);
	
//	glVertex3f(nearpt.x,nearpt.y,nearpt.z);		
//	glVertex3f(near_right.x,near_right.y,near_right.z);
//	
//	glVertex3f(nearpt.x,nearpt.y,nearpt.z);		
//	glVertex3f(near_up.x,near_up.y,near_up.z);
	
//	glVertex3f(nearpt.x,nearpt.y,nearpt.z);		
//	glVertex3f(near_top_left.x,near_top_left.y,near_top_left.z);
//	
//	glVertex3f(nearpt.x,nearpt.y,nearpt.z);		
//	glVertex3f(near_top_right.x,near_top_right.y,near_top_right.z);
//	
//	glVertex3f(nearpt.x,nearpt.y,nearpt.z);		
//	glVertex3f(near_bottom_left.x,near_bottom_left.y,near_bottom_left.z);
//	
//	glVertex3f(nearpt.x,nearpt.y,nearpt.z);		
//	glVertex3f(near_bottom_right.x,near_bottom_right.y,near_bottom_right.z);

	
	glVertex3f(far_top_left.x,far_top_left.y,far_top_left.z);
	glVertex3f(near_top_left.x,near_top_left.y,near_top_left.z);
	
	glVertex3f(far_top_right.x,far_top_right.y,far_top_right.z);
	glVertex3f(near_top_right.x,near_top_right.y,near_top_right.z);
	
	glVertex3f(far_bottom_left.x,far_bottom_left.y,far_bottom_left.z);
	glVertex3f(near_bottom_left.x,near_bottom_left.y,near_bottom_left.z);
	
	glVertex3f(far_bottom_right.x,far_bottom_right.y,far_bottom_right.z);
	glVertex3f(near_bottom_right.x,near_bottom_right.y,near_bottom_right.z);
		
	glEnd();
	
	
	glColor3f(1,0,0);
	glBegin(GL_LINE_STRIP);
	
	glVertex3f(far_bottom_left.x,far_bottom_left.y,far_bottom_left.z);
	glVertex3f(far_bottom_right.x,far_bottom_right.y,far_bottom_right.z);
	glVertex3f(far_top_right.x,far_top_right.y,far_top_right.z);
	glVertex3f(far_top_left.x,far_top_left.y,far_top_left.z);
	glVertex3f(far_bottom_left.x,far_bottom_left.y,far_bottom_left.z);
	
	glEnd();
	
	
	glColor3f(0,1,0);
	glBegin(GL_LINE_STRIP);
	
	glVertex3f(near_bottom_left.x,near_bottom_left.y,near_bottom_left.z);
	glVertex3f(near_bottom_right.x,near_bottom_right.y,near_bottom_right.z);
	glVertex3f(near_top_right.x,near_top_right.y,near_top_right.z);
	glVertex3f(near_top_left.x,near_top_left.y,near_top_left.z);
	glVertex3f(near_bottom_left.x,near_bottom_left.y,near_bottom_left.z);
	
	glEnd();
	glPointSize(1);
#ifndef ARCH_DC
	glDisable(GL_LINE_SMOOTH);
#endif
#endif
#endif
}	



void SectorMap::querySectorAABB(std::set<SceneObject*> &objList, const XYZ &aabb1_in, const XYZ &aabb2_in, std::set<Light *> *lightList)
{
	SectorLoc loc_start, loc_end;
	unsigned long a,b,c;
	
	loc_start = getSector(aabb1_in);
	loc_end = getSector(aabb2_in);
	
	std::set<SceneObject *>::iterator i;
	
	for (a = loc_start.x; a <= loc_end.x; a++)
	{
		if (a > divx || a < 0) continue;
		if (map.find(a) == map.end()) continue;
		
		for (b = loc_start.y; b <= loc_end.y; b++)
		{
			if (b > divy || b < 0) continue;
			if (map[a].find(b) == map[a].end()) continue;
			
			for (c = loc_start.z; c <= loc_end.z; c++)
			{
				if (c > divz || c < 0) continue;						
				if (map[a][b].find(c) == map[a][b].end()) continue;
				
				SectorNode *tmpNode = map[a][b][c];

				for (i = tmpNode->sceneobj.begin(); i != tmpNode->sceneobj.end(); i++)
				{
					objList.insert((*i));
				}
				
				if (lightList != NULL) for (light_i = tmpNode->light.begin(); light_i != tmpNode->light.end(); light_i++)
				{					
					lightList->insert(*light_i);
				}
				
			}
		}
	}				
}


void SectorMap::process()
{
	cvrIndex a,b,c,numObjs;
	SectorLoc loc_start_last, loc_end_last;		
	SectorLoc loc_start, loc_end;
	
	numObjs = sceneobjs.size();
	
	std::set<SceneObject *>::iterator i;
	
	set<SectorNode *>::iterator active_i;
	
	
	for (active_i = active_map.begin(); active_i != active_map.end(); active_i++)
	{
		if (!(*active_i)->sceneobj.size() && !(*active_i)->light.size())
		{
			map[(*active_i)->loc.x][(*active_i)->loc.y].erase((*active_i)->loc.z);
			
			if (!map[(*active_i)->loc.x][(*active_i)->loc.y].size()) 
			{
				map[(*active_i)->loc.x].erase((*active_i)->loc.y);
				
				if (!map[(*active_i)->loc.x].size())
				{
					map.erase((*active_i)->loc.x);
				}
			}
		}
	}
	
	
	active_map.clear();
	
	for (i = sceneobjs.begin(); i != sceneobjs.end(); ++i)
	{
		if (!(*i)->active) continue;
		
		bool moved = false;
		
		XYZ bbMin,bbMax;
		XYZ bbMinTmp, bbMaxTmp;
		XYZ bbMinLast, bbMaxLast;
		SectorNode *tmpNode;
		
		//(*i)->moved();
		
		
		bbMin = (*i)->aabbMin;//getAABB(bbMin,bbMax);
		bbMax = (*i)->aabbMax;
		
		
		//(*i)->getLastAABB(bbMinLast,bbMaxLast);
		bbMinLast = (*i)->aabbMin_last;
		bbMaxLast = (*i)->aabbMax_last;
		
		moved = (((bbMin != bbMinLast) || (bbMax != bbMaxLast)) && (bbMinLast != bbMaxLast)); // (*i)->moved(); // 

		if (!moved) continue;
		
		Vector bbOverflow;
		
		bbOverflow = bbMax - bbMin;
		if (fabs(bbOverflow.x) > bounds.x || fabs(bbOverflow.y) > bounds.y || fabs(bbOverflow.z) > bounds.z)
		{
			Logger::log(LOG_ERROR,"sector overflow error");
			continue;
		}
		
		// fell outside sector map
		if (!(*i)->aabbIntersect(bbMin,bbMax,bb1,bb2,bbMinTmp,bbMaxTmp)) 
		{
			if ((*i)->aabbIntersect(bbMinLast,bbMaxLast,bb1,bb2,bbMinTmp,bbMaxTmp))
			{
				loc_start_last = getSector(bbMinLast);
				loc_end_last = getSector(bbMaxLast);
				
				for (a = loc_start_last.x; a <= loc_end_last.x; a++)
				{
					if (a > divx || a < 0) continue;
					if (map.find(a) == map.end()) continue;
					
					for (b = loc_start_last.y; b <= loc_end_last.y; b++)
					{
						if (b > divy || b < 0) continue;
						if (map[a].find(b) == map[a].end()) continue;
						
						for (c = loc_start_last.z; c <= loc_end_last.z; c++)
						{
							if (c > divz || c < 0) continue;						
							if (map[a][b].find(c) == map[a][b].end()) continue;
							//printf("out!");
							
							// REMOVE COMPLETELY, OUTSIDE BOUNDS
							
							tmpNode = map[a][b][c];

							active_map.insert(tmpNode);
							
							tmpNode->sceneobj.erase((*i));
							
//							if (!map[a][b][c].sceneobj.size()) map[a][b].erase(c);
//							if (!map[a][b].size()) map[a].erase(b);
//							if (!map[a].size()) map.erase(a);
							
						}
					}
				}				
			}
			
			// DEACTIVATE
			(*i)->active = false;
			
			continue;
		}
		
		
		
		
		if (moved)	// else no change
		{
			loc_start_last = getSector(bbMinLast);
			loc_end_last = getSector(bbMaxLast);
			
			loc_start = getSector(bbMin);
			loc_end = getSector(bbMax);

//			glColor3f(1.0,0.0,0.0);
			
			
//			drawBB(bbMinLast,bbMaxLast);												
			
//			glColor3f(0.0,0.5,0.8);
			
//			drawBB(bbMin,bbMax);
			
			XYZ bbMinI, bbMaxI;
			
			
//			glColor3f(1.0,0.0,0.0);
			
			// scan through previous sectors and discard anything that isn't in the new sector range
			for (a = loc_start_last.x; a <= loc_end_last.x; a++)
			{
				if (a > divx || a < 0) continue;
				if (map.find(a) == map.end()) continue;
				
				for (b = loc_start_last.y; b <= loc_end_last.y; b++)
				{
					if (b > divy || b < 0) continue;
					if (map[a].find(b) == map[a].end()) continue;
					
					for (c = loc_start_last.z; c <= loc_end_last.z; c++)
					{
						if (map[a][b].find(c) == map[a][b].end()) continue;

						tmpNode = map[a][b][c];
						active_map.insert(tmpNode);

						if (c > divz || c < 0) continue;						
						if (a >= loc_start.x && a <= loc_end.x && b >= loc_start.y && b <= loc_end.y && c >= loc_start.z && c <= loc_end.z) continue;
					

						//						XYZ bbpos = XYZ(a*(bounds.x/(float)divx),b*(bounds.y/(float)divy),c*(bounds.z/(float)divz));		
						//						bbpos += bb1;
						//						drawBB(bbpos,bbpos+bbsec);
						
						// REMOVE SECTOR
						// sector is no longer occupied, remove
						map[a][b][c]->sceneobj.erase((*i));
						
						//if (!map[a][b][c].sceneobj.size()) map[a][b].erase(c);						
					}
//					if (!map[a][b].size()) map[a].erase(b);
				}
//				if (!map[a].size()) map.erase(a);
			}
				
			// scan thru and add anything that wasn't in the last sector range
			for (a = loc_start.x; a <= loc_end.x; a++)
			{
				if (a > divx || a < 0) continue;				
				
				for (b = loc_start.y; b <= loc_end.y; b++)
				{
					if (b > divy || b < 0) continue;
					
					for (c = loc_start.z; c <= loc_end.z; c++)
					{
						 if (c > divz || c < 0) continue;
						


						if (a >= loc_start_last.x && a <= loc_end_last.x && b >= loc_start_last.y && b <= loc_end_last.y && c >= loc_start_last.z && c <= loc_end_last.z)
						{
							// NCHG
							// falls within last bounds, no change
							//							glColor3f(0.0,0.0,1.0);
						}
						else
						{
							// NEW SECTOR
							// new range, outside of last range, add to sector
							if (map[a][b].find(c) == map[a][b].end())
							{
								map[a][b][c] = new SectorNode();
							}
							
							tmpNode = map[a][b][c];

							tmpNode->sceneobj.insert((*i));
							tmpNode->loc.x=a;
							tmpNode->loc.y=b;
							tmpNode->loc.z=c;

							active_map.insert(tmpNode);
						}
						
						//					XYZ bbpos = XYZ(a*(bounds.x/(float)divx),b*(bounds.y/(float)divy),c*(bounds.z/(float)divz));		
						//					bbpos += bb1;
						//					drawBB(bbpos,bbpos+bbsec);
						
					}
				}
			}		
		
		//		drawBB(bbMin,bbMax);
		}
	
	
		if (moved && bbMinLast==bbMaxLast)
		{
			(*i)->aabbMin_last = bbMin;
			(*i)->aabbMax_last = bbMax;
		}
	}
	
	// process static cast objects here
	
	
	//printf("objs:%d\n",sceneobjs.size());
}



void SectorMap::drawBB(const XYZ &bb1, const XYZ &bb2)
{
#if !defined(OPENGL_ES) && !defined(ARCH_PSP)
	XYZ bounds;
	
	bounds = bb2;
	bounds -= bb1;
	
	glBegin(GL_LINES);
		glVertex3f(bb1.x,bb1.y,bb1.z);
		glVertex3f(bb1.x,bb1.y,bb1.z+bounds.z);

		glVertex3f(bb1.x+bounds.x,bb1.y,bb1.z);
		glVertex3f(bb1.x+bounds.x,bb1.y,bb1.z+bounds.z);

		glVertex3f(bb1.x,bb1.y+bounds.y,bb1.z);
		glVertex3f(bb1.x,bb1.y+bounds.y,bb1.z+bounds.z);

		glVertex3f(bb1.x+bounds.x,bb1.y+bounds.y,bb1.z);
		glVertex3f(bb1.x+bounds.x,bb1.y+bounds.y,bb1.z+bounds.z);



		glVertex3f(bb1.x,bb1.y,bb1.z);
		glVertex3f(bb1.x,bb1.y+bounds.y,bb1.z);

		glVertex3f(bb1.x+bounds.x,bb1.y,bb1.z);
		glVertex3f(bb1.x+bounds.x,bb1.y+bounds.y,bb1.z);

		glVertex3f(bb1.x+bounds.x,bb1.y,bb1.z+bounds.z);
		glVertex3f(bb1.x+bounds.x,bb1.y+bounds.y,bb1.z+bounds.z);

		glVertex3f(bb1.x,bb1.y,bb1.z+bounds.z);
		glVertex3f(bb1.x,bb1.y+bounds.y,bb1.z+bounds.z);



		glVertex3f(bb1.x,bb1.y,bb1.z);
		glVertex3f(bb1.x+bounds.x,bb1.y,bb1.z);

		glVertex3f(bb1.x,bb1.y+bounds.y,bb1.z);
		glVertex3f(bb1.x+bounds.x,bb1.y+bounds.y,bb1.z);

		glVertex3f(bb1.x,bb1.y+bounds.y,bb1.z+bounds.z);
		glVertex3f(bb1.x+bounds.x,bb1.y+bounds.y,bb1.z+bounds.z);

		glVertex3f(bb1.x,bb1.y,bb1.z+bounds.z);
		glVertex3f(bb1.x+bounds.x,bb1.y,bb1.z+bounds.z);

	glEnd();
#endif
}


void SectorMap::debugDraw()
{
#if !defined(ARCH_PSP) && !defined(OPENGL_ES)

#ifndef ARCH_DC
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPointSize(2);
	glEnable(GL_POINT_SMOOTH);
#endif
	
	glDepthMask(false);
	glDisable(GL_TEXTURE_2D);
	glColor3f(0,1,0);
	glDisable(GL_LIGHTING);

	drawBB(bb1,bb2);
	
	std::map<cvrIndex, std::map<cvrIndex, std::map<cvrIndex, SectorNode *, ltulong>, ltulong>, ltulong>::iterator map_a;
	std::map<cvrIndex, std::map<cvrIndex, SectorNode *, ltulong>, ltulong>::iterator map_b;
	std::map<cvrIndex, SectorNode *, ltulong>::iterator map_c;
	
	for (map_a = map.begin(); map_a != map.end(); map_a++)
	{
		for (map_b = (*map_a).second.begin(); map_b != (*map_a).second.end(); map_b++)
		{
			for (map_c = (*map_b).second.begin(); map_c != (*map_b).second.end(); map_c++)
			{
				glColor3f((*map_c).second->sceneobj.size()/10.0f,(*map_c).second->sceneobj.size()/20.0f,0.5f+(*map_c).second->sceneobj.size()/5.0f);

				if (active_map.find((*map_c).second) != active_map.end())
					glColor3f(1,0,0);
				else
					glColor3f(0,0,1);

//				if (last_cast.find((*map_c).second) != last_cast.end())
//					glColor3f(1.0,0,1.0);
				
				XYZ bbpos = XYZ((*map_a).first*(bounds.x/(float)divx),(*map_b).first*(bounds.y/(float)divy),(*map_c).first*(bounds.z/(float)divz));		
				bbpos += bb1;
				drawBB(bbpos,bbpos+bbsec);					
			}
		}
	}
	
	
//	for (static_light_i = static_light.begin(); static_light_i != static_light.end(); static_light_i++)
//	{
//		for (lightnode_i = (*static_light_i).second.begin(); lightnode_i != (*static_light_i).second.end(); lightnode_i++)
//		{
//			glColor3f(0,1,0);
//
//			XYZ bbpos = XYZ((*lightnode_i)->loc.x*(bounds.x/(float)divx),(*lightnode_i)->loc.y*(bounds.y/(float)divy),(*lightnode_i)->loc.z*(bounds.z/(float)divz));		
//			bbpos += bb1;
//			drawBB(bbpos,bbpos+bbsec);					
//
//		}
//	}
	
	
//	std::set<SectorNode *>::iterator last_cast_i;
//
//	for (last_cast_i = last_cast.begin(); last_cast_i != last_cast.end(); last_cast_i++)
//	{
//		glColor3f(1.0,0.0,1.0);
//		
//		XYZ bbpos = XYZ((*last_cast_i)->loc.x*(bounds.x/(float)divx),(*last_cast_i)->loc.y*(bounds.y/(float)divy),(*last_cast_i)->loc.z*(bounds.z/(float)divz));
//		bbpos += bb1;
//		drawBB(bbpos,bbpos+bbsec);					
//	}
	
	
	glPointSize(1);
#ifndef ARCH_DC
	glPopAttrib();
#endif
#endif
}

void SectorMap::bind(SceneObject *sceneobj_in)
{
	sceneobjs.insert(sceneobj_in);
	
	unsigned int a,b,c;
	
	sceneobj_in->setMatrixLock(false);
	sceneobj_in->clearTransform();
	sceneobj_in->calcAABB();
	sceneobj_in->aabbMin_last=XYZ(0,0,0);
	sceneobj_in->aabbMax_last=XYZ(0,0,0);
//	sceneobj_in->movedReset();
	
	SectorLoc loc_start = getSector(sceneobj_in->aabbMin);
	SectorLoc loc_end = getSector(sceneobj_in->aabbMin);
	
	SectorNode *tmpNode;
	
	// add sectors
	for (a = loc_start.x; a <= loc_end.x; a++)
	{
		if (a > divx || a < 0) continue;				
		
		for (b = loc_start.y; b <= loc_end.y; b++)
		{
			if (b > divy || b < 0) continue;
			
			for (c = loc_start.z; c <= loc_end.z; c++)
			{
				if (c > divz || c < 0) continue;
								
					// NEW SECTOR
					if (map[a][b].find(c) == map[a][b].end())
					{
						map[a][b][c] = new SectorNode();
					}
					
					tmpNode = map[a][b][c];
					
					tmpNode->sceneobj.insert(sceneobj_in);
					tmpNode->loc.x=a;
					tmpNode->loc.y=b;
					tmpNode->loc.z=c;
					
					active_map.insert(tmpNode);
				}
			}
		}
	
//	sceneobj_in->movedForce();
//	sceneobj_in->transformBegin(false,false,true);
//	sceneobj_in->transformEnd();
}

void SectorMap::unbind(SceneObject *sceneobj_in)
{
	cvrIndex a,b,c;
	SectorLoc loc_start_last, loc_end_last;		

	XYZ bbMin,bbMax;
	XYZ bbMinTmp, bbMaxTmp;
	XYZ bbMinLast, bbMaxLast;
	
	//sceneobj_in->getAABB(bbMin,bbMax);
	bbMin = sceneobj_in->aabbMin;
	bbMax = sceneobj_in->aabbMax;
	
	Vector bbOverflow;
	
	bbOverflow = bbMax - bbMin;
	
	if (fabs(bbOverflow.x) > bounds.x || fabs(bbOverflow.y) > bounds.y || fabs(bbOverflow.z) > bounds.z)
	{
		Logger::log(LOG_ERROR,"sector overflow error");
		sceneobjs.erase(sceneobj_in);
		return;
	}
	
	// fell outside sector map

	loc_start_last = getSector(bbMin);
	loc_end_last = getSector(bbMax);
	
	for (a = loc_start_last.x; a <= loc_end_last.x; a++)
	{
		if (a > divx || a < 0) continue;
		
		for (b = loc_start_last.y; b <= loc_end_last.y; b++)
		{
			if (b > divy || b < 0) continue;
			
			for (c = loc_start_last.z; c <= loc_end_last.z; c++)
			{
				if (c > divz || c < 0) continue;						
				
				if (map.find(a) == map.end()) continue;
				if (map[a].find(b) == map[a].end()) continue;
				if (map[a][b].find(c) == map[a][b].end()) continue;
				
				map[a][b][c]->sceneobj.erase(sceneobj_in);
				
				if (!map[a][b][c]->sceneobj.size() && !map[a][b][c]->light.size()) 
				{
					delete (map[a][b][c]);
					map[a][b].erase(c);
				}
				if (!map[a][b].size()) map[a].erase(b);
				if (!map[a].size()) map.erase(a);
				
			}
		}
	}				
	
	sceneobjs.erase(sceneobj_in);
}


bool SectorMap::lightActivity(Light *light_in)
{
	for (lightnode_i = static_light[light_in].begin(); lightnode_i != static_light[light_in].end(); lightnode_i++)
	{	
		if (active_map.find(*lightnode_i) != active_map.end())
		{
			return true;
		}
	}
	
	return false;
}

void SectorMap::bind(Light *light_in)
{
	std::vector<SectorLoc *> tmp_cast;
	std::vector<SectorLoc *>::iterator tmp_cast_i;

	switch (light_in->type)
	{
		case LIGHT_SPOT:			
			castFrustum(tmp_cast,light_in->position,light_in->viewMatrix,light_in->cutoff,light_in->nearclip,light_in->farclip,1.0f);
		break;			
		case LIGHT_POINT:			
			castSphere(tmp_cast,light_in->position,light_in->cutoff);
		break;			
		case LIGHT_DIRECTIONAL:
			global_light.insert(light_in);
		break;
		case LIGHT_AREA:
			global_light.insert(light_in);
		break;
	}

	for (tmp_cast_i = tmp_cast.begin(); tmp_cast_i != tmp_cast.end(); tmp_cast_i++)
	{	
		SectorLoc *loc = (*tmp_cast_i);
		SectorNode *sec_node;
		
		if (map[loc->x][loc->y].find(loc->z) == map[loc->x][loc->y].end())
		{
			sec_node = new SectorNode;
			sec_node->loc.x = loc->x;
			sec_node->loc.y = loc->y;
			sec_node->loc.z = loc->z;					
			map[loc->x][loc->y][loc->z] = sec_node;
		}
		else
		{
			sec_node = map[loc->x][loc->y][loc->z];
		}
		
		sec_node->light.insert(light_in);
		static_light[light_in].insert(sec_node);
	}
	
	disposeStaticCast(tmp_cast);
	
}


void SectorMap::unbind(Light *light_in)
{
	
	
}

