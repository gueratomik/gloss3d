#ifndef _G3DSCENEVTABLE_H_
#define _G3DSCENEVTABLE_H_

#define G3DSCENEVTABLECAST(a)     (((G3DSCENEVTABLE*)a))

#include <g3dengine/g3dengine.h>
#include "g3dobjectvtable.h"

/******************************************************************************/
typedef struct _G3DSCENE G3DSCENE;

/******************************************************************************/
typedef struct _G3DSCENEVTABLE {
    G3DOBJECTVTABLE objectVTable;
} G3DSCENEVTABLE;

/************************** Object vtable overrides ***************************/
void       g3dscene_default_free       ( G3DOBJECT *obj );
void       g3dscene_default_anim       ( G3DOBJECT *obj,
                                         float      frame,
                                         uint64_t   engineFlags );
void       g3dscene_default_update     ( G3DOBJECT *obj,
                                         uint64_t   updateFlags,
                                         uint64_t   engineFlags );
void       g3dscene_default_activate   ( G3DOBJECT *obj,
                                         uint64_t   engineFlags );
void       g3dscene_default_deactivate ( G3DOBJECT *obj,
                                         uint64_t engineFlags );

/******************************************************************************/
#define G3DSCENEVTABLE_DEFAULT \
.objectVTable = { G3DOBJECTVTABLE_DEFAULT, \
                 .free       = FREE_CALLBACK(g3dobject_default_free),  \
                 .anim       = ANIM_CALLBACK(g3dobject_default_anim),  \
                 .update     = UPDATE_CALLBACK(g3dobject_default_update),  \
                 .activate   = ACTIVATE_CALLBACK(g3dobject_default_activate), \
                 .deactivate = DEACTIVATE_CALLBACK(g3dobject_default_deactivate) }

#endif /* _G3DSCENEVTABLE_H_ */
