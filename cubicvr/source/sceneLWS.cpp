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

#include "CubicVR/sceneLWS.h"
#ifndef ARCH_PSP
#include "CubicVR/Bone.h"
#endif
#include "CubicVR/StringUtil.h"

using namespace std;


IMPEXP void loadLWS(Scene &scene, const std::string fn, MapString *mapStr)
{
	int motionType = NO_MOTION;
	//int newObj = -1, newCam = -1, newLight = -1, newBone = -1;
	Object *newObj;
	Camera *newCam;
	Light *newLight;
	

	int newBone;
	
//	int newSceneObj = -1;
	SceneObject *newSceneObj = NULL;
	
	int i;
	
	string lastStill;
	
	map<string, SceneObject *, string_less> sceneObj_ref;	// scene target # => instance target # maps
	map<string, Light *, string_less> light_ref;	
	map<string, unsigned int, string_less> bone_ref;	
	map<string, Object *, string_less> obj_ref;
	
	vector<string> parentRef;
	vector<SceneObject *> childRef;

	vector<string> light_parentRef;
	vector<Light *> light_childRef;

	vector<string> bone_parentRef;
	vector<unsigned int> bone_childRef;

	Motion *sceneMotion = new Motion();
	sceneMotion->setId(fn);
	scene.bind(*sceneMotion);
	
////	cvrIndex sceneMotion;
////	sceneMotion = scene.newMotion(fn);
#ifndef ARCH_PSP	
	BoneSet *bones = NULL;
#endif
	string boneId;
	
//	map<unsigned int, unsigned long, ltulong> bone_parent_map;
//	unsigned long boneId=0;
	

	int slashPos = fn.find_last_of('/');
	
	string baseDir = fn.substr(0,slashPos+1);
//	string lwsFn = fn.substr(slashPos+1,fn.length()-(slashPos+1));
	
	
	ifstream File(fn.c_str());

	string input;
	
	File >> input;
	
	if (input.compare("LWSC"))
	{
		Logger::log("%s is not a valid lightwave scene file.\n",fn.c_str());
	}
	
	File >> input;
	
	Logger::log("Loading version %s lightwave scene file '%s'\n",input.c_str(),fn.c_str());
	
	char nlchar = File.peek();
	
	while (!File.eof())
	{
		File >> input;
				
		if (input.compare("ObjectMotion") == 0)	motionType = OBJECT_MOTION;		
		if (input.compare("CameraMotion") == 0)	motionType = CAMERA_MOTION;		
		if (input.compare("LightMotion") == 0) motionType = LIGHT_MOTION;	
		if (input.compare("BoneMotion") == 0) motionType = BONE_MOTION;	
		
		if (input.compare("Still") == 0)
		{
			//File >> lastStill;
			getline(File,lastStill,nlchar);
			getline(File,lastStill,nlchar);

			size_t filenameOfs = lastStill.find_last_of('/');
			
			if (filenameOfs == string::npos) filenameOfs = lastStill.find_first_of("\"");
			
			string stillFnTrimmed = lastStill.substr(filenameOfs+1,lastStill.length()-(filenameOfs+2));
			
			if (mapStr)
			{
				lastStill.clear();
				lastStill.append(mapStr->getString(stillFnTrimmed.c_str()));
			}
			else
			{
				lastStill = baseDir;
				lastStill.append(stillFnTrimmed);
			}
			
			Logger::log(" found still %s",lastStill.c_str());
		}

		if (input.compare("ShadowMapSize") == 0)
		{
			int shadowMapSize;
			
			File >> shadowMapSize;
			
			Logger::log(" light projector %s",lastStill.c_str());
						
//			if (lastStill.length())
//			{
//				*newLight.setShadow(shadowMapSize,lastStill);
//			}
			
			newLight->setShadow(shadowMapSize,"");
		}		

		if (input.compare("LightName") == 0)
		{
			string lightName;
			
			File >> lightName;
			
			Logger::log("Adding light: [%s]\n",lightName.c_str());
			
			newLight = new Light();
			newLight->setId(lightName);
			
			newObj = NULL;
			newCam = NULL;
			newBone = -1;
			
			scene.bind(*newLight);
		}

#ifndef ARCH_PSP		
		if (input.compare("AddBone") == 0)
		{
			File >> boneId;
			
			Logger::log("Adding bone id: [%s]\n",boneId.c_str());
			
			newLight = NULL;
			newObj = NULL;
			newCam = NULL;
			if (!bones) bones = new BoneSet();
			newBone = bones->bind(new Bone());
			
			if (!bones) bones = new BoneSet();
		}
		
		if (input.compare("BoneName") == 0)
		{
			string boneName;
			
			File >> boneName;
			
			Logger::log("Adding bone: [%s]\n",boneName.c_str());

//			if (!bones) bones = new BoneSet();

			newLight = NULL;
			newObj = NULL;
			newCam = NULL;
//			newBone = -1;
			
			bone_ref[boneId] = newBone;
		}

		if (input.compare("BoneRestPosition") == 0 && newBone != -1)
		{
			XYZ restPos;
			
			File >> restPos.x;
			File >> restPos.y;
			File >> restPos.z;
			
			Logger::log("Bone rest position: [%f,%f,%f]\n",restPos.x,restPos.y,restPos.z);
			
			bones->bone(newBone)->setRestPosition(restPos);
		}

		if (input.compare("BoneRestDirection") == 0 && newBone != -1)
		{
			XYZ restDir;
			
			File >> restDir.y;
			File >> restDir.x;
			File >> restDir.z;
			
//		case 3: tKey = sceneMotion->setKey(*bones->bone(newBone),CONTROLLER_ROTATION,MOTION_Y,time,(-value+M_PI)*(180/M_PI)); break;
//		case 4: tKey = sceneMotion->setKey(*bones->bone(newBone),CONTROLLER_ROTATION,MOTION_X,time,-value*(180/M_PI)); break;
//		case 5: tKey = sceneMotion->setKey(*bones->bone(newBone),CONTROLLER_ROTATION,MOTION_Z,time,value*(180/M_PI)); break;
			restDir.y = -restDir.y;
			restDir.x = restDir.x;
			restDir.z = -restDir.z;
			
			Logger::log("Bone rest direction: [%f,%f,%f]\n",restDir.x,restDir.y,restDir.z);
			
			bones->bone(newBone)->setRestDirection(restDir);
		}
		

		if (input.compare("BoneRestLength") == 0 && newBone != -1)
		{
			float restLen;
			
			File >> restLen;
			
			Logger::log("Bone rest length: [%f]\n",restLen);
			
			bones->bone(newBone)->setRestLength(restLen);
		}
		
		
		if (input.compare("BoneStrength") == 0 && newBone != -1)
		{
			float boneStrength;
			
			File >> boneStrength;
			
			Logger::log("Bone strength length: [%f]\n",boneStrength);
			
			bones->bone(newBone)->setStrength(boneStrength);
		}
		
//		BoneRestPosition 0 -1.31392 1.347624
//		BoneRestDirection 0 -89.99998 0
//		BoneRestLength 1
//		BoneStrength 1
//		ScaleBoneStrength 1
		
#endif
		
		if (input.compare("LightType") == 0)
		{
			int lightType;
			
			File >> lightType;

			Logger::log("set light type %i\n",lightType);

			switch (lightType)
			{
				case 0:	// Distant 
					newLight->setType(LIGHT_DIRECTIONAL);
				break;
				case 1:	// Point
					newLight->setType(LIGHT_POINT);
				break;
				case 2:	// Spot 
					newLight->setType(LIGHT_SPOT);
				break;
			}
		}

		if (input.compare("LightConeAngle") == 0)
		{
			float coneAngle;
			
			File >> coneAngle;
			
			Logger::log("cone angle %f\n",coneAngle);

			newLight->setCutoff(coneAngle*2.0f);
			newLight->setExponent(3.0f);
		}
				
		if (input.compare("LightColor") == 0)
		{
			float r,g,b;
			
			File >> r;
			File >> g;
			File >> b;

			Logger::log("light color %f, %f, %f\n",r,g,b);
			
			newLight->setDiffuse(RGB(r,g,b));
//			newLight->setSpecular(RGB(0.1*r,0.1*g,0.1*b));
//			newLight->setAmbient(RGB(0.1*r,0.1*g,0.1*b));
			newLight->setSpecular(RGB(r,g,b));
			newLight->setAmbient(RGB(0,0,0));
		}
			
		if (input.compare("LightIntensity") == 0)
		{
			float lightIntensity;
			
			File >> lightIntensity;
			
			Logger::log("light intensity %f\n",lightIntensity);
			
			newLight->setConstantAttenuation(0.015f/lightIntensity);
 			newLight->setLinearAttenuation(0.03f*lightIntensity);
			newLight->setQuadraticAttenuation(0.0f);
			
			
//			*newLight.setLinearAttenuation(lightIntensity/10000.0);
//			*newLight.setQuadraticAttenuation(0);
		}
		
		
		if (input.compare("LightRange") == 0)
		{
			float lightRange;
			
			File >> lightRange;
			
			Logger::log("light range %f\n",lightRange);
			
			newLight->setCutoff(lightRange);
			
			//			*newLight.setLinearAttenuation(lightIntensity/10000.0);
			//			*newLight.setQuadraticAttenuation(0);
		}
		
		
		if (input.compare("CameraName") == 0)
		{
			string camName;
			
			File >> camName;

			Logger::log("Adding camera %s\n",camName.c_str());
			
			newCam = new Camera();
			newCam->setSize(scene.getWidth(), scene.getHeight());
			newCam->setAspect(scene.getWidth(), scene.getHeight());
//			*newCam.setClip(1.0, 2000.0);
			scene.bind(*newCam);

			newLight = NULL;
			newObj = NULL;
			newBone = -1;
		}

		if (input.compare("TargetItem") == 0)
		{
			if (newCam != NULL)
			{
				string targetId;
				
				File >> targetId;
				
				if (targetId[0] == '1')	// object
				{
					Logger::log("Set camera target object: %s\n",targetId.c_str());
					newCam->bindTarget(sceneObj_ref[targetId]);
				}
			}
			if (newLight != NULL)
			{
				string targetId;
				
				File >> targetId;
				
				if (targetId[0]=='1') // object
				{
					Logger::log("Set light target object: %s\n",targetId.c_str());
					newLight->bindTarget(sceneObj_ref[targetId]);
				}
			}
		}
		
		if (input.compare("ZoomFactor") == 0)
		{
			string zoomFactor_str;
		
			File >> zoomFactor_str;
			
			if (zoomFactor_str.compare("(envelope)") == 0)
			{
				Logger::log("cam fov env\n");
				
				File >> input;
				if (input.compare("{") != 0) break;
				File >> input;
				if (input.compare("Envelope") != 0) break;
				
				Logger::log("end\n");
				
				unsigned int nKeys;
				File >> nKeys;				
				
				Logger::log("numKeys %d\n",nKeys);
				
				while (!File.eof())
				{
					File >> input;
						
					if (input.compare("}") == 0) break;
				
					if (input.compare("Key") == 0)
					{
						float value, time, p1, p2, p3, p4, p5, p6;
						int spantype;
						
						File >> value >> time >> spantype >> p1 >> p2 >> p3 >> p4 >> p5 >> p6;
						
						EnvelopeKey *tKey = NULL;

						if (motionType == CAMERA_MOTION)	
						{
							cvrFloat zoomFactor;
							
							zoomFactor = value;
							
						  //double vfov = ((double)scene.getWidth()/(double)scene.getHeight()) * atan( 1.0 / zoomFactor ) * (180.0/M_PI);
						
							cvrFloat hfov =  (2.0f * atan( ((cvrFloat)scene.getWidth()/(cvrFloat)scene.getHeight()) / zoomFactor ) * (180.0f/M_PI));
								
							tKey = sceneMotion->setKey(*newCam,CAMERA_CONTROLLER_FOV,CAMERA_MOTION_FOV,time,hfov);
						}

						if (tKey)
						{
							tKey->shape = spantype;
							tKey->tension = p1;
							tKey->continuity = p2;
							tKey->bias = p3;
							tKey->param[0] = p4;
							tKey->param[1] = p5;
						};
					}
					
					if (input.compare("Behaviors") == 0)
					{
						int in_behavior, out_behavior;
						
						File >> in_behavior;
						File >> out_behavior;
						
						if (motionType == CAMERA_MOTION)
						{
							sceneMotion->setBehavior(*newCam,CAMERA_CONTROLLER_FOV, CAMERA_MOTION_FOV, in_behavior, out_behavior);
						};
					}
				}
			}
			else
			{
				cvrFloat zoomFactor;
				
				zoomFactor = (cvrFloat)atof(zoomFactor_str.c_str());
				
			  //double vfov = 2.0 * atan( 1.0 / zoomFactor ) * (180.0/M_PI);
				cvrFloat hfov = (2.0f * atan( ((cvrFloat)scene.getWidth()/(cvrFloat)scene.getHeight()) / zoomFactor ) * (180.0f/M_PI));
				
			  newCam->setFOV(hfov);
			}
		}
		

		if (input.compare("PivotPosition") == 0)
		{
			float x, y, z;
			
			File >> x >> y >> z;
		
			if (motionType == OBJECT_MOTION)
			{
				newSceneObj->setPivot(XYZ(-x,y,z));
			}
		}

		if (input.compare("LoadObjectLayer") == 0)
		{
			int layerNum;
			string objFn;
			string objId;
			
			File >> layerNum;
			File >> objId;
			
			getline(File,objFn,nlchar);
			
			size_t filenameOfs = objFn.find_last_of('/')+1;
			
			if (filenameOfs == string::npos) filenameOfs = objFn.find_first_not_of(' ');
			
			string objFnTrimmed;
								 
			if (mapStr)
			{
				string tmpFile = objFn.substr(filenameOfs,objFn.length()-filenameOfs);
				
				objFnTrimmed.clear();
				objFnTrimmed.append(mapStr->getString(tmpFile.c_str()));
//				Logger::log("LOADOBJ: [%s]\n",objFn.substr(filenameOfs,objFn.length()-filenameOfs).c_str());

			}
			else
			{
				 objFnTrimmed.append(baseDir);
				 objFnTrimmed.append(objFn.substr(filenameOfs,objFn.length()-filenameOfs));
			}

			Logger::log("LOADOBJ: [%s]\n",objFnTrimmed.c_str());
			
			if ((obj_ref.find(objFnTrimmed) == obj_ref.end()) && objFnTrimmed.length()) // see if we've already loaded this one
			{
				newObj = new Object();
				obj_ref[objFnTrimmed] = newObj;

				loadLWO(*newObj,objFnTrimmed,mapStr);
				
				if (newObj->faces.size())
				{
#if !defined(OPENGL_ES) && !defined(ARCH_DC) && !defined(ARCH_PSP)
					newObj->triangulate();
#endif
					newObj->cache(true);
				}
				else
				{
					delete newObj;
					newObj = NULL;
					obj_ref[objFnTrimmed] = NULL;
				}
				
				newLight = NULL;
				newCam = NULL;
				newBone = -1;

				//scene.object(newObj).cache(true);
			}
			else
			{
				newObj = obj_ref[objFnTrimmed];
				newLight = NULL;
				newCam = NULL;
				newBone = -1;
			}
			
			string sceneObjId;
			vector<string> tmpStr;
			
			newSceneObj = new SceneObject();
			
			sceneObj_ref[objId] = newSceneObj;

			str_explode(tmpStr,".",objFnTrimmed);
			if (tmpStr.size()!=1 && tmpStr.size()) tmpStr.pop_back();
			str_implode(sceneObjId ,".",tmpStr);
			
			sceneObjId = scene.nextSceneObjectName(sceneObjId);
			
			newSceneObj->setId(sceneObjId);
			newSceneObj->setModelId(objFnTrimmed);
			
			printf("Generated Id: %s\n",sceneObjId.c_str());
			
			newSceneObj->bind(*newObj);
			newSceneObj->setMatrixLock(false);
			scene.bind(*newSceneObj);
		}
		
		if (input.compare("AddNullObject") == 0)
		{
			string nullId;
			string nullName;
			
//			getline(File,tempName,nlchar);
			
//			unsigned int nameOfs = tempName.find_last_of(' ');
//
//			if (nameOfs != string::npos) nameOfs = tempName.find_last_not_of(' ',nameOfs+1);
//			if (nameOfs == string::npos) nameOfs = 0;
			
//			string objName(tempName.substr(nameOfs,tempName.length()-nameOfs));
			
			File >> nullId;
			File >> nullName;

			Logger::log("Loaded null obj: [%s]\n",nullName.c_str());

			newSceneObj = new SceneObject();
			newSceneObj->setId(nullName);
			scene.bind(*newSceneObj);
//			sceneObj_ref.push_back(newSceneObj);
			sceneObj_ref[nullId] = newSceneObj;
		}

		if (input.compare("NumChannels") == 0)
		{
			int numChannels, currentChannel = 0;

			File >> numChannels;

			while (currentChannel < numChannels)
			{
				File >> input;
				
				if (input.compare("Channel") == 0)
				{
					int channelNum; 
					
					File >> channelNum >> input;
					
					if (input.compare("{") != 0)
					{
						Logger::log("no channel opening brace\n");;
						continue;
					}
					
					File >> input;
					
					int numKeys, keyNum = 0;
					
					File >> numKeys;

					while (!File.eof())
					{
						File >> input;
						
						if (input.compare("}") == 0) break;
					
						if (input.compare("Key") == 0)
						{
							float value, time, p1, p2, p3, p4, p5, p6;
							int spantype;
							
							File >> value >> time >> spantype >> p1 >> p2 >> p3 >> p4 >> p5 >> p6;
							
							EnvelopeKey *tKey = NULL;

							if (motionType == OBJECT_MOTION)
							{
								switch (channelNum)
								{
									case 0: tKey = sceneMotion->setKey(*newSceneObj,CONTROLLER_POSITION,MOTION_X,time,-value); break;
									case 1: tKey = sceneMotion->setKey(*newSceneObj,CONTROLLER_POSITION,MOTION_Y,time,value); break;
									case 2: tKey = sceneMotion->setKey(*newSceneObj,CONTROLLER_POSITION,MOTION_Z,time,value); break;
									case 3: tKey = sceneMotion->setKey(*newSceneObj,CONTROLLER_ROTATION,MOTION_Y,time,RADTODEG(-value)); break;
									case 4: tKey = sceneMotion->setKey(*newSceneObj,CONTROLLER_ROTATION,MOTION_X,time,RADTODEG(value)); break;
									case 5: tKey = sceneMotion->setKey(*newSceneObj,CONTROLLER_ROTATION,MOTION_Z,time,RADTODEG(-value)); break;
									case 6: tKey = sceneMotion->setKey(*newSceneObj,CONTROLLER_SCALE,MOTION_X,time,value); break;
									case 7: tKey = sceneMotion->setKey(*newSceneObj,CONTROLLER_SCALE,MOTION_Y,time,value); break;
									case 8: tKey = sceneMotion->setKey(*newSceneObj,CONTROLLER_SCALE,MOTION_Z,time,value); break;
								};
							}

							if (motionType == CAMERA_MOTION)
							{
								switch (channelNum)
								{
									case 0: tKey = sceneMotion->setKey(*newCam,CONTROLLER_POSITION,MOTION_X,time,-value); break;
									case 1: tKey = sceneMotion->setKey(*newCam,CONTROLLER_POSITION,MOTION_Y,time,value); break;
									case 2: tKey = sceneMotion->setKey(*newCam,CONTROLLER_POSITION,MOTION_Z,time,value); break;
									case 3: tKey = sceneMotion->setKey(*newCam,CONTROLLER_ROTATION,MOTION_Y,time,RADTODEG(-value)+180.0f); break;
									case 4: tKey = sceneMotion->setKey(*newCam,CONTROLLER_ROTATION,MOTION_X,time,RADTODEG(-value)); break;
									case 5: tKey = sceneMotion->setKey(*newCam,CONTROLLER_ROTATION,MOTION_Z,time,RADTODEG(-value)); break;
								};
							}

							if (motionType == LIGHT_MOTION)
							{
								switch (channelNum)
								{
									case 0: tKey = sceneMotion->setKey(*newLight,CONTROLLER_POSITION,MOTION_X,time,-value); break;
									case 1: tKey = sceneMotion->setKey(*newLight,CONTROLLER_POSITION,MOTION_Y,time,value); break;
									case 2: tKey = sceneMotion->setKey(*newLight,CONTROLLER_POSITION,MOTION_Z,time,value); break;
									case 3: tKey = sceneMotion->setKey(*newLight,CONTROLLER_ROTATION,MOTION_Y,time,RADTODEG(-value)); break;
									case 4: tKey = sceneMotion->setKey(*newLight,CONTROLLER_ROTATION,MOTION_X,time,RADTODEG(value)); break;
									case 5: tKey = sceneMotion->setKey(*newLight,CONTROLLER_ROTATION,MOTION_Z,time,RADTODEG(-value)); break;
								};
							}
#ifndef ARCH_PSP							
							if (motionType == BONE_MOTION)
							{
								switch (channelNum)
								{
									case 0: tKey = sceneMotion->setKey(*bones->bone(newBone),CONTROLLER_POSITION,MOTION_X,time,-value); break;
									case 1: tKey = sceneMotion->setKey(*bones->bone(newBone),CONTROLLER_POSITION,MOTION_Y,time,value); break;
									case 2: tKey = sceneMotion->setKey(*bones->bone(newBone),CONTROLLER_POSITION,MOTION_Z,time,value); break;
									case 3: tKey = sceneMotion->setKey(*bones->bone(newBone),CONTROLLER_ROTATION,MOTION_Y,time,RADTODEG(-value)); break;
									case 4: tKey = sceneMotion->setKey(*bones->bone(newBone),CONTROLLER_ROTATION,MOTION_X,time,RADTODEG(value)); break;
									case 5: tKey = sceneMotion->setKey(*bones->bone(newBone),CONTROLLER_ROTATION,MOTION_Z,time,RADTODEG(-value)); break;
								};
							}
							if (tKey)
							{
								tKey->shape = spantype;
								tKey->tension = p1;
								tKey->continuity = p2;
								tKey->bias = p3;
								tKey->param[0] = p4;
								tKey->param[1] = p5;
							};
#endif

							keyNum++;
						}
						
						if (input.compare("Behaviors") == 0)
						{
							int in_behavior, out_behavior;
							
							File >> in_behavior >> out_behavior;
							if (motionType == OBJECT_MOTION)
							{
								switch (channelNum)
								{
									case 0: sceneMotion->setBehavior(*newSceneObj,CONTROLLER_POSITION, MOTION_X, in_behavior, out_behavior); break;
									case 1: sceneMotion->setBehavior(*newSceneObj,CONTROLLER_POSITION, MOTION_Y, in_behavior, out_behavior); break;
									case 2: sceneMotion->setBehavior(*newSceneObj,CONTROLLER_POSITION, MOTION_Z, in_behavior, out_behavior); break;
									case 3: sceneMotion->setBehavior(*newSceneObj,CONTROLLER_ROTATION, MOTION_X, in_behavior, out_behavior); break;
									case 4: sceneMotion->setBehavior(*newSceneObj,CONTROLLER_ROTATION, MOTION_Y, in_behavior, out_behavior); break;
									case 5: sceneMotion->setBehavior(*newSceneObj,CONTROLLER_ROTATION, MOTION_Z, in_behavior, out_behavior); break;
									case 6: sceneMotion->setBehavior(*newSceneObj,CONTROLLER_SCALE, MOTION_X, in_behavior, out_behavior); break;
									case 7: sceneMotion->setBehavior(*newSceneObj,CONTROLLER_SCALE, MOTION_Y, in_behavior, out_behavior); break;
									case 8: sceneMotion->setBehavior(*newSceneObj,CONTROLLER_SCALE, MOTION_Z, in_behavior, out_behavior); break;
								};
							}							
							else if (motionType == CAMERA_MOTION)
							{
								switch (channelNum)
								{
									case 0: sceneMotion->setBehavior(*newCam,CONTROLLER_POSITION, MOTION_X, in_behavior, out_behavior); break;
									case 1: sceneMotion->setBehavior(*newCam,CONTROLLER_POSITION, MOTION_Y, in_behavior, out_behavior); break;
									case 2: sceneMotion->setBehavior(*newCam,CONTROLLER_POSITION, MOTION_Z, in_behavior, out_behavior); break;
									case 3: sceneMotion->setBehavior(*newCam,CONTROLLER_ROTATION, MOTION_X, in_behavior, out_behavior); break;
									case 4: sceneMotion->setBehavior(*newCam,CONTROLLER_ROTATION, MOTION_Y, in_behavior, out_behavior); break;
									case 5: sceneMotion->setBehavior(*newCam,CONTROLLER_ROTATION, MOTION_Z, in_behavior, out_behavior); break;
								};
							}
							else if (motionType == LIGHT_MOTION)
							{
								switch (channelNum)
								{
									case 0: sceneMotion->setBehavior(*newLight,CONTROLLER_POSITION, MOTION_X, in_behavior, out_behavior); break;
									case 1: sceneMotion->setBehavior(*newLight,CONTROLLER_POSITION, MOTION_Y, in_behavior, out_behavior); break;
									case 2: sceneMotion->setBehavior(*newLight,CONTROLLER_POSITION, MOTION_Z, in_behavior, out_behavior); break;
									case 3: sceneMotion->setBehavior(*newLight,CONTROLLER_ROTATION, MOTION_X, in_behavior, out_behavior); break;
									case 4: sceneMotion->setBehavior(*newLight,CONTROLLER_ROTATION, MOTION_Y, in_behavior, out_behavior); break;
									case 5: sceneMotion->setBehavior(*newLight,CONTROLLER_ROTATION, MOTION_Z, in_behavior, out_behavior); break;
								};
							}
#ifndef ARCH_PSP
							else if (motionType == BONE_MOTION)
							{
								switch (channelNum)
								{
									case 0: sceneMotion->setBehavior(*bones->bone(newBone),CONTROLLER_POSITION, MOTION_X, in_behavior, out_behavior); break;
									case 1: sceneMotion->setBehavior(*bones->bone(newBone),CONTROLLER_POSITION, MOTION_Y, in_behavior, out_behavior); break;
									case 2: sceneMotion->setBehavior(*bones->bone(newBone),CONTROLLER_POSITION, MOTION_Z, in_behavior, out_behavior); break;
									case 3: sceneMotion->setBehavior(*bones->bone(newBone),CONTROLLER_ROTATION, MOTION_X, in_behavior, out_behavior); break;
									case 4: sceneMotion->setBehavior(*bones->bone(newBone),CONTROLLER_ROTATION, MOTION_Y, in_behavior, out_behavior); break;
									case 5: sceneMotion->setBehavior(*bones->bone(newBone),CONTROLLER_ROTATION, MOTION_Z, in_behavior, out_behavior); break;
								};
							};
#endif
						}
					}
					currentChannel++;
				}
			}
		}
		
		if (input.compare("ParentItem") == 0)
		{
			string parentItem;
			
			File >> parentItem;
			
			Logger::log("ParentItem: %s\n",parentItem.c_str());
			
//			int type,itemNum;
//			
//			type = strtoul(parentItem.substr(0,1).c_str(), NULL, 16);
//			itemNum = strtoul(parentItem.substr(1,7).c_str(), NULL, 16);
			
			if (newSceneObj != NULL)
			{
				childRef.push_back(newSceneObj);
				parentRef.push_back(parentItem);
			};

			if (newLight != NULL)
			{
				light_childRef.push_back(newLight);
				light_parentRef.push_back(parentItem);
			};

			if (newBone != -1)
			{				
				bone_childRef.push_back(newBone);
				bone_parentRef.push_back(parentItem);
			};			
			
		}


		if (input.compare("ShadowOptions") == 0)
		{
			#define SHADOWOPT_SELF 1
			#define SHADOWOPT_CAST 2
			#define SHADOWOPT_RECV 4
			
			int shadowOpts;
			
			File >> shadowOpts;
			
			if (motionType == OBJECT_MOTION)
			{
				newSceneObj->shadowSelf(shadowOpts & SHADOWOPT_SELF);
				newSceneObj->shadowCast(shadowOpts & SHADOWOPT_CAST);
				newSceneObj->shadowReceive(shadowOpts & SHADOWOPT_RECV);
			};
		}

	}
	
	for (i = 0; i < (int)childRef.size(); i++)
	{
		Logger::log("parented object %d to object: %s\n", childRef[i], parentRef[i].c_str());
		childRef[i]->bindParent(*sceneObj_ref[parentRef[i]]);
	}

	for (i = 0; i < (int)light_childRef.size(); i++)
	{
		Logger::log("parented light %d to scene object: %s\n", light_childRef[i], light_parentRef[i].c_str());
		light_childRef[i]->bindParent(*sceneObj_ref[light_parentRef[i]]);
	}

#ifndef ARCH_PSP
	for (i = 0; i < (int)bone_childRef.size(); i++)
	{
		if (bone_parentRef[i][0] == '1')
		{
			Logger::log("parented bone %d to scene object: %s\n", bone_childRef[i], bone_parentRef[i].c_str());
			sceneObj_ref[bone_parentRef[i]]->bones = bones;
		}
		else
		{
			Logger::log("parented bone %d to bone: %s\n", bone_childRef[i], bone_parentRef[i].c_str());
			bones->bone(bone_childRef[i])->bindParent(*bones->bone(bone_ref[bone_parentRef[i]]));
		}
	}
#endif
	File.close();
};
