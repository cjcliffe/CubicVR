/*
    This file is part of CubicVR.

    Copyright (C) 2003 by Charles J. Cliffe

		Permission is hereby granted, free of charge, to any person obtaining a copy
		of this software and associated documentation files (the "Software"), to deal
		in the Software without restriction, including without limitation the rights
		to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
		copies of the Software, and to permit persons to whom the Software is
		furnished to do so, subject to the following conditions:

		The above copyright notice and this permission notice shall be included in
		all copies or substantial portions of the Software.

		THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
		IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
		FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
		AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
		LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
		OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
		THE SOFTWARE.
*/

#include <CubicVR/Scene.h>


#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)
//#include <CubicVR/scene_shaders.h>

/*
 
 vec3 unpack_vec3a(vec3 inp_vec)
 {
 return vec3(fract(inp_vec.x)*10.0*2.0-1.0,fract(inp_vec.y)*10.0*2.0-1.0,fract(inp_vec.z)*10.0*2.0-1.0);	
 }
 
 vec3 unpack_vec3b(vec3 inp_vec)
 {
 return vec3(floor(inp_vec.x)/1000.0,floor(inp_vec.y)/1000.0,floor(inp_vec.z)/1000.0);	
 }

 */

const char *shader_post_ssao_v = 
STRINGIFY(
		  
	  void main(void)
	  {
		  gl_TexCoord[0] = gl_MultiTexCoord0;
		  
		  gl_Position = ftransform();
	  }
		  
);

const char *shader_post_ssao_f = 
STRINGIFY(
		  uniform sampler2D lumiBuffer; // depth texture
		  uniform sampler2D renderTargetTex; // rendered scene color texture
		  
		  uniform vec3 texel_ofs;
		  uniform float nearPlane;
		  uniform float farPlane;
		  
		  
		  float compareDepths( in float depth1, in float depth2, float aoMultiplier ) 
		  {
		  float aoCap = 1.4;
		  float depthTolerance=0.000;
		  float aorange = 20.0;// units in space the AO effect extends to (this gets divided by the camera far range
		  float diff = sqrt( clamp(1.0-(depth1-depth2) / (aorange/(farPlane-nearPlane)),0.0,1.0) );
		  float ao = min(aoCap,max(0.0,depth1-depth2-depthTolerance) * aoMultiplier) * diff;
		  return ao;
		  }
		  
		  void main(void)
		  {	
		  vec2 texCoord = gl_TexCoord[0].xy;
		  float depth = texture2D( lumiBuffer, texCoord ).w;
		  float d;
		  
		  float pw = texel_ofs.x;
		  float ph = texel_ofs.y;
		  
		  float aoCap = 1.0;
		  
		  float ao = 0.0;
		  
		  float aoMultiplier=10000.0;
		  		  
		  float aoscale=1.0;
		  
		  d=texture2D( lumiBuffer,  vec2(texCoord.x+pw,texCoord.y+ph), aoMultiplier).w;
		  ao+=compareDepths(depth, d, aoMultiplier)/aoscale;
		  
		  d=texture2D( lumiBuffer,  vec2(texCoord.x-pw,texCoord.y+ph), aoMultiplier).w;
		  ao+=compareDepths(depth, d, aoMultiplier)/aoscale;
		  
		  d=texture2D( lumiBuffer,  vec2(texCoord.x+pw,texCoord.y-ph), aoMultiplier).w;
		  ao+=compareDepths(depth, d, aoMultiplier)/aoscale;
		  
		  d=texture2D( lumiBuffer,  vec2(texCoord.x-pw,texCoord.y-ph), aoMultiplier).w;
		  ao+=compareDepths(depth, d, aoMultiplier)/aoscale;
		  
		  pw*=2.0;
		  ph*=2.0;
		  aoMultiplier/=2.0;
		  aoscale*=2.0;
		  
		  d=texture2D( lumiBuffer,  vec2(texCoord.x+pw,texCoord.y+ph)).w;
		  ao+=compareDepths(depth, d, aoMultiplier)/aoscale;
		  
		  d=texture2D( lumiBuffer,  vec2(texCoord.x-pw,texCoord.y+ph)).w;
		  ao+=compareDepths(depth, d, aoMultiplier)/aoscale;
		  
		  d=texture2D( lumiBuffer,  vec2(texCoord.x+pw,texCoord.y-ph)).w;
		  ao+=compareDepths(depth, d, aoMultiplier)/aoscale;
		  
		  d=texture2D( lumiBuffer,  vec2(texCoord.x-pw,texCoord.y-ph)).w;
		  ao+=compareDepths(depth, d, aoMultiplier)/aoscale;
		  
		  pw*=2.0;
		  ph*=2.0;
		  aoMultiplier/=2.0;
		  aoscale*=2.0;
		  
		  d=texture2D( lumiBuffer,  vec2(texCoord.x+pw,texCoord.y+ph)).w;
		  ao+=compareDepths(depth, d, aoMultiplier)/aoscale;
		  
		  d=texture2D( lumiBuffer,  vec2(texCoord.x-pw,texCoord.y+ph)).w;
		  ao+=compareDepths(depth, d, aoMultiplier)/aoscale;
		  
		  d=texture2D( lumiBuffer,  vec2(texCoord.x+pw,texCoord.y-ph)).w;
		  ao+=compareDepths(depth, d, aoMultiplier)/aoscale;
		  
		  d=texture2D( lumiBuffer,  vec2(texCoord.x-pw,texCoord.y-ph)).w;
		  ao+=compareDepths(depth, d, aoMultiplier)/aoscale;
		  
		  pw*=2.0;
		  ph*=2.0;
		  aoMultiplier/=2.0;
		  aoscale*=2.0;
		  
		  d=texture2D( lumiBuffer,  vec2(texCoord.x+pw,texCoord.y+ph)).w;
		  ao+=compareDepths(depth, d, aoMultiplier)/aoscale;
		  
		  d=texture2D( lumiBuffer,  vec2(texCoord.x-pw,texCoord.y+ph)).w;
		  ao+=compareDepths(depth, d, aoMultiplier)/aoscale;
		  
		  d=texture2D( lumiBuffer,  vec2(texCoord.x+pw,texCoord.y-ph)).w;
		  ao+=compareDepths(depth, d, aoMultiplier)/aoscale;
		  
		  d=texture2D( lumiBuffer,  vec2(texCoord.x-pw,texCoord.y-ph)).w;
		  ao+=compareDepths(depth, d, aoMultiplier)/aoscale;
		  
		  ao/=16.0;
		  
		  gl_FragColor = vec4(vec3(clamp(1.0-ao,0.0,1.0)),1.0) * texture2D(renderTargetTex,texCoord);
		  }
);

/*
 
 uniform sampler2D lumiBuffer; // depth texture
 uniform sampler2D renderTargetTex; // rendered scene color texture
 
 uniform vec3 texel_ofs;
 
 void main(void)
 {	
 vec2 texCoord = gl_TexCoord[0].xy;
 
 float depth = texture2D( lumiBuffer, texCoord ).w;
 float d;
 
 float pw = texel_ofs.x;
 float ph = texel_ofs.y;
 
 float aoCap = 1.0;
 
 float ao = 0.0;
 
 float aoMultiplier=1000.0;
 
 float depthTolerance = 0.0001;
 
 d=texture2D( lumiBuffer, vec2(texCoord.x+pw,texCoord.y+ph)).w;
 ao+=min(aoCap,max(0.0,depth-d-depthTolerance) * aoMultiplier);
 
 d=texture2D( lumiBuffer, vec2(texCoord.x-pw,texCoord.y+ph)).w;
 ao+=min(aoCap,max(0.0,depth-d-depthTolerance) * aoMultiplier);
 
 d=texture2D( lumiBuffer, vec2(texCoord.x+pw,texCoord.y-ph)).w;
 ao+=min(aoCap,max(0.0,depth-d-depthTolerance) * aoMultiplier);
 
 d=texture2D( lumiBuffer, vec2(texCoord.x-pw,texCoord.y-ph)).w;
 ao+=min(aoCap,max(0.0,depth-d-depthTolerance) * aoMultiplier);
 
 pw*=2.0;
 ph*=2.0;
 aoMultiplier/=2.0;
 
 d=texture2D( lumiBuffer, vec2(texCoord.x+pw,texCoord.y+ph)).w;
 ao+=min(aoCap,max(0.0,depth-d-depthTolerance) * aoMultiplier);
 
 d=texture2D( lumiBuffer, vec2(texCoord.x-pw,texCoord.y+ph)).w;
 ao+=min(aoCap,max(0.0,depth-d-depthTolerance) * aoMultiplier);
 
 d=texture2D( lumiBuffer, vec2(texCoord.x+pw,texCoord.y-ph)).w;
 ao+=min(aoCap,max(0.0,depth-d-depthTolerance) * aoMultiplier);
 
 d=texture2D( lumiBuffer, vec2(texCoord.x-pw,texCoord.y-ph)).w;
 ao+=min(aoCap,max(0.0,depth-d-depthTolerance) * aoMultiplier);
 
 pw*=2.0;
 ph*=2.0;
 aoMultiplier/=2.0;
 
 d=texture2D( lumiBuffer, vec2(texCoord.x+pw,texCoord.y+ph)).w;
 ao+=min(aoCap,max(0.0,depth-d-depthTolerance) * aoMultiplier);
 
 d=texture2D( lumiBuffer, vec2(texCoord.x-pw,texCoord.y+ph)).w;
 ao+=min(aoCap,max(0.0,depth-d-depthTolerance) * aoMultiplier);
 
 d=texture2D( lumiBuffer, vec2(texCoord.x+pw,texCoord.y-ph)).w;
 ao+=min(aoCap,max(0.0,depth-d-depthTolerance) * aoMultiplier);
 
 d=texture2D( lumiBuffer, vec2(texCoord.x-pw,texCoord.y-ph)).w;
 ao+=min(aoCap,max(0.0,depth-d-depthTolerance) * aoMultiplier);
 
 pw*=2.0;
 ph*=2.0;
 aoMultiplier/=2.0;
 
 d=texture2D( lumiBuffer, vec2(texCoord.x+pw,texCoord.y+ph)).w;
 ao+=min(aoCap,max(0.0,depth-d-depthTolerance) * aoMultiplier);
 
 d=texture2D( lumiBuffer, vec2(texCoord.x-pw,texCoord.y+ph)).w;
 ao+=min(aoCap,max(0.0,depth-d-depthTolerance) * aoMultiplier);
 
 d=texture2D( lumiBuffer, vec2(texCoord.x+pw,texCoord.y-ph)).w;
 ao+=min(aoCap,max(0.0,depth-d-depthTolerance) * aoMultiplier);
 
 d=texture2D( lumiBuffer, vec2(texCoord.x-pw,texCoord.y-ph)).w;
 ao+=min(aoCap,max(0.0,depth-d-depthTolerance) * aoMultiplier);
 
 ao/=16.0;
 
 gl_FragColor = vec4(1.0-ao) * texture2D(renderTargetTex,texCoord);
 }		  

 */

/*
			uniform vec3 texel_ofs;
		  uniform sampler2D renderTargetTex;
		  uniform sampler2D positionBuffer;
		  uniform sampler2D normalBuffer;
		  uniform	sampler2D lumiBuffer;
		  
		  vec2 sample_points[8]; 
		  
		  void main(void) 
		  { 
		  //get the depth 
		  vec2 radius = vec2(texel_ofs.x * 8.0,texel_ofs.y * 8.0);
		  float f_depth = texture2D(lumiBuffer, gl_TexCoord[0].st).w; 
		  //	  vec3 f_norm = normalize(texture2D(normalBuffer, gl_TexCoord[1].st).xyz); 
		  
		  sample_points[0] = vec2(-0.326212, -0.405805);
		  sample_points[1] = vec2(-0.840144, -0.073580);
		  sample_points[2] = vec2(-0.695914,  0.457137);
		  sample_points[3] = vec2(-0.203345,  0.620716);
		  sample_points[4] = vec2( 0.962340, -0.194983);
		  sample_points[5] = vec2( 0.473434, -0.480026);
		  sample_points[6] = vec2( 0.519456,  0.767022);
		  sample_points[7] = vec2( 0.185461, -0.893124);
		  
		  float occlusion = 0.0; 
		  
		  float depth_sample; 
		  vec2 sp;
		  
		  for (int i=0; i < 8; i++) 
		  { 		  
			  sp = vec2(radius.x * sample_points[i].x, radius.y * sample_points[i].y) + gl_TexCoord[0].st; 
			  depth_sample = f_depth - texture2D(lumiBuffer, sp).w; 
			  
			  //if the sample is closer than the cur rent pixel -> occlude 
			  if(depth_sample > 0.0)// && depth_sample < radius * 0.05) 
			  { 
				occlusion += 1.0-(depth_sample*30.0); 
			  } 
		  } 
		  
		  if (occlusion < 5.0)
		  { 
			  occlusion = 0.0; 
		  }
		  else
		  { 
			  //quadratic mapping 
			  occlusion = pow(occlusion, 2.0) / 64.0; 
//		  occlusion /= 8.0;
		  } 
		  
		  
		  //output shade 
		  gl_FragColor = vec4(texture2D(renderTargetTex,gl_TexCoord[0].st).xyz*(1.0-occlusion),1.0); 
		  
//			  gl_FragColor = vec4(vec3(1.0-occlusion),1.0); 
		  }
);
*/

const char *shader_post_copy_v = 
STRINGIFY(
		  
		  void main(void)
		  {
		  gl_TexCoord[0] = gl_MultiTexCoord0;
		  
		  gl_Position = ftransform();
		  }
		  
);

