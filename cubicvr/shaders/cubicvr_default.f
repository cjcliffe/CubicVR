#pragma optimize(on)
varying vec4 ambientGlobal;

#define hasLightMap %hasLightMap%
#define hasColorMap %hasColorMap%
#define hasAlphaMap %hasAlphaMap%
#define hasParallaxMap %hasParallaxMap%

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

#if hasParallaxMap
	uniform sampler2D parallaxMap;
	uniform float parallaxScale;
	uniform float parallaxBias;
	varying vec3 eyeVec; 
#endif

void main() 
{	   	

#if hasParallaxMap
	  float height = clamp((texture2D(parallaxMap, gl_TexCoord[0].xy).r+texture2D(parallaxMap, gl_TexCoord[0].xy).g+texture2D(parallaxMap, gl_TexCoord[0].xy).b),0.1,0.7);  
	  float v = (height) * parallaxScale - parallaxBias; 
	  vec3 eye = normalize(eyeVec); 
	  vec2 texCoord = gl_TexCoord[0].xy + (eye.xy * v);					 
#else
	  vec2 texCoord = gl_TexCoord[0].xy;
#endif



#if hasLightMap
	color.rgb = texture2D(lightMap, gl_TexCoord[2].st).rgb*2.0;
#endif

#if hasColorMap
	color *= texture2D(colorMap, texCoord);
#endif

#if hasAlphaMap
		color.a = texture2D(colorMap, gl_TexCoord[0].st).a; 
#else
		color.a = gl_FrontLightModelProduct.sceneColor.a;
#endif

	gl_FragColor = vec4(color.rgb + ambientGlobal.rgb,1.0);
}
