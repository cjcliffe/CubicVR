
varying vec3 position;
varying vec3 normal;

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
	uniform sampler2DShadow shadowMap0;
	varying vec4 ProjShadow0;
#endif


#if hasParallaxMap
	uniform sampler2D parallaxMap;
	uniform float parallaxScale;
	uniform float parallaxBias;
	varying vec3 eyeVec; 
#endif


void main()
{   
#if hasParallaxMap
	  float height = texture2D(parallaxMap, gl_TexCoord[0].xy).r;  
	  float eyev = (height) * parallaxScale - parallaxBias; 
	  vec3 eye = normalize(eyeVec); 
	  vec2 texCoord = gl_TexCoord[0].xy + (eye.xy * eyev);					 
#else
	  vec2 texCoord = gl_TexCoord[0].xy;
#endif


    vec3 l = gl_LightSource[%LIGHTID%].position.xyz - position;
    float d = length(l);
    float atten = 1.0 / (gl_LightSource[%LIGHTID%].constantAttenuation +
                         gl_LightSource[%LIGHTID%].linearAttenuation * d +
                         gl_LightSource[%LIGHTID%].quadraticAttenuation * d * d);
    
    l = normalize(l);

    float spotDot = dot(-l, normalize(gl_LightSource[%LIGHTID%].spotDirection));

    float spotEffect = (spotDot < gl_LightSource[%LIGHTID%].spotCosCutoff)
                        ? 0.0 : pow(spotDot, gl_LightSource[%LIGHTID%].spotExponent);

    atten *= spotEffect;

    vec3 n;

#if hasBumpMap
		vec3 bumpNorm = vec3(texture2D(bumpMap, texCoord));
		n = normalize(normal+((bumpNorm-0.5)*2.0));
#else
		n = normalize(normal);
#endif

    vec3 v = normalize(-position);
    vec3 h = normalize(l + v);
    
    float nDotL = max(0.0, dot(n, l));
    float nDotH = max(0.0, dot(n, h));
    float power = (nDotL == 0.0) ? 0.0 : pow(nDotH, gl_FrontMaterial.shininess);
    
    vec4 diffuse = gl_FrontLightProduct[%LIGHTID%].diffuse * nDotL * atten;

	vec4 specVal;
	
#if hasSpecMap
			specVal = texture2D(specMap, texCoord);
#else
			specVal = gl_FrontMaterial.specular;
#endif

    vec4 specular = specVal * gl_FrontLightProduct[%LIGHTID%].specular * power * atten;

    vec4 ambient = gl_FrontLightProduct[%LIGHTID%].ambient + gl_LightSource[%LIGHTID%].ambient;
    vec4 color = gl_FrontLightModelProduct.sceneColor + diffuse + specular;

#if hasEnvSphereMap|hasLightMap
	vec4 litColor = color;
#endif



#if hasShadowMap0|hasShadowMap1|hasShadowMap2|hasShadowMap3|hasShadowMap4|hasShadowMap5|hasShadowMap6|hasShadowMap7
		color *= clamp(shadow2DProj(shadowMap0, ProjShadow0),0.0,1.0);
#endif

		color += ambient*gl_FrontMaterial.diffuse;

#if hasColorMap
		color *= texture2D(colorMap, texCoord);
#endif 

#if hasLumiMap
		color.rgb += (vec3(1.0,1.0,1.0)-color.rgb) * 2.0 * texture2D(lumiMap, texCoord).rgb;
#endif

#if hasLightMap
		color.rgb += (texture2D(lightMap, gl_TexCoord[2].st).rgb*2.0)*texture2D(colorMap, texCoord).rgb;
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
