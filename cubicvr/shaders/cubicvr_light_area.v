varying vec4 diffuse,ambient;
varying vec3 normal,lightDir,halfVector;

#define hasParallaxMap %hasParallaxMap%
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

#if hasEnvSphereMap
	varying vec4 projEnv;
#if hasBumpMap
	varying vec3 u;
#endif
#endif


#if hasShadowMap0|hasShadowMap1|hasShadowMap2|hasShadowMap3|hasShadowMap4|hasShadowMap5|hasShadowMap6|hasShadowMap7
	varying vec4 ProjShadow0;
	varying vec4 ProjShadow1;
	varying vec4 ProjShadow2;
#endif

#if hasParallaxMap||hasEnvSphereMap
varying vec3 eyeVec; 
varying mat3 TBNMatrix;
#endif


uniform mat4 objMatrix;
uniform vec3 camPos;


void main()
{
	/* first transform the normal into eye space and normalize the result */
	normal = normalize(gl_NormalMatrix * gl_Normal);

//	lightDist = length(gl_Vertex.xyz-gl_LightSource[%LIGHTID%].position.xyz);

	lightDir = normalize(vec3(gl_LightSource[%LIGHTID%].position));

	/* Normalize the halfVector to pass it to the fragment shader */
	halfVector = normalize(gl_LightSource[%LIGHTID%].halfVector.xyz);
                       
	/* Compute the diffuse, ambient and globalAmbient terms */
	diffuse = gl_FrontMaterial.diffuse * gl_LightSource[%LIGHTID%].diffuse;
	ambient = gl_FrontMaterial.ambient * gl_LightSource[%LIGHTID%].ambient;
	ambient += gl_LightModel.ambient * gl_FrontMaterial.ambient;
	
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

#if hasShadowMap0|hasShadowMap1|hasShadowMap2|hasShadowMap3|hasShadowMap4|hasShadowMap5|hasShadowMap6|hasShadowMap7
	vec4 wpos = (objMatrix*gl_Vertex);
	ProjShadow0 = gl_TextureMatrix[%LIGHTID%] * wpos;
	ProjShadow1 = gl_TextureMatrix[%LIGHTID%+1] * wpos;
	ProjShadow2 = gl_TextureMatrix[%LIGHTID%+2] * wpos;
#endif

    gl_TexCoord[0] = gl_MultiTexCoord0;

#if hasLightMap
	gl_TexCoord[2] = gl_MultiTexCoord1;
#endif


#if hasParallaxMap||hasEnvSphereMap
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

	TBNMatrix = mat3(gl_NormalMatrix * tangent,gl_NormalMatrix * binormal,gl_NormalMatrix * gl_Normal);
	eyeVec = vec3(gl_ModelViewMatrix * gl_Vertex) * TBNMatrix;
#endif


#if hasEnvSphereMap
//	projEnv = gl_TextureMatrix[0] * gl_Vertex;

#if hasBumpMap

	u = normalize( vec3(gl_ModelViewMatrix * gl_Vertex) );

#else
	vec4 position = objMatrix * gl_Vertex;
	vec3 u = normalize(position.xyz-camPos);
	vec3 n = normalize((objMatrix * vec4(gl_Normal.xyz+gl_Vertex.xyz,1.0)).xyz - position.xyz); 
	vec3 r = reflect( u, n );

	float m = 2.0 * sqrt( r.x*r.x + r.y*r.y + (r.z+1.0)*(r.z+1.0) );
	gl_TexCoord[1].s = r.x/m + 0.5;
	gl_TexCoord[1].t = r.y/m + 0.5;

#endif	
#endif




}