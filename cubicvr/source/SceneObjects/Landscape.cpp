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

#include <CubicVR/SceneObject.h>
#include <CubicVR/SceneObjects/Landscape.h>
#include <CubicVR/lodepng.h>
#include <CubicVR/Pencil.h>

#ifdef _WIN32
#include <math.h>
#define round(x) (x<0?ceil((x)-0.5):floor((x)+0.5))
#endif

#if defined(ARCH_PSP) || defined(ARCH_DC) || defined(OPENGL_ES)
	#define SEG_SQUARE 10
#else 
	#define SEG_SQUARE 32
#endif

Landscape::Landscape(cvrFloat size_in, unsigned int divisions_in_w, unsigned int divisions_in_h, float max_height_in, Material *matRef_in, bool cache) : heightfieldData(NULL), debugShape(NULL)
{
	divisions_w = divisions_in_w;
	divisions_h = divisions_in_h;
	matRef = matRef_in;
	max_height=max_height_in;
	hasVisibility=true;
	
	obj = new Mesh();
	
	Logger::log("build points\n");

	float i,j;
	XYZ tmp;
	
	if (divisions_w > divisions_h)
	{
		size_w = size_in;
		size_h = (size_in/(cvrFloat)divisions_w)*divisions_h;
	}
	else if (divisions_h > divisions_w)
	{
		size_w = (size_in/(cvrFloat)divisions_h)*divisions_w;
		size_h = size_in;
	}
	else
	{
		size_w = size_in;
		size_h = size_in;
	}

	for (j = -(size_h/2.0f); j < (size_h/2.0f); j+=(size_h/(cvrFloat)divisions_h))
	{
		for (i = -(size_w/2.0f); i < (size_w/2.0f); i+=(size_w/(cvrFloat)divisions_w))
		{
			tmp.x = i+((size_w/(divisions_w))/2.0f);
			tmp.y = 0;
			tmp.z = j+((size_h/(divisions_h))/2.0f);
			obj->addPoint(tmp);
		}
	}
	
	Logger::log("build faces\n");
	
	int seg_x,seg_y,segofs_x,segofs_y;
	unsigned int k,l,faceNum;
	float uvm_x,uvm_y;
	float uvm_ofs_x,uvm_ofs_y;
	
	uvm_ofs_x = (50.0/(cvrFloat)SEG_SQUARE);
	uvm_ofs_y = (50.0/(cvrFloat)SEG_SQUARE);
		
	for (l = 0; l < divisions_h-1; l++)
	{
		seg_y = l/SEG_SQUARE;
		segofs_y = l % SEG_SQUARE;
		uvm_y = (float)segofs_y/(float)SEG_SQUARE;
		uvm_y += uvm_ofs_y*(float)seg_y*(float)SEG_SQUARE;
		
		for (k = 0; k < divisions_w-1; k++)
		{
			seg_x = k/SEG_SQUARE;
			segofs_x = k % SEG_SQUARE;
			uvm_x = (float)segofs_x/(float)SEG_SQUARE;
			uvm_x += uvm_ofs_x*(float)seg_x*(float)SEG_SQUARE;

			
//			printf("%d, %d\n",seg_x,seg_y);
			int seg_num = (seg_y * (divisions_w/SEG_SQUARE)) + seg_x;

			obj->setSegment(seg_num);
			
			faceNum = obj->addFace();
			obj->addFacePoint((k)+((l+1)*divisions_w));
			obj->addFacePoint((k+1)+((l)*divisions_w));
			obj->addFacePoint((k)+((l)*divisions_w));

			obj->faces[faceNum]->setUV(0,UV(uvm_x,uvm_y+uvm_ofs_y));
			obj->faces[faceNum]->setUV(1,UV(uvm_x+uvm_ofs_x,uvm_y));
			obj->faces[faceNum]->setUV(2,UV(uvm_x,uvm_y));
			
			obj->bindFaceMaterial(matRef);

			faceNum = obj->addFace();
			obj->addFacePoint((k)+((l+1)*divisions_w));
			obj->addFacePoint((k+1)+((l+1)*divisions_w));
			obj->addFacePoint((k+1)+((l)*divisions_w));

			obj->faces[faceNum]->setUV(0,UV(uvm_x,uvm_y+uvm_ofs_y));
			obj->faces[faceNum]->setUV(1,UV(uvm_x+uvm_ofs_x,uvm_y+uvm_ofs_y));
			obj->faces[faceNum]->setUV(2,UV(uvm_x+uvm_ofs_x,uvm_y));
			
			obj->bindFaceMaterial(matRef);
		}
	}

	

	
	Logger::log("num Segments = %d\n",obj->numSegments);

	Logger::log("calc normals\n");
	obj->calcNormals();

//	printf("calc uv\n");

	segmentMask.init(obj->numSegments);
	segmentMask.setAll();
	obj->segmentMask = &segmentMask;
//	obj->calcSegmentBounds();
	
//	UVMapper lsMapper;
//	lsMapper.setProjection(UV_PROJECTION_PLANAR);
//	lsMapper.setAxis(UV_AXIS_Y);

//	lsMapper.setRotation(XYZ(0,25,0));

//	if (Material::getMaterial(matRef).layer.size())
//	{
//		lsMapper.setScale(XYZ(2,2,2));
//		lsMapper.apply(*obj,matRef,0);
//	}
//	if (Material::getMaterial(matRef).layer.size()>1)
//	{
//		lsMapper.setRotation(XYZ(0,60,0));
//		lsMapper.setScale(XYZ(1,1,1));
//		lsMapper.apply(*obj,matRef,1);
//	}

#ifdef ARCH_DC
	obj->hasColorMap = true;
#endif
	Logger::log("cache uv\n");
	
	if (cache) obj->cache(true,true);
	calcAABB();
};	


int Landscape::getFaceAt(XYZ &pt)
{
	cvrFloat ofs_w = (size_w/2.0f)-((size_w/(divisions_w))/2.0f);
	cvrFloat ofs_h = (size_h/2.0f)-((size_h/(divisions_h))/2.0f);
	
	unsigned int i = (unsigned int)floor(((pt.x + ofs_w)/size_w)*(cvrFloat)(divisions_w));
	unsigned int j = (unsigned int)floor(((pt.z + ofs_h)/size_h)*(cvrFloat)(divisions_h));

	if (i < 0) return -1;
	if (i >= divisions_w-1) return -1;
	if (j < 0) return -1;
	if (j >= divisions_h-1) return -1;
	
	cvrIndex faceNum1 = (i+(j*(divisions_w-1)))*2;
	cvrIndex faceNum2 = faceNum1+1;

	XYZ testPt = *obj->points[obj->faces[faceNum1]->pointref[0]];

#ifdef ARCH_PSP
	if (fabs(pt.x - testPt.x)<0.000001) return faceNum1;
#endif
	
	cvrFloat slope = fabs(pt.z - testPt.z) / fabs(pt.x - testPt.x);
	
	if (slope >= 1)
	{
		return (faceNum1);
	}
	else
	{
		return (faceNum2);
	}

}

cvrFloat Landscape::getHeightValue(XYZ &pt)
{	
	Face *tmpFace;
	XYZ *tmpPoint;

	int faceNum = getFaceAt(pt);

	if (faceNum == -1) return 0;

	tmpFace = obj->faces[faceNum];
	tmpPoint = obj->points[obj->faces[faceNum]->pointref[0]];
	
	tmpFace->calcFaceNormal();

	cvrFloat na = tmpFace->face_normal.x;
	cvrFloat nb = tmpFace->face_normal.y;
	cvrFloat nc = tmpFace->face_normal.z;

	cvrFloat d = -na * tmpPoint->x - nb * tmpPoint->y - nc * tmpPoint->z;

	return ((na * pt.x + nc * pt.z+d)/-nb)+getPosition().y;
}

