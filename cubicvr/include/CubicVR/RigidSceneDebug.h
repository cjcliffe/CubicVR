/*
 * Adapted from bullet 2.69 samples
 */
 
#ifndef RIGIDSCENEDEBUG_H
#define RIGIDSCENEDEBUG_H

#include "LinearMath/btIDebugDraw.h"
#include <CubicVR/cvr_defines.h>
#include <CubicVR/GLExt.h>


class RigidSceneDebug : public btIDebugDraw
{
private:
	int m_debugMode;

public:

	RigidSceneDebug();

	virtual void	drawLine(const btVector3& from,const btVector3& to,const btVector3& color);
	
	virtual void	drawTriangle(const btVector3& a,const btVector3& b,const btVector3& c,const btVector3& color,btScalar alpha);
	
	virtual void	drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);

	virtual void	reportErrorWarning(const char* warningString);

	virtual void	draw3dText(const btVector3& location,const char* textString);

	virtual void	setDebugMode(int debugMode);

	virtual int		getDebugMode() const { return m_debugMode;}

};

#endif