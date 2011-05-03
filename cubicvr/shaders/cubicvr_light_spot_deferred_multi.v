varying vec4 ambientGlobal;

#define hasShadowMap0 %hasShadowMap0%
#define hasShadowMap1 %hasShadowMap1%
#define hasShadowMap2 %hasShadowMap2%
#define hasShadowMap3 %hasShadowMap3%
#define hasShadowMap4 %hasShadowMap4%
#define hasShadowMap5 %hasShadowMap5%
#define hasShadowMap6 %hasShadowMap6%
#define hasShadowMap7 %hasShadowMap7%

//#if hasShadowMap0|hasShadowMap1|hasShadowMap2|hasShadowMap3|hasShadowMap4|hasShadowMap5|hasShadowMap6|hasShadowMap7
//	varying vec4 ProjShadow[8];
//#endif

void main()
{		
	ambientGlobal = gl_LightModel.ambient;
          	
			
    gl_TexCoord[0] = gl_MultiTexCoord0;
		
	gl_Position = ftransform();
} 

