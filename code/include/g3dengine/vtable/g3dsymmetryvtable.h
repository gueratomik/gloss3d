#ifndef _G3DSYMMETRYVTABLE_H_
#define _G3DSYMMETRYVTABLE_H_

#define G3DSYMMETRYVTABLECAST(a)     (((G3DSYMMETRYVTABLE*)a))

#include <g3dengine/g3dengine.h>
#include "g3dobjectvtable.h"

/******************************************************************************/
typedef struct _G3DSYMMETRY G3DSYMMETRY;

/******************************************************************************/
typedef struct _G3DSYMMETRYVTABLE {
    G3DOBJECTVTABLE objectVTable;
} G3DSYMMETRYVTABLE;

/************************** Object vtable overrides ***************************/
uint32_t     g3dsymmetry_default_draw   ( G3DSYMMETRY *sym, 
                                          G3DCAMERA   *curcam, 
                                          G3DENGINE   *engine, 
                                          uint64_t     engineFlags );
void         g3dsymmetry_default_free   ( G3DSYMMETRY *sym );
G3DSYMMETRY* g3dsymmetry_default_copy   ( G3DSYMMETRY *sym,
                                          uint32_t     id,
                                          const char  *name,
                                          uint64_t     engineFlags );
G3DOBJECT*   g3dsymmetry_default_commit ( G3DSYMMETRY *sym,
                                          uint32_t     id,
                                          const char  *name,
                                          uint64_t     engineFlags );

/******************************************************************************/
#define G3DSYMMETRYVTABLE_DEFAULT \
.objectVTable = { G3DOBJECTVTABLE_DEFAULT,  \
                 .draw   = DRAW_CALLBACK(g3dsymmetry_default_draw), \
                 .free   = FREE_CALLBACK(g3dsymmetry_default_free), \
                 .copy   = COPY_CALLBACK(g3dsymmetry_default_copy), \
                 .commit = COMMIT_CALLBACK(g3dsymmetry_default_commit) }

#endif /* _G3DSYMMETRYVTABLE_H_ */