const char *shader_post_copy_f = 
STRINGIFY(
		  
		  uniform sampler2D renderTargetTex;
		  uniform sampler2D positionBuffer;
		  //	uniform	sampler2D lumiBuffer;
		  
		  void main(void)
		  {
		  gl_FragColor = vec4(texture2D(renderTargetTex, gl_TexCoord[0].xy).rgb,1.0);
		  gl_FragDepth = texture2D(positionBuffer, gl_TexCoord[0].xy).w;
		  }
		  
);


const char *shader_post_blur_v = 
STRINGIFY(
		  
		  void main(void)
		  {
			  gl_TexCoord[0] = gl_MultiTexCoord0;
			  
			  gl_Position = ftransform();
		  }
		  
);

const char *shader_post_blur_f = 
STRINGIFY(
		  
		  uniform sampler2D renderTargetTex;
		  uniform sampler2D positionBuffer;
		  //	uniform	sampler2D lumiBuffer;
		  uniform float blurAmt;
		  
		  void main(void)
		  {
			  gl_FragColor = vec4(texture2D(renderTargetTex, gl_TexCoord[0].xy).rgb,blurAmt);
			  gl_FragDepth = texture2D(positionBuffer, gl_TexCoord[0].xy).w;
		  }
		  
);



const char *shader_post_dof_12tap_v = 
STRINGIFY(
		  
		  void main(void)
		  {
		  gl_TexCoord[0] = gl_MultiTexCoord0;
		  
		  gl_Position = ftransform();
		  }
		  
);

const char *shader_post_dof_12tap_f = 
STRINGIFY(
		  
		  uniform sampler2D renderTargetTex;
		  uniform sampler2D positionBuffer;
		  uniform	sampler2D lumiBuffer;
		  uniform vec3 texel_ofs;
		  
		  void main(void)
		  {
		  float depth_test = texture2D(lumiBuffer, gl_TexCoord[0].xy).w;
		  
		  vec4 color = vec4(0.0,0.0,0.0,1.0);
		  
		  vec2 filterTaps[12];
		  
		  filterTaps[0] = vec2(-0.326212, -0.405805);
		  filterTaps[1] = vec2(-0.840144, -0.073580);
		  filterTaps[2] = vec2(-0.695914,  0.457137);
		  filterTaps[3] = vec2(-0.203345,  0.620716);
		  filterTaps[4] = vec2( 0.962340, -0.194983);
		  filterTaps[5] = vec2( 0.473434, -0.480026);
		  filterTaps[6] = vec2( 0.519456,  0.767022);
		  filterTaps[7] = vec2( 0.185461, -0.893124);
		  filterTaps[8] = vec2( 0.507431,  0.064425);
		  filterTaps[9] = vec2( 0.896420,  0.412458);
		  filterTaps[10] = vec2(-0.321940, -0.932615);
		  filterTaps[11] = vec2(-0.791559, -0.597705);
		  
		  int i;
		  
		  float far_depth = 0.1;
		  float near_depth = 0.05;
		  float depthSample = 1.0;
		  
		  vec2 radius = vec2(texel_ofs.x*10.0,texel_ofs.y*10.0);
		  float effect = 0.0;
		  vec2 ofsSample;
		  
		  //	  float colorDiv = 0.0;
		  
		  for (i = 0; i < 12; i++)
		  {
			  depthSample = texture2D( lumiBuffer, vec2(gl_TexCoord[0].x+filterTaps[i].x*radius.x*effect,gl_TexCoord[0].y+filterTaps[i].y*radius.y*effect)).w; 
			  effect = 0.0;
			  
			  if (depth_test > far_depth || depthSample > far_depth)
			  {
				  effect = (depthSample > depth_test)?((depthSample-far_depth)/(1.0-far_depth)):((depth_test-far_depth)/(1.0-far_depth));	// far		  
			  }
			  else if (depth_test < near_depth || depthSample < near_depth)
			  {
				  effect = (depthSample < depth_test)?(1.0-1.0/(near_depth/depthSample)):(1.0-1.0/(near_depth/depth_test)); // near
			  }
			  else if ((depth_test < near_depth && depthSample > far_depth)||(dgf && depthSample < near_depth))
			  {
				  effect = (depth_test < near_depth && depthSample > far_depth)?(1.0-1.0/(near_depth/depth_test)): //near
				  ((depth_test-far_depth)/(1.0-far_depth));	// far		  
			  }
		  
		  
		  ofsSample = vec2(gl_TexCoord[0].x+filterTaps[i].x*radius.x*effect,gl_TexCoord[0].y+filterTaps[i].y*radius.y*effect);
		  
		  if (abs(ofsSample.x)>1.0 || abs(ofsSample.y)>1.0 || abs(ofsSample.x)<0.0 || abs(ofsSample.y)<0.0) ofsSample = gl_TexCoord[0].xy;
		  
		  color += texture2D( renderTargetTex, ofsSample);   
		  //		  color += texture2D( renderTargetTex, gl_TexCoord[0].xy); 		  
		  }
		  
		  color /= 12.0;
		  
		  gl_FragColor = vec4(color.rgb,1.0);
		  }
		  
);

/*
 
 uniform sampler2D renderTargetTex;
 uniform sampler2D positionBuffer;
 uniform	sampler2D lumiBuffer;
 uniform vec3 texel_ofs;
 
 void main(void)
 {
 float depth_test = texture2D(lumiBuffer, gl_TexCoord[0].xy).w;
 
 vec4 color = vec4(0.0,0.0,0.0,1.0);
 
 vec2 filterTaps[12];
 
 filterTaps[0] = vec2(-0.326212, -0.405805);
 filterTaps[1] = vec2(-0.840144, -0.073580);
 filterTaps[2] = vec2(-0.695914,  0.457137);
 filterTaps[3] = vec2(-0.203345,  0.620716);
 filterTaps[4] = vec2( 0.962340, -0.194983);
 filterTaps[5] = vec2( 0.473434, -0.480026);
 filterTaps[6] = vec2( 0.519456,  0.767022);
 filterTaps[7] = vec2( 0.185461, -0.893124);
 filterTaps[8] = vec2( 0.507431,  0.064425);
 filterTaps[9] = vec2( 0.896420,  0.412458);
 filterTaps[10] = vec2(-0.321940, -0.932615);
 filterTaps[11] = vec2(-0.791559, -0.597705);
 
 int i;
 
 float far_depth = 0.1;
 float near_depth = 0.05;
 float depthSample = 1.0;
 
 vec2 radius = vec2(texel_ofs.x*10.0,texel_ofs.y*10.0);
 float effect = 0.0;
 vec2 ofsSample;
 
 //	  float colorDiv = 0.0;
 
 for (i = 0; i < 12; i++)
 {
 depthSample = texture2D( lumiBuffer, vec2(gl_TexCoord[0].x+filterTaps[i].x*radius.x*effect,gl_TexCoord[0].y+filterTaps[i].y*radius.y*effect)).w; 
 effect = 0.0;
 
 if (depth_test > far_depth || depthSample > far_depth)
 {
 effect = (depthSample > depth_test)?((depthSample-far_depth)/(1.0-far_depth)):((depth_test-far_depth)/(1.0-far_depth));	// far		  
 }
 else if (depth_test < near_depth || depthSample < near_depth)
 {
 effect = (depthSample < depth_test)?(1.0-1.0/(near_depth/depthSample)):(1.0-1.0/(near_depth/depth_test)); // near
 }
 else if ((depth_test < near_depth && depthSample > far_depth)||(depth_test > far_depth && depthSample < near_depth))
 {
 effect = (depth_test < near_depth && depthSample > far_depth)?(1.0-1.0/(near_depth/depth_test)): //near
 ((depth_test-far_depth)/(1.0-far_depth));	// far		  
 }
 
 
 ofsSample = vec2(gl_TexCoord[0].x+filterTaps[i].x*radius.x*effect,gl_TexCoord[0].y+filterTaps[i].y*radius.y*effect);
 
 if (abs(ofsSample.x)>1.0 || abs(ofsSample.y)>1.0 || abs(ofsSample.x)<0.0 || abs(ofsSample.y)<0.0) ofsSample = gl_TexCoord[0].xy;
 
 color += texture2D( renderTargetTex, ofsSample);   
 //		  color += texture2D( renderTargetTex, gl_TexCoord[0].xy); 		  
 }
 
 color /= 12.0;
 
 gl_FragColor = vec4(color.rgb,1.0);
 }

 */


const char *shader_post_msaa4_v = 
STRINGIFY(
		  
		  void main(void)
		  {
			  gl_TexCoord[0] = gl_MultiTexCoord0;
		  
			  gl_Position = ftransform();
		  }
		  
);

const char *shader_post_msaa4_f = 
STRINGIFY(
		  
		  uniform sampler2D renderTargetTex;
		  uniform sampler2D positionBuffer;
		  uniform sampler2D lumiBuffer;
		  uniform vec3 texel_ofs;
		  
		  void main(void)
		  {
			  float depth_test = texture2D(lumiBuffer, gl_TexCoord[0].xy).w;
			  
			  vec4 color = vec4(texture2D(renderTargetTex, gl_TexCoord[0].xy).rgb,1.0);
			  
			  float depthSample;
			  vec2 ofsSample;

			  ofsSample = vec2(texel_ofs.x,texel_ofs.y);
			  depthSample = texture2D( lumiBuffer, gl_TexCoord[0].xy+ofsSample*2.0).w;
			  if (abs(depth_test-depthSample) > 0.001) color = mix(color,texture2D( renderTargetTex, gl_TexCoord[0].xy+ofsSample*0.25),0.5);   

			  ofsSample = vec2(-texel_ofs.x,-texel_ofs.y);
			  depthSample = texture2D( lumiBuffer, gl_TexCoord[0].xy+ofsSample*2.0).w;
			  if (abs(depth_test-depthSample) > 0.001) color = mix(color,texture2D( renderTargetTex, gl_TexCoord[0].xy+ofsSample*0.25),0.5);   

			  ofsSample = vec2(texel_ofs.x,-texel_ofs.y);
			  depthSample = texture2D( lumiBuffer, gl_TexCoord[0].xy+ofsSample*2.0).w;
			  if (abs(depth_test-depthSample) > 0.001) color = mix(color,texture2D( renderTargetTex, gl_TexCoord[0].xy+ofsSample*0.25),0.5);   

			  ofsSample = vec2(-texel_ofs.x,texel_ofs.y);
			  depthSample = texture2D( lumiBuffer, gl_TexCoord[0].xy+ofsSample*2.0).w;
			  if (abs(depth_test-depthSample) > 0.001) color = mix(color,texture2D( renderTargetTex, gl_TexCoord[0].xy+ofsSample*0.25),0.5);   

			  gl_FragColor = vec4(color.rgb,1.0);
		  }
		  
);



const char *shader_post_hdr_12tap_v = 
STRINGIFY(
		  
//		  varying vec2 filterTaps[12];
		  
		  void main(void)
		  {		  
//			  filterTaps[0] = vec2(-0.326212, -0.405805);
//			  filterTaps[1] = vec2(-0.840144, -0.073580);
//			  filterTaps[2] = vec2(-0.695914,  0.457137);
//			  filterTaps[3] = vec2(-0.203345,  0.620716);
//			  filterTaps[4] = vec2( 0.962340, -0.194983);
//			  filterTaps[5] = vec2( 0.473434, -0.480026);
//			  filterTaps[6] = vec2( 0.519456,  0.767022);
//			  filterTaps[7] = vec2( 0.185461, -0.893124);
//			  filterTaps[8] = vec2( 0.507431,  0.064425);
//			  filterTaps[9] = vec2( 0.896420,  0.412458);
//			  filterTaps[10] = vec2(-0.321940, -0.932615);
//			  filterTaps[11] = vec2(-0.791559, -0.597705);
		  
			  gl_TexCoord[0] = gl_MultiTexCoord0;
			  
			  gl_Position = ftransform();
		  }
		  
);

const char *shader_post_hdr_12tap_f = 
STRINGIFY(
		  
		  uniform sampler2D renderTargetTex;
		  uniform sampler2D positionBuffer;
		  uniform sampler2D lumiBuffer;
		  uniform vec3 texel_ofs;
		  
//		  varying vec2 filterTaps[12];
		  
		  vec3 rangeValHDR(vec3 src)
		  {
			  return (src.r>1.0||src.g>1.0||src.b>1.0)?(src):vec3(0.0,0.0,0.0);		  
		  }
		  
		  vec4 hdrSample()
		  {
//			  float rad = texel_ofs.x;
		       
			  vec3 sample;
			
			  vec3 color = texture2D(renderTargetTex, gl_TexCoord[0].xy).rgb;
			
			  vec3 accum;

   			  float rad = 1.0;

		      accum = rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*rad, gl_TexCoord[0].t)).rgb);
			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t+texel_ofs.y*rad)).rgb);
			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s-texel_ofs.x*rad, gl_TexCoord[0].t)).rgb);
			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t-texel_ofs.y*rad)).rgb);
			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*rad, gl_TexCoord[0].t+texel_ofs.y*rad)).rgb);
			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s-texel_ofs.x*rad, gl_TexCoord[0].t-texel_ofs.y*rad)).rgb);
			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*rad, gl_TexCoord[0].t-texel_ofs.y*rad)).rgb);
			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s-texel_ofs.x*rad, gl_TexCoord[0].t+texel_ofs.y*rad)).rgb);

		  
//			  rad += 1.0;
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*rad, gl_TexCoord[0].t)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t+texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s-texel_ofs.x*rad, gl_TexCoord[0].t)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t-texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*rad, gl_TexCoord[0].t+texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s-texel_ofs.x*rad, gl_TexCoord[0].t-texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*rad, gl_TexCoord[0].t-texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s-texel_ofs.x*rad, gl_TexCoord[0].t+texel_ofs.y*rad)).rgb);

