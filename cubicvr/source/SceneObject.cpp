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

#include <CubicVR/Light.h>
#include <CubicVR/SceneObject.h>
#include <CubicVR/Transform.h>
#include <CubicVR/CollisionMap.h>


#ifdef ARCH_DC
static matrix_t ml __attribute__((aligned(32)));
#endif

SceneObject::SceneObject() : parent(NULL), cmap(NULL), obj(NULL), bones(NULL), segmentMask(NULL), shadow_self(true), shadow_cast(true), shadow_receive(true), isvisible(true), active(true), hasVisibility(false), aabbMin(0,0,0), aabbMax(0,0,0)
#ifndef ARCH_PSP
	,dynamic_colors(NULL)
#endif
{
//	center = XYZ(0,0,0);
	Resource::typeName = std::string("sceneobject");
	aabbMax_last = aabbMax = XYZ(0,0,0);
	aabbMin_last = aabbMin = XYZ(0,0,0);
	
	dynamic_object = false;
	bounds_only = false;
	mass = 0;
	restitution = 0;
	friction = 0;
};


SceneObject::SceneObject(Mesh &obj_in) : parent(NULL), cmap(NULL), bones(NULL), segmentMask(NULL), shadow_self(true), shadow_cast(true), shadow_receive(true), isvisible(true), active(true), hasVisibility(false), aabbMin(0,0,0), aabbMax(0,0,0)
#ifndef ARCH_PSP
	,dynamic_colors(NULL)
#endif
{
	obj = &obj_in;
	Resource::typeName = std::string("sceneobject");
	aabbMax_last = aabbMax = XYZ(0,0,0);
	aabbMin_last = aabbMin = XYZ(0,0,0);
	
	dynamic_object = false;
	bounds_only = false;
	mass = 0;
	restitution = 0;
	friction = 0;
};	


SceneObject::~SceneObject()
{

};


//public:
//		
//	void bindResourceModel(ResourceModel *model_in)
//	{
//		model = model_in;
//		modelType = model_in->getType();
//		modelId = model_in->getId();
//		
//		SceneObject::bind(model->getObject());
//	};
//};

Resource *SceneObject::upcast(ResourceManager *rm_in, Resource *res_in)
{
	SceneObject *new_obj = new SceneObject();
	new_obj->Import(*res_in);
	new_obj->onLoad();

	if (new_obj->getModelType() != string("") && new_obj->getModelId() != string(""))
	{
		if (rm_in->hasResource(new_obj->getModelType(),new_obj->getModelId()))
		{
			Mesh *bindModel = (Mesh *)rm_in->upcast(&rm_in->getResource(new_obj->getModelType(),new_obj->getModelId()));
			if (!bindModel->isInitialized()) bindModel->init();
			new_obj->bind(*bindModel);
		}	
	}

	string parent_id = new_obj->getParentId();
	
	if (parent_id != "")
	{
		Resource *tmpRes = &rm_in->getResource("sceneobject",parent_id);
		if (tmpRes)
		{		
			SceneObject *sceneObj_parent = (SceneObject *)rm_in->upcast(tmpRes);
			if (sceneObj_parent)
			{
				sceneObj_parent->bindChild(*new_obj);
//				sceneObj_parent->clearBounds();
			}
		}
		else
		{
			Logger::log(LOG_ERROR,"Unable to locate parent [%s] for scene object [%s] in resource manager.\n",parent_id.c_str(),res_in->getId().c_str());
		}
	}
	
	return new_obj;
}


bool SceneObject::onLoad() 
{
	arrayParam.clear();
	
	if (properties->rootNode().hasAnother("model_type")) properties->rootNode().child("model_type").element().get(modelType);
	if (properties->rootNode().hasAnother("model_id")) properties->rootNode().child("model_id").element().get(modelId);
	
	if (properties->rootNode().hasAnother("position")) properties->rootNode().child("position").element().get(position);
	if (properties->rootNode().hasAnother("scale")) properties->rootNode().child("scale").element().get(scale);
	if (properties->rootNode().hasAnother("rotation")) properties->rootNode().child("rotation").element().get(rotation);
		
	// Physics
	if (properties->rootNode().hasAnother("dynamic_object")) properties->rootNode().child("dynamic_object").element().get((int&)dynamic_object);
	if (properties->rootNode().hasAnother("mass")) properties->rootNode().child("mass").element().get(mass);
	if (properties->rootNode().hasAnother("friction")) properties->rootNode().child("friction").element().get(friction);
	if (properties->rootNode().hasAnother("restitution")) properties->rootNode().child("restitution").element().get(restitution);
//	properties->rootNode().hasAnother("bounds_only")		? true : false;
//	if (properties->rootNode().hasAnother("mass"))			setMass(properties->rootNode().child("mass").element().getFloat());
//	if (properties->rootNode().hasAnother("friction"))		friction	= properties->rootNode().child("friction").element().getDouble();
//	if (properties->rootNode().hasAnother("restitution"))	restitution = properties->rootNode().child("restitution").element().getDouble();
	
	// Physics info
/*
	printf("-------------------\nPhysics Properties\n-------------------\n");
	printf("dynamic_object?\t%d\n", dynamic_object);
	printf("bounds_only?\t%d\n", bounds_only);
	printf("mass?\t %f\n", mass);
	printf("friction?\t %f\n", friction);
	printf("restitution?\t %f\n", restitution);
	printf("-------------------\n");
*/
	
	//if (properties->rootNode().hasAnother("friction")) properties->rootNode().child("friction").element().get(friction);
	//if (properties->rootNode().hasAnother("restitution")) properties->rootNode().child("restitution").element().get(restitution);
		
	// Array
	if (properties->rootNode().hasAnother("array"))  properties->rootNode().child("array").element().get(arrayParam);
	if (properties->rootNode().hasAnother("array_spacing"))  properties->rootNode().child("array_spacing").element().get(arraySpacing);
		
	if (properties->rootNode().hasAnother("parent")) properties->rootNode().child("parent").element().get(parentId);	
	
	if (arrayParam.size() > 6) arrayParam.resize(6);
	
	w_init = l_init = false;
	
	return true;
};

