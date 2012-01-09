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

#include <CubicVR/Face.h>

#include <CubicVR/cvr_defines.h>

Face::Face() : mat_ref(&Material::nullMaterial), segment_id(0)
{ 

};


Face::~Face() 
{ 

};


void Face::flip()
{
	std::vector<XYZ *>::reverse_iterator r_pt_i;
	std::vector<cvrIndex>::reverse_iterator r_ptref_i;
	
	std::vector<XYZ *> points_temp;
	std::vector<cvrIndex> pointref_temp;
	
	points_temp = points;
	pointref_temp = pointref;
		
	points.clear();
	pointref.clear();
		
	for (r_pt_i = points_temp.rbegin(); r_pt_i != points_temp.rend(); r_pt_i++)
	{
		points.push_back(*r_pt_i);
	}

	for (r_ptref_i = pointref_temp.rbegin(); r_ptref_i != pointref_temp.rend(); r_ptref_i++)
	{
		pointref.push_back(*r_ptref_i);
	}
	
}

void Face::calcNormal(Vector &norm_out)
{
	float v1[3],v2[3];
	
	if (points.size() < 3) return;	/* Need a minimum of 3 points to return a valid normal */

	v1[0] = points[0]->x - points[1]->x;
	v1[1] = points[0]->y - points[1]->y;
	v1[2] = points[0]->z - points[1]->z;

	v2[0] = points[1]->x - points[2]->x;
	v2[1] = points[1]->y - points[2]->y;
	v2[2] = points[1]->z - points[2]->z;
	
	norm_out.x = v1[1]*v2[2] - v1[2]*v2[1];
	norm_out.y = v1[2]*v2[0] - v1[0]*v2[2];
	norm_out.z = v1[0]*v2[1] - v1[1]*v2[0];
};


/* calculate normal vector and unit normal vector for this face  */
void Face::calcFaceNormal()
{
	// calculate the face normal
	calcNormal(face_normal);
	face_normal.makeUnit();
}



float Face::planeDistanceTo(XYZ &v_test)
{

//	XYZ P = v_test;
//	XYZ Q = *points[0];
//	Vector v = P - Q;

	Vector w,v;
	
	calcNormal(v);
	
	w = v_test;
	w -= *points[0];
	
	return v.dotProduct(w)/v.magnitude();
}


bool Face::triangleDistanceTest(XYZ &v_test,float dist_test)
{
	float d;
	XYZ t,u,ret;
	Vector v;
	
	if (face_normal.x == 0 && face_normal.y == 0 && face_normal.z == 0) {
		calcFaceNormal();
	}
	
	d = planeDistanceTo(v_test);
	
	v = face_normal;
	v.makeUnit();
	v *= d;
	
	t = v_test;
	t -= v;
	
	u = v_test;
	
	bool onTri = segmentIntersects(ret,t,u);
	
//	printf("ont: %d, dist: %f \n",onTri,d);
	
	if (onTri)
	{
		return (d <= dist_test);
	}
	else
	{
		return false;
	}
}




/* only works with triangles */
bool Face::segmentIntersects(XYZ &returned_intersect_point, XYZ &segment_start, XYZ &segment_end, float tolerance)
{
	cvrFloat ang_a,ang_b,ang_c;
	cvrFloat denom,mu,d;
	Vector pa,pb,pc;
	
	calcFaceNormal();
	
	d = - face_normal.x * points[0]->x - face_normal.y * points[0]->y - face_normal.z * points[0]->z;
	
	/* calculate position where the plane intersects the segment */
	denom = face_normal.x * (segment_end.x - segment_start.x) + face_normal.y * (segment_end.y - segment_start.y) + face_normal.z * (segment_end.z - segment_start.z);
	
	if (fabs(denom) < FLOAT_TOLERANCE) return false; /* point on plane is outside of line, no intersection */
	
	mu = - (d + face_normal.x * segment_start.x + face_normal.y * segment_start.y + face_normal.z * segment_start.z) / denom;
	returned_intersect_point = XYZ(
			   (float)(segment_start.x + mu * (segment_end.x - segment_start.x)),
			   (float)(segment_start.y + mu * (segment_end.y - segment_start.y)),
			   (float)(segment_start.z + mu * (segment_end.z - segment_start.z))
			   );
	
	if (mu < 0 || mu > 1) return false;
	
	pa = returned_intersect_point - *points[0];
	pb = returned_intersect_point - *points[1];
	pc = returned_intersect_point - *points[2];
	
	pa.makeUnit();   pb.makeUnit();   pc.makeUnit();
	
	ang_a = pa.x*pb.x + pa.y*pb.y + pa.z*pb.z;
	ang_b = pb.x*pc.x + pb.y*pc.y + pb.z*pc.z;
	ang_c = pc.x*pa.x + pc.y*pa.y + pc.z*pa.z;
	
	/* check to see if angles add up to 2*PI, if so the point lies within the triangle */
	if (fabs((acos(ang_a) + acos(ang_b) + acos(ang_c)) - M_TWO_PI) > tolerance) return false;
	
	return true;
}





/* only works with triangles */
bool Face::onTriangle(XYZ &p_test, float tolerance)
{
	cvrFloat ang_a,ang_b,ang_c;
	Vector pa,pb,pc;
	
	pa = p_test - *points[0];
	pb = p_test - *points[1];
	pc = p_test - *points[2];
	
	pa.makeUnit();   pb.makeUnit();   pc.makeUnit();
	
	ang_a = pa.x*pb.x + pa.y*pb.y + pa.z*pb.z;
	ang_b = pb.x*pc.x + pb.y*pc.y + pb.z*pc.z;
	ang_c = pc.x*pa.x + pc.y*pa.y + pc.z*pa.z;
	
	/* check to see if angles add up to 2*PI, if so the point lies within the triangle */
	if (fabs((acos(ang_a) + acos(ang_b) + acos(ang_c)) - (cvrFloat)M_TWO_PI) > tolerance) return false;
	
	return true;
}




/* todo: make this actually return useful information about the intersection, triangle tests only */
bool Face::intersects(Face &test_face)
{
	XYZ vtx[3];
	/* check plane intersection for each segment */
	if (test_face.segmentIntersects(vtx[0],*points[0],*points[1])) return true;
	if (test_face.segmentIntersects(vtx[1],*points[1],*points[2])) return true;
	if (test_face.segmentIntersects(vtx[2],*points[2],*points[0])) return true;
	
	return false;
}


void Face::copyUV(Face &dst, unsigned int dstPointNum, unsigned int srcPointNum)
{
	unsigned int i;
	
	if (dst.uv.size() != uv.size())
	{
		dst.uv.resize(uv.size());
	}
	
	for (i = 0; i < dst.uv.size(); i++)
	{
		if (dst.uv[i].size() < dstPointNum+1)
		{
			dst.uv[i].resize(dstPointNum+1);
		}
	}

	for (i = 0; i < uv.size(); i++)
	{
		if (uv[i].size())
		{
			dst.uv[i][dstPointNum] = uv[i][srcPointNum];
		}
	}
};


void Face::setUV(unsigned int facePoint, UV pointUV, unsigned int uvLayer)
{
	if (!(uv.size() > uvLayer))
	{
		uv.resize(uvLayer+1);
	}
	
	if (!(uv[uvLayer].size() > facePoint))
	{
		uv[uvLayer].resize(facePoint+1);
	}
	
	uv[uvLayer][facePoint] = pointUV;
};


void Face::setUV(unsigned int facePoint, vector< UV > pointUVs)
{
	unsigned int i;
	
	for (i = 0; i < pointUVs.size(); i++)
	{
		setUV(facePoint,pointUVs[i],i);
	}
};


void Face::setColor(unsigned int facePoint, RGB &clr)
{
	if (facePoint >= point_colors.size()) point_colors.resize(facePoint);
	
	point_colors[facePoint]=clr;
}

unsigned int Face::getSegment()
{
	return segment_id;
}

void Face::setSegment(unsigned int segment)
{
	segment_id = segment;
}


