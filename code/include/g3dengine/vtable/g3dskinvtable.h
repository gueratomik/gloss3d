#ifndef _G3DSKINVTABLE_H_
#define _G3DSKINVTABLE_H_

#define G3DSKINVTABLECAST(a)     (((G3DSKINVTABLE*)a))

#include <g3dengine/g3dengine.h>
#include "g3dmodifiervtable.h"

/******************************************************************************/
typedef struct _G3DSKINVTABLE {
    G3DMODIFIERVTABLE modifierVTable;
} G3DSKINVTABLE;

/************************** Object vtable overrides ***************************/
void     g3dskin_default_free       ( G3DSKIN *skn );
void     g3dskin_default_update     ( G3DSKIN *skn,
                                      uint64_t updateFlags,
                                      uint64_t engineFlags );
void     g3dskin_default_activate   ( G3DSKIN  *skn, 
                                      uint64_t  engine_flags );
void     g3dskin_default_deactivate ( G3DSKIN  *skn,
                                      uint64_t  engine_flags );
G3DMESH *g3dskin_default_commit     ( G3DSKIN       *skn,
                                      uint32_t       id,
                                      unsigned char *name,
                                      uint64_t       engine_flags );

/**************************** Mesh vtable overrides ***************************/


/************************** Modifier vtable overrides *************************/
uint32_t g3dskin_default_modify  ( G3DSKIN     *skin,
                                   G3DMODIFYOP op,
                                   uint64_t    engine_flags );
uint32_t g3dskin_default_huddraw ( G3DSKIN   *skn,
                                   G3DCAMERA *cam, 
                                   uint64_t   engine_flags );

/******************************************************************************/

#define G3DSKINVTABLE_DEFAULT \
.modifierVTable = \
     { .meshVTable = \
        { .objectVTable = { .free       = FREE_CALLBACK(g3dskin_default_free), \
                            .update     = UPDATE_CALLBACK(g3dskin_default_update), \
                            .activate   = ACTIVATE_CALLBACK(g3dskin_default_activate), \
                            .deactivate = DEACTIVATE_CALLBACK(g3dskin_default_deactivate), \
                            .commit     = COMMIT_CALLBACK(g3dskin_default_commit) }, \
        }, \
        .modify = MODIFY_CALLBACK(g3dskin_default_modify), \
        .huddraw = HUDDRAW_CALLBACK(g3dskin_default_huddraw) \
    }

#endif /* _G3DSKINVTABLE_H_ */
