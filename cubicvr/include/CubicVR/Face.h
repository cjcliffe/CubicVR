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

#ifndef CUBICVR_FACE_H
#define CUBICVR_FACE_H

/* need to allow for floating point inacuracies */
#define FLOAT_TOLERANCE 0.1f


#include <CubicVR/cvr_defines.h>

#include <vector>

#include <CubicVR/XYZRGB.h>
#include <CubicVR/Vector.h>
#include <CubicVR/Material.h>
#include <CubicVR/UV.h>

using namespace std;


class IMPEXP Face
{
public:
	std::vector<XYZ *> points;
	std::vector<cvrIndex> pointref;
	std::vector< std::vector<UV> > uv;
	std::vector<Vector> point_normals;
	std::vector<RGB> point_colors;

	Material *mat_ref;
	Vector face_normal;
	
	unsigned int segment_id;

//#ifndef OPENGL_ES|ARCH_PSP
//	Vector invNormal;
//	Vector binormal;
//	Vector tangent;
//#endif
	
	Face();
	~Face();
	
	void calcNormal(Vector &norm_out);
	void calcFaceNormal();
	void flip();

	float planeDistanceTo(XYZ &v_test);
	bool triangleDistanceTest(XYZ &v_test,float dist_test);

	bool segmentIntersects(XYZ &returned_intersect_point, XYZ &segment_start, XYZ &segment_end, float tolerance=FLOAT_TOLERANCE);
	bool intersects(Face &test_face);
	bool onTriangle(XYZ &p_test, float tolerance=FLOAT_TOLERANCE);

	void copyUV(Face &dst, unsigned int dstPointNum, unsigned int srcPointNum);
	
	void setUV(unsigned int facePoint, UV pointUV, unsigned int uvLayer=0);
	void setUV(unsigned int facePoint, vector< UV > pointUVs);

	void setColor(unsigned int facePoint, RGB &clr);
	
	void setSegment(unsigned int segment);
	unsigned int getSegment();
	
//#ifndef OPENGL_ES|ARCH_PSP
//	void FindInvTBN(XYZ Vertices[3], UV TexCoords[3], Vector & InvNormal, Vector & InvBinormal, Vector & InvTangent);
//	void calcTangentBinormal();
//#endif
};


#endif

