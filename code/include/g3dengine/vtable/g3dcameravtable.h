#ifndef _G3DCAMERAVTABLE_H_
#define _G3DCAMERAVTABLE_H_

#define G3DCAMERAVTABLECAST(a)     (((G3DCAMERAVTABLE*)a))

#include "g3dobjectvtable.h"

/******************************************************************************/
typedef struct _G3DCAMERA G3DCAMERA;

/******************************************************************************/
typedef struct _G3DCAMERAVTABLE {
    G3DOBJECTVTABLE objectVTable;
} G3DCAMERAVTABLE;

/************************** Object vtable overrides ***************************/
uint32_t   g3dcamera_default_draw       ( G3DCAMERA *mes, 
                                          G3DCAMERA *curcam, 
                                          G3DENGINE *engine, 
                                          uint64_t   engineFlags );
void       g3dcamera_default_free       ( G3DCAMERA *mes );
uint32_t   g3dcamera_default_pick       ( G3DCAMERA   *mes, 
                                          G3DCAMERA *curcam, 
                                          uint64_t   engineFlags );
void       g3dcamera_default_anim       ( G3DOBJECT *obj,
                                          float      frame,
                                          uint64_t   engineFlags );
void       g3dcamera_default_pose       ( G3DOBJECT *obj,
                                          G3DKEY    *key );
G3DOBJECT* g3dcamera_default_copy       ( G3DOBJECT  *obj,
                                          uint32_t    id,
                                          const char *name,
                                          uint64_t    engineFlags );
void       g3dcamera_default_transform  ( G3DCAMERA *, uint64_t );

/******************************************************************************/
#define G3DCAMERAVTABLE_DEFAULT \
.objectVTable = { G3DOBJECTVTABLE_DEFAULT,  \
                 .draw       = DRAW_CALLBACK(g3dcamera_default_draw), \
                 .free       = FREE_CALLBACK(g3dcamera_default_free), \
                 .pick       = PICK_CALLBACK(g3dcamera_default_pick), \
                 .anim       = ANIM_CALLBACK(g3dcamera_default_anim), \
                 .pose       = POSE_CALLBACK(g3dcamera_default_pose), \
                 .copy       = COPY_CALLBACK(g3dcamera_default_copy), \
                 .transform  = TRANSFORM_CALLBACK(g3dcamera_default_transform) }

#endif /* _G3DCAMERAVTABLE_H_ */
