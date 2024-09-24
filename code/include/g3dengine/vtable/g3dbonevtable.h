#ifndef _G3DBONEVTABLE_H_
#define _G3DBONEVTABLE_H_

#define G3DBONEVTABLECAST(a)     (((G3DBONEVTABLE*)a))

#include "g3dobjectvtable.h"

/******************************************************************************/
typedef struct _G3DBONE G3DBONE;

/******************************************************************************/
typedef struct _G3DBONEVTABLE {
    G3DOBJECTVTABLE objectVTable;
} G3DBONEVTABLE;

/************************** Object vtable overrides ***************************/
uint32_t g3dbone_default_draw       ( G3DBONE *mes, 
                                      G3DCAMERA *curcam, 
                                      G3DENGINE *engine, 
                                      uint64_t   engineFlags );
void     g3dbone_default_free       ( G3DBONE *mes );
uint32_t g3dbone_default_pick       ( G3DBONE   *mes, 
                                      G3DCAMERA *curcam, 
                                      uint64_t   engineFlags );
void     g3dbone_default_transform  ( G3DBONE *, uint64_t );
void     g3dbone_default_activate   ( G3DBONE *, uint64_t );
void     g3dbone_default_deactivate ( G3DBONE *, uint64_t );

/******************************************************************************/
#define G3DBONEVTABLE_DEFAULT \
.objectVTable = { G3DOBJECTVTABLE_DEFAULT,  \
                 .draw       = DRAW_CALLBACK(g3dbone_default_draw), \
                 .free       = FREE_CALLBACK(g3dbone_default_free), \
                 .pick       = PICK_CALLBACK(g3dbone_default_pick), \
                 .transform  = TRANSFORM_CALLBACK(g3dbone_default_transform), \
                 .activate   = ACTIVATE_CALLBACK(g3dbone_default_activate), \
                 .deactivate = DEACTIVATE_CALLBACK(g3dbone_default_deactivate) }

#endif /* _G3DBONEVTABLE_H_ */



