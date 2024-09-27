#ifndef _G3DMORPHERVTABLE_H_
#define _G3DMORPHERVTABLE_H_

#define G3DMORPHERVTABLECAST(a)     (((G3DMORPHERVTABLE*)a))

#include <g3dengine/g3dengine.h>
#include "g3dmodifiervtable.h"

/******************************************************************************/
typedef struct _G3DMORPHERVTABLE {
    G3DMODIFIERVTABLE modifierVTable;
} G3DMORPHERVTABLE;

/************************** Object vtable overrides ***************************/
void       g3dmorpher_default_free       ( G3DMORPHER *mpr );
uint32_t   g3dmorpher_default_pick       ( G3DMORPHER *obj, 
                                           G3DCAMERA *cam, 
                                           uint64_t   engineFlags );
void       g3dmorpher_default_anim       ( G3DMORPHER *mpr,
                                          float      frame,
                                          uint64_t   engineFlags );
void       g3dmorpher_default_update     ( G3DMORPHER *mpr,
                                           uint64_t   updateFlags,
                                           uint64_t   engineFlags );
void       g3dmorpher_default_pose       ( G3DMORPHER *mpr,
                                           G3DKEY    *key );
void       g3dmorpher_default_activate   ( G3DMORPHER *mpr,
                                           uint64_t   engineFlags );
void       g3dmorpher_default_deactivate ( G3DMORPHER *mpr,
                                           uint64_t engineFlags );

/**************************** Mesh vtable overrides ***************************/


/************************** Modifier vtable overrides *************************/
uint32_t g3dmorpher_default_modify ( G3DMORPHER *mpr,
                                     G3DMODIFYOP op,
                                     uint64_t engine_flags );
uint32_t g3dmorpher_default_huddraw ( G3DMORPHER *mpr, 
                                     G3DCAMERA  *cam,
                                     uint64_t    engine_flags );

/******************************************************************************/

#define G3DMORPHERVTABLE_DEFAULT \
.modifierVTable = \
     { .meshVTable = \
        { .objectVTable = { .free       = FREE_CALLBACK(g3dmorpher_default_free), \
                            .pick       = PICK_CALLBACK(g3dmorpher_default_pick), \
                            .anim       = ANIM_CALLBACK(g3dmorpher_default_anim), \
                            .update     = UPDATE_CALLBACK(g3dmorpher_default_update), \
                            .pose       = POSE_CALLBACK(g3dmorpher_default_pose), \
                            .activate   = ACTIVATE_CALLBACK(g3dmorpher_default_activate), \
                            .deactivate = DEACTIVATE_CALLBACK(g3dmorpher_default_deactivate) }, \
        }, \
        .modify = MODIFY_CALLBACK(g3dmorpher_default_modify), \
        .huddraw = HUDDRAW_CALLBACK(g3dmorpher_default_huddraw) \
    }

#endif /* _G3DMORPHERVTABLE_H_ */
