varying vec3 position;
varying vec3 normal;

#define hasBumpMap %hasBumpMap%
#define hasShadowMap0 %hasShadowMap0%
#define hasShadowMap1 %hasShadowMap1%
#define hasShadowMap2 %hasShadowMap2%
#define hasShadowMap3 %hasShadowMap3%
#define hasShadowMap4 %hasShadowMap4%
#define hasShadowMap5 %hasShadowMap5%
#define hasShadowMap6 %hasShadowMap6%
#define hasShadowMap7 %hasShadowMap7%
#define hasEnvSphereMap %hasEnvSphereMap%
#define hasLightMap %hasLightMap%
#define hasParallaxMap %hasParallaxMap%

#if hasEnvSphereMap
	varying vec4 projEnv;
#if hasBumpMap
	varying vec3 u;
#endif
#endif

#if hasParallaxMap
varying vec3 eyeVec; 
#endif

#if hasShadowMap0|hasShadowMap1|hasShadowMap2|hasShadowMap3|hasShadowMap4|hasShadowMap5|hasShadowMap6|hasShadowMap7
	varying vec4 ProjShadow0;
#endif

uniform mat4 objMatrix;

void main()
{
    vec4 eyeCoordPos = gl_ModelViewMatrix * gl_Vertex;

    position = eyeCoordPos.xyz / eyeCoordPos.w;
    normal = normalize(gl_NormalMatrix * gl_Normal);


    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

#if hasShadowMap0|hasShadowMap1|hasShadowMap2|hasShadowMap3|hasShadowMap4|hasShadowMap5|hasShadowMap6|hasShadowMap7
	ProjShadow0 = gl_TextureMatrix[%LIGHTID%] * (objMatrix*gl_Vertex);
#endif

    gl_TexCoord[0] = gl_MultiTexCoord0;

#if hasLightMap
	gl_TexCoord[2] = gl_MultiTexCoord1;
#endif

#if hasEnvSphereMap
	//projEnv = gl_TextureMatrix[0] * gl_Vertex;

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