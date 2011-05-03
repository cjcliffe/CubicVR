/*
 *  ProceduralObject.cpp
 *  Builder
 *
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
 *
 */

#include <CubicVR/ProceduralObject.h>

ProceduralObject::ProceduralObject(): Object(), needs_regen(true), generated(false), mat(0)
{
};

void ProceduralObject::regenerate()
{
};

Material &ProceduralObject::material()
{
	return *mat;
};


void ProceduralObject::generateSphere(float radius, unsigned int hor_div, unsigned int vert_div, Material *material, bool dynamicUpdate)
{
	std::vector<XYZ> pointList;
	
	float rStart = -M_PI/2.0f, rEnd = M_PI/2.0f, step=(rEnd-rStart)/((float)vert_div), r;
	
	for (r = rStart; r <= rEnd; r+= step)
	{
		pointList.push_back(XYZ(cos(r)*radius,sin(r)*radius,0));
	}
	
	pointList[0].x = 0;
	pointList[pointList.size()-1].x = 0;
	
	generateLathe(pointList,hor_div,material,dynamicUpdate);
}

void ProceduralObject::generateLathe(std::vector<XYZ> &pointList, unsigned int lathe_divisions, Material *material, bool dynamicUpdate)
{
	float i;
	unsigned int j,k,sliceNum;
	
	std::vector<XYZ>::iterator point_i;
	std::vector<std::vector<XYZ> > slices;
	
	//		curve_in.getCurve(pointList);
	//		printf("curve len:%d\n",pointList.size());
	slices.resize(lathe_divisions);
	//		printf("slices:%d\n",slices.size());
	
	
	//		if (needs_regen) regenerate();
	if (CacheShader::lockObj == this) CacheShader::lockObj = NULL;
	bool bUpdate = false;
	
	if (lastDivisions == lathe_divisions && lastPoints.size() == pointList.size() && dynamicUpdate)
	{
		bUpdate = true;
	}
	else
	{
		clean();			
	}
	
	Vector up(0,1,0);
	Vector right;
	XYZ pos;
	
	if (material)
	{
		mat = material;
	}
	else
	{
		mat = new Material();
	}		
	
	sliceNum = 0;
	
	for (i = 0; i < M_PI*2.0f; i+=((M_PI*2.0f)/(float)lathe_divisions))
	{
		if (sliceNum == slices.size()) break;
		
		//			slices[sliceNum].resize(pointList.size());
		right = Vector(cos(i),0,sin(i));
		
		for (point_i = pointList.begin(); point_i != pointList.end(); point_i++)
		{				
			pos = right * (*point_i).x + up * (*point_i).y;
			
			slices[sliceNum].push_back(pos);
		}
		
		sliceNum++;
	}
	
	if (bUpdate && buffer_state) openBuffer();
	
	for (j = 0; j < lathe_divisions; j++)
	{
		for (k = 0; k < pointList.size(); k++)
		{
			if (bUpdate)
			{
				if (pointList[k] != lastPoints[k])
				{
					*points[k+(j*(pointList.size()))] = slices[j][k];
					updatePointCache(k+(j*(pointList.size())));
				}
			}
			else
			{
				addPoint(slices[j][k]);	
			}
		}
	}
	
	
	if (!bUpdate) 
	{
		for (k = 0; k < lathe_divisions; k++)
		{			
			for (j = 0; j < pointList.size()-1; j++)
			{
				addFace();
				unsigned long pt = j+(pointList.size()*k);
				unsigned long pt_r = j+(pointList.size()*((k+1)%(lathe_divisions)));
				
				if ((XYZ)*points[pt] == (XYZ)*points[pt_r])
				{
					addFacePoint(pt+1);
					addFacePoint(pt_r+1);
					addFacePoint(pt_r);
				}
				else if ((XYZ)*points[pt+1] == (XYZ)*points[pt_r+1])
				{
					addFacePoint(pt);
					addFacePoint(pt+1);
					addFacePoint(pt_r);					
				}
				else
				{
					addFacePoint(pt);
					addFacePoint(pt+1);
					addFacePoint(pt_r+1);
					addFacePoint(pt_r);					
				}
				bindFaceMaterial(mat);
			}
		}
	}
	
	//		triangulate();
	if (!bUpdate) 
	{
		calcNormals();
	}
	else 
	{
		//			reCalcNormals();
		//			fullCopyCache(false,true);
	}
	
	
	if (bUpdate && buffer_state) closeBuffer();
	
	//		// Create a UV Mapper
	//		UVMapper myUVMapper;
	//		// Set the projection type to cubic
	//		myUVMapper.setProjection(UV_PROJECTION_CUBIC);
	//		// Match the scale of our box
	//		myUVMapper.setScale(XYZ(3.0,3.0,3.0));	
	//		
	//		// Apply the UV map to the material we bound to our box object
	//		myUVMapper.apply(*this,mat);
	
	if (!bUpdate) cache(true);
	
	lastPoints.clear();
	//		curve_in.getCurve(lastCurve);
	for (j = 0; j < pointList.size(); j++) lastPoints.push_back(pointList[j]);
	
	lastDivisions = lathe_divisions;
	generated = true;
};

void ProceduralObject::generateExtruded(Curve &curve_in)
{
	
	
};

void ProceduralObject::staticRotate(const XYZ &rot)
{
	bool cache_val = cache_state;
	
	if (cache_state) cache(false);
	
	float rotMat[16];
	
	glMatrixMode(GL_MODELVIEW);
	
	glPushMatrix();
	
	glRotatef(rot.z,0,0,1);
	glRotatef(rot.y,0,1,0);
	glRotatef(rot.x,1,0,0);
	
	glGetFloatv(GL_MODELVIEW_MATRIX , rotMat);
	
	glPopMatrix();
	
	for (unsigned long i = 0; i < points.size(); i++)
	{
		XYZ newPt;
		
		matTransform(rotMat,*points[i],newPt);
		
		*points[i] = newPt;
	}
	
	calcNormals();
	
	cache(cache_val);
}
