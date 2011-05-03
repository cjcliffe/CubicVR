varying vec4 diffuse,ambient;
varying vec3 normal,lightDir;


#define hasSpecMap %hasSpecMap%
#define hasBumpMap %hasBumpMap%
#define hasAlphaMap %hasAlphaMap%
#define hasColorMap %hasColorMap%
#define hasLumiMap %hasLumiMap%
#define hasEnvSphereMap %hasEnvSphereMap%
#define hasReflectionMap %hasReflectionMap%
#define hasLightMap	%hasLightMap%
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


#if hasLightMap
	uniform sampler2D lightMap;
#endif


#if hasParallaxMap
	uniform sampler2D parallaxMap;
	uniform float parallaxScale;
	uniform float parallaxBias;
	varying vec3 eyeVec; 
#endif


//uniform sampler2D spat0;
//uniform sampler2D spat1;
//uniform sampler2D spat2;
//varying vec4 worldPos;

void main()
{
		vec3 n,halfV;
		float NdotL,NdotHV;

		/* The ambient term will always be present */

		vec4 color = ambient;
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

		color += diffuse * NdotL;
//		halfV = normalize(halfVector);
//		NdotHV = max(dot(n,halfV),0.0);
//		color += specVal * gl_LightSource[%LIGHTID%].specular * pow(NdotHV, gl_FrontMaterial.shininess);

		NdotHV = max(dot(n, gl_LightSource[%LIGHTID%].halfVector.xyz),0.0001);

		color += gl_FrontMaterial.specular * gl_LightSource[%LIGHTID%].specular * pow(NdotHV,gl_FrontMaterial.shininess);

		
#if hasEnvSphereMap
	vec4 litColor = color;
#endif




#if hasColorMap
			color *= texture2D(colorMap, texCoord);

//			float spat_scale0,spat_scale1,spat_scale2;
//			
//			spat_scale0 = 2.0;
//			spat_scale1 = 20.5;
//			spat_scale2 = 5.0;
//			
//			if (worldPos.y > 120.0)
//				color *= texture2D(spat1, vec2(texCoord.x*spat_scale1,texCoord.y*spat_scale1));
//			else if (worldPos.y<0.0)
//				color *= texture2D(spat2, vec2(texCoord.x*spat_scale2,texCoord.y*spat_scale2));
//			else
//				color *= texture2D(spat0, vec2(texCoord.x*spat_scale0,texCoord.y*spat_scale0));

#endif


#if hasLumiMap
			color.rgb += (vec3(1.0,1.0,1.0)-color.rgb) * 2.0 * texture2D(lumiMap, texCoord).rgb;
#endif

#if hasLightMap
		color.rgb += (2.0*texture2D(lightMap, gl_TexCoord[2].st).rgb)*texture2D(colorMap, texCoord).rgb;
#endif


#if hasEnvSphereMap

#if hasBumpMap
	vec3 r = reflect( u, n );
	float m = 2.0 * sqrt( r.x*r.x + r.y*r.y + (r.z+1.0)*(r.z+1.0) );
//
	vec3 coord;
	coord.s = r.x/m + 0.5;
	coord.t = r.y/m + 0.5;
	
	#if hasReflectionMap
		color += litColor * texture2D( envSphereMap, coord.st)/2.0 * texture2D( reflectionMap, texCoord);
	#else
		color += litColor * texture2D( envSphereMap, coord.st)/2.0 * envAmount;
	#endif

#else
	#if hasReflectionMap
		color += litColor * texture2D( envSphereMap, gl_TexCoord[1].st)/2.0 * texture2D( reflectionMap, texCoord);
	#else
		color += litColor * texture2D( envSphereMap, gl_TexCoord[1].st)/2.0 * envAmount;
	#endif
#endif

#endif

#if hasAlphaMap
			color.a = texture2D(colorMap, texCoord).a; 
#else
			color.a = gl_FrontLightModelProduct.sceneColor.a;
#endif


		gl_FragColor = color;
}