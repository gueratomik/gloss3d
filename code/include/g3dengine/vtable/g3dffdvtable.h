#ifndef _G3DFFDVTABLE_H_
#define _G3DFFDVTABLE_H_

#define G3DFFDVTABLECAST(a)     (((G3DFFDVTABLE*)a))

#include <g3dengine/g3dengine.h>
#include "g3dmodifiervtable.h"

/******************************************************************************/
typedef struct _G3DFFDVTABLE {
    G3DMODIFIERVTABLE modifierVTable;
} G3DFFDVTABLE;

/************************** Object vtable overrides ***************************/
uint32_t   g3dffd_default_draw       ( G3DFFD    *ffd,
                                       G3DCAMERA *cam, 
                                       G3DENGINE *engine, 
                                       uint64_t   engine_flags );

void       g3dffd_default_free       ( G3DOBJECT *obj );

void       g3dffd_default_activate   ( G3DFFD  *ffd, 
                                       uint64_t engine_flags );

void       g3dffd_default_deactivate ( G3DFFD  *ffd,
                                       uint64_t engine_flags );

G3DOBJECT *g3dffd_default_commit     ( G3DFFD        *ffd,
                                       uint32_t       id,
                                       unsigned char *name,
                                       uint64_t       engine_flags );

/**************************** Mesh vtable overrides ***************************/


/************************** Modifier vtable overrides *************************/
uint32_t g3dffd_default_modify ( G3DFFD     *ffd,
                                 G3DMODIFYOP op,
                                 uint64_t    engine_flags );

uint32_t g3dffd_default_huddraw ( G3DOBJECT *obj,
                                   G3DCAMERA *cam, 
                                   uint64_t   engine_flags );

/******************************************************************************/

#define G3DFFDVTABLE_DEFAULT \
.modifierVTable = \
     { .meshVTable = \
        { .objectVTable = { .draw       = DRAW_CALLBACK(g3dffd_default_draw), \
                            .free       = FREE_CALLBACK(g3dffd_default_free), \
                            .activate   = ACTIVATE_CALLBACK(g3dffd_default_activate), \
                            .deactivate = DEACTIVATE_CALLBACK(g3dffd_default_deactivate), \
                            .commit     = COMMIT_CALLBACK(g3dffd_default_commit) }, \
        }, \
        .modify = MODIFY_CALLBACK(g3dffd_default_modify), \
        .huddraw = HUDDRAW_CALLBACK(g3dffd_default_huddraw) \
    }

#endif /* _G3DFFDVTABLE_H_ */