//			  rad += 2.0;
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*rad, gl_TexCoord[0].t)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t+texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s-texel_ofs.x*rad, gl_TexCoord[0].t)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t-texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*rad, gl_TexCoord[0].t+texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s-texel_ofs.x*rad, gl_TexCoord[0].t-texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*rad, gl_TexCoord[0].t-texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s-texel_ofs.x*rad, gl_TexCoord[0].t+texel_ofs.y*rad)).rgb);
//
//		  
//			  rad += 3.0;
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*rad, gl_TexCoord[0].t)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t+texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s-texel_ofs.x*rad, gl_TexCoord[0].t)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t-texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*rad, gl_TexCoord[0].t+texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s-texel_ofs.x*rad, gl_TexCoord[0].t-texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*rad, gl_TexCoord[0].t-texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s-texel_ofs.x*rad, gl_TexCoord[0].t+texel_ofs.y*rad)).rgb);

		  
		  
//			  accum = rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+filterTaps[0].s*texel_ofs.x*rad, gl_TexCoord[0].t+filterTaps[0].t*texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+filterTaps[1].s*texel_ofs.x*rad, gl_TexCoord[0].t+filterTaps[1].t*texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+filterTaps[2].s*texel_ofs.x*rad, gl_TexCoord[0].t+filterTaps[2].t*texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+filterTaps[3].s*texel_ofs.x*rad, gl_TexCoord[0].t+filterTaps[3].t*texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+filterTaps[4].s*texel_ofs.x*rad, gl_TexCoord[0].t+filterTaps[4].t*texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+filterTaps[5].s*texel_ofs.x*rad, gl_TexCoord[0].t+filterTaps[5].t*texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+filterTaps[6].s*texel_ofs.x*rad, gl_TexCoord[0].t+filterTaps[6].t*texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+filterTaps[7].s*texel_ofs.x*rad, gl_TexCoord[0].t+filterTaps[7].t*texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+filterTaps[8].s*texel_ofs.x*rad, gl_TexCoord[0].t+filterTaps[8].t*texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+filterTaps[9].s*texel_ofs.x*rad, gl_TexCoord[0].t+filterTaps[9].t*texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+filterTaps[10].s*texel_ofs.x*rad, gl_TexCoord[0].t+filterTaps[10].t*texel_ofs.y*rad)).rgb);
//			  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+filterTaps[11].s*texel_ofs.x*rad, gl_TexCoord[0].t+filterTaps[11].t*texel_ofs.y*rad)).rgb);
			  
			  accum /= 8.0;
		  
			  return vec4(color+accum,1.0);
		  }
		  
		  void main(void)
		  {
		  float depth_test = texture2D(lumiBuffer, gl_TexCoord[0].xy).w;
		  
//		  vec4 color = vec4(texture2D(renderTargetTex, gl_TexCoord[0].xy).rgb,1.0);
		  vec4 color;
		  
//		  if (color.r>1.0||color.b>1.0||color.g>1.0)
//		  {
//			  color.rgb = vec3(1.0,0.0,0.0);
//		  }

		  color = hdrSample();
		  
//		  float depthSample;
//		  vec2 ofsSample;
//		  
//		  ofsSample = vec2(texel_ofs.x,texel_ofs.y);
//		  depthSample = texture2D( lumiBuffer, gl_TexCoord[0].xy+ofsSample*2.0).w;
//		  if (abs(depth_test-depthSample) > 0.001) color = mix(color,texture2D( renderTargetTex, gl_TexCoord[0].xy+ofsSample*0.25),0.5);   
//		  
//		  ofsSample = vec2(-texel_ofs.x,-texel_ofs.y);
//		  depthSample = texture2D( lumiBuffer, gl_TexCoord[0].xy+ofsSample*2.0).w;
//		  if (abs(depth_test-depthSample) > 0.001) color = mix(color,texture2D( renderTargetTex, gl_TexCoord[0].xy+ofsSample*0.25),0.5);   
//		  
//		  ofsSample = vec2(texel_ofs.x,-texel_ofs.y);
//		  depthSample = texture2D( lumiBuffer, gl_TexCoord[0].xy+ofsSample*2.0).w;
//		  if (abs(depth_test-depthSample) > 0.001) color = mix(color,texture2D( renderTargetTex, gl_TexCoord[0].xy+ofsSample*0.25),0.5);   
//		  
//		  ofsSample = vec2(-texel_ofs.x,texel_ofs.y);
//		  depthSample = texture2D( lumiBuffer, gl_TexCoord[0].xy+ofsSample*2.0).w;
//		  if (abs(depth_test-depthSample) > 0.001) color = mix(color,texture2D( renderTargetTex, gl_TexCoord[0].xy+ofsSample*0.25),0.5);   
		  
		  gl_FragColor = color;
		  }
		  
);



const char *shader_post_hdr_12tap_halfbuffer_v = 
STRINGIFY(
		  
//		   varying vec2 filterTaps[12];
		  
		  void main(void)
		  {		  
//			  filterTaps[0] = vec2(-0.326212, -0.405805);
//			  filterTaps[1] = vec2(-0.840144, -0.073580);
//			  filterTaps[2] = vec2(-0.695914,  0.457137);
//			  filterTaps[3] = vec2(-0.203345,  0.620716);
//			  filterTaps[4] = vec2( 0.962340, -0.194983);
//			  filterTaps[5] = vec2( 0.473434, -0.480026);
//			  filterTaps[6] = vec2( 0.519456,  0.767022);
//			  filterTaps[7] = vec2( 0.185461, -0.893124);
//			  filterTaps[8] = vec2( 0.507431,  0.064425);
//			  filterTaps[9] = vec2( 0.896420,  0.412458);
//			  filterTaps[10] = vec2(-0.321940, -0.932615);
//			  filterTaps[11] = vec2(-0.791559, -0.597705);
		  
		  gl_TexCoord[0] = gl_MultiTexCoord0;
		  
		  gl_Position = ftransform();
		  }
		  
);

const char *shader_post_hdr_12tap_halfbuffer_f = 
STRINGIFY(
		  
		  uniform sampler2D renderTargetTex;
		  uniform sampler2D positionBuffer;
		  uniform sampler2D lumiBuffer;
		  uniform vec3 texel_ofs;
		  
//		  varying vec2 filterTaps[12];
		  
		  vec3 rangeValHDR(vec3 src)
		  {
			return (src.r>1.0||src.g>1.0||src.b>1.0)?(src):vec3(0.0,0.0,0.0);		  
		  }
		  
		  vec4 hdrSample(float rad)
		  {
		  //			  float rad = texel_ofs.x;
		  
//		  vec3 sample;
		  
//		  vec3 color = texture2D(renderTargetTex, gl_TexCoord[0].xy).rgb;
		  
		  vec3 accum;

//		  accum = rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*filterTaps[0].x*rad, gl_TexCoord[0].t+texel_ofs.y*filterTaps[0].y*rad)).rgb);
//		  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*filterTaps[1].x*rad, gl_TexCoord[0].t+texel_ofs.y*filterTaps[1].y*rad)).rgb);
//		  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*filterTaps[2].x*rad, gl_TexCoord[0].t+texel_ofs.y*filterTaps[2].y*rad)).rgb);
//		  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*filterTaps[3].x*rad, gl_TexCoord[0].t+texel_ofs.y*filterTaps[3].y*rad)).rgb);
//		  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*filterTaps[4].x*rad, gl_TexCoord[0].t+texel_ofs.y*filterTaps[4].y*rad)).rgb);
//		  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*filterTaps[5].x*rad, gl_TexCoord[0].t+texel_ofs.y*filterTaps[5].y*rad)).rgb);
//		  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*filterTaps[6].x*rad, gl_TexCoord[0].t+texel_ofs.y*filterTaps[6].y*rad)).rgb);
//		  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*filterTaps[7].x*rad, gl_TexCoord[0].t+texel_ofs.y*filterTaps[7].y*rad)).rgb);
//		  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*filterTaps[8].x*rad, gl_TexCoord[0].t+texel_ofs.y*filterTaps[8].y*rad)).rgb);
//		  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*filterTaps[9].x*rad, gl_TexCoord[0].t+texel_ofs.y*filterTaps[9].y*rad)).rgb);
//		  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*filterTaps[10].x*rad, gl_TexCoord[0].t+texel_ofs.y*filterTaps[10].y*rad)).rgb);
//		  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*filterTaps[11].x*rad, gl_TexCoord[0].t+texel_ofs.y*filterTaps[11].y*rad)).rgb);
		  
		  float radb = rad*0.707106781;
		  
		  accum = rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*rad, gl_TexCoord[0].t)).rgb);
		  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t+texel_ofs.y*rad)).rgb);
		  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s-texel_ofs.x*rad, gl_TexCoord[0].t)).rgb);
		  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s, gl_TexCoord[0].t-texel_ofs.y*rad)).rgb);
		  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*radb, gl_TexCoord[0].t+texel_ofs.y*radb)).rgb);
		  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s-texel_ofs.x*radb, gl_TexCoord[0].t-texel_ofs.y*radb)).rgb);
		  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s+texel_ofs.x*radb, gl_TexCoord[0].t-texel_ofs.y*radb)).rgb);
		  accum += rangeValHDR(texture2D(renderTargetTex, vec2(gl_TexCoord[0].s-texel_ofs.x*radb, gl_TexCoord[0].t+texel_ofs.y*radb)).rgb);
//		  
//		  
			accum /= 8.0;
		  
		  return vec4(accum,1.0); //vec4(color+accum,1.0);
		  }
		  
		  void main(void)
		  {
			  vec4 color;
		
			  color = hdrSample(4.0);
			  color += hdrSample(8.0);
//			  color += hdrSample(6.0);
			  
			  gl_FragColor = color/2.0;
		  }
		  
);



const char *shader_post_dof_6tap_v = 
STRINGIFY(

		  varying vec2 filterTaps[6];
		  void main(void)
		  {
		  gl_TexCoord[0] = gl_MultiTexCoord0;
		  
		  
		  filterTaps[0] = vec2(-0.326212, -0.405805);
		  filterTaps[1] = vec2(-0.840144, -0.073580);
		  filterTaps[2] = vec2(-0.695914,  0.457137);
		  filterTaps[3] = vec2(-0.203345,  0.620716);
		  filterTaps[4] = vec2( 0.962340, -0.194983);
		  filterTaps[5] = vec2( 0.473434, -0.480026);
		  
		  gl_Position = ftransform();
		  }
		  
);

const char *shader_post_dof_6tap_f = 
STRINGIFY(
		  varying vec2 filterTaps[6];
		  uniform sampler2D renderTargetTex;
		  uniform sampler2D positionBuffer;
		  uniform sampler2D lumiBuffer;
		  uniform vec3 texel_ofs;
		  
		  uniform float near_depth;
		  uniform float far_depth;
		  
		  void main(void)
		  {
		  float depth_test = texture2D(lumiBuffer, gl_TexCoord[0].xy).w;
		  
		  vec4 color = vec4(0.0,0.0,0.0,1.0);
		  
		  
		  int i;
		  
//		  float far_depth = 0.1;
//		  float near_depth = 0.05;
		  float depthSample = 1.0;
		  
		  vec2 radius = vec2(texel_ofs.x*10.0,texel_ofs.y*10.0);
		  float effect = 0.0;
		  vec2 ofsSample;
		  
		  //	  float colorDiv = 0.0;
		  
		  bool dln = depth_test < near_depth;
		  bool dgf = depth_test > far_depth;
		  		  
		  for (i = 0; i < 6; i++)
		  {
		  depthSample = texture2D( lumiBuffer, vec2(gl_TexCoord[0].x+filterTaps[i].x*radius.x*effect,gl_TexCoord[0].y+filterTaps[i].y*radius.y*effect)).w; 
		  effect = 0.0;

		  bool ds_gf = depthSample > far_depth;
		  bool ds_ln = depthSample < near_depth;

		  if (dln || ds_gf)
		  {
			effect = (depthSample > depth_test)?((depthSample-far_depth)/(1.0-far_depth)):((depth_test-far_depth)/(1.0-far_depth));	// far		  
		  }
		  else if (dln || ds_ln)
		  {
			effect = (depthSample < depth_test)?(1.0-1.0/(near_depth/depthSample)):(1.0-1.0/(near_depth/depth_test)); // near
		  }
		  else if ((dln && ds_gf)||(dln && ds_ln))
		  {
			  effect = (dln && ds_gf)?(1.0-1.0/(near_depth/depth_test)): //near
			  ((depth_test-far_depth)/(1.0-far_depth));	// far		  
		  }
		  
		  
		  ofsSample = vec2(gl_TexCoord[0].x+filterTaps[i].x*radius.x*effect,gl_TexCoord[0].y+filterTaps[i].y*radius.y*effect);
		  
		  if (abs(ofsSample.x)>1.0 || abs(ofsSample.y)>1.0 || abs(ofsSample.x)<0.0 || abs(ofsSample.y)<0.0) ofsSample = gl_TexCoord[0].xy;
		  
		  color += texture2D( renderTargetTex, ofsSample);   
		  //		  color += texture2D( renderTargetTex, gl_TexCoord[0].xy); 		  
		  }
		  
		  color /= 6.0;
		  
		  gl_FragColor = vec4(color.rgb,1.0);
		  }
		  
);



#endif






Scene::Scene() : screen_w(512), screen_h(512), cam(NULL), sectorMap(NULL), initialized(false) 
#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)
,hdr_enabled(false), hdr_buffer_open(false), hdrProcess(512, 512), deferred_shading(false), renderBuffer(NULL), render_buffer_open(false)
#endif
{
//	setRenderDimensions(512, 512);
	memset(&debug,0,sizeof(SceneDebugInfo));
#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)
	memset(&postProcessConfig,0,sizeof(PostProcessInfo));
#endif
};


