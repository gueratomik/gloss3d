#ifndef _G3DSPLINEREVOLVERVTABLE_H_
#define _G3DSPLINEREVOLVERVTABLE_H_

#define G3DSPLINEREVOLVERVTABLECAST(a)     (((G3DSPLINEREVOLVERVTABLE*)a))

#include <g3dengine/g3dengine.h>
#include "g3dmeshvtable.h"
#include "g3dmodifiervtable.h"

/******************************************************************************/
typedef struct _G3DSPLINEREVOLVERVTABLE {
    G3DMODIFIERVTABLE modifierVTable;
} G3DSPLINEREVOLVERVTABLE;

/************************** Object vtable overrides ***************************/
uint32_t           g3dsplinerevolver_default_draw       ( G3DSPLINEREVOLVER *spr,
                                                          G3DCAMERA         *cam, 
                                                          G3DENGINE         *engine, 
                                                          uint64_t           engineFlags );
void               g3dsplinerevolver_default_free       ( G3DSPLINEREVOLVER *spr );
void               g3dsplinerevolver_default_activate   ( G3DSPLINEREVOLVER *spr, 
                                                          uint64_t           engineFlags );
void               g3dsplinerevolver_default_deactivate ( G3DSPLINEREVOLVER *spr,
                                                          uint64_t           engineFlags );
G3DSPLINEREVOLVER *g3dsplinerevolver_default_copy       ( G3DSPLINEREVOLVER *spr, 
                                                          uint32_t           id,
                                                          const char        *name,
                                                          uint64_t           engineFlags );
void               g3dsplinerevolver_default_transform  ( G3DSPLINEREVOLVER *spr, 
                                                          uint64_t           engineFlags );
/*
G3DSPLINEREVOLVER *g3dsplinerevolver_default_commit     ( G3DSPLINEREVOLVER *spr,
                                                          uint32_t           id,
                                                          const char        *name,
                                                          uint64_t           engineFlags );
*/
void               g3dsplinerevolver_default_setParent  ( G3DSPLINEREVOLVER *spr, 
                                                          G3DOBJECT         *parent, 
                                                          G3DOBJECT         *oldParent, 
                                                          uint64_t           engineFlags );

/**************************** Mesh vtable overrides ***************************/


/************************** Modifier vtable overrides *************************/
uint32_t g3dsplinerevolver_default_modify ( G3DSPLINEREVOLVER *spr,
                                            G3DMODIFYOP        op,
                                            uint64_t           engineFlags );

/******************************************************************************/

#define G3DSPLINEREVOLVERVTABLE_DEFAULT \
.modifierVTable = \
     { .meshVTable = \
        { .objectVTable = { .draw       = DRAW_CALLBACK(g3dsplinerevolver_default_draw), \
                            .free       = FREE_CALLBACK(g3dsplinerevolver_default_free), \
                            .activate   = ACTIVATE_CALLBACK(g3dsplinerevolver_default_activate), \
                            .deactivate = DEACTIVATE_CALLBACK(g3dsplinerevolver_default_deactivate), \
                            .copy       = COPY_CALLBACK(g3dsplinerevolver_default_copy), \
                            .transform  = TRANSFORM_CALLBACK(g3dsplinerevolver_default_transform), \
                            .commit     = COMMIT_CALLBACK(g3dmesh_default_copy), \
                            .setParent  = SETPARENT_CALLBACK(g3dsplinerevolver_default_setParent) }, \
        }, \
        .modify = MODIFY_CALLBACK(g3dsplinerevolver_default_modify), \
    }

#endif /* _G3DSPLINEREVOLVERVTABLE_H_ */
