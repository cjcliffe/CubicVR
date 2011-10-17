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

#include "CubicVR/objectLWO.h"

#include "CubicVR/map_string.h"
#include <iomanip>
#include "CubicVR/UVMapper.h"

/* Warning :: some portions are in a state of chaos ;) */

#define CHAN_COLR 1
#define CHAN_REFL 2
#define CHAN_BUMP 3
#define CHAN_TRAN 4
#define CHAN_SPEC 5
#define CHAN_LUMI 6
#define CHAN_DIFF 7


struct mapPair
{
	unsigned int faceref;
	unsigned int pointref;
};





IMPEXP void getResourcesLWO(vector<string> &fnList, const string &fn)
{
	IFFReader lwo_data;
	vector<string> clips;
	
	int slashPos = fn.find_last_of('/');
	
	string baseDir = fn.substr(0,slashPos+1);
	string lwoFn = fn.substr(slashPos+1,fn.length()-(slashPos+1));
	
	lwo_data.load(fn);
	
	/* Image Clips */
	IFFBlock *clipData;
	unsigned int clipNum = 0;
	
	while ( (clipData = lwo_data.getBlock("CLIP",clipNum++)) )
	{
		IFFReader clipTemp = IFFReader();

		clipTemp.processIFFBlock(clipData->data+4, clipData->size-4,2);
		
		IFFBlock *stilData = clipTemp.getBlock("STIL");
		if (stilData) if (stilData->size)
		{
			string strRef;
			strRef = string(stilData->data);
			fnList.push_back(strRef);
		}
	}
	
}





IMPEXP void loadLWO(Mesh &obj, const string &fn, MapString *mapStr, map<Material *,UVMapper *> *mappers_out)
{
	unsigned int i;
	IFFReader lwo_data;
	vector<string> clips;
	map<string, Material *, string_less> surftags;
	map<string, map<int, map<int, map<int, UV, int_less>, int_less>, int_less >, string_less> vmad_ref;	
	vector<string> taglist;

	vector<UVMapper *> mappers;
	vector<unsigned short> mapper_layer;
	vector<Material *> mapper_mat;
	
	int slashPos = fn.find_last_of('/');

	string baseDir = fn.substr(0,slashPos+1);
	string lwoFn = fn.substr(slashPos+1,fn.length()-(slashPos+1));
	
	Logger::log("Loading Object: %s\n",fn.c_str());
	
//	if (mapStr)
//	{
//		//fn = 
//		mapStr->getString(fn.c_str());
//	}
	
	FILE *file;
	
	file = fopen(fn.c_str(), "rb");

	if(!file) 
	{
		Logger::log("file not found: %s\n",fn.c_str());	
		return;
	}
	
	fclose(file);
	
	lwo_data.load(fn);

	unsigned int totalPoints = 0;
	unsigned int pointSetCount = 0;
	
	vector<unsigned int> pointSetOfs;
	
	IFFBlock *pointData;
	
	/* Points */
	while ((pointData = lwo_data.getBlock("PNTS",pointSetCount++)))
	{
		unsigned int numPoints = pointData->size/12;
		
		for (unsigned int t = 0; t < numPoints; t++)
		{
			float x,y,z;

			/* x-axis is reversed in lw? */		
			x = -pointData->getFloat();
			y = pointData->getFloat();
			z = pointData->getFloat();
			
			obj.addPoint(totalPoints+t,XYZ(x,y,z));
		}

		pointSetOfs.push_back(totalPoints);
		totalPoints += numPoints;
	}

	std::vector<string> blockList;
	lwo_data.getBlockNames(blockList);
	
	int vmadLayer = -1;
	std::vector<int> vmadIndex;
	std::map<string,vector<int> > vmadNamedIndex;
	
	for (unsigned int i = 0; i < blockList.size(); i++)
	{
		if (blockList[i] == "LAYR")
		{
			vmadLayer++;
		}
		if (blockList[i] == "VMAD")
		{
			printf("VMAD: %d\n",vmadLayer);
			vmadIndex.push_back(vmadLayer);
		}
	}
	
	int polsCount = 0;
	unsigned int faceNum = 0;
	
	vector<unsigned int> faceSetOfs;

	/* Faces */
	IFFBlock *faceData; // = lwo_data.getBlock("POLS");

	while ((faceData = lwo_data.getBlock("POLS",polsCount)))
	{
		char faceHeader[4];
		bool isPatchData=false; 
		faceData->getHeader(faceHeader);

		Logger::log("POLS type:%s\n",faceHeader);
		isPatchData = (strncmp("PTCH",faceHeader, 4) == 0);
		
		if (isPatchData) Logger::log("Loading Faces as Patches..\n");
		
		faceSetOfs.push_back(faceNum);

		obj.setSegment(polsCount);

		while (!faceData->finished())
		{
			unsigned short numPoints;
			vector<unsigned long> face_temp;
			int p;

			numPoints = faceData->getShort();
			obj.addFace(faceNum);
			face_temp.resize(numPoints);

			/* lightwave defines faces clockwise, we need counterclockwise */
			if (numPoints)
			{
				for (p = numPoints-1; p >= 0; p--)
				{
					face_temp[p] = faceData->getVx();
				}
			}
			else
			{
				Logger::log(LOG_WARNING,"Warning face %lu has no points.\n",faceNum);
			}
			
			for (p = 0; p < numPoints; p++)
			{
				obj.addFacePoint(faceNum,pointSetOfs[polsCount]+face_temp[p]);
			}				
			
			if (isPatchData) obj.setPatch(faceNum,true);

			faceNum++;
		}
		

		if (isPatchData) 		Logger::log("Patch Count: [%lu]\n",faceNum);
		else					Logger::log("Face Count: [%lu]\n",faceNum);


		polsCount++;
	}

	/* Discontinuous UV maps */
	/* UV Map */
	IFFBlock *vmadData;
	int vmadCount = 0;
	
	while ((vmadData = lwo_data.getBlock("VMAD",vmadCount)))
	{
		char vmadTag[5];
		string vmadName;
		int dimension;

		std::map<string,int> vmadSubCounter;

		if (vmadData->size)
		{
			vmadData->getHeader(vmadTag);
			vmadTag[4] = 0;

			dimension = vmadData->getShort();
			
			vmadData->getString(vmadName);
			
//			vmad_ref[vmadName].resize(obj.faces.size());
										
			Logger::log("vmad name: %s\n",vmadName.c_str());
			Logger::log("vmad type: %s\n",vmadTag);

			vmadNamedIndex[vmadName].push_back(vmadIndex[vmadCount]);

			unsigned int subCount = vmadNamedIndex[vmadName].size()-1;
			
			if (strncmp(vmadTag,"TXUV",4)==0)
			{
				while (!vmadData->finished())
				{
					unsigned int polyIndex,pointIndex;
					float u, v;

					pointIndex = vmadData->getVx();
					polyIndex = vmadData->getVx();
					
					u = vmadData->getFloat();
					v = vmadData->getFloat();							
												
					vmad_ref[vmadName][subCount][pointIndex][polyIndex] = UV(u,v);
				}
			}
			else
			{
				Logger::log("non TXUV vmad, can't apply..\n");
			}
		}
		
		vmadCount++;
	}



	/* Image Clips */
	IFFBlock *clipData;
	unsigned int clipNum = 0;

	while ( (clipData = lwo_data.getBlock("CLIP",clipNum++)) )
	{
		IFFReader clipTemp = IFFReader();

		unsigned long clipIndex = clipData->getLong();
		clipTemp.processIFFBlock(clipData->data+4, clipData->size-4,2);

		IFFBlock *stilData = clipTemp.getBlock("STIL");
		if (stilData)
		{
			string strRef;

			if (!(clips.size() > clipIndex)) clips.resize(clipIndex+1);

			strRef = string(stilData->data);

			clips[clipIndex] = strRef;
		
			Logger::log("Still Image #%lu : %s\n",clipIndex,strRef.c_str());
		}
	}


	std::vector< std::vector<mapPair> > point_mapref;
	point_mapref.resize(obj.points.size());

	/* build a list of which faces share which points */
	for (i = 0; i < obj.faces.size(); i++)	
	{
		for (unsigned int j = 0; j < obj.faces[i]->points.size(); j++)
		{
			mapPair point_map_temp;
			
			point_map_temp.faceref = i;
			point_map_temp.pointref = j;
			
			point_mapref[obj.faces[i]->pointref[j]].push_back(point_map_temp);
		}
	}


	/* Surfaces (SURF) */
	int surfCount = 0;
	IFFBlock *surfaceData;

	while ((surfaceData = lwo_data.getBlock("SURF",surfCount))) 
	{	
		RGBA color;
		Material *surfMat;
		unsigned short texLayer = 0;
		unsigned short texCount = 0;
		int reflCount = 0;
		int bumpCount = 0;

		IFFReader surfTemp = IFFReader();

		string surfName(surfaceData->data);

		int surfPtr = 0;

		Logger::log("Surface name: %s\n",surfName.c_str());

		surfPtr = surfName.size();

		while (*(surfaceData->data+surfPtr) == 0) surfPtr++;
		
		Logger::log(" size: %lu\n",(surfaceData->size-surfPtr));
		
		surfTemp.processIFFBlock(surfaceData->data+surfPtr, surfaceData->size-surfPtr,2);

		if (!surfTemp.getBlock("COLR"))
		{
			string surfName2 = surfaceData->data+surfPtr;
			
			Logger::log("Surface name2: %s\n",surfName2.c_str());
			
			surfPtr+=surfName2.size();
			while (*(surfaceData->data+surfPtr) == 0) surfPtr++;
			
			surfTemp.processIFFBlock(surfaceData->data+surfPtr, surfaceData->size-surfPtr,2);
		}
		


		
		surfMat = new Material();
		surfMat->setId(surfName);
		surftags[surfName] = surfMat;
		

		float tran = 1.0f;
		IFFBlock *tranData = surfTemp.getBlock("TRAN");
		if (tranData)
		{
			tran = 1.0f-tranData->getFloat();
		}


		IFFBlock *colorData = surfTemp.getBlock("COLR");
		if (colorData)
		{
			color.r = colorData->getFloat();
			color.g = colorData->getFloat();
			color.b = colorData->getFloat();
			color.a = tran;

			Logger::log("Surface %s color: %f, %f, %f\n",surfName.c_str(),color.r,color.g,color.b);
			surfMat->setColor(color);			
		}

		float diff = 1.0f;
		IFFBlock *diffData = surfTemp.getBlock("DIFF");
		if (diffData)
		{
			diff = diffData->getFloat();
		}
		surfMat->setDiffuse(RGBA(diff,diff,diff,1.0f)); 

		Logger::log("diff: %f\n",diff);

		float lumi = 0.0f;
		IFFBlock *lumiData = surfTemp.getBlock("LUMI");
		if (lumiData)
		{
			lumi = lumiData->getFloat(); 
			
			Logger::log(", lumi amount: %f ",lumi);
		}
		surfMat->setAmbient(RGBA(color.r*lumi,color.g*lumi,color.b*lumi,1.0f));
		

		float spec = 0.0f;
		IFFBlock *specData = surfTemp.getBlock("SPEC");
		if (specData)
		{
			spec = specData->getFloat(); 

			Logger::log(", specular: %f ",spec);
		}
		surfMat->setSpecular(RGBA(spec,spec,spec,1.0f));
		
		float glos = 0.40f;
		IFFBlock *glosData = surfTemp.getBlock("GLOS");
		if (glosData)
		{
			glos = glosData->getFloat();

			Logger::log(", shininess: %f",glos);
		}
		surfMat->setShininess(128.0f*glos);


		float max_smooth = 0.0f;
		IFFBlock *smoothData = surfTemp.getBlock("SMAN");
		if (smoothData)
		{
			max_smooth = smoothData->getFloat();

			Logger::log(", smoothing: %f",max_smooth*180.0f/M_PI);
		}
		surfMat->setMaxSmooth(max_smooth*180.0f/M_PI);


		float refl = 0.0f;
		IFFBlock *reflData = surfTemp.getBlock("REFL");
		if (reflData)
		{
			refl = reflData->getFloat();

			Logger::log(", reflection: %f\n",refl);
			
			surfMat->setReflective(refl);
		}
//		surfMat->env_opacity(refl);

		unsigned short sidedness = 1;
		IFFBlock *sideData = surfTemp.getBlock("SIDE");
		if (sideData)
		{
			sidedness = sideData->getShort();
		}
		surfMat->setSidedness((sidedness==1)?MATERIAL_FACE_FRONT:MATERIAL_FACE_BOTH);

		/* BLOK Sub-chunks */
		int blokCount = 0;

		std::vector<unsigned short> layer_order_ref;

		IFFBlock *surfBlok = surfTemp.getBlock("BLOK");
		while ((surfBlok = surfTemp.getBlock("BLOK",blokCount)))
		{
			IFFReader blokData;
			blokData.processIFFBlock(surfBlok->data, surfBlok->size,2);

			/* IMAP Blok -- we only handle IMAPs (for now) */
			IFFBlock *imap = blokData.getBlock("IMAP");
			char chanHeader[5] = "COLR";

			if (imap)
			{
				string imapStr;
				imap->getString(imapStr); // ordinal string

				IFFReader imapData;
				imapData.processIFFBlock(imap->data_ptr(), imap->remainder(),2);
				IFFBlock *chan = imapData.getBlock("CHAN");

				chan->getHeader(chanHeader);

				if (strncmp(chanHeader,"REFL",4) == 0) layer_order_ref.push_back(CHAN_REFL);
				if (strncmp(chanHeader,"DIFF",4) == 0) layer_order_ref.push_back(CHAN_COLR);
				if (strncmp(chanHeader,"COLR",4) == 0) layer_order_ref.push_back(CHAN_COLR);
				if (strncmp(chanHeader,"BUMP",4) == 0) layer_order_ref.push_back(CHAN_BUMP);
				if (strncmp(chanHeader,"SPEC",4) == 0) layer_order_ref.push_back(CHAN_SPEC);
				if (strncmp(chanHeader,"LUMI",4) == 0) layer_order_ref.push_back(CHAN_LUMI);
				if (strncmp(chanHeader,"TRAN",4) == 0) layer_order_ref.push_back(CHAN_TRAN);
			}
			blokCount++;
		}
		
		// sort order for layers
		std::vector<unsigned short> layer_order_map;
		layer_order_map.push_back(CHAN_COLR);
		layer_order_map.push_back(CHAN_TRAN);
		layer_order_map.push_back(CHAN_REFL);
		layer_order_map.push_back(CHAN_BUMP);
		layer_order_map.push_back(CHAN_SPEC);
		layer_order_map.push_back(CHAN_LUMI);
		layer_order_map.push_back(CHAN_DIFF);
		
		
		// stack the layers based on the sort order
		std::vector<unsigned short> layer_order_sorted;
		
		for (i = 0; i < layer_order_map.size(); i++)
		{
			for (unsigned int j = 0; j < layer_order_ref.size(); j++)
			{
				if (layer_order_map[i] == layer_order_ref[j])
				{
					layer_order_sorted.push_back(j);
				}
			}
		}
		
		// assign the stacked layers their index in the stack
		std::vector<unsigned short> layer_order;
		
		layer_order.resize(layer_order_sorted.size());
		
		for (i = 0; i < layer_order_sorted.size(); i++)
		{
			layer_order[layer_order_sorted[i]] = i;
		}
		

		blokCount = 0;
	
		while ((surfBlok = surfTemp.getBlock("BLOK",blokCount)))
		{
			IFFReader blokData;
			blokData.processIFFBlock(surfBlok->data, surfBlok->size,2);

			unsigned long newTex = 0;

			/* IMAP Blok -- we only handle IMAPs (for now) */
			IFFBlock *imap = blokData.getBlock("IMAP");
			char chanHeader[5] = "COLR";
			int chanType = -1;

			if (imap)
			{
				string imapStr;
				imap->getString(imapStr); // ordinal string

				IFFReader imapData;
				imapData.processIFFBlock(imap->data_ptr(), imap->remainder(),2);
				IFFBlock *chan = imapData.getBlock("CHAN");
				
				chan->getHeader(chanHeader);

				if (strncmp(chanHeader,"REFL",4) == 0) chanType = CHAN_REFL;
				if (strncmp(chanHeader,"COLR",4) == 0) chanType = CHAN_COLR;
				if (strncmp(chanHeader,"BUMP",4) == 0) chanType = CHAN_BUMP;//{ blokCount++; continue; }
				if (strncmp(chanHeader,"SPEC",4) == 0) chanType = CHAN_SPEC;
				if (strncmp(chanHeader,"DIFF",4) == 0) chanType = CHAN_COLR;
				if (strncmp(chanHeader,"TRAN",4) == 0) chanType = CHAN_TRAN;
				if (strncmp(chanHeader,"LUMI",4) == 0) chanType = CHAN_LUMI;

//				if (chanType == -1) break;
				/* Texture IMAG block */
				UVMapper *mapper = new UVMapper();

				IFFBlock *imag = blokData.getBlock("IMAG");
				if (imag)
				{
					int temp;
					long clip_index = IFFBlock::vxbuf2vx(imag->data,temp);

					if (clip_index <= 0) break;
					if (clips[clip_index] == "") break;					
					
					string texFn;
					
					if (mapStr)
					{			
						texFn.append(mapStr->getString(string(strrchr(clips[clip_index].c_str(),'/')+1).c_str()));
					}
					else
					{
						texFn = baseDir;
						
						texFn.append(string(strrchr(clips[clip_index].c_str(),'/')+1).c_str());
					}

					Logger::log("Clip Index: %li : %s\n",clip_index,texFn.c_str());

					newTex = Texture::getTextureId(texFn);					
					
					Logger::log("Loading clip image file..\n");
					if (!newTex) newTex = Texture::create(texFn,texFn);

					Logger::log("Loading surface parameters..\n");

					if (!newTex) Logger::log(LOG_ERROR,"Error loading texture %s\n",texFn.c_str());
					if (!newTex) break;
					
					texLayer = layer_order[texCount];
					surfMat->addLayer(texLayer);

					switch (chanType)
					{
						case CHAN_BUMP:
							if (bumpCount)
							{
								Logger::log("Channel type: BUMP-parallaxHeight\n");
								surfMat->bindTexture(texLayer,newTex,TEXTURE_BUMP);
							}
							else
							{
								Logger::log("Channel type: BUMP-normal\n");
								surfMat->bindTexture(texLayer,newTex,TEXTURE_NORM);
								bumpCount++;
							}
						break;
						case CHAN_SPEC:
							Logger::log("Channel type: SPECULAR\n");
							surfMat->bindTexture(texLayer,newTex,TEXTURE_SPEC);
						break;
						case CHAN_COLR:
							Logger::log("Channel type: COLOR\n");
							surfMat->bindTexture(texLayer,newTex,TEXTURE_DIFF);
						break;
						case CHAN_LUMI:
							Logger::log("Channel type: LUMI\n");
							surfMat->bindTexture(texLayer,newTex,TEXTURE_LUMI);
						break;
						case CHAN_REFL:
							Logger::log("Channel type: REFL\n");

							if (reflCount)
							{
								Logger::log("Channel type: REFL-material\n");

								surfMat->bindTexture(texLayer,newTex,TEXTURE_REFL);
							}
							else
							{
								Logger::log("Channel type: REFL-map\n");

								surfMat->bindTexture(texLayer,newTex,TEXTURE_ENVSPHERE);
							}
							
//							surfMat->layer[texLayer].map_mode = MATERIAL_MAP_SPHERICAL;
//							surfMat->bindTexture(texLayer,newTex,TEXTURE_DIFF);
							reflCount++;
						break;
						case CHAN_TRAN:
//warning undef behavior //
							surfMat->bindTexture(texLayer,newTex,TEXTURE_ALPHA);
							Logger::log("Channel type: TRAN\n");
//warning undef behavior //
							surfMat->layer[texLayer].blend_mode = MATERIAL_BLEND_ALPHA;
						break;
					}
					
	/*				if (chanType == CHAN_BUMP)
					{
					}*/
					
					
					IFFBlock *opac = imapData.getBlock("OPAC");
					if (opac)
					{
						unsigned int blend_mode = opac->getShort();
						Logger::log("Layer %d blending mode: ",texLayer);

						switch (blend_mode)
						{
							case MATERIAL_BLEND_NORMAL:
								Logger::log("Normal\n");
							break;
							case MATERIAL_BLEND_SUBTRACTIVE:
								Logger::log("Subtractive\n");
							break;
							case MATERIAL_BLEND_DIFFERENCE:
								Logger::log("Difference\n");
							break;
							case MATERIAL_BLEND_MULTIPLY:
								Logger::log("Multiply\n");
							break;
							case MATERIAL_BLEND_DIVIDE:
								Logger::log("Divide\n");
							break;
							case MATERIAL_BLEND_ALPHA:
								Logger::log("Alpha\n");
							break;
							case MATERIAL_BLEND_DISPLACE:
								Logger::log("Displacement\n");
							break;
							case MATERIAL_BLEND_ADDITIVE:
								Logger::log("Additive\n");
							break;
						}
						
						if (chanType != CHAN_TRAN) surfMat->layer[texLayer].blend_mode = blend_mode;
					}
					else
					{
						surfMat->layer[texLayer].blend_mode = MATERIAL_BLEND_NORMAL;
					}
					
					
					texCount++;
				}

				
				/* TMAP Sub-Chunk */
				IFFBlock *tmap = blokData.getBlock("TMAP");
				if (tmap && chanType == CHAN_COLR)
				{
					Logger::log("Processing TMAP chunk..\n");
					
					IFFReader tmapData;
					tmapData.processIFFBlock(tmap->data, tmap->size,2);
					
					IFFBlock *cntr, *size, *rota;

					cntr = tmapData.getBlock("CNTR");
					size = tmapData.getBlock("SIZE");
					rota = tmapData.getBlock("ROTA");

					XYZ v_center, v_size, v_rota;

					if (cntr) 
					{
						cntr->getVector(v_center);
						mapper->center = v_center;
					}

					if (size) 
					{
						size->getVector(v_size);
						mapper->scale = v_size;
					}
					
					if (rota) 
					{
						rota->getVector(v_rota);
						mapper->rotation.x = v_rota.y;
						mapper->rotation.y = v_rota.x;
						mapper->rotation.z = v_rota.z;
					}
					
					Logger::log("tmap_cntr: %f, %f, %f\n",v_center.x,v_center.y,v_center.z);
					Logger::log("tmap_size: %f, %f, %f\n",v_size.x,v_size.y,v_size.z);
					Logger::log("tmap_rota: %f, %f, %f\n",v_rota.x,v_rota.y,v_rota.z);

					IFFBlock *oref = tmapData.getBlock("OREF");
					if (oref)
					{
						Logger::log("tmap_oref: %s\n",oref->data);
					}

					unsigned short coord_system = 0;

					IFFBlock *csys = tmapData.getBlock("CSYS");
					if (csys)
					{
						coord_system = csys->getShort();
						Logger::log("tmap_csys: %d\n",coord_system);
					}

					unsigned short falloff_type;
					XYZ v_falloff;

					IFFBlock *fall = tmapData.getBlock("FALL");
					if (fall)
					{
						falloff_type = fall->getShort();
						fall->getVector(v_falloff);

						Logger::log("tmap_fall: %d\n",falloff_type);
						Logger::log("tmap_fall_vector: %f, %f, %f\n",v_falloff.x,v_falloff.y,v_falloff.z);
					}
					else v_falloff = XYZ(0,0,0);
				} 
				/* End TMAP */

				/* PROJ Sub-chunk (projection) */
				unsigned short projection_axis;
				unsigned short projection_mode = 0;
				//unsigned short width_wrap, height_wrap;

				/* Projection axis */
				IFFBlock *axis = blokData.getBlock("AXIS");
				if (axis)
				{
					projection_axis = axis->getShort();
					
					Logger::log("Projection axis: ");
					
					switch (projection_axis)
					{
					case 0: Logger::log("X"); projection_axis = UV_AXIS_X; break; /* X */
					case 1: Logger::log("Y"); projection_axis = UV_AXIS_Y; break; /* Y */
					case 2: Logger::log("Z"); projection_axis = UV_AXIS_Z; break; /* Z */
					}

					mapper->setAxis(projection_axis);
					
					Logger::log("\n");
				}

				/* Wrap Type */
				IFFBlock *wrap = blokData.getBlock("WRAP");
				if (wrap)
				{
					unsigned short width_wrap, height_wrap;
					
					width_wrap = wrap->getShort();
					height_wrap = wrap->getShort();

					Logger::log("wrap_w: ");
					switch(width_wrap)
					{
						case 0: Logger::log("reset"); break;  // Reset 
						case 1: Logger::log("repeat"); break;  // Repeat 
						case 2: Logger::log("mirror"); break;  // Mirror 
						case 3: Logger::log("edge"); break;  // Edge 
					}

					Logger::log(", wrap_h: ");
					switch(height_wrap)
					{
						case 0: Logger::log("reset"); break;  // Reset 
						case 1: Logger::log("repeat"); break;  // Repeat 
						case 2: Logger::log("mirror"); break;  // Mirror 
						case 3: Logger::log("edge"); break;  // Edge 
					}
					Logger::log("\n");
				}

				/* Width and height wrap */
				float width_count = 1.0f, height_count = 1.0f;

				IFFBlock *wrapw = blokData.getBlock("WRPW");
				if (wrapw)
				{
					width_count = wrapw->getFloat();
				}

				IFFBlock *wraph = blokData.getBlock("WRPH");
				if (wraph)
				{
					height_count = wraph->getFloat();
				}
				
				mapper->setWrap(width_count,height_count);

				IFFBlock *proj = blokData.getBlock("PROJ");

				if (proj && chanType == CHAN_COLR)
				{
					unsigned int proj_temp;
					
					proj_temp = proj->getShort();

					Logger::log("Projection mode: ");

					switch (proj_temp)
					{
					case 0:	/* Planar */
						projection_mode = UV_PROJECTION_PLANAR;	
						Logger::log("planar");
						break;
					case 1: /* Cylindrical */
						projection_mode = UV_PROJECTION_CYLINDRICAL;	
						Logger::log("cylindrical");
						break;
					case 2: /* Spherical */
						projection_mode = UV_PROJECTION_SPHERICAL;	
						Logger::log("spherical");
						break;	
					case 3: /* Cubic */
						projection_mode = UV_PROJECTION_CUBIC;	
						Logger::log("cubic");
						break;	
	/*				case 4: // Front Projection
						//if (chanType == CHAN_REFL) projection_mode = TEX_PROJ_ENVFRONT;	
						if (chanType == CHAN_COLR) projection_mode = TEX_PROJ_FRONT;	
						Logger::log("front");
						break;	*/
					case 5: /* UV */
						projection_mode = UV_PROJECTION_UV;	
						Logger::log("uv");
						break;	

					}
					Logger::log("\n");

					mapper->setProjection(projection_mode);
					
#ifdef ARCH_PSP
					if (mappers.size()==0)
#endif	
					mappers.push_back(mapper);

					mapper_layer.push_back(texLayer);
					mapper_mat.push_back(surfMat);
				}
				
				
				int vmapCount = 0;
				
				IFFBlock *vmapData;
				
				while (vmapData = blokData.getBlock("VMAP",vmapCount))
				{
					string vmapName;
					vmapData->getString(vmapName);
					
					Logger::log("VMAP Loading: [%s]\n",vmapName.c_str());

					unsigned int subCount = 0,vLayer,startPt,endPt;

					while (subCount < vmadNamedIndex[vmapName].size())
					{
						vLayer = vmadNamedIndex[vmapName][subCount];
						startPt = pointSetOfs[vLayer];
						endPt = pointSetOfs[vLayer];
					
						
						if (vLayer+1 < pointSetOfs.size())
						{
							endPt = pointSetOfs[vLayer+1];
						}
						else
						{
							endPt = obj.points.size();
						}
						
						Logger::log("VMAP start,end,layer: [%d,%d,%d]\n",startPt,endPt,vLayer);
						
						for (unsigned int j = startPt; j < endPt; j++)
						{		
							unsigned int pointIndex = j-pointSetOfs[vLayer];
							
							for (unsigned int i = 0; i < point_mapref[j].size(); i++)
							{												
								unsigned int facenum,facepoint;
								
								facenum = point_mapref[j][i].faceref;
								facepoint = point_mapref[j][i].pointref;
								
								if (!(obj.faces[facenum]->uv.size()))
								{
									obj.faces[facenum]->uv.resize(1);
									obj.faces[facenum]->uv[0].resize(obj.faces[facenum]->points.size());
								}
								
								if (obj.faces[facenum]->pointref[facepoint] == j)
								{
									if (vmad_ref[vmapName][subCount][pointIndex].find(facenum-faceSetOfs[vLayer]) != vmad_ref[vmapName][subCount][pointIndex].end())
									{
										obj.faces[facenum]->uv[0][facepoint] = vmad_ref[vmapName][subCount][pointIndex][facenum-faceSetOfs[vLayer]];
									}
								}
							}							
								
						}
						subCount++;	
					}
					vmapCount++;
				}
			}
			/* End if IMAP */
			
			/* UV Map */
			int vmapCount = 0;

			IFFBlock *vmapData;
	
			while ((vmapData = lwo_data.getBlock("VMAP",vmapCount)))
			{
				vmapCount++;

				Logger::log("VMAP: [%s]\n",vmapData->data);
			
				char vmapTag[5];
				string vmapName;
				int dimension;

				vmapData->getHeader(vmapTag);
				vmapTag[4] = 0;

				dimension = vmapData->getShort();
				vmapData->getString(vmapName);

				Logger::log("VMAP type: [%s]\n",vmapTag);
				
				
				
				if (strncmp(vmapTag,"TXUV",4)==0)
				{
					Logger::log("VMAP: decoding [%s]\n",vmapData->data);

					while (!vmapData->finished())
					{
						unsigned int pointIndex;
						float u, v;

						pointIndex = vmapData->getVx();
						
						u = vmapData->getFloat();
						v = vmapData->getFloat();

						unsigned int subCount = 0,vLayer,startPt,endPt;
						
						while (subCount < vmadNamedIndex[vmapName].size())
						{
							vLayer = vmadNamedIndex[vmapName][subCount];
							startPt = pointSetOfs[vLayer];
							endPt = pointSetOfs[vLayer];							
							
							for (unsigned int i = 0; i < point_mapref[pointIndex+startPt].size(); i++)
							{												
								unsigned int facenum,facepoint;
								
								facenum = point_mapref[pointIndex+startPt][i].faceref;
								facepoint = point_mapref[pointIndex+startPt][i].pointref;
													
								if (!(obj.faces[point_mapref[pointIndex+startPt][i].faceref]->uv.size()))
								{
									obj.faces[facenum]->uv.resize(1);
									obj.faces[facenum]->uv[0].resize(obj.faces[facenum]->points.size());
								}

								if (obj.faces[facenum]->pointref[facepoint] == pointIndex+startPt)
								{
									if (vmad_ref[vmapName][subCount][pointIndex].find(facenum-faceSetOfs[vLayer]) != vmad_ref[vmapName][subCount][pointIndex].end())
									{
										obj.faces[facenum]->uv[0][facepoint] = vmad_ref[vmapName][subCount][pointIndex][facenum-faceSetOfs[vLayer]];
									}
									else
									{
										obj.faces[facenum]->uv[0][facepoint] = UV(u,v);
									}
								}
							}
							subCount++;
						}
					}
				}
				else
				if (strncmp(vmapTag,"MNVW",4)==0)
				{
					Logger::log("Loading subpatch weights..\n");
					
					while (!vmapData->finished())
					{
						unsigned int pointIndex;
						float w;
						
						pointIndex = vmapData->getVx();						
						w = vmapData->getFloat();
						
						Logger::log("Vertex: %d, Weight: %f\n",pointIndex,w);
						obj.setWeight(pointIndex, w);
					}
					
					
				}
				else
				{
					Logger::log(LOG_ERROR,"non TXUV vmap [%s, %s], can't apply..\n",vmapData->data,vmapTag);
				}
			}
			/* END VMAP */
			
			
			blokCount++;
		}
		/* End BLOK */

		Logger::log("\n");
		surfCount++;
	} 
	/* End Surfaces */


	
	/* Patches (NURBS) */
	IFFBlock *patchData = lwo_data.getBlock("PTCH");
	if (patchData)
	{
		Logger::log("Found Patches\n");
//		while (!tagsData.finished())
//		{
//			string tagn;
//			tagsData->getString(tagn);
//			taglist.push_back(tagn);
//			Logger::log("Tag List [%s:%d]\n",tagn.c_str(),taglist.size());
//		}
	}


	
	

	/* Tags (names) */
	IFFBlock *tagsData = lwo_data.getBlock("TAGS");
	if (tagsData)
	{
		while (!tagsData->finished())
		{
			string tagn;
			tagsData->getString(tagn);
			taglist.push_back(tagn);
			Logger::log("Tag List [%s:%d]\n",tagn.c_str(),taglist.size());
		}
	}


	/* Surface PTAGs */
	int ptagCount = 0;
	int ptagSurfCount = 0;
	
	IFFBlock *ptagData;
			
	while (ptagData = lwo_data.getBlock("PTAG",ptagCount))
	{
		char ptagHeader[4];
		ptagData->getHeader(ptagHeader);

		if (strncmp(ptagHeader,"SURF", 4) == 0)
		{
			while (!ptagData->finished())
			{
				unsigned long polyIndex;
				unsigned short pTag;

				polyIndex = ptagData->getVx();
				pTag = ptagData->getShort();
				
				obj.bindFaceMaterial(polyIndex+faceSetOfs[ptagSurfCount],surftags[taglist[pTag]]);
			}
			ptagSurfCount++;
		}
		else if (strncmp(ptagHeader,"PART", 4) == 0)
		{
			string strVal;
			
			Logger::log(LOG_WARNING,"Part Name: %s\n");
//			while (!ptagData->finished())
//			{
//				unsigned long polyIndex;
//				unsigned short pTag;
//				
//				polyIndex = ptagData->getVx();
//				pTag = ptagData->getShort();
//				
//				obj.bindFaceMaterial(polyIndex+faceSetOfs[ptagSurfCount],surftags[taglist[pTag]]);
//			}
//			ptagSurfCount++;
		}
		else
		{
			Logger::log(LOG_WARNING,"Unhandled PTAG: %s\n",ptagHeader);
		}

		ptagCount++;
	}

	obj.calcNormals();

	for (unsigned int j = 0; j < mappers.size(); j++)
	{
		mappers[j]->apply(obj,mapper_mat[j],mapper_layer[j]);
		if (mappers_out && mapper_layer[j]==0)
		{
			(*mappers_out)[mapper_mat[j]] = mappers[j];
		}
		else
		{
			delete mappers[j];
		}
	}
	
}
