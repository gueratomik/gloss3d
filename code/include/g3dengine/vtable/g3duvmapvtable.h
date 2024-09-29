#ifndef _G3DUVMAPVTABLE_H_
#define _G3DUVMAPVTABLE_H_

#define G3DUVMAPVTABLECAST(a)     (((G3DUVMAPVTABLE*)a))

#include <g3dengine/g3dengine.h>
#include "g3dmeshvtable.h"

/******************************************************************************/
typedef struct _G3DUVMAPVTABLE {
    G3DMESHVTABLE meshVTable;
} G3DUVMAPVTABLE;

/************************** Object vtable overrides ***************************/
uint32_t g3duvmap_default_draw      ( G3DUVMAP   *uvmap,
                                      G3DCAMERA *curcam,
                                      G3DENGINE *engine,
                                      uint64_t   engineFlags );
void     g3duvmap_default_transform ( G3DUVMAP   *uvmap,
                                      uint64_t   engineFlags );

/************************** Mesh vtable overrides ***************************/


/******************************************************************************/

#define G3DUVMAPVTABLE_DEFAULT \
.meshVTable = \
{ \
    .objectVTable = \
    { \
        .draw = DRAW_CALLBACK(g3duvmap_default_draw), \
        .transform = TRANSFORM_CALLBACK(g3duvmap_default_transform) \
    } \
}

#endif /* _G3DUVMAPVTABLE_H_ */
