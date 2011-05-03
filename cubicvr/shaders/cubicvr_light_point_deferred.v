varying vec4 diffuse,ambientGlobal,ambient;

void main()
{		
	/* Compute the diffuse, ambient and globalAmbient terms */
	diffuse = gl_FrontMaterial.diffuse * gl_LightSource[%LIGHTID%].diffuse;
	
	/* The ambient terms have been separated since one of them */
	/* suffers attenuation */
	ambient = gl_FrontMaterial.ambient * gl_LightSource[%LIGHTID%].ambient;
	ambientGlobal = gl_LightModel.ambient * gl_FrontMaterial.ambient;
		
    gl_TexCoord[0] = gl_MultiTexCoord0;
		
	gl_Position = ftransform();
} 

