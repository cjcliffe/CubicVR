varying vec4 ambientGlobal;

uniform sampler2D colorBuffer;
uniform sampler2D normalBuffer;
uniform sampler2D positionBuffer;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;


void main()
{
	vec3 n,halfV,viewV,ldir;
	float NdotL,NdotHV;
	vec4 color = vec4(0,0,0,1);
	vec4 litColor = ambientGlobal;

	vec2 texCoord = gl_TexCoord[0].xy;

	vec3 aux;
	
	vec3 lightDir;
	
	vec4 ecPosDepth = texture2D(positionBuffer, texCoord);

	if (ecPosDepth.w == 0.0) discard;

	vec3 ecPos = ecPosDepth.xyz;

	float dist;

	vec4 lightPos;	
	vec3 halfVector;

	float att;
	float distSqr;
	
	vec4 normalSpec = texture2D(normalBuffer, texCoord);

	n = normalSpec.xyz;

	float specVal = normalSpec.w;

	vec4 baseColor = texture2D(colorBuffer, texCoord);

%LIGHTLOOP:START%

	lightPos = gl_LightSource[%LIGHTID%].position;	
	halfVector = normalize(gl_LightSource[%LIGHTID%].halfVector.xyz);
		
	aux=gl_LightSource[%LIGHTID%].position.xyz-ecPos;
	
	if (gl_LightSource[%LIGHTID%].spotExponent > 0.0)
	{
		lightDir=aux;
	}
	else 
	{
		lightDir = normalize(aux); 
	}
	


	// compute the dot product between normal and normalized lightdir 
	NdotL = max(dot(n,normalize(lightDir)),0.0);

	if (NdotL > 0.0) 
	{

		if (gl_LightSource[%LIGHTID%].spotExponent > 0.0)
		{		
			distSqr = dot(lightDir, lightDir);
			att = clamp(1.0 - gl_LightSource[%LIGHTID%].spotExponent * sqrt(distSqr), 0.0, 1.0);
//			vec3 L = lightDir * inversesqrt(distSqr);
		}
		else
		{
			dist = length(lightDir);
			att = 1.0 / (gl_LightSource[%LIGHTID%].constantAttenuation +
					gl_LightSource[%LIGHTID%].linearAttenuation * dist +
					gl_LightSource[%LIGHTID%].quadraticAttenuation * dist * dist);
		}

			
		litColor += att * (gl_LightSource[%LIGHTID%].diffuse * NdotL) * baseColor;
	
//		vec4 specVal;
//		

		halfV = normalize(halfVector);
		NdotHV = max(dot(n,halfV),0.0);
		litColor += att * specVal * gl_LightSource[%LIGHTID%].specular * baseColor *
						pow(NdotHV,gl_FrontMaterial.shininess);
	}

	color += litColor;
	litColor = vec4(0,0,0,1);

%LIGHTLOOP:END%


	gl_FragColor = color;
//	gl_FragColor = vec4(ecPosDepth.w,ecPosDepth.w,ecPosDepth.w,1.0);
	gl_FragDepth = ecPosDepth.w;
}


