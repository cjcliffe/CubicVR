varying vec4 diffuse,ambientGlobal,ambient;
varying vec3 normal,lightDir,halfVector;
varying float dist;

#define hasEnvSphereMap %hasEnvSphereMap%
#define hasBumpMap %hasBumpMap%
#define hasParallaxMap %hasParallaxMap%


#if hasEnvSphereMap
	varying vec4 projEnv;
#if hasBumpMap
	varying vec3 u;
#endif
#endif

#if hasParallaxMap
varying vec3 eyeVec; 
#endif


void main()
{	
	vec3 ecPos;
	vec3 aux;
	
	normal = normalize(gl_NormalMatrix * gl_Normal);
	
	/* these are the new lines of code to compute the light's direction */
	ecPos = vec3(gl_ModelViewMatrix * gl_Vertex);
	aux = gl_LightSource[%LIGHTID%].position.xyz-ecPos;
	
	
	if (gl_LightSource[%LIGHTID%].spotExponent > 0.0)
	{
		lightDir = aux;
	}
	else 
	{
		lightDir = normalize(aux); 
	}
	dist = length(aux);

	halfVector = normalize(gl_LightSource[%LIGHTID%].halfVector.xyz);
	
	/* Compute the diffuse, ambient and globalAmbient terms */
	diffuse = gl_FrontMaterial.diffuse * gl_LightSource[%LIGHTID%].diffuse;
	
	/* The ambient terms have been separated since one of them */
	/* suffers attenuation */
	ambient = gl_FrontMaterial.ambient * gl_LightSource[%LIGHTID%].ambient;
	ambientGlobal = gl_LightModel.ambient * gl_FrontMaterial.ambient;
	
	
#if hasEnvSphereMap
#if hasBumpMap

	u = normalize( vec3(gl_ModelViewMatrix * gl_Vertex) );

#else

	vec3 u = normalize( vec3(gl_ModelViewMatrix * gl_Vertex) );
	vec3 n = normalize( gl_NormalMatrix * gl_Normal );
	vec3 r = reflect( u, n );
	float m = 2.0 * sqrt( r.x*r.x + r.y*r.y + (r.z+1.0)*(r.z+1.0) );
	gl_TexCoord[1].s = r.x/m + 0.5;
	gl_TexCoord[1].t = r.y/m + 0.5;

#endif
#endif
	
	
	
    gl_TexCoord[0] = gl_MultiTexCoord0;
		
	gl_Position = ftransform();
	
#if hasParallaxMap
	vec3 tangent;
	vec3 binormal;

	vec3 c1 = cross( gl_Normal, vec3(0.0, 0.0, 1.0) );
	vec3 c2 = cross( gl_Normal, vec3(0.0, 1.0, 0.0) );

	if( length(c1)>length(c2) )
	{
		tangent = c1;
	}
	else
	{
		tangent = c2;
	}

	tangent = normalize(tangent);

	binormal = cross(gl_Normal, tangent);
	binormal = normalize(binormal);

	mat3 TBNMatrix = mat3(gl_NormalMatrix * tangent,gl_NormalMatrix * binormal,gl_NormalMatrix * gl_Normal);
	eyeVec = vec3(gl_ModelViewMatrix * gl_Vertex) * TBNMatrix;

#endif

} 

