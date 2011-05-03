#pragma optimize(on)

varying vec3 normal;
varying vec4 position;
varying vec3 g_norm;
//varying vec4 g_pos;

#define hasSpecMap %hasSpecMap%
#define hasBumpMap %hasBumpMap%
#define hasAlphaMap %hasAlphaMap%
#define hasColorMap %hasColorMap%
#define hasLumiMap %hasLumiMap%
#define hasShadowMap0 %hasShadowMap0%
#define hasShadowMap1 %hasShadowMap1%
#define hasShadowMap2 %hasShadowMap2%
#define hasShadowMap3 %hasShadowMap3%
#define hasShadowMap4 %hasShadowMap4%
#define hasShadowMap5 %hasShadowMap5%
#define hasShadowMap6 %hasShadowMap6%
#define hasShadowMap7 %hasShadowMap7%
#define hasEnvSphereMap %hasEnvSphereMap%
#define hasReflectionMap %hasReflectionMap%
#define hasLightMap %hasLightMap%
#define hasParallaxMap %hasParallaxMap%

#if hasBumpMap
uniform sampler2D bumpMap;
#endif

#if hasSpecMap
uniform sampler2D specMap;
#endif

#if hasLightMap
uniform sampler2D lightMap;
#endif

#if hasColorMap
uniform sampler2D colorMap;
#endif

//#if hasAlphaMap
//uniform sampler2D alphaMap;
//#endif

vec4 color = gl_FrontMaterial.diffuse;
uniform mat4 objMatrix;



#if hasParallaxMap
	uniform sampler2D parallaxMap;
	uniform float parallaxScale;
	uniform float parallaxBias;
	varying vec3 eyeVec; 
#endif

#if hasEnvSphereMap
	uniform sampler2D envSphereMap;
	uniform vec4 projEnv;
	uniform float envAmount;
#if hasBumpMap
	varying vec3 u;
#endif
#endif
	varying vec3 v_n;


#if hasReflectionMap
	uniform sampler2D reflectionMap;
#endif


uniform float nearPlane;
uniform float farPlane;

float ConvertDepth3(float d)
{
  return (nearPlane*farPlane)/(farPlane-d*(farPlane-nearPlane));
}

// transform range in world-z to 0-1 for near-far
float DepthRange( float d )
{
  return ( d - nearPlane ) / ( farPlane - nearPlane );
}


void main() 
{	   	
	vec3 n;

	vec3 view_norm;
	
#if hasParallaxMap
	  float height = texture2D(parallaxMap, gl_TexCoord[0].xy).r;  
	  float v = (height) * parallaxScale - parallaxBias; 
	  vec3 eye = normalize(eyeVec); 
	  vec2 texCoord = gl_TexCoord[0].xy + (eye.xy * v);					 
#else
	  vec2 texCoord = gl_TexCoord[0].xy;
#endif


#if hasBumpMap
		vec3 bumpNorm = vec3(texture2D(bumpMap, texCoord));
//		bumpNorm = (objMatrix * vec4(bumpNorm,1)).xyz;
#if hasEnvSphereMap
#if hasBumpMap
		view_norm = normalize(v_n+((bumpNorm-0.5)*2.0));
#endif
#endif

		bumpNorm = (objMatrix * vec4(((bumpNorm-0.5)*2.0)+(g_norm),1)).xyz; 
		bumpNorm -= position.xyz;
	
		n = normalize(normal+bumpNorm);
#else
		n = normalize(normal);
		view_norm = normalize(v_n);
#endif

#if hasLightMap
	color.rgb = texture2D(lightMap, gl_TexCoord[2].st).rgb*2.0;
#endif

#if hasColorMap
	color *= texture2D(colorMap, texCoord);
#endif

//#if hasAlphaMap
//		color.a = texture2D(colorMap, texCoord).a; 
//#else
//		color.a = gl_FrontLightModelProduct.sceneColor.a;
//#endif


#if hasEnvSphereMap

#if hasBumpMap
	vec3 r = reflect( u, view_norm );
	float m = 2.0 * sqrt( r.x*r.x + r.y*r.y + (r.z+1.0)*(r.z+1.0) );

	vec3 coord;
	coord.s = r.x/m + 0.5;
	coord.t = r.y/m + 0.5;
	
	#if hasReflectionMap
		color += texture2D( envSphereMap, coord.st) * texture2D( reflectionMap, texCoord);
	#else
		color += texture2D( envSphereMap, coord.st) * envAmount;
	#endif

#else
	#if hasReflectionMap
		color += texture2D( envSphereMap, gl_TexCoord[1].st) * texture2D( reflectionMap, texCoord);
	#else
		color += texture2D( envSphereMap, gl_TexCoord[1].st) * envAmount;
	#endif
#endif

#endif

		vec4 specVal;
	
#if hasSpecMap
			specVal = texture2D(specMap, texCoord);
#else
			specVal = gl_FrontMaterial.specular;
#endif

	float linear_depth = DepthRange( ConvertDepth3( gl_FragCoord.z ));

	// RGB
	gl_FragData[0] = color;
		
	// Normal + Specular
	gl_FragData[1] = vec4(n,specVal);
	
	// World Position + Depth
	gl_FragData[2] = vec4(position.xyz,gl_FragCoord.z);

	// Lighting Info + Linear depth (reserved)
	gl_FragData[3] = vec4(0.0,0.0,gl_FrontMaterial.shininess,linear_depth);
}