bool SceneObject::onSave() 
{
	if (properties) delete properties;
	
	properties = new DataTree();
	properties->rootNode().newChild("model_type").element().set(modelType);
	properties->rootNode().newChild("model_id").element().set(modelId);
		
	properties->rootNode().newChild("position").element().set(position);
	properties->rootNode().newChild("scale").element().set(scale);
	properties->rootNode().newChild("rotation").element().set(rotation);
	
	// Physics
	if(dynamic_object)	properties->rootNode().newChild("dynamic_object").element().set(dynamic_object);
	if(bounds_only)		properties->rootNode().newChild("bounds_only").element().set(bounds_only);
	
	if (mass) properties->rootNode().newChild("mass").element().set(mass);
	if (friction) properties->rootNode().newChild("friction").element().set(friction);
	if (restitution) properties->rootNode().newChild("restitution").element().set(restitution);
	
	// Array
	if (!arrayParam.empty()) properties->rootNode().newChild("array").element().set(arrayParam);
	if (arraySpacing != XYZ(0,0,0)) properties->rootNode().newChild("array_spacing").element().set(arraySpacing);
	
	clearTransform();
	
	if (hasParent())
	{
		properties->rootNode().newChild("parent").element().set(parent->getId());	
	}
	
	return true;
};


void SceneObject::setDynamicObject(bool active) { dynamic_object = active; }
void SceneObject::setBoundsOnly(bool active) { bounds_only = active; }
void SceneObject::setMass(float m) { mass = m; }
void SceneObject::setFriction(float f) { friction = f; }
bool SceneObject::getDynamicObject() { return dynamic_object; }
bool SceneObject::getBoundsOnly() { return bounds_only; }
float SceneObject::getMass() { return mass; }
float SceneObject::getFriction() { return friction; }

string &SceneObject::getModelId() 
{ 
	return modelId; 
};

void SceneObject::setModelId(const string &m_id) 
{ 
	modelId = m_id; 
};

string &SceneObject::getParentId() 
{ 
	return parentId; 
};

void SceneObject::setParentId(const string &p_id) 
{ 
	parentId = p_id; 
};


string &SceneObject::getModelType() 
{ 
	return modelType; 
};

void SceneObject::setModelType(const string &m_type) 
{ 
	modelType = m_type; 
};


void SceneObject::setCollisionMap(Mesh *objMap)
{
	if (objMap == NULL)
	{
		if (obj == NULL) return;
		
		objMap = new Mesh();
		objMap->cloneStructure(*obj);
#if !defined(ARCH_PSP) && !defined(ARCH_DC) && !defined(OPENGL_ES)
		objMap->triangulate();
#endif
	}
	
	if (!cmap) cmap = new CollisionMap();
	
	cmap->addMesh(*objMap);
}


void SceneObject::compileSphereMap(float nominal_rad)
{
	if (cmap) cmap->compileSphereMap(nominal_rad);
}


#ifndef ARCH_PSP

void SceneObject::calculateBoneMap(BoneSet *bones)
{
	if (!obj) return;
	if (!obj->cache_state) 
	{
		Logger::log("Error, can only calculate bone maps for cached objects!\n");
		return;
	}
	
	bones->bone(((int)0))->calcNodes();
		
//	std::map<cvrIndex, std::set<cvrIndex>, ltindex> *boneMap = &bone_map[(cvrIndex)bones];
//	std::vector<XYZ*> *points = &obj->points;
//	std::vector<XYZ*>::iterator point_i;
	
	bone_vertex_index.clear();
	bone_vertex_weight.clear();
	
	va_xyz blnk;
	blnk.x=blnk.y=blnk.z=0.0f;
	
	bone_vertex_index.resize(obj->cache_data.vertex_count,blnk);
	bone_vertex_weight.resize(obj->cache_data.vertex_count,blnk);
	
	obj->openBuffer();
	
	va_xyz *vertexBuffer = (va_xyz *) (obj->open_buffer+obj->pointCacheOffset);
	
	for (unsigned int i = 0; i < obj->cache_data.vertex_count; i++)
	{
		XYZ pt = XYZ(vertexBuffer[i].x,vertexBuffer[i].y,vertexBuffer[i].z);
			
		for (unsigned int j = 0; j < bones->bones.size(); j++)
		{
			XYZ test_pt = Vector::getClosestTo(bones->bones[j]->start_pt, bones->bones[j]->end_pt, pt);

			XYZ pt1 = bones->bones[j]->start_pt;
			XYZ pt2 = bones->bones[j]->end_pt;
			
			float dist;
			
			if (
				((test_pt.x >= pt1.x && test_pt.x <= pt2.x) || (test_pt.x >= pt2.x && test_pt.x <= pt1.x)) &&
				((test_pt.y >= pt1.y && test_pt.y <= pt2.y) || (test_pt.y >= pt2.y && test_pt.y <= pt1.y)) &&
				((test_pt.z >= pt1.z && test_pt.z <= pt2.z) || (test_pt.z >= pt2.z && test_pt.z <= pt1.z))
			)
			{
				dist = Vector(test_pt,pt).magnitude();
			}
			else
			{
				float dist1 = Vector(test_pt,pt1).magnitude();
				float dist2 = Vector(test_pt,pt2).magnitude();
				
				dist = (dist1>dist2)?dist2:dist1;
			}
			
			printf("weighting dist: %f\n",dist);
			
			float influence = 0.2f;
			
			dist /= influence;
			
			if (dist < 1.0f) dist = 1.0f;
			float weight = (1.0f / (dist*dist));
			
			bone_vertex_index[i].z=bone_vertex_index[i].y;
			bone_vertex_index[i].y=bone_vertex_index[i].x;
			bone_vertex_index[i].x=j;
			
			bone_vertex_weight[i].z=bone_vertex_weight[i].y;
			bone_vertex_weight[i].y=bone_vertex_weight[i].x;
			bone_vertex_weight[i].x=weight;

			printf("index[v:%d,b:%d]: %f, %f, %f\n",i,j,bone_vertex_index[i].x,bone_vertex_index[i].y,bone_vertex_index[i].z);
			printf("weight[v:%d,b:%d]: %f, %f, %f\n",i,j,bone_vertex_weight[i].x,bone_vertex_weight[i].y,bone_vertex_weight[i].z);
			
		}		
	}
	
	obj->closeBuffer();
	
	
//	bone_vertex_map.resize(obj->points.size());
	
	
//	boneMap = 
}
#endif

