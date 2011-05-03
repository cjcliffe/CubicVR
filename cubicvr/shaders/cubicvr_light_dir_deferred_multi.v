varying vec4 ambientGlobal;

void main()
{		
	ambientGlobal = gl_LightModel.ambient;
          	
    gl_TexCoord[0] = gl_MultiTexCoord0;
		
	gl_Position = ftransform();
} 

