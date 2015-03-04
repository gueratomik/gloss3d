/******************************************************************************/
/*  GLOSS3D is free software: you can redistribute it and/or modify           */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation, either version 3 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  GLOSS3D is distributed in the hope that it will be useful,                */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with GLOSS3D.  If not, see http://www.gnu.org/licenses/.            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/
#include <config.h>
#include <g3d.h>

/******************************************************************************/
inline void g3dsubvertex_addUV ( G3DSUBVERTEX *subver, G3DUV *uv ) {
    LIST *nextluv = subver->ver.luv;

    /*** Go with dynamic allocation if this subvertex UV buffer is too small***/
    if ( subver->ver.flags & VERTEXMALLOCUVS ) {
        g3dvertex_addUV ( ( G3DVERTEX * ) subver, uv );

        return;
    }

    subver->ver.luv       = &subver->luvbuf[subver->ver.nbuv];
    subver->ver.luv->next = nextluv;
    subver->ver.luv->data = uv;

    subver->ver.nbuv++;
}

/******************************************************************************/
inline void g3dsubvertex_addEdge ( G3DSUBVERTEX *subver, G3DEDGE *edg ) {
    LIST *nextledg = subver->ver.ledg;

    if ( subver->ver.flags & VERTEXMALLOCEDGES ) {
        g3dvertex_addEdge ( ( G3DVERTEX * ) subver, edg );

        return;
    }

    subver->ver.ledg       = &subver->ledgbuf[subver->ver.nbedg];
    subver->ver.ledg->next = nextledg;
    subver->ver.ledg->data = edg;

    subver->ver.nbedg++;
}

/******************************************************************************/
inline void g3dsubvertex_addFace ( G3DSUBVERTEX *subver, G3DFACE *fac ) {
    LIST *nextlfac = subver->ver.lfac;

    if ( subver->ver.flags & VERTEXMALLOCFACES ) {
        g3dvertex_addFace ( ( G3DVERTEX * ) subver, fac );

        return;
    }

    subver->ver.lfac       = &subver->lfacbuf[subver->ver.nbfac];
    subver->ver.lfac->next = nextlfac;
    subver->ver.lfac->data = fac;

    subver->ver.nbfac++;
}
