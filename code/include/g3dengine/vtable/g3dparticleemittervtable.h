#ifndef _G3DPARTICLEEMITTERVTABLE_H_
#define _G3DPARTICLEEMITTERVTABLE_H_

#define G3DPARTICLEEMITTERVTABLECAST(a)     (((G3DPARTICLEEMITTERVTABLE*)a))

#include <g3dengine/g3dengine.h>
#include "g3dobjectvtable.h"

/******************************************************************************/
typedef struct _G3DPARTICLEEMITTER G3DPARTICLEEMITTER;

/******************************************************************************/
typedef struct _G3DPARTICLEEMITTERVTABLE {
    G3DOBJECTVTABLE objectVTable;
} G3DPARTICLEEMITTERVTABLE;

/************************** Object vtable overrides ***************************/
uint32_t            g3dparticleemitter_default_draw ( G3DPARTICLEEMITTER *pem, 
                                                      G3DCAMERA          *curcam, 
                                                      G3DENGINE          *engine, 
                                                      uint64_t            engineFlags );
void                g3dparticleemitter_default_free ( G3DPARTICLEEMITTER *pem );
void                g3dparticleemitter_default_anim ( G3DPARTICLEEMITTER *pem,
                                                      float               frame,
                                                      uint64_t            engineFlags );
void                g3dparticleemitter_default_pose ( G3DPARTICLEEMITTER *pem,
                                                      G3DKEY             *key );
G3DPARTICLEEMITTER* g3dparticleemitter_default_copy ( G3DPARTICLEEMITTER *pem,
                                                      uint32_t            id,
                                                      const char         *name,
                                                      uint64_t            engineFlags );

/******************************************************************************/
#define G3DPARTICLEEMITTERVTABLE_DEFAULT \
.objectVTable = { G3DOBJECTVTABLE_DEFAULT,  \
                 .draw       = DRAW_CALLBACK(g3dparticleemitter_default_draw), \
                 .free       = FREE_CALLBACK(g3dparticleemitter_default_free), \
                 .anim       = ANIM_CALLBACK(g3dparticleemitter_default_anim), \
                 .pose       = POSE_CALLBACK(g3dparticleemitter_default_pose), \
                 .copy       = COPY_CALLBACK(g3dparticleemitter_default_copy) }

#endif /* _G3DPARTICLEEMITTERVTABLE_H_ */
