varying vec4 diffuse,ambientGlobal, ambient;

uniform sampler2D colorBuffer;
uniform sampler2D normalBuffer;
uniform sampler2D positionBuffer;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

//uniform vec3 lightPos;
//
//uniform vec3 buffersize;
//uniform vec3 camerarange;

void main()
{
	vec3 n,halfV,viewV,ldir;
	float NdotL,NdotHV;
	vec4 color = ambientGlobal;

	vec2 texCoord = gl_TexCoord[0].xy;

	vec3 aux;
	
	vec3 lightDir;
	
	vec4 lightPos = gl_LightSource[%LIGHTID%].position;
	
	float dist;

	vec3 halfVector = normalize(gl_LightSource[%LIGHTID%].halfVector.xyz);


//	vec3 screencoord; 
//	screencoord = vec3(((gl_FragCoord.x/buffersize.x)-0.5) * 2.0,((-gl_FragCoord.y/buffersize.y)+0.5) * 2.0 /  (buffersize.x/buffersize.y),DepthToZPosition( gl_FragCoord.z )); 
//	screencoord.x *= screencoord.z; 
//	screencoord.y *= -screencoord.z; 


	vec4 ecPosDepth = texture2D(positionBuffer, texCoord);

	vec3 ecPos = ecPosDepth.xyz;
		
	aux=gl_LightSource[%LIGHTID%].position.xyz-ecPos;
	
	if (gl_LightSource[%LIGHTID%].spotExponent > 0.0)
	{
		lightDir=aux;
	}
	else 
	{
		lightDir = normalize(aux); 
	}
	
	dist = length(lightDir);

	vec4 normalSpec = texture2D(normalBuffer, texCoord);

	n = normalSpec.xyz;

	float specVal = normalSpec.w;

	// compute the dot product between normal and normalized lightdir 
	NdotL = max(dot(n,normalize(lightDir)),0.0);

	float att;
	float distSqr;

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
			att = 1.0 / (gl_LightSource[%LIGHTID%].constantAttenuation +
					gl_LightSource[%LIGHTID%].linearAttenuation * dist +
					gl_LightSource[%LIGHTID%].quadraticAttenuation * dist * dist);
		}

			
		color += att * (diffuse * NdotL + ambient);
	
//		vec4 specVal;
//		

		halfV = normalize(halfVector);
		NdotHV = max(dot(n,halfV),0.0);
		color += att * specVal * gl_LightSource[%LIGHTID%].specular * 
						pow(NdotHV,gl_FrontMaterial.shininess);
	}


	color *= texture2D(colorBuffer, texCoord);

	gl_FragColor = color;
	gl_FragDepth = ecPosDepth.w;
}


