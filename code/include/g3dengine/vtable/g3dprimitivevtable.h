#ifndef _G3DPRIMITIVEVTABLE_H_
#define _G3DPRIMITIVEVTABLE_H_

#define G3DPRIMITIVEVTABLECAST(a)     (((G3DPRIMITIVEVTABLE*)a))

#include <g3dengine/g3dengine.h>
#include "g3dmeshvtable.h"

/******************************************************************************/
typedef struct _G3DPRIMITIVEVTABLE {
    G3DMESHVTABLE meshVTable;
} G3DPRIMITIVEVTABLE;

/************************** Object vtable overrides ***************************/
uint32_t      g3dprimitive_default_draw ( G3DPRIMITIVE *pri, 
                                          G3DCAMERA    *curcam, 
                                          G3DENGINE    *engine, 
                                          uint64_t      engineFlags );
void          g3dprimitive_default_free ( G3DPRIMITIVE *pri );
uint32_t      g3dprimitive_default_pick ( G3DOBJECT    *obj, 
                                          G3DCAMERA    *cam, 
                                          uint64_t      engineFlags );
G3DPRIMITIVE* g3dprimitive_default_copy ( G3DPRIMITIVE *pri,
                                          uint32_t      id,
                                          const char   *name,
                                          uint64_t      engineFlags );

/************************** Mesh vtable overrides ***************************/


/******************************************************************************/

#define G3DPRIMITIVEVTABLE_DEFAULT \
.meshVTable = { \
    .objectVTable = { \
        .draw = DRAW_CALLBACK(g3dprimitive_default_draw), \
        .free = FREE_CALLBACK(g3dprimitive_default_free), \
        .copy = COPY_CALLBACK(g3dprimitive_default_copy) } }

#endif /* _G3DPRIMITIVEVTABLE_H_ */
