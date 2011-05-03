varying vec4 ambientGlobal;

void main()
{		
	/* Compute the diffuse, ambient and globalAmbient terms */
	
	/* The ambient terms have been separated since one of them */
	/* suffers attenuation */
	ambientGlobal = gl_LightModel.ambient;
		
    gl_TexCoord[0] = gl_MultiTexCoord0;
		
	gl_Position = ftransform();
} 

