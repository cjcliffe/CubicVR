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

#ifndef CUBICVR_OBJECT_H
#define CUBICVR_OBJECT_H

#include <CubicVR/cvr_defines.h>

#include <CubicVR/GLExt.h>

#include <CubicVR/Vector.h>
#include <CubicVR/XYZRGB.h>
#include <CubicVR/Face.h>
#include <CubicVR/Material.h>
#include <CubicVR/BITSET.h>
#include <CubicVR/ResourceManager.h>

#include <vector>
#include <set>
#include <map>
#include <deque>
#include <list>
#include <memory>

#ifdef ARCH_PSP
#include <pspdebug.h>
#define printf pspDebugScreenPrintf 
#endif



/* vertex array structures */
struct va_xyz
{
	float x, y, z;
};

struct va_rgb
{
	float r, g, b;
};

#ifdef ARCH_PSP
struct va_rgba
{
	unsigned char r, g, b, a;
};
#else
struct va_rgba
{
	float r, g, b, a;
};
#endif

struct va_uv
{
	float u, v;
};

struct va_ptref
{
//	cvrIndex faceNum;
	Face *faceRef;
	unsigned char pointNum;
//	va_uv *uvs;
//	unsigned short uvsize;
	cvrIndex point_ref;
	cvrIndex this_index;
//	Vector normal;
//	RGB color;
};

struct va_faceref
{
	std::vector<cvrIndex> ptref_index;
};


struct va_block
{
	unsigned int vertex_count;
	unsigned int max_uvs;
	
	unsigned int dataBufferNum;
	unsigned int elementBufferNum;

#ifdef ARCH_PSP
	char __attribute__((aligned(16))) *data;
#else
	char *data;
#endif
	cvrElement *cache_element;
};


struct va_ref
{
	cvrElement *elements;
	unsigned int element_count;
	unsigned int group_size;
	unsigned int range_min;
	unsigned int range_max;
};


struct seg_bound
{
	XYZ bb1;
	XYZ bb2;
};

struct smoothingPair
{
	cvrIndex faceref;
	cvrIndex pointref;
};

/// Object class, provides a structure and helper commands for constructing 3D objects
/*
 *	Collection of points, faces, describe an object in 3D space. State based logic or
 *	specific references can be used in the creation of the object.
 */
 
 
class IMPEXP Object : public Resource
{
private:
	/// used internally to make sure we have enough points for a given request
	bool ensurePoints(cvrIndex pointNum);
	/// used internally to make sure we have enough faces for a given request
	bool ensureFaces(cvrIndex faceNum);
	/// used internally for face/material reference switching
//	void reflistRemove(cvrIndex faceNum);
	void reflistAdd(cvrIndex faceNum);
		
public:
	/// points vector stores the points for this object
	std::vector<XYZ *> points;

	/// faces stores the Faces for this object
	std::vector<Face *> faces;

	std::vector<bool> isPatch;
	std::vector<cvrIndex> patches;
	std::vector<float> weights;
	
	/// cached object bounds
	XYZ bbMin,bbMax;
	
	/// cache state
	bool cache_state;
	/// if true we've calculated the binormals and tangents for the faces and they will be included in the cache data
	bool tangent_binormal_state;
	/// is object buffered on card?
	bool buffer_state;
	/// if set, there's currently a buffer open for writing and this is it's address
	char *open_buffer;
	
	bool hasColorMap;

	int numSegments;
	int activeSegment;

#ifdef ARCH_PSP
	unsigned char pointCacheOffset;
	unsigned char normalCacheOffset;
	unsigned char uvCacheOffset;
	unsigned char colorMapOffset;
	unsigned char dataOfs;
#else
	unsigned long pointCacheOffset;
	unsigned long normalCacheOffset;
	unsigned long uvCacheOffset;
	unsigned long colorMapOffset;
#endif
	
	va_rgb *dynamic_colors;
	
//#ifndef OPENGL_ES|ARCH_PSP
//	unsigned long binormalCacheOffset;
//	unsigned long tangentCacheOffset;
//#endif
	BITSET *segmentMask;
	

//#ifdef ARCH_PSP
//	std::map<Material *, std::map<cvrIndex, char *, ltindex> > psp_reflist;
//	std::map<Material *, std::map<cvrIndex, unsigned int, ltindex> > psp_elemcount;
//	std::map<cvrIndex, std::deque<char *> > vertex_cacheref;
//#else
	va_block cache_data;
	map<Material *, map<unsigned int, map<cvrIndex, va_ref, ltindex>, ltuint > > mat_cache_data;
	map<cvrIndex, set<cvrIndex> > vertex_cacheref;	
//#endif

	std::vector< std::vector< std::vector< smoothingPair> > > point_smoothCache;

	///	mat_reflist stores material/face reference lists for storing face pointers by materials to avoid heavy texture context switching
	/*
	 *  mat_reflist -- works like this, map key is set to the material index from the global materials in 
	 *  the Material class, and faces are added to the set which resides at that key if they use that material
	 *  keys are created as needed while binding faces, maps and sets make this an inexpensive luxury :)
	 *
	 *	This allows us to iterate through the map, pull the .first keyname as the material reference
	 *  and the .second as the set of face indicies which we need to draw for this material
	 *
	 *	map [ Material * ] [ Segment ] [ Face Type ] [ Face List ]
	 *
	 *	[ Face Type ] = 0 - null face, 1 - points, 2 - line, 3 - triangles, 4 - quads, 5+ polys.
	 *
	 */
	map<Material *, map< unsigned int, map<unsigned short, set<cvrIndex>, ltushort>, ltuint > > mat_reflist;
	
	
	
	Object();
	~Object();
		

	
	std::string modelFile;
	std::string originalFile;

	// Resource Interface
	