XYZ &SceneObject::getArraySpacing()
{
	return arraySpacing;
}


void SceneObject::setArrayParam(vector<int> &arrayParam_in)
{
	vector<int>::iterator i;
	
	arrayParam.clear();
	
	for (i = arrayParam_in.begin(); i < arrayParam_in.end(); i++)
	{
		arrayParam.push_back(*i);
	}
}

vector<int> &SceneObject::getArrayParam() 
{ 
	return arrayParam; 
}


void SceneObject::setArrayParam(int right, int top, int back, int left, int bottom, int front)
{
	if (!arrayParam.empty()) arrayParam.clear();
	
	if (right && top && back && left && bottom && front)
	{
		arrayParam.push_back(right);
		arrayParam.push_back(top);
		arrayParam.push_back(back);
		arrayParam.push_back(left);
		arrayParam.push_back(bottom);
		arrayParam.push_back(front);
	}	
}

void SceneObject::setArrayParam(SceneObjectArrayParam param, int val)
{
	if (arrayParam.empty()) arrayParam.resize(6,0);
	
	arrayParam[param] = val;
}


void SceneObject::control(int controllerId, int motionId, float value)
{
	switch (controllerId)
	{
		case CONTROLLER_POSITION:
			switch (motionId)
			{
				case MOTION_X: position.x = value; break;
				case MOTION_Y: position.y = value; break;
				case MOTION_Z: position.z = value; break;
			}
			clearTransform();
		break;
		
		case CONTROLLER_ROTATION:
			switch (motionId)
			{
				case MOTION_X: rotation.x = value; break;
				case MOTION_Y: rotation.y = value; break;
				case MOTION_Z: rotation.z = value; break;
			}
			clearTransform();
		break;
		
		case CONTROLLER_SCALE:
			switch (motionId)
			{
				case MOTION_X: scale.x = value; break;
				case MOTION_Y: scale.y = value; break;
				case MOTION_Z: scale.z = value; break;
			}
			clearTransform();
		break;
	}
};


float SceneObject::read_control(int controllerId, int motionId)
{
	switch (controllerId)
	{
		case CONTROLLER_POSITION:
			switch (motionId)
			{
				case MOTION_X: return position.x;
				case MOTION_Y: return position.y;
				case MOTION_Z: return position.z;
			}
		break;
		
		case CONTROLLER_ROTATION:
			switch (motionId)
			{
				case MOTION_X: return rotation.x;
				case MOTION_Y: return rotation.y;
				case MOTION_Z: return rotation.z;
			}
		break;
		
		case CONTROLLER_SCALE:
			switch (motionId)
			{
				case MOTION_X: return scale.x;
				case MOTION_Y: return scale.y;
				case MOTION_Z: return scale.z;
			}
		break;
	}
	
	return 0;
};


void SceneObject::bind(Mesh &obj_in)
{
	obj = &obj_in;
	if (obj)
	{
//		if (obj->numSegments > 1)
//		{
//			segmentMask = new BITSET;
//			segmentMask->init(obj->numSegments);
//		}
		setMatrixLock(false);
		clearTransform();
		if (obj->faces.size()) calcAABB();		
	}
};


void SceneObject::bindChild(SceneObject &sceneobj_in)
{
	children.insert(&sceneobj_in);
	sceneobj_in.parent = this;
	sceneobj_in.p_trans = this;
	sceneobj_in.parentId = getId();
//	sceneobj_in.clearTransform();
//	sceneobj_in.calcAABB();
};


void SceneObject::bindParent(SceneObject &sceneObj_in)
{
	sceneObj_in.bindChild(*this);
};


bool SceneObject::hasParent()
{
	return (parent != NULL);
};


bool SceneObject::getAllChildren(vector<SceneObject *> &child_list)
{

	if (children.size())
	{
		for (children_i = children.begin(); children_i != children.end(); children_i++)
		{
			if ((*children_i)->children.size())
			{
				(*children_i)->getAllChildren(child_list);
			}
			
			child_list.push_back(*children_i);
		}
		
		return true;
	}
	
	return false;
}


void SceneObject::unbindChild(SceneObject &sceneobj_in)
{
	if (children.find(&sceneobj_in) != children.end())
	{
		children.erase(&sceneobj_in);
		sceneobj_in.parent = NULL;
		sceneobj_in.p_trans = NULL;
		sceneobj_in.setParentId("");
	}
};

void SceneObject::render(Renderer &renderer, int stage, bool initShadowMatrix, bool no_pivot, bool no_scale)
{	
//	float obj_trans[16];
	
//	if (renderer.lod)
//	{
//		XYZ bbdelta = aabbMax-aabbMin;
//		float bbDist = (bbdelta.x>bbdelta.y)?((bbdelta.x>bbdelta.z)?bbdelta.x:bbdelta.z):((bbdelta.y>bbdelta.z)?bbdelta.y:bbdelta.z);
//		XYZ lod_pos = (aabbMin+aabbMax)*0.5f;
//		Vector lod_dist(lod_pos,renderer.lod_src);
//		if ((lod_dist.magnitude()/20.0f)>bbDist) 
//		{
//			return; 
//		}
//	}
	
#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)
	if (bones && bone_vertex_index.size())
	{
		GLShader::defaultShader.setShaderAttrib("bone_index", &bone_vertex_index[0]);
		GLShader::defaultShader.setShaderAttrib("bone_weight", &bone_vertex_weight[0]);	
	}
#endif
	
	glMatrixMode(GL_MODELVIEW);

	#ifndef ARCH_PSP
	if (obj) if (obj->hasColorMap)
	{
		if (dynamic_colors) 
		{
			calcVertexLightMap(renderer);
			obj->dynamic_colors = dynamic_colors;
		}
		else
		{
			obj->dynamic_colors = NULL;
		}
	}

	#endif

	transformBegin(no_pivot,no_scale);

//	if (center.x||center.y||center.z) 
//	{
//		glTranslatef(-center.x,-center.y,-center.z);
//	}

#if !defined(ARCH_PSP) && !defined(OPENGL_ES)  && !defined(ARCH_DC)
	float s_trans[16];
	
	if (initShadowMatrix) 
	{
//		memcpy(s_trans,w_trans,sizeof(float)*16);
		glPushMatrix();
		glLoadIdentity();
		glLoadMatrixf(w_trans);
		glScalef(scale.x,scale.y,scale.z);
		glGetFloatv(GL_MODELVIEW_MATRIX,s_trans);
//		s_trans[0] *= scale.x;
//		s_trans[5] *= scale.y;
//		s_trans[10] *= scale.z;
		glPopMatrix();
		
		renderer.shadowModelMatrix = s_trans;
		
		GLShader::defaultShader.setShaderValue("objMatrix",s_trans);
	}
#endif
	
	if (obj) 
	{
		if (!arrayParam.empty())
		{
			XYZ bb1,bb2,bb;
			
			obj->calcBB();
			obj->getBoundingBox(bb1,bb2);
			bb = bb2-bb1;
			
			int end[3];
			
			end[0] = arrayParam[0]+arrayParam[3];
			end[1] = arrayParam[1]+arrayParam[4];
			end[2] = arrayParam[2]+arrayParam[5];
									
			glTranslatef((-bb.x-arraySpacing.x)*(float)arrayParam[3],(-bb.y-arraySpacing.y)*(float)arrayParam[4],(-bb.z-arraySpacing.z)*(float)arrayParam[5]);
#if !defined(ARCH_PSP) && !defined(OPENGL_ES)  && !defined(ARCH_DC)
			if (initShadowMatrix)
			{
				glPushMatrix();
				glLoadMatrixf(s_trans);
				glTranslatef((-bb.x-arraySpacing.x)*(float)arrayParam[3],(-bb.y-arraySpacing.y)*(float)arrayParam[4],(-bb.z-arraySpacing.z)*(float)arrayParam[5]);
				glGetFloatv(GL_MODELVIEW_MATRIX, s_trans);
				glPopMatrix();
			}
#endif			
			for (int a = 0; a <= end[0]; a++)
			{
				if (a) glTranslatef(bb.x+arraySpacing.x,0,0);

#if !defined(ARCH_PSP) && !defined(OPENGL_ES)  && !defined(ARCH_DC)
				if (initShadowMatrix)
				{
					glPushMatrix();
					glLoadMatrixf(s_trans);
					if (a) glTranslatef(bb.x+arraySpacing.x,0,0);
					glGetFloatv(GL_MODELVIEW_MATRIX, s_trans);
					glPopMatrix();
				}
#endif

				for (int b = 0; b <= end[1]; b++)
				{
					if (b) glTranslatef(0,bb.y+arraySpacing.y,0);

#if !defined(ARCH_PSP) && !defined(OPENGL_ES)  && !defined(ARCH_DC)
					if (initShadowMatrix)
					{
						glPushMatrix();
						glLoadMatrixf(s_trans);
						if (b) glTranslatef(0,bb.y+arraySpacing.y,0);
						glGetFloatv(GL_MODELVIEW_MATRIX, s_trans);
						glPopMatrix();
					}					
#endif
					
					for (int c = 0; c <= end[2]; c++)
					{
						if (c) glTranslatef(0,0,bb.z+arraySpacing.z);

#if !defined(ARCH_PSP) && !defined(OPENGL_ES)  && !defined(ARCH_DC)
						if (initShadowMatrix)
						{
							glPushMatrix();
							glLoadMatrixf(s_trans);
							if (c) glTranslatef(0,0,bb.z+arraySpacing.z);
							glGetFloatv(GL_MODELVIEW_MATRIX, s_trans);
							glPopMatrix();
						}					
#endif						
						
					#if !defined(ARCH_PSP) && !defined(OPENGL_ES)  && !defined(ARCH_DC)
						if (initShadowMatrix) 
						{
//							glGetFloatv(GL_MODELVIEW_MATRIX, w_trans);
							
							GLShader::defaultShader.setShaderValue("objMatrix",s_trans);				
						}
					#endif
						
						renderer.render(*obj,stage);

					}

#if !defined(ARCH_PSP) && !defined(OPENGL_ES)  && !defined(ARCH_DC)
					if (initShadowMatrix)
					{
						glPushMatrix();
						glLoadMatrixf(s_trans);
						if (end[2]) glTranslatef(0,0,(-bb.z-arraySpacing.z)*(float)end[2]);
						glGetFloatv(GL_MODELVIEW_MATRIX, s_trans);
						glPopMatrix();
					}					
#endif
					
					if (end[2]) glTranslatef(0,0,(-bb.z-arraySpacing.z)*(float)end[2]);
				}

#if !defined(ARCH_PSP) && !defined(OPENGL_ES)  && !defined(ARCH_DC)
				if (initShadowMatrix)
				{
					glPushMatrix();
					glLoadMatrixf(s_trans);
					if (end[1]) glTranslatef(0,(-bb.y-arraySpacing.y)*(float)end[1],0);				
					glGetFloatv(GL_MODELVIEW_MATRIX, s_trans);
					glPopMatrix();
				}					
#endif				
				if (end[1]) glTranslatef(0,(-bb.y-arraySpacing.y)*(float)end[1],0);				
			}
		}
		else
		{
			renderer.render(*obj,stage);	
		}
	}

	renderer.shadowModelMatrix = NULL;

	transformEnd();
	
//	if (worldspaceChildren) transformEnd();

	//	glPushMatrix();
	for (children_i = children.begin(); children_i != children.end(); children_i++)
	{
		(*children_i)->render(renderer,stage,initShadowMatrix);
	}
	//	glPopMatrix();

//	if (!worldspaceChildren) transformEnd();
	
};




void SceneObject::setId(const std::string &id_in)
{
	Resource::setId(id_in);
	
	for (children_i = children.begin(); children_i != children.end(); children_i++)
	{
		(*children_i)->setParentId(id_in);
	}
};


bool SceneObject::aabbIntersect(XYZ &aabbMin1_in, XYZ &aabbMax1_in, XYZ &aabbMin2_in, XYZ &aabbMax2_in, XYZ &aabbMinI_out, XYZ &aabbMaxI_out)
{
	XYZ aabbMin1,aabbMin2,aabbMax1,aabbMax2;
	
	aabbMin1 = XYZ((aabbMin1_in.x < aabbMin2.x)?aabbMin1_in.x:aabbMin2_in.x,(aabbMin1_in.y < aabbMin2.y)?aabbMin1_in.y:aabbMin2_in.y,(aabbMin1_in.z < aabbMin2_in.z)?aabbMin1_in.z:aabbMin2_in.z);
	aabbMin2 = XYZ((aabbMin1_in.x < aabbMin2.x)?aabbMin2_in.x:aabbMin1_in.x,(aabbMin1_in.y < aabbMin2.y)?aabbMin2_in.y:aabbMin1_in.y,(aabbMin1_in.z < aabbMin2_in.z)?aabbMin2_in.z:aabbMin1_in.z);

	aabbMax1 = XYZ((aabbMax1_in.x < aabbMax2_in.x)?aabbMax1_in.x:aabbMax2_in.x,(aabbMax1_in.y < aabbMax2_in.y)?aabbMax1_in.y:aabbMax2_in.y,(aabbMax1_in.z < aabbMax2_in.z)?aabbMax1_in.z:aabbMax2_in.z);
	aabbMax2 = XYZ((aabbMax1_in.x < aabbMax2_in.x)?aabbMax2_in.x:aabbMax1_in.x,(aabbMax1_in.y < aabbMax2_in.y)?aabbMax2_in.y:aabbMax1_in.y,(aabbMax1_in.z < aabbMax2_in.z)?aabbMax2_in.z:aabbMax1_in.z);

	if (aabbMax1.x < aabbMin2.x || aabbMax1.y < aabbMin2.y || aabbMax1.z < aabbMin2.z) return false;
	
	aabbMinI_out = XYZ(aabbMin2.x,aabbMin2.y,aabbMin2.z);
	aabbMaxI_out = XYZ(aabbMax1.x,aabbMax1.y,aabbMax1.z);
	
	return true;
}



// true if aabb moved since last test
bool SceneObject::calcAABB()
{
	if (!obj) return false;
	
	XYZ p[8];
	
	XYZ aabbMin_tmp = aabbMin;
	XYZ aabbMax_tmp = aabbMax;

//	if (t_bounds) l_init = w_init = false;
	
	transformBegin(false,false,true);
			
	if (obj && (t_bounds))
	{
		obj->calcBB();
		
		if (obj->bbMin == obj->bbMax && children.empty())
		{
			aabbMin = XYZ(0,0,0);//-(scale*0.5f);
			aabbMax = XYZ(0,0,0);//+(scale*0.5f);
			
			return false;
		}

		aabbMin = obj->bbMin;
		aabbMax = obj->bbMax;
		

		if (!arrayParam.empty())
		{
			XYZ bb=aabbMax-aabbMin;
			
			aabbMax += XYZ((arraySpacing.x+bb.x)*(float)arrayParam[0],(arraySpacing.y+bb.y)*(float)arrayParam[1],(arraySpacing.z+bb.z)*(float)arrayParam[2]);
			aabbMin -= XYZ((arraySpacing.x+bb.x)*(float)arrayParam[3],(arraySpacing.y+bb.y)*(float)arrayParam[4],(arraySpacing.z+bb.z)*(float)arrayParam[5]);
		}
		

		if (scale.x!=1||scale.y!=1||scale.z!=1)
		{
			aabbMin.x*=scale.x;
			aabbMin.y*=scale.y;
			aabbMin.z*=scale.z;
			aabbMax.x*=scale.x;
			aabbMax.y*=scale.y;
			aabbMax.z*=scale.z;
		}
		
		XYZ obj_aabb = aabbMin;
		XYZ obj_bounds = aabbMax-aabbMin;
		
		p[0] = XYZ(obj_aabb.x,					obj_aabb.y,					obj_aabb.z);
		p[1] = XYZ(obj_aabb.x,					obj_aabb.y,					obj_aabb.z + obj_bounds.z);
		p[2] = XYZ(obj_aabb.x + obj_bounds.x,	obj_aabb.y,					obj_aabb.z);
		p[3] = XYZ(obj_aabb.x + obj_bounds.x,	obj_aabb.y,					obj_aabb.z + obj_bounds.z);
		p[4] = XYZ(obj_aabb.x,					obj_aabb.y + obj_bounds.y,	obj_aabb.z);
		p[5] = XYZ(obj_aabb.x,					obj_aabb.y + obj_bounds.y,	obj_aabb.z + obj_bounds.z);
		p[6] = XYZ(obj_aabb.x + obj_bounds.x,	obj_aabb.y + obj_bounds.y,	obj_aabb.z);
		p[7] = XYZ(obj_aabb.x + obj_bounds.x,	obj_aabb.y + obj_bounds.y,	obj_aabb.z + obj_bounds.z);

		XYZ aabbTest;
		
		matTransform(w_trans,p[0],aabbTest);
		aabbMin = aabbTest;
		aabbMax = aabbTest;
			
		for (int i = 1; i < 8; i++)
		{
			matTransform(w_trans,p[i],aabbTest);

			if (aabbMin.x > aabbTest.x) aabbMin.x = aabbTest.x;
			if (aabbMin.y > aabbTest.y) aabbMin.y = aabbTest.y;
			if (aabbMin.z > aabbTest.z) aabbMin.z = aabbTest.z;

			if (aabbMax.x < aabbTest.x) aabbMax.x = aabbTest.x;
			if (aabbMax.y < aabbTest.y) aabbMax.y = aabbTest.y;
			if (aabbMax.z < aabbTest.z) aabbMax.z = aabbTest.z;
		}
		
	
	}
	else if (t_bounds)
	{
		aabbMin = XYZ(0,0,0);//-(scale*0.5f);
		aabbMax = XYZ(0,0,0);//+(scale*0.5f);
	}
	
	if (!children.empty() && (t_bounds))
	{
		XYZ bbc1, bbc2;
		XYZ aabbTest,bbChildMin,bbChildMax;
		
		(*children.begin())->t_bounds = true;
		(*children.begin())->clearWorldTransform();
		(*children.begin())->calcAABB();

		bbChildMin = (*children.begin())->aabbMin;
		bbChildMax = (*children.begin())->aabbMax;
		
		for (children_i = children.begin(); children_i != children.end(); children_i++)
		{
			(*children_i)->t_bounds = true;
			(*children_i)->clearWorldTransform();
			(*children_i)->calcAABB();

			bbc1 = (*children_i)->aabbMin;
			bbc2 = (*children_i)->aabbMax;
			
			if (bbc1.x < bbChildMin.x) bbChildMin.x = bbc1.x;
			if (bbc1.y < bbChildMin.y) bbChildMin.y = bbc1.y;
			if (bbc1.z < bbChildMin.z) bbChildMin.z = bbc1.z;
			if (bbc2.x > bbChildMax.x) bbChildMax.x = bbc2.x;
			if (bbc2.y > bbChildMax.y) bbChildMax.y = bbc2.y;
			if (bbc2.z > bbChildMax.z) bbChildMax.z = bbc2.z;
		}	
		
		if (aabbMin==aabbMax)
		{
			aabbMin = bbChildMin;
			aabbMax = bbChildMax;
		}
		else
		{
			if (bbChildMin.x < aabbMin.x) aabbMin.x = bbChildMin.x;
			if (bbChildMin.y < aabbMin.y) aabbMin.y = bbChildMin.y;
			if (bbChildMin.z < aabbMin.z) aabbMin.z = bbChildMin.z;

			if (bbChildMax.x > aabbMax.x) aabbMax.x = bbChildMax.x;
			if (bbChildMax.y > aabbMax.y) aabbMax.y = bbChildMax.y;
			if (bbChildMax.z > aabbMax.z) aabbMax.z = bbChildMax.z;
		}
	}
	
	if (!t_moved && aabbMin_last != aabbMin && aabbMax_last != aabbMax)
	{
		t_moved = true;

		aabbMin_last = aabbMin_tmp;
		aabbMax_last = aabbMax_tmp;
	}
		
	t_bounds = false;
	
	return true;
};


