#define hasShadowMap0 %hasShadowMap0%
#define hasShadowMap1 %hasShadowMap1%
#define hasShadowMap2 %hasShadowMap2%
#define hasShadowMap3 %hasShadowMap3%
#define hasShadowMap4 %hasShadowMap4%
#define hasShadowMap5 %hasShadowMap5%
#define hasShadowMap6 %hasShadowMap6%
#define hasShadowMap7 %hasShadowMap7%

uniform mat4 modelViewInv;




#if hasShadowMap0
	uniform sampler2DShadow shadowMap0;
#endif



varying vec4 ambientGlobal;

uniform sampler2D colorBuffer;
uniform sampler2D normalBuffer;
uniform sampler2D positionBuffer;

//uniform mat4 viewMatrix;
//uniform mat4 projMatrix;
//
//uniform vec3 buffersize;
//uniform vec3 camerarange;

void main()
{
	vec2 texCoord = gl_TexCoord[0].xy;

	vec4 color = vec4(0,0,0,1);

	vec4 ecPosDepth = texture2D(positionBuffer, texCoord);

	vec3 ecPos = ecPosDepth.xyz;

	vec4 baseColor = texture2D(colorBuffer, texCoord);

	vec3 n;

	vec4 litColor = ambientGlobal;

	vec4 normalSpec = texture2D(normalBuffer, texCoord);

	n = normalize(normalSpec.xyz);

	float specVal = normalSpec.w;

	vec3 l;
	float d;
	float atten;
	float spotDot;
	float spotEffect;
	vec3 v;
	vec3 h;
    float nDotL, nDotH, power;	
	vec4 diffuse, specular;
	
///////////////////////////////

    l = gl_LightSource[%LIGHTID%].position.xyz - ecPos;
    d = length(l);
    atten = 1.0 / (gl_LightSource[%LIGHTID%].constantAttenuation +
                         gl_LightSource[%LIGHTID%].linearAttenuation * d +
                         gl_LightSource[%LIGHTID%].quadraticAttenuation * d * d);
    
    l = normalize(l);

    spotDot = dot(-l, normalize(gl_LightSource[%LIGHTID%].spotDirection));

    spotEffect = (spotDot < gl_LightSource[%LIGHTID%].spotCosCutoff)
                        ? 0.0 : pow(spotDot, gl_LightSource[%LIGHTID%].spotExponent);

    atten *= spotEffect;

    v = normalize(-ecPos);
    h = normalize(l + v);
    
    nDotL = max(0.0, dot(n, l));
    nDotH = max(0.0, dot(n, h));
    power = (nDotL == 0.0) ? 0.0 : pow(nDotH, gl_FrontMaterial.shininess);
    
    diffuse = gl_FrontLightProduct[%LIGHTID%].diffuse * nDotL * atten;

    specular = specVal * gl_FrontLightProduct[%LIGHTID%].specular * power * atten;

    litColor = baseColor*(diffuse+specular);

#if hasShadowMap0
 	litColor = litColor*(0.5+clamp(shadow2DProj(shadowMap0,  gl_TextureMatrix[0]*vec4(ecPos,1)),0.0,1.0));
#endif

	color += litColor;

///////////////////////////////



	gl_FragColor = color;
	gl_FragDepth = ecPosDepth.w;
}