Scene::Scene(int scr_width, int scr_height) : cam(NULL), sectorMap(NULL), initialized(false)
#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)
, hdr_enabled(false), hdrProcess(scr_width, scr_height), hdr_buffer_open(false), deferred_shading(false), renderBuffer(NULL), render_buffer_open(false), blurAmt(0.0)
#endif
{
	setRenderDimensions(scr_width, scr_height);	
	memset(&debug,0,sizeof(SceneDebugInfo));
#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)
	memset(&postProcessConfig,0,sizeof(PostProcessInfo));
#endif
};

Scene::~Scene()
{
};


#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)

void Scene::setHDR(bool hdr_mode, int filter_levels)
{
	hdr_enabled = hdr_mode;
	hdrProcess.setFilterLevel(filter_levels);
	hdrProcess.init();
}

void Scene::setBlur(float amt)
{
//	hdrProcess.setBlurLevel(blur_levels);
	blurAmt = amt;
}


void Scene::openRenderBuffer()
{
	if (!initialized)
	{
		blendBuffer->bindRenderTarget();
		glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	}
	
	if (hdr_enabled && !hdr_buffer_open) 
	{
		hdrProcess.drawOpen();
		hdrProcess.drawClear();
		hdr_buffer_open = true;
	}
	
	if (!render_buffer_open && deferred_shading) 
	{
		renderBufferTarget[0]->bindRenderTarget();
		glClearDepth(1.0);
		glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
		render_buffer_open = true;
	}
	else if (deferred_shading)
	{
		renderBufferTarget[0]->bindRenderTarget();
	}
	
	
}

void Scene::closeRenderBuffer()
{
//	if (!initialized) return;
	if (hdr_enabled && hdr_buffer_open)
	{
		hdrProcess.drawEnd();
	}
		
	if (deferred_shading) renderPostProcess();
	
	render_buffer_open = false;
	hdr_buffer_open = false;
}

void Scene::clearRenderBuffer()
{
	if (!initialized) return;
	if (!hdr_enabled) return;
	
	if (!hdr_buffer_open) hdrProcess.drawOpen();
	if (hdr_enabled) hdrProcess.drawClear();	
	hdrProcess.drawClose();
	hdr_buffer_open = false;
}


void Scene::setDeferredShading(bool deferred_mode)
{
	unsigned int renderTexId;
	
	if (!deferred_shading)
	{
		renderTexId = Texture::create();
		renderBuffer = Texture::textures[renderTexId];
		renderBuffer->createRenderTarget(screen_w, screen_h, true, 2, false, false, 4);

		renderTexId = Texture::create();
		renderBufferTarget[0] = Texture::textures[renderTexId];
		renderBufferTarget[0]->createRenderTarget(screen_w, screen_h, false, 2, true, false, 1);		
		
		renderTexId = Texture::create();
		renderBufferTarget[1] = Texture::textures[renderTexId];
		renderBufferTarget[1]->createRenderTarget(screen_w, screen_h, false, 2, true, false, 1);		

		renderTexId = Texture::create();
		halfRenderBufferTarget[0] = Texture::textures[renderTexId];
		halfRenderBufferTarget[0]->createRenderTarget(screen_w/HALFBUFFER_DIV, screen_h/HALFBUFFER_DIV, false, 2, true, false, 1);		

		renderTexId = Texture::create();
		halfRenderBufferTarget[1] = Texture::textures[renderTexId];
		halfRenderBufferTarget[1]->createRenderTarget(screen_w/HALFBUFFER_DIV, screen_h/HALFBUFFER_DIV, false, 2, true, false, 1);		
		
		renderTexId = Texture::create();
		blendBuffer = Texture::textures[renderTexId];
		blendBuffer->createRenderTarget(screen_w, screen_h, false, 2, true, false, 1);
		
		deferredSphereMat = new Material();
		deferredSphereMat->setMaxSmooth(60);
		
		deferred_sphere = new ProceduralObject();
		deferred_sphere->generateSphere(1.06f,10,10,deferredSphereMat);
		deferred_sphere->cache(true);
		
		deferredSphereMat->color = RGBA(0.8f,0.8f,0.0f,1.0f);		
		
		Logger::log("copy shader setup...\n");
		postProcessCopy.setup(shader_post_copy_v,shader_post_copy_f);
		postProcessDOF.setup(shader_post_dof_6tap_v,shader_post_dof_6tap_f);
		postProcessMSAA.setup(shader_post_msaa4_v,shader_post_msaa4_f);
		postProcessSSAO.setup(shader_post_ssao_v,shader_post_ssao_f);
		postProcessBlur.setup(shader_post_blur_v,shader_post_blur_f);
		postProcessHDR.setup(shader_post_hdr_12tap_halfbuffer_v,shader_post_hdr_12tap_halfbuffer_f);
	}
	
	deferred_shading = deferred_mode;	
}

#endif

void Scene::initSectorMap(const XYZ &aabb1, const XYZ &aabb2, double sector_size)
{
	sectorMap = new SectorMap(aabb1,aabb2,sector_size);
	
	std::set<Light *>::iterator lights_i;
	
	for (lights_i = lights_bound.begin(); lights_i != lights_bound.end(); lights_i++)
	{
		sectorMap->bind(*lights_i);
	}
}

void Scene::clear()
{
	std::map<std::string, SceneObject*, string_less> objs_bound_ref;
	std::map<std::string, SceneObject*, string_less> sceneobjs_bound_ref;
	std::map<std::string, SceneObject*, string_less> cams_bound_ref;
	std::map<std::string, SceneObject*, string_less> lights_bound_ref;
	std::map<std::string, SceneObject*, string_less> motions_bound_ref;
	
	objs_bound_ref.clear();
	sceneobjs_bound_ref.clear();
	cams_bound_ref.clear();
	motions_bound_ref.clear();
	lights_bound_ref.clear();
	sceneobjs_bound.clear();
	cams_bound.clear();
	lights_bound.clear();
	motions_bound.clear();
}

void Scene::setRenderDimensions(int scr_width, int scr_height)
{
	screen_w = scr_width; 
	screen_h = scr_height;

	if (cam) 
	{
		cam->setAspect(scr_width,scr_height);
		cam->setSize(scr_width,scr_height);
	}

#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)	
	if (initialized && hdr_enabled) hdrProcess.setViewSize(scr_width,scr_height);
	if (initialized && deferred_shading) 
	{
		renderBuffer->destroyRenderTarget();
		renderBuffer->createRenderTarget(screen_w, screen_h, true, true, false, false, 4);
	}
#endif
};


int Scene::getWidth()
{
	return screen_w;
};

int Scene::getHeight()
{
	return screen_h;
};



void Scene::bind(SceneObject &sceneObj_in)
{
	XYZ bb1,bb2;
//	sceneObj_in.transformBegin(false,false,true);
//	sceneObj_in.movedReset();
//	sceneObj_in.calcAABB(bb1,bb2);
//	sceneObj_in.clearTransform();
//	sceneObj_in.movedReset();

//	rigid_sceneObj_in.evaluate();
	//	rigid_sceneObj_in.transformBegin(false,false,true);
	sceneObj_in.setMatrixLock(false);
	sceneObj_in.clearTransform();
	//	rigid_sceneObj_in.movedReset();
	sceneObj_in.calcAABB();
	
	
	std::string objName = sceneObj_in.getId();
	
	if (sceneObj_in.hasVisibility)
	{
		sceneobjs_visibility.insert(&sceneObj_in);
	}
	else
	{
		active_sceneobjs.insert(&sceneObj_in);
		sceneobjs_bound.insert(&sceneObj_in);

		if (objName != "" && objName != "null")
		{
			sceneobjs_bound_ref[objName] = &sceneObj_in;
		}

		if (sectorMap) 
		{
			sectorMap->bind(&sceneObj_in);
		}
	}
	
};

void Scene::unbind(SceneObject &sceneObj_in)
{
	
	std::string objName = sceneObj_in.getId();
	
	if (sceneObj_in.hasVisibility)
	{
		sceneobjs_visibility.erase(&sceneObj_in);
	}
	else
	{
		active_sceneobjs.erase(&sceneObj_in);
		sceneobjs_bound.erase(&sceneObj_in);
		
		if (objName != "" && objName != "null")
		{
			sceneobjs_bound_ref.erase(objName);
		}
		
		if (sectorMap) sectorMap->unbind(&sceneObj_in);
	}
	
};


string Scene::nextSceneObjectName(string baseName)
{	
	int testVal = 1;
	
	bool found = false;
	
	string newId;
	
	while (!found)
	{
		std::ostringstream o;
		
		if (testVal < 10) o << "_000";
		else if (testVal < 100) o << "_00";
		else if (testVal < 1000) o << "_0";
		else o << "_";
		
		o << testVal;
		
		newId = baseName+o.str();
		
		testVal++;
		
		if (testVal > 5000) break;	// seems like a reasonable limit :P
		
		if (sceneobjs_bound_ref.find(newId) == sceneobjs_bound_ref.end()) { found=true; break; }
	}	
	
	return newId;
}

void Scene::bind(Motion &motion_in)
{
	string idName = motion_in.getId();
	
	motions_bound.insert(&motion_in);	
	
	if (idName != "" && idName != "null")
	{
		motions_bound_ref[idName] = &motion_in;
	}
};


void Scene::bind(Light &light_in)
{
	string idName = light_in.getId();
	
	active_lights.insert(&light_in);	
	lights_bound.insert(&light_in);	
	
	if (idName != "" && idName != "null")
	{
		lights_bound_ref[idName] = &light_in;
	}
	
	if (sectorMap) 
	{
		if (light_in.type == LIGHT_SPOT)
		{
			if (light_in.hasShadow())
			{			
				light_in.shadowInit();
				light_in.shadowBegin();
				sectorMap->bind(&light_in);
				light_in.shadowEnd();
			}
			else
			{
				sectorMap->bind(&light_in);	
			}
		}
		else if (light_in.type == LIGHT_POINT)
		{
			sectorMap->bind(&light_in);
		}
		else if (light_in.type == LIGHT_DIRECTIONAL)
		{
			sectorMap->bind(&light_in);
		}
		else if (light_in.type == LIGHT_AREA)
		{
			if (light_in.hasShadow())
			{			
//				light_in.shadowInit();
//				light_in.shadowBegin();
				sectorMap->bind(&light_in);
//				light_in.shadowEnd();
			}
			else
			{
				sectorMap->bind(&light_in);
			}
		}
		
		
	}
};


void Scene::bind(Camera &cam_in)
{
	string idName = cam_in.getId();
	
	cam = &cam_in;
	cams_bound.insert(&cam_in);
	
	if (idName != "" && idName != "null")
	{
		cams_bound_ref[idName] = &cam_in;
	}
};


void Scene::unbind(Motion &motion_in)
{
	string idName = motion_in.getId();
	
	motions_bound.erase(&motion_in);	
	
	if (idName != "" && idName != "null")
	{
		motions_bound_ref.erase(idName);
	}
};


void Scene::unbind(Light &light_in)
{
	string idName = light_in.getId();
	
	active_lights.erase(&light_in);	
	lights_bound.erase(&light_in);	
	
	if (idName != "" && idName != "null")
	{
		lights_bound_ref.erase(idName);
	}
};


void Scene::unbind(Camera &cam_in)
{
	string idName = cam_in.getId();
	
	if (cam == &cam_in) cam = NULL;
	
	cams_bound.erase(&cam_in);
	
	if (idName != "" && idName != "null")
	{
		cams_bound_ref.erase(idName);
	}
};


void Scene::process()
{
	std::set<SceneObject *>::iterator obj_i;
	
	XYZ tmp,tmp2;
	
	for (obj_i = sceneobjs_bound.begin(); obj_i != sceneobjs_bound.end(); obj_i++)
	{
		if (!(*obj_i)->active) continue;
		if ((*obj_i)->hasParent()) continue;

		(*obj_i)->calcAABB();
	}

}

void Scene::renderStage(Shader &renderer, int stage)
{
	std::vector<Light *> tmp;
	std::set<SceneObject *>::iterator obj_i;
//	Logger::log("%d\n",active_sceneobjs.size());
#ifndef ARCH_PSP
	CacheShader::lockObj = NULL;
#endif
	
	switch (stage)
	{
		case SHADER_STAGE_SHADOW:

			for (obj_i = active_sceneobjs.begin(); obj_i != active_sceneobjs.end(); obj_i++)
			{
				if (!(*obj_i)->active) continue;
				if (!(*obj_i)->hasParent() && (*obj_i)->shadowCast() && (*obj_i)->visible()) (*obj_i)->render(renderer,stage,false);		
			}
		break;
		default:
			for (obj_i = active_sceneobjs.begin(); obj_i != active_sceneobjs.end(); obj_i++)
			{
				if (!(*obj_i)->active) continue;
/*				if ((*obj_i)->dynamic_colors)
				{
					(*obj_i)->calcVertexLightMap(renderer);
				}
*/
				if (!(*obj_i)->hasParent() && (*obj_i)->visible()) (*obj_i)->render(renderer,stage,true);

#if !defined(ARCH_PSP) && !defined(OPENGL_ES)
				if (debug.aabb)
				{
					glDisable(GL_LIGHTING);
					glDisable(GL_TEXTURE_2D);
					GLShader::clear();

					Pencil::drawBB((*obj_i)->aabbMin,(*obj_i)->aabbMax);
				}
#endif
			}
		break;
	}
}


#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)

