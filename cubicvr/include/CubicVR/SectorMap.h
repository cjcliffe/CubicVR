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

#ifndef SECTORMAP_H
#define SECTORMAP_H

#include <CubicVR/cvr_defines.h>
#include <CubicVR/SceneObject.h>
#include <CubicVR/Camera.h>
#include <CubicVR/SectorRef.h>
#include <CubicVR/SceneObject.h>
#include <CubicVR/Light.h>

#include <map>
#include <set>


class SectorMap
{
	private:
		std::map<cvrIndex, std::map<cvrIndex, std::map<cvrIndex, SectorNode *, ltulong>, ltulong>, ltulong> map;
		std::map<Light *, set<SectorNode *> > static_light;
		std::map<Light *, set<SectorNode *> >::iterator static_light_i;
		std::set<SectorNode *>::iterator lightnode_i;
		std::set<SectorNode *> active_map;
		std::set<SectorNode *> last_cast;
		std::set<SceneObject *> sceneobjs;
		std::set<Light *>::iterator light_i;
		std::set<Light *> global_light;
	
		XYZ bb1, bb2;	// bounding box co-ordinates low/high
		XYZ bounds;		// XYZ representing bounds
		XYZ bbsec;		// XYZ representing single sector span
	
		Vector right,up;
		FRUSTUM frust;
		
		unsigned int divx,divy,divz;	// sector divisions
		
	public:
		SectorMap(const XYZ &bb_pt1, const XYZ &bb_pt2, double sector_size_in);

		void queryFrustum(std::set<SceneObject*> &objList, MATRIX4X4 projMatrix, MATRIX4X4 viewMatrix, float nearClip, float farClip, const int viewport[4], std::set<Light *> *lightList=NULL);
		void queryOrthoFrustum(std::set<SceneObject*> &objList, XYZ position, float width, float height, MATRIX4X4 projMatrix, MATRIX4X4 modelMatrix, float nearClip, float farClip, const int viewport[4], std::set<Light *> *lightList=NULL);
		void querySectorAABB(std::set<SceneObject*> &objList, const XYZ &aabb1_in, const XYZ &aabb2_in, std::set<Light *> *lightList=NULL);
//		void queryFrustum(std::set<SceneObject*> &objList, XYZ &position, float modelview[16], float fov, float nearclip, float farclip, float aspect, std::set<Light *> *lightList=NULL);
		void renderFrustum(Camera *cam);
		void renderOrthoFrustum(XYZ position, float width, float height, MATRIX4X4 projMatrix, MATRIX4X4 modelMatrix, float nearClip, float farClip, const int viewport[4]);
	
		void castFrustum(std::vector<SectorLoc *> &staticCast, XYZ &position, float modelview[16], float fov, float nearclip, float farclip, float aspect);
		void castSphere(std::vector<SectorLoc *> &staticCast, XYZ &position, float radius);
		void disposeStaticCast(std::vector<SectorLoc *> &staticCast);

		bool lightActivity(Light *light_in);

		void renderFrustum(MATRIX4X4 projMatrix, MATRIX4X4 modelMatrix, float nearClip, float farClip, const int viewport[4]);
	
		SectorLoc getSector(const XYZ &pt);
		void process();
		void debugDraw();
		void bind(SceneObject *sceneobj_in);		
		void unbind(SceneObject *sceneobj_in);		

		void bind(Light *light_in);		
		void unbind(Light *light_in);		

	
		static void drawBB(const XYZ &bb1, const XYZ &bb2);
};


#endif
