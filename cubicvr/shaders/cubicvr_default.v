#pragma optimize(on)
varying vec4 ambientGlobal;

#define hasLightMap %hasLightMap%
#define hasParallaxMap %hasParallaxMap%

#if hasParallaxMap
varying vec3 eyeVec; 
#endif


void main()
{
	gl_Position = ftransform();

    gl_TexCoord[0] = gl_MultiTexCoord0;

#if hasLightMap
	gl_TexCoord[2] = gl_MultiTexCoord1;
#endif

#if hasParallaxMap
	vec3 tangent;
	vec3 binormal;

	vec3 c1 = cross( gl_Normal, vec3(0.0, 0.0, 1.0) );
	vec3 c2 = cross( gl_Normal, vec3(0.0, 1.0, 0.0) );

	if( length(c1)>length(c2) )
	{
		tangent = c1;
	}
	else
	{
		tangent = c2;
	}

	tangent = normalize(tangent);

	binormal = cross(gl_Normal, tangent);
	binormal = normalize(binormal);

	mat3 TBNMatrix = mat3(gl_NormalMatrix * tangent,gl_NormalMatrix * binormal,gl_NormalMatrix * gl_Normal);
	eyeVec = vec3(gl_ModelViewMatrix * gl_Vertex) * TBNMatrix;

#endif

	ambientGlobal = gl_LightModel.ambient;

}