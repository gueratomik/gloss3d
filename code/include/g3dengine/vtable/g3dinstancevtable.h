#ifndef _G3DINSTANCEVTABLE_H_
#define _G3DINSTANCEVTABLE_H_

#define G3DINSTANCEVTABLECAST(a)     (((G3DINSTANCEVTABLE*)a))

#include <g3dengine/g3dengine.h>
#include "g3dobjectvtable.h"

/******************************************************************************/
typedef struct _G3DINSTANCEVTABLE {
    G3DOBJECTVTABLE objectVTable;
} G3DINSTANCEVTABLE;

/************************** Object vtable overrides ***************************/
uint32_t     g3dinstance_default_draw ( G3DINSTANCE *ffd,
                                        G3DCAMERA   *cam, 
                                        G3DENGINE   *engine, 
                                        uint64_t     engine_flags );

uint32_t     g3dinstance_default_pick ( G3DINSTANCE *ins, 
                                        G3DCAMERA   *curcam, 
                                        uint64_t     engine_flags );

G3DINSTANCE *g3dinstance_default_copy ( G3DINSTANCE   *ins, 
                                        uint32_t       id, 
                                        unsigned char *name,
                                        uint64_t       engine_flags );

/******************************************************************************/

#define G3DINSTANCEVTABLE_DEFAULT \
.objectVTable = { .draw = DRAW_CALLBACK(g3dinstance_default_draw), \
                  .pick = PICK_CALLBACK(g3dinstance_default_pick), \
                  .copy = COPY_CALLBACK(g3dinstance_default_copy) }


#endif /* _G3DINSTANCEVTABLE_H_ */
