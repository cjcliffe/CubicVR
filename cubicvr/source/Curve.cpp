/*
 *  Curve.cpp
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

#include <CubicVR/Curve.h>

CurveNode::CurveNode() : shape(CURVE_TCB), tension(0), continuity(0), bias(0)
{
	param[0] = 0;
	param[1] = 0;
	param[2] = 0;
	param[3] = 0;
}



CurveNode &CurveNode::operator=(const CurveNode &node_in)
{
	shape = node_in.shape;
	tension = node_in.tension;
	continuity = node_in.continuity;
	bias = node_in.bias;
	
	param[0] = node_in.param[0];
	param[1] = node_in.param[1];
	param[2] = node_in.param[2];
	param[3] = node_in.param[3];
	
	return *this;
};


Curve::Curve(): needs_regen(true), divisions(10), threshold(0) //threshold(DEGTORAD(1.0)) //, threshold(DEGTORAD(1.0)) // 
{
}

void Curve::regenerate()
{
	Envelope env_x,env_y;
	cvrFloat step_rate;
	cvrFloat j;
	
	step_rate = 1.0f/(cvrFloat)divisions;
	
	unsigned int i = 0;
	
	if (!pointList.size()) return;

	EnvelopeKey *tmpKeyX;
	EnvelopeKey *tmpKeyY;
	
	if (pointList.size()>2) if (pointList[0] == pointList[pointList.size()-1])
	{
		tmpKeyX = env_x.addKey(-1,pointList[pointList.size()-2].x);
		tmpKeyY = env_y.addKey(-1,pointList[pointList.size()-2].y);

		tmpKeyX->tension = tcbList[pointList.size()-2].tension;
		tmpKeyX->continuity = tcbList[pointList.size()-2].continuity;
		tmpKeyX->bias = tcbList[pointList.size()-2].bias;
		tmpKeyY->tension = tcbList[pointList.size()-2].tension;
		tmpKeyY->continuity = tcbList[pointList.size()-2].continuity;
		tmpKeyY->bias = tcbList[pointList.size()-2].bias;
	};
	
	for (point_i = pointList.begin(); point_i < pointList.end(); point_i++)
	{
		tmpKeyX = env_x.addKey(i,(*point_i).x);
		tmpKeyY = env_y.addKey(i,(*point_i).y);
	
		tmpKeyX->tension = tcbList[i].tension;
		tmpKeyX->continuity = tcbList[i].continuity;
		tmpKeyX->bias = tcbList[i].bias;
		tmpKeyY->tension = tcbList[i].tension;
		tmpKeyY->continuity = tcbList[i].continuity;
		tmpKeyY->bias = tcbList[i].bias;
		
		i++;
	}

	if (pointList.size()>2) if (pointList[0] == pointList[pointList.size()-1])
	{
		tmpKeyX = env_x.addKey(i,pointList[1].x);
		tmpKeyY = env_y.addKey(i,pointList[1].y);

		tmpKeyX->tension = tcbList[1].tension;
		tmpKeyX->continuity = tcbList[1].continuity;
		tmpKeyX->bias = tcbList[1].bias;
		tmpKeyY->tension = tcbList[1].tension;
		tmpKeyY->continuity = tcbList[1].continuity;
		tmpKeyY->bias = tcbList[1].bias;
		
	};
	
	pointListGen.clear();
	
	j = 0;

	Vector lVec,cVec;
	XYZ lastEval,eval;
	
	for (i = 0; i < pointList.size()*divisions; i++)
	{
		eval = XYZ(env_x.evaluate(j),env_y.evaluate(j),0);
		
		if (threshold)
		{
			if (i > 1 && i < ((pointList.size()*divisions)-1))
			{
				cVec = eval - lastEval;
				
				if (fabs(cVec.angle(lVec)) > threshold || (i % divisions) == 0)
				{
					pointListGen.push_back(eval);	

					lVec = cVec;
					lastEval = eval;
				}
			}
			else
			{
				pointListGen.push_back(eval);
				if (i>0) lVec = eval - lastEval;
				else lastEval = eval;
			}
		}
		else
		{
			pointListGen.push_back(XYZ(env_x.evaluate(j),env_y.evaluate(j),0));
		}
		
		j+=step_rate;
	}
	
	needs_regen = false;
}

int Curve::numPoints()
{
	return pointList.size();
}


void Curve::clear()
{
	pointList.clear();
	pointListGen.clear();
	needs_regen = true;
}


int Curve::addPoint(const XYZ &pt)
{	
	pointList.push_back(pt);
	tcbList.resize(pointList.size());
	
	needs_regen = true;
	
	return pointList.size()-1;
}

void Curve::deletePoint(unsigned int ptNum)
{
	pointList.erase(pointList.begin()+ptNum);
	tcbList.erase(tcbList.begin()+ptNum);	
	
	needs_regen = true;
}

void Curve::setAdaptive(float angle_threshold)
{
	threshold = angle_threshold;
}

bool Curve::empty()
{
	return pointList.empty();
}

XYZ &Curve::getPoint(unsigned int ptNum)
{
	return pointList[ptNum];
}

void Curve::setNode(unsigned int ptNum, CurveNode &node_in)
{
	tcbList[ptNum] = node_in;
	needs_regen = true;
	
}

CurveNode &Curve::getNode(unsigned int ptNum)
{
	return tcbList[ptNum];
}

void Curve::setPoint(unsigned int ptNum, const XYZ &pt)
{
	//if (ptNum == -1) return;
	if (ptNum >= pointList.size()) return;
	pointList[ptNum]=pt;
	needs_regen = true;
}

void Curve::getCurve(std::vector<XYZ>&curve_out)
{
	if (needs_regen) regenerate();
	
	for (pointGen_i = pointListGen.begin(); pointGen_i != pointListGen.end(); pointGen_i++)
	{
		curve_out.push_back((*pointGen_i));
	}
}


void Curve::draw()
{
#if !defined(OPENGL_ES) && !defined(ARCH_PSP)

	if (needs_regen) regenerate();

	glLineWidth(3);
	glColor3f(0.8f,0.6f,0);
	Pencil::drawLine(pointList);
	glColor3f(0,1,0);
	Pencil::drawLine(pointListGen);
	
	glLineWidth(1);
	glColor3f(0.2f,0.8f,1);
	Pencil::drawCircles(pointList,0.250f);
	glColor3f(0,1,0);
	Pencil::drawCircles(pointListGen,0.075f);
#endif
}

void Curve::drawExtruded()
{
	if (needs_regen) regenerate();
#if !defined(OPENGL_ES) && !defined(ARCH_PSP)

	Pencil::drawExtruded(pointListGen);
#endif
}

#if !defined(ARCH_PSP) && !defined(OPENGL_ES)
void Curve::drawLathe(unsigned int lathe_divisions)
{
	if (needs_regen) regenerate();
	
	Vector up(0,1,0);
	Vector right;
	
	float i;
//	int j;
	
	for (i = 0; i < M_PI*2.0f; i+=((M_PI*2.0f)/(float)lathe_divisions))
	{
		right = Vector(cos(i),0,sin(i));
		Pencil::drawLine(pointListGen,up,right);
	}

	for (pointGen_i = pointListGen.begin(); pointGen_i != pointListGen.end(); pointGen_i++)
	{
		glBegin(GL_LINE_STRIP);
		for (i = 0; i < (M_PI*2.0f)+((M_PI*2.0f)/(float)lathe_divisions); i+=((M_PI*2.0f)/(float)lathe_divisions))
		{
			right = Vector(cos(i),0,sin(i));

			XYZ pt = up * (*pointGen_i).y + right * (*pointGen_i).x;
			glVertex3f(pt.x,pt.y,pt.z);
		}	
		glEnd();
	}
}
#endif

void Curve::setDivisions(unsigned int divisions_in)
{
	divisions = divisions_in;
	needs_regen = true;
}


int Curve::closestPointTo(XYZ &pt)
{
	if (needs_regen) regenerate();

	int ptnum = -1,i = 0;
	float closest;
	
	if (pointList.empty()) return -1;
	
	for (point_i = pointList.begin(); point_i < pointList.end(); point_i++)
	{
		Vector testVect;
		
		testVect = (*point_i);
		testVect -= pt;
		
		if (testVect.magnitude() < closest || i == 0) 
		{
			ptnum = i;
			closest = testVect.magnitude();
		}
		
		i++;
	}
	
	return ptnum;
}


XYZ &Curve::getCurvePoint(unsigned int ptNum)
{
	return pointListGen[ptNum];
}

int Curve::addCurvePoint(unsigned int ptNum)
{
	int oPt;
	
	oPt = (int)trunc((float)ptNum/divisions)+1;
	
	CurveNode tmp;
	
	pointList.insert(pointList.begin()+oPt, pointListGen[ptNum]);
	tcbList.insert(tcbList.begin()+oPt, tmp);

	
	needs_regen = true;
	
	return oPt;
}


int Curve::closestCurvePointTo(XYZ &pt)
{
	int ptnum = -1,i = 0;
	float closest;
	
	if (pointList.empty()) return -1;
	
	for (point_i = pointListGen.begin(); point_i < pointListGen.end(); point_i++)
	{
		Vector testVect;
		
		testVect = (*point_i);
		testVect -= pt;
		
		if (testVect.magnitude() < closest || i == 0) 
		{
			ptnum = i;
			closest = testVect.magnitude();
		}
		
		i++;
	}
	
	return ptnum;
}