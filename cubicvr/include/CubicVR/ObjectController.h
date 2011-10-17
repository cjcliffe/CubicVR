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

#ifndef CUBICVR_OBJECTCONTROLLER_H
#define CUBICVR_OBJECTCONTROLLER_H

#include <CubicVR/cvr_defines.h>

#include <CubicVR/GLExt.h>

#include <CubicVR/XYZ.h>
#include <CubicVR/Mesh.h>
//#include <CubicVR/Shader.h>
//#include <CubicVR/Envelope.h>

#ifdef ARCH_PSP
#include <fastmath.h>
#else
#include <math.h>
#endif

/// ObjectController -- 
class IMPEXP ObjectController
{
protected:
	XYZ scale, rotation, position, pivot, target;
//	XYZ center;	

	
	// Transform states
	bool l_init, w_init, matrix_lock, t_moved, t_bounds;
	bool is_targeted;
	
	// Parent Controller
	ObjectController *p_trans;
	
	inline void clearBounds() {	t_bounds = true; if (p_trans) p_trans->clearBounds(); }	
	
public:	
	// World and Local transform matrix
	float w_trans[16], l_trans[16];
	
	
	ObjectController();
	~ObjectController();

	void targeted(bool targeted_in);
	
	void transformBegin(bool no_pivot=false, bool no_scale=false, bool update_only=false);
	void transformEnd();		
	void transformReverseBegin();
	void transformReverseEnd();		
	
	void setMatrix(float t_trans_in[16]);
	void setMatrixLock(bool matrix_lock_state=true);

	inline XYZ &getPosition() { return position; };
	inline XYZ &getRotation() { return rotation; };
	inline XYZ &getScale() { return scale; };
	inline XYZ &getPivot() { return pivot; };
	inline XYZ &getTarget() { return target; };
	
	inline void setPosition(const XYZ &pos_in) { t_bounds = true; l_init = false; position = pos_in; };
	inline void setRotation(const XYZ &rot_in) { t_bounds = true; l_init = false; rotation = rot_in; };
	inline void setScale(const XYZ &scale_in) { t_bounds = true; l_init = false; scale = scale_in; };
	inline void setPivot(const XYZ &pivot_in) { t_bounds = true; l_init = false; pivot = pivot_in; };
	inline void setTarget(const XYZ target_in) { t_bounds = true; l_init = false; target = target_in; };	
	inline void clearTarget();
	
	inline void clearTransform() { l_init = w_init = false; clearBounds(); };
	inline void clearWorldTransform() { w_init = false; clearBounds(); };

	inline bool moved() { return t_moved; }
	inline void movedForce() { t_moved = true; }
	inline void movedReset() { t_moved = false; }
	
	void getAxis(XYZ &right, XYZ &up, XYZ &forward);
	void getWorldAxis(XYZ &right, XYZ &up, XYZ &forward);
	XYZ getWorldRotation();
	XYZ getWorldPosition();	
//	XYZ getWorldScale();
};



#endif
