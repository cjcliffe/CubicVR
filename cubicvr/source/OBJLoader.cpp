#include <CubicVR/OBJLoader.h>

OBJLoader::OBJLoader() : objMesh(NULL) {

}
		
OBJLoader::OBJLoader(const string fileName) : objMesh(NULL) {
	load(fileName);
}

Mesh *OBJLoader::getMesh() {
	return objMesh;
}

string OBJLoader::readStr(istringstream &input) {
	return input.str().substr(1+input.tellg());
}

RGBA OBJLoader::readRGB(istringstream &input) {
	float r,g,b;
	input >> r; input >> g; input >> b;				
	return RGBA(r,g,b,1.0);
}

Material *OBJLoader::initMaterial(string matName) {
	if (materials.find(matName)!=materials.end()) {
		return materials[matName];
	} else {
		return (materials[matName] = new Material());
	}
}

cvrIndex OBJLoader::initTexture(string fileName) {
	if (textures.find(fileName)!=textures.end()) {
		return textures[fileName];
	} else {
		cvrIndex prevTexture = Texture::getTextureId(fileName);
		if (prevTexture != 0) {
			return (textures[fileName] = prevTexture);
		}
		return (textures[fileName] = Texture::create(fileName,fileName));
	}
}

Mesh *OBJLoader::load(const string fileName) {
	objMesh = new Mesh();

	float x, y, z;
	string strline, op;
	string mtllib;

	vector<UV> uvs;
	vector<XYZ> normals;
	vector<string> ptData;

	Material *currentMaterial = NULL;

	string path_str,file_str,file_base,file_ext;
	str_file_extract(fileName, path_str, file_str, file_base, file_ext);

	Logger::log("Loading .OBJ Model [%s]\n",fileName.c_str());

	ifstream File(fileName.c_str());

	while (!File.eof())
	{
		getline(File,strline,'\n');
		istringstream input(strline);
		
		op.clear();
		input >> op;
		if (!op.length()) continue;

		if (op.length() && op[0] == '#') {
			Logger::log("%s\n",input.str().c_str());
			continue;			
		}

		if (op.compare("o")==0) { // Object Name
			string objName = readStr(input);
			Logger::log(" - Object Name [%s]\n",objName.c_str());
			continue;
		}

		if (op.compare("mtllib")==0) { // Material Lib
			mtllib = readStr(input);
			Logger::log(" - Material Library [%s]\n",mtllib.c_str());
			continue;
		}

		if (op.compare("v")==0) { // Vertex: X,Y,Z,[w]
			input >> x; input >> y; input >> z;

			objMesh->addPoint(XYZ(x,y,z));
			continue;
		}

		if (op.compare("vt")==0) {	// TexCoord: U,V,[w]
			input >> x; input >> y; 

			uvs.push_back(UV(x,y));
			continue;
		}

		if (op.compare("n")==0) { // Normal: X,Y,Z
			input >> x; input >> y; input >> z; 

			normals.push_back(XYZ(x,y,z));
			continue;
		}

		if (op.compare("usemtl")==0) { // Material reference
			string matName;
			matName = readStr(input);
			currentMaterial = initMaterial(matName);
			continue;
		}

		if (op.compare("s")==0) { // Smoothing?
			string smoothMode;
			input >> smoothMode;
			if (smoothMode.compare("1")==0 && currentMaterial != NULL) {
				currentMaterial->setMaxSmooth(89);
			} else if (currentMaterial!=NULL) {
				currentMaterial->setMaxSmooth(0);
			}
		}

		if (op.compare("f") == 0) { // Face
			unsigned int faceNum = objMesh->addFace();
			string faceData;

			if (currentMaterial != NULL) {
				objMesh->bindFaceMaterial(currentMaterial);				
			}

			int ptIdx=0;

			while (!input.eof()) {	// Face-Point List
				input >> faceData;

				ptData.clear();
				str_explode(ptData,"/",faceData);

				int ptsz = ptData.size();

				if (ptsz) {	// point index, required
					objMesh->addFacePoint(atoi(ptData[0].c_str())-1);
				}
				if (ptsz > 1) {
					if (ptData[1][0] != 0) { // uv, optional -- empty string?						
						objMesh->faces[faceNum]->setUV(ptIdx,uvs[atoi(ptData[1].c_str())-1]);
					}
				}
				if (ptsz > 2) {	// normal
					objMesh->faces[faceNum]->point_normals.push_back(normals[atoi(ptData[2].c_str())-1]);
				}

				ptIdx++;
			}
			continue;
		}
	}		

	if (mtllib.length()) {
		Logger::log(" - Loading material lib [%s]\n", mtllib.c_str());
		ifstream mFile((path_str+PATH_SEP+mtllib).c_str());

		while (!mFile.eof()) {
			getline(mFile,strline,'\n');
			istringstream input(strline);

			op.clear();
			input >> op;
			if (!op.length()) continue;


			if (op.length() && op[0] == '#') {
				Logger::log("%s\n",input.str().c_str());
				continue;			
			}

			if (op.compare("newmtl")==0) { // Material Name
				string matName = readStr(input);
				Logger::log(" - Material Name [%s]\n",matName.c_str());
				currentMaterial = initMaterial(matName);				
				continue;
			}

			if (currentMaterial != NULL) {
				if (op.compare("Ka")==0) {
					currentMaterial->setAmbient(readRGB(input));
					continue;
				}
				if (op.compare("Kd")==0) {
					currentMaterial->setDiffuse(readRGB(input));
					continue;
				}
				if (op.compare("Ks")==0) {
					currentMaterial->setSpecular(readRGB(input));
					continue;
				}
				if (op.compare("Ns")==0) {
					float s;
					input >> s;
					currentMaterial->setShininess(128.0*(s/1000.0));
					continue;
				}

				if ((op.compare("Tr") == 0) || (op.compare("d") == 0)) {
					float s;
					input >> s;
					currentMaterial->color.a = s;
					continue;
				}

				if (op.compare("map_Ka")==0) {
					currentMaterial->bindTexture(initTexture(path_str+PATH_SEP+readStr(input)),TEXTURE_LUMI);
//							currentMaterial->bindTexture(initTexture(readStr(input)),TEXTURE_DIFF);
					continue;
				}
				if (op.compare("map_Kd")==0) {
					currentMaterial->bindTexture(initTexture(path_str+PATH_SEP+readStr(input)),TEXTURE_DIFF);
					continue;
				}
				if (op.compare("map_Ks")==0) {
					currentMaterial->bindTexture(initTexture(path_str+PATH_SEP+readStr(input)),TEXTURE_SPEC);
					continue;
				}
				if (op.compare("map_d")==0) {
					currentMaterial->bindTexture(initTexture(path_str+PATH_SEP+readStr(input)),TEXTURE_ALPHA);
					continue;
				}
				if ((op.compare("map_bump")==0) || (op.compare("bump")==0)) {
					currentMaterial->bindTexture(initTexture(path_str+PATH_SEP+readStr(input)),TEXTURE_BUMP);
					continue;
				}
				/*
				if (op.compare("illum")==0) {
						0. Color on and Ambient off
						1. Color on and Ambient on
						2. Highlight on
						3. Reflection on and Ray trace on
						4. Transparency: Glass on, Reflection: Ray trace on
						5. Reflection: Fresnel on and Ray trace on
						6. Transparency: Refraction on, Reflection: Fresnel off and Ray trace on
						7. Transparency: Refraction on, Reflection: Fresnel on and Ray trace on
						8. Reflection on and Ray trace off
						9. Transparency: Glass on, Reflection: Ray trace off
						10. Casts shadows onto invisible surfaces
				}
				*/
			}
		}
	}

	Logger::log(" - # Points [%u]\n",objMesh->points.size());

	objMesh->triangulate();

	Logger::log(" - # Faces [%u]\n",objMesh->faces.size());

	if (normals.size() == 0) {
		objMesh->calcNormals();
	}

	return objMesh;
}