bool Scene::renderShadowVolumes()
{	
//	std::set<Light *> tmp;
//	oShader.bind(&tmp);
	bool has_shadow_lights = false;
	std::set<SceneObject *>::iterator obj_i;

	
//	if (!deferred_shading) 	glPolygonOffset (2,0);
	GLShader::clear();

	glDisable(GL_LIGHTING);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);		
	glColorMask(0, 0, 0, 0);

	
	if (deferred_shading) 
	{
//		glDisable(GL_CULL_FACE);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);	

		glEnable(GL_POLYGON_OFFSET_FILL);

		glPolygonOffset (2.0, 4.0);		
//		glDepthRange(-0.001,0.999);
	}
	else
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);	
		
		
	}
	
	for (active_light_i = active_lights.begin(); active_light_i != active_lights.end(); active_light_i++)
	{
		if ((*active_light_i)->hasShadow() && (*active_light_i)->type == LIGHT_SPOT)
		{
			has_shadow_lights = true;
			
			if (sectorMap) 
			{
				if (!sectorMap->lightActivity((*active_light_i)))
				{
//					(*active_light_i)->diffuse = RGB(1,1,1);
					continue;
				}
			}
//			(*active_light_i)->diffuse = RGB(0,0,5);

			
			if (sectorMap) 
			{
				shadow_sceneobjs.clear();
				
				(*active_light_i)->shadowBegin();
				
				sectorMap->queryFrustum(shadow_sceneobjs,(*active_light_i)->projectionMatrix,(*active_light_i)->viewMatrix,(*active_light_i)->nearclip,(*active_light_i)->farclip,(*active_light_i)->viewport);
			}
			else
			{
				(*active_light_i)->shadowBegin();
			}
			
			if (!shadow_sceneobjs.size() && sectorMap) 
			{
				(*active_light_i)->shadowEnd();
				continue;
			}
			else 
			if (!shadow_sceneobjs.size())
			{
				shadow_sceneobjs = active_sceneobjs;
			}
			
			if (sceneobjs_visibility.size())
			{
				std::set<SceneObject *>::iterator vis_obj_i;
				
				for (vis_obj_i = sceneobjs_visibility.begin(); vis_obj_i != sceneobjs_visibility.end(); vis_obj_i++)
				{
					if (!(*vis_obj_i)->shadowCast()) continue;
					spot_frustum.Update();
					
					(*vis_obj_i)->calcVisibility((*active_light_i)->position, spot_frustum);
					shadow_sceneobjs.insert((*vis_obj_i));
				}
			}
			
			
//			renderStage(sShader,SHADER_STAGE_SHADOW);
			
			//	Logger::log("%d\n",active_sceneobjs.size());
			
			CacheShader::lockObj = NULL;
			
			glColorMask(false,false,false,false);
			for (obj_i = shadow_sceneobjs.begin(); obj_i != shadow_sceneobjs.end(); obj_i++)
			{
				if (!(*obj_i)->active) continue;
				if (!(*obj_i)->hasParent() && (*obj_i)->shadowCast() && (*obj_i)->visible()) (*obj_i)->render(sShader,SHADER_STAGE_SHADOW,false);
			}
			glColorMask(true,true,true,true);
			
			(*active_light_i)->shadowEnd();
		} // Spotlights End
		else if ((*active_light_i)->hasShadow() && (*active_light_i)->type == LIGHT_AREA)
		{
			has_shadow_lights = true;
			AreaLight *pLight = (AreaLight *)(*active_light_i);
			
			pLight->update();
			
			for (int i = 0; i < 3; i++)
			{
				Light *aLight = &pLight->oLight[i];
				
				if (sectorMap) 
				{
					shadow_sceneobjs.clear();
					
					aLight->shadowBegin();
					
					sectorMap->queryOrthoFrustum(shadow_sceneobjs,aLight->position,aLight->ortho_size,aLight->ortho_size,aLight->projectionMatrix,aLight->viewMatrix,aLight->nearclip,aLight->farclip,aLight->viewport);
				}
				else
				{
					aLight->shadowBegin();
				}
				
				if (!shadow_sceneobjs.size() && sectorMap) 
				{
					aLight->shadowEnd();
					continue;
				}
				else 
					if (!shadow_sceneobjs.size())
					{
						shadow_sceneobjs = active_sceneobjs;
					}
				
				if (sceneobjs_visibility.size())
				{
					std::set<SceneObject *>::iterator vis_obj_i;
					
					for (vis_obj_i = sceneobjs_visibility.begin(); vis_obj_i != sceneobjs_visibility.end(); vis_obj_i++)
					{
						if (!(*vis_obj_i)->shadowCast()) continue;
						spot_frustum.Update();
						
						(*vis_obj_i)->calcVisibility(aLight->position, spot_frustum);
						shadow_sceneobjs.insert((*vis_obj_i));
					}
				}
				
				
				//			renderStage(sShader,SHADER_STAGE_SHADOW);
				
				//	Logger::log("%d\n",active_sceneobjs.size());
				
				CacheShader::lockObj = NULL;
				
				glColorMask(false,false,false,false);
				for (obj_i = shadow_sceneobjs.begin(); obj_i != shadow_sceneobjs.end(); obj_i++)
				{
					if (!(*obj_i)->active) continue;
					if (!(*obj_i)->hasParent() && (*obj_i)->shadowCast() && (*obj_i)->visible()) (*obj_i)->render(sShader,SHADER_STAGE_SHADOW,false);
				}
				glColorMask(true,true,true,true);
				
				aLight->shadowEnd();
			}
		}
	}	
	
//	if (!deferred_shading) 	glPolygonOffset (0,0);
	if (deferred_shading) 
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
		
//		glPolygonOffset (0.0, 0.0);
		
		glDepthRange(0.0,1.0);
	}
	else
	{
		glCullFace(GL_BACK);		
	}
	
	///	
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	
	
	return has_shadow_lights;
}
#endif

#if !defined(ARCH_PSP) && !defined(OPENGL_ES)
void Scene::renderDebug()
{
	if (debug.frustum && sectorMap)
	{
		
		Texture::clearAllTextures();
#if !defined(ARCH_DC)		
		GLShader::clear();
#endif		
		glDepthFunc(GL_ALWAYS);
		glDisable(GL_LIGHTING);
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		
		float modelView[16];

		if (cam) cam->setup();		

		glGetFloatv(GL_MODELVIEW_MATRIX,modelView);
				
		if (sectorMap && cam)
		{
			sectorMap->renderFrustum(cam);
			
//			int viewport[4] = { 0, 0, screen_w, screen_h };
//			sectorMap->renderFrustum(cam->projectionMatrix,cam->viewMatrix,cam->nearclip,cam->farclip,viewport);
		}
		
	}
}	
#endif


#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)

void Scene::renderPostProcess()
{
		renderBufferTarget[0]->resetRenderTarget();
		
		Texture::clearAllTextures();
		
		glDisable(GL_LIGHTING);
		glDepthFunc(GL_LEQUAL);
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_BLEND);
	
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
	
		int renderTarget=0;
		int numTargets=2;
	
		Texture::setTexture(0);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,renderBufferTarget[renderTarget]->renderSurface.texture);
		
		Texture::setTexture(1);		
		glBindTexture(GL_TEXTURE_2D, renderBuffer->renderSurface.attachments[1]);

		Texture::setTexture(2);		
		glBindTexture(GL_TEXTURE_2D, renderBuffer->renderSurface.attachments[2]);

		Texture::setTexture(3);		
		glBindTexture(GL_TEXTURE_2D, renderBuffer->renderSurface.attachments[0]);
	
		XYZ texel_ofs = XYZ(1.0f/(float)screen_w,1.0f/(float)screen_h,0);
		XYZ half_texel_ofs = XYZ(1.0f/(float)(screen_w*2),1.0f/(float)(screen_h*2),0);
		int processCount = (postProcessConfig.msaa?1:0)+(postProcessConfig.ssao?1:0)+(postProcessConfig.dof?1:0)+(postProcessConfig.hdr?1:0);
		bool doProcess = (processCount||blurAmt)?true:false;
	
		if (processCount > 1) 
		{
			renderTarget = (renderTarget+1)%numTargets;
			renderBufferTarget[renderTarget]->bindRenderTarget();
			glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
		}
	
		if (doProcess)
		{
			
			if (postProcessConfig.msaa)
			{
				if (processCount > 1) renderBufferTarget[renderTarget]->bindRenderTarget();

				postProcessMSAA.setShaderValue("renderTargetTex",(unsigned int)0);
				postProcessMSAA.setShaderValue("positionBuffer", (unsigned int)1); 
				postProcessMSAA.setShaderValue("lumiBuffer", (unsigned int)2); 
				postProcessMSAA.setShaderValue("texel_ofs",texel_ofs);
				
				postProcessMSAA.setupShader();
				postProcessMSAA.use();

				glColor4f(1,1,1,1);
				
				glBegin(GL_QUADS);
				glTexCoord2i(0, 0); glVertex2i(-1,-1);
				glTexCoord2i(1, 0); glVertex2i(1, -1);
				glTexCoord2i(1, 1); glVertex2i(1, 1);
				glTexCoord2i(0, 1); glVertex2i(-1, 1);
				glEnd();		
				
				if (processCount > 1) 
				{
					Texture::setTexture(0);
					glBindTexture(GL_TEXTURE_2D,renderBufferTarget[renderTarget]->renderSurface.texture);
					renderTarget = (renderTarget+1)%numTargets;
					
					renderBufferTarget[renderTarget]->bindRenderTarget();
					glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
				}
			}
				
			if (postProcessConfig.ssao)
			{
				postProcessSSAO.setShaderValue("renderTargetTex",(unsigned int)0);
				postProcessSSAO.setShaderValue("positionBuffer", (unsigned int)1); 
				postProcessSSAO.setShaderValue("lumiBuffer", (unsigned int)2); 
				postProcessSSAO.setShaderValue("normalBuffer", (unsigned int)3); 
				postProcessSSAO.setShaderValue("nearPlane", (float)cam->nearclip); 
				postProcessSSAO.setShaderValue("farPlane", (float)cam->farclip); 
				postProcessSSAO.setShaderValue("texel_ofs",texel_ofs);
				
				postProcessSSAO.setupShader();
				postProcessSSAO.use();
				
				glColor4f(1,1,1,1);
				
				glBegin(GL_QUADS);
				glTexCoord2i(0, 0); glVertex2i(-1,-1);
				glTexCoord2i(1, 0); glVertex2i(1, -1);
				glTexCoord2i(1, 1); glVertex2i(1, 1);
				glTexCoord2i(0, 1); glVertex2i(-1, 1);
				glEnd();		


				if (processCount > 1) 
				{
					Texture::setTexture(0);
					glBindTexture(GL_TEXTURE_2D,renderBufferTarget[renderTarget]->renderSurface.texture);

					renderTarget = (renderTarget+1)%numTargets;					
					renderBufferTarget[renderTarget]->bindRenderTarget();
					glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
				}
			}
		
			if (postProcessConfig.dof)
			{
				postProcessDOF.setShaderValue("renderTargetTex",(unsigned int)0);
				postProcessDOF.setShaderValue("positionBuffer", (unsigned int)1); 
				postProcessDOF.setShaderValue("lumiBuffer", (unsigned int)2); 
				postProcessDOF.setShaderValue("texel_ofs",texel_ofs);
				postProcessDOF.setShaderValue("near_depth", (float)cam->dof_near_linear); 
				postProcessDOF.setShaderValue("far_depth", (float)cam->dof_far_linear); 
			
				postProcessDOF.setupShader();
				postProcessDOF.use();
				
				glColor4f(1,1,1,1);
				
				glBegin(GL_QUADS);
				glTexCoord2i(0, 0); glVertex2i(-1,-1);
				glTexCoord2i(1, 0); glVertex2i(1, -1);
				glTexCoord2i(1, 1); glVertex2i(1, 1);
				glTexCoord2i(0, 1); glVertex2i(-1, 1);
				glEnd();		
				

				if (processCount > 1) 
				{
					Texture::setTexture(0);
					glBindTexture(GL_TEXTURE_2D,renderBufferTarget[renderTarget]->renderSurface.texture);

					renderTarget = (renderTarget+1)%numTargets;
					
					//					renderBufferTarget[renderTarget]->bindRenderTarget();
					//					glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
				}
				
			}
			
		}

	
		if (blurAmt)
		{
			blendBuffer->bindRenderTarget();
			Texture::setTexture(0);
			glBindTexture(GL_TEXTURE_2D,renderBufferTarget[renderTarget]->renderSurface.texture);

			postProcessBlur.setShaderValue("renderTargetTex",(unsigned int)0);
			postProcessBlur.setShaderValue("positionBuffer", (unsigned int)1); 
			postProcessBlur.setShaderValue("lumiBuffer", (unsigned int)2); 
			postProcessBlur.setShaderValue("texel_ofs",texel_ofs);
			postProcessBlur.setShaderValue("blurAmt",blurAmt);
			
			postProcessBlur.setupShader();
			postProcessBlur.use();
			
			glColor4f(1,1,1,1);
			glEnable(GL_BLEND);
			glDepthMask(true);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			
			glBegin(GL_QUADS);
			glTexCoord2i(0, 0); glVertex2i(-1,-1);
			glTexCoord2i(1, 0); glVertex2i(1, -1);
			glTexCoord2i(1, 1); glVertex2i(1, 1);
			glTexCoord2i(0, 1); glVertex2i(-1, 1);
			glEnd();		
			glDisable(GL_BLEND);
			
		}
	
	
		if (!processCount || processCount>1 || postProcessConfig.hdr)
		{
			renderBufferTarget[renderTarget]->resetRenderTarget();
			
			if (blurAmt)
			{
				Texture::setTexture(0);
				glBindTexture(GL_TEXTURE_2D,blendBuffer->renderSurface.texture);	
			}
			
			postProcessCopy.setShaderValue("renderTargetTex",(unsigned int)0);
			postProcessCopy.setShaderValue("positionBuffer", (unsigned int)1); 
			postProcessCopy.setShaderValue("lumiBuffer", (unsigned int)2); 
			postProcessCopy.setShaderValue("texel_ofs",texel_ofs);
			
			postProcessCopy.setupShader();
			postProcessCopy.use();
			
			glColor4f(1,1,1,1);
			
			glBegin(GL_QUADS);
			glTexCoord2i(0, 0); glVertex2i(-1,-1);
			glTexCoord2i(1, 0); glVertex2i(1, -1);
			glTexCoord2i(1, 1); glVertex2i(1, 1);
			glTexCoord2i(0, 1); glVertex2i(-1, 1);
			glEnd();		
		}
	
		
	
		if (postProcessConfig.hdr)
		{
			halfRenderBufferTarget[renderTarget]->bindRenderTarget();
			glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
			glViewport(0,0,screen_w/HALFBUFFER_DIV,screen_h/HALFBUFFER_DIV); 

			postProcessHDR.setShaderValue("renderTargetTex",(unsigned int)0);
			postProcessHDR.setShaderValue("positionBuffer", (unsigned int)1); 
			postProcessHDR.setShaderValue("lumiBuffer", (unsigned int)2); 
			postProcessHDR.setShaderValue("texel_ofs",texel_ofs);
			
			postProcessHDR.setupShader();
			postProcessHDR.use();
			
			glColor4f(1,1,1,1);
			
			glBegin(GL_QUADS);
			glTexCoord2i(0, 0); glVertex2i(-1,-1);
			glTexCoord2i(1, 0); glVertex2i(1, -1);
			glTexCoord2i(1, 1); glVertex2i(1, 1);
			glTexCoord2i(0, 1); glVertex2i(-1, 1);
			glEnd();		
			
			Texture::setTexture(0);
			glBindTexture(GL_TEXTURE_2D,halfRenderBufferTarget[renderTarget]->renderSurface.texture);

//				if (processCount > 1) 
//				{
////					renderTarget = (renderTarget+1)%numTargets;
//					renderTarget--;
//					if (renderTarget<0) renderTarget=numTargets-1;
//					
//					renderBufferTarget[renderTarget]->bindRenderTarget();
////					glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
//				}
//				else
//				{
				renderBufferTarget[renderTarget]->resetRenderTarget();
//				}
			
//				glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

//				if (processCount > 1) renderBufferTarget[renderTarget]->bindRenderTarget();
			
			glViewport(0,0,screen_w,screen_h); 
			
			postProcessCopy.setShaderValue("renderTargetTex",(unsigned int)0);
			postProcessCopy.setShaderValue("positionBuffer", (unsigned int)1); 
			postProcessCopy.setShaderValue("lumiBuffer", (unsigned int)2); 
			postProcessCopy.setShaderValue("texel_ofs",texel_ofs);
			
			postProcessCopy.setupShader();
			postProcessCopy.use();
			
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE,GL_ONE);
			glColor4f(1,1,1,1);
			
			glBegin(GL_QUADS);
			glTexCoord2i(0, 0); glVertex2i(-1,-1);
			glTexCoord2i(1, 0); glVertex2i(1, -1);
			glTexCoord2i(1, 1); glVertex2i(1, 1);
			glTexCoord2i(0, 1); glVertex2i(-1, 1);
			glEnd();
			glDisable(GL_BLEND);				

		
//				Texture::setTexture(0);
//				glBindTexture(GL_TEXTURE_2D,renderBufferTarget[renderTarget]->renderSurface.texture);
//				if (processCount > 1) 
//				{
//					renderTarget = (renderTarget+1)%numTargets;					
//					renderBufferTarget[renderTarget]->bindRenderTarget();
//					glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
//				}
			
		}
	
		GLShader::clear();
}

void Scene::renderDeferredLighting()
{
//	glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, renderBuffer->renderSurface.fbo);
//	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
//
//	glBlitFramebufferEXT(0, 0, screen_w-1, screen_h-1,
//                      0, 0, screen_w-1, screen_h-1,
//                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);
//
//	glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);

	

	if (!render_buffer_open)
	{
		renderBufferTarget[0]->bindRenderTarget();
		glClearDepth(1.0);
		glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	}
	else
	{
		renderBufferTarget[0]->bindRenderTarget();
	}
	
	
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	Texture::clearAllTextures();
	Texture::setTexture(0);		
	glBindTexture(GL_TEXTURE_2D, renderBuffer->renderSurface.texture);
	GLShader::defaultShader.setShaderValue("colorBuffer", (unsigned int)0); 
	
	Texture::setTexture(1);		
	glBindTexture(GL_TEXTURE_2D, renderBuffer->renderSurface.attachments[0]);
	GLShader::defaultShader.setShaderValue("normalBuffer", (unsigned int)1); 
	
	Texture::setTexture(2);		
	glBindTexture(GL_TEXTURE_2D, renderBuffer->renderSurface.attachments[1]);
	GLShader::defaultShader.setShaderValue("positionBuffer", (unsigned int)2); 

	Texture::setTexture(3);		
	glBindTexture(GL_TEXTURE_2D, renderBuffer->renderSurface.attachments[2]);
	GLShader::defaultShader.setShaderValue("lumiBuffer", (unsigned int)3); 
	
	
	int passNum = 0;
	int shadow_count = 0;
	int light_count = 0;
	int total_count = 0;
	bool multiLight;
	
	RGB amb_temp = Light::global_ambient;
	
	map<unsigned long, set<Light *>, ltulong> lights_sorted;
	map<unsigned long, set<Light *>, ltulong>::iterator lights_sorted_i;

	for (active_light_i = active_lights.begin(); active_light_i != active_lights.end(); active_light_i++)
	{
		lights_sorted[(*active_light_i)->type + LIGHT_DEFERRED].insert((*active_light_i));
	}
	
	int num_lights;
	
	
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//	
//	gluLookAt(cam->position.x, cam->position.y, cam->position.z, cam->target.x, cam->target.y, cam->target.z, 0, 1, 0);
//	GLfloat tmp[16];
//	glGetFloatv(GL_MODELVIEW_MATRIX,tmp);
//	GLShader::defaultShader.setShaderValue("modelViewInv", tmp); 
	
	
	for (lights_sorted_i = lights_sorted.begin(); lights_sorted_i != lights_sorted.end(); lights_sorted_i++)
	{
		num_lights = (*lights_sorted_i).second.size();
		multiLight = false;//GLShader::isMultiLight((*lights_sorted_i).first);
		total_count = 0;
		unsigned long shadowMask = 0;
//		Texture::clearAllTextures();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		glDisable(GL_ALPHA_TEST);
		
		for (active_light_i = (*lights_sorted_i).second.begin(); active_light_i != (*lights_sorted_i).second.end(); active_light_i++)
		{
			//		cam->setup();
//			if (!light_count)
			{
//				glMatrixMode(GL_PROJECTION);
//				glLoadMatrixf(cam->projectionMatrix);
//				glMatrixMode(GL_MODELVIEW);
//				glLoadMatrixf(cam->viewMatrix);
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				(*active_light_i)->setup(GL_LIGHT0+light_count);
			}
			
			
			if ((*active_light_i)->has_shadow)
			{				
				char *mapNames[8] = { "shadowMap0", "shadowMap1", "shadowMap2", "shadowMap3", "shadowMap4", "shadowMap5", "shadowMap6", "shadowMap7" }; 
				
//				printf("[%d]",(*active_light_i)->type);
				
				if ((*active_light_i)->type == (LIGHT_SPOT))
				{
						
					glActiveTexture(GL_TEXTURE0+(*active_light_i)->shadow_mtex);
					glClientActiveTexture(GL_TEXTURE0+(*active_light_i)->shadow_mtex);
					GLShader::defaultShader.setShaderValue(mapNames[light_count], (*active_light_i)->shadow_mtex); 
					
					glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, (*active_light_i)->shadowmapTex);
									
					(*active_light_i)->shadowInit();				// grab view matrix
					(*active_light_i)->shadowRenderStart(0,light_count);

					
					//						//	glAlphaFunc(GL_GEQUAL, 0.99f);
					////							glEnable(GL_ALPHA_TEST);
					
					//							mat->useProgram();
					
					switch (light_count)
					{
						case 0: shadowMask |= SHADER_VARIANT_SHADOW0; break;
						case 1: shadowMask |= SHADER_VARIANT_SHADOW1; break;
						case 2: shadowMask |= SHADER_VARIANT_SHADOW2; break;
						case 3: shadowMask |= SHADER_VARIANT_SHADOW3; break;
						case 4: shadowMask |= SHADER_VARIANT_SHADOW4; break;
						case 5: shadowMask |= SHADER_VARIANT_SHADOW5; break;
						case 6: shadowMask |= SHADER_VARIANT_SHADOW6; break;
						case 7: shadowMask |= SHADER_VARIANT_SHADOW7; break;
					}
					
//					(*active_light_i)->setup(GL_LIGHT0+light_count);

				}
				else if ((*active_light_i)->type == (LIGHT_AREA))
				{
					AreaLight *aLight = (AreaLight*)(*active_light_i);

					if (aLight->hasShadow())
					{
						
						glActiveTexture(GL_TEXTURE0+aLight->oLight[0].shadow_mtex);
						glClientActiveTexture(GL_TEXTURE0+aLight->oLight[0].shadow_mtex);
						GLShader::defaultShader.setShaderValue("shadowMap0", aLight->oLight[0].shadow_mtex); 
						
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, aLight->oLight[0].shadowmapTex);
						
						
						glActiveTexture(GL_TEXTURE0+aLight->oLight[1].shadow_mtex);
						glClientActiveTexture(GL_TEXTURE0+aLight->oLight[1].shadow_mtex);
						GLShader::defaultShader.setShaderValue("shadowMap1", aLight->oLight[1].shadow_mtex); 
						
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, aLight->oLight[1].shadowmapTex);
						
						
						glActiveTexture(GL_TEXTURE0+aLight->oLight[2].shadow_mtex);
						glClientActiveTexture(GL_TEXTURE0+aLight->oLight[2].shadow_mtex);
						GLShader::defaultShader.setShaderValue("shadowMap2", aLight->oLight[2].shadow_mtex); 
						
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, aLight->oLight[2].shadowmapTex);
						
						
						GLShader::defaultShader.setShaderValue("nearShadowRes",(float)aLight->near_res);
						GLShader::defaultShader.setShaderValue("farShadowRes",(float)aLight->far_res);
						
						
						//						//	glAlphaFunc(GL_GEQUAL, 0.99f);
						////							glEnable(GL_ALPHA_TEST);
						
						//							mat->useProgram();
						shadow_count = light_count;
						
						for (int n = 0; n < 3; n++)
						{
							switch (shadow_count)
							{
								case 0: shadowMask |= SHADER_VARIANT_SHADOW0; break;
								case 1: shadowMask |= SHADER_VARIANT_SHADOW1; break;
								case 2: shadowMask |= SHADER_VARIANT_SHADOW2; break;
								case 3: shadowMask |= SHADER_VARIANT_SHADOW3; break;
								case 4: shadowMask |= SHADER_VARIANT_SHADOW4; break;
								case 5: shadowMask |= SHADER_VARIANT_SHADOW5; break;
								case 6: shadowMask |= SHADER_VARIANT_SHADOW6; break;
								case 7: shadowMask |= SHADER_VARIANT_SHADOW7; break;
							}
							
							shadow_count++;							
						}
						
	//					
	//					glActiveTexture(GL_TEXTURE0+(*active_light_i)->shadow_mtex);
	//					glClientActiveTexture(GL_TEXTURE0+(*active_light_i)->shadow_mtex);
	//					GLShader::defaultShader.setShaderValue(mapNames[light_count], (*active_light_i)->shadow_mtex); 
	//					
	//					glEnable(GL_TEXTURE_2D);
	//					glBindTexture(GL_TEXTURE_2D, (*active_light_i)->shadowmapTex);
						
						shadow_count = 0;
						
						aLight->oLight[0].setup(GL_LIGHT0+light_count);
						aLight->oLight[1].setup(GL_LIGHT0+light_count+1);
						aLight->oLight[2].setup(GL_LIGHT0+light_count+2);
						
						aLight->oLight[0].shadowInit();				
						aLight->oLight[0].shadowRenderStart(0,shadow_count);
						shadow_count++;
						aLight->oLight[1].shadowInit();				
						aLight->oLight[1].shadowRenderStart(0,shadow_count);
						shadow_count++;
						aLight->oLight[2].shadowInit();				
						aLight->oLight[2].shadowRenderStart(0,shadow_count);
						shadow_count++;
					}
					else
					{
						aLight->update();
					}
					
					aLight->setup(GL_LIGHT0+light_count);
					
					//						//	glAlphaFunc(GL_GEQUAL, 0.99f);
					////							glEnable(GL_ALPHA_TEST);
					
					//							mat->useProgram();
					
//					switch (light_count)
//					{
//						case 0: shadowMask |= SHADER_VARIANT_SHADOW0; break;
//						case 1: shadowMask |= SHADER_VARIANT_SHADOW1; break;
//						case 2: shadowMask |= SHADER_VARIANT_SHADOW2; break;
//						case 3: shadowMask |= SHADER_VARIANT_SHADOW3; break;
//						case 4: shadowMask |= SHADER_VARIANT_SHADOW4; break;
//						case 5: shadowMask |= SHADER_VARIANT_SHADOW5; break;
//						case 6: shadowMask |= SHADER_VARIANT_SHADOW6; break;
//						case 7: shadowMask |= SHADER_VARIANT_SHADOW7; break;
//					}
					
				}
			}
						

			if (multiLight && (light_count != 7) && (total_count != num_lights-1) && num_lights != 1)
			{
				light_count++;
				total_count++;
				continue;
			}

			total_count++;
						
			if (passNum==1)
			{
				glEnable(GL_BLEND);
				glEnable(GL_ALPHA_TEST);
				glBlendFunc(GL_ONE,GL_ONE);
				//			glDepthMask(false);				
				Light::setGlobalAmbient(RGB(0,0,0));
			}
			else if (!passNum)
			{
				glDisable(GL_BLEND);
			}
			
			if (!GLShader::defaultShader.activateLight((*lights_sorted_i).first,light_count,shadowMask,true)) 
			{
				if (!GLShader::defaultShader.activateLight((*lights_sorted_i).first,light_count,shadowMask,true)) continue;
			}
				
			glDepthFunc(GL_LEQUAL);

			glColor4f(1,1,1,1);

			if ((*active_light_i)->type == (LIGHT_AREA))
			{
				AreaLight *aLight = (AreaLight*)(*active_light_i);
				aLight->setup(GL_LIGHT0);
			}
			
			glBegin(GL_QUADS);
			glTexCoord2i(0, 0); glVertex2i(-1,-1);
			glTexCoord2i(1, 0); glVertex2i(1, -1);
			glTexCoord2i(1, 1); glVertex2i(1, 1);
			glTexCoord2i(0, 1); glVertex2i(-1, 1);
			glEnd();
			
			
			if ((*active_light_i)->type == (LIGHT_AREA))
			{
				AreaLight *aLight = (AreaLight*)(*active_light_i);
				
				if (aLight->hasShadow())
				{
					shadow_count = 0;
					aLight->oLight[0].shadowRenderFinish(0,shadow_count);
					shadow_count++;
					aLight->oLight[1].shadowRenderFinish(0,shadow_count);
					shadow_count++;
					aLight->oLight[2].shadowRenderFinish(0,shadow_count);
				}
			}
			
			passNum++;
