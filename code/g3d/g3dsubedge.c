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
void g3dsubedge_position ( G3DSUBEDGE *subedg ) {
    subedg->edg.pos.x = ( subedg->edg.ver[0x00]->pos.x + 
                          subedg->edg.ver[0x01]->pos.x ) * 0.5;

    subedg->edg.pos.y = ( subedg->edg.ver[0x00]->pos.y + 
                          subedg->edg.ver[0x01]->pos.y ) * 0.5;

    subedg->edg.pos.z = ( subedg->edg.ver[0x00]->pos.z + 
                          subedg->edg.ver[0x01]->pos.z ) * 0.5;
}

/******************************************************************************/
void g3dsubedge_addFace ( G3DSUBEDGE *subedg, G3DFACE *fac ) {
    LIST *nextlfac = subedg->edg.lfac;

    if ( subedg->edg.flags & EDGEMALLOCFACES ) {
        g3dedge_addFace ( ( G3DEDGE * ) subedg, fac );

        return;
    }

    subedg->edg.lfac       = &subedg->lfacbuf[subedg->edg.nbfac];
    subedg->edg.lfac->next = nextlfac;
    subedg->edg.lfac->data = fac;

    subedg->edg.nbfac++;
}
