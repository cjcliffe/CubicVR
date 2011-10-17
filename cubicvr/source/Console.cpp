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

#include <CubicVR/Console.h>

Console::Console(unsigned int consoleWidth_in, unsigned int consoleHeight_in, Material *fontMaterial_in, Material *uiMaterial_in) : consoleWidth(consoleWidth_in),consoleHeight(consoleHeight_in)
{
	fontMaterial = fontMaterial_in;
	uiMaterial = uiMaterial_in;
	
	if (uiMaterial != 0)
	{
		obj = new Mesh();
		
		obj->addFace();
		obj->bindFaceMaterial(uiMaterial);
		
		obj->addFacePoint(XYZ(-0.5f,0,0.5f));
		obj->addFacePoint(XYZ(0.5f,0,0.5f));
		obj->addFacePoint(XYZ(0.5f,0,-0.5f));
		obj->addFacePoint(XYZ(-0.5f,0,-0.5f));

		obj->calcNormals();


		UVMapper uiMap;
		uiMap.setProjection(UV_PROJECTION_CUBIC);
		uiMap.setScale(XYZ(1,1,1));
		uiMap.apply(*obj,uiMaterial);

		obj->cache(true);
	}
	
	unsigned int i,j;
	
	uiFontLetterObjs.reserve(256);
	
	for (i = 0; i < 256; i++)
	{
		uiFontLetterObjs[i] = new Mesh();
		makeLetter(*uiFontLetterObjs[i],i,fontMaterial);
	}

	uiFontLetters.reserve(consoleWidth*consoleHeight);
	
	for (i = 0; i < consoleHeight; i++)
	{
		for (j = 0; j < consoleWidth; j++)
		{
			SceneObject *tmpObj;
			
			tmpObj = new SceneObject();
			
			tmpObj->getScale() = XYZ(1.0f/consoleWidth,1,1.0f/consoleHeight);
			tmpObj->getPosition().z = ((1.0f/consoleHeight)/2.0f)-0.5f+((1.0f/consoleHeight)*i);
			tmpObj->getPosition().x = ((1.0f/consoleWidth)/2.0f)-0.5f+((1.0f/consoleWidth)*j);
			tmpObj->clearTransform();
			
			bindChild(*tmpObj);
			uiFontLetters[i*consoleWidth+j] = tmpObj;
		}
	}
	
}

void Console::fxWave(float timeVal)
{
	float q = timeVal*45.0f;

	for (unsigned int i = 0; i < consoleWidth*consoleHeight; i++)
	{
		XYZ tmpPos;
		XYZ tmpRot;
		
		tmpPos = uiFontLetters[i]->getPosition();
		tmpRot = uiFontLetters[i]->getRotation();

		tmpPos.y = 0.02f + 0.01f *sin(sqrt(tmpPos.x*tmpPos.x+tmpPos.z*tmpPos.z)*24.0f+(q/20.0f));

		tmpRot.x = tmpRot.y = 5.0f*sin(sqrt(tmpPos.x*tmpPos.x+tmpPos.y*tmpPos.y)*24.0f);

		uiFontLetters[i]->setPosition(tmpPos);
		uiFontLetters[i]->setRotation(tmpRot);
	}
}


void Console::writeLn(string console_str)
{
	if (console_str == "") console_str = " ";
	consoleData.push_back(console_str);
	refreshConsole();
}


void Console::write(string console_str)
{
	if (console_str == "") console_str = " ";

	if (!consoleData.size())
	{
		consoleData.push_back(console_str);
	}
	else
	{
		consoleData[consoleData.size()-1] = console_str;
	}

	refreshConsole();
}

void Console::refreshConsole()
{
	
	unsigned int i,j;
	
	unsigned int icount = 0;
	
	for (i = consoleData.size()-1; i >= 0; i--)
	{
		icount++;
	
		if (icount > consoleHeight) break;
		
		for (j = 0; j < consoleWidth; j++)
		{
			if ((consoleData[i].length()-1)<j)
			{
				uiFontLetters[(consoleHeight-icount)*consoleWidth+j]->obj = NULL;
			}
			else
			{
				if (consoleData[i].c_str()[j] == 32)
				{
					uiFontLetters[(consoleHeight-icount)*consoleWidth+j]->obj = NULL;
				}
				else
				{
					uiFontLetters[(consoleHeight-icount)*consoleWidth+j]->bind(*uiFontLetterObjs[consoleData[i].c_str()[j]]);
				}
			}
		}
	}

}

void Console::makeLetter(Mesh &obj, unsigned short charnum, Material *fontMaterial)
{
	cvrFloat u_step = 1.0f/16.0f;	// 16 x 16 font grid
	cvrFloat v_step = 1.0f/16.0f;
	cvrFloat u_left, v_top;
	
	obj.addFace();
	obj.bindFaceMaterial(fontMaterial);
	
	obj.addFacePoint(XYZ(-0.5f,0,0.5f));
	obj.addFacePoint(XYZ(0.5f,0,0.5f));
	obj.addFacePoint(XYZ(0.5f,0,-0.5f));
	obj.addFacePoint(XYZ(-0.5f,0,-0.5f));
	
	v_top = (15-(int)(charnum / 16))*v_step;
	u_left = ((int)(charnum % 16))*u_step;
	
	obj.faces[0]->setUV(0,UV(u_left,v_top));
	obj.faces[0]->setUV(1,UV(u_left+u_step,v_top));
	obj.faces[0]->setUV(2,UV(u_left+u_step,v_top+v_step));
	obj.faces[0]->setUV(3,UV(u_left,v_top+v_step));

//	obj.cache(true);
}