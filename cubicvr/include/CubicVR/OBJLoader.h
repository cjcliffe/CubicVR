#pragma once

#include <CubicVR/Mesh.h>
#include <CubicVR/StringUtil.h>



class OBJLoader {
	private:
		Mesh *objMesh;
		string sourceFile;
		map<string,Material *> materials;
		map<string,cvrIndex> textures;

		string readStr(istringstream &input);
		RGBA readRGB(istringstream &input);

		Material *initMaterial(string matName);
		cvrIndex initTexture(string fileName);

	public:
		OBJLoader();
		OBJLoader(const string fileName);

		Mesh *getMesh();
		Mesh *load(const string fileName);
};

