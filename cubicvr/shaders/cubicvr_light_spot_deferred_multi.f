#define hasShadowMap0 %hasShadowMap0%
#define hasShadowMap1 %hasShadowMap1%
#define hasShadowMap2 %hasShadowMap2%
#define hasShadowMap3 %hasShadowMap3%
#define hasShadowMap4 %hasShadowMap4%
#define hasShadowMap5 %hasShadowMap5%
#define hasShadowMap6 %hasShadowMap6%
#define hasShadowMap7 %hasShadowMap7%

uniform mat4 modelViewInv;

%LIGHTLOOP:START%

#if hasShadowMap%LIGHTID%
	uniform sampler2DShadow shadowMap%LIGHTID%;
#endif

%LIGHTLOOP:END%

varying vec4 ambientGlobal;

uniform sampler2D colorBuffer;
uniform sampler2D normalBuffer;
uniform sampler2D positionBuffer;
uniform sampler2D lumiBuffer;

uniform vec3 camPos;

//uniform mat4 viewMatrix;
//uniform mat4 projMatrix;
//
//uniform vec3 buffersize;
//uniform vec3 camerarange;


float shadowVal(vec4 shadowProj, sampler2DShadow shadowMap, float rad, float shadowRes)
{
	vec2 filterTaps[12]; 
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


	vec3 ecPos = ecPosDepth.xyz;

	vec4 baseColor = texture2D(colorBuffer, texCoord);


	vec4 litColor = ambientGlobal;

	vec4 normalSpec = texture2D(normalBuffer, texCoord);

	vec4 lumiDepth = texture2D(lumiBuffer, texCoord);

	vec3 n;

	n = normalSpec.xyz;	

	float specVal = normalSpec.w;

	vec3 l;
	float d;
	float atten;
//	float spotDot;
	float spotEffect;
	vec3 v;
	vec3 h;
    float nDotL, nDotHV, power;	
	vec4 diffuse, specular;
	
///////////////////////////////

%LIGHTLOOP:START%
    l = gl_LightSource[%LIGHTID%].position.xyz - ecPos;
    d = length(l);
	
    atten = 1.0 / (gl_LightSource[%LIGHTID%].constantAttenuation +
                         gl_LightSource[%LIGHTID%].linearAttenuation * d +
                         gl_LightSource[%LIGHTID%].quadraticAttenuation * d * d);
    
    l = normalize(l);


    spotEffect = -dot(-l,normalize(gl_LightSource[%LIGHTID%].spotDirection));

    spotEffect = (spotEffect > gl_LightSource[%LIGHTID%].spotCosCutoff) ? pow(spotEffect, gl_LightSource[%LIGHTID%].spotExponent):0.0;

    atten *= spotEffect;

    v = normalize(-ecPos);
    //h = normalize(l + v);
    
    nDotL = max(0.0, dot(n, l));
//    nDotH = max(0.0, dot(n, h));

	nDotHV = max(dot(n,normalize(gl_LightSource[%LIGHTID%].halfVector.xyz)),0.0);

	
		power = (nDotL == 0.0) ? 0.0 : pow(nDotHV, lumiDepth.z);
		
		diffuse = gl_FrontLightProduct[%LIGHTID%].diffuse * nDotL * atten;

		specular = specVal * gl_FrontLightProduct[%LIGHTID%].specular * power * atten;
	
    litColor = baseColor*(diffuse+specular);


#if hasShadowMap%LIGHTID%
	vec3 lightDir = normalize(gl_LightSource[%LIGHTID%].spotDirection.xyz);

	vec4 shadowpos = vec4(ecPosDepth.xyz,1.0) - vec4(normalize(ecPosDepth.xyz-camPos)*0.1,0.0) + vec4(normalize(lightDir)*0.1,0.0);

	vec4 shadowpos0 = gl_TextureMatrix[%LIGHTID%]*shadowpos;
	
 	litColor = litColor*shadowVal(shadowpos0, shadowMap%LIGHTID%, 2.0, 1024.0);
#endif

	color += litColor;

//litColor = vec4(0,0,0,1);
%LIGHTLOOP:END%
///////////////////////////////
	gl_FragColor = color;
	gl_FragDepth = ecPosDepth.w;
}


