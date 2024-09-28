#ifndef _G3DWIREFRAMEVTABLE_H_
#define _G3DWIREFRAMEVTABLE_H_

#define G3DWIREFRAMEVTABLECAST(a)     (((G3DWIREFRAMEVTABLE*)a))

#include <g3dengine/g3dengine.h>
#include "g3dmodifiervtable.h"

/******************************************************************************/
typedef struct _G3DWIREFRAMEVTABLE {
    G3DMODIFIERVTABLE modifierVTable;
} G3DWIREFRAMEVTABLE;

/************************** Object vtable overrides ***************************/
uint32_t   g3dwireframe_default_draw       ( G3DWIREFRAME *wir, 
                                             G3DCAMERA    *curcam, 
                                             G3DENGINE    *engine, 
                                             uint64_t      engineFlags );
void       g3dwireframe_default_free       ( G3DWIREFRAME *wir );
G3DOBJECT* g3dwireframe_default_copy       ( G3DWIREFRAME *wir,
                                             uint32_t      id,
                                             const char   *name,
                                             uint64_t      engineFlags );
void       g3dwireframe_default_activate   ( G3DWIREFRAME *wir,
                                             uint64_t      engineFlags );
void       g3dwireframe_default_deactivate ( G3DWIREFRAME *wir,
                                             uint64_t      engineFlags );

/**************************** Mesh vtable overrides ***************************/


/************************** Modifier vtable overrides *************************/
uint32_t g3dwireframe_default_modify ( G3DWIREFRAME *mpr,
                                       G3DMODIFYOP op,
                                       uint64_t engine_flags );

/******************************************************************************/

#define G3DWIREFRAMEVTABLE_DEFAULT \
.modifierVTable = \
{ \
    .meshVTable = \
    { \
        .objectVTable = \
        { .draw = DRAW_CALLBACK(g3dwireframe_default_draw), \
          .free       = FREE_CALLBACK(g3dwireframe_default_free), \
          .copy       = COPY_CALLBACK(g3dwireframe_default_copy), \
          .activate   = ACTIVATE_CALLBACK(g3dwireframe_default_activate), \
          .deactivate = DEACTIVATE_CALLBACK(g3dwireframe_default_deactivate) \
        } \
    }, \
    .modify = MODIFY_CALLBACK(g3dwireframe_default_modify) \
}

#endif /* _G3DWIREFRAMEVTABLE_H_ */
