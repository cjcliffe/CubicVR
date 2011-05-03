#pragma optimize(on)

varying vec3 normal;
varying vec4 position;
varying vec3 g_norm;
//varying vec4 g_pos;

#define hasLightMap %hasLightMap%
#define hasParallaxMap %hasParallaxMap%
#define hasEnvSphereMap %hasEnvSphereMap%
#define hasBumpMap %hasBumpMap%


#if hasParallaxMap
varying vec3 eyeVec; 
#endif


#if hasEnvSphereMap
	varying vec4 projEnv;
#if hasBumpMap
	varying vec3 u;
#endif
#endif
	varying vec3 v_n;

uniform vec3 camPos;
uniform mat4 objMatrix;

void main()
{
	
//	vec4 eyeCoordPos = gl_ModelViewMatrix * gl_Vertex;

//    position = eyeCoordPos.xyz / eyeCoordPos.w;

//	g_pos = gl_ModelViewMatrix * gl_Vertex; // gl_ModelViewMatrix * gl_Vertex;

	g_norm = gl_Normal.xyz+gl_Vertex.xyz;
	
	position = objMatrix * gl_Vertex;

	normal = (objMatrix * vec4(gl_Normal.xyz+gl_Vertex.xyz,1.0)).xyz; 
	normal -= position.xyz;
	normal = normalize(normal);
  
    gl_TexCoord[0] = gl_MultiTexCoord0;

#if hasLightMap
	gl_TexCoord[2] = gl_MultiTexCoord1;
#endif

	v_n = normalize( gl_NormalMatrix * gl_Normal );


#if hasEnvSphereMap
#if hasBumpMap

	u = normalize( vec3(gl_ModelViewMatrix * gl_Vertex) );

#else

	vec3 u = normalize( vec3(gl_ModelViewMatrix * gl_Vertex) );
//	vec3 n = normalize( gl_NormalMatrix * gl_Normal );
	vec3 r = reflect( position.xyz-camPos, normal );
	float m = 2.0 * sqrt( r.x*r.x + r.y*r.y + (r.z+1.0)*(r.z+1.0) );
	gl_TexCoord[1].s = r.x/m + 0.5;
	gl_TexCoord[1].t = r.y/m + 0.5;

#endif
#endif


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

	gl_Position = ftransform();
}