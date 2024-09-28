#ifndef _G3DSPLINEVTABLE_H_
#define _G3DSPLINEVTABLE_H_

#define G3DSPLINEVTABLECAST(a)     (((G3DSPLINEVTABLE*)a))

#include <g3dengine/g3dengine.h>
#include "g3dobjectvtable.h"


/******************************************************************************/
typedef struct _G3DSPLINEVTABLE {
    G3DOBJECTVTABLE objectVTable;
} G3DSPLINEVTABLE;

/************************** Object vtable overrides ***************************/
uint32_t g3dspline_default_draw   ( G3DSPLINE *spline, 
                                    G3DCAMERA *curcam, 
                                    G3DENGINE *engine, 
                                    uint64_t   engineFlags );
void     g3dspline_default_free   ( G3DSPLINE *spline );
uint32_t g3dspline_default_pick   ( G3DSPLINE *spline, 
                                    G3DCAMERA *curcam, 
                                    uint64_t   engineFlags );
void     g3dspline_default_update ( G3DSPLINE *spline,
                                    uint64_t   updateFlags,
                                    uint64_t   engineFlags );
G3DSPLINE *g3dspline_default_copy ( G3DSPLINE  *spline, 
                                    uint32_t    id, 
                                    const char *name,
                                    uint64_t    engineFlags );

/******************************************************************************/

#define G3DSPLINEVTABLE_DEFAULT \
.objectVTable = \
    { G3DOBJECTVTABLE_DEFAULT,  \
      .draw   = DRAW_CALLBACK(g3dspline_default_draw), \
      .free   = FREE_CALLBACK(g3dspline_default_free), \
      .pick   = PICK_CALLBACK(g3dspline_default_pick), \
      .update = UPDATE_CALLBACK(g3dspline_default_update), \
      .copy   = COPY_CALLBACK(g3dspline_default_copy) \
    }

#endif /* _G3DSPLINEVTABLE_H_ */
