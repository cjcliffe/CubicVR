#pragma once
#include "CubicVR/Vector.h"

namespace cvr {

static const float identity_matrix[16] = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

class mat4 {

#define mat4_set(mat, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) \
        mat[0] = a; mat[1] = b; mat[2] = c; mat[3] = d; \
        mat[4] = e; mat[5] = f; mat[6] = g; mat[7] = h; \
        mat[8] = i; mat[9] = j; mat[10] = k; mat[11] = l; \
        mat[12] = m; mat[13] = n; mat[14] = o; mat[15] = p;

public:
   static float *alloc() {
      float *m = (float *)malloc(sizeof(float)*16);
      return m;
   }

   static float *alloc_identity() {
      float *m = (float *)malloc(sizeof(float)*16);
      mat4::copy(m,(float *)identity_matrix);
      return m;
   }

   static void set_identity(float *m) {
      mat4::copy(m,(float *)identity_matrix);
   }

   static void copy(float *mDst,float *mSrc) {
      memcpy(mDst,mSrc,sizeof(float)*16);
   }

   static void lookat(float *m, XYZ &eye, XYZ &center, XYZ &up) {
          Vector forward, side;
          
          forward = center - eye;          
          forward.makeUnit();

          /* Side = forward x up */
          side = forward * up;
          side.makeUnit();

          /* Recompute up as: up = side x forward */
          up = side * forward;

          mat4_set( m, side.x, up.x, -forward.x, 0, side.y, up.y, -forward.y, 0, side.z, up.z, -forward.z, 0, 0, 0, 0, 1 );

         // mat4::translate(m, -eye);
   };
    
   static void multiply(float *mOut, float *matL, float *matR) {
        mOut[0] = matL[0] * matR[0] + matL[4] * matR[1] + matL[8] * matR[2] + matL[12] * matR[3];
        mOut[1] = matL[1] * matR[0] + matL[5] * matR[1] + matL[9] * matR[2] + matL[13] * matR[3];
        mOut[2] = matL[2] * matR[0] + matL[6] * matR[1] + matL[10] * matR[2] + matL[14] * matR[3];
        mOut[3] = matL[3] * matR[0] + matL[7] * matR[1] + matL[11] * matR[2] + matL[15] * matR[3];
        mOut[4] = matL[0] * matR[4] + matL[4] * matR[5] + matL[8] * matR[6] + matL[12] * matR[7];
        mOut[5] = matL[1] * matR[4] + matL[5] * matR[5] + matL[9] * matR[6] + matL[13] * matR[7];
        mOut[6] = matL[2] * matR[4] + matL[6] * matR[5] + matL[10] * matR[6] + matL[14] * matR[7];
        mOut[7] = matL[3] * matR[4] + matL[7] * matR[5] + matL[11] * matR[6] + matL[15] * matR[7];
        mOut[8] = matL[0] * matR[8] + matL[4] * matR[9] + matL[8] * matR[10] + matL[12] * matR[11];
        mOut[9] = matL[1] * matR[8] + matL[5] * matR[9] + matL[9] * matR[10] + matL[13] * matR[11];
        mOut[10] = matL[2] * matR[8] + matL[6] * matR[9] + matL[10] * matR[10] + matL[14] * matR[11];
        mOut[11] = matL[3] * matR[8] + matL[7] * matR[9] + matL[11] * matR[10] + matL[15] * matR[11];
        mOut[12] = matL[0] * matR[12] + matL[4] * matR[13] + matL[8] * matR[14] + matL[12] * matR[15];
        mOut[13] = matL[1] * matR[12] + matL[5] * matR[13] + matL[9] * matR[14] + matL[13] * matR[15];
        mOut[14] = matL[2] * matR[12] + matL[6] * matR[13] + matL[10] * matR[14] + matL[14] * matR[15];
        mOut[15] = matL[3] * matR[12] + matL[7] * matR[13] + matL[11] * matR[14] + matL[15] * matR[15];
    };
   
    static void vec4_multiply(float *vecOut, float *vecL, float *matR) {
        vecOut[0] = matR[0] * vecL[0] + matR[4] * vecL[1] + matR[8] * vecL[2] + matR[12] * vecL[3];
        vecOut[1] = matR[1] * vecL[0] + matR[5] * vecL[1] + matR[9] * vecL[2] + matR[13] * vecL[3];
        vecOut[2] = matR[2] * vecL[0] + matR[6] * vecL[1] + matR[10] * vecL[2] + matR[14] * vecL[3];
        vecOut[3] = matR[3] * vecL[0] + matR[7] * vecL[1] + matR[11] * vecL[2] + matR[15] * vecL[3];
    };
    
    static void vec3_multiply(float *vecOut, float *vecL, float *matR) {
        vecOut[0] = matR[0] * vecL[0] + matR[4] * vecL[1] + matR[8] * vecL[2] + matR[12];
        vecOut[1] = matR[1] * vecL[0] + matR[5] * vecL[1] + matR[9] * vecL[2] + matR[13];
        vecOut[2] = matR[2] * vecL[0] + matR[6] * vecL[1] + matR[10] * vecL[2] + matR[14];
    };
    
    static void xyz_multiply(XYZ &vecOut, const XYZ &vecL, float *matR) {
        vecOut.x = matR[0] * vecL.x + matR[4] * vecL.y + matR[8] * vecL.z + matR[12];
        vecOut.y = matR[1] * vecL.x + matR[5] * vecL.y + matR[9] * vecL.z + matR[13];
        vecOut.z = matR[2] * vecL.x + matR[6] * vecL.y + matR[10] * vecL.z + matR[14];
    };
  
    static void perspective(float *m_perspective, float fovy, float aspect, float near, float far) {
        float yFac = tanf(fovy * M_PI / 360.0f);
        float xFac = yFac * aspect;

        mat4_set( m_perspective, 1.0 / xFac, 0, 0, 0, 0, 1.0 / yFac, 0, 0, 0, 0, -(far + near) / (far - near), -1, 0, 0, -(2.0 * far * near) / (far - near), 0);
    };
    
    static void ortho(float *m_ortho, float left, float right, float bottom, float top, float near, float far) {
      mat4_set(m_ortho, 2.0 / (right - left), 0, 0, 0, 0, 2.0 / (top - bottom), 0, 0, 0, 0, -2 / (far - near), 0, -(left + right) / (right - left), -(top + bottom) / (top - bottom), -(far + near) / (far - near), 1);
    };
    
    static float determinant(float *m) {
        float a0 = m[0] * m[5] - m[1] * m[4];
        float a1 = m[0] * m[6] - m[2] * m[4];
        float a2 = m[0] * m[7] - m[3] * m[4];
        float a3 = m[1] * m[6] - m[2] * m[5];
        float a4 = m[1] * m[7] - m[3] * m[5];
        float a5 = m[2] * m[7] - m[3] * m[6];
        float b0 = m[8] * m[13] - m[9] * m[12];
        float b1 = m[8] * m[14] - m[10] * m[12];
        float b2 = m[8] * m[15] - m[11] * m[12];
        float b3 = m[9] * m[14] - m[10] * m[13];
        float b4 = m[9] * m[15] - m[11] * m[13];
        float b5 = m[10] * m[15] - m[11] * m[14];

        return a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;
    };
      
    static void transpose(float *m_transposed, float *m) {
        mat4_set(m_transposed, m[0], m[4], m[8], m[12], m[1], m[5], m[9], m[13], m[2], m[6], m[10], m[14], m[3], m[7], m[11], m[15]);
    };

    static void inverse_mat3(float *mat3_inv, float *mat) {
          float a00 = mat[0], a01 = mat[1], a02 = mat[2],
          a10 = mat[4], a11 = mat[5], a12 = mat[6],
          a20 = mat[8], a21 = mat[9], a22 = mat[10];

          float b01 = a22*a11-a12*a21,
          b11 = -a22*a10+a12*a20,
          b21 = a21*a10-a11*a20;

          float d = a00*b01 + a01*b11 + a02*b21;
          if (!d) { mat3_inv = NULL; return; }
          float id = 1.0/d;

          mat3_inv[0] = b01*id;
          mat3_inv[1] = (-a22*a01 + a02*a21)*id;
          mat3_inv[2] = (a12*a01 - a02*a11)*id;
          mat3_inv[3] = b11*id;
          mat3_inv[4] = (a22*a00 - a02*a20)*id;
          mat3_inv[5] = (-a12*a00 + a02*a10)*id;
          mat3_inv[6] = b21*id;
          mat3_inv[7] = (-a21*a00 + a01*a20)*id;
          mat3_inv[8] = (a11*a00 - a01*a10)*id;
   };
      
   static void inverse(float *m_inv, float*m) {
      float a0 = m[0] * m[5] - m[1] * m[4];
      float a1 = m[0] * m[6] - m[2] * m[4];
      float a2 = m[0] * m[7] - m[3] * m[4];
      float a3 = m[1] * m[6] - m[2] * m[5];
      float a4 = m[1] * m[7] - m[3] * m[5];
      float a5 = m[2] * m[7] - m[3] * m[6];
      float b0 = m[8] * m[13] - m[9] * m[12];
      float b1 = m[8] * m[14] - m[10] * m[12];
      float b2 = m[8] * m[15] - m[11] * m[12];
      float b3 = m[9] * m[14] - m[10] * m[13];
      float b4 = m[9] * m[15] - m[11] * m[13];
      float b5 = m[10] * m[15] - m[11] * m[14];

      float determinant = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

      if (determinant != 0) {
          m_inv[0] = 0 + m[5] * b5 - m[6] * b4 + m[7] * b3;
          m_inv[4] = 0 - m[4] * b5 + m[6] * b2 - m[7] * b1;
          m_inv[8] = 0 + m[4] * b4 - m[5] * b2 + m[7] * b0;
          m_inv[12] = 0 - m[4] * b3 + m[5] * b1 - m[6] * b0;
          m_inv[1] = 0 - m[1] * b5 + m[2] * b4 - m[3] * b3;
          m_inv[5] = 0 + m[0] * b5 - m[2] * b2 + m[3] * b1;
          m_inv[9] = 0 - m[0] * b4 + m[1] * b2 - m[3] * b0;
          m_inv[13] = 0 + m[0] * b3 - m[1] * b1 + m[2] * b0;
          m_inv[2] = 0 + m[13] * a5 - m[14] * a4 + m[15] * a3;
          m_inv[6] = 0 - m[12] * a5 + m[14] * a2 - m[15] * a1;
          m_inv[10] = 0 + m[12] * a4 - m[13] * a2 + m[15] * a0;
          m_inv[14] = 0 - m[12] * a3 + m[13] * a1 - m[14] * a0;
          m_inv[3] = 0 - m[9] * a5 + m[10] * a4 - m[11] * a3;
          m_inv[7] = 0 + m[8] * a5 - m[10] * a2 + m[11] * a1;
          m_inv[11] = 0 - m[8] * a4 + m[9] * a2 - m[11] * a0;
          m_inv[15] = 0 + m[8] * a3 - m[9] * a1 + m[10] * a0;

          float inverse_det = 1.0f / determinant;

          m_inv[0] *= inverse_det;
          m_inv[1] *= inverse_det;
          m_inv[2] *= inverse_det;
          m_inv[3] *= inverse_det;
          m_inv[4] *= inverse_det;
          m_inv[5] *= inverse_det;
          m_inv[6] *= inverse_det;
          m_inv[7] *= inverse_det;
          m_inv[8] *= inverse_det;
          m_inv[9] *= inverse_det;
          m_inv[10] *= inverse_det;
          m_inv[11] *= inverse_det;
          m_inv[12] *= inverse_det;
          m_inv[13] *= inverse_det;
          m_inv[14] *= inverse_det;
          m_inv[15] *= inverse_det;
          
      } else {
        m_inv = NULL;
        return;
      }
   }
      
   static void translate(float *mOut, float x, float y, float z) {    
      float m[16],result[16];
      
      mat4::set_identity(m);
      
      m[12] = x;
      m[13] = y;
      m[14] = z;

      mat4::multiply(result,mOut,m);
      mat4::copy(mOut,result);
   }

   static void rotate(float *mOut, float r, float x, float y, float z) {
      float m[16],result[16];
      
      mat4::set_identity(m);
      
	    float sAng,cAng;

	    sAng = sinf(r*(M_PI/180.0f));
	    cAng = cosf(r*(M_PI/180.0f));

      mat4_set(m, cAng+(x*x)*(1.0-cAng), x*y*(1.0-cAng) - z*sAng, x*z*(1.0-cAng) + y*sAng, 0,
                  y*x*(1.0-cAng)+z*sAng, cAng + y*y*(1.0-cAng), y*z*(1.0-cAng)-x*sAng, 0,
                  z*x*(1.0-cAng)-y*sAng, z*y*(1-cAng)+x*sAng, cAng+(z*z)*(1.0-cAng), 0, 
                  0, 0, 0, 1);
	
      mat4::multiply(result,mOut,m);
      mat4::copy(mOut,result);
   }


   static void rotate(float *mOut, float x, float y, float z) {
      float m[16],result[16];
	    float sAng,cAng;

	    if (x) {
	      sAng = sinf(x*(M_PI/180.0f));
	      cAng = cosf(x*(M_PI/180.0f));

        mat4::set_identity(m);
        
	      m[5] = cAng; m[6] = sAng;
	      m[9] = -sAng; m[10] = cAng;    
	    
        mat4::multiply(result,mOut,m);
        mat4::copy(mOut,result);
	    }
	    
	    if (y) {
	      sAng = sinf(y*(M_PI/180.0f));
	      cAng = cosf(y*(M_PI/180.0f));

        mat4::set_identity(m);
        
	      m[0] = cAng; m[2] = -sAng;
	      m[8] = sAng; m[10] = cAng;

        mat4::multiply(result,mOut,m);
        mat4::copy(mOut,result);
	    }
	    
	    if (z) {
	      sAng = sinf(z*(M_PI/180.0f));
	      cAng = cosf(z*(M_PI/180.0f));

        mat4::set_identity(m);

	      m[0] = cAng; m[1] = sAng;
	      m[4] = -sAng; m[5] = cAng;
	
        mat4::multiply(result,mOut,m);
        mat4::copy(mOut,result);
	    }
   }

   static void scale(float *mOut, float x, float y, float z) {    
      float m[16],result[16];
      
      mat4::set_identity(m);

      m[0] = x;
      m[5] = y;
      m[10] = z;

      mat4::multiply(result,mOut,m);
      mat4::copy(mOut,result);
   }
      
  };
}
