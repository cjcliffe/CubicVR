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

#include <CubicVR/UVMapper.h>

UVMapper::UVMapper() : rotation(0,0,0), scale(1,1,1), center(0,0,0), projection_mode(UV_PROJECTION_PLANAR), projection_axis(UV_AXIS_X), wrap_w_count(1), wrap_h_count(1)
{
	
}


UVMapper::~UVMapper()
{

}

void UVMapper::apply(Object &obj, Material *materialRef)
{
	apply(obj, materialRef, 0);
}


void UVMapper::apply(Object &obj, Material *materialRef, unsigned short layerRef)
{
	float u,v,s,t;
	Vector normal;

	map<unsigned short, set<cvrIndex>, ltushort>::iterator h;
	/* i -- used to iterate through the set of faces which belong to the given material */
	set<cvrIndex>::iterator i;
	/* j -- used to step through the points of the face which is referenced by i */
	vector<cvrIndex>::iterator j;
	unsigned int seg;
	
	obj.buildRefList();
	if (obj.mat_reflist[materialRef].empty()) return;

	/* calculate the uv for the points referenced by this face's pointref vector */
	switch (projection_mode)
	{
	case UV_PROJECTION_CUBIC: /* cubic projection needs to know the surface normal */

		for (seg = 0; seg < obj.mat_reflist[materialRef].size(); seg++)
		{
			for (h = obj.mat_reflist[materialRef][seg].begin(); h != obj.mat_reflist[materialRef][seg].end(); h++)
			{	
				for (i = (*h).second.begin(); i != (*h).second.end(); i++)
				{
					/* check and see if the uv's are allocated, if not, allocate them */
					if ((unsigned short)obj.faces[(*i)]->uv.size() < layerRef+1) obj.faces[(*i)]->uv.resize(layerRef+1);
					if (obj.faces[(*i)]->uv[layerRef].size() != obj.faces[(*i)]->pointref.size())	obj.faces[(*i)]->uv[layerRef].resize(obj.faces[(*i)]->pointref.size());

					cvrIndex pt_count = 0;	/* used to keep track of which UV we're calculating (since pointrefs aren't necessicarily in order) */
					for (j = obj.faces[(*i)]->pointref.begin(); j != obj.faces[(*i)]->pointref.end(); j++)
					{
						XYZ uvpoint = *obj.points[*j];

						uvpoint -= center;	/* shift the point by the centerpoint */
						
						/* if we have a rotation, rotate the point to match it */
						if (rotation.x || rotation.y || rotation.z)	
						{
							XYZ npoint = uvpoint;
							rotatexyz(rotation,uvpoint,npoint);
							uvpoint = npoint;
						}

						/* first we need to check what the most 'dominant' direction of this face is (axis) */
						
						float nx, ny, nz;
						
						nx = fabs(obj.faces[(*i)]->face_normal.x);
						ny = fabs(obj.faces[(*i)]->face_normal.y);
						nz = fabs(obj.faces[(*i)]->face_normal.z);
						
						/* x portion of vector is dominant, we're mapping in the Y/Z plane */
						if (nx >= ny && nx >= nz)
						{
							/* we use a .5 offset because texture coordinates range from 0->1, so to center it we need to offset by .5 */
							s = uvpoint.z / scale.z + 0.5f;	/* account for scale here */
							t = uvpoint.y / scale.y + 0.5f;
							u = fract(s);
							v = fract(t);
						}

						/* y portion of vector is dominant, we're mapping in the X/Z plane */
						if (ny >= nx && ny >= nz)
						{
							
							s = -uvpoint.x / scale.x + 0.5f;
							t = uvpoint.z / scale.z + 0.5f;
							u = fract(s);
							v = fract(t);
						}

						/* z portion of vector is dominant, we're mapping in the X/Y plane */
						if (nz >= nx && nz >= ny)
						{
							s = -uvpoint.x / scale.x + 0.5f;
							t = uvpoint.y / scale.y + 0.5f;
							u = fract(s);
							v = fract(t);
						}

						if (obj.faces[(*i)]->face_normal.x > 0) { u = -u;  }
						if (obj.faces[(*i)]->face_normal.y < 0) { u = -u;  }
						if (obj.faces[(*i)]->face_normal.z > 0) { u = -u;  }
						
						obj.faces[(*i)]->uv[layerRef][pt_count].u = u;
						obj.faces[(*i)]->uv[layerRef][pt_count].v = v;
						pt_count++; /* next point UV please */
					}
				}
			}
		}
		break;

	default:	/* simple XYZ to UV calc will suffice for non-cubic mapping */

		for (seg = 0; seg < obj.mat_reflist[materialRef].size(); seg++)
		{
			for (h = obj.mat_reflist[materialRef][seg].begin(); h != obj.mat_reflist[materialRef][seg].end(); h++)
			{	
				for (i = (*h).second.begin(); i != (*h).second.end(); i++)
				{
					if ((unsigned short)obj.faces[(*i)]->uv.size() < layerRef+1) obj.faces[(*i)]->uv.resize(layerRef+1);
					if (obj.faces[(*i)]->uv[layerRef].size() != obj.faces[(*i)]->pointref.size())	obj.faces[(*i)]->uv[layerRef].resize(obj.faces[(*i)]->pointref.size());
			
					cvrIndex pt_count = 0;	
					for (j = obj.faces[(*i)]->pointref.begin(); j != obj.faces[(*i)]->pointref.end(); j++)
					{
						getUV(*obj.points[*j], obj.faces[*i]->uv[layerRef][pt_count++]);	/* map it, see getUV */
					}
				}
			}
		}
		break;
	}
};


