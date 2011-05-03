varying vec4 ambientGlobal;

uniform sampler2D colorBuffer;
uniform sampler2D normalBuffer;
uniform sampler2D positionBuffer;
uniform sampler2D lumiBuffer;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec3 buffersize;
uniform vec3 camerarange;

#define hasShadowMap0 %hasShadowMap0%
#define hasShadowMap1 %hasShadowMap1%
#define hasShadowMap2 %hasShadowMap2%
#define hasShadowMap3 %hasShadowMap3%
#define hasShadowMap4 %hasShadowMap4%
#define hasShadowMap5 %hasShadowMap5%
#define hasShadowMap6 %hasShadowMap6%
#define hasShadowMap7 %hasShadowMap7%

uniform mat4 modelViewInv;
uniform vec3 camPos;


#if hasShadowMap0|hasShadowMap1|hasShadowMap2|hasShadowMap3|hasShadowMap4|hasShadowMap5|hasShadowMap6|hasShadowMap7
	uniform sampler2DShadow shadowMap0;
	uniform sampler2DShadow shadowMap1;
	uniform sampler2DShadow shadowMap2;

	uniform float nearShadowRes;
	uniform float farShadowRes;
#endif


float shadowVal(vec4 shadowProj, sampler2DShadow shadowMap, float rad, float shadowRes)
{
	vec2 filterTaps[12]; // = 
//	{
	filterTaps[0] = vec2(-0.326212,-0.40581);
	filterTaps[1] = vec2(-0.840144,-0.07358);
	filterTaps[2] = vec2(-0.695914,0.457137);
	filterTaps[3] = vec2(-0.203345,0.620716);
	filterTaps[4] = vec2(0.96234,-0.194983);
	filterTaps[5] = vec2(0.473434,-0.480026); 
	filterTaps[6] = vec2(0.519456,0.767022);
	filterTaps[7] = vec2(0.185461,-0.893124); 
	filterTaps[8] = vec2(0.507431,0.064425);
	filterTaps[9] = vec2(0.89642,0.412458) ;
	filterTaps[10] =vec2(-0.32194,-0.932615);
	filterTaps[11] =vec2(-0.791559,-0.59771);
//	};

//	filterTaps[0] = vec2(-0.326212, -0.405805);
//	filterTaps[1] = vec2(-0.840144, -0.073580);
//	filterTaps[2] = vec2(-0.695914,  0.457137);
//	filterTaps[3] = vec2(-0.203345,  0.620716);
//	filterTaps[4] = vec2( 0.962340, -0.194983);
//	filterTaps[5] = vec2( 0.473434, -0.480026);
//	filterTaps[6] = vec2( 0.519456,  0.767022);
//	filterTaps[7] = vec2( 0.185461, -0.893124);
//	filterTaps[8] = vec2( 0.507431,  0.064425);
//	filterTaps[9] = vec2( 0.896420,  0.412458);
//	filterTaps[10] = vec2(-0.321940, -0.932615);
//	filterTaps[11] = vec2(-0.791559, -0.597705);

	float mapScale = rad / shadowRes;

	vec3 shadowST = shadowProj.xyz / shadowProj.w;
	
	vec4 shadowColor = shadow2D(shadowMap, shadowST);
	
	shadowColor += shadow2D(shadowMap, shadowST.xyz + vec3( filterTaps[0].s*mapScale,  filterTaps[0].s*mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowST.xyz + vec3( filterTaps[1].s*mapScale,  filterTaps[1].s*mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowST.xyz + vec3( filterTaps[2].s*mapScale,  filterTaps[2].s*mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowST.xyz + vec3(-filterTaps[3].s*mapScale,  filterTaps[3].s*mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowST.xyz + vec3(-filterTaps[4].s*mapScale,  filterTaps[4].s*mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowST.xyz + vec3(-filterTaps[5].s*mapScale,  filterTaps[5].s*mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowST.xyz + vec3(filterTaps[6].s*mapScale,  filterTaps[6].s*mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowST.xyz + vec3(filterTaps[7].s*mapScale,  filterTaps[7].s*mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowST.xyz + vec3(filterTaps[8].s*mapScale,  filterTaps[8].s*mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowST.xyz + vec3(filterTaps[9].s*mapScale,  filterTaps[9].s*mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowST.xyz + vec3(filterTaps[10].s*mapScale,  filterTaps[10].s*mapScale, 0));
	shadowColor += shadow2D(shadowMap, shadowST.xyz + vec3(filterTaps[11].s*mapScale,  filterTaps[11].s*mapScale, 0));
	shadowColor = shadowColor / 13.0;

	
	return (shadowProj.w > 0.000) ? shadowColor.r : 1.0;
}




void main()
{

	vec2 texCoord = gl_TexCoord[0].xy;

	vec4 color = vec4(0,0,0,1);

	vec4 ecPosDepth = texture2D(positionBuffer, texCoord);

	if (ecPosDepth.w == 0.0) discard;

	vec4 baseColor = texture2D(colorBuffer, texCoord);

	vec3 n,halfV;
	float NdotL,NdotHV;

	vec3 lightDir;
	vec3 halfVector;

	vec4 normalSpec = texture2D(normalBuffer, texCoord);
	vec4 litColor = vec4(0,0,0,1);

	n = normalSpec.xyz;

	float specVal = normalSpec.w;
	
%LIGHTLOOP:START%


	lightDir = normalize(gl_LightSource[%LIGHTID%].spotDirection.xyz);
	halfVector = normalize(gl_LightSource[%LIGHTID%].halfVector.xyz);

#if hasShadowMap0|hasShadowMap1|hasShadowMap2|hasShadowMap3|hasShadowMap4|hasShadowMap5|hasShadowMap6|hasShadowMap7		

	vec4 shadowpos = vec4(ecPosDepth.xyz,1.0) - vec4(normalize(ecPosDepth.xyz-camPos)*0.1,0.0) + vec4(normalize(lightDir)*0.1,0.0);

	vec4 shadowpos0 = gl_TextureMatrix[%LIGHTID%]*shadowpos;
	vec4 shadowpos1 = gl_TextureMatrix[%LIGHTID%+1]*shadowpos;
	vec4 shadowpos2 = gl_TextureMatrix[%LIGHTID%+2]*shadowpos;

	float shadowv = min(shadowVal(shadowpos0, shadowMap0, 2.0, nearShadowRes),min(
					shadowVal(shadowpos1, shadowMap1, 1.0, farShadowRes),
					shadowVal(shadowpos2, shadowMap2, 1.0, farShadowRes)));


//	 float shadowv = shadowVal(gl_TextureMatrix[%LIGHTID%]*vec4(ecPosDepth.xyz+(lightDir*0.2),1.0), shadowMap0)
//			   *shadowVal(gl_TextureMatrix[%LIGHTID%+1]*vec4(ecPosDepth.xyz+(lightDir*0.2),1.0), shadowMap1)
//			   *shadowVal(gl_TextureMatrix[%LIGHTID%+2]*vec4(ecPosDepth.xyz+(lightDir*0.2),1.0), shadowMap2);


//	float shadowv = 1.0;
#endif


	NdotL = max(dot(n,lightDir),0.0);

	if (NdotL > 0.0) 
	{
		litColor += gl_LightSource[%LIGHTID%].diffuse * NdotL;		
	}

	NdotHV = max(dot(n, halfVector),0.0001);

	litColor += specVal * pow(NdotHV,texture2D(lumiBuffer,texCoord).z);


#if hasShadowMap0|hasShadowMap1|hasShadowMap2|hasShadowMap3|hasShadowMap4|hasShadowMap5|hasShadowMap6|hasShadowMap7

	litColor *= shadowv;
	
// 	color *= 
//	clamp(
//	 shadow2DProj(shadowMap0, gl_TextureMatrix[%LIGHTID%]*vec4(ecPosDepth.xyz+(lightDir.xyz*0.1),1.0)).r*
//	 shadow2DProj(shadowMap1, gl_TextureMatrix[%LIGHTID%+1]*vec4(ecPosDepth.xyz+(lightDir.xyz*0.4),1.0)).r*
// 	 shadow2DProj(shadowMap2, gl_TextureMatrix[%LIGHTID%+2]*vec4(ecPosDepth.xyz+(lightDir.xyz*0.4),1.0)).r,0.3,1.0);
#endif

	litColor += ambientGlobal;

	color += baseColor*litColor;
	litColor = vec4(0,0,0,1);
	
	
%LIGHTLOOP:END%

	gl_FragColor = vec4(color.rgb,1);
	gl_FragDepth = ecPosDepth.w;
}