//			Logger::log("%d - %d - %d\n",light_count, num_lights, total_count);
			light_count = 0;
		}
	}

//	GLShader::clear();
	
//	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
//	glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, renderBuffer->renderSurface.fbo);
//	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
//	glBlitFramebufferEXT(0, 0, screen_w, screen_h, 0, 0, screen_w, screen_h, GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT, GL_NEAREST);
//	glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, 0);
//	glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);

	
	if (!render_buffer_open) renderBufferTarget[0]->resetRenderTarget();
	
	passNum = 0;
	shadow_count = 0;
	dummySet.clear();
	
	//Texture::clearLocks();
	
	
	/*
	 glMatrixMode(GL_PROJECTION);
	 glLoadMatrixf(cam->projectionMatrix);
	 glMatrixMode(GL_MODELVIEW);
	 glLoadMatrixf(cam->viewMatrix);
	 
	 for (active_light_i = active_lights.begin(); active_light_i != active_lights.end(); active_light_i++)
	 {
	 (*active_light_i)->setup(GL_LIGHT0+passNum);
	 
	 if ((*active_light_i)->hasShadow())
	 {
	 (*active_light_i)->shadowInit();				// grab view matrix
	 (*active_light_i)->shadowRenderStart(0,shadow_count);
	 shadow_count++;
	 }
	 
	 dummySet.insert((*active_light_i));
	 
	 passNum++;g
	 
	 if (passNum == 2) break;
	 }	
	 
	 glEnable(GL_DEPTH_TEST);
	 
	 glDepthFunc(GL_LESS);
	 
	 glDisable(GL_POLYGON_OFFSET_FILL);
	 
	 glPolygonOffset (0.0,2.0);
	 glDepthMask(false);
	 
	 renderStage(oShader,SHADER_STAGE_TRANSPARENT);
	 
	 passNum = 0;
	 for (active_light_i = active_lights.begin(); active_light_i != active_lights.end(); active_light_i++)
	 {
	 if ((*active_light_i)->hasShadow()) 
	 {
	 (*active_light_i)->shadowRenderFinish(0,shadow_count);
	 
	 shadow_count++; 
	 }
	 
	 (*active_light_i)->disable();
	 
	 if (passNum == 2) break;
	 }
	 */
	
	GLShader::clear();
	
	Light::setGlobalAmbient(amb_temp);	
	
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glBlendFunc(GL_ONE,GL_ZERO);
	
}
#endif

void Scene::render()
{
	unsigned int i;
	
	cubicvr_profiler.shaderval_count = 0;
	
	if (!initialized) 
	{
#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)
		if (hdr_enabled) hdrProcess.init();
#endif
		firstInit();

		if (cam) 
		{
			cam->setAspect(screen_w,screen_h);
			cam->setSize(screen_w,screen_h);
		}
		
		initialized = true;
		
#if !defined(ARCH_DC) && !defined(ARCH_PSP)
		glViewport(0,0,screen_w,screen_h); 
#endif		
	}	

	process();
	
	
	if (sectorMap) 
	{
		sectorMap->process();		
	}
	
	
	bool has_shadow_lights = false;
	bool has_regular_lights = false;

	
	if (sectorMap && cam) 
	{
		active_sceneobjs.clear();
		active_lights.clear();
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		if (cam) cam->setup();
		
		int viewport[4] = {0,0,screen_w,screen_h};
		sectorMap->queryFrustum(active_sceneobjs,cam->projectionMatrix,cam->viewMatrix,cam->nearclip,cam->farclip,viewport,&active_lights);
//		sectorMap->queryFrustum(active_sceneobjs,cam->position,cam->viewMatrix,cam->fov,cam->nearclip,cam->farclip,cam->aspect,&active_lights);
//		sectorMap->queryFrustum(active_sceneobjs,cam,&active_lights);
	}
	
	
#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)

	has_shadow_lights = renderShadowVolumes();
#endif
	

#if !defined(ARCH_PSP)
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);	

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	if (active_lights.size()) glEnable(GL_LIGHTING); else glDisable(GL_LIGHTING);
	if (cam) cam->setup();

	i = 0;

#if !defined(OPENGL_ES) && !defined(ARCH_DC)
	if (!deferred_shading)
#endif
	for (active_light_i = active_lights.begin(); active_light_i != active_lights.end(); active_light_i++)
	{
#if !defined(OPENGL_ES) && !defined(ARCH_DC)
		if ((*active_light_i)->hasShadow() && (*active_light_i)->type == LIGHT_SPOT) // GLExt::have_depth_texture && 
		{
			(*active_light_i)->setup(GL_LIGHT0+i);
		}
		else if ((*active_light_i)->hasShadow() && (*active_light_i)->type == LIGHT_AREA) // GLExt::have_depth_texture && 
		{
			AreaLight *aLight = (AreaLight *)(*active_light_i);
			
			aLight->oLight[0].setup(GL_LIGHT0+i);
			aLight->oLight[1].setup(GL_LIGHT0+i+1);
			aLight->oLight[2].setup(GL_LIGHT0+i+2);
			aLight->setup(GL_LIGHT0+i);
//			i+=2;			
		}
		else
#endif
		{
			if ((*active_light_i)->type == LIGHT_AREA)
			{
				AreaLight *aLight = (AreaLight*)(*active_light_i);
				aLight->update();
			}
			
			has_regular_lights = true; 
			(*active_light_i)->setup(GL_LIGHT0+i);
		}
		
		i++;
	}
	
	if (!has_regular_lights && !active_lights.size()) glDisable(GL_LIGHTING);

#if !defined(ARCH_DC) && !defined(ARCH_PSP) && !defined(OPENGL_ES)
	if (!deferred_shading)
#endif
		oShader.bind(&active_lights);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	if (cam) cam->setup();
#endif

#if defined(ARCH_PSP)
	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();		// Reset the Matrix
	if (cam) cam->setup();

	// No Shadows on psp for now
	unsigned short l=0;
	for (active_light_i = active_lights.begin(); active_light_i != active_lights.end(); active_light_i++)
	{
		if (l>MAX_LIGHTS-1) break;
		if (!(*active_light_i)->hasShadow()) 
		{
			(*active_light_i)->setup(l); 
		}
		l++;
	}
	
#endif	

	
	int shadow_count = 0;
		
#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)
	if (!deferred_shading)
	for (active_light_i = active_lights.begin(); active_light_i != active_lights.end(); active_light_i++)
	{
		if ((*active_light_i)->hasShadow() && (*active_light_i)->type == LIGHT_SPOT)
		{
			(*active_light_i)->shadowInit();				// grab view matrix
			(*active_light_i)->shadowRenderStart(0,shadow_count);
			shadow_count++;
		}
		else if ((*active_light_i)->hasShadow() && (*active_light_i)->type == LIGHT_AREA)
		{
			AreaLight *aLight = (AreaLight*)(*active_light_i);
			aLight->oLight[0].shadowInit();				
			aLight->oLight[0].shadowRenderStart(0,shadow_count);
			shadow_count++;
			aLight->oLight[1].shadowInit();				
			aLight->oLight[1].shadowRenderStart(0,shadow_count);
			shadow_count++;
			aLight->oLight[2].shadowInit();				
			aLight->oLight[2].shadowRenderStart(0,shadow_count);
			shadow_count++;
		}
	}	
		
	if (hdr_enabled)
	{
#if !defined(ARCH_DC) && !defined(ARCH_PSP) && !defined(OPENGL_ES)
		if (!deferred_shading)
#endif
		if (hdr_buffer_open) hdrProcess.drawOpen(); else hdrProcess.drawBegin();
		if (cam) cam->setup();
	}
#endif

	if (sceneobjs_visibility.size())
	{
		std::set<SceneObject *>::iterator vis_obj_i;
		
		frustum.Update();
		
		for (vis_obj_i = sceneobjs_visibility.begin(); vis_obj_i != sceneobjs_visibility.end(); vis_obj_i++)
		{
 			(*vis_obj_i)->calcVisibility(cam->position, frustum);
			active_sceneobjs.insert((*vis_obj_i));
		}
	}
	
	
#if !defined(ARCH_DC) && !defined(ARCH_PSP) && !defined(OPENGL_ES)
	if (deferred_shading)
	{
		dummyLight.setType(LIGHT_DEFERRED);
		dummySet.insert(&dummyLight);
		
		oShader.bind(&dummySet);

		renderBuffer->bindRenderTarget();
//		XYZ buf = XYZ(screen_w,screen_h,0);
//		GLShader::defaultShader.setShaderValue("buffersize", buf); 
//		XYZ crange = XYZ(cam->nearclip,cam->farclip,0);
//		GLShader::defaultShader.setShaderValue("camerarange", crange); 
//		glClearDepth(1.0);
		GLShader::defaultShader.setShaderValue("nearPlane", (float)cam->nearclip); 
		GLShader::defaultShader.setShaderValue("farPlane", (float)cam->farclip); 		
		
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// clear the render buffer
	}
#endif
	
	
#if !defined(ARCH_PSP) && !defined(OPENGL_ES)
	if (debug.wireframe)
	{
		renderStage(lShader,SHADER_STAGE_OPAQUE);		
		renderStage(lShader,SHADER_STAGE_TRANSPARENT);		
	}
	else
	{
#endif
//		glDisable(GL_LIGHTING);
//		GLShader::clear();
//		if (cam) oShader.setLODSource(cam->position);
		
//		if (debug.frustum && sectorMap)
//		{
//			
//			glMatrixMode(GL_MODELVIEW);
//			glLoadIdentity();
//			
//			glMatrixMode(GL_PROJECTION);
//			glLoadIdentity();
//			
//			
//			Camera tmpCam;
//			tmpCam.setFOV(90);
//			//	tmpCam.target = XYZ(0,0,0);
//			tmpCam.position = XYZ(-400,100,-400);
//			tmpCam.rotation = XYZ(-25,45,0);
//			tmpCam.setSize(screen_w,screen_h);
//			tmpCam.setAspect(screen_w,screen_h);
//			tmpCam.setClip(1,2000);
//			//	tmpCam.setType(CAMERA_TARGET);
//			tmpCam.setup();
//		}
		
		// Early Z?
//		renderStage(sShader,SHADER_STAGE_SHADOW);
		renderStage(oShader,SHADER_STAGE_OPAQUE);
		
#if !defined(ARCH_DC) && !defined(ARCH_PSP) && !defined(OPENGL_ES)
	if (!deferred_shading)
#endif
	renderStage(oShader,SHADER_STAGE_TRANSPARENT);	
		
#if !defined(ARCH_DC) && !defined(ARCH_PSP) && !defined(OPENGL_ES)
	if (deferred_shading)
	{
		renderBuffer->resetRenderTarget();
	}
#endif
		
#if !defined(ARCH_PSP) && !defined(OPENGL_ES)
	}	
#endif
	
	shadow_count = 0;
	
#if !defined(ARCH_DC) && !defined(ARCH_PSP) && !defined(OPENGL_ES)
	if (!deferred_shading)
#endif
	for (active_light_i = active_lights.begin(); active_light_i != active_lights.end(); active_light_i++)
	{
		
#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)

		if ((*active_light_i)->hasShadow() && (*active_light_i)->type==LIGHT_SPOT) 
		{
			(*active_light_i)->shadowRenderFinish(0,shadow_count);

//			if (hdr_enabled) break;	// ??? works ???
			shadow_count++; 
		}
		else if ((*active_light_i)->hasShadow() && (*active_light_i)->type == LIGHT_AREA)
		{
			AreaLight *aLight = (AreaLight*)(*active_light_i);

			aLight->oLight[0].shadowRenderFinish(0,shadow_count);
			shadow_count++;
			aLight->oLight[1].shadowRenderFinish(0,shadow_count);
			shadow_count++;
			aLight->oLight[2].shadowRenderFinish(0,shadow_count);
			shadow_count++;
		}
		
#endif

//		(*active_light_i)->disable(i);
	}


	
