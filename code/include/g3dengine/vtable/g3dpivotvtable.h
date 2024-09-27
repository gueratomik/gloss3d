#ifndef _G3DPIVOTVTABLE_H_
#define _G3DPIVOTVTABLE_H_

#define G3DPIVOTVTABLECAST(a)     (((G3DPIVOTVTABLE*)a))

#include <g3dengine/g3dengine.h>
#include "g3dobjectvtable.h"

/******************************************************************************/
typedef struct _G3DPIVOT G3DPIVOT;

/******************************************************************************/
typedef struct _G3DPIVOTVTABLE {
    G3DOBJECTVTABLE objectVTable;
} G3DPIVOTVTABLE;

/************************** Object vtable overrides ***************************/
uint32_t g3dpivot_default_draw ( G3DPIVOT  *piv, 
                                 G3DCAMERA *curcam, 
                                 G3DENGINE *engine, 
                                 uint64_t   engine_flags );

/******************************************************************************/
#define G3DPIVOTVTABLE_DEFAULT \
.objectVTable = { G3DOBJECTVTABLE_DEFAULT,  \
                 .draw       = DRAW_CALLBACK(g3dpivot_default_draw) }

#endif /* _G3DPIVOTVTABLE_H_ */