void SceneObject::calcVisibility(const XYZ & camPosition, FRUSTUM &frustum)
{
	//	nil
}


#ifndef ARCH_PSP
	void SceneObject::buildVertexLightMap()
	{
		if (dynamic_colors) return;
		if (!obj) return;
		if (!obj->cache_state) return;

		dynamic_colors = new va_rgb[obj->cache_data.vertex_count];
	}

	/*

#define sh4_dot_single3(x, y, z, a, b, c, d) { \
	register float __x __asm__("fr0") = (x); \
	register float __y __asm__("fr1") = (y); \
	register float __z __asm__("fr2") = (z); \
	register float __a __asm__("fr4") = (a); \
	register float __b __asm__("fr5") = (b); \
	register float __c __asm__("fr6") = (c); \
	register float __d __asm__("fr7") = (d); \
	__asm__ __volatile__( \
		"fldi1	fr3\n" \
		"fldi1	fr7\n" \
		"fipr	fv0,fv4\n" \
		"fmov	fr7,fr0\n" \
		: "=f" (__x), "=f" (__y), "=f" (__z), "=f" (__a), "=f" (__b), "=f" (__c), "=f" (__d) \
		: "0" (__x), "1" (__y), "2" (__z), "3" (__a), "4" (__b), "5" (__c), "6" (__d) \
		: "fr3" ); \
	d = __d;  \
}
	*/
#ifdef ARCH_DC

#include <dc/matrix3d.h>

#define sh4_magnitude(x, y, z, d) { \
	register float __x __asm__("fr0") = (x); \
	register float __y __asm__("fr1") = (y); \
	register float __z __asm__("fr2") = (z); \
	register float __d __asm__("fr3") = (d); \
	__asm__ __volatile__( \
		"fldi1	fr3\n" \
		"fipr	fv0,fv0\n" \
		"fsqrt	fr3\n" \
		: "=f" (__x), "=f" (__y), "=f" (__z), "=f" (__d) \
		: "0" (__x), "1" (__y), "2" (__z), "3" (__d) \
		:  ); \
	d = __d;  \
}

// d = sqrt((x-a)*(x-a),(y-b)*(y-b),(z-c)*(z-c))

#define sh4_magnitude2(a, b, c, x, y, z, d) { \
	register float __x __asm__("fr0") = (x); \
	register float __y __asm__("fr1") = (y); \
	register float __z __asm__("fr2") = (z); \
	register float __d __asm__("fr3") = (d); \
	register float __a __asm__("fr4") = (a); \
	register float __b __asm__("fr5") = (b); \
	register float __c __asm__("fr6") = (c); \
	__asm__ __volatile__( \
		"fldi1	fr3\n" \
		"fsub	fr4,fr0\n"\
		"fsub	fr5,fr1\n"\
		"fsub	fr6,fr2\n"\
		"fipr	fv0,fv0\n" \
		"fsqrt	fr3\n" \
		: "=f" (__x), "=f" (__y), "=f" (__z), "=f" (__a), "=f" (__b), "=f" (__c), "=f" (__d) \
		: "0" (__x), "1" (__y), "2" (__z), "3" (__a), "4" (__b), "5" (__c), "6" (__d) \
		:  ); \
	d = __d;  \
}

	/*
#define sh4_magnitude(a, b, c, x, y, z, l, m, n, d) { \
	register float __x __asm__("fr0") = (x); \
	register float __y __asm__("fr1") = (y); \
	register float __z __asm__("fr2") = (z); \
	register float __d __asm__("fr3") = (d); \
	register float __a __asm__("fr4") = (a); \
	register float __b __asm__("fr5") = (b); \
	register float __c __asm__("fr6") = (c); \
	__asm__ __volatile__( \
		"fldi1	fr3\n" \
		"fsub	fr4,fr0\n"\
		"fsub	fr5,fr1\n"\
		"fsub	fr6,fr2\n"\
		"fipr	fv0,fv0\n" \
		"fsqrt	fr3" \
		: "=f" (__x), "=f" (__y), "=f" (__z), "=f" (__a), "=f" (__b), "=f" (__c), "=f" (__d) \
		: "0" (__x), "1" (__y), "2" (__z), "0" (__a), "1" (__b), "2" (__c), "3" (__d) \
		:  ); \
	d = __d;  \
	l = __x;  \
	m = __y;  \
	n = __z;  \
}
*/