/* texture rotate tool, basic XYZ rotation */
void UVMapper::rotatexyz(XYZ &rot_in, XYZ &point_in, XYZ &point_out)
{
  float x1,y1,z1;
  
	x1 = cos(rot_in.y) * point_in.x - sin(rot_in.y) * point_in.z;
	z1 = sin(rot_in.y) * point_in.x + cos(rot_in.y) * point_in.z;
	point_out.x = cos(rot_in.z) * x1 + sin(rot_in.z) * point_in.y;
	y1 = cos(rot_in.z) * point_in.y - sin(rot_in.z) * x1;
	point_out.z = cos(rot_in.x) * z1 - sin(rot_in.x) * y1;
	point_out.y = sin(rot_in.x) * z1 + cos(rot_in.x) * y1;
};


void UVMapper::getUV(XYZ &point_in, UV &uv_out)
{
	/* s, t = texture space coordinates, lon, lat = longitude and latitude space coordinates */
	float s,t,lon,lat;
	
	XYZ uvpoint = point_in;

	uvpoint -= center;	/* shift point by centerpoint */

	/* apply our rotation (if there is one) */
	if (rotation.x || rotation.y || rotation.z)
	{
		XYZ npoint;
		rotatexyz(rotation,point_in,npoint);
		uvpoint = npoint;
	}

	/* check the projection mode and map accordingly */
	switch (projection_mode)
	{
	
	case UV_PROJECTION_PLANAR:
				/* planar, just figure out the axis and return the point from that plane & apply scale */
        s = ((projection_axis == UV_AXIS_X) ? uvpoint.z / scale.z + 0.5f : -uvpoint.x / scale.x + 0.5f);
        t = ((projection_axis == UV_AXIS_Y) ? uvpoint.z / scale.z + 0.5f : uvpoint.y / scale.y + 0.5f);
        uv_out.u = fract(s);
        uv_out.v = fract(t);
		break;

	case UV_PROJECTION_CYLINDRICAL:

		/* Cylindrical is a little more tricky, we map based on the degree around the center point */
		switch (projection_axis)
		{

			case UV_AXIS_X:
				/* xyz_to_h takes the point and returns a value representing the 'unwrapped' height position of this point */
				xyz_to_h(uvpoint.z,uvpoint.x,-uvpoint.y,&lon);
				t = -uvpoint.x / scale.x + 0.5f;
				break;

			case UV_AXIS_Y:
		    xyz_to_h(-uvpoint.x,uvpoint.y,uvpoint.z,&lon);
		    t = -uvpoint.y / scale.y + 0.5f;
				break;

	    case UV_AXIS_Z:
	      xyz_to_h(-uvpoint.x,uvpoint.z,-uvpoint.y,&lon);
	      t = -uvpoint.z / scale.z + 0.5f;
				break;

		}

		/* convert it from radian space to texture space 0 to 1 * wrap, TWO_PI = 360 degrees */
    lon = 1.0f - lon / M_TWO_PI;

    if (wrap_w_count != 1.0) lon = fract(lon) * wrap_w_count;
    uv_out.u = fract(lon);
    uv_out.v = fract(t);

		break;

	case UV_PROJECTION_SPHERICAL:
				
		/* spherical is similar to cylindrical except we also unwrap the 'width' */
		switch(projection_axis)
		{
			case UV_AXIS_X:
				/* xyz to hp takes the point value and 'unwraps' the latitude and longitude that projects to that point */
        xyz_to_hp(uvpoint.z,uvpoint.x,-uvpoint.y,&lon,&lat);
        break;
      case UV_AXIS_Y:
        xyz_to_hp(uvpoint.x,-uvpoint.y,uvpoint.z,&lon,&lat);
      	break;
      case UV_AXIS_Z:
        xyz_to_hp(-uvpoint.x,uvpoint.z,-uvpoint.y,&lon,&lat);
				break;
		}
    
    /* convert longitude and latitude to texture space coordinates, multiply by wrap height and width */
		lon = 1.0f - lon / M_TWO_PI;
    lat = 0.5f - lat / M_PI;

		if (wrap_w_count != 1.0f) lon = fract(lon) * wrap_w_count;
		if (wrap_h_count != 1.0f) lat = fract(lat) * wrap_h_count;
		
		uv_out.u = fract(lon);
		uv_out.v = fract(lat);
		
	break;

	case UV_PROJECTION_UV:
		// not handled here..
	break;

	default:	// else mapping cannot be handled here (i.e. cubic), this shouldn't have happened
			uv_out.u = 0;
			uv_out.v = 0;
		break;


	}
};


