
varying vec4 diffuse,ambient;
varying vec3 normal,lightDir,halfVector;


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


#if hasSpecMap
uniform sampler2D specMap;
#endif

#if hasBumpMap
uniform sampler2D bumpMap;
#endif

//#if hasAlphaMap
//uniform sampler2D alphaMap;
//#endif

#if hasColorMap
uniform sampler2D colorMap;
#endif

#if hasLumiMap
uniform sampler2D lumiMap;
#endif

#if hasLightMap
uniform sampler2D lightMap;
#endif

#if hasEnvSphereMap
	uniform sampler2D envSphereMap;
	uniform vec4 projEnv;
	uniform float envAmount;
#if hasBumpMap
	varying vec3 u;
#endif
#endif

#if hasReflectionMap
	uniform sampler2D reflectionMap;
#endif

#if hasShadowMap0|hasShadowMap1|hasShadowMap2|hasShadowMap3|hasShadowMap4|hasShadowMap5|hasShadowMap6|hasShadowMap7
	uniform sampler2D shadowMap0,shadowMap1,shadowMap2;
	varying vec4 ProjShadow0;
	varying vec4 ProjShadow1;
	varying vec4 ProjShadow2;
#endif

#if hasParallaxMap
	uniform sampler2D parallaxMap;
	uniform float parallaxScale;
	uniform float parallaxBias;
#endif
#if hasParallaxMap||hasEnvSphereMap
	varying vec3 eyeVec; 
	varying mat3 TBNMatrix;
#endif


float shadowVal(vec4 shadowProj, sampler2D shadowMap)
{
		vec4 shadowCoordinateWdivide;
		float distanceFromLight;

		shadowCoordinateWdivide = shadowProj / shadowProj.w;
		shadowCoordinateWdivide.w += 0.0005;

		distanceFromLight = texture2D(shadowMap,shadowCoordinateWdivide.st).z;

		return (distanceFromLight < shadowCoordinateWdivide.z && shadowProj.w > 0.00) ? 0.0 : 1.0;
}

void main()
{   
#if hasShadowMap0|hasShadowMap1|hasShadowMap2|hasShadowMap3|hasShadowMap4|hasShadowMap5|hasShadowMap6|hasShadowMap7		
		float shadowv = shadowVal(ProjShadow0, shadowMap0)*shadowVal(ProjShadow1, shadowMap1)*shadowVal(ProjShadow2, shadowMap2);
#endif


	vec3 n,halfV;
	float NdotL,NdotHV;

	vec4 color;
	vec4 specVal;

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
//			n = normalize(normal);
			n = normalize(normal+((bumpNorm-0.5)*2.0));
#else
			n = normalize(normal);
#endif
		
		/* compute the dot product between normal and ldir */

		NdotL = max(dot(n,lightDir),0.0);

		if (NdotL > 0.0) 
		{
#if hasSpecMap
					specVal = texture2D(specMap, texCoord);
#else
					specVal = gl_FrontMaterial.specular;
#endif

		}

		color = diffuse;

#if hasColorMap
		color *= texture2D(colorMap, texCoord);
#endif 

		vec4 litColor = vec4(0.0,0.0,0.0,1.0);

		litColor += NdotL;
//		halfV = normalize(halfVector);
//		NdotHV = max(dot(n,halfV),0.0);
//		color += specVal * gl_LightSource[%LIGHTID%].specular * pow(NdotHV, gl_FrontMaterial.shininess);

		NdotHV = max(dot(n, halfVector.xyz),0.001);

		litColor += specVal * gl_LightSource[%LIGHTID%].specular * pow(NdotHV,gl_FrontMaterial.shininess);		

#if hasShadowMap0|hasShadowMap1|hasShadowMap2|hasShadowMap3|hasShadowMap4|hasShadowMap5|hasShadowMap6|hasShadowMap7		
		litColor *= shadowv;
#endif


#if hasLightMap
		litColor.rgb += (texture2D(lightMap, gl_TexCoord[2].st).rgb*2.0)*texture2D(colorMap, texCoord).rgb;
#endif



#if hasLumiMap
		litColor.rgb += (vec3(1.0,1.0,1.0)-color.rgb) * 2.0 * texture2D(lumiMap, texCoord).rgb;
#endif


	litColor += ambient;

#if hasEnvSphereMap

#if hasBumpMap
	vec3 r = reflect( u, n );
	float m = 2.0 * sqrt( r.x*r.x + r.y*r.y + (r.z+1.0)*(r.z+1.0) );
//
	vec3 coord;
	coord.s = r.x/m + 0.5;
	coord.t = r.y/m + 0.5;
	
	#if hasReflectionMap
		color += litColor * texture2D( envSphereMap, coord.st) * texture2D( reflectionMap, texCoord);
	#else
		color += litColor * texture2D( envSphereMap, coord.st) * envAmount;
	#endif

#else
	#if hasReflectionMap
		color += litColor * texture2D( envSphereMap, gl_TexCoord[1].st) * texture2D( reflectionMap, texCoord);
	#else
		color += litColor * texture2D( envSphereMap, gl_TexCoord[1].st) * envAmount;
	#endif
#endif

#endif
		
		
		color *= litColor;

#if hasAlphaMap
		color.a = texture2D(colorMap, texCoord).a; 
#else
		color.a = gl_FrontLightModelProduct.sceneColor.a;
#endif

	
	gl_FragColor = color;
}