void Landscape::loadRAWMap(char *raw_map, float lo, float hi)
{
	XYZ tmp;
	unsigned int i;
	
	if (lo==hi) 
	{
		lo = -(max_height/2.0f);
		hi = (max_height/2.0f);
	}
	
	float height_scale = (hi-lo);
	
	if (raw_map)
	{
		FILE *fp;
		
		fp = fopen(raw_map,"rb");
		
		obj->openBuffer(true);

		for (i = 0; i < obj->points.size(); i++)
		{
			unsigned short tmp_int;
			
			char tmp_char = fgetc(fp);
			
			memcpy(((char *)&tmp_int)+1,&tmp_char,1);
			tmp_char = fgetc(fp);
			memcpy(((char *)&tmp_int),&tmp_char,1);
			
			float height_val = (((float)tmp_int)/(65535.0f));
			

			obj->points[i]->y = lo+height_val*height_scale;

			if (heightfieldData) heightfieldData[i] = obj->points[i]->y;

			obj->updatePointCache(i);
		}
		
		
		fclose(fp);		
		
		quickNormalCalc();

		obj->closeBuffer(true);
	}
}

bool Landscape::loadPNGMap(char *png_map, float lo, float hi)
{
	XYZ tmp;
	unsigned int i;

	if (lo==hi) 
	{
		lo = -(max_height/2.0f);
		hi = (max_height/2.0f);
	}
	
	float height_scale = (hi-lo);
	
	if (png_map)
	{
		LodePNG::Decoder pngDecoder;
		std::vector<unsigned char> buffer;
		std::vector<unsigned char> image;
		
		Logger::log("Loading png file %s..\n",png_map);
		
		LodePNG::loadFile(buffer,png_map);

		pngDecoder.getInfoRaw().color.colorType = 0;
		pngDecoder.getInfoRaw().color.bitDepth = 16;
		
		Logger::log("Calling PNG decoder..\n");
		pngDecoder.decode(image, buffer);

		if (pngDecoder.hasError())
		{
			image.clear();
			Logger::log("Attempting to load 8-bit map..\n");
			pngDecoder.getInfoRaw().color.bitDepth = 8;	
			pngDecoder.decode(image, buffer);
		}
		
		Logger::log("Checking decoder state..\n");
		if(pngDecoder.hasError())
		{
			Logger::log(LOG_ERROR,"[loadTextureFromPng] Error %d\n", pngDecoder.getError());
			Logger::log(LOG_ERROR," while trying to load \'%s\'\n",png_map);
			return false;
		}
		
		int pngwidth = pngDecoder.getWidth();
		int pngheight = pngDecoder.getHeight();
		
		Logger::log("PNG Decoder Info [ width: %d, height: %d ]..\n",pngwidth,pngheight);
		
		Logger::log("image size: %d\n",image.size());
		
		Logger::log("isGreyScale: %s\n",pngDecoder.isGreyscaleType()?"true":"false");
		Logger::log("isAlphaType: %s\n",pngDecoder.isAlphaType()?"true":"false");
		Logger::log("Channels: %d\n",pngDecoder.getChannels());
		Logger::log("BPP: %d\n",pngDecoder.getBpp());
		
		int bpp = pngDecoder.getBpp();
		
		for (i = 0; i < obj->points.size(); i++)
		{
			unsigned short tmp_int;
			float height_val;
			
			switch (bpp)
			{
				case 8: 
						tmp_int = image[i];
						height_val = (((float)tmp_int)/(255.0f));
				break;
				case 16: 
						tmp_int = 0;
						memcpy(((char *)&tmp_int)+1,&image[i*2],1);
						memcpy(((char *)&tmp_int),&image[i*2+1],1);
					
						height_val = (((float)tmp_int)/(65535.0f));
				break;
			}
			
			obj->points[i]->y = lo+height_val*height_scale;

			if (heightfieldData) heightfieldData[i] = obj->points[i]->y;
			
			obj->updatePointCache(i);
		}
		
		quickNormalCalc();
		
		obj->closeBuffer(true);

		return true;
	}
	return false;
}


bool Landscape::savePNGMap(char *png_map, float lo, float hi)
{
	if (lo==hi) 
	{
		lo = -(max_height/2.0f);
		hi = (max_height/2.0f);
	}
	
	float height_scale = (hi-lo);
	
	std::vector<unsigned char> image;
	std::vector<unsigned char> buffer;
	
	image.resize(2 * divisions_w * divisions_h);
	
	for(unsigned int i = 0; i < obj->points.size(); i++)
	{
		float height = obj->points[i]->y;;
		if (height > hi) height = hi;
		if (height < lo) height = lo;
		
		height -= lo;
		height /= height_scale;
		
		char tmp_char;
		unsigned short tmp_int = (unsigned short)(height*65535.0);
		
		memcpy(&tmp_char,((char *)&tmp_int)+1,1);
		image[i*2]  = tmp_char;
		
		memcpy(&tmp_char,((char *)&tmp_int),1);
		image[i*2+1]  = tmp_char;
	}
	
	//create encoder and set settings and info (optional)
	LodePNG::Encoder encoder;
	encoder.addText("Comment", "CubicVR Landscape File");
	encoder.getSettings().zlibsettings.windowSize = 2048;
	encoder.infoPng.color.bitDepth=16;
	encoder.infoPng.color.colorType=0;
	encoder.infoRaw.color.bitDepth=16;
	encoder.infoRaw.color.colorType=0;
	
	encoder.encode(buffer, image.empty() ? 0 : &image[0], divisions_w, divisions_h);

	if(encoder.hasError())
	{
		Logger::log("[savePng] Error %d\n", encoder.getError());
	}
	
	LodePNG::saveFile(buffer, png_map);
	
	return true;
}

void Landscape::saveRAWMap(char *raw_map, float lo, float hi)
{
	XYZ tmp;
	unsigned int i;

	if (lo==hi) 
	{
		lo = -(max_height/2.0f);
		hi = (max_height/2.0f);
	}
	
	float height_scale = (hi-lo);
	
	if (raw_map)
	{
		FILE *fp;
		
		fp = fopen(raw_map,"wb");
		
		for (i = 0; i < obj->points.size(); i++)
		{
			float height = obj->points[i]->y;;
			if (height > hi) height = hi;
			if (height < lo) height = lo;
			
			height -= lo;
			height /= height_scale;
			
			char tmp_char;
			unsigned short tmp_int = (unsigned short)(height*65535.0f);
			
			memcpy(&tmp_char,((char *)&tmp_int)+1,1);
			fputc(tmp_char,fp);
			memcpy(&tmp_char,((char *)&tmp_int),1);
			fputc(tmp_char,fp);
		}
		
		fclose(fp);		
	}
}


void Landscape::quickNormalCalc()
{
	cvrIndex i,j;
	Vector xyztmp;
	XYZ *src, *al,*ar,*at,*ab;

	for (i = 1; i < divisions_w-1; i++)
	{
		for (j = 1; j < divisions_h-1; j++)
		{
			cvrIndex ptNum = j*divisions_w+i;

			src = obj->points[ptNum];
			
			al = obj->points[ptNum-1];
			ar = obj->points[ptNum+1];
			at = obj->points[ptNum-divisions_w];
			ab = obj->points[ptNum+divisions_w];
			
			float sl, sr, st, sb;
			
			sl = al->y-src->y;
			sr = src->y-ar->y;

			st = at->y-src->y;
			sb = src->y-ab->y;
			
			xyztmp.x = (sl+sr);
			xyztmp.z = (st+sb);
			xyztmp.y = size_w/(cvrFloat)divisions_w;

//			xyztmp.makeUnit();
			
			obj->updatePointNormalCache(j*divisions_w+i,xyztmp);
		}
	}
}

void Landscape::quickNormalCalc(cvrIndex i, cvrIndex j)
{
	Vector xyztmp;
	XYZ *src, *al,*ar,*at,*ab;
	
	cvrIndex ptNum = j*divisions_w+i;

	if (ptNum < divisions_w) return;
	if (ptNum > obj->points.size()-divisions_w-1) return;

	src = obj->points[ptNum];
	
	al = obj->points[ptNum-1];
	ar = obj->points[ptNum+1];
	at = obj->points[ptNum-divisions_w];
	ab = obj->points[ptNum+divisions_w];
	
	float sl, sr, st, sb;
	
	sl = al->y-src->y;
	sr = src->y-ar->y;

	st = at->y-src->y;
	sb = src->y-ab->y;
	
	xyztmp.x = (sl+sr);
	xyztmp.z = (st+sb);
	xyztmp.y = size_w/(cvrFloat)divisions_w;

	xyztmp.makeUnit();
	
	obj->updatePointNormalCache(j*divisions_w+i,xyztmp);
}



float Landscape::orient(SceneObject *obj_in, float width, float length, float heading, float center)
{
  float xpos, zpos;
  float xrot, zrot;
  float heightsample[5];
  XYZ	xyzTmp;
	
  float halfw = width/2.0f;
  float halfl = length/2.0f;
	
  float mag = sqrt(halfl*halfl+halfw*halfw);

  float ang = atan2(halfl,halfw);

  heading *= ((float)M_PI/180.0f);	// reverse heading to match

  xpos = obj_in->getPosition().x+(sin(heading)*center);
  zpos = obj_in->getPosition().z+(cos(heading)*center);

  //	  width
  //   2+--4--+1
  //   |       |   l		 x (0)
  //   |   .   |   e		 ^
  //   |   o   |   n (PI/2) -|->z (-PI/2)
  //   |       |   g       (+-PI)
  //   |       |   t
  //  3+       +0  h

  // 0  +z-x   
  // 1  +z+x   
  // 2  -z+x   
  // 3  -z-x

  //	extra debug code:

//  	glPushMatrix();
//	glDisable(GL_LIGHTING);
//	glBegin(GL_LINE_STRIP);

  xyzTmp = XYZ(xpos+mag*cos(-ang-(M_PI/2.0f)+heading),0,zpos+mag*-sin(-ang-(M_PI/2.0f)+heading));
  heightsample[0] = getHeightValue(xyzTmp);

 // 
//    glColor3f(1.0,0.0,0.0);
//    glVertex3f(xpos+mag*cos(-ang-(M_PI/2.0)+heading),
//    heightsample[0],
//    zpos+mag*-sin(-ang-(M_PI/2.0)+heading));

  
  xyzTmp = XYZ(xpos+mag*cos(ang-(M_PI/2.0f)+heading),0,	
			      zpos+mag*(-sin(ang-(M_PI/2.0f)+heading)));
  heightsample[1] = getHeightValue(xyzTmp);

//  
//    glColor3f(1.0,0.0,0.0);
//    glVertex3f(xpos+mag*cos(ang-(M_PI/2.0)+heading),
//    heightsample[1],
//    zpos+mag*-sin(ang-(M_PI/2.0)+heading));
//	
  
  xyzTmp = XYZ(xpos+mag*cos(-ang+(M_PI/2.0f)+heading),0,	
			      zpos+mag*(-sin(-ang+(M_PI/2.0f)+heading)));
  heightsample[2] = getHeightValue(xyzTmp);

//
//  	glColor3f(0.0,0.0,1.0);
//	glVertex3f(xpos+mag*cos(-ang+(M_PI/2.0)+heading),
//	heightsample[2],
//	zpos+mag*-sin(-ang+(M_PI/2.0)+heading));
//  	
  
  xyzTmp = XYZ(xpos+mag*cos(ang+(M_PI/2.0f)+heading),0,		
			      zpos+mag*(-sin(ang+(M_PI/2.0f)+heading)));
  heightsample[3] = getHeightValue(xyzTmp);

 // 	glColor3f(0.0,0.0,1.0);
//	glVertex3f(xpos+mag*cos(ang+(M_PI/2.0)+heading),
//	heightsample[0],
//	zpos+mag*-sin(ang+(M_PI/2.0)+heading));
//
//	glEnd();
//	glPopMatrix();
		
  xrot = -atan2((heightsample[1]-heightsample[2]),width);	
  zrot = -atan2((heightsample[0]-heightsample[1]),length);
		
  xrot += -atan2((heightsample[0]-heightsample[3]),width);	
  zrot += -atan2((heightsample[3]-heightsample[2]),length);

  xrot /= 2.0f;	// average angles
  zrot /= 2.0f;

  obj_in->getRotation().z = zrot*(180.0f/M_PI);
  obj_in->getRotation().x = xrot*(180.0f/M_PI);

  return (heightsample[2]-heightsample[3]) + (heightsample[1]-heightsample[0]) / 2.0f;	// average
}


void Landscape::useBrush(int brushType, XYZ &position, float value, float brushSize)
{
	obj->openBuffer();

	float cx = position.x;
	float cz = position.z;
	cvrFloat ofs_w = (size_w/2.0f);
	cvrFloat ofs_h = (size_h/2.0f);

	int numPts = obj->points.size();
	
	long i_start = (unsigned int)floor(((cx-brushSize + ofs_w)/size_w)*(cvrFloat)(divisions_w))-1;
	long j_start = (unsigned int)floor(((cz-brushSize + ofs_h)/size_h)*(cvrFloat)(divisions_h))-1;
	long i_end = (unsigned int)floor(((cx+brushSize + ofs_w)/size_w)*(cvrFloat)(divisions_w))+1;
	long j_end = (unsigned int)floor(((cz+brushSize + ofs_h)/size_h)*(cvrFloat)(divisions_h))+1;

	for (long i = i_start; i < i_end; i++)
	{
		for (long j = j_start; j < j_end; j++)
		{
			long ptNum = i+j*divisions_w;
		
			if (!(ptNum > numPts-1) && !(ptNum < 0) ) 
			{
				cvrFloat x = obj->points[ptNum]->x, z = obj->points[ptNum]->z;
				cvrFloat dx = x-cx, dz = z - cz;
				cvrFloat dist = sqrt(dx*dx+dz*dz);

				switch (brushType)
				{
					case LANDSCAPE_BRUSH_BUMP:
						if (dist < brushSize) obj->points[ptNum]->y += value*((brushSize-dist)/brushSize);
					break;
					
					case LANDSCAPE_BRUSH_RAMP:
						if (dist < brushSize) obj->points[ptNum]->y += value*sin(M_PI/2.0f*((brushSize-dist)/brushSize));
					break;
					
					case LANDSCAPE_BRUSH_PLATEAU:
						if (dist < brushSize) obj->points[ptNum]->y += value*sin(M_PI/2.0f*(sin(M_PI/2.0f*((brushSize-dist)/brushSize))));
					break;
				}

				if (heightfieldData) heightfieldData[i+j*divisions_w] = obj->points[ptNum]->y;

				obj->updatePointCache(ptNum);
			}
		}
	}

	for (long i = i_start; i < i_end; i++)
	{
		for (long j = j_start; j < j_end; j++)
		{
				quickNormalCalc(i,j);
		}
	}

	obj->closeBuffer();
}


void Landscape::function(int functionType, cvrFloat funcSize, cvrFloat funcScale, cvrFloat funcIndex)
{
	cvrFloat val;

	obj->openBuffer(true);

	for (cvrIndex i = 0; i < obj->points.size(); i++)
	{
		cvrFloat xofs=1*funcScale,zofs=3*funcScale;
		cvrFloat x = obj->points[i]->x/funcScale, z = obj->points[i]->z/funcScale;
		cvrFloat x2 = (obj->points[i]->x+xofs)/funcScale, z2 = (obj->points[i]->z+zofs)/funcScale;

		cvrFloat q = funcIndex*3.0f;
		cvrFloat w = funcIndex*2.0f;
		
		val = obj->points[i]->y;
		
		switch (functionType)
		{
			case LANDSCAPE_FUNC_SIN:
				val = 0.8f*sin(sqrt(2.0f*(x*x+z*z)+q)+0.1f*cos(sqrt(2.0f*(x2*x2+z2*z2)))+w);
			break;
			case LANDSCAPE_FUNC_WAVES:
				val = 0.4f*(sin(z+q)-sin(x+w))+0.3f*(cos(z+x-w)-sin(x-z-q))+0.19f*(cos(z*1.4f- x*1.8f +q)-sin(2.0f*x+z +q));
			break;
			case LANDSCAPE_FUNC_SIN_QUANTIZED:
				val = round(1.5f*cos(sqrt(x*x+z*z)+w))/1.5f;
			break;
			case LANDSCAPE_FUNC_WAVES_QUANTIZED:
				val = round(1.5f*(0.4f*(sin(z+q)-sin(x+w))+0.3f*(cos(z+x-w)-sin(x-z-q))+0.19f*(cos(z*1.4f - x*1.8f +q)-sin(2.0f*x+z +q))))/1.5f;
			break;
			case LANDSCAPE_FUNC_SCALE:
			break;
			case LANDSCAPE_FUNC_CLAMP:
				val-=0.003f*(val)*(val);
				if (val > funcSize) val = funcSize;
				if (val < -funcSize) val = -funcSize;
				
				funcSize = 1;
			break;
		
		}
		
		obj->points[i]->y=val*funcSize;
		if (heightfieldData) heightfieldData[i] = obj->points[i]->y;

		obj->updatePointCache(i);
	}
	
	quickNormalCalc();
	
	obj->closeBuffer(true);
}