/* convert XYZ space to longitude */
void UVMapper::xyz_to_h(float x,float y,float z,float *h)
{
	if (x == 0 && z == 0)
	{
	  *h = 0;
	}
	else 
	{
	  if (z == 0)
		{
	    *h = (x < 0) ? M_HALF_PI : -M_HALF_PI;
		}
    else if (z < 0)
		{
	    *h = -atan(x / z) + M_PI;
		}
    else
		{
      *h = -atan(x / z);
		}
	}
};


/* convert XYZ space to latitude and longitude */
void UVMapper::xyz_to_hp(float x,float y,float z,float *h,float *p)
{
	if (x == 0 && z == 0) 
	{
	  *h = 0;
		if (y != 0)
		{
			*p = (y < 0) ? -M_HALF_PI : M_HALF_PI;
		}
		else
		{
			*p = 0;
		}
	}
	else 
	{
		if (z == 0)
		{
			*h = (x < 0) ? M_HALF_PI : -M_HALF_PI;
		}
		else if (z < 0)
		{
			*h = -atan(x / z) + M_PI;
		}
		else
		{
			*h = -atan(x / z);
		}

		x = sqrt(x * x + z * z);

		if (x == 0)
		{
			*p = (y < 0) ? -M_HALF_PI : M_HALF_PI;
		}
		else
		{
			*p = atan(y / x);
		}
	}
};


/* return fraction portion */
float UVMapper::fract (float x)
{
  if ((x >= 0) || (x <= 1)) return x;

  return (float)(x - floor(x));
};


void UVMapper::setProjection(unsigned short proj_in)
{
	projection_mode = proj_in;
};


void UVMapper::setAxis(unsigned short axis_in)
{
	projection_axis = axis_in;
};


void UVMapper::setWrap(float w_in, float h_in)
{
	wrap_w_count = w_in;
	wrap_h_count = h_in;
};


void UVMapper::setScale(const XYZ &scale_in)
{
	scale = scale_in;
};


void UVMapper::setRotation(const XYZ &rotation_in)
{
	rotation = rotation_in;	
};


void UVMapper::setCenter(const XYZ &center_in)
{
	center = center_in;
};
