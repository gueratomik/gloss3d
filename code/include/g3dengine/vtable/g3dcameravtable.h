#ifndef _G3DCAMERAVTABLE_H_
#define _G3DCAMERAVTABLE_H_

#define G3DCAMERAVTABLECAST(a)     (((G3DCAMERAVTABLE*)a))

#include <g3dengine/g3dengine.h>
#include "g3dobjectvtable.h"

/******************************************************************************/
typedef struct _G3DCAMERA G3DCAMERA;

/******************************************************************************/
typedef struct _G3DCAMERAVTABLE {
    G3DOBJECTVTABLE objectVTable;
} G3DCAMERAVTABLE;

/************************** Object vtable overrides ***************************/
uint32_t   g3dcamera_default_draw       ( G3DCAMERA *cam, 
                                          G3DCAMERA *curcam, 
                                          G3DENGINE *engine, 
                                          uint64_t   engineFlags );
void       g3dcamera_default_free       ( G3DCAMERA *cam );
uint32_t   g3dcamera_default_pick       ( G3DCAMERA *cam, 
                                          G3DCAMERA *curcam, 
                                          uint64_t   engineFlags );
void       g3dcamera_default_anim       ( G3DCAMERA *cam,
                                          float      frame,
                                          uint64_t   engineFlags );
void       g3dcamera_default_pose       ( G3DCAMERA *cam,
                                          G3DKEY    *key );
G3DCAMERA* g3dcamera_default_copy       ( G3DCAMERA  *cam,
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
