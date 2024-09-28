#ifndef _G3DTEXTVTABLE_H_
#define _G3DTEXTVTABLE_H_

#define G3DTEXTVTABLECAST(a)     (((G3DTEXTVTABLE*)a))

#include <g3dengine/g3dengine.h>
#include "g3dmeshvtable.h"

/******************************************************************************/
typedef struct _G3DTEXTVTABLE {
    G3DMESHVTABLE meshVTable;
} G3DTEXTVTABLE;

/************************** Object vtable overrides ***************************/
uint32_t g3dtext_default_draw ( G3DTEXT   *txt,
                                G3DCAMERA *curcam,
                                G3DENGINE *engine,
                                uint64_t   engineFlags );
void     g3dtext_default_free ( G3DTEXT *txt );
uint32_t g3dtext_default_pick ( G3DTEXT   *txt,
                                G3DCAMERA *cam, 
                                uint64_t   engineFlags );
G3DTEXT* g3dtext_default_copy ( G3DTEXT    *txt,
                                uint32_t    id,
                                const char *name,
                                uint64_t    engineFlags );

/************************** Mesh vtable overrides ***************************/


/******************************************************************************/

#define G3DTEXTVTABLE_DEFAULT \
.meshVTable = { \
    .objectVTable = { \
        .draw = DRAW_CALLBACK(g3dtext_default_draw), \
        .free = FREE_CALLBACK(g3dtext_default_free), \
        .pick = PICK_CALLBACK(g3dtext_default_pick), \
        .copy = COPY_CALLBACK(g3dtext_default_copy) } }

#endif /* _G3DTEXTVTABLE_H_ */
