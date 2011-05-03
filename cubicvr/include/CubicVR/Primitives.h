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



#ifndef CUBICVR_PRIMITIVES
#define CUBICVR_PRIMITIVES

#include <CubicVR/cvr_defines.h>
#include <CubicVR/Scene.h>
#include <CubicVR/SceneObject.h>
#include <CubicVR/UVMapper.h>


class IMPEXP Primitive: public SceneObject
{
	public:

		Primitive();

		void createSolidCube(Scene &scene, int size);

	/*
	unsigned int uiMaterial, fontMaterial;
	int consoleWidth, consoleHeight;
	
	vector<string> consoleData;
	vector<SceneObject *> uiFontLetters;
	vector<Object *> uiFontLetterObjs;
	
	Console(int consoleWidth_in, int consoleHeight_in, unsigned int fontMaterial_in, unsigned int uiMaterial_in = 0);

	void makeLetter(Object &obj, unsigned short charnum, unsigned int fontMaterial);

	void writeLn(string console_str = "");
	void write(string console_str = "");
	void refreshConsole();

	void fxWave(float timeVal);

	*/	
	private:
		int size;

};

#endif