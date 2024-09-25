#ifndef _G3DLIGHTVTABLE_H_
#define _G3DLIGHTVTABLE_H_

#define G3DLIGHTVTABLECAST(a)     (((G3DLIGHTVTABLE*)a))

#include <g3dengine/g3dengine.h>
#include "g3dobjectvtable.h"

/******************************************************************************/
typedef struct _G3DLIGHTVTABLE {
    G3DOBJECTVTABLE objectVTable;
} G3DLIGHTVTABLE;

/************************** Object vtable overrides ***************************/
uint32_t  g3dlight_default_draw ( G3DLIGHT  *ffd,
                                  G3DCAMERA *cam, 
                                  G3DENGINE *engine, 
                                  uint64_t   engine_flags );

uint32_t  g3dlight_default_pick ( G3DLIGHT  *ins, 
                                  G3DCAMERA *curcam, 
                                  uint64_t   engine_flags );

void      g3dlight_default_anim ( G3DLIGHT *lig, 
                                  float     frame, 
                                  uint64_t  engine_flags );

void      g3dlight_default_pose ( G3DLIGHT *lig,
                                  G3DKEY   *key );

G3DLIGHT *g3dlight_default_copy ( G3DLIGHT      *ins, 
                                  uint32_t      id, 
                                  unsigned char *name,
                                  uint64_t       engine_flags );

/******************************************************************************/

#define G3DLIGHTVTABLE_DEFAULT \
.objectVTable = { .draw = DRAW_CALLBACK(g3dlight_default_draw), \
                  .pick = PICK_CALLBACK(g3dlight_default_pick), \
                  .anim = ANIM_CALLBACK(g3dlight_default_anim), \
                  .pose = POSE_CALLBACK(g3dlight_default_pose), \
                  .copy = COPY_CALLBACK(g3dlight_default_copy) }


#endif /* _G3DLIGHTVTABLE_H_ */
