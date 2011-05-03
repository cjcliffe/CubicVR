varying vec4 diffuse,ambient;
varying vec3 normal,lightDir;

#define hasParallaxMap %hasParallaxMap%

#define hasEnvSphereMap %hasEnvSphereMap%
#define hasBumpMap %hasBumpMap%
#define hasLightMap %hasLightMap%

#if hasEnvSphereMap
	varying vec4 projEnv;
#if hasBumpMap
	varying vec3 u;
#endif
#endif


#if hasParallaxMap
varying vec3 eyeVec; 
#endif

//uniform mat4 objMatrix;
//varying vec4 worldPos;

void main()
{  
//	worldPos = objMatrix * gl_Vertex;

	/* first transform the normal into eye space and normalize the result */
	normal = normalize(gl_NormalMatrix * gl_Normal);

	lightDir = normalize(vec3(gl_LightSource[%LIGHTID%].position));

	/* Normalize the halfVector to pass it to the fragment shader */
//	halfVector = normalize(gl_LightSource[%LIGHTID%].halfVector.xyz);
          
	/* Compute the diffuse, ambient and globalAmbient terms */
	diffuse = gl_FrontMaterial.diffuse * gl_LightSource[%LIGHTID%].diffuse;
	ambient = gl_FrontMaterial.ambient * gl_LightSource[%LIGHTID%].ambient;
	ambient += gl_LightModel.ambient * gl_FrontMaterial.ambient;

    gl_TexCoord[0] = gl_MultiTexCoord0;
#if hasLightMap
	gl_TexCoord[2] = gl_MultiTexCoord1;
#endif

#if hasEnvSphereMap
#if hasBumpMap

	u = normalize( vec3(gl_ModelViewMatrix * gl_Vertex) );

#else

	vec3 u = normalize( vec3(gl_ModelViewMatrix * gl_Vertex) );
	vec3 n = normalize( gl_NormalMatrix * gl_Normal );
	vec3 r = reflect( u, n );
	float m = 2.0 * sqrt( r.x*r.x + r.y*r.y + (r.z+1.0)*(r.z+1.0) );
	gl_TexCoord[1].s = r.x/m + 0.5;
	gl_TexCoord[1].t = r.y/m + 0.5;

#endif
#endif


gl_Position = ftransform();

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

	
}