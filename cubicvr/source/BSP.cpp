#if !defined(_PSP)
//////////////////////////////////////////////////////////////////////////////////////////
//	BSP.cpp
//	Functions for bsp file
//	Downloaded from: www.paulsprojects.net
//	Created:	8th August 2002
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	

#include <CubicVR/GLExt.h>
#include <stdio.h>
#include <memory.h>
#include <CubicVR/IMAGE.h>
#include <CubicVR/math/Maths.h>
#include <CubicVR/BSP.h>
#include <CubicVR/Texture.h>
#include <CubicVR/SectorMap.h>

////////////////////BSP::Load///////////////
////////////////////////////////////////////
bool BSP::Load(char * filename, int curveTesselation)
{
	FILE * file;

	file=fopen(filename, "rb");
	if(!file)
	{
		Logger::log(LOG_ERROR,"Unable to open %s", filename);
		return false;
	}

	//read in header
	fread(&header, sizeof(BSP_HEADER), 1, file);

	//check header data is correct
	if(	header.string[0]!='I' || header.string[1]!='B' ||
		header.string[2]!='S' || header.string[3]!='P' ||
		header.version  !=0x2E )
	{
		Logger::log(LOG_ERROR,"%s is not a version 0x2E .bsp map file", filename);
		return false;
	}


	//Load in vertices
	if(!LoadVertices(file))
		return false;


	//Load in mesh indices
	//Calculate number of indices
	int numMeshIndices=header.directoryEntries[bspMeshIndices].length/sizeof(int);

	//Create space
	meshIndices=new int[numMeshIndices];
	if(!meshIndices)
	{
		Logger::log(LOG_ERROR,"Unable to allocate memory for %d mesh indices", numMeshIndices);
		return false;
	}

	//read in the mesh indices
	fseek(file, header.directoryEntries[bspMeshIndices].offset, SEEK_SET);
	fread(meshIndices, header.directoryEntries[bspMeshIndices].length, 1, file);

	

	//Load in faces
	if(!LoadFaces(file, curveTesselation))
		return false;

	
	//Load textures
	if(!LoadTextures(file))
		return false;

		
	//Load Lightmaps
	if(!LoadLightmaps(file))
		return false;


	//Load BSP Data
	if(!LoadBSPData(file))
		return false;


	//Load in entity string
	entityString=new char[header.directoryEntries[bspEntities].length];
	if(!entityString)
	{
		Logger::log(LOG_ERROR,"Unable to allocate memory for %d length entity string",
								header.directoryEntries[bspEntities].length);
		return false;
	}

	//Go to entity string in file
	fseek(file, header.directoryEntries[bspEntities].offset, SEEK_SET);
	fread(entityString, 1, header.directoryEntries[bspEntities].length, file);

	//Output the entity string
	//printf("Entity String: %s", entityString);


	fclose(file);

	Logger::log(LOG_NOTICE,"%s Loaded successfully", filename);

	return true;
}



///////////////////BSP::LoadVertices////////
////////////////////////////////////////////
bool BSP::LoadVertices(FILE * file)
{
	//calculate number of vertices
	numVertices=header.directoryEntries[bspVertices].length/sizeof(BSP_LOAD_VERTEX);

	//Create space for this many BSP_LOAD_VERTICES
	BSP_LOAD_VERTEX * loadVertices=new BSP_LOAD_VERTEX[numVertices];
	if(!loadVertices)
	{
		Logger::log(LOG_ERROR,"Unable to allocate memory for %d BSP_LOAD_VERTEXes", numVertices);
		return false;
	}

	//go to vertices in file
	fseek(file, header.directoryEntries[bspVertices].offset, SEEK_SET);

	//read in the vertices
	fread(loadVertices, header.directoryEntries[bspVertices].length, 1, file);

	//Convert to BSP_VERTEXes
	vertices=new BSP_VERTEX[numVertices];
	if(!vertices)
	{
		Logger::log(LOG_ERROR,"Unable to allocate memory for vertices");
		return false;
	}

	for(int i=0; i<numVertices; ++i)
	{
		//swap y and z and negate z
		vertices[i].position.x=loadVertices[i].position.x;
		vertices[i].position.y=loadVertices[i].position.z;
		vertices[i].position.z=-loadVertices[i].position.y;

		//scale down
		vertices[i].position/=64;

		//Transfer texture coordinates (Invert t)
		vertices[i].decalS=loadVertices[i].decalS;
		vertices[i].decalT=-loadVertices[i].decalT;

		//Transfer lightmap coordinates
		vertices[i].lightmapS=loadVertices[i].lightmapS;
		vertices[i].lightmapT=loadVertices[i].lightmapT;
	}

	if(loadVertices)
		delete [] loadVertices;
	loadVertices=NULL;

	return true;
}

