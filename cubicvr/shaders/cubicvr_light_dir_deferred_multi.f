varying vec4 ambientGlobal;

uniform sampler2D colorBuffer;
uniform sampler2D normalBuffer;
uniform sampler2D positionBuffer;
uniform sampler2D lumiBuffer;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec3 buffersize;
uniform vec3 camerarange;


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

	vec4 litColor = ambientGlobal*baseColor;

	vec4 normalSpec = texture2D(normalBuffer, texCoord);

	n = normalSpec.xyz;

	float specVal = normalSpec.w;
	
%LIGHTLOOP:START%
	lightDir = normalize(gl_LightSource[%LIGHTID%].position.xyz);
	halfVector = normalize(gl_LightSource[%LIGHTID%].halfVector.xyz);

	NdotL = max(dot(n,lightDir),0.0);

	if (NdotL > 0.0) 
	{
		color += baseColor * gl_LightSource[%LIGHTID%].diffuse * NdotL;		
	}

	NdotHV = max(dot(n, halfVector),0.0001);

	litColor += specVal * baseColor * pow(NdotHV,texture2D(lumiBuffer,texCoord).z);

	color += litColor;
	litColor = vec4(0,0,0,1);
%LIGHTLOOP:END%

	gl_FragColor = vec4(color.rgb,1);
	gl_FragDepth = ecPosDepth.w;
}