#define sh4_magnitude_unit(a, b, c, x, y, z, l, m, n, d) { \
	register float __x __asm__("fr0") = (x); \
	register float __y __asm__("fr1") = (y); \
	register float __z __asm__("fr2") = (z); \
	register float __d __asm__("fr3") = (d); \
	register float __a __asm__("fr4") = (a); \
	register float __b __asm__("fr5") = (b); \
	register float __c __asm__("fr6") = (c); \
	__asm__ __volatile__( \
		"fldi1	fr3\n" \
		"fsub	fr4,fr0\n"\
		"fsub	fr5,fr1\n"\
		"fsub	fr6,fr2\n"\
		"fipr	fv0,fv0\n" \
		"fsqrt	fr3\n" \
		"fdiv	fr3,fr0\n"\
		"fdiv	fr3,fr1\n"\
		"fdiv	fr3,fr2\n"\
		: "=f" (__x), "=f" (__y), "=f" (__z), "=f" (__d), "=f" (__a), "=f" (__b), "=f" (__c) \
		: "0" (__x), "1" (__y), "2" (__z), "3" (__d), "4" (__a), "5" (__b), "6" (__c) \
		:  ); \
	d = __d;  \
	l = __x;  \
	m = __y;  \
	n = __z;  \
}

#define sh4_getunitvector(a, b, c, x, y, z, l, m, n) { \
	register float __x __asm__("fr0") = (x); \
	register float __y __asm__("fr1") = (y); \
	register float __z __asm__("fr2") = (z); \
	register float __a __asm__("fr4") = (a); \
	register float __b __asm__("fr5") = (b); \
	register float __c __asm__("fr6") = (c); \
	__asm__ __volatile__( \
		"fldi1	fr3\n" \
		"fsub	fr4,fr0\n"\
		"fsub	fr5,fr1\n"\
		"fsub	fr6,fr2\n"\
		"fipr	fv0,fv0\n" \
		"fsqrt	fr3\n" \
		"fdiv	fr3,fr0\n"\
		"fdiv	fr3,fr1\n"\
		"fdiv	fr3,fr2\n"\
		: "=f" (__x), "=f" (__y), "=f" (__z), "=f" (__a), "=f" (__b), "=f" (__c) \
		: "0" (__x), "1" (__y), "2" (__z), "3" (__a), "4" (__b), "5" (__c) \
		: "fr3"); \
	l = __x;  \
	m = __y;  \
	n = __z;  \
}

#define sh4_unitvector(x, y, z) { \
	register float __x __asm__("fr0") = (x); \
	register float __y __asm__("fr1") = (y); \
	register float __z __asm__("fr2") = (z); \
	__asm__ __volatile__( \
		"fldi1	fr3\n" \
		"fipr	fv0,fv0\n" \
		"fsqrt	fr3\n" \
		"fdiv	fr3,fr0\n"\
		"fdiv	fr3,fr1\n"\
		"fdiv	fr3,fr2\n"\
		: "=f" (__x), "=f" (__y), "=f" (__z) \
		: "0" (__x), "1" (__y), "2" (__z) \
		: "fr3" ); \
	x = __x;  \
	y = __y;  \
	z = __z;  \
}


#define sh4_dot_single3_unit(x, y, z, a, b, c, d) { \
	register float __x __asm__("fr0") = (x); \
	register float __y __asm__("fr1") = (y); \
	register float __z __asm__("fr2") = (z); \
	register float __a __asm__("fr4") = (a); \
	register float __b __asm__("fr5") = (b); \
	register float __c __asm__("fr6") = (c); \
	register float __d __asm__("fr7") = (d); \
	__asm__ __volatile__( \
		"fipr	fv0,fv0\n" \
		"fsqrt	fr3\n" \
		"fdiv	fr3,fr0\n"\
		"fdiv	fr3,fr1\n"\
		"fdiv	fr3,fr2\n"\
		"fipr	fv4,fv4\n" \
		"fsqrt	fr7\n" \
		"fdiv	fr7,fr4\n"\
		"fdiv	fr7,fr5\n"\
		"fdiv	fr7,fr6\n"\
		"fldi1	fr3\n" \
		"fldi1	fr7\n" \
		"fipr	fv0,fv4\n" \
		: "=f" (__x), "=f" (__y), "=f" (__z), "=f" (__a), "=f" (__b), "=f" (__c), "=f" (__d) \
		: "0" (__x), "1" (__y), "2" (__z), "3" (__a), "4" (__b), "5" (__c), "6" (__d) \
		: "fr3" ); \
	d = __d;  \
}
#endif

	void locTransform(float *mat, va_xyz *v_in, va_xyz *v_out)
	{

		v_out->x = 
		mat[0]*v_in->x+ 
		mat[4]*v_in->y+ 
		mat[8]*v_in->z+
		mat[12]*1.0f;
		
		v_out->y = 
		mat[1]*v_in->x+
		mat[5]*v_in->y+
		mat[9]*v_in->z+
		mat[13]*1.0f;
		
		v_out->z = 
		mat[2]*v_in->x+
		mat[6]*v_in->y+
		mat[10]*v_in->z+
		mat[14]*1.0f;

	}


	void SceneObject::calcVertexLightMap(Renderer &renderer)
	{
		// todo: patch this up to work on non-SH4 again
#ifdef ARCH_DC
		if (!obj) return;
		if (!obj->cache_state) return;

		bool first_pass = true;
//		float tmp[16];

		//float (*sa)[NumTimeSteps] = reinterpret_cast<float(*)[NumTimeSteps]>(thatpointer); 

//		matrix_t tmp_matrix;

		/*
		memcpy(tmp_matrix[0],&t_trans,sizeof(float)*4);
		memcpy(tmp_matrix[1],&t_trans+4,sizeof(float)*4);
		memcpy(tmp_matrix[2],&t_trans+8,sizeof(float)*4);
		memcpy(tmp_matrix[3],&t_trans+12,sizeof(float)*4);
*/
	//	mat_load(&tmp_matrix);

		va_xyz *cache_normals = (va_xyz *)obj->cache_data.data+obj->normalCacheOffset;
//		va_rgb *cache_colors = (va_rgb *)obj->cache_data.data+obj->colorMapOffset;
		va_xyz *cache_points = (va_xyz *)obj->cache_data.data+obj->pointCacheOffset;

		if (renderer.lights)
		{
			glPushMatrix();
//			glLoadIdentity();
			
			memcpy(ml,w_trans,sizeof(float)*16);

			mat_load(&ml);

			//sh4_loadmatrix(t_trans);
			//glMultMatrixf(t_trans);

			Light *light;
			va_xyz trans_pt;
			va_xyz trans_normal;
#ifdef ARCH_DC
			va_xyz lightDir,norm;
#else
			Vector lightDir,norm;
#endif
			va_xyz tmp_normal;
			va_xyz lightPos;
			float w = 1;

			float dist=0, ang=0;
			bool pos_calc;
			set<Light *>::iterator lights_i;
			
			lights_i = (*renderer.lights).begin();

			for (unsigned int j = 0; j < renderer.lights->size(); j++)
			{
				light = (*lights_i);

				lightPos.x = light->position.x;
				lightPos.y = light->position.y;
				lightPos.z = light->position.z;

				pos_calc = false;

				for (unsigned int i = 0; i < obj->cache_data.vertex_count; i++)
				{
					#ifdef ARCH_DC
						trans_pt = cache_points[i];
						w=1;
						mat_trans_nodiv(trans_pt.x,trans_pt.y,trans_pt.z,w);
					#else
						locTransform(t_trans,&cache_points[i],&trans_pt);
					#endif


#ifdef ARCH_DC
					sh4_magnitude_unit(lightPos.x,lightPos.y,lightPos.z,trans_pt.x,trans_pt.y,trans_pt.z,lightDir.x,lightDir.y,lightDir.z,dist);
	//				sh4_magnitude(dx,dy,dz,dist);
#else
					dx = trans_pt.x-lightPos.x;
					dy = trans_pt.y-lightPos.y;
					dz = trans_pt.z-lightPos.z;

					dist = sqrt(dx*dx+dy*dy+dz*dz);
#endif

					if (light->cutoff < dist) 
					{
						if (first_pass)
						{
							dynamic_colors[i].r = 0;//((float)rand()/(float)RAND_MAX);
							dynamic_colors[i].g = 0;//((float)rand()/(float)RAND_MAX);
							dynamic_colors[i].b = 0;//((float)rand()/(float)RAND_MAX);
						}
						continue;
					}

	
#ifdef ARCH_DC
//					sh4_unitvector(lightDir.x,lightDir.y,lightDir.z);
#else
					lightDir.x = dx; 
					lightDir.y = dy;
					lightDir.z = dz;

					lightDir.makeUnit();
#endif

					tmp_normal.x = cache_normals[i].x + cache_points[i].x;
					tmp_normal.y = cache_normals[i].y + cache_points[i].y;
					tmp_normal.z = cache_normals[i].z + cache_points[i].z;

					#ifdef ARCH_DC
						trans_normal = tmp_normal;
						w=1;

						mat_trans_nodiv(trans_normal.x,trans_normal.y,trans_normal.z,w);
					#else
						locTransform(t_trans,&tmp_normal,&trans_normal);
					#endif


#ifdef ARCH_DC
					sh4_getunitvector(trans_pt.x,trans_pt.y,trans_pt.z,trans_normal.x,trans_normal.y,trans_normal.z,norm.x,norm.y,norm.z);
//					sh4_unitvector(norm.x,norm.y,norm.z);
#else
					norm.x = trans_normal.x-trans_pt.x;
					norm.y = trans_normal.y-trans_pt.y;
					norm.z = trans_normal.z-trans_pt.z;
					norm.makeUnit();					
#endif



#ifdef ARCH_DC

					sh4_dot_single3(lightDir.x,lightDir.y,lightDir.z, norm.x, norm.y, norm.z, ang);
					ang = fabs(acos(ang));
#else
					ang = fabs(lightDir.angle(norm));
#endif
					if (light->cutoff < dist || ang > (M_PI/2.0))
					{
						if (first_pass)
						{
							dynamic_colors[i].r = 0;//((float)rand()/(float)RAND_MAX);
							dynamic_colors[i].g = 0;//((float)rand()/(float)RAND_MAX);
							dynamic_colors[i].b = 0;//((float)rand()/(float)RAND_MAX);
						}
					}
					else
					{
						//float angf = ((ang-(M_PI/2.0))/(M_PI/2.0))*2.0;
						float angf = (((M_PI/2.0)-ang)/(M_PI/2.0));
						//float angf = 1.0;
						if (first_pass)
						{
							dynamic_colors[i].r = angf * light->diffuse.r * ((light->cutoff - dist)/light->cutoff); //((float)rand()/(float)RAND_MAX);
							dynamic_colors[i].g = angf * light->diffuse.g * ((light->cutoff - dist)/light->cutoff); //((float)rand()/(float)RAND_MAX);
							dynamic_colors[i].b = angf * light->diffuse.b * ((light->cutoff - dist)/light->cutoff); //((float)rand()/(float)RAND_MAX);
						}
						else
						{
							dynamic_colors[i].r += angf * light->diffuse.r * ((light->cutoff - dist)/light->cutoff); //((float)rand()/(float)RAND_MAX);
							dynamic_colors[i].g += angf * light->diffuse.g * ((light->cutoff - dist)/light->cutoff); //((float)rand()/(float)RAND_MAX);
							dynamic_colors[i].b += angf * light->diffuse.b * ((light->cutoff - dist)/light->cutoff); //((float)rand()/(float)RAND_MAX);
						}

						if (dynamic_colors[i].r > 1.0) dynamic_colors[i].r = 1.0;
						if (dynamic_colors[i].g > 1.0) dynamic_colors[i].g = 1.0;
						if (dynamic_colors[i].b > 1.0) dynamic_colors[i].b = 1.0;
					}
				}
				
				lights_i++;
				first_pass = false;
			}
			glPopMatrix();
		}
#endif
	}
#endif

