#ifndef _G3DMODIFIERVTABLE_H_
#define _G3DMODIFIERVTABLE_H_

#define G3DMODIFIERVTABLECAST(a)     (((G3DMODIFIERVTABLE*)a))

#include <g3dengine/g3dengine.h>
#include "g3dmeshvtable.h"

/******************************************************************************/
typedef struct _G3DMODIFIERVTABLE {
    G3DMESHVTABLE meshVTable;
    uint32_t    (*huddraw) ( G3DMODIFIER *mod,
                             G3DCAMERA   *curcam,
                             G3DENGINE   *engine,
                             uint64_t     engineFlags );
    uint32_t    (*hudpick) ( G3DMODIFIER *mod,
                             G3DCAMERA   *curcam,
                             uint64_t     engineFlags );
    uint32_t    (*modify)  ( G3DMODIFIER *mod,
                             G3DMODIFYOP  op,
                             uint64_t     engineFlags );
} G3DMODIFIERVTABLE;

/************************** Object vtable overrides ***************************/


/************************** Mesh vtable overrides ***************************/
uint32_t g3dmodifier_default_dump ( G3DMODIFIER *mod, 
                                    void (*Alloc)( uint32_t, /*vertexCount */
                                                   uint32_t, /* nbtris */
                                                   uint32_t, /* nbquads */
                                                   uint32_t, /* nbuv */
                                                   void * ),
                                    void (*Dump) ( G3DFACE *,
                                                   G3DVECTOR3F *,
                                                   G3DVECTOR3F *,
                                                   void * ),
                                    void *data,
                                    uint64_t engineFlags );


/******************************************************************************/

uint32_t g3dmodifier_default_hudpick ( G3DMODIFIER *mod,
                                       G3DCAMERA   *curcam, 
                                       uint64_t     engineFlags );

uint32_t g3dmodifier_default_huddraw ( G3DMODIFIER *mod,
                                       G3DCAMERA   *curcam, 
                                       G3DENGINE   *engine, 
                                       uint64_t     engineFlags );

uint32_t g3dmodifier_default_modify  ( G3DMODIFIER *mod, 
                                       G3DMODIFYOP  op,
                                       uint64_t     engineFlags );

/******************************************************************************/

#define G3DMODIFIERVTABLE_DEFAULT \
.meshVTable = { .dump = g3dmodifier_default_dump }, \
.huddraw = g3dmodifier_default_huddraw, \
.hudpick = g3dmodifier_default_hudpick, \
.modify = g3dmodifier_default_modify

#endif /* _G3DMODIFIERVTABLE_H_ */