///////////////////BSP::LoadFaces///////////
////////////////////////////////////////////
bool BSP::LoadFaces(FILE * file, int curveTesselation)
{
	//calculate number of load faces
	numTotalFaces=header.directoryEntries[bspFaces].length/sizeof(BSP_LOAD_FACE);

	//Create space for this many BSP_LOAD_FACES
	BSP_LOAD_FACE * loadFaces=new BSP_LOAD_FACE[numTotalFaces];
	if(!loadFaces)
	{
		Logger::log(LOG_ERROR,"Unable to allocate memory for %d BSP_LOAD_FACEs", numTotalFaces);
		return false;
	}

	//go to faces in file
	fseek(file, header.directoryEntries[bspFaces].offset, SEEK_SET);

	//read in the faces
	fread(loadFaces, header.directoryEntries[bspFaces].length, 1, file);


	//Create space for face directory
	faceDirectory=new BSP_FACE_DIRECTORY_ENTRY[numTotalFaces];
	if(!faceDirectory)
	{
		Logger::log(LOG_ERROR,"Unable to allocate space for face directory with %d entries",
								numTotalFaces);
		return false;
	}
	
	//Clear the face directory
	memset(faceDirectory, 0, numTotalFaces*sizeof(BSP_FACE_DIRECTORY_ENTRY));

	//Init the "faces drawn" bitset
	facesToDraw.Init(numTotalFaces);

	numMeshFaces = 0;
	numPatches = 0;
	numPolygonFaces = 0;

	//Calculate how many of each face type there is
	for(int i=0; i<numTotalFaces; ++i)
	{
		if(loadFaces[i].type==bspPolygonFace)
			++numPolygonFaces;
		if(loadFaces[i].type==bspPatch)
			++numPatches;
		if(loadFaces[i].type==bspMeshFace)
			++numMeshFaces;
	}



	//Create space for BSP_POLYGON_FACEs
	polygonFaces=new BSP_POLYGON_FACE[numPolygonFaces];
	if(!polygonFaces)
	{
		Logger::log(LOG_ERROR,"Unable To Allocate memory for BSP_POLYGON_FACEs");
		return false;
	}

	int currentFace=0;
	//convert loadFaces to polygonFaces
	for(int i=0; i<numTotalFaces; ++i)
	{
		if(loadFaces[i].type!=bspPolygonFace)		//skip this loadFace if it is not a polygon face
			continue;

		polygonFaces[currentFace].firstVertexIndex=loadFaces[i].firstVertexIndex;
		polygonFaces[currentFace].numVertices=loadFaces[i].numVertices;
		polygonFaces[currentFace].textureIndex=loadFaces[i].texture;
		polygonFaces[currentFace].lightmapIndex=loadFaces[i].lightmapIndex;

		//fill in this entry on the face directory
		faceDirectory[i].faceType=bspPolygonFace;
		faceDirectory[i].typeFaceNumber=currentFace;

		++currentFace;
	}



	//Create space for BSP_MESH_FACEs
	meshFaces=new BSP_MESH_FACE[numMeshFaces];
	if(!meshFaces)
	{
		Logger::log(LOG_ERROR,"Unable To Allocate memory for BSP_MESH_FACEs");
		return false;
	}

	int currentMeshFace=0;
	//convert loadFaces to faces
	for(int i=0; i<numTotalFaces; ++i)
	{
		if(loadFaces[i].type!=bspMeshFace)		//skip this loadFace if it is not a mesh face
			continue;

		meshFaces[currentMeshFace].firstVertexIndex=loadFaces[i].firstVertexIndex;
		meshFaces[currentMeshFace].numVertices=loadFaces[i].numVertices;
		meshFaces[currentMeshFace].textureIndex=loadFaces[i].texture;
		meshFaces[currentMeshFace].lightmapIndex=loadFaces[i].lightmapIndex;
		meshFaces[currentMeshFace].firstMeshIndex=loadFaces[i].firstMeshIndex;
		meshFaces[currentMeshFace].numMeshIndices=loadFaces[i].numMeshIndices;

		//fill in this entry on the face directory
		faceDirectory[i].faceType=bspMeshFace;
		faceDirectory[i].typeFaceNumber=currentMeshFace;

		++currentMeshFace;
	}
	



	//Create space for BSP_PATCHes
	patches=new BSP_PATCH[numPatches];
	if(!patches)
	{
		Logger::log(LOG_ERROR,"Unable To Allocate memory for BSP_PATCHes");
		return false;
	}

	int currentPatch=0;
	//convert loadFaces to patches
	for(int i=0; i<numTotalFaces; ++i)
	{
		if(loadFaces[i].type!=bspPatch)		//skip this loadFace if it is not a patch
			continue;

		patches[currentPatch].textureIndex=loadFaces[i].texture;
		patches[currentPatch].lightmapIndex=loadFaces[i].lightmapIndex;
		patches[currentPatch].width=loadFaces[i].patchSize[0];
		patches[currentPatch].height=loadFaces[i].patchSize[1];
		
		//fill in this entry on the face directory
		faceDirectory[i].faceType=bspPatch;
		faceDirectory[i].typeFaceNumber=currentPatch;

		//Create space to hold quadratic patches
		int numPatchesWide=(patches[currentPatch].width-1)/2;
		int numPatchesHigh=(patches[currentPatch].height-1)/2;

		patches[currentPatch].numQuadraticPatches=	numPatchesWide*numPatchesHigh;
		patches[currentPatch].quadraticPatches=new BSP_BIQUADRATIC_PATCH
													[patches[currentPatch].numQuadraticPatches];
		if(!patches[currentPatch].quadraticPatches)
		{
			Logger::log(LOG_ERROR,"Unable to allocate memory for %d quadratic patches", 
									patches[currentPatch].numQuadraticPatches);
			return false;
		}

		//fill in the quadratic patches
		for(int y=0; y<numPatchesHigh; ++y)
		{
			for(int x=0; x<numPatchesWide; ++x)
			{
				for(int row=0; row<3; ++row)
				{
					for(int point=0; point<3; ++point)
					{
						patches[currentPatch].quadraticPatches[y*numPatchesWide+x].
							controlPoints[row*3+point]=vertices[loadFaces[i].firstVertexIndex+
								(y*2*patches[currentPatch].width+x*2)+
									row*patches[currentPatch].width+point];
					}
				}

				//tesselate the patch
				patches[currentPatch].quadraticPatches[y*numPatchesWide+x].Tesselate(curveTesselation);
			}
		}


		++currentPatch;
	}

	if(loadFaces)
		delete [] loadFaces;
	loadFaces=NULL;

	return true;
}