void Landscape::mkHeightField()
{
	int width=divisions_w;
	int length=divisions_h;

	int upIndex = 1; 

//	btTransform tr;
	startTransform.setIdentity();
	startTransform.setOrigin((getPosition()+XYZ(0,max_height/2,0)).cast());

	heightfieldData = new float[divisions_w*divisions_h];

	cvrIndex i,j;
	
	for (i = 0; i < divisions_w-1; i++)
	{
		for (j = 0; j < divisions_h-1; j++)
		{
			cvrIndex ptNum = j*divisions_w+i;

			heightfieldData[i+j*divisions_w] = obj->points[ptNum]->y;
		}
	}

	btScalar maxHeight = max_height;
	
	bool useFloatDatam=true;
	bool flipQuadEdges=true;

//	btCollisionShape* groundShape;

	btHeightfieldTerrainShape* heightFieldShape = new btHeightfieldTerrainShape(width,length,heightfieldData,maxHeight,upIndex,useFloatDatam,flipQuadEdges);;
	debugShape = heightFieldShape;
	colShape = heightFieldShape;
	
	heightFieldShape->setUseDiamondSubdivision(true);

	btVector3 localScaling(size_w/(divisions_w),1,size_h/(divisions_h));
	colShape->setLocalScaling(localScaling);

	//create ground object
	setMass(0.0f);
}

#ifndef ARCH_PSP
#ifndef OPENGL_ES
class debugLandscapeRender : public btTriangleCallback
{
public:
	void processTriangle(btVector3* triangle, int partId, int triangleIndex)
	{
		glLineWidth(1);
		glColor3f(0.0f,0.2f,1.0f);
		glBegin(GL_LINES);
		glVertex3f(triangle[0].getX(),triangle[0].getY(),triangle[0].getZ());
		glVertex3f(triangle[1].getX(),triangle[1].getY(),triangle[1].getZ());
		glVertex3f(triangle[2].getX(),triangle[2].getY(),triangle[2].getZ());
		glVertex3f(triangle[0].getX(),triangle[0].getY(),triangle[0].getZ());
		glEnd();
	}
};


void Landscape::drawDebugHeightField()
{
	Texture::clearAllTextures();
#if !defined(ARCH_DC)
	GLShader::clear();
#endif
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	
//	if (debugShape)
//	{
//		debugLandscapeRender objCallback;
//		glPushMatrix();
//		
//		debugShape->processAllTriangles(&objCallback,btVector3(-size-10,-size-10,-size-10),btVector3(size+10,size+10,size+10));
//
//		glPopMatrix();
//	}
	
	XYZ pts[2];
	
	glColor3f(0,1,0);
	
	for (int i = 0; i < obj->numSegments; i++)
	{
		if (segmentMask.isSet(i))
		{
			obj->getSegmentBounds(i,pts[0],pts[1]);
			Pencil::drawBB(pts[0],pts[1]);
		}
	}
	
}
#endif
#endif

void Landscape::initialize(btDiscreteDynamicsWorld &dynamicsWorld_in)
{
	RigidSceneObject::initialize(dynamicsWorld_in);
	
	mkHeightField();
	createRigidBody();

//	
//	dynamicsWorld->addRigidBody(mRigidBody);
}



Landscape::~Landscape()
{

};