//#ifndef OPENGL_ES|ARCH_PSP
//
//// http://wiki.gamedev.net/index.php/OpenGL:Tutorials:GLSL_Bump_Mapping
//
//#define ROUNDOFF(x) ((x>=-0.001f&&x<=0.001f)?0:x)
//void Face::FindInvTBN(XYZ Vertices[3], UV TexCoords[3], Vector & InvNormal, Vector & InvBinormal, Vector & InvTangent) 
//{
//	/* Calculate the vectors from the current vertex
//	 to the two other vertices in the triangle */
//
//	XYZ v2v1 = Vertices[0] - Vertices[2];
//	XYZ v3v1 = Vertices[1] - Vertices[2];
//
//	//Calculate the "direction" of the triangle based on texture coordinates.
//
//	// Calculate c2c1_T and c2c1_B
//	float c2c1_T = TexCoords[0].u - TexCoords[2].u;
//	float c2c1_B = TexCoords[0].v - TexCoords[2].v;
//
//	// Calculate c3c1_T and c3c1_B
//	float c3c1_T = TexCoords[1].u - TexCoords[2].v;
//	float c3c1_B = TexCoords[1].u - TexCoords[2].v;
//
//	//Look at the references for more explanation for this one.
//	float fDenominator = c2c1_T * c3c1_B - c3c1_T * c2c1_B;  
//
//	if (ROUNDOFF(fDenominator) == 0.0f) 
//	{
//		/* We won't risk a divide by zero, so set the tangent matrix to the
//		 identity matrix */
//		InvTangent = XYZ(1.0f, 0.0f, 0.0f);
//		InvBinormal = XYZ(0.0f, 1.0f, 0.0f);
//		InvNormal = XYZ(0.0f, 0.0f, 1.0f);
//	}
//	else
//	{            
//		// Calculate the reciprocal value once and for all (to achieve speed)
//		float fScale1 = 1.0f / fDenominator;
//		
//		/* Time to calculate the tangent, binormal, and normal.
//		 Look at S�ren�s article for more information. */
//		Vector T, B, N;
//		T = XYZ((c3c1_B * v2v1.x - c2c1_B * v3v1.x) * fScale1,
//					 (c3c1_B * v2v1.y - c2c1_B * v3v1.y) * fScale1,
//					 (c3c1_B * v2v1.z - c2c1_B * v3v1.z) * fScale1);
//		
//		B = XYZ((-c3c1_T * v2v1.x + c2c1_T * v3v1.x) * fScale1,
//					 (-c3c1_T * v2v1.y + c2c1_T * v3v1.y) * fScale1,
//					 (-c3c1_T * v2v1.z + c2c1_T * v3v1.z) * fScale1);
//		
//		N = T * B; //Cross product!
//		
//		/*This is where programmers should break up the function to smooth the tangent, binormal and
//		 normal values. */
//		
//		//Look at "Derivation of the Tangent Space Matrix" for more information.
//		
//		float fScale2 = 1.0f / ((T.x * B.y * N.z - T.z * B.y * N.x) + 
//								(B.x * N.y * T.z - B.z * N.y * T.x) + 
//								(N.x * T.y * B.z - N.z * T.y * B.x));
//		InvTangent = XYZ((B * N).x * fScale2,
//					   ((N * -1.0f) * T).x * fScale2,
//					   (T * B).x * fScale2);
//		InvTangent.makeUnit();
//		
//		InvBinormal = XYZ(((B * -1.0f) * N).y * fScale2,
//						(N  * T).y * fScale2,
//						((T * -1.0f) * B).y * fScale2);
//		InvBinormal.makeUnit();
//		
//		InvNormal = XYZ((B * N).z * fScale2,
//					  ((N * -1.0f) * T).z * fScale2,
//					  (T * B).z * fScale2);
//		InvNormal.makeUnit();			
//	}
//}
//	
//void Face::calcTangentBinormal()
//{
//	if (points.size() < 3) return;
//	
//	XYZ Verticies[3];
//	UV TexCoords[3];
//	
//	if (uv.size()) if (uv[0].size() >= 3)
//	{
//		for (int i = 0; i < 3; i++)
//		{
//			TexCoords[i] = uv[0][i];
//		}
//	}
//	
//	for (int i = 0; i < 3; i++)
//	{
//		Verticies[i] = *points[i];
//	}
//	
//	FindInvTBN(Verticies,TexCoords,invNormal,binormal,tangent);
//}
//
//#endif
