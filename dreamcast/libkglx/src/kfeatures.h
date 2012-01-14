#ifndef _KGL_features_h_
#define _KGL_features_h_

/* It is possible to manipulate (at compile time) features in this
   header file. */

/* max. number of Displaylists */

#ifndef REDUCED_MEMORY
#define MAX_DISPLAY_LISTS 512
#else
#define MAX_DISPLAY_LISTS 256
#endif

/* max. opcode sizes of one Displaylist */

#ifndef REDUCED_MEMORY
#define OP_BUFFER_MAX_SIZE 512
#else
#define OP_BUFFER_MAX_SIZE 256
#endif


/* Size of max verticies in a glBegin()...glEnd() block */

#ifndef REDUCED_MEMORY
#define VBUF_SIZE (1024*8)
#else
#define VBUF_SIZE (1024)
#endif

#endif /* _KGL_features_h_ */
