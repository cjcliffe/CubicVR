varying vec4 diffuse,ambientGlobal, ambient;
varying vec3 normal,lightDir,halfVector;
varying float dist;

#define hasSpecMap %hasSpecMap%
#define hasBumpMap %hasBumpMap%
#define hasAlphaMap %hasAlphaMap%
#define hasColorMap %hasColorMap%
#define hasLumiMap %hasLumiMap%
#define hasEnvSphereMap %hasEnvSphereMap%
#define hasReflectionMap %hasReflectionMap%
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


#if hasParallaxMap
	uniform sampler2D parallaxMap;
	uniform float parallaxScale;
	uniform float parallaxBias;
	varying vec3 eyeVec; 
#endif


void main()
{
	vec3 n,halfV,viewV,ldir;
	float NdotL,NdotHV;
	vec4 color = ambientGlobal;


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
		n = normalize(normal+((bumpNorm-0.5)*2.0));
#else
		n = normalize(normal);
#endif

	/* compute the dot product between normal and normalized lightdir */
	NdotL = max(dot(n,normalize(lightDir)),0.0);

	float att;
	float distSqr;

	if (NdotL > 0.0) {

		if (gl_LightSource[%LIGHTID%].spotExponent > 0.0)
		{		
			distSqr = dot(lightDir, lightDir);
			att = clamp(1.0 - gl_LightSource[%LIGHTID%].spotExponent * sqrt(distSqr), 0.0, 1.0);
//			vec3 L = lightDir * inversesqrt(distSqr);
		}
		else
		{
			att = 1.0 / (gl_LightSource[%LIGHTID%].constantAttenuation +
					gl_LightSource[%LIGHTID%].linearAttenuation * dist +
					gl_LightSource[%LIGHTID%].quadraticAttenuation * dist * dist);
		}

			
		color += att * (diffuse * NdotL + ambient);

	
		vec4 specVal;
	
#if hasSpecMap
			specVal = texture2D(specMap, texCoord);
#else
			specVal = gl_FrontMaterial.specular;
#endif
		
		halfV = normalize(halfVector);
		NdotHV = max(dot(n,halfV),0.0);
		color += att * specVal * gl_LightSource[%LIGHTID%].specular * 
						pow(NdotHV,gl_FrontMaterial.shininess);
	}


#if hasEnvSphereMap
	vec4 litColor = color;
#endif


#if hasColorMap
		color *= texture2D(colorMap, texCoord);
#endif


#if hasLumiMap
			color.rgb += (vec3(1.0,1.0,1.0)-color.rgb) * 2.0 * texture2D(lumiMap, texCoord).rgb;
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


#if hasAlphaMap
		color.a = texture2D(colorMap, texCoord).a; 
#else
		color.a = gl_FrontLightModelProduct.sceneColor.a;
#endif

	gl_FragColor = color;
}