	static Resource *upcast(ResourceManager *rm_in, Resource *res_in);
	virtual bool onLoad();
	virtual bool onSave();
	virtual bool init();
	virtual bool destroy();
	virtual bool reload();
	
	
	string getModelFile();
	void setModelFile(string modelFile_in);
	
	string getOriginalFile();
	void setOriginalFile(string originalFile_in);

	
	
	// clear the cache and contents of this object
	void clean();
	
	
	map <char *, int, string_less> seg_ref;
	map <int, seg_bound> seg_bounds;
	
	void setSegment(int segment_id, char *seg_name = NULL);
	int getSegmentId(char *seg_name = NULL);
	void calcSegmentBounds();
	void getSegmentBounds(int segment_id, XYZ &bb1, XYZ &bb2);
	
	///	single XYZ point addition
	cvrIndex addPoint(const XYZ &xyz_in);
	/// single XYZ point addition with specified point reference
	void addPoint(cvrIndex pointNum, const XYZ &xyz_in);


	/// calculate the bounds for this object, subsequent calls cached
	void calcBB();
	/// fetch the bounds for this object
	void getBoundingBox(XYZ &bbMin, XYZ &bbMax);

//#ifndef ARCH_DC
//	/// sets whether the material mask is active for this object
//	void setMaterialMask(bool matMask);
//#endif
	
	void setPatch(cvrIndex faceNum, bool isPatchState=true);
	void setWeight(cvrIndex pointNum, float weightVal);
	inline bool hasPatches() { return (patches.size()>0); }; 
	void generatePatches();

	
	/// appends a new face
	cvrIndex addFace();
	/// appends a new face, specific facenum
	void addFace(cvrIndex faceNum);


	/// adds a point reference to the last created face
	int addFacePoint(cvrIndex pointNum);
	/// adds a point reference to a specific face reference
	void addFacePoint(cvrIndex faceNum, cvrIndex pointNum);
	/// adds a point & binds reference to a specific face reference
	void addFacePoint(cvrIndex faceNum, const XYZ &xyz_in);
	/// adds a point & binds reference to last face added
	void addFacePoint(const XYZ &xyz_in);

	/// adds a point reference to the last created face
	int addFacePoint(cvrIndex pointNum, RGB &rgb_val);
	/// adds a point reference to a specific face reference + color
	void addFacePoint(cvrIndex faceNum, cvrIndex pointNum, RGB &rgb_val);
	/// adds a point & binds reference to a specific face reference + color
	void addFacePoint(cvrIndex faceNum, const XYZ &xyz_in, RGB &rgb_val);
	/// adds a point & binds reference to last face added
	void addFacePoint(const XYZ &xyz_in, RGB &rgb_val);
	
	
	/// If point exists return point number, else -1
	int hasPoint(const XYZ &point_compare);


	void setFaceSegment(cvrIndex faceNum, int segment_id);
	
//	/// binds the last material created using Material::createMaterial to the last created face
//	void bindFaceMaterial();
	/// binds the last material created using Material::createMaterial to a specific face reference
	void bindFaceMaterial(Material *mat);
	///	binds a specific material reference to a specific face reference
	void bindFaceMaterial(cvrIndex faceNum, Material *mat);
	
	
	/// calculates the face normals for all faces and points
	void calcNormals();
	/// re-calculates face and smooth point normals using cached material smoothing reference
	void reCalcNormals();	
	
	/// create this object's point/face structure from that of another
	void cloneStructure(Object &obj);

	/// fully clone / add an object into this object (surface params & materials included)
	void cloneObject(Object &obj);
	
	
	/// build mat_reflist
	void buildRefList();
	void calcGroupSizes();
	
	/// set cache state, used for optimizing rendering performance, if point/normal modification/update needed set dynamic_buffer to true
//#ifdef ARCH_PSP
//	void cache(bool cache_state_in, bool dynamic_buffer = false);
//#else
	void cache(bool cache_state_in, bool dynamic_cache = false, bool vertex_buffer=true); // , bool tangent_binormal_calc = false
//#endif
	
	/// do a full copy cache update, points? normals?
	void fullCopyCache(bool doPoints=true, bool doNormals=true);

	/// Initiate a cache update for pointNum
	void updatePointCache(cvrIndex pointNum);
	/// Set normal value and initiate a cache update for pointNum
	void updatePointNormalCache(cvrIndex pointNum, XYZ &normal_val);
	/// Set normal value and initiate a cache update for pointNum (subpoint for smoothed surfaces)
	void updatePointNormalCache(cvrIndex pointNum, int subPoint, XYZ &normal_val);
	/// copy bind_colors to the cache_colors (and upload if need be)
	void updateColorCache();
	
	/// clear face faceNum from the object
	void clearFace(cvrIndex faceNum);
	
	/// duplicate face srcFaceNum to dstFaceNum 
	void copyFace(cvrIndex srcFaceNum, cvrIndex dstFaceNum);


#ifndef ARCH_PSP
#ifndef OPENGL_ES
#ifndef ARCH_DC
	/// convert any polygons (4+ points) into triangles
	void triangulate();
#endif
#endif
#endif	
	
	// open the dynamic buffer for cache updates
	void openBuffer(bool fullCopy=false);
	// close the dynamics buffer for cache updates
	void closeBuffer(bool fullCopy=false); 
	
	inline void applyTransform(float m[16])
	{
		vector<XYZ*>::iterator points_i;
		for (points_i = points.begin(); points_i != points.end(); points_i++)
		{
			matTransform(m,**points_i,**points_i);
		}
	}
	
//#ifndef OPENGL_ES|ARCH_PSP
//	// build attribute arrays for tangents and binormals
//	void buildTangentBinormalAttribs();
//#endif

};


#include <CubicVR/objectLWO.h>

#endif

