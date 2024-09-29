#ifndef _G3DSUBDIVIDERVTABLE_H_
#define _G3DSUBDIVIDERVTABLE_H_

#define G3DSUBDIVIDERVTABLECAST(a)     (((G3DSUBDIVIDERVTABLE*)a))

#include <g3dengine/g3dengine.h>
#include "g3dmodifiervtable.h"

/******************************************************************************/
typedef struct _G3DSUBDIVIDERVTABLE {
    G3DMODIFIERVTABLE modifierVTable;
} G3DSUBDIVIDERVTABLE;

/************************** Object vtable overrides ***************************/
uint32_t       g3dsubdivider_default_draw       ( G3DSUBDIVIDER *sdv,
                                                  G3DCAMERA     *cam, 
                                                  G3DENGINE     *engine,
                                                  uint64_t       engineFlags );
void           g3dsubdivider_default_free       ( G3DSUBDIVIDER *sdv );
uint32_t       g3dsubdivider_default_pick       ( G3DSUBDIVIDER *sdv, 
                                                  G3DCAMERA     *cam,
                                                  uint64_t       engineFlags );
void           g3dsubdivider_default_update     ( G3DSUBDIVIDER *sdv,
                                                  uint64_t       updateFlags,
                                                  uint64_t       engineFlags );
G3DSUBDIVIDER* g3dsubdivider_default_copy       ( G3DSUBDIVIDER *sdv,
                                                  uint32_t       id,
                                                  const char    *name,
                                                  uint64_t       engineFlags );
void           g3dsubdivider_default_activate   ( G3DSUBDIVIDER  *sdv, 
                                                  uint64_t        engineFlags );
void           g3dsubdivider_default_deactivate ( G3DSUBDIVIDER  *sdv,
                                                  uint64_t        engineFlags );
G3DMESH       *g3dsubdivider_default_commit     ( G3DSUBDIVIDER        *sdv,
                                                  uint32_t       id,
                                                  unsigned char *name,
                                                  uint64_t       engineFlags );
void           g3dsubdivider_default_setParent  ( G3DSUBDIVIDER *sdv, 
                                                  G3DOBJECT     *parent, 
                                                  G3DOBJECT     *oldParent, 
                                                  uint64_t       engineFlags );

/**************************** Mesh vtable overrides ***************************/
uint32_t g3dsubdivider_default_dump   ( G3DSUBDIVIDER *sdv,
                                        void   (*Alloc)( uint32_t, /*vertexCount */
                                                         uint32_t, /* nbtris */
                                                         uint32_t, /* nbquads */
                                                         uint32_t, /* nbuv */
                                                         void * ),
                                        void   (*Dump) ( G3DFACE *,
                                                         G3DVECTOR3F *,
                                                         G3DVECTOR3F *,
                                                         void * ),
                                        void    *data,
                                        uint64_t engineFlags );

/************************** Modifier vtable overrides *************************/
uint32_t g3dsubdivider_default_modify ( G3DSUBDIVIDER *sdv,
                                        G3DMODIFYOP    op,
                                        uint64_t       engineFlags );

/******************************************************************************/

#define G3DSUBDIVIDERVTABLE_DEFAULT \
.modifierVTable = \
{ \
    .meshVTable = \
    { \
        .objectVTable =  \
        { .draw       = DRAW_CALLBACK(g3dsubdivider_default_draw), \
          .free       = FREE_CALLBACK(g3dsubdivider_default_free), \
          .pick       = PICK_CALLBACK(g3dsubdivider_default_pick), \
          .update     = UPDATE_CALLBACK(g3dsubdivider_default_update), \
          .copy       = COPY_CALLBACK(g3dsubdivider_default_copy), \
          .activate   = ACTIVATE_CALLBACK(g3dsubdivider_default_activate), \
          .deactivate = DEACTIVATE_CALLBACK(g3dsubdivider_default_deactivate), \
          .commit     = COMMIT_CALLBACK(g3dsubdivider_default_commit), \
          .setParent  = SETPARENT_CALLBACK(g3dsubdivider_default_setParent) \
        }, \
        .dump = DUMP_CALLBACK(g3dsubdivider_default_dump) \
    }, \
    .modify = MODIFY_CALLBACK(g3dsubdivider_default_modify), \
}

#endif /* _G3DSUBDIVIDERVTABLE_H_ */