///////////////////BSP::LoadTextures////////
////////////////////////////////////////////
bool BSP::LoadTextures(FILE * file)
{
	//Calculate number of textures
	numTextures=header.directoryEntries[bspTextures].length/sizeof(BSP_LOAD_TEXTURE);

	//Create space for this many BSP_LOAD_TEXTUREs
	loadTextures=new BSP_LOAD_TEXTURE[numTextures];
	if(!loadTextures)
	{
		Logger::log(LOG_ERROR,"Unable to allocate space for %d BSP_LOAD_TEXTUREs", numTextures);
		return false;
	}

	//Load textures
	fseek(file, header.directoryEntries[bspTextures].offset, SEEK_SET);
	fread(loadTextures, 1, header.directoryEntries[bspTextures].length, file);

	
	//Create storage space for that many texture identifiers
	decalTextures=new unsigned long[numTextures];
	if(!decalTextures)
	{
		Logger::log(LOG_ERROR,"Unable to create storage space for %d textures", numTextures);
		return false;
	}
	
	//Create storage space for that many texture identifiers
	decalMaterials=new unsigned long[numTextures];
	if(!decalMaterials)
	{
		Logger::log(LOG_ERROR,"Unable to create storage space for %d materials", numTextures);
		return false;
	}
	
	//Create storage space for that many booleans to tell if texture has loaded
	isTextureLoaded=new bool[numTextures];
	if(!isTextureLoaded)
	{
		Logger::log(LOG_ERROR,"Unable to create storage space for %d booleans", numTextures);
		return false;
	}
	
	isTextureTGA=new bool[numTextures];
	if(!isTextureTGA)
	{
		Logger::log(LOG_ERROR,"Unable to create storage space for %d booleans", numTextures);
		return false;
	}
	
	

	for(int i=0; i<numTextures; ++i)
	{
		decalTextures[i] = Texture::create();
		
		Texture::textures[decalTextures[i]]->use();
		
		bool isJpgTexture = false;
		//add file extension to the name
		char tgaExtendedName[68];
		char jpgExtendedName[68];
		strcpy(tgaExtendedName, loadTextures[i].name);
		strcat(tgaExtendedName, ".tga");
		strcpy(jpgExtendedName, loadTextures[i].name);
		strcat(jpgExtendedName, ".jpg");
		
		//Load texture imagek
		if(!Texture::textures[decalTextures[i]]->loadMipmap(tgaExtendedName))	//try to load .tga, if not
		{
			if(!Texture::textures[decalTextures[i]]->loadMipmap(jpgExtendedName))	//try to load jpg
			{
				isJpgTexture=true;
				isTextureTGA[i]=false;
				isTextureLoaded[i]=true;
			}
			else
			{
				isTextureLoaded[i]=true;
			}
		}
		else
		{
			isTextureTGA[i]=true;
			isTextureLoaded[i]=true;
		}
		
		//Set Parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	return true;
}

///////////////////BSP::LoadLightmaps///////
////////////////////////////////////////////
bool BSP::LoadLightmaps(FILE * file)
{
	//Calculate number of lightmaps
	numLightmaps=header.directoryEntries[bspLightmaps].length/sizeof(BSP_LOAD_LIGHTMAP);

	//Create space for this many BSP_LOAD_LIGHTMAPs
	BSP_LOAD_LIGHTMAP * loadLightmaps=new BSP_LOAD_LIGHTMAP[numLightmaps];
	if(!loadLightmaps)
	{
		Logger::log(LOG_ERROR,"Unable to allocate space for %d BSP_LOAD_LIGHTMAPs", numLightmaps);
		return false;
	}

	//Load textures
	fseek(file, header.directoryEntries[bspLightmaps].offset, SEEK_SET);
	fread(loadLightmaps, 1, header.directoryEntries[bspLightmaps].length, file);

	//Create storage space for that many texture identifiers
	lightmapTextures=new unsigned long[numLightmaps];
	if(!lightmapTextures)
	{
		Logger::log(LOG_ERROR,"Unable to create storage space for %d texture IDs", numLightmaps);
		return false;
	}
	
	//Change the gamma settings on the lightmaps (make them brighter)
	for(int i=0; i<numLightmaps; ++i)
	{
		for(int j=0; j<128*128; ++j)
		{
			float r, g, b;
			r=loadLightmaps[i].lightmapData[j*3+0];
			g=loadLightmaps[i].lightmapData[j*3+1];
			b=loadLightmaps[i].lightmapData[j*3+2];

			r*=gamma/255.0f;
			g*=gamma/255.0f;
			b*=gamma/255.0f;

			//find the value to scale back up
			float scale=1.0f;
			float temp;
			if(r > 1.0f && (temp = (1.0f/r)) < scale) scale=temp;
			if(g > 1.0f && (temp = (1.0f/g)) < scale) scale=temp;
			if(b > 1.0f && (temp = (1.0f/b)) < scale) scale=temp;

			// scale up color values
			scale*=255.0f;		
			r*=scale;
			g*=scale;
			b*=scale;

			//fill data back in
			loadLightmaps[i].lightmapData[j*3+0]=(GLubyte)r;
			loadLightmaps[i].lightmapData[j*3+1]=(GLubyte)g;
			loadLightmaps[i].lightmapData[j*3+2]=(GLubyte)b;
		}
	}

	for(int i=0; i<numLightmaps; ++i)
	{
		lightmapTextures[i] = Texture::create();

		Texture::textures[lightmapTextures[i]]->loadLightMap(loadLightmaps[i].lightmapData,128,128);
	}

	//Create white texture for if no lightmap specified
	glGenTextures(1, &whiteTexture);
	glBindTexture(GL_TEXTURE_2D, whiteTexture);
	//Create texture
//	gluBuild2DMipmaps(	GL_TEXTURE_2D, GL_RGBA8, 1, 1,
//						GL_RGB, GL_FLOAT, white);
	//Set Parameters
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if(loadLightmaps)
		delete [] loadLightmaps;
	loadLightmaps=NULL;

	return true;
}

///////////////////BSP::LoadBSPData/////////
////////////////////////////////////////////
bool BSP::LoadBSPData(FILE * file)
{
	//Load leaves
	//Calculate number of leaves
	numLeaves=header.directoryEntries[bspLeaves].length/sizeof(BSP_LOAD_LEAF);


	//Create space for this many BSP_LOAD_LEAFS
	BSP_LOAD_LEAF * loadLeaves=new BSP_LOAD_LEAF[numLeaves];
	if(!loadLeaves)
	{
		Logger::log(LOG_ERROR,"Unable to allocate space for %d BSP_LOAD_LEAFs", numLeaves);
		return false;
	}

	//Create space for this many BSP_LEAFs
	leaves=new BSP_LEAF[numLeaves];
	if(!leaves)
	{
		Logger::log(LOG_ERROR,"Unable to allocate space for %d BSP_LEAFs", numLeaves);
		return false;
	}

	//Load leaves
	fseek(file, header.directoryEntries[bspLeaves].offset, SEEK_SET);
	fread(loadLeaves, 1, header.directoryEntries[bspLeaves].length, file);

	//Convert the load leaves to leaves
	for(int i=0; i<numLeaves; ++i)
	{
		leaves[i].cluster=loadLeaves[i].cluster;
		leaves[i].firstLeafFace=loadLeaves[i].firstLeafFace;
		leaves[i].numFaces=loadLeaves[i].numFaces;

		//Create the bounding box
		leaves[i].boundingBoxVertices[0].Set((float)loadLeaves[i].mins[0], (float)loadLeaves[i].mins[2],-(float)loadLeaves[i].mins[1]);
		leaves[i].boundingBoxVertices[1].Set((float)loadLeaves[i].mins[0], (float)loadLeaves[i].mins[2],-(float)loadLeaves[i].maxs[1]);
		leaves[i].boundingBoxVertices[2].Set((float)loadLeaves[i].mins[0], (float)loadLeaves[i].maxs[2],-(float)loadLeaves[i].mins[1]);
		leaves[i].boundingBoxVertices[3].Set((float)loadLeaves[i].mins[0], (float)loadLeaves[i].maxs[2],-(float)loadLeaves[i].maxs[1]);
		leaves[i].boundingBoxVertices[4].Set((float)loadLeaves[i].maxs[0], (float)loadLeaves[i].mins[2],-(float)loadLeaves[i].mins[1]);
		leaves[i].boundingBoxVertices[5].Set((float)loadLeaves[i].maxs[0], (float)loadLeaves[i].mins[2],-(float)loadLeaves[i].maxs[1]);
		leaves[i].boundingBoxVertices[6].Set((float)loadLeaves[i].maxs[0], (float)loadLeaves[i].maxs[2],-(float)loadLeaves[i].mins[1]);
		leaves[i].boundingBoxVertices[7].Set((float)loadLeaves[i].maxs[0], (float)loadLeaves[i].maxs[2],-(float)loadLeaves[i].maxs[1]);

		for(int j=0; j<8; ++j)
			leaves[i].boundingBoxVertices[j]/=64;
	}

	
	
	//Load leaf faces array
	int numLeafFaces=header.directoryEntries[bspLeafFaces].length/sizeof(int);

	//Create space for this many leaf faces
	leafFaces=new int[numLeafFaces];
	if(!leafFaces)
	{
		Logger::log(LOG_ERROR,"Unable to allocate space for %d leaf faces", numLeafFaces);
		return false;
	}

	//Load leaf faces
	fseek(file, header.directoryEntries[bspLeafFaces].offset, SEEK_SET);
	fread(leafFaces, 1, header.directoryEntries[bspLeafFaces].length, file);


	
	//Load Planes
	numPlanes=header.directoryEntries[bspPlanes].length/sizeof(PLANE);

	//Create space for this many planes
	planes=new PLANE[numPlanes];
	if(!planes)
	{
		Logger::log(LOG_ERROR,"Unable to allocate space for %d planes", numPlanes);
		return false;
	}

	fseek(file, header.directoryEntries[bspPlanes].offset, SEEK_SET);
	fread(planes, 1, header.directoryEntries[bspPlanes].length, file);

	//reverse the intercept on the planes and convert planes to OGL coordinates
	for(int i=0; i<numPlanes; ++i)
	{
		//swap y and z and negate z
		float temp=planes[i].normal.y;
		planes[i].normal.y=planes[i].normal.z;
		planes[i].normal.z=-temp;

		planes[i].intercept=-planes[i].intercept;
		planes[i].intercept/=64;	//scale down
	}




	//Load nodes
	numNodes=header.directoryEntries[bspNodes].length/sizeof(BSP_NODE);

	//Create space for this many nodes
	nodes=new BSP_NODE[numNodes];
	if(!nodes)
	{
		Logger::log(LOG_ERROR,"Unable to allocate space for %d nodes", numNodes);
		return false;
	}

	fseek(file, header.directoryEntries[bspNodes].offset, SEEK_SET);
	fread(nodes, 1, header.directoryEntries[bspNodes].length, file);





	//Load visibility data
	//load numClusters and bytesPerCluster
	fseek(file, header.directoryEntries[bspVisData].offset, SEEK_SET);
	fread(&visibilityData, 2, sizeof(int), file);

	//Calculate the size of the bitset
	int bitsetSize=visibilityData.numClusters*visibilityData.bytesPerCluster;

	//Create space for bitset
	visibilityData.bitset=new GLubyte[bitsetSize];
	if(!visibilityData.bitset)
	{
		Logger::log(LOG_ERROR,"Unable to allocate memory for visibility bitset of size %d bytes",
								bitsetSize);
		return false;
	}
	//read bitset
	fread(visibilityData.bitset, 1, bitsetSize, file);

	if(loadLeaves)
		delete [] loadLeaves;
	loadLeaves=NULL;

	leafActive.Init(numLeaves);

	return true;
}







//BSP Functions
//calculate which leaf the camera lies in
int BSP::CalculateCameraLeaf(const VECTOR3D & cameraPosition)
{
	int currentNode=0;
	
	//loop until we find a negative index
	while(currentNode>=0)
	{
		//if the camera is in front of the plane for this node, assign i to be the front node
		if(planes[nodes[currentNode].planeIndex].ClassifyPoint(cameraPosition)==POINT_IN_FRONT_OF_PLANE)
			currentNode=nodes[currentNode].front;
		else
			currentNode=nodes[currentNode].back;
	}

	//return leaf index
	return ~currentNode;
}

//See if one cluster is visible from another
int BSP::isClusterVisible(int cameraCluster, int testCluster)
{
	int index=	cameraCluster*visibilityData.bytesPerCluster + testCluster/8;

	int returnValue=visibilityData.bitset[index] & (1<<(testCluster & 7));

	return returnValue;
}


void BSP::showAll(bool show_all_in)
{
	show_all = show_all_in;
}

void BSP::disableVIS(bool disable_vis_in)
{
	disable_vis = disable_vis_in;
}

//Calculate which faces to draw given a position & frustum
void BSP::CalculateVisibleFaces(const XYZ &cameraPosition_xyz, FRUSTUM &frustum, BITSET &segmentMask)
{
	if (disable_vis) return;
	
	//Clear the list of faces drawn
	if (single_cluster)
	{
		if (!show_all) segmentMask.ClearAll();
	}
	else
	{
		if (!show_all) leafActive.ClearAll();
	}
	
	VECTOR3D cameraPosition;
	cameraPosition.x = cameraPosition_xyz.x;
	cameraPosition.y = cameraPosition_xyz.y;
	cameraPosition.z = cameraPosition_xyz.z;
	
	//calculate the camera leaf
	int cameraLeaf=CalculateCameraLeaf(cameraPosition);
	int cameraCluster=leaves[cameraLeaf].cluster;

	//loop through the leaves
	for(int i=0; i<numLeaves; ++i)
	{
		//if the leaf is not in the PVS, continue
		if(!isClusterVisible(cameraCluster, leaves[i].cluster))
			continue;

		//if this leaf does not lie in the frustum, continue
		if(!frustum.IsBoundingBoxInside(leaves[i].boundingBoxVertices))
			continue;

		//loop through faces in this leaf and mark them to be drawn
		if (single_cluster)
		{
			for(int j=0; j<leaves[i].numFaces; ++j)
			{
				//facesToDraw.Set(leafFaces[leaves[i].firstLeafFace+j]);
				int faceNumber = leafFaces[leaves[i].firstLeafFace+j];
				
				if (faceDirectory[faceNumber].faceType==bspPolygonFace)
				{
					int polygonFaceNumber = faceDirectory[faceNumber].typeFaceNumber;
					
//					printf("polygonFaces[polygonFaceNumber].segmentIndex: %d\n",polygonFaces[polygonFaceNumber].segmentIndex);
					if (polygonFaces[polygonFaceNumber].segmentIndex)
					{
						segmentMask.Set(polygonFaces[polygonFaceNumber].segmentIndex);
					}
				}
				if (faceDirectory[faceNumber].faceType==bspPatch)
				{
					int polygonFaceNumber = faceDirectory[faceNumber].typeFaceNumber;
					
//					printf("patches[polygonFaceNumber].segmentIndex: %d\n",patches[polygonFaceNumber].segmentIndex);
					if (patches[polygonFaceNumber].segmentIndex)
					{
						segmentMask.Set(patches[polygonFaceNumber].segmentIndex);
					}
				}
				if (faceDirectory[faceNumber].faceType==bspMeshFace)
				{
					int polygonFaceNumber = faceDirectory[faceNumber].typeFaceNumber;
					
//					printf("meshFaces[polygonFaceNumber].segmentIndex: %d\n",meshFaces[polygonFaceNumber].segmentIndex);
					if (meshFaces[polygonFaceNumber].segmentIndex)
					{
						segmentMask.Set(meshFaces[polygonFaceNumber].segmentIndex);
					}
				}
			}
		}
		else
		{
			leafActive.Set(i);
		}
	}
}


/*
void BSP::buildClusterObjects()
{
	unsigned long faceMat;
	unsigned long newFace;
//	std::map<unsigned long, bool, ltulong> face_visit;

	clusterObjects.resize(numLeaves);
	std::map<cvrIndex, std::map<cvrIndex, cvrIndex, ltindex>, ltindex> materialRef;
	
	//loop through the leaves
	for(int i=0; i<numLeaves; ++i)
	{
		//loop through faces in this leaf and mark them to be drawn
		if(leaves[i].numFaces)
		{
			clusterObjects[i] = new Object();

			for(int j=0; j<leaves[i].numFaces; ++j)
			{
				int faceNumber = leafFaces[leaves[i].firstLeafFace+j];
				if(faceDirectory[faceNumber].faceType!=bspPolygonFace) continue;

				int polygonFaceNumber = faceDirectory[faceNumber].typeFaceNumber;

				faceMat = -1;
				
				// Generate a material
				if (polygonFaces[polygonFaceNumber].textureIndex>=0 && (polygonFaces[polygonFaceNumber].lightmapIndex>=0)) 
					if (materialRef.find(polygonFaces[polygonFaceNumber].textureIndex) != materialRef.end())
						if (materialRef[polygonFaces[polygonFaceNumber].textureIndex].find(polygonFaces[polygonFaceNumber].lightmapIndex) != materialRef[polygonFaces[polygonFaceNumber].textureIndex].end())
						{
							faceMat = materialRef[polygonFaces[polygonFaceNumber].textureIndex][polygonFaces[polygonFaceNumber].lightmapIndex];
						}
				
				if (faceMat == -1)
				{
					faceMat = Material::create();
					
					faceMat->setMaxSmooth(89);
					// Apply the textures as layer 0, 1
					faceMat->bindTexture(0,(polygonFaces[polygonFaceNumber].textureIndex>=0)?decalTextures[polygonFaces[polygonFaceNumber].textureIndex]:0,TEXTURE_DIFF);
					faceMat->bindTexture(1,(polygonFaces[polygonFaceNumber].lightmapIndex>=0)?lightmapTextures[polygonFaces[polygonFaceNumber].lightmapIndex]:0,TEXTURE_LIGHTMAP);
					// parallax test
//					faceMat->bindTexture(2,(polygonFaces[polygonFaceNumber].textureIndex>=0)?decalTextures[polygonFaces[polygonFaceNumber].textureIndex]:0,TEXTURE_BUMP);
				}
								
				materialRef[polygonFaces[polygonFaceNumber].textureIndex][polygonFaces[polygonFaceNumber].lightmapIndex] = faceMat;
				
				polygonFaces[polygonFaceNumber].materialIndex = faceMat;

				for (int k = polygonFaces[polygonFaceNumber].firstVertexIndex+2; k <  polygonFaces[polygonFaceNumber].numVertices+polygonFaces[polygonFaceNumber].firstVertexIndex; k++)
				{			
					cvrIndex newFace;
					
					newFace = clusterObjects[i]->addFace();
					
					clusterObjects[i]->bindFaceMaterial(faceMat);
					
					int v1, v2, v3;
					v1 = k;
					v2 = k-1;
					v3 = polygonFaces[polygonFaceNumber].firstVertexIndex;
					
					
					clusterObjects[i]->addFacePoint(clusterObjects[i]->addPoint(XYZ(vertices[v1].position.x,vertices[v1].position.y,vertices[v1].position.z)));
					clusterObjects[i]->addFacePoint(clusterObjects[i]->addPoint(XYZ(vertices[v2].position.x,vertices[v2].position.y,vertices[v2].position.z)));
					clusterObjects[i]->addFacePoint(clusterObjects[i]->addPoint(XYZ(vertices[v3].position.x,vertices[v3].position.y,vertices[v3].position.z)));

					clusterObjects[i]->faces[newFace]->setUV(0,UV(vertices[v1].decalS,vertices[v1].decalT));
					clusterObjects[i]->faces[newFace]->setUV(1,UV(vertices[v2].decalS,vertices[v2].decalT));
					clusterObjects[i]->faces[newFace]->setUV(2,UV(vertices[v3].decalS,vertices[v3].decalT));

					clusterObjects[i]->faces[newFace]->setUV(0,UV(vertices[v1].lightmapS,vertices[v1].lightmapT),1);
					clusterObjects[i]->faces[newFace]->setUV(1,UV(vertices[v2].lightmapS,vertices[v2].lightmapT),1);
					clusterObjects[i]->faces[newFace]->setUV(2,UV(vertices[v3].lightmapS,vertices[v3].lightmapT),1);
					
				}
			}

			clusterObjects[i]->calcNormals();
			clusterObjects[i]->cache(true);
	
		}
		else
		{
			clusterObjects[i] = NULL;
		}
	}
}
*/



void BSP::buildSingleCluster()
{
	Material *faceMat;
//	unsigned long newFace;
	std::map<long, bool, ltulong> face_visit;	
	std::map<long, bool, ltulong> curvesurf_visit;	
	std::map<long, bool, ltulong> meshface_visit;	
	std::map<long, long, ltulong> vertex_map;
	std::map<long, long, ltulong> patch_map;
	std::map<long, std::map<long, std::map<long, unsigned int, ltulong> , ltulong> , ltulong> segRef;
	std::map<long, std::map<cvrIndex, Material *, ltindex>, ltulong> matRef;
	std::map<long, std::map<cvrIndex, Material *, ltindex>, ltulong> meshRef;
	std::map<float, std::map<float, std::map<float, cvrIndex, ltufloat>, ltufloat>, ltufloat> pointRef;
	
	XYZ segPt;
	cvrIndex newFace, p1, p2, p3;
	XYZ xyz1, xyz2, xyz3;

	int segCount = 1;
	int segIndex = 1;
	
	nullMat = new Material();
	nullMat->color=RGB(0.5f,0.5f,0.5f);
	
//	SectorMap segTest(XYZ(-10000,-10000,-10000),XYZ(10000,10000,10000),25);
	
	//loop through the leaves
	for(int i=0; i<numLeaves; ++i)
	{				
		if(leaves[i].numFaces)
		{
			segIndex = segCount;
			segCount++;
			clusterObject.setSegment(segIndex);
			
			for(int j=0; j<leaves[i].numFaces; ++j)
			{
				int faceNumber = leafFaces[leaves[i].firstLeafFace+j];
				int polygonFaceNumber = faceDirectory[faceNumber].typeFaceNumber;

//				SectorLoc secRef;
				int patchNumber;

				// todo: implement other face types.
				switch (faceDirectory[faceNumber].faceType)
				{
				case bspPolygonFace:
					if (face_visit.find(polygonFaceNumber) != face_visit.end()) continue;
					face_visit[polygonFaceNumber] = true;
					
					faceMat = NULL;
//					segIndex = -1;
					
					segPt = XYZ(vertices[polygonFaces[polygonFaceNumber].firstVertexIndex+2].position.x,vertices[polygonFaces[polygonFaceNumber].firstVertexIndex+2].position.y,vertices[polygonFaces[polygonFaceNumber].firstVertexIndex+2].position.z);

					segPt *= 100.0f;
						
//					secRef = segTest.getSector(segPt);
					
	
					// Generate a material
					if (polygonFaces[polygonFaceNumber].textureIndex>=0 && (polygonFaces[polygonFaceNumber].lightmapIndex>=0)) 
					if (matRef.find(polygonFaces[polygonFaceNumber].textureIndex) != matRef.end())
					if (matRef[polygonFaces[polygonFaceNumber].textureIndex].find(polygonFaces[polygonFaceNumber].lightmapIndex) != matRef[polygonFaces[polygonFaceNumber].textureIndex].end())
					{
						faceMat = matRef[polygonFaces[polygonFaceNumber].textureIndex][polygonFaces[polygonFaceNumber].lightmapIndex];
					}
					
					if ((polygonFaces[polygonFaceNumber].lightmapIndex>=0) && faceMat == NULL) 
					{
						faceMat = new Material();
						faceMat->color=RGB(1,1,1);
						faceMat->setMaxSmooth(80);

						// Apply the textures as layer 0, 1
						faceMat->bindTexture(0,(polygonFaces[polygonFaceNumber].lightmapIndex>=0)?lightmapTextures[polygonFaces[polygonFaceNumber].lightmapIndex]:0,TEXTURE_LIGHTMAP);
						if (polygonFaces[polygonFaceNumber].textureIndex>=0) 
						{
							faceMat->bindTexture(1,(polygonFaces[polygonFaceNumber].textureIndex>=0)?decalTextures[polygonFaces[polygonFaceNumber].textureIndex]:0,TEXTURE_DIFF);
						}
						
						// parallax test
//						faceMat->bindTexture(2,(polygonFaces[polygonFaceNumber].textureIndex>=0)?decalTextures[polygonFaces[polygonFaceNumber].textureIndex]:0,TEXTURE_BUMP);

//						if ((polygonFaces[polygonFaceNumber].textureIndex>=0) && isTextureTGA[polygonFaces[polygonFaceNumber].textureIndex])
//						{
//							faceMat->bindTexture(2,(polygonFaces[polygonFaceNumber].textureIndex>=0)?decalTextures[polygonFaces[polygonFaceNumber].textureIndex]:0,TEXTURE_ALPHA);
//						}
						matRef[polygonFaces[polygonFaceNumber].textureIndex][polygonFaces[polygonFaceNumber].lightmapIndex] = faceMat;
					}
					else if (faceMat == NULL)
					{
						faceMat = nullMat;
					}
					
//					if (segRef.find(secRef.x) != segRef.end())
//					if (segRef[secRef.x].find(secRef.y) != segRef[secRef.x].end())
//					if (segRef[secRef.x][secRef.y].find(secRef.z) != segRef[secRef.x][secRef.y].end())
//					{
//						segIndex = segRef[secRef.x][secRef.y][secRef.z];
//					}
//						
//					if (segIndex == -1)
//					{
//						segRef[secRef.x][secRef.y][secRef.z] = segIndex = segCount;
//						segCount++;
//					}
						
//					clusterObject.setSegment(segIndex);					
					
					polygonFaces[polygonFaceNumber].segmentIndex = segIndex;
					
					//decode triangle fan
					for (int k = polygonFaces[polygonFaceNumber].firstVertexIndex+2; k <  polygonFaces[polygonFaceNumber].numVertices+polygonFaces[polygonFaceNumber].firstVertexIndex; k++)
					{			
						newFace = clusterObject.addFace();
						
						clusterObject.bindFaceMaterial(faceMat);
						
						int v1, v2, v3;
						v1 = k;
						v2 = k-1;
						v3 = polygonFaces[polygonFaceNumber].firstVertexIndex;
						
//						xyz2 = XYZ(vertices[v2].position.x,vertices[v2].position.y,vertices[v2].position.z);
//						xyz3 = XYZ(vertices[v3].position.x,vertices[v3].position.y,vertices[v3].position.z);
						
						if (vertex_map.find(v1)!=vertex_map.end())
						{
							p1 = vertex_map[v1];
						}
						else
						{
							xyz1 = XYZ(vertices[v1].position.x,vertices[v1].position.y,vertices[v1].position.z);
							p1 = vertex_map[v1] = clusterObject.addPoint(xyz1);
						}

						if (vertex_map.find(v2)!=vertex_map.end())
						{
							p2 = vertex_map[v2];
						}
						else
						{
							xyz2 = XYZ(vertices[v2].position.x,vertices[v2].position.y,vertices[v2].position.z);
							p2 = vertex_map[v2] = clusterObject.addPoint(xyz2);
						}
				
						if (vertex_map.find(v3)!=vertex_map.end())
						{
							p3 = vertex_map[v3];
						}
						else
						{
							xyz3 = XYZ(vertices[v3].position.x,vertices[v3].position.y,vertices[v3].position.z);
							p3 = vertex_map[v3] = clusterObject.addPoint(xyz3);
						}
						
						
//						p1 = (pointRef[xyz1.x][xyz1.y].find(xyz1.z) != pointRef[xyz1.x][xyz1.y].end())?pointRef[xyz1.x][xyz1.y][xyz1.z]:clusterObject.addPoint(xyz1);
//						p2 = (pointRef[xyz2.x][xyz2.y].find(xyz2.z) != pointRef[xyz2.x][xyz2.y].end())?pointRef[xyz2.x][xyz2.y][xyz2.z]:clusterObject.addPoint(xyz2);
//						p3 = (pointRef[xyz3.x][xyz3.y].find(xyz3.z) != pointRef[xyz3.x][xyz3.y].end())?pointRef[xyz3.x][xyz3.y][xyz3.z]:clusterObject.addPoint(xyz3);						
						
						clusterObject.addFacePoint(p1);
						clusterObject.addFacePoint(p2);
						clusterObject.addFacePoint(p3);
						
						clusterObject.faces[newFace]->setUV(0,UV(vertices[v1].decalS,vertices[v1].decalT),0);
						clusterObject.faces[newFace]->setUV(1,UV(vertices[v2].decalS,vertices[v2].decalT),0);
						clusterObject.faces[newFace]->setUV(2,UV(vertices[v3].decalS,vertices[v3].decalT),0);
						
						//if (polygonFaces[polygonFaceNumber].lightmapIndex>=0)
						{
							clusterObject.faces[newFace]->setUV(0,UV(vertices[v1].lightmapS,vertices[v1].lightmapT),1);
							clusterObject.faces[newFace]->setUV(1,UV(vertices[v2].lightmapS,vertices[v2].lightmapT),1);
							clusterObject.faces[newFace]->setUV(2,UV(vertices[v3].lightmapS,vertices[v3].lightmapT),1);
						}
					}
				break;
				case bspPatch:	
					patchNumber = polygonFaceNumber;

					if (curvesurf_visit.find(patchNumber) != curvesurf_visit.end()) continue;
					curvesurf_visit[patchNumber] = true;
	
					faceMat = new Material();
					
					// Apply the textures as layer 0, 1
					faceMat->bindTexture(0,(patches[patchNumber].textureIndex>=0)?decalTextures[patches[patchNumber].textureIndex]:0,TEXTURE_DIFF);
					faceMat->bindTexture(1,(patches[patchNumber].lightmapIndex>=0)?lightmapTextures[patches[patchNumber].lightmapIndex]:0,TEXTURE_LIGHTMAP);
					faceMat->setMaxSmooth(80);
					// parallax test
//					faceMat->bindTexture(2,(patches[patchNumber].textureIndex>=0)?decalTextures[patches[patchNumber].textureIndex]:0,TEXTURE_BUMP);
					
					patches[patchNumber].segmentIndex=segIndex;	
//					clusterObject.setSegment(segCount);					
//					segCount++;
												
					for(int j=0; j<patches[patchNumber].numQuadraticPatches; ++j)
					{
						BSP_BIQUADRATIC_PATCH *qp = &patches[patchNumber].quadraticPatches[j];

						int v1, v2, v3;
						float ofs = 0;
						
						patch_map.clear();
						
						for(int row=0; row < qp->tesselation; ++row)
						{
							bool flip = false;

							v2 = qp->indices[(row*2*(qp->tesselation+1))];
							v3 = qp->indices[(row*2*(qp->tesselation+1))+1];																				
							
							if (patch_map.find(v2)!=patch_map.end())
							{
								p2 = patch_map[v2];
							}
							else
							{
								xyz2 = XYZ(qp->vertices[v2].position.x,qp->vertices[v2].position.y,qp->vertices[v2].position.z);
								p2 = patch_map[v2] = clusterObject.addPoint(xyz2);
							}
							
							if (patch_map.find(v3)!=patch_map.end())
							{
								p3 = patch_map[v3];
							}
							else
							{
								xyz3 = XYZ(qp->vertices[v3].position.x,qp->vertices[v3].position.y,qp->vertices[v3].position.z);
								p3 = patch_map[v3] = clusterObject.addPoint(xyz3);
							}
							
							// decode triangle strip
							for (int k = (row*2*(qp->tesselation+1))+2; k < ((row+1)*2*(qp->tesselation+1)); k++)
							{
								newFace = clusterObject.addFace();
								
								clusterObject.bindFaceMaterial(faceMat);

								v1 = v2;
								v2 = v3;
								v3 = qp->indices[k];

//								printf("vert: %d, %d, %d\n",v1,v2,v3);
//								continue;

								p1 = p2;
								p2 = p3;
//								p3 = clusterObject.addPoint(xyz3);
												
								if (patch_map.find(v3)!=patch_map.end())
								{
									p3 = patch_map[v3];
								}
								else
								{
									xyz3 = XYZ(qp->vertices[v3].position.x,qp->vertices[v3].position.y,qp->vertices[v3].position.z);
									p3 = patch_map[v3] = clusterObject.addPoint(xyz3);
								}
								
								
								if (flip) 
								{									
									clusterObject.addFacePoint(p1);
									clusterObject.addFacePoint(p2);
									clusterObject.addFacePoint(p3);

									clusterObject.faces[newFace]->setUV(0,UV(qp->vertices[v1].decalS,qp->vertices[v1].decalT+ofs),0);
									clusterObject.faces[newFace]->setUV(1,UV(qp->vertices[v2].decalS,qp->vertices[v2].decalT+ofs),0);
									clusterObject.faces[newFace]->setUV(2,UV(qp->vertices[v3].decalS,qp->vertices[v3].decalT+ofs),0);
									
									//if (polygonFaces[polygonFaceNumber].lightmapIndex>=0)
									{
										clusterObject.faces[newFace]->setUV(0,UV(qp->vertices[v1].lightmapS,qp->vertices[v1].lightmapT+ofs),1);
										clusterObject.faces[newFace]->setUV(1,UV(qp->vertices[v2].lightmapS,qp->vertices[v2].lightmapT+ofs),1);
										clusterObject.faces[newFace]->setUV(2,UV(qp->vertices[v3].lightmapS,qp->vertices[v3].lightmapT+ofs),1);
									}
								}
								else
								{
									clusterObject.addFacePoint(p3);
									clusterObject.addFacePoint(p2);
									clusterObject.addFacePoint(p1);									

									clusterObject.faces[newFace]->setUV(2,UV(qp->vertices[v1].decalS,qp->vertices[v1].decalT+ofs),0);
									clusterObject.faces[newFace]->setUV(1,UV(qp->vertices[v2].decalS,qp->vertices[v2].decalT+ofs),0);
									clusterObject.faces[newFace]->setUV(0,UV(qp->vertices[v3].decalS,qp->vertices[v3].decalT+ofs),0);
									
									//if (polygonFaces[polygonFaceNumber].lightmapIndex>=0)
									{
										clusterObject.faces[newFace]->setUV(2,UV(qp->vertices[v1].lightmapS,qp->vertices[v1].lightmapT+ofs),1);
										clusterObject.faces[newFace]->setUV(1,UV(qp->vertices[v2].lightmapS,qp->vertices[v2].lightmapT+ofs),1);
										clusterObject.faces[newFace]->setUV(0,UV(qp->vertices[v3].lightmapS,qp->vertices[v3].lightmapT+ofs),1);
									}
								}
								
								flip = !flip;
							}
						}
					}
						break;
					case bspMeshFace:
						
						if (meshface_visit.find(polygonFaceNumber) != meshface_visit.end()) continue;
						meshface_visit[polygonFaceNumber] = true;
						
						faceMat = NULL;
						
						// Generate a material
						if (meshFaces[polygonFaceNumber].textureIndex>=0 && (meshFaces[polygonFaceNumber].lightmapIndex>=0)) 
							if (meshRef[meshFaces[polygonFaceNumber].textureIndex].find(meshFaces[polygonFaceNumber].lightmapIndex) != meshRef[meshFaces[polygonFaceNumber].textureIndex].end())
							{
								faceMat = meshRef[meshFaces[polygonFaceNumber].textureIndex][meshFaces[polygonFaceNumber].lightmapIndex];
							}
						
						if (faceMat == NULL)
						{
							faceMat = new Material();
							
							// Apply the textures as layer 0, 1
							
							faceMat->bindTexture(0,(meshFaces[polygonFaceNumber].textureIndex>=0)?decalTextures[meshFaces[polygonFaceNumber].textureIndex]:0,TEXTURE_DIFF);
							faceMat->bindTexture(1,(meshFaces[polygonFaceNumber].lightmapIndex>=0)?lightmapTextures[meshFaces[polygonFaceNumber].lightmapIndex]:0,TEXTURE_LIGHTMAP);
							faceMat->setMaxSmooth(80);
//							printf("contents: %d, flags: %d\n",loadTextures[meshFaces[polygonFaceNumber].textureIndex].contents,loadTextures[meshFaces[polygonFaceNumber].textureIndex].flags);

								
							int flags = loadTextures[meshFaces[polygonFaceNumber].textureIndex].flags;
//							int contents = loadTextures[meshFaces[polygonFaceNumber].textureIndex].contents;

							
//							printf("Material %d\n",faceMat);
//
//							if (contents & CONTENTS_TRANSLUCENT) printf("flag: CONTENTS_TRANSLUCENT\n");							
//							if (flags & SURF_TRANS33) printf("flag: SURF_TRANS33\n");							
//							if (flags & SURF_TRANS66) printf("flag: SURF_TRANS66\n");							
							
							
							
							if (meshFaces[polygonFaceNumber].textureIndex>=0)
							{
								if (Texture::textures[decalTextures[meshFaces[polygonFaceNumber].textureIndex]]->img.bpp>24)
								{
									faceMat->bindTexture(2,decalTextures[meshFaces[polygonFaceNumber].textureIndex],TEXTURE_ALPHA);
								}
								else 
								if ((flags & SURF_TRANS66)) // (contents & CONTENTS_TRANSLUCENT) && 
								{
									faceMat->colorMask(true);
								}
							}
						}
						
						
						meshRef[meshFaces[polygonFaceNumber].textureIndex][meshFaces[polygonFaceNumber].lightmapIndex] = faceMat;
						meshFaces[polygonFaceNumber].segmentIndex = segIndex;
//						clusterObject.setSegment(segCount);
//						segCount++;
						
						
						
						//decode triangles
						for (int k = 0; k < meshFaces[polygonFaceNumber].numMeshIndices; k+=3)
						{			
							
							newFace = clusterObject.addFace();
							
							clusterObject.bindFaceMaterial(faceMat);
							
							int v1, v2, v3;
							v1 = meshFaces[polygonFaceNumber].firstVertexIndex+meshIndices[meshFaces[polygonFaceNumber].firstMeshIndex+k+2];
							v2 = meshFaces[polygonFaceNumber].firstVertexIndex+meshIndices[meshFaces[polygonFaceNumber].firstMeshIndex+k+1];
							v3 = meshFaces[polygonFaceNumber].firstVertexIndex+meshIndices[meshFaces[polygonFaceNumber].firstMeshIndex+k];
							
//							xyz1 = XYZ(vertices[v1].position.x,vertices[v1].position.y,vertices[v1].position.z);
//							xyz2 = XYZ(vertices[v2].position.x,vertices[v2].position.y,vertices[v2].position.z);
//							xyz3 = XYZ(vertices[v3].position.x,vertices[v3].position.y,vertices[v3].position.z);
//							
//							
//							p1 = (pointRef[xyz1.x][xyz1.y].find(xyz1.z) != pointRef[xyz1.x][xyz1.y].end())?pointRef[xyz1.x][xyz1.y][xyz1.z]:clusterObject.addPoint(xyz1);
//							p2 = (pointRef[xyz2.x][xyz2.y].find(xyz2.z) != pointRef[xyz2.x][xyz2.y].end())?pointRef[xyz2.x][xyz2.y][xyz2.z]:clusterObject.addPoint(xyz2);
//							p3 = (pointRef[xyz3.x][xyz3.y].find(xyz3.z) != pointRef[xyz3.x][xyz3.y].end())?pointRef[xyz3.x][xyz3.y][xyz3.z]:clusterObject.addPoint(xyz3);						

							
							
							if (vertex_map.find(v1)!=vertex_map.end())
							{
								p1 = vertex_map[v1];
							}
							else
							{
								xyz1 = XYZ(vertices[v1].position.x,vertices[v1].position.y,vertices[v1].position.z);
								p1 = vertex_map[v1] = clusterObject.addPoint(xyz1);
							}
							
							if (vertex_map.find(v2)!=vertex_map.end())
							{
								p2 = vertex_map[v2];
							}
							else
							{
								xyz2 = XYZ(vertices[v2].position.x,vertices[v2].position.y,vertices[v2].position.z);
								p2 = vertex_map[v2] = clusterObject.addPoint(xyz2);
							}
							
							if (vertex_map.find(v3)!=vertex_map.end())
							{
								p3 = vertex_map[v3];
							}
							else
							{
								xyz3 = XYZ(vertices[v3].position.x,vertices[v3].position.y,vertices[v3].position.z);
								p3 = vertex_map[v3] = clusterObject.addPoint(xyz3);
							}
							
							
							
							clusterObject.addFacePoint(p1);
							clusterObject.addFacePoint(p2);
							clusterObject.addFacePoint(p3);
							
							clusterObject.faces[newFace]->setUV(0,UV(vertices[v1].decalS,vertices[v1].decalT),0);
							clusterObject.faces[newFace]->setUV(1,UV(vertices[v2].decalS,vertices[v2].decalT),0);
							clusterObject.faces[newFace]->setUV(2,UV(vertices[v3].decalS,vertices[v3].decalT),0);
							
							//if (meshFaces[polygonFaceNumber].lightmapIndex>=0)
							{
								clusterObject.faces[newFace]->setUV(0,UV(vertices[v1].lightmapS,vertices[v1].lightmapT),1);
								clusterObject.faces[newFace]->setUV(1,UV(vertices[v2].lightmapS,vertices[v2].lightmapT),1);
								clusterObject.faces[newFace]->setUV(2,UV(vertices[v3].lightmapS,vertices[v3].lightmapT),1);
							}
						}

						break;
				}
			}
		}
		else
		{
			//clusterObjects[i] = NULL;
		}
		
	}
	
	
//	clusterObject.setMaterialMask(true);
	Logger::log(LOG_NOTICE,"\n\nNum Segments: %d\n",clusterObject.numSegments);
	Logger::log(LOG_NOTICE,"\n\nNum Faces: %d\n",clusterObject.faces.size());
//	Logger::log(LOG_NOTICE,"\n\nNum Materials: %d\n",Material::materials.size());
	clusterObject.calcNormals();
	clusterObject.cache(true);
	
	single_cluster = true;
}




//DRAWING FUNCTIONS
//Draw all faces marked as visible
void BSP::DrawGLSL(ObjectShader &shader)
{
	if (single_cluster)
	{
		shader.render(clusterObject,SHADER_STAGE_OPAQUE);
	}
	else
	{
//		for (int i = 0; i < numLeaves; i++)
//		{
//			if (leafActive.IsSet(i) && clusterObjects[i] != NULL) shader.render(*clusterObjects[i],SHADER_STAGE_OPAQUE);
//		}
	}
}


//Tesselate a biquadratic patch
bool BSP_BIQUADRATIC_PATCH::Tesselate(int newTesselation)
{
	tesselation=newTesselation;

	float px, py;
	BSP_VERTEX temp[3];
	vertices=new BSP_VERTEX[(tesselation+1)*(tesselation+1)];

	for(int v=0; v<=tesselation; ++v)
	{
		px=(float)v/tesselation;

		vertices[v]=controlPoints[0]*((1.0f-px)*(1.0f-px))+
					controlPoints[3]*((1.0f-px)*px*2)+
					controlPoints[6]*(px*px);
	}

	for(int u=1; u<=tesselation; ++u)
	{
		py=(float)u/tesselation;

		temp[0]=controlPoints[0]*((1.0f-py)*(1.0f-py))+
				controlPoints[1]*((1.0f-py)*py*2)+
				controlPoints[2]*(py*py);

		temp[1]=controlPoints[3]*((1.0f-py)*(1.0f-py))+
				controlPoints[4]*((1.0f-py)*py*2)+
				controlPoints[5]*(py*py);

		temp[2]=controlPoints[6]*((1.0f-py)*(1.0f-py))+
				controlPoints[7]*((1.0f-py)*py*2)+
				controlPoints[8]*(py*py);

		for(int v=0; v<=tesselation; ++v)
		{
			px=(float)v/tesselation;

			vertices[u*(tesselation+1)+v]=	temp[0]*((1.0f-px)*(1.0f-px))+
											temp[1]*((1.0f-px)*px*2)+
											temp[2]*(px*px);
		}
	}

	//Create indices
	indices=new unsigned int[tesselation*(tesselation+1)*2];
	if(!indices)
	{
		Logger::log(LOG_ERROR,"Unable to allocate memory for surface indices");
		return false;
	}

	for(int row=0; row<tesselation; ++row)
	{
		for(int point=0; point<=tesselation; ++point)
		{
			//calculate indices
			//reverse them to reverse winding
			indices[(row*(tesselation+1)+point)*2+1]=row*(tesselation+1)+point;
			indices[(row*(tesselation+1)+point)*2]=(row+1)*(tesselation+1)+point;
		}
	}


	//Fill in the arrays for multi_draw_arrays
	trianglesPerRow=new int[tesselation];
	rowIndexPointers=new unsigned int *[tesselation];
	if(!trianglesPerRow || !rowIndexPointers)
	{
		Logger::log(LOG_ERROR,"Unable to allocate memory for indices for multi_draw_arrays");
		return false;
	}

	for(int row=0; row<tesselation; ++row)
	{
		trianglesPerRow[row]=2*(tesselation+1);
		rowIndexPointers[row]=&indices[row*2*(tesselation+1)];
	}

	return true;
}


//#if !defined(OPENGL_ES) && !defined(ARCH_DC) && !defined(ARCH_PSP)
////Draw a biquadratic patch
//void BSP_BIQUADRATIC_PATCH::Draw()
//{
//	//set array pointers
//	glVertexPointer(3, GL_FLOAT, sizeof(BSP_VERTEX), &vertices[0].position);
//		
//	glTexCoordPointer(2, GL_FLOAT, sizeof(BSP_VERTEX), &vertices[0].decalS);
//	
//	Texture::setTexture(1);
//	glTexCoordPointer(2, GL_FLOAT, sizeof(BSP_VERTEX), &vertices[0].lightmapS);
//	Texture::setTexture(0);
//	
//	//Draw a triangle strip for each row
//	if(0) // !EXT_multi_draw_arrays_supported
//	{
//		for(int row=0; row<tesselation; ++row)
//		{
//			glDrawElements(	GL_TRIANGLE_STRIP, 2*(tesselation+1), GL_UNSIGNED_INT,
//							&indices[row*2*(tesselation+1)]);
//		}
//	}
//	else
//	{
//		glMultiDrawElementsEXT(	GL_TRIANGLE_STRIP, trianglesPerRow,
//								GL_UNSIGNED_INT, (const void **)rowIndexPointers,
//								tesselation);
//	}							
//}
//#endif
#endif