#if !defined(ARCH_DC) && !defined(OPENGL_ES) && !defined(ARCH_PSP)
if (deferred_shading)
{
	if (hdr_enabled)
	{
		if (hdr_buffer_open) hdrProcess.drawOpen(); else 
		{
			hdrProcess.drawBegin();
			//glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
		}
	}
	
	renderDeferredLighting();
	
}
#endif
	
//	cam->setup();
	

#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)
	if (hdr_enabled && !hdr_buffer_open) 
	{
		hdrProcess.drawEnd();	
	}
	if (deferred_shading && !render_buffer_open)
	{
		closeRenderBuffer();
	}
#endif
	
	
#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)
//	glPopAttrib();
	GLShader::processShaderQueue();	
#endif
	
	
#if !defined(ARCH_PSP) && !defined(OPENGL_ES)
	if (debug.lights)
	{
		float q;
		
		
		if (cam) cam->setup();
		
		Texture::clearAllTextures();
		GLShader::clear();
		glDepthFunc(GL_LESS);
		glDepthMask(GL_TRUE);
		glDisable(GL_LIGHTING);
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);

		
		//Pencil::drawFrustum(cam->position,cam->viewMatrix,cam->fov,cam->nearclip,cam->farclip,cam->aspect);

		for (active_light_i = active_lights.begin(); active_light_i != active_lights.end(); active_light_i++)
		{
			if ((*active_light_i)->type == LIGHT_POINT && (*active_light_i)->cutoff)
			{
				float divs = 10;
				
				Vector tmp((*active_light_i)->diffuse.r,(*active_light_i)->diffuse.g,(*active_light_i)->diffuse.b);
				tmp.makeUnit();
				glColor4f(tmp.x,tmp.y,tmp.z,1);

				glBegin(GL_LINE_STRIP);
					for (q = 0; q <= (M_PI*2.0)+(M_PI/divs); q+=(M_PI/divs))
					{
						glVertex3f((*active_light_i)->position.x+cos(q)*(*active_light_i)->cutoff,(*active_light_i)->position.y+sin(q)*(*active_light_i)->cutoff,(*active_light_i)->position.z);
					}
				glEnd();
					
				glBegin(GL_LINE_STRIP);
				for (q = 0; q <= (M_PI*2.0)+(M_PI/divs); q+=(M_PI/divs))
				{
					glVertex3f((*active_light_i)->position.x+cos(q)*(*active_light_i)->cutoff,(*active_light_i)->position.y,(*active_light_i)->position.z-sin(q)*(*active_light_i)->cutoff);
				}
				glEnd();

				glBegin(GL_LINE_STRIP);
				for (q = 0; q <= (M_PI*2.0)+(M_PI/divs); q+=(M_PI/divs))
				{
					glVertex3f((*active_light_i)->position.x,(*active_light_i)->position.y+cos(q)*(*active_light_i)->cutoff,(*active_light_i)->position.z-sin(q)*(*active_light_i)->cutoff);
				}
				glEnd();	
				
				
//				std::set<Light *> light_tmp;
//				oShader.bind(&light_tmp);

				dummySet.clear();
				dummyLight.setType(LIGHT_NULL);
				dummySet.insert(&dummyLight);
				
				oShader.bind(&dummySet);

				deferredSphereMat->color = RGBA((*active_light_i)->diffuse.r,(*active_light_i)->diffuse.g,(*active_light_i)->diffuse.b,1);		

				glPushMatrix();
					glTranslatef((*active_light_i)->position.x,(*active_light_i)->position.y,(*active_light_i)->position.z);
					glPushMatrix();
						glScalef((*active_light_i)->cutoff,(*active_light_i)->cutoff,(*active_light_i)->cutoff);
						oShader.render(*deferred_sphere,SHADER_STAGE_OPAQUE);
					glPopMatrix();
				glPopMatrix();
				
				
			}
			else if ((*active_light_i)->type == LIGHT_SPOT)
			{
//				Pencil::drawFrustum((*active_light_i)->position,(*active_light_i)->viewMatrix,(*active_light_i)->cutoff,(*active_light_i)->nearclip,(*active_light_i)->farclip,1.0f);
				
				float clipDist = (*active_light_i)->farclip-(*active_light_i)->nearclip;
				
				sectorMap->renderFrustum((*active_light_i)->projectionMatrix,(*active_light_i)->viewMatrix,-clipDist/2.0f,clipDist/2.0f,(*active_light_i)->viewport);
			}
			else if ((*active_light_i)->type == LIGHT_AREA)
			{
//				glMatrixMode(GL_PROJECTION);
//				glPushMatrix();
//				glMatrixMode(GL_MODELVIEW);
//				glPushMatrix();
//				
//				Camera dummyCam;
//				
//				dummyCam.position = (*active_light_i)->position;
//				dummyCam.rotation = (*active_light_i)->rotation;
//				dummyCam.target = (*active_light_i)->target;
//				if ((*active_light_i)->has_target) dummyCam.setTarget((*active_light_i)->target);
//				dummyCam.setClip((*active_light_i)->nearclip,(*active_light_i)->farclip);
//				dummyCam.setup();
//
//				glMatrixMode(GL_PROJECTION);
//				glPopMatrix();
//				glMatrixMode(GL_MODELVIEW);
//				glPopMatrix();
				
				AreaLight *aLight = (AreaLight *)(*active_light_i);
				
				for (int n = 0; n < 3; n++)
				{
					sectorMap->renderOrthoFrustum(aLight->oLight[n].position,aLight->oLight[n].ortho_size,aLight->oLight[n].ortho_size,aLight->oLight[n].projectionMatrix,aLight->oLight[n].viewMatrix,aLight->oLight[n].nearclip,aLight->oLight[n].farclip,aLight->oLight[n].viewport);
				}
			}
			
		}		
		glDepthFunc(GL_LESS);
	}
#endif
	
	std::set<SceneObject *>::iterator obj_i;
	
	for (obj_i = sceneobjs_bound.begin(); obj_i != sceneobjs_bound.end(); obj_i++)
	{
		(*obj_i)->movedReset();
	}	
	
	
#if !defined(ARCH_PSP) && !defined(OPENGL_ES)
	renderDebug();		
#endif
	
}


void Scene::sectorDebugDraw()
{
	if (!sectorMap) return;
#if !defined(ARCH_PSP) && !defined(OPENGL_ES) && !defined(ARCH_DC)
	GLShader::clear();
	cam->setup();
	sectorMap->debugDraw();
#endif
}


/* by name managed */


SceneObject &Scene::sceneObject(const std::string name_in)
{
	return *sceneobjs_bound_ref[name_in];
};


Camera &Scene::camera(const std::string name_in)
{
	return *cams_bound_ref[name_in];
};

Light &Scene::light(const std::string name_in)
{
	return *lights_bound_ref[name_in];
};

Motion &Scene::motion(const std::string name_in)
{
	return *motions_bound_ref[name_in];
};

//
//void Scene::setCollisionMap(float nominal_rad)
//{
//	unsigned int i;
//	
//	for (i = 0; i < sceneobjs_managed.size(); i++)
//	{
//		if (sceneobjs_managed[i] != NULL) 
//		{
//			sceneobjs_managed[i]->setCollisionMap();
//			sceneobjs_managed[i]->compileSphereMap(nominal_rad);
//		}
//	}
//}
//
//
//


#if !defined(ARCH_DC) && !defined(ARCH_PSP)
void Scene::saveScreenshot(char *path, int seq)
{
	FILE * file;

#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif
	
	//first calculate the filename to save to
	char filename[PATH_MAX];
	
	if (seq == -1) 
	{
		for(int i=0; i<10000; i++)
		{
			sprintf(filename, "%sscreen%05d.tga", path, i);
			
			//try opening this file - if not possible, use this filename
			file=fopen(filename, "rb");
			
			if(!file)
			{
				break;
			}
			
			//otherwise, the file exists, try next, except if this is the last one
			fclose(file);
			
			if(i==9999)
			{
				Logger::log(LOG_ERROR,"No space to save screenshot - 0-9999 exist");
				return;
			}
		}
	}
	else
	{
		sprintf(filename, "%sscreen%05d.tga", path, seq);	
	}
	
	Logger::log("Saving %s\n", filename);
	
	GLubyte		TGAheader[12]={0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};	//Uncompressed TGA header
	GLubyte		infoHeader[6];
	
	unsigned char * data=new unsigned char[4*screen_w*screen_h];
	if(!data)
	{
		Logger::log(LOG_ERROR,"Unable to allocate memory for screen data");
		return;
	}
	
	//read in the screen data
	glReadPixels(0, 0, screen_w, screen_h, GL_RGBA, GL_UNSIGNED_BYTE, data);
	
	//data needs to be in BGR format
	//swap b and r
	for(int i=0; i<(int)screen_w*screen_h*4; i+=4)
	{	
		//repeated XOR to swap bytes 0 and 2
		data[i] ^= data[i+2] ^= data[i] ^= data[i+2];
	}
	
	//open the file
	file = fopen(filename, "wb");
	
	//save header
	fwrite(TGAheader, 1, sizeof(TGAheader), file);
	
	//save info header
	infoHeader[0]=(screen_w & 0x00FF);
	infoHeader[1]=(screen_w & 0xFF00) >> 8;
	infoHeader[2]=(screen_h & 0x00FF);
	infoHeader[3]=(screen_h & 0xFF00) >> 8;
	infoHeader[4]=32;
	infoHeader[5]=0;
	
	//save info header
	fwrite(infoHeader, 1, sizeof(infoHeader), file);
	
	//save the image data
	fwrite(data, 1, screen_w*screen_h*4, file);
	
	fclose(file);
	
	delete data;
	
	Logger::log("Saved Screenshot: %s", filename);
	return;
}
#endif

#ifdef ARCH_PSP
#include <pspiofilemgr.h>
#define		PIXELSIZE	1				//in short
#define		LINESIZE	256				//in short
#define		FRAMESIZE	0x20000			//in byte

void writeByte(int fd, unsigned char data) {
	sceIoWrite(fd, &data, 1);
}

void Scene::saveScreenshot(char *path, void *fb, int seq)
{
	
	const char tgaHeader[] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	const int width = 480;
	const int lineWidth = 512;
	const int height = 272;
	unsigned char lineBuffer[width*4];
	u32* vram = (u32*)(0x04000000+0x40000000);
	int x, y;
	
	
	//first calculate the filename to save to
	char filename[1024];
	FILE *file;
	
	if (seq == -1) 
	{
		for(int i=0; i<10000; i++)
		{
			sprintf(filename, "%sscreen%05d.tga", path, i);
			
			//try opening this file - if not possible, use this filename
			file=fopen(filename, "rb");
			
			if(!file)
			{
				break;
			}
			
			//otherwise, the file exists, try next, except if this is the last one
			fclose(file);
			
			if(i==9999)
			{
				Logger::log(LOG_ERROR,"No space to save screenshot - 0-9999 exist");
				return;
			}
		}
	}
	else
	{
		sprintf(filename, "%sscreen%05d.tga", path, seq);	
	}
	
	Logger::log("Saving %s\n", filename);

	int fd = sceIoOpen(filename, PSP_O_CREAT | PSP_O_TRUNC | PSP_O_WRONLY, 0777);
	if (!fd) return;
	sceIoWrite(fd, tgaHeader, sizeof(tgaHeader));
	writeByte(fd, width & 0xff);
	writeByte(fd, width >> 8);
	writeByte(fd, height & 0xff);
	writeByte(fd, height >> 8);
	writeByte(fd, 24);
	writeByte(fd, 0);
	for (y = height - 1; y >= 0; y--) {
		for (x = 0; x < width; x++) {
			u32 color = vram[y * lineWidth + x];
			unsigned char red = color & 0xff;
			unsigned char green = (color >> 8) & 0xff;
			unsigned char blue = (color >> 16) & 0xff;
			lineBuffer[3*x] = blue;
			lineBuffer[3*x+1] = green;
			lineBuffer[3*x+2] = red;
		}
		sceIoWrite(fd, lineBuffer, width * 3);
	}
	sceIoClose(fd);
	
	Logger::log("Saved Screenshot: %s", filename);
	return;
}
#endif


void Scene::bbRayTest(XYZ pos, Vector ray, SceneRayResult &selList, int axisMatch)
{
	XYZ pt1, pt2;
	
	pt1 = pos;
	pt2 = pos+ray;
	
	cvrIndex i = 0;
	
//	std::map<float,SceneObject *,ltufloat> selList;
	std::map<float,SceneObject *,ltufloat>::iterator sel_i;
	std::set<SceneObject *>::iterator obj_i;
	
	for (obj_i = sceneobjs_bound.begin(); obj_i != sceneobjs_bound.end(); obj_i++)
	{
		if (!(*obj_i)) continue;
//		if (!(*obj_i)->obj) continue;
		if ((*obj_i)->hasParent()) continue;
		
		XYZ bb1, bb2;
		
		bb1 = (*obj_i)->aabbMin;
		bb2 = (*obj_i)->aabbMax;
		
		XYZ center = (bb1+bb2)*0.5f;
		
		XYZ testPt = Vector::getClosestTo(pt1,pt2,center);
		
		Vector testDist(testPt,center);
		
		if (((testPt.x >= bb1.x && testPt.x <= bb2.x)?1:0) +
			((testPt.y >= bb1.y && testPt.y <= bb2.y)?1:0) +
			((testPt.z >= bb1.z && testPt.z <= bb2.z)?1:0) >= axisMatch)
		{
			selList[testDist.magnitude()] = (*obj_i);
		}
		i++;
	}
	
	//for (sel_i = selList.begin(); sel_i != selList.end(); sel_i++)
//	{
//		objList.insert((*sel_i).second);
//	}
}


SceneObjectSet &Scene::sceneObjects()
{
	return sceneobjs_bound;
}

SceneObjectNamedSet &Scene::sceneObjectsNamed()
{
	return sceneobjs_bound_ref;	
}

