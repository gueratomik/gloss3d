#ifndef _G3DMESHVTABLE_H_
#define _G3DMESHVTABLE_H_

#define G3DMESHVTABLECAST(a)     (((G3DMESHVTABLE*)a))

#include "g3dobjectvtable.h"

/******************************************************************************/
typedef struct _G3DFACE     G3DFACE;
typedef struct _G3DVECTOR3F G3DVECTOR3F;

/******************************************************************************/
typedef struct _G3DMESHVTABLE {
    G3DOBJECTVTABLE objectVTable;
    uint32_t ( *const dump) ( G3DMESH *,
                              void (*Alloc)( uint32_t, /* nbver */
                                             uint32_t, /* nbtris */
                                             uint32_t, /* nbquads */
                                             uint32_t, /* nbuv */
                                             void * ),
                              void (*Dump) ( G3DFACE *,
                                             G3DVECTOR3F *,
                                             G3DVECTOR3F *,
                                             void * ),
                              void *data,
                              uint64_t );
} G3DMESHVTABLE;

/************************** Object vtable overrides ***************************/
uint32_t g3dmesh_default_draw   ( G3DMESH *mes, 
                                  G3DCAMERA *curcam, 
                                  G3DENGINE *engine, 
                                  uint64_t   engineFlags );
void     g3dmesh_default_free   ( G3DMESH *mes );
uint32_t g3dmesh_default_pick   ( G3DMESH   *mes, 
                                  G3DCAMERA *curcam, 
                                  uint64_t   engineFlags );
void     g3dmesh_default_update ( G3DMESH *mes,
                                  uint64_t updateFlags,
                                  uint64_t engineFlags );
G3DMESH *g3dmesh_default_copy   ( G3DMESH       *mes, 
                                  uint32_t       id, 
                                  unsigned char *name,
                                  uint64_t       engineFlags );

/******************************************************************************/
uint32_t g3dmesh_default_dump   ( G3DMESH *mes, 
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

/******************************************************************************/

#define G3DMESHVTABLE_DEFAULT \
.objectVTable = { G3DOBJECTVTABLE_DEFAULT,  \
                  .draw   = DRAW_CALLBACK(g3dmesh_default_draw), \
                  .free   = FREE_CALLBACK(g3dmesh_default_free), \
                  .pick   = PICK_CALLBACK(g3dmesh_default_pick), \
                  .update = UPDATE_CALLBACK(g3dmesh_default_update), \
                  .copy   = COPY_CALLBACK(g3dmesh_default_copy) }, \
.dump = g3dmesh_default_dump


#endif /* _G3DMESHVTABLE_H_ */
