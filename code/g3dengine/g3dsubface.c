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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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
#include <g3dengine/g3dengine.h>

/******************************************************************************/
void g3dsubface_importUVSets ( G3DSUBFACE *subfac, G3DFACE   *parent,
                                                   uint32_t   i,
                                                   G3DUVSET  *subuvs,
                                                   uint32_t   curdiv  ) {
    uint32_t p = ( i + parent->nbver - 0x01 ) % parent->nbver;
    LIST *ltmpuvs = parent->luvs;

    while ( ltmpuvs ) {
        G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;

        subuvs->map = uvs->map;

        subuvs->veruv[0x00].u = uvs->veruv[i].u;
        subuvs->veruv[0x00].v = uvs->veruv[i].v;
        subuvs->veruv[0x00].set = uvs;

        subuvs->veruv[0x01].u = uvs->miduv[i].u;
        subuvs->veruv[0x01].v = uvs->miduv[i].v;
        subuvs->veruv[0x01].set = uvs;

        subuvs->veruv[0x02].u = uvs->cenuv.u;
        subuvs->veruv[0x02].v = uvs->cenuv.v;
        subuvs->veruv[0x02].set = uvs;

        subuvs->veruv[0x03].u = uvs->miduv[p].u;
        subuvs->veruv[0x03].v = uvs->miduv[p].v;
        subuvs->veruv[0x03].set = uvs;

        g3dsubface_addUVSet ( subfac, subuvs, curdiv );

        subuvs++;

        ltmpuvs = ltmpuvs->next;
    }
}

/******************************************************************************/
void g3dsubface_position ( G3DSUBFACE *subfac  ) {
    subfac->fac.pos.x = ( subfac->fac.ver[0x00]->pos.x + 
                          subfac->fac.ver[0x01]->pos.x + 
                          subfac->fac.ver[0x02]->pos.x + 
                          subfac->fac.ver[0x03]->pos.x ) * 0.25;

    subfac->fac.pos.y = ( subfac->fac.ver[0x00]->pos.y + 
                          subfac->fac.ver[0x01]->pos.y + 
                          subfac->fac.ver[0x02]->pos.y + 
                          subfac->fac.ver[0x03]->pos.y ) * 0.25;

    subfac->fac.pos.z = ( subfac->fac.ver[0x00]->pos.z + 
                          subfac->fac.ver[0x01]->pos.z + 
                          subfac->fac.ver[0x02]->pos.z + 
                          subfac->fac.ver[0x03]->pos.z ) * 0.25;
}

/******************************************************************************/
void g3dsubface_topology ( G3DSUBFACE *subfac, uint32_t subdiv_flags  ) {
    G3DFACE *fac = ( G3DFACE * ) subfac;
    int i;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        int p = ( i + fac->nbver - 0x01 ) % fac->nbver;
        G3DSUBVERTEX *subver = ( G3DSUBVERTEX * ) fac->ver[i];
        G3DSUBEDGE *subedg = ( G3DSUBEDGE * ) fac->edg[i];

        if ( subver->ver.flags & VERTEXTOPOLOGY ) {
            g3dsubvertex_addFace ( subver, ( G3DFACE * ) fac );
        }

        if ( subedg && ( subedg->edg.flags & EDGETOPOLOGY ) ) {
            g3dsubedge_addFace ( subedg, ( G3DFACE * ) fac );
        }
    }
}

/******************************************************************************/
void g3dsubface_addUVSet ( G3DSUBFACE *subfac, G3DUVSET *uvs,
                                               uint32_t curdiv ) {
    LIST *nextluvs = subfac->fac.luvs;
    uint32_t i;

    /*** if mallocated, use the normal function ***/
    if ( subfac->fac.flags & FACEMALLOCUVSETS ) {
        g3dface_addUVSet ( ( G3DFACE * ) subfac, uvs );

        return;
    }

    /*** or else use the static linked list functions ***/
    subfac->fac.luvs       = &subfac->luvsbuf[subfac->fac.nbuvs];
    subfac->fac.luvs->next = nextluvs;
    subfac->fac.luvs->data = uvs;

    subfac->fac.nbuvs++;
}
