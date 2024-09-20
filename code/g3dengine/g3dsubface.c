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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2020         */
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
    uint32_t p = ( i + parent->vertexCount - 0x01 ) % parent->vertexCount;
    uint32_t n = ( i                 + 0x01 ) % parent->vertexCount;
    uint32_t o = ( i                 + 0x02 ) % parent->vertexCount;

    uint32_t i0 = ( i     );
    uint32_t i1 = ( i + 1 ) % 0x04;
    uint32_t i2 = ( i + 2 ) % 0x04;
    uint32_t i3 = ( i + 3 ) % 0x04;
    LIST *ltmpuvs = parent->uvsetList;

    while ( ltmpuvs ) {
        G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;

        subuvs->map = uvs->map;

        subuvs->veruv[i0].u = uvs->veruv[i].u;
        subuvs->veruv[i0].v = uvs->veruv[i].v;
        subuvs->veruv[i0].set = uvs;

        /*** Note: this could be precalculated at previous step ***/
        subuvs->veruv[i1].u = ( uvs->veruv[n].u + uvs->veruv[i].u ) * 0.5f;
        subuvs->veruv[i1].v = ( uvs->veruv[n].v + uvs->veruv[i].v ) * 0.5f;
        subuvs->veruv[i1].set = uvs;

        if ( parent->vertexCount == 0x03 ) {
            /*** Note: this could be precalculated at previous step ***/
            subuvs->veruv[i2].u = ( uvs->veruv[n].u + 
                                      uvs->veruv[i].u +
                                      uvs->veruv[p].u ) * 0.3333f;

            /*** Note: this could be precalculated at previous step ***/
            subuvs->veruv[i2].v = ( uvs->veruv[n].v + 
                                      uvs->veruv[i].v +
                                      uvs->veruv[p].v ) * 0.3333f;

            subuvs->veruv[i2].set = uvs;
        } else {
            /*** Note: this could be precalculated at previous step ***/
            subuvs->veruv[i2].u = ( uvs->veruv[n].u + 
                                      uvs->veruv[i].u +
                                      uvs->veruv[o].u +
                                      uvs->veruv[p].u ) * 0.25f;

            /*** Note: this could be precalculated at previous step ***/
            subuvs->veruv[i2].v = ( uvs->veruv[n].v + 
                                      uvs->veruv[i].v +
                                      uvs->veruv[o].v +
                                      uvs->veruv[p].v ) * 0.25f;

            subuvs->veruv[i2].set = uvs;
        }

        /*** Note: this could be precalculated at previous step ***/
        subuvs->veruv[i3].u = ( uvs->veruv[p].u + uvs->veruv[i].u ) * 0.5f;
        subuvs->veruv[i3].v = ( uvs->veruv[p].v + uvs->veruv[i].v ) * 0.5f;
        subuvs->veruv[i3].set = uvs;

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

    for ( i = 0x00; i < fac->vertexCount; i++ ) {
        int p = ( i + fac->vertexCount - 0x01 ) % fac->vertexCount;
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
    LIST *nextluvs = subfac->fac.uvsetList;
    uint32_t i;

    /*** if mallocated, use the normal function ***/
    if ( subfac->fac.flags & FACEMALLOCUVSETS ) {
        g3dface_addUVSet ( ( G3DFACE * ) subfac, uvs );

        return;
    }

    /*** or else use the static linked list functions ***/
    subfac->fac.uvsetList       = &subfac->luvsbuf[subfac->fac.uvsetCount];
    subfac->fac.uvsetList->next = nextluvs;
    subfac->fac.uvsetList->data = uvs;

    subfac->fac.uvsetCount++;

    /**** prepare the next step ***/
    /*g3duvset_subdivide ( uvs, subfac );*/
}
