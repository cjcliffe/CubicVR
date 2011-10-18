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

#include <CubicVR/Envelope.h>


#include <CubicVR/cvr_defines.h>

/*
	Portions adapted from the work of Ernie Wright from the lightwave SDK
*/


/*
======================================================================
range()

Given the value v of a periodic function, returns the equivalent value
v2 in the principal interval [lo, hi].  If i isn't NULL, it receives
the number of wavelengths between v and v2.

   v2 = v - i * (hi - lo)

For example, range( 3 pi, 0, 2 pi, i ) returns pi, with i = 1.
====================================================================== */

static float range( float v, float lo, float hi, int *i )
{
   float v2, r = hi - lo;

   if ( r == 0.0 ) {
      if ( i ) *i = 0;
      return lo;
   }

   v2 = v - r * ( float ) floor(( v - lo ) / r );
   if ( i ) *i = -( int )(( v2 - v ) / r + ( v2 > v ? 0.5 : -0.5 ));

   return v2;
}


/*
======================================================================
hermite()

Calculate the Hermite coefficients.
====================================================================== */

static void hermite( float t, float *h1, float *h2, float *h3, float *h4 )
{
   float t2, t3;

   t2 = t * t;
   t3 = t * t2;

   *h2 = 3.0f * t2 - t3 - t3;
   *h1 = 1.0f - *h2;
   *h4 = t3 - t2;
   *h3 = *h4 - t2 + t;
}


/*
======================================================================
bezier()

Interpolate the value of a 1D Bezier curve.
====================================================================== */

static float bezier( float x0, float x1, float x2, float x3, float t )
{
   float a, b, c, t2, t3;

   t2 = t * t;
   t3 = t2 * t;

   c = 3.0f * ( x1 - x0 );
   b = 3.0f * ( x2 - x1 ) - c;
   a = x3 - x0 - c - b;

   return a * t3 + b * t2 + c * t + x0;
}


/*
======================================================================
bez2_time()

Find the t for which bezier() returns the input time.  The handle
endpoints of a BEZ2 curve represent the control points, and these have
(time, value) coordinates, so time is used as both a coordinate and a
parameter for this curve type.
====================================================================== */

static float bez2_time( float x0, float x1, float x2, float x3, float time,
   float *t0, float *t1 )
{
   float v, t;

   t = *t0 + ( *t1 - *t0 ) * 0.5f;
   v = bezier( x0, x1, x2, x3, t );
   if ( fabs( time - v ) > .0001f ) {
      if ( v > time )
         *t1 = t;
      else
         *t0 = t;
      return bez2_time( x0, x1, x2, x3, time, t0, t1 );
   }
   else
      return t;
}


/*
======================================================================
bez2()

Interpolate the value of a BEZ2 curve.
====================================================================== */

static float bez2( EnvelopeKey &key0, EnvelopeKey &key1, float time )
{
   float x, y, t, t0 = 0.0f, t1 = 1.0f;

   if ( key0.shape == ENV_SHAPE_BEZ2 )
      x = key0.time + key0.param[ 2 ];
   else
      x = key0.time + ( key1.time - key0.time ) / 3.0f;

   t = bez2_time( key0.time, x, key1.time + key1.param[ 0 ], key1.time,
      time, &t0, &t1 );

   if ( key0.shape == ENV_SHAPE_BEZ2 )
      y = key0.value + key0.param[ 3 ];
   else
      y = key0.value + key0.param[ 1 ] / 3.0f;

   return bezier( key0.value, y, key1.param[ 1 ] + key1.value, key1.value, t );
}


/*
======================================================================
outgoing()

Return the outgoing tangent to the curve at key0.  The value returned
for the BEZ2 case is used when extrapolating a linear pre behavior and
when interpolating a non-BEZ2 span.
====================================================================== */

static float outgoing( EnvelopeKey &key0, EnvelopeKey &key1 )
{
   float a, b, d, t, out;

   switch ( key0.shape )
   {
      case ENV_SHAPE_TCB:
         a = ( 1.0f - key0.tension )
           * ( 1.0f + key0.continuity )
           * ( 1.0f + key0.bias );
         b = ( 1.0f - key0.tension )
           * ( 1.0f - key0.continuity )
           * ( 1.0f - key0.bias );
         d = key1.value - key0.value;

         if ( key0.prev ) {
            t = ( key1.time - key0.time ) / ( key1.time - (*key0.prev).time );
            out = t * ( a * ( key0.value - (*key0.prev).value ) + b * d );
         }
         else
            out = b * d;
         break;

      case ENV_SHAPE_LINE:
         d = key1.value - key0.value;
         if ( key0.prev ) {
            t = ( key1.time - key0.time ) / ( key1.time - (*key0.prev).time );
            out = t * ( key0.value - (*key0.prev).value + d );
         }
         else
            out = d;
         break;

      case ENV_SHAPE_BEZI:
      case ENV_SHAPE_HERM:
         out = key0.param[ 1 ];
         if ( key0.prev )
            out *= ( key1.time - key0.time ) / ( key1.time - (*key0.prev).time );
         break;

      case ENV_SHAPE_BEZ2:
         out = key0.param[ 3 ] * ( key1.time - key0.time );
         if ( fabs( key0.param[ 2 ] ) > 1e-5f )
            out /= key0.param[ 2 ];
         else
            out *= 1e5f;
         break;

      case ENV_SHAPE_STEP:
      default:
         out = 0.0f;
         break;
   }

   return out;
}


/*
======================================================================
incoming()

Return the incoming tangent to the curve at key1.  The value returned
for the BEZ2 case is used when extrapolating a linear post behavior.
====================================================================== */

static float incoming( EnvelopeKey &key0, EnvelopeKey &key1 )
{
   float a, b, d, t, in;

   switch ( key1.shape )
   {
      case ENV_SHAPE_LINE:
         d = key1.value - key0.value;
         if ( key1.next ) {
            t = ( key1.time - key0.time ) / ( (*key1.next).time - key0.time );
            in = t * ( (*key1.next).value - key1.value + d );
         }
         else
            in = d;
         break;

      case ENV_SHAPE_TCB:
         a = ( 1.0f - key1.tension )
           * ( 1.0f - key1.continuity )
           * ( 1.0f + key1.bias );
         b = ( 1.0f - key1.tension )
           * ( 1.0f + key1.continuity )
           * ( 1.0f - key1.bias );
         d = key1.value - key0.value;

         if ( key1.next ) {
            t = ( key1.time - key0.time ) / ( (*key1.next).time - key0.time );
            in = t * ( b * ( (*key1.next).value - key1.value ) + a * d );
         }
         else
            in = a * d;
         break;

      case ENV_SHAPE_BEZI:
      case ENV_SHAPE_HERM:
         in = key1.param[ 0 ];
         if ( key1.next )
            in *= ( key1.time - key0.time ) / ( (*key1.next).time - key0.time );
         break;
         return in;

      case ENV_SHAPE_BEZ2:
         in = key1.param[ 1 ] * ( key1.time - key0.time );
         if ( fabs( key1.param[ 0 ] ) > 1e-5f )
            in /= key1.param[ 0 ];
         else
            in *= 1e5f;
         break;

      case ENV_SHAPE_STEP:
      default:
         in = 0.0f;
         break;
   }

   return in;
}


/*
======================================================================
evalEnvelope()

Given a list of keys and a time, returns the interpolated value of the
envelope at that time.
====================================================================== */

float Envelope::evaluate(float time)
{
   EnvelopeKey *key0, *key1, *skey, *ekey;
   float t, h1, h2, h3, h4, in, out, offset = 0.0f;
   int noff;


   /* if there's no key, the value is 0 */
   if ( nKeys == 0 ) return 0.0f;

   /* if there's only one key, the value is constant */
   if ( nKeys == 1 ) return (*keys).value;

   /* find the first and last keys */
   skey = ekey = keys;
   while ( ekey->next ) ekey = ekey->next;

   /* use pre-behavior if time is before first key time */
   if ( time < skey->time ) 
   {
      switch ( in_behavior )
      {
         case ENV_BEH_RESET: return 0.0f;

         case ENV_BEH_CONSTANT: return skey->value;

         case ENV_BEH_REPEAT: time = range( time, skey->time, ekey->time, NULL ); break;

         case ENV_BEH_OSCILLATE:
            time = range( time, skey->time, ekey->time, &noff );
            if ( noff % 2 )
               time = ekey->time - skey->time - time;
            break;

         case ENV_BEH_OFFSET:
            time = range( time, skey->time, ekey->time, &noff );
            offset = noff * ( ekey->value - skey->value );
            break;

         case ENV_BEH_LINEAR:
            out = outgoing( *skey, *skey->next )
                / ( skey->next->time - skey->time );
            return out * ( time - skey->time ) + skey->value;
      }
   }

   /* use post-behavior if time is after last key time */
   else if ( time > ekey->time ) 
   {
      switch ( out_behavior )
      {
         case ENV_BEH_RESET:
            return 0.0f;

         case ENV_BEH_CONSTANT:
            return ekey->value;

         case ENV_BEH_REPEAT:
            time = range( time, skey->time, ekey->time, NULL );
            break;

         case ENV_BEH_OSCILLATE:
            time = range( time, skey->time, ekey->time, &noff );
            if ( noff % 2 )
               time = ekey->time - skey->time - time;
            break;

         case ENV_BEH_OFFSET:
            time = range( time, skey->time, ekey->time, &noff );
            offset = noff * ( ekey->value - skey->value );
            break;

         case ENV_BEH_LINEAR:
            in = incoming( *ekey->prev, *ekey )
               / ( ekey->time - ekey->prev->time );
            return in * ( time - ekey->time ) + ekey->value;
      }
   }

   /* get the endpoints of the interval being evaluated */
   key0 = keys;
   while ( time > key0->next->time )
   {
    key0 = key0->next;
   }
   key1 = key0->next;

   /* check for singularities first */
   if ( time == key0->time )
      return key0->value + offset;
   else if ( time == key1->time )
      return key1->value + offset;

   /* get interval length, time in [0, 1] */
   t = ( time - key0->time ) / ( key1->time - key0->time );

   /* interpolate */
   switch ( key1->shape )
   {
      case ENV_SHAPE_TCB:
      case ENV_SHAPE_BEZI:
      case ENV_SHAPE_HERM:
         out = outgoing( *key0, *key1 );
         in = incoming( *key0, *key1 );
         hermite( t, &h1, &h2, &h3, &h4 );
         return h1 * key0->value + h2 * key1->value + h3 * out + h4 * in + offset;

      case ENV_SHAPE_BEZ2:
         return bez2( *key0, *key1, time ) + offset;

      case ENV_SHAPE_LINE:
         return key0->value + t * ( key1->value - key0->value ) + offset;

      case ENV_SHAPE_STEP:
         return key0->value + offset;

      default:
         return offset;
   }
};


EnvelopeKey::EnvelopeKey() : value(0), time(0), shape(ENV_SHAPE_TCB), tension(0), continuity(0), bias(0), prev(NULL), next(NULL)
{
	param[0] = 0;
	param[1] = 0;
	param[2] = 0;
	param[3] = 0;
};



Envelope::Envelope() : nKeys(0), keys(NULL)
{
	in_behavior = ENV_BEH_CONSTANT;
	out_behavior = ENV_BEH_CONSTANT;
};

Envelope::~Envelope()
{
	std::map<float, EnvelopeKey *, float_less>::iterator key_free_i;
	
	for (key_free_i = key_map.begin(); key_free_i != key_map.end(); key_free_i++)
	{
		delete (*key_free_i).second;
	}
};

void Envelope::behavior(int in_b, int out_b)
{
	in_behavior = in_b;
	out_behavior = out_b;
};

bool Envelope::empty()
{
	return (nKeys == 0);
};

EnvelopeKey *Envelope::addKey(float time, float value)
{
	EnvelopeKey *tempKey;
	
	tempKey = addKey(time);
	tempKey->value = value;
	
	return tempKey;
};

EnvelopeKey *Envelope::addKey(float time)
{
	EnvelopeKey *tempKey;
	
	tempKey = new EnvelopeKey();
	tempKey->time = time;
	
	key_map[time] = tempKey;
	
	EnvelopeKey *k1 = keys;
	
	if (!nKeys)
	{
		keys = tempKey;

		nKeys++;

		return tempKey;
	}
	
	while (k1)
	{
		if (k1->time > tempKey->time)
		{
			tempKey->prev = k1->prev;
			if (tempKey->prev)
			{
				tempKey->prev->next = tempKey;
			}
			
			tempKey->next = k1;
			tempKey->next->prev = tempKey;

			nKeys++;
				
			return tempKey;
		}
		else if (!k1->next)
		{
			tempKey->prev = k1;
			k1->next = tempKey;
			
			nKeys++;
			
			return tempKey;
		}
					
		k1 = k1->next;
	}
	
	delete tempKey;
	
	return NULL;	// you should not be here, the world has imploded
};




