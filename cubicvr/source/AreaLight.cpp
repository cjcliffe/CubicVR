/*
 *  AreaLight.cpp
 *  CubicVR

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


#include <CubicVR/AreaLight.h>
#include <CubicVR/Camera.h>

AreaLight::AreaLight(Camera *cam_in, unsigned int near_map_res, unsigned int far_map_res, float near_map_range, float far_map_range, float ceiling_height, float floor_height)
{
	cam = cam_in;
	near_res = near_map_res;
	far_res = far_map_res;
	near_range = near_map_range;
	far_range = far_map_range;
	ceiling = ceiling_height;
	floor = floor_height;
	
	oLight[0].ortho_size = near_map_range;
	oLight[0].setType(LIGHT_AREA);
	oLight[1].setType(LIGHT_AREA);
	oLight[2].setType(LIGHT_AREA);
	
	oLight[0].setTargeted(true);
	oLight[1].setTargeted(true);
	oLight[2].setTargeted(true);

	oLight[0].setClipping(0.0,1.0);
	oLight[1].setClipping(0.0,1.0);
	oLight[2].setClipping(0.0,1.0);
	
	if (near_map_res && far_map_res)
	{		
		oLight[0].setShadow(near_map_res,"null");
		oLight[1].setShadow(far_map_res,"null");
		oLight[2].setShadow(far_map_res,"null");	
		
		has_shadow = true;
	}
	else
	{	
		has_shadow = false;
	}
	
	setType(LIGHT_AREA);
	setTargeted(true);
	shadow_vec = XYZ(40,40,0);
}

void AreaLight::setShadowVector(XYZ vec)
{ 
	shadow_vec = vec; 
}	

void AreaLight::update()
{
//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
//	glLoadIdentity();
//	
//	cam->setup();
	float dist = 0.0f;
	float sx = cam->distSlopeX(1.0f);
	float far_clip_range = far_range;
	//	float sy = cam->distSlopeY(1.0);
	
	for (int i = 0; i < 3; i++) 
	{
		oLight[i].diffuse = diffuse;
		oLight[i].specular = specular;
		oLight[i].ambient = ambient;
	}
	
	Vector vview = Vector(cam->position,cam->target);
	vview.y = 0;
	vview.makeUnit();
	
	Vector vleft = (vview*Vector(0,1,0));
	vleft.makeUnit();
	
	float fwd_ang = -atan2l(vview.z,vview.x);
	
	dist = ((oLight[0].ortho_size/2.0)*fabs(sx))-(oLight[0].ortho_size/2.0);
	
	if (dist < (oLight[0].ortho_size/3.0)/2.0) dist = (oLight[0].ortho_size/3.0)/2.0;
	
	vview *= dist;
	
	Vector l_ofs(shadow_vec);
	Vector l_vec(l_ofs);
	l_vec.makeUnit();
	
	float ofs_ang = -atan2l(l_ofs.x,l_ofs.z);
	
	fwd_ang-=ofs_ang;
	
	oLight[0].position = cam->position+vview+l_ofs;
	oLight[0].position.y = ceiling;
	oLight[0].target = cam->position+vview;
	oLight[0].target.y = 0;
	oLight[0].rotation.z = RADTODEG(fwd_ang);
	
	position = oLight[0].position;
	target = oLight[0].target;
	
	dist += (oLight[0].ortho_size/2.0);
	
	float split_ortho_width = ((dist+far_clip_range)*fabs(sx));
	
	dist += split_ortho_width/2.0;
	
	//	printf("[w: %.2f, d: %.2f, sx: %2f]\n",split_ortho_width,dist,fabs(sx));
	
	oLight[1].ortho_size = split_ortho_width;
	oLight[2].ortho_size = split_ortho_width;
	
	vview = Vector(cam->position,cam->target);
	vview.y = 0;
	vview.makeUnit();
	vview *= dist;
	
	oLight[1].position = cam->position+vview+l_ofs+(vleft*(oLight[1].ortho_size/2.0));
	oLight[1].position.y = ceiling;
	oLight[1].target = cam->position+vview+(vleft*(oLight[1].ortho_size/2.001));
	oLight[1].target.y = 0;
	oLight[1].rotation.z = RADTODEG(fwd_ang);
	
	
	oLight[2].position = cam->position+vview+l_ofs+(vleft*-(oLight[2].ortho_size/2.0));
	oLight[2].position.y = ceiling;
	oLight[2].target = cam->position+vview+(vleft*-(oLight[2].ortho_size/2.001));
	oLight[2].target.y = 0;
	oLight[2].rotation.z = RADTODEG(fwd_ang);
	
	
	oLight[1].ortho_size = split_ortho_width*1.2;
	oLight[2].ortho_size = split_ortho_width*1.2;

	oLight[0].nearclip=oLight[1].nearclip=oLight[2].nearclip=0.0;
	oLight[0].farclip=oLight[1].farclip=oLight[2].farclip=1.0;

	
	XYZ aabb1,aabb2;

	int i;

	nearclip = oLight[0].nearclip;
	farclip = oLight[0].farclip;

	for (i = 0; i < 3; i++)
	{
		Camera::orthoNearBounds(oLight[i].position, oLight[i].ortho_size, oLight[i].ortho_size, oLight[i].projectionMatrix, oLight[i].viewMatrix, oLight[i].nearclip, aabb1, aabb2);
		
		if (aabb1.y < ceiling)
		{
			oLight[i].nearclip-=(ceiling-aabb1.y)/l_vec.y;
		}
	}
	
	for (i = 0; i < 3; i++)
	{
		Camera::orthoFarBounds(oLight[i].position, oLight[i].ortho_size, oLight[i].ortho_size, oLight[i].projectionMatrix, oLight[i].viewMatrix, oLight[i].farclip, aabb1, aabb2);
		
		if (aabb2.y > floor)
		{
			oLight[i].farclip+=(aabb2.y-floor)/l_vec.y;
		}
	}
	
//	float clipDist = farclip-nearclip;
//	GLShader::defaultShader.setShaderValue("lightClip",clipDist);
	
	
//	glPopMatrix();
}