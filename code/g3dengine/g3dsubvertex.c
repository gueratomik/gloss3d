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
void g3dsubvertex_renumberArray ( G3DSUBVERTEX *subver, uint32_t nbver ) {
    uint32_t vertexID = 0x00;
    uint32_t i;

    for ( i = 0x00; i < nbver; i++ ) {
        subver[i].ver.id = vertexID++;
    }
}

/******************************************************************************/
void g3dsubvertex_elevate ( G3DSUBVERTEX *subver,
                            uint64_t      sculptExtensionName,
                            uint32_t    (*tri_indexes)[0x04],
                            uint32_t    (*qua_indexes)[0x04],
                            uint32_t      sculptMode ) {
    G3DVECTOR3F pos = { 0.0f, 0.0f, 0.0f };
    G3DHEIGHT hei = { 0.0f, 0.0f };
    LIST *ltmpfac = subver->ver.lfac;
    uint32_t nbfac = 0x00;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        G3DFACESCULPTEXTENSION *fse = ( G3DFACESCULPTEXTENSION * ) g3dface_getExtension ( fac, 
                                                                                          sculptExtensionName );

        if ( fse && qua_indexes && tri_indexes ) {
            uint32_t i;

            for ( i = 0x00; i < fac->nbver; i++ ) {
                if ( fac->ver[i] == ( G3DVERTEX * ) subver ) {
                    uint32_t verID = ( fac->flags & FACEFROMQUAD ) ? qua_indexes[fac->id][i] :
                                                                     tri_indexes[fac->id][i];
                    switch ( sculptMode ) {
                        case SCULPTMODE_SCULPT :
                            if ( fse->pos[verID].w ) {
                                pos.x += fse->pos[verID].x;
                                pos.y += fse->pos[verID].y;
                                pos.z += fse->pos[verID].z;

                                nbfac++;
                            }
                        break;

                        default :
                            if ( fse->hei[verID].w ) {
                                hei.s += fse->hei[verID].s;

                                nbfac++;
                            }
                        break;
                    }
                }
            }
        }

        ltmpfac = ltmpfac->next;
    }

    if ( nbfac ) {
        float elevation = hei.s / nbfac;

        switch ( sculptMode ) {
            case SCULPTMODE_SCULPT :
                subver->ver.pos.x += ( pos.x / nbfac );
                subver->ver.pos.y += ( pos.y / nbfac );
                subver->ver.pos.z += ( pos.z / nbfac );
            break;

            default :
                subver->ver.pos.x += ( subver->ver.nor.x * elevation );
                subver->ver.pos.y += ( subver->ver.nor.y * elevation );
                subver->ver.pos.z += ( subver->ver.nor.z * elevation );
            break;
        }
    }
}

/******************************************************************************/
void g3dsubvertex_resetEdgeList ( G3DSUBVERTEX *subver ) {
    memset ( subver->ledgbuf, 0x00, sizeof ( subver->ledgbuf ) );

    subver->ver.ledg  = NULL;
    subver->ver.nbedg = 0x00;
}

/******************************************************************************/
void g3dsubvertex_resetFaceList ( G3DSUBVERTEX *subver ) {
    memset ( subver->lfacbuf, 0x00, sizeof ( subver->lfacbuf ) );

    subver->ver.lfac  = NULL;
    subver->ver.nbfac = 0x00;
}

/******************************************************************************/
void g3dsubvertex_addEdge ( G3DSUBVERTEX *subver, G3DEDGE *edg ) {
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
void g3dsubvertex_addFace ( G3DSUBVERTEX *subver, G3DFACE *fac ) {
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
