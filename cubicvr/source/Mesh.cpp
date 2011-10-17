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

#include <CubicVR/Mesh.h>

#ifdef __linux__
#include <cstring>
#include <stdlib.h>
#include <cstdlib>
#endif

using namespace std;


#if USE_GLUTESS


typedef struct _tessInfo
{
	Mesh *obj;
	cvrIndex face;
} tessInfo;

typedef struct _tessVertexInfo
{
	cvrIndex vtx;
	cvrIndex facevtx;
	std::vector<UV> uv;
	
} tessVertexInfo;

#ifndef ARCH_PSP
#ifndef OPENGL_ES
#ifndef ARCH_DC

extern "C" 
{

vector<tessVertexInfo> tessBuffer;
vector<tessVertexInfo *> tessNewVertices;
tessInfo tess_obj_info;
GLenum tess_type;
    

GLvoid tessBegin ( GLenum type );
GLvoid tessVertex ( tessVertexInfo *vertex_info );
GLvoid tessCombine( GLdouble coords[3], tessVertexInfo *vertexData[4], GLfloat weight[4], unsigned int *outData );
GLvoid tessEnd ( void );
GLvoid tessError ( GLenum errnum );

    
GLvoid tessBegin ( GLenum type )
{
	tess_type = type;
	
	
//	cout << "tessBegin(";
//	
//	switch (type)
//	{
//		case GL_TRIANGLE_FAN:
//			cout << "GL_TRIANGLE_FAN"; break;
//		case GL_TRIANGLE_STRIP:
//			cout << "GL_TRIANGLE_STRIP"; break;
//		case GL_TRIANGLES:
//			cout << "GL_TRIANGLES"; break;
//		case GL_LINE_LOOP:
//			cout << "GL_LINE_LOOP"; break;
//		default:
//			cout << "unknown"; break;
//	}
//	
//	cout << ")" << endl;
}


GLvoid tessVertex(tessVertexInfo *vertex_info)
{
	tessBuffer.push_back(*vertex_info);
}


GLvoid tessCombine(GLdouble coords[3], tessVertexInfo *vertexData[4], GLfloat weight[4], unsigned int *outData)
{
    // construct new vertex with the specified coordinates
	unsigned int uvLayerCount;
	unsigned int i,j;

	tessVertexInfo *vtxDataOut = new tessVertexInfo;
	
	vtxDataOut->vtx = tess_obj_info.obj->addPoint(XYZ((cvrFloat)coords[0],(cvrFloat)coords[1],(cvrFloat)coords[2]));
	
	uvLayerCount = tess_obj_info.obj->faces[tess_obj_info.face]->uv.size();
	
	vtxDataOut->uv.resize(uvLayerCount);
	
    // interpolate texture coordinates
	for (i = 0; i < uvLayerCount; i++)
	{
		vtxDataOut->uv[i] = UV(0,0);
		
		for (j = 0; j < 4; j++)
		{
			if (vertexData[j] != NULL)
			{
				if (tess_obj_info.obj->faces[tess_obj_info.face]->uv[i].size())
				{
					vtxDataOut->uv[i].u += tess_obj_info.obj->faces[tess_obj_info.face]->uv[i][vertexData[j]->facevtx].u * weight[j];
					vtxDataOut->uv[i].v += tess_obj_info.obj->faces[tess_obj_info.face]->uv[i][vertexData[j]->facevtx].v * weight[j];
				
				}
			}
		}
	}
	
    (*outData) = (unsigned long)vtxDataOut;
	
	tessNewVertices.push_back(vtxDataOut);
	
	//cout << "tessCombine() - created new vertex." << endl;
}



GLvoid tessEnd ( void )
{
	unsigned int i,j;
	Mesh *tessObj = tess_obj_info.obj;
	cvrIndex faceNum;
	cvrIndex ptNum[3];
	cvrIndex newFaceCount;
	
	faceNum = tess_obj_info.face;

	//cout << "Face Source: ";
	//cout << tess_obj_info.face;
	//cout << endl;
	
	if (tess_type == GL_TRIANGLE_STRIP)
	{
		cout << "Processing triangle strip" << endl;
		
		for (i = 2; i < tessBuffer.size(); i++)
		{
			tessObj->addFace();
			
			if ((i % 2)==0 && i != 0)
			{
				ptNum[0] = i;
				ptNum[1] = i-2;
				ptNum[2] = i-1;
			}
			else
			{
				ptNum[0] = i;
				ptNum[1] = i-1;
				ptNum[2] = i-2;
			}
				
			
			for (j = 0; j < 3; j++)
			{
				tessObj->addFacePoint(tessBuffer[ptNum[j]].vtx);
			
				if (tessBuffer[ptNum[j]].uv.empty())
				{
					tessObj->faces[faceNum]->copyUV(*tessObj->faces.back(),j,tessBuffer[ptNum[j]].facevtx);
				}
				else
				{
					tessObj->faces.back()->setUV(j,tessBuffer[ptNum[j]].uv);
				}
			}
			
			
			tessObj->bindFaceMaterial(tessObj->faces[faceNum]->mat_ref);

		}
		
		newFaceCount = tessBuffer.size()-2;
	}
		
	if (tess_type == GL_TRIANGLE_FAN)
	{
	//	cout << "Processing triangle fan" << endl;
		
		for (i = 2; i < tessBuffer.size(); i++)
		{
			tessObj->addFace();
			
			ptNum[0] = 0;
			ptNum[1] = i-1;
			ptNum[2] = i;
			
			for (j = 0; j < 3; j++)
			{
				tessObj->addFacePoint(tessBuffer[ptNum[j]].vtx);

				if (tessBuffer[ptNum[j]].uv.empty())
				{
					tessObj->faces[faceNum]->copyUV(*tessObj->faces.back(),j,tessBuffer[ptNum[j]].facevtx);
				}
				else
				{
					tessObj->faces.back()->setUV(j,tessBuffer[ptNum[j]].uv);
				}
			}

			tessObj->bindFaceMaterial(tessObj->faces[faceNum]->mat_ref);
		}
		newFaceCount = tessBuffer.size()-2;
	}

	
	if (tess_type == GL_TRIANGLES)
	{
//		cout << "Processing triangles" << endl;
		
		for (i = 0; i < tessBuffer.size(); i+=3)
		{
			tessObj->addFace();
			
			ptNum[0] = i;
			ptNum[1] = i+1;
			ptNum[2] = i+2;
			
			for (j = 0; j < 3; j++)
			{
				tessObj->addFacePoint(tessBuffer[ptNum[j]].vtx);

				if (tessBuffer[ptNum[j]].uv.empty())
				{
					tessObj->faces[faceNum]->copyUV(*tessObj->faces.back(),j,tessBuffer[ptNum[j]].facevtx);
				}
				else
				{
					tessObj->faces.back()->setUV(j,tessBuffer[ptNum[j]].uv);
				}
			}
			
			tessObj->bindFaceMaterial(tessObj->faces[faceNum]->mat_ref);
		}
		newFaceCount = tessBuffer.size()/3;
	}
		
	
//	cout << "tessEnd():" << newFaceCount << " face(s) added." << endl;
	
	tessBuffer.clear();
}


GLvoid tessError  ( GLenum errnum )
{
	cout << "gluTess error: ";

	switch (errnum)
	{
		case GLU_TESS_MISSING_BEGIN_POLYGON:
			cout << "Missing begin polygon"; break;
		case GLU_TESS_MISSING_END_POLYGON:
			cout << "Missing end polygon"; break;
		case GLU_TESS_MISSING_BEGIN_CONTOUR:
			cout << "Missing begin contour"; break;
		case GLU_TESS_MISSING_END_CONTOUR:
			cout << "Missing end contour"; break;
		case GLU_TESS_COORD_TOO_LARGE:
			cout << "Coordinates too large"; break;
		case GLU_TESS_NEED_COMBINE_CALLBACK:
			cout << "Need a combine callback"; break;
		case GLU_OUT_OF_MEMORY:
			cout << "Out of memory"; break;
		default:
			cout << "Unknown error"; break;
	}
	
//	cout << "(" << errno << ")";
	cout << endl;
}

	
void nurbsError(GLenum errorCode)
{
	const GLubyte *estring;
	
	estring = gluErrorString(errorCode);
	printf ("Nurbs Error: %s\n", estring);
	exit (0);
}
	
	
} // extern "C"
#endif
#endif
#endif
#endif

Mesh::Mesh() : cache_state(false) , buffer_state(false), open_buffer(NULL), bbMin(0,0,0), bbMax(0,0,0), tangent_binormal_state(false), hasColorMap(false), dynamic_colors(NULL), numSegments(1), activeSegment(0), segmentMask(NULL), Resource()
{
	pointCacheOffset = 0;
	normalCacheOffset = 0;
	uvCacheOffset = 0;
	colorMapOffset = 0;	
	
	typeName = "model";
};


Mesh::~Mesh()
{
	clean();
};



Resource *Mesh::upcast(ResourceManager *rm_in, Resource *res_in)
{
	Mesh *res_out = new Mesh();
	res_out->Import(*res_in);
	res_out->onLoad();
	
	res_out->setManager(rm_in);
	
	return (Resource *)res_out;
};


bool Mesh::init()
{
	if (initialized) return true;
	
	string modelFn = rm->pool("resources").getMap().getString(getModelFile().c_str());	
	loadLWO(*this,modelFn,&rm->pool("resources").getMap());
	
#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)
	triangulate();
#endif
	
	cache(true);				
	
	initialized = true;
	
	return true;
}

bool Mesh::destroy()
{
	if (!initialized) return true;
	
	clean();

	initialized = false;

	return true;
}

bool Mesh::reload()
{
	if (!initialized) return true;

	destroy();

	return init();
}



bool Mesh::onLoad() 
{
	if (properties)
	{
		if (properties->rootNode().hasAnother("model_file")) properties->rootNode().child("model_file").element().get(modelFile);
		if (properties->rootNode().hasAnother("original_file")) properties->rootNode().child("original_file").element().get(originalFile);
	}
	
	return true;
};

bool Mesh::onSave() 
{
	if (properties) delete properties;
	
	properties = new DataTree();
	if (!modelFile.empty()) properties->rootNode().newChild("model_file").element().set(modelFile);
	if (!originalFile.empty()) properties->rootNode().newChild("original_file").element().set(originalFile);
	
	return true;
};

string Mesh::getModelFile() 
{ 
	return modelFile; 
}

void Mesh::setModelFile(string modelFile_in) 
{ 
	modelFile = modelFile_in; 
}	

string Mesh::getOriginalFile() 
{ 
	return originalFile; 
}

void Mesh::setOriginalFile(string originalFile_in) 
{ 
	originalFile = originalFile_in; 
}

void Mesh::clean()
{
	if (cache_state) cache(false);	// clean up any cache data
	
	for (cvrIndex i = 0; i < points.size(); i++)
	{
		delete points[i];
	}

//#ifndef ARCH_DC
//	setMaterialMask(false);
//#endif	
	
	points.clear();
	faces.clear();
	mat_reflist.clear();
#ifndef ARCH_PSP
	mat_cache_data.clear();
#endif
	vertex_cacheref.clear();
	normalCacheOffset = 0;
	point_smoothCache.clear();		

	numSegments = 1;
	
	bbMin = bbMax = XYZ(0,0,0);
//	printf("ptsize:%d\n",points.size());
}

//#ifndef WIN32
//#warning need to fix up triangulate for PSP
//#endif

void Mesh::triangulate()
{
	bool cache_state_temp = cache_state;
	
    if (cache_state) cache(false);

#if USE_GLUTESS
	cvrIndex i,faceCount;
	unsigned int j;
    unsigned int numPoints;
    
	vector<cvrIndex> tessFaces;

	
	// create tesselation object
	GLUtesselator* tess = gluNewTess();

	if (tess == NULL)
	{
		return;
	}

    

#ifdef WIN32
	gluTessCallback(tess, GLU_TESS_BEGIN,  (GLvoid (__stdcall *) ())  tessBegin);
	gluTessCallback(tess, GLU_TESS_VERTEX,  (GLvoid (__stdcall *) ())  tessVertex);	
	gluTessCallback(tess, GLU_TESS_COMBINE,  (GLvoid (__stdcall *) ())  tessCombine);
	gluTessCallback(tess, GLU_TESS_END,  (GLvoid (__stdcall *) ()) tessEnd);
	gluTessCallback(tess, GLU_TESS_ERROR, (GLvoid (__stdcall *) ()) tessError);	
#else
	gluTessCallback(tess, GLU_TESS_BEGIN,  (GLvoid(*)())  tessBegin);
	gluTessCallback(tess, GLU_TESS_VERTEX,  (GLvoid(*)())  tessVertex);	
	gluTessCallback(tess, GLU_TESS_COMBINE,  (GLvoid(*)())  tessCombine);
	gluTessCallback(tess, GLU_TESS_END,  (GLvoid(*)()) tessEnd);
	gluTessCallback(tess, GLU_TESS_ERROR, (GLvoid(*)()) tessError);	
#endif

	// faces.size() may change during this process
	faceCount = faces.size();
	

	for (i = 0; i < faceCount; i++)
	{
		numPoints = faces[i]->points.size();

		if (numPoints > 3)
		{
#ifdef _WIN32
			double** tess_vertex = new double *[numPoints];

			for ( unsigned p = 0; p < numPoints; ++i )
				tess_vertex[p] = new double[3];
#else
			double (*tess_vertex)[3] = new double [numPoints][3];
#endif

			tessVertexInfo *tess_vertex_info = new tessVertexInfo[numPoints];			
			
			tessFaces.push_back(i);
			
			tess_obj_info.obj = this;
			tess_obj_info.face = i;

//			gluTessProperty(tess,GLU_TESS_WINDING_RULE,GLU_TESS_WINDING_POSITIVE);
			gluTessBeginPolygon(tess,NULL);
			gluTessBeginContour(tess); 
			
			for (j = 0; j < numPoints; j++)
			{		
				tess_vertex[j][0] = faces[i]->points[j]->x;
				tess_vertex[j][1] = faces[i]->points[j]->y;
				tess_vertex[j][2] = faces[i]->points[j]->z;
				
				tess_vertex_info[j].facevtx = j;
				tess_vertex_info[j].vtx = faces[i]->pointref[j];
				
				gluTessVertex(tess, tess_vertex[j], &tess_vertex_info[j]);
			}

			gluTessEndContour(tess);
			gluTessEndPolygon(tess);
			
//			free (tess_vertex);
//			delete tess_vertex_info;
		}
		
	}
	
	// destroy the tesselation object
	gluDeleteTess(tess);
	
	for (i = 0; i < tessFaces.size(); i++)
	{
		copyFace(faces.size()-1,tessFaces[i]);
		clearFace(faces.size()-1);
		faces.pop_back();
	}
	
	for (i = 0; i < tessNewVertices.size(); i++)
	{
		delete tessNewVertices[i];
	}
	
	tessNewVertices.clear();
#else

    // this one is just for quads but works on all platforms at least
    for (int i = 0, iMax = faces.size(); i < iMax; i++) {
        if (faces[i]->points.size() == 4) {
            
            int p = addFace();

            faces[p]->mat_ref = faces[i]->mat_ref;

            faces[p]->setSegment(faces[i]->getSegment());

            addFacePoint(p,faces[i]->pointref[2]);
            addFacePoint(p,faces[i]->pointref[3]);
            addFacePoint(p,faces[i]->pointref[0]);
            
            faces[i]->points.pop_back();
            faces[i]->pointref.pop_back();
            
            faces[p]->face_normal = faces[i]->face_normal;
            
            int numUVLayer = faces[i]->uv.size();
            
            if (numUVLayer) {
                for (int j = 0; j < numUVLayer; j++)
                {
                    if (faces[i]->uv[j].size() == 4) {
                        faces[p]->setUV(0, faces[i]->uv[j][2], j);
                        faces[p]->setUV(1, faces[i]->uv[j][3], j);
                        faces[p]->setUV(2, faces[i]->uv[j][0], j);
                        
                        faces[i]->uv[j].pop_back();
                    }
                }
            }
            
            if (faces[i]->point_normals.size() == 4) {
                faces[p]->point_normals[0] = faces[i]->point_normals[2];
                faces[p]->point_normals[1] = faces[i]->point_normals[3];
                faces[p]->point_normals[2] = faces[i]->point_normals[0];
                
                faces[i]->point_normals.pop_back();
            }
            
        }
    }

#endif
	
	calcNormals();

	buildRefList();
	
	cache(cache_state_temp);
}


	

void Mesh::calcBB()
{
	if (bbMin==bbMax) getBoundingBox(bbMin,bbMax);
}

void Mesh::getBoundingBox(XYZ &bbMin_out, XYZ &bbMax_out)
{
	if (!points.size()) 
	{
		bbMin_out = bbMax_out = XYZ(0,0,0);
		return;
	}
		
	bbMin_out = *points[0];
	bbMax_out = *points[0];
	
	unsigned int i;
	cvrIndex numPoints = points.size();
	
	for (i = 0; i < numPoints; i++)
	{
		if (bbMin_out.x > points[i]->x) bbMin_out.x = points[i]->x;
		if (bbMin_out.y > points[i]->y) bbMin_out.y = points[i]->y;
		if (bbMin_out.z > points[i]->z) bbMin_out.z = points[i]->z;

		if (bbMax_out.x < points[i]->x) bbMax_out.x = points[i]->x;
		if (bbMax_out.y < points[i]->y) bbMax_out.y = points[i]->y;
		if (bbMax_out.z < points[i]->z) bbMax_out.z = points[i]->z;
	}
}



bool Mesh::ensurePoints(cvrIndex pointNum)
{
	if (!(points.size() > pointNum))	/* ensure we have enough capacity to handle this point reference */
	{
		points.resize(pointNum+1,new XYZ());
		return false;
	}
	else 
	{
		return true;
	}
};


bool Mesh::ensureFaces(cvrIndex faceNum)
{
	cvrIndex numFaces;
	
	numFaces = faces.size();

	if (!(faces.size() > faceNum))	/* ensure we have enough capacity to handle this face reference */
	{
		faces.resize(faceNum+1,new Face());

//		for (i = numFaces-1; i <= faceNum; i++)
//		{
//			reflistAdd(i);
//		}
		
		return false;
	}
	else
	{
		return true;
	}
};


//void Object::reflistRemove(cvrIndex faceNum)
//{
//	mat_reflist[faces[faceNum]->mat_ref][faces[faceNum]->pointref.size()].erase(faceNum);	/* remove from previous mat reference */
//#ifndef _WIN32
//	if (!mat_reflist[faces[faceNum]->mat_ref][faces[faceNum]->pointref.size()].size()) mat_reflist[faces[faceNum]->mat_ref].erase(faces[faceNum]->pointref.size());	/* check empty */
//	if (!mat_reflist[faces[faceNum]->mat_ref].size()) mat_reflist.erase(faces[faceNum]->mat_ref); /* check empty */
//#endif
//}


void Mesh::reflistAdd(cvrIndex faceNum)
{	
	mat_reflist[faces[faceNum]->mat_ref][faces[faceNum]->getSegment()][faces[faceNum]->pointref.size()].insert(faceNum);	/* add to new */
}


void Mesh::setSegment(int segment_id, char *seg_name)
{
	if (segment_id >= numSegments) numSegments = segment_id+1;
	activeSegment = segment_id;
		
	if (seg_name) seg_ref[seg_name] = segment_id;
}


void Mesh::calcSegmentBounds()
{
	int currentSegment; 
	XYZ pts[2];
	
	std::vector<Face *>::iterator face_i;
	std::vector<XYZ *>::iterator point_i;
	
	seg_bounds.clear();
	calcBB();
	
	for (face_i = faces.begin(); face_i != faces.end(); ++face_i)
	{
		currentSegment = (*face_i)->segment_id;
		
		if ((*face_i)->points.empty()) continue;
		
		if (seg_bounds.find(currentSegment) == seg_bounds.end())
		{						
			pts[0] = seg_bounds[currentSegment].bb1 = bbMax;
			pts[1] = seg_bounds[currentSegment].bb2 = bbMin;
		}
		else
		{
			pts[0] = seg_bounds[currentSegment].bb1;
			pts[1] = seg_bounds[currentSegment].bb2;
		}
		
		for (point_i = (*face_i)->points.begin(); point_i != (*face_i)->points.end(); point_i++)
		{
			if ((*point_i)->x < pts[0].x) pts[0].x = (*point_i)->x;
			if ((*point_i)->y < pts[0].y) pts[0].y = (*point_i)->y;
			if ((*point_i)->z < pts[0].z) pts[0].z = (*point_i)->z;

			if ((*point_i)->x > pts[1].x) pts[1].x = (*point_i)->x;
			if ((*point_i)->y > pts[1].y) pts[1].y = (*point_i)->y;
			if ((*point_i)->z > pts[1].z) pts[1].z = (*point_i)->z;			
		}

		seg_bounds[currentSegment].bb1 = pts[0];
		seg_bounds[currentSegment].bb2 = pts[1];		
		
//		printf("Segment: %d, bounds: %f, %f, %f - %f, %f, %f\n",currentSegment,pts[0].x,pts[0].y,pts[0].z,pts[1].x,pts[1].y,pts[1].z);

	}
}


void Mesh::getSegmentBounds(int segment_id, XYZ &bb1, XYZ &bb2)
{
	if (!seg_bounds.size())
	{
		calcSegmentBounds();
	}
	
//	if (seg_bounds.find(segment_id) == seg_bounds.end())
//	{
//		bb1=bb2=XYZ(0,0,0);
//		return;
//	}

	if (seg_bounds.find(segment_id) == seg_bounds.end()) { bb1=bb2=XYZ(0,0,0); return; }
	
	
	//printf("Segment: %d, bounds: %f, %f, %f - %f, %f, %f\n",segment_id,seg_bounds[segment_id].bb1.x,seg_bounds[segment_id].bb1.y,seg_bounds[segment_id].bb1.z,seg_bounds[segment_id].bb2.x,seg_bounds[segment_id].bb2.y,seg_bounds[segment_id].bb2.z);
	
	bb1 = seg_bounds[segment_id].bb1;
	bb2 = seg_bounds[segment_id].bb2;
}


int Mesh::getSegmentId(char *seg_name)
{
	if (seg_ref.find(seg_name) != seg_ref.end())
	{
		return 0;
	}
	else 
	{
		return seg_ref[seg_name];
	}
}


cvrIndex Mesh::addPoint(const XYZ &xyz_in)
{
	points.push_back(new XYZ());	/* add a new point at the end of points and set its xyz values */
	(*points.back()) = xyz_in;
	
	return points.size()-1;
};



void Mesh::addPoint(cvrIndex pointNum, const XYZ &xyz_in)
{
	ensurePoints(pointNum);
	points[pointNum] = new XYZ();
	*points[pointNum] = xyz_in;
};


cvrIndex Mesh::addFace()
{
	ensureFaces(faces.size());  /* create a new face */

	faces[faces.size()-1]->setSegment(activeSegment);
	
//	reflistAdd(faces.size()-1); /* set it to the null material by default, will change when bound */

	return faces.size()-1;
};


int Mesh::hasPoint(const XYZ &point_compare)
{
	for (cvrIndex i=0; i < points.size(); i++)
	{
		if ((*points[i]) == point_compare) return i;
	}
	
	return -1;
}


void Mesh::buildRefList()
{
	mat_reflist.clear();
	
	for (cvrIndex i = 0; i < faces.size(); i++)
	{
		reflistAdd(i);
	}
}


void Mesh::calcGroupSizes()
{
	unsigned int segSize;

	// mat_reflist_i -- used to iterate through the material mat_reflist of the object
	std::map<Material *, map< unsigned int, std::map<cvrIndex, va_ref, ltindex>, ltuint > >::iterator obj_matref_i;
	
	// segment iterator
	map< unsigned int, std::map<cvrIndex, va_ref, ltindex>, ltuint >::iterator mat_segment_type_i;
	// mat_facetypelist_i -- used to iterate through the set of face types which belong to the mat_reflist_i map
	std::map<cvrIndex, va_ref, ltindex>::iterator matref_type_i;
	
	for (obj_matref_i = mat_cache_data.begin(); obj_matref_i != mat_cache_data.end(); obj_matref_i++)
	{
		for (mat_segment_type_i = (*obj_matref_i).second.begin(); mat_segment_type_i != (*obj_matref_i).second.end(); mat_segment_type_i++)
		{
			segSize = 0;
			
			for (matref_type_i = (*mat_segment_type_i).second.begin(); matref_type_i != (*mat_segment_type_i).second.end(); matref_type_i++)
			{
				segSize+=(*matref_type_i).second.element_count;
			}
			
			(*mat_cache_data[(*obj_matref_i).first][(*mat_segment_type_i).first].begin()).second.group_size = segSize;
		}
		
	}	
}

void Mesh::addFace(cvrIndex faceNum)
{
	ensureFaces(faceNum);

	faces[faceNum]->setSegment(activeSegment);

//	reflistAdd(faceNum);
};


int Mesh::addFacePoint(cvrIndex pointNum)
{
	addFacePoint(faces.size()-1,pointNum);
	return faces[faces.size()-1]->points.size()-1;
};


void Mesh::addFacePoint(cvrIndex faceNum, cvrIndex pointNum)
{
	ensurePoints(pointNum);
	ensureFaces(faceNum);
	
//	reflistRemove(faceNum);
	faces[faceNum]->points.push_back(points[pointNum]);
	faces[faceNum]->pointref.push_back(pointNum);
	faces[faceNum]->point_normals.push_back(Vector());

//	reflistAdd(faceNum);
};

int Mesh::addFacePoint(cvrIndex pointNum, RGB &rgb_val)
{
	addFacePoint(faces.size()-1,pointNum,rgb_val);
	return faces[faces.size()-1]->points.size()-1;
};


void Mesh::addFacePoint(cvrIndex faceNum, cvrIndex pointNum, RGB &rgb_val)
{
	ensurePoints(pointNum);
	ensureFaces(faceNum);
	
//	reflistRemove(faceNum);
	
	faces[faceNum]->points.push_back(points[pointNum]);
	faces[faceNum]->pointref.push_back(pointNum);
	faces[faceNum]->point_normals.push_back(Vector());
	faces[faceNum]->point_colors.push_back(rgb_val);
	
//	reflistAdd(faceNum);
};


void Mesh::clearFace(cvrIndex faceNum)
{
//	reflistRemove(faceNum);	
	faces[faceNum]->points.clear();
	faces[faceNum]->pointref.clear();
	faces[faceNum]->point_normals.clear();
	faces[faceNum]->mat_ref = 0;
	
//	reflistAdd(faceNum);
};


void Mesh::copyFace(cvrIndex srcFaceNum, cvrIndex dstFaceNum)
{
	unsigned int i;
	
	clearFace(dstFaceNum);
	bindFaceMaterial(dstFaceNum,faces[srcFaceNum]->mat_ref);

	for (i = 0; i < faces[srcFaceNum]->pointref.size(); i++)
	{
		addFacePoint(dstFaceNum,faces[srcFaceNum]->pointref[i]);
		faces[dstFaceNum]->point_normals[i] = faces[srcFaceNum]->point_normals[i];
		faces[srcFaceNum]->copyUV(*faces[dstFaceNum],i,i);
	}
	
	faces[dstFaceNum]->segment_id = faces[srcFaceNum]->segment_id;
 
};


#ifndef ARCH_PSP
void Mesh::reCalcNormals()
{
	cvrIndex i,j,k;

	/* calculate all the normals for the faces */
	for (i = 0; i < faces.size(); i++)	
	{
		faces[i]->calcFaceNormal();
	}

	for (i = 0; i < point_smoothCache.size(); i++)
	{
		for (j = 0; j < point_smoothCache[i].size(); j++)
		{
			unsigned int numFaces = point_smoothCache[i][j].size();
			Vector norm(0,0,0);
			
			for (k = 0; k < numFaces; k++)
			{
				norm += faces[point_smoothCache[i][j][k].faceref]->face_normal;
			}
			
			norm /= (float)numFaces;
			
			faces[point_smoothCache[i][j][0].faceref]->point_normals[point_smoothCache[i][j][0].pointref] = norm;
		}
	}
}


void Mesh::updateColorCache()
{
	if (!dynamic_colors) return;

	va_rgb *bind;
	
	if (!bind) return;
	
	if (open_buffer) 
	{
		memcpy(open_buffer+colorMapOffset,dynamic_colors,sizeof(va_rgb)*cache_data.vertex_count);
	}
	else
	{
		memcpy(cache_data.data+colorMapOffset,dynamic_colors,sizeof(va_rgb)*cache_data.vertex_count);
	}	
}
#endif



void Mesh::calcNormals()
{
	cvrIndex i,j,k;

	std::vector<cvrIndex> point_facecount;	/* keep track of how many faces use a given point */
	point_facecount.resize(points.size());
			
	/* calculate all the normals for the faces */
	for (i = 0; i < faces.size(); i++)	
	{
		faces[i]->calcFaceNormal();
	}


	std::vector< std::vector<smoothingPair> > point_smoothRef;

	point_smoothCache.clear();

	point_smoothRef.resize(points.size());
	point_smoothCache.resize(points.size());

	/* build a list of which faces share which points */
	for (i = 0; i < faces.size(); i++)	
	{
		for (j = 0; j < faces[i]->points.size(); j++)
		{
			smoothingPair smoothRef;
			
			smoothRef.faceref = i;
			smoothRef.pointref = j;
			
			point_smoothRef[faces[i]->pointref[j]].push_back(smoothRef);
		}
	}

	for (i = 0; i < points.size(); i++)
	{
		point_smoothCache[i].resize(point_smoothRef[i].size());
	}
	
	cvrIndex point_smoothRef_size = point_smoothRef.size();
	
	/* step through smoothing references and compute normals */
	for (i = 0; i < point_smoothRef_size; i++)
	{
		cvrIndex point_smoothRef_i_size = point_smoothRef[i].size();
		for (j = 0; j < point_smoothRef_i_size; j++)
		{
			cvrFloat smoothCount = 1;
			
			/* set point to it's face's normal */
			faces[point_smoothRef[i][j].faceref]->point_normals[point_smoothRef[i][j].pointref] = faces[point_smoothRef[i][j].faceref]->face_normal;

			smoothingPair smoothFaceInitial;
			
			smoothFaceInitial.faceref = point_smoothRef[i][j].faceref;
			smoothFaceInitial.pointref = point_smoothRef[i][j].pointref;

			point_smoothCache[i][j].push_back(smoothFaceInitial);
			
			/* step through all other faces which share this point */
			for (k = 0; k < point_smoothRef_i_size; k++)
			{
				if (j==k) continue;	/* don't include self in comparison */
				
				cvrFloat ang = faces[point_smoothRef[i][k].faceref]->face_normal.angle(faces[point_smoothRef[i][j].faceref]->face_normal);
				
				/* if the face's material's smoothing angle is less than the angle between the two faces sharing this point currently being compared then add it's normal to the smoothing calculation */
				/* note that ang != ang tests for infinity case where the normals are the same */
				if (ang != ang || ang*(180.0f/M_PI) <= faces[point_smoothRef[i][j].faceref]->mat_ref->max_smooth)
				{
					faces[point_smoothRef[i][j].faceref]->point_normals[point_smoothRef[i][j].pointref] += faces[point_smoothRef[i][k].faceref]->face_normal;
					smoothCount++;

					smoothingPair smoothFaceAdditional;
					
					smoothFaceAdditional.faceref = point_smoothRef[i][k].faceref;
					smoothFaceAdditional.pointref = point_smoothRef[i][k].pointref;
					
					point_smoothCache[i][j].push_back(smoothFaceAdditional);
				}
			}
			faces[point_smoothRef[i][j].faceref]->point_normals[point_smoothRef[i][j].pointref] /= smoothCount;
			faces[point_smoothRef[i][j].faceref]->point_normals[point_smoothRef[i][j].pointref].makeUnit();
		}
	}
};


void Mesh::cloneStructure(Mesh &obj)
{
	cvrIndex i,j;
	
	cvrIndex oldNumPts = points.size();
	cvrIndex inNumPts = obj.points.size();
	
	for (i = 0; i < inNumPts; i++)
	{
		addPoint(*obj.points[i]);
	}
		
	for (i = 0; i < obj.faces.size(); i++)
	{
		addFace();
		
		for (j = 0; j < obj.faces[i]->pointref.size(); j++)
		{
			addFacePoint(obj.faces[i]->pointref[j]+oldNumPts);
		}	
	}
	
};


void Mesh::cloneObject(Mesh &obj)
{
	cvrIndex i,j;
	
	cvrIndex oldNumPts = points.size();
	cvrIndex inNumPts = obj.points.size();
	cvrIndex oldNumFaces = faces.size();
	
	for (i = 0; i < inNumPts; i++)
	{
		addPoint(i+oldNumPts,*obj.points[i]);
	}
	
	for (i = 0; i < obj.faces.size(); i++)
	{
		obj.setSegment(obj.faces[i]->segment_id);
		
		for (j = 0; j < obj.faces[i]->pointref.size(); j++)
		{
			addFacePoint(i+oldNumFaces,obj.faces[i]->pointref[j]+oldNumPts);
			obj.faces[i]->copyUV(*faces[i+oldNumFaces],j,j);
		}	
		
		bindFaceMaterial(obj.faces[i]->mat_ref);
		
	}

	calcNormals();
};


void Mesh::setFaceSegment(cvrIndex faceNum, int segment_id)
{
	faces[faceNum]->setSegment(segment_id);
};

//void Object::bindFaceMaterial()
//{
//	bindFaceMaterial(Material::materials.size()-1);
//};
//

void Mesh::bindFaceMaterial(Material *mat)
{
	bindFaceMaterial(faces.size()-1,mat);
};


void Mesh::bindFaceMaterial(cvrIndex faceNum, Material *mat)
{
	ensureFaces(faceNum);

//	reflistRemove(faceNum);
	
	faces[faceNum]->mat_ref = mat;

//	reflistAdd(faceNum);
};


/*
 
#ifdef ARCH_PSP
void Object::cache(bool cache_state_in, bool dynamic_state_in)
{
	unsigned int m;
	map<Material *, map< unsigned int, map<unsigned short, set<cvrIndex>, ltushort>, ltuint > >::iterator obj_matref_i;
	map< unsigned int, map<unsigned short, set<cvrIndex>, ltushort>, ltuint >::iterator matref_segment_i;
	map<unsigned short, set<cvrIndex>, ltushort>::iterator matref_type_i;
	set<cvrIndex>::iterator type_face_i;

	if (!cache_state && cache_state_in)		
	{
		printf("Constructing Un-optimized PSP Cache..");
		// construct a compiled vertex array cache that can be called via drawarray
	
//		std::map<cvrIndex, std::map<cvrIndex, std::set<cvrIndex>, ltindex>, ltindex>::iterator obj_matref_i;
//		std::map<cvrIndex, std::set<cvrIndex>, ltindex>::iterator matref_type_i;
//		std::set<cvrIndex>::iterator type_face_i;

		buildRefList();
		
		for (obj_matref_i = mat_reflist.begin(); obj_matref_i != mat_reflist.end(); obj_matref_i++)
		{
			printf("Material Ref: %lu\n",(*obj_matref_i).first);
			if (!(*obj_matref_i).second.empty())
			{
				unsigned int cache_total;
				map<cvrIndex,cvrIndex> cache_size;
				map<Material *,map<cvrIndex,cvrIndex> > faceCount;
				unsigned int interleave_size = sizeof(va_uv) + sizeof(unsigned int) + sizeof(va_xyz) + sizeof(va_xyz);
				unsigned int cache_multiplier = 0;
				
				cache_size.clear();

				for (matref_segment_i = (*obj_matref_i).second.begin(); matref_segment_i != (*obj_matref_i).second.end(); matref_segment_i++)
				{					
					for (matref_type_i = (*matref_segment_i).second.begin(); matref_type_i != (*matref_segment_i).second.end(); matref_type_i++)
					{
						cache_size[(*matref_type_i).first] = 0;
						psp_elemcount[(*obj_matref_i).first][(*matref_type_i).first] = 0;
						faceCount[(*obj_matref_i).first][(*matref_type_i).first] = 0;
					}					
				}
				
				for (matref_segment_i = (*obj_matref_i).second.begin(); matref_segment_i != (*obj_matref_i).second.end(); matref_segment_i++)
				{					
					for (matref_type_i = (*matref_segment_i).second.begin(); matref_type_i != (*matref_segment_i).second.end(); matref_type_i++)
					{
						cache_multiplier = (*matref_type_i).first * (*matref_type_i).second.size();

						cache_size[(*matref_type_i).first] += sizeof(va_uv)*cache_multiplier;	// UV
						cache_size[(*matref_type_i).first] += sizeof(unsigned int)*cache_multiplier;	// Color
						cache_size[(*matref_type_i).first] += sizeof(va_xyz)*cache_multiplier; // Normal
						cache_size[(*matref_type_i).first] += sizeof(va_xyz)*cache_multiplier; // Vertex
					}

				}
				
				for (matref_segment_i = (*obj_matref_i).second.begin(); matref_segment_i != (*obj_matref_i).second.end(); matref_segment_i++)
				{					
					for (matref_type_i = (*matref_segment_i).second.begin(); matref_type_i != (*matref_segment_i).second.end(); matref_type_i++)
					{
						if (cache_size[(*matref_type_i).first] != 0)
						{
							psp_reflist[(*obj_matref_i).first][(*matref_type_i).first] = new char[cache_size[(*matref_type_i).first]];
							cache_total += cache_size[(*matref_type_i).first];
							cache_size[(*matref_type_i).first] = 0;
						}
					}
				}
				
				printf("Allocated object cache: %d bytes\n",cache_total);				
				printf("Interleave size: %d bytes\n",interleave_size);					

					for (matref_type_i = (*matref_segment_i).second.begin(); matref_type_i != (*matref_segment_i).second.end(); matref_type_i++)
					{
						printf("Face Type: %lu points\n",(*matref_type_i).first);
	
						if (!(*matref_type_i).second.empty())
						{							
							// num points * num faces
							
							printf("Processing faces... ");
							for (type_face_i = (*matref_type_i).second.begin(); type_face_i != (*matref_type_i).second.end(); type_face_i++)
							{
								unsigned int pointCount = 0;
								std::vector<XYZ *>::iterator points_i;
								
								Face *currentFace = faces[(*type_face_i)];						
								
								//printf("%lu, ",(*type_face_i));
															
								for (points_i = currentFace->points.begin(); points_i != currentFace->points.end(); points_i++)
								{
									char *psp_reflist_p = psp_reflist[(*obj_matref_i).first][(*matref_type_i).first];
									
									va_xyz xyz_tmp;
									va_xyz normal_tmp;
									va_uv uv_tmp;
									
									xyz_tmp.x = currentFace->points[pointCount]->x;
									xyz_tmp.y = currentFace->points[pointCount]->y;
									xyz_tmp.z = currentFace->points[pointCount]->z;
									
									if (currentFace->point_normals.size())
									{									
										normal_tmp.x = currentFace->point_normals[pointCount].x;
										normal_tmp.y = currentFace->point_normals[pointCount].y;
										normal_tmp.z = currentFace->point_normals[pointCount].z;
									}
									else
									{
										normal_tmp.x = currentFace->face_normal.x;
										normal_tmp.y = currentFace->face_normal.y;
										normal_tmp.z = currentFace->face_normal.z;
									}
									
									if (currentFace->uv.size())
									{
										uv_tmp.u = currentFace->uv[0][pointCount].u;
										uv_tmp.v = currentFace->uv[0][pointCount].v;
									}
									else
									{
										uv_tmp.u = 0;
										uv_tmp.v = 0;
									}								

									char *ofs = psp_reflist_p;
									
									ofs += (*matref_type_i).first * faceCount[(*obj_matref_i).first][(*matref_type_i).first] * interleave_size;
									ofs += pointCount * interleave_size;
									
									unsigned int color = GU_COLOR( 1.0f, 1.0f, 1.0f, 1.0f );

									memcpy(ofs,&uv_tmp,sizeof(va_uv));
									ofs += sizeof(va_uv);
									memcpy(ofs,&color,sizeof(unsigned int));
									ofs += sizeof(unsigned int);
									memcpy(ofs,&normal_tmp,sizeof(va_xyz));

									ofs += sizeof(va_xyz);
									vertex_cacheref[currentFace->pointref[pointCount]].push_back(ofs);	
									memcpy(ofs,&xyz_tmp,sizeof(va_xyz));
									
									pointCount++;
								}
								
								faceCount[(*obj_matref_i).first][(*matref_type_i).first]++;
							}
	//						printf("%lu",(*matref_type_i).second.size());
							
							psp_elemcount[(*obj_matref_i).first][(*matref_type_i).first] += (*matref_type_i).second.size();
						}
						printf("\n");
					}
				}
			}
		
		// cache now initialized :) 
		cache_state = true;	
	}
	else if (cache_state && !cache_state_in)
	{
		// cache deinitialized
		cache_state = false;
	}
	else	// state already set, do nothing
	{
	
	}
};

void Object::updatePointNormalCache(cvrIndex pointNum, int subCount, XYZ &normal_val)
{
	std::deque<char *>::iterator pointref_i;

	va_xyz xyzTemp;
	xyzTemp.x = normal_val.x;
	xyzTemp.y = normal_val.y;
	xyzTemp.z = normal_val.z;
	
	
//	for (pointref_i = vertex_cacheref[pointNum].begin(); pointref_i != vertex_cacheref[pointNum].end(); pointref_i++)
//	{		
		float *ofs = (float *)vertex_cacheref[pointNum][subCount];

		memmove(ofs - sizeof(va_xyz),(char *)&xyzTemp,sizeof(va_xyz));
//	}
}

void Object::updatePointCache(cvrIndex pointNum)
{
	std::deque<char *>::iterator pointref_i;

	for (pointref_i = vertex_cacheref[pointNum].begin(); pointref_i != vertex_cacheref[pointNum].end(); pointref_i++)
	{		
		char *ofs = (*pointref_i);

		va_xyz xyzTemp;
		xyzTemp.x = points[pointNum]->x;
		xyzTemp.y = points[pointNum]->y;
		xyzTemp.z = points[pointNum]->z;
		
		memmove(ofs,(char *)&xyzTemp,sizeof(va_xyz));
	}
}
 

*/

void Mesh::updatePointCache(cvrIndex pointNum)
{
	std::set<cvrIndex>::iterator pointref_i;
	
	va_xyz xyzTemp;
	xyzTemp.x = points[pointNum]->x;
	xyzTemp.y = points[pointNum]->y;
	xyzTemp.z = points[pointNum]->z;

#ifdef ARCH_PSP	
	for (pointref_i = vertex_cacheref[pointNum].begin(); pointref_i != vertex_cacheref[pointNum].end(); pointref_i++)
	{		
		char *cacheData = cache_data.data + ((*pointref_i)*dataOfs);

		memcpy(cacheData+pointCacheOffset,(char *)&xyzTemp,sizeof(va_xyz));		
	}
#else	
	for (pointref_i = vertex_cacheref[pointNum].begin(); pointref_i != vertex_cacheref[pointNum].end(); pointref_i++)
	{		
		long ofs = (*pointref_i);
		
		if (open_buffer) 
		{
			memcpy(open_buffer+(ofs*sizeof(va_xyz)),(char *)&xyzTemp,sizeof(va_xyz));
		}
		else
		{
			memcpy(cache_data.data+(ofs*sizeof(va_xyz)),(char *)&xyzTemp,sizeof(va_xyz));
		}
	}
#endif	
//	vertex_cacheref[(*face_point_i)].push_front(pt_index);
							
}

void Mesh::updatePointNormalCache(cvrIndex pointNum, XYZ &normal_val)
{
	std::set<cvrIndex>::iterator pointref_i;

	va_xyz xyzTemp;
	xyzTemp.x = normal_val.x;
	xyzTemp.y = normal_val.y;
	xyzTemp.z = normal_val.z;

#ifdef ARCH_PSP
	for (pointref_i = vertex_cacheref[pointNum].begin(); pointref_i != vertex_cacheref[pointNum].end(); pointref_i++)
	{		
		char *cacheData = cache_data.data + ((*pointref_i)*dataOfs);
		
		memcpy(cacheData+normalCacheOffset,(char *)&xyzTemp,sizeof(va_xyz));
	}
#else
	for (pointref_i = vertex_cacheref[pointNum].begin(); pointref_i != vertex_cacheref[pointNum].end(); pointref_i++)
	{		
		long ofs = (*pointref_i);
		
		if (open_buffer)
		{
			memcpy(open_buffer+(ofs*sizeof(va_xyz))+normalCacheOffset,(char *)&xyzTemp,sizeof(va_xyz));
		}
		else
		{
			memcpy(cache_data.data+(ofs*sizeof(va_xyz))+normalCacheOffset,(char *)&xyzTemp,sizeof(va_xyz));
		}
	}
#endif
	
//	vertex_cacheref[(*face_point_i)].push_front(pt_index);
							
}


void Mesh::updatePointNormalCache(cvrIndex pointNum, int subPoint, XYZ &normal_val)
{
	std::set<cvrIndex>::iterator pointref_i;
	
	va_xyz xyzTemp;
	xyzTemp.x = normal_val.x;
	xyzTemp.y = normal_val.y;
	xyzTemp.z = normal_val.z;
	
	pointref_i = vertex_cacheref[pointNum].begin();
	
	do
	{
		pointref_i++;
	}
	while (subPoint--);

#ifdef ARCH_PSP
	char *cacheData = cache_data.data + ((*pointref_i)*dataOfs);
	
	memcpy(cacheData+normalCacheOffset,(char *)&xyzTemp,sizeof(va_xyz));
#else
	long ofs = (*pointref_i);

	if (open_buffer)
	{
		memcpy(open_buffer+(ofs*sizeof(va_xyz))+normalCacheOffset,(char *)&xyzTemp,sizeof(va_xyz));
	}
	else
	{
		memcpy(cache_data.data+(ofs*sizeof(va_xyz))+normalCacheOffset,(char *)&xyzTemp,sizeof(va_xyz));
	}
#endif
	//	vertex_cacheref[(*face_point_i)].push_front(pt_index);
	
}



void Mesh::fullCopyCache(bool doPoints, bool doNormals)
{
#ifndef ARCH_PSP
	if (open_buffer)
	{
		if (doNormals) memcpy(open_buffer+normalCacheOffset,cache_data.data+normalCacheOffset,sizeof(va_xyz)*cache_data.vertex_count);
		if (doPoints) memcpy(open_buffer,cache_data.data,sizeof(va_xyz)*cache_data.vertex_count);
	}
#endif
}


void Mesh::cache(bool cache_state_in, bool dynamic_cache, bool vertex_buffer) //, bool tangent_binormal_calc
{
	unsigned int m;
	map<Material *, map< unsigned int, map<unsigned short, set<cvrIndex>, ltushort>, ltuint > >::iterator obj_matref_i;
	map< unsigned int, map<unsigned short, set<cvrIndex>, ltushort>, ltuint >::iterator matref_segment_i;
	map<unsigned short, set<cvrIndex>, ltushort>::iterator matref_type_i;
	set<cvrIndex>::iterator type_face_i;

//	for (obj_matref_i = mat_reflist.begin(); obj_matref_i != mat_reflist.end(); obj_matref_i++)
//	{
//		if (Material::materials[(*obj_matref_i).first].hasParallax())
//		{
//			tangent_binormal_calc = true;
//		}
//	}	// didn't seem to work well, doing it all in the shader now
//	
	
//#ifdef ARCH_PSP
//	hasColorMap=true;
//#endif
	
	buildRefList();

	
//	if (segmentMask) delete segmentMask;
//
//	if (numSegments)
//	{
//		segmentMask = new BITSET;
//		segmentMask->Init(numSegments);
//		segmentMask->SetAll();
//	}
		

	if (!cache_state && cache_state_in)		
	{
		// construct a compiled vertex array cache that can be called via drawelements
	
		std::vector<cvrIndex>::iterator face_point_i;	/* face point ref iterator */
				
		std::vector< std::vector<va_ptref> > va_compiled;	/* compiled vector of vertex array pointrefs */
		std::vector<va_ptref>::iterator va_compiled_ptref_i;	/* iterator for it */
		std::vector<va_faceref> va_facerefs;

		va_compiled.resize(points.size());	// a vector of vectors of points
		va_facerefs.resize(faces.size());	// vector of faces and their new indicies into the compiled points
		
		cvrIndex pt_index = 0;	// keeps an index when new points are found
		cvrIndex element_count = 0;	// counts the total number of elements so we know how much to allocate afterwards

		cache_data.max_uvs=0;	// tracks the 'maximum' amount of UV lists required, unfortunately even if a UV isn't used for a specific material it's list will just remain 0's in that portion to allow proper alignment
//		if (tangent_binormal_calc)
//		{
//			buildTangentBinormalAttribs();
//		}
		
		/* 
			va_compiled explanation:
			
			va_compiled is used to 'compile' the points
			
			each face can share a point x,y,z but may not share that point's normal value or it's U,V coords --
			therefore we have to 'compile' the points so that we have a minimum set of only those points which
			actually differ from others.
			
			this is done by running through all the faces and storing their points into the vector at the va_compiled
		 	position at which their point reference specifies, but only if it isn't already there.
			
			so, at each va_compiled position for the point reference there is a vector of va_ptrefs, if the 
			first ptref does not match than we check the next and so on, if no point is found that matches the 
			current face point being checked we then add new point to the vector at that postion.  
			
			Each time a new point is created it is assigned one more than the previous index (pt_index) 
			and	that index is stored in the va_facerefs for the current face
			
			And when we're done we have a nice va_compiled block of only those points which are actually different
			and a va_facerefs which specifies the faces using the new point references.
			
			also note we're building mat_cache_data which is a counterpart to mat_reflist but only stores
			element indicies and element counts for rendering vertex arrays and buffer objects.	
		
			
			todo:
			
			In the future I will be creating an additional optimization which uses the information gathered in
			va_facerefs and va_compiled to create triangle strips and fans to provide another speed boost.
			
		*/

		/* count the max number of uv layers we'll need to avoid costly reallocations when drawing, trades video card memory for drawing speed */
		for (obj_matref_i = mat_reflist.begin(); obj_matref_i != mat_reflist.end(); obj_matref_i++)
		{
			if ((*obj_matref_i).first->layer.size() > (unsigned int)cache_data.max_uvs)
			cache_data.max_uvs = (*obj_matref_i).first->layer.size();
		}
		
		/* iterate through the list of materials with face reference sets */
		for (obj_matref_i = mat_reflist.begin(); obj_matref_i != mat_reflist.end(); obj_matref_i++)
		{
			//mat_cache_data[(*obj_matref_i).first].resize(numSegments);
			
			for (matref_segment_i = (*obj_matref_i).second.begin(); matref_segment_i != (*obj_matref_i).second.end(); matref_segment_i++)
			{
				// (*obj_matref_i).first // material reference
				for (matref_type_i = (*matref_segment_i).second.begin(); matref_type_i != (*matref_segment_i).second.end(); matref_type_i++)
				{
					// (*matref_type_i).first // primitive type
					
					// current overall point count for this material & primitive
					unsigned int pt_count = 0;

					// total number of points this primitive and it's count will consume
					unsigned int num_points = 0; // (*matref_type_i).second.size()*(*matref_type_i).first;
					
					// account for patch exclusions
					for (type_face_i = (*matref_type_i).second.begin(); type_face_i != (*matref_type_i).second.end(); type_face_i++)
					{
						if (patches.size()) if (isPatch[(*type_face_i)]) continue;
						
						num_points += (*matref_type_i).first;
					}
					
					va_ref *cache_ref = &mat_cache_data[(*obj_matref_i).first][(*matref_segment_i).first][(*matref_type_i).first];
					
					// track the point count for when we call drawelements
					cache_ref->element_count = num_points;
															
					/* allocate a list of point references, this data will be used for vertex arrays */
	#if defined(OPENGL_ES) || defined(ARCH_PSP)
					cache_ref->elements = new unsigned short[num_points];				
	#else
					cache_ref->elements = new unsigned int[num_points];
	#endif				
				
					cache_ref->range_min = num_points;
					cache_ref->range_max = 0;
					
					/* track the total number of elements for caching allocation later */
					element_count += num_points;	
			
					/* iterate through the faces at this primitive type */
					for (type_face_i = (*matref_type_i).second.begin(); type_face_i != (*matref_type_i).second.end(); type_face_i++)
					{
						/* current point for this face */
						unsigned int face_pt_count = 0;
						
						if (patches.size()) if (isPatch[(*type_face_i)]) continue;
						
						Face *currentFace = faces[(*type_face_i)];
						
						/* iterate through the points on this face */
						for (face_point_i = currentFace->pointref.begin(); face_point_i != currentFace->pointref.end(); face_point_i++)
						{
							/* did we find a matching point? assume false for starters */
							bool found_pt = false;

							/* check for this point reference in the va_compiled vector */
							if (!va_compiled[(*face_point_i)].size())
							{
								/* there's no points at the vector at this point reference index, there can't be a matching point */
								found_pt = false;
							}
							else /* it had a size, there might be a matching point */
							{	
								/* step through the points at this reference */
								for (unsigned int i = 0; i < va_compiled[(*face_point_i)].size(); i++)
								{	
									va_ptref *cacheRef = &va_compiled[(*face_point_i)][i];
									/* does the normal match? if not then it can't be our point, try the next one */
									if (currentFace->point_normals[face_pt_count] != cacheRef->faceRef->point_normals[cacheRef->pointNum]) continue;
									
									/* normals matched, does it have the same amount of UV data? */
									if (cacheRef->faceRef->uv.size() == currentFace->uv.size())
									{
										/* has the same amount of UV data, we'll assume true and say it's false if one of them doesn't match */
										found_pt = true;
		
										for (unsigned int m = 0; m < cacheRef->faceRef->uv.size(); m++)
										{
											if (cacheRef->faceRef->uv[m][cacheRef->pointNum] != currentFace->uv[m][face_pt_count]);
											{
												found_pt=false;	/* oops, a value didn't match, this isn't our point */
											}
										}
										
										/* we have a winner! */
										if (found_pt)
										{
											/* assign the point's index into the elements list for vertex arrays */
											cvrIndex t_index = va_compiled[(*face_point_i)][i].this_index;
											
											cache_ref->elements[pt_count] = t_index;
											
											if (cache_ref->range_min > t_index) 
											{
												cache_ref->range_min = t_index;
											}
											
											if (cache_ref->range_max < t_index) 
											{
												cache_ref->range_max = t_index;
											}											
											
											/* add this point's index to the faceref for this face */
											va_facerefs[(*type_face_i)].ptref_index.push_back(t_index);
											vertex_cacheref[(*face_point_i)].insert(t_index);
//											printf("[X]");
										}
									}
									else /* UV data isn't the same size, can't be a match */
									{
										found_pt = false;
									}
								} 
							}
							
							/* no point was found in the above tests, therefore we have a new point */
							if (!found_pt)
							{
								/* create a va point reference */
								va_ptref temp_pt;
								temp_pt.point_ref = (*face_point_i);
								temp_pt.this_index = pt_index;
								
	//							(*face_point_i) current point
	//							pt_index = vertex cache index

								vertex_cacheref[(*face_point_i)].insert(pt_index);
								
								/* store this point's reference */
								temp_pt.faceRef = currentFace;
								temp_pt.pointNum = face_pt_count;
								
								
								if (currentFace->point_colors.size() || hasColorMap)
								{
//									if (currentFace->point_colors)
//									{
//										if (va_compiled[(*face_point_i)].size() != currentFace->point_colors.size())
//										{
//											currentFace->point_colors.resize(va_compiled[(*face_point_i)].size());
//										}
//									}
									hasColorMap = true;
								}
									
								/* push it onto the vector at the point reference index */
								va_compiled[(*face_point_i)].push_back(temp_pt);

								/* set it's index */
								cache_ref->elements[pt_count] = temp_pt.this_index;

								// compute ranges for elements
								if (cache_ref->range_min > temp_pt.this_index) 
								{
									cache_ref->range_min = temp_pt.this_index;
								}
								
								if (cache_ref->range_max < temp_pt.this_index) 
								{
									cache_ref->range_max = temp_pt.this_index;
								}																			
								
								/* add it to the faceref for this face */
								va_facerefs[(*type_face_i)].ptref_index.push_back(temp_pt.this_index);
		
								pt_index++;	/* increase point index so next point has new index */
							}
														
							pt_count++;	/* increase total point count */
							face_pt_count++; /* increase point count for this face */
						}			
						
					}					
				}		
			
			}
		}

		
		va_facerefs.clear();

		/* build the cache using the data compiled above, pt_index will contain the number of points needed */				
#ifdef ARCH_PSP
		va_uv *cache_uvs;
		if (cache_data.max_uvs > 1) cache_data.max_uvs = 1;
		va_rgba *cache_colors;
#else
		std::vector<va_uv *> cache_uvs;
		/* allocate cache array pointers */
		cache_uvs.resize(cache_data.max_uvs);				
		va_rgb *cache_colors;
#endif
		va_xyz *cache_points;
		va_xyz *cache_normals;
				
		/* total vertex count for this cache */
		cache_data.vertex_count = pt_index;
		
		// xyz + uv
		unsigned long dataSize = (pt_index*sizeof(va_xyz)) + ((pt_index*cache_data.max_uvs)*sizeof(va_uv)); // + (tangent_binormal_calc?(pt_index*2*sizeof(va_xyz)):0);
		
		// normals
		dataSize += (pt_index*sizeof(va_xyz));

		if (hasColorMap)
		{
#ifdef ARCH_PSP
			dataSize += (pt_index*4);
#else
			dataSize += (pt_index*sizeof(va_rgb));
#endif
		}

		/* allocate cache_data.data with enough space for all points, normals and uvs */

		cache_data.data = new char[ dataSize ];
		
#ifdef ARCH_PSP
		
		printf("Created object buffer: %d bytes\n",dataSize);

		/*
		 
		 * Vertex order:
		 * [for vertices(1-8)]
		 * [weights (0-8)]
		 * [texture uv]
		 * [color]
		 * [normal]
		 * [vertex]
		 * [/for] 
		 
		 */
				
		dataOfs = 0;	// will result in interleave block size
		
		if (cache_data.max_uvs)
		{			
			uvCacheOffset = dataOfs;
			dataOfs+=2*sizeof(float);
		}		

		if (hasColorMap)
		{
			colorMapOffset = dataOfs;			
			dataOfs+=4*sizeof(unsigned char);
		}		
		
		// normals
		normalCacheOffset = dataOfs;		
		dataOfs += 3*sizeof(float);
		
		// points
		pointCacheOffset = dataOfs;		
		dataOfs += 3*sizeof(float);
						
#else
		unsigned long dataOfs = 0;
		
		/* copy into single cache data buffer, point xyz goes first */
		cache_points = (va_xyz *)cache_data.data;
		
		dataOfs += (sizeof(va_xyz)*pt_index);
		
		normalCacheOffset = dataOfs;
		
		/* normal data is appended after point xyz */
		cache_normals = (va_xyz *)(cache_data.data+dataOfs);
				
		dataOfs += (sizeof(va_xyz)*pt_index);
		
		if (hasColorMap)
		{
			colorMapOffset = dataOfs;
			
			cache_colors = (va_rgb *)(cache_data.data+dataOfs);
			
			dataOfs += (sizeof(va_rgb)*pt_index);
		}		
		
		uvCacheOffset = dataOfs;
		
		/* and then we append the uv data */
		for (m = 0; m < cache_data.max_uvs; m++)
		{			
			cache_uvs[m] = (va_uv *)(cache_data.data+dataOfs);
			
			dataOfs += (sizeof(va_uv)*pt_index);
		}
#endif		
		

		/* now we pull all the data gathered from compiling the points and throw it into the cache arrays */
		for (unsigned int index = 0; index < va_compiled.size(); index++)
		{	
			/* for each point reference we've generated one or more points, iterate through them */
			for (va_compiled_ptref_i = va_compiled[index].begin(); va_compiled_ptref_i != va_compiled[index].end(); va_compiled_ptref_i++)
			{
#ifdef ARCH_PSP
				/* since the points we compiled above didn't really get entered in order we use the index that was created */
				unsigned int va_index = (*va_compiled_ptref_i).this_index;
				
				// set up base pointer for element
				unsigned int cacheOfs = va_index*dataOfs;
				char *cacheData = cache_data.data + cacheOfs;
						
				// map pointers into interleaved data
				cache_normals = (va_xyz *)(cacheData + (normalCacheOffset));		
				cache_points = (va_xyz *)(cacheData + (pointCacheOffset));		
						
				cache_points->x = points[index]->x;
				cache_points->y = points[index]->y;
				cache_points->z = points[index]->z;
				
				cache_normals->x = (*va_compiled_ptref_i).faceRef->point_normals[(*va_compiled_ptref_i).pointNum].x;
				cache_normals->y = (*va_compiled_ptref_i).faceRef->point_normals[(*va_compiled_ptref_i).pointNum].y;
				cache_normals->z = (*va_compiled_ptref_i).faceRef->point_normals[(*va_compiled_ptref_i).pointNum].z;
				
				if (hasColorMap)
				{
					cache_colors = (va_rgba *)(cacheData + (colorMapOffset));	
					if ((*va_compiled_ptref_i).faceRef->point_colors.size())
					{
						cache_colors->r = (unsigned char)(255.0*(*va_compiled_ptref_i).faceRef->point_colors[(*va_compiled_ptref_i).pointNum].r);
						cache_colors->g = (unsigned char)(255.0*(*va_compiled_ptref_i).faceRef->point_colors[(*va_compiled_ptref_i).pointNum].g);
						cache_colors->b = (unsigned char)(255.0*(*va_compiled_ptref_i).faceRef->point_colors[(*va_compiled_ptref_i).pointNum].b);
						cache_colors->a = 1;
					}
					else
					{
						cache_colors->r = cache_colors->g = cache_colors->b = 0;
						cache_colors->a = 1;
					}
				}
				
				if (cache_data.max_uvs)
				{
					cache_uvs = (va_uv *)(cacheData + (uvCacheOffset));			
					if ((*va_compiled_ptref_i).faceRef->uv.size())
					{
						if ((*va_compiled_ptref_i).faceRef->uv[0].size())
						{
							cache_uvs->u = (*va_compiled_ptref_i).faceRef->uv[0][(*va_compiled_ptref_i).pointNum].u;
							cache_uvs->v = (*va_compiled_ptref_i).faceRef->uv[0][(*va_compiled_ptref_i).pointNum].v;
						}
						else
						{
							cache_uvs->u = 0;
							cache_uvs->v = 0;
						}
					}
					else
					{
						cache_uvs->u = 0;
						cache_uvs->v = 0;
					}
				}
				
//				if ((*va_compiled_ptref_i).uvs) delete (*va_compiled_ptref_i).uvs;
#else				
				/* since the points we compiled above didn't really get entered in order we use the index that was created */
				unsigned int va_index = (*va_compiled_ptref_i).this_index;

				cache_points[va_index].x = points[index]->x;
				cache_points[va_index].y = points[index]->y;
				cache_points[va_index].z = points[index]->z;
				
				cache_normals[va_index].x = (*va_compiled_ptref_i).faceRef->point_normals[(*va_compiled_ptref_i).pointNum].x;
				cache_normals[va_index].y = (*va_compiled_ptref_i).faceRef->point_normals[(*va_compiled_ptref_i).pointNum].y;
				cache_normals[va_index].z = (*va_compiled_ptref_i).faceRef->point_normals[(*va_compiled_ptref_i).pointNum].z;
				
				if (hasColorMap)
				{
					cache_colors[va_index].r = (*va_compiled_ptref_i).faceRef->point_colors[(*va_compiled_ptref_i).pointNum].r;
					cache_colors[va_index].g = (*va_compiled_ptref_i).faceRef->point_colors[(*va_compiled_ptref_i).pointNum].g;
					cache_colors[va_index].b = (*va_compiled_ptref_i).faceRef->point_colors[(*va_compiled_ptref_i).pointNum].b;
				}
				
				for (unsigned int m = 0; m < cache_data.max_uvs; m++)
				{
					if ((*va_compiled_ptref_i).faceRef->uv.size() > m)
					{
						cache_uvs[m][va_index].u = (*va_compiled_ptref_i).faceRef->uv[m][(*va_compiled_ptref_i).pointNum].u;
						cache_uvs[m][va_index].v = (*va_compiled_ptref_i).faceRef->uv[m][(*va_compiled_ptref_i).pointNum].v;
					}
					else
					{
						cache_uvs[m][va_index].u = 0;
						cache_uvs[m][va_index].v = 0;
					}
				}
				
#endif
			}			
		}
		
		// at this point everything should be packed into the cache data memory, we can clear our compilation reference
		va_compiled.clear();

		// build bounding box groups for segments
		calcGroupSizes();

		
			// todo: memory management
			
			cvrIndex element_index = 0;	// element index walker 
			
			// this is why we counted elements earlier, so we'd know how many to allocate now
#ifdef ARCH_PSP
			cache_data.cache_element = (cvrElement *)malloc(element_count*sizeof(cvrElement));
#else
			cache_data.cache_element = (cvrElement *)std::calloc(element_count,sizeof(cvrElement));
#endif		
			// step through the materials/faces 
			for (obj_matref_i = mat_reflist.begin(); obj_matref_i != mat_reflist.end(); obj_matref_i++)
			{				
				// (*obj_matref_i).first // material reference
				for (matref_segment_i = (*obj_matref_i).second.begin(); matref_segment_i != (*obj_matref_i).second.end(); matref_segment_i++)
				{
					// (*obj_matref_i).first // material reference
					for (matref_type_i = (*matref_segment_i).second.begin(); matref_type_i != (*matref_segment_i).second.end(); matref_type_i++)
					{
						if (!(*matref_type_i).second.empty())
						{
							// (*matref_type_i).first // primitive type
							
							// had to do this because for some reason memcpy() chokes when the data is around 600k+ :P
							for (cvrIndex i = 0; i < mat_cache_data[(*obj_matref_i).first][(*matref_segment_i).first][(*matref_type_i).first].element_count; i++)
							{
								// append the element data to the element cache
								cache_data.cache_element[i+element_index] = mat_cache_data[(*obj_matref_i).first][(*matref_segment_i).first][(*matref_type_i).first].elements[i];
							}
							
							cvrElement *old_elems = mat_cache_data[(*obj_matref_i).first][(*matref_segment_i).first][(*matref_type_i).first].elements;
							
							mat_cache_data[(*obj_matref_i).first][(*matref_segment_i).first][(*matref_type_i).first].elements = cache_data.cache_element+element_index;
							
							delete(old_elems);
							// increase the element walker for the next loop
							element_index += mat_cache_data[(*obj_matref_i).first][(*matref_segment_i).first][(*matref_type_i).first].element_count;
						}					
					}
				}
				
			}

#ifndef OPENGL_ES		
#ifndef ARCH_DC
#ifndef ARCH_PSP
			if (vertex_buffer)
			{
				// Build Vertex Buffer Object
			
				glDisableClientState(GL_NORMAL_ARRAY);
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				glDisableClientState(GL_COLOR_ARRAY);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
				glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
			
				GLuint newbuffer[2];

				glGenBuffers(2, newbuffer);
			
				cache_data.dataBufferNum = newbuffer[0];
				
				printf("Created object buffer: %u, %lu bytes\n",cache_data.dataBufferNum,dataSize);
				
				// create a new buffer on the card
				glBindBuffer(GL_ARRAY_BUFFER_ARB, cache_data.dataBufferNum);
				// upload our data into the buffer
				glBufferData(GL_ARRAY_BUFFER_ARB, dataSize, cache_data.data, dynamic_cache?GL_STREAM_DRAW_ARB:GL_STATIC_DRAW_ARB);

				cache_data.elementBufferNum = newbuffer[1];
			
				// create and upload element cache 
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, cache_data.elementBufferNum);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER_ARB, element_count*sizeof(unsigned int), cache_data.cache_element, GL_STATIC_DRAW_ARB);

				// card now holds the data, no more need to keep it in system memory :) 
				std::free(cache_data.cache_element);

				cache_data.cache_element = NULL;
				
				buffer_state = true;

				glDisableClientState(GL_NORMAL_ARRAY);
				glDisableClientState(GL_VERTEX_ARRAY);
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);		
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
				glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
			}
#endif
#endif
#endif

		/* cache now initialized :) */
		cache_state = true;	
	}
	else if (cache_state && !cache_state_in)
	{
		GLuint buffer[2];

		/* remove any current buffer assignment */
#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
			glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
			
		
			/* clear our data and element buffers */
			buffer[0] = cache_data.dataBufferNum;
		//	glDeleteBuffers(1,buffer);

			buffer[1] = cache_data.elementBufferNum;
		//	glDeleteBuffers(1,buffer);
		
			glDeleteBuffers(2,buffer);
		
			buffer_state = false;
#endif

//		/* clear out all the vertex array element data */		
//		for (obj_matref_i = mat_reflist.begin(); obj_matref_i != mat_reflist.end(); obj_matref_i++)
//		{
//			for (matref_segment_i = (*obj_matref_i).second.begin(); matref_segment_i != (*obj_matref_i).second.end(); matref_segment_i++)
//			{
//				// (*obj_matref_i).first // material reference
//				for (matref_type_i = (*matref_segment_i).second.begin(); matref_type_i != (*matref_segment_i).second.end(); matref_type_i++)
//				{
//					// (*matref_type_i).first // primitive type
//					delete mat_cache_data[(*obj_matref_i).first][(*matref_segment_i).first][(*matref_type_i).first].elements;
//				}
//			}
//		}
		
		/* clear out our mat cache data */
		mat_cache_data.clear();
		vertex_cacheref.clear();

		/* delete our cache data */
		delete cache_data.data;
		if (cache_data.cache_element) free(cache_data.cache_element);

		//		if (cache_data.elementData) delete cache_data.elementData;
//		
//		cache_data.elementData = NULL;
		cache_data.elementBufferNum = 0;
		cache_data.dataBufferNum = 0;
		cache_data.vertex_count = 0;
		cache_data.max_uvs = 0;

		/* cache deinitialized */		
		cache_state = false;
	}
	else	/* state already set, do nothing */
	{
	
	}
};



void Mesh::openBuffer(bool fullCopy) 
{ 
#ifndef ARCH_PSP
#ifndef OPENGL_ES
#ifndef ARCH_DC
	if (!buffer_state) return;

	if (!fullCopy)
	{
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, cache_data.dataBufferNum);
		open_buffer = (char*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB); 
	}
#endif
#endif
#endif
};

void Mesh::closeBuffer(bool fullCopy) 
{
#ifndef ARCH_PSP
#ifndef OPENGL_ES
#ifndef ARCH_DC
	if (fullCopy)
	{
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, cache_data.dataBufferNum);
		open_buffer = (char*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
		fullCopyCache();
	}
	
	if (open_buffer)
	{
		glUnmapBufferARB(GL_ARRAY_BUFFER_ARB); // unmap it after use
		open_buffer = NULL;
	}
#endif
#endif
#endif
};


void Mesh::setPatch(cvrIndex faceNum, bool isPatchState)
{
	if (isPatch.size()<faces.size()) isPatch.resize(faces.size(),false);
	isPatch[faceNum] = isPatchState;
	patches.push_back(faceNum);
}


void Mesh::setWeight(cvrIndex pointNum, float weightVal)
{
	if (weights.size() < points.size()) weights.resize(points.size());
	weights[pointNum] = weightVal;
}


void Mesh::generatePatches()
{
	std::map<cvrIndex, std::map<cvrIndex, cvrIndex, ltindex>, ltindex> edgeMap;
	
	cvrIndex i;
	unsigned int j;
	
	for (i = 0; i < faces.size(); i++)
	{
		for (j = 0; j < faces[i]->pointref.size(); j++)
		{
			int pt1,pt2;
			
			pt1 = faces[i]->pointref[j];
			pt2 = faces[i]->pointref[(j+1) % faces[i]->pointref.size()];
			
			edgeMap[pt1][pt2] = i;
		}
	}
	
	
	/* todo: actually generate patches */

}


//#ifndef OPENGL_ES|ARCH_PSP
//void Object::buildTangentBinormalAttribs()
//{
//	unsigned long i,j,k;
//
//	for (i = 0; i < faces.size(); i++)
//	{
//		faces[i].calcTangentBinormal();
////		printf("Face %d: biNormal: %f, %f, %f\n",i,faces[i].invBinormal.x,faces[i].invBinormal.y,faces[i].invBinormal.z);
////		printf("Face %d: tangent: %f, %f, %f\n",i,faces[i].invTangent.x,faces[i].invTangent.y,faces[i].invTangent.z);
//	}
//
//	// requires calcNormals() to have been called
//	for (i = 0; i < point_smoothCache.size(); i++)
//	{
//		for (j = 0; j < point_smoothCache[i].size(); j++)
//		{
//			int numFaces = point_smoothCache[i][j].size();
//			Vector binorm(0,0,0);
//			
//			for (k = 0; k < numFaces; k++)
//			{
//				binorm += faces[point_smoothCache[i][j][k].faceref].binormal;
//
//				if (faces[point_smoothCache[i][j][k].faceref].point_binormals.size() != faces[point_smoothCache[i][j][k].faceref].points.size())
//				{
//					faces[point_smoothCache[i][j][k].faceref].point_binormals.resize(faces[point_smoothCache[i][j][k].faceref].points.size());
//				}
//			}
//			
//			binorm /= (float)numFaces;
//			
//			
//			faces[point_smoothCache[i][j][0].faceref].point_binormals[point_smoothCache[i][j][0].pointref] = binorm;
//		}
//
//		for (j = 0; j < point_smoothCache[i].size(); j++)
//		{
//			int numFaces = point_smoothCache[i][j].size();
//			Vector tang(0,0,0);
//			
//			for (k = 0; k < numFaces; k++)
//			{
//				tang += faces[point_smoothCache[i][j][k].faceref].tangent;
//
//				if (faces[point_smoothCache[i][j][k].faceref].point_tangents.size() != faces[point_smoothCache[i][j][k].faceref].points.size())
//				{
//					faces[point_smoothCache[i][j][k].faceref].point_tangents.resize(faces[point_smoothCache[i][j][k].faceref].points.size());
//				}
//			}
//			
//			tang /= (float)numFaces;
//			
//			
//			faces[point_smoothCache[i][j][0].faceref].point_tangents[point_smoothCache[i][j][0].pointref] = tang;
//		}		
//	}
//
//	
//	
//	tangent_binormal_state = true;
//}
//#endif
