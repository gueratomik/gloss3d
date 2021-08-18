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
G3DWEIGHTGROUP *g3dweightgroup_mirror ( G3DWEIGHTGROUP *grp,
                                        uint32_t orientation ) {
    G3DWEIGHTGROUP *mirgrp = g3dweightgroup_new ( grp->mes, "Mirrored Group" );
    G3DMESH *mes = grp->mes;
    LIST *ltmp = grp->lwei;

    while ( ltmp ) {
        G3DWEIGHT *wei = ( G3DWEIGHT * ) ltmp->data;
        G3DVERTEX *ver = wei->ver;
        G3DVERTEX *mirver;

        switch ( orientation ) {
            case ORIENTATIONZX :
                mirver = g3dmesh_seekVertexByPosition ( mes,  ver->pos.x,
                                                             -ver->pos.y,
                                                              ver->pos.z,
                                                              0.0001f );
            break;

            case ORIENTATIONXY :
                mirver = g3dmesh_seekVertexByPosition ( mes,  ver->pos.x,
                                                              ver->pos.y,
                                                             -ver->pos.z,
                                                              0.0001f );
            break;

            case ORIENTATIONYZ :
                mirver = g3dmesh_seekVertexByPosition ( mes, -ver->pos.x,
                                                              ver->pos.y,
                                                              ver->pos.z,
                                                              0.0001f );
            break;

            default :
            break;
        }

        if ( mirver ) {
            g3dweightgroup_addVertex ( mirgrp, mirver, wei->weight );
        }

        ltmp = ltmp->next;
    }


    return mirgrp;
}

/******************************************************************************/
G3DWEIGHT *g3dweightgroup_addVertex ( G3DWEIGHTGROUP *grp, 
                                      G3DVERTEX *ver, float weight ) {
    G3DWEIGHT *wei = g3dweight_new ( ver, weight );
    /*LIST *ltmpfac = ver->lfac;*/

    list_insert ( &grp->lwei, wei );
    /*list_insert ( &grp->lver, ver );*/

    grp->nbwei++;

    g3dvertex_addWeight ( wei->ver, wei );

    /*while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        if ( list_seek ( grp->lfac, fac ) == NULL ) {
            list_insert ( &grp->lfac, fac );
        }

        ltmpfac = ltmpfac->next;
    }*/


    return wei;
}

/******************************************************************************/
LIST *g3dweightgroup_getVertices ( G3DWEIGHTGROUP *grp ) {
    LIST *ltmpwei = grp->lwei;
    LIST *lver = NULL;

    while ( ltmpwei ) {
        G3DWEIGHT *wei = ( G3DWEIGHT * ) ltmpwei->data;

        list_insert ( &lver, wei->ver );

        ltmpwei = ltmpwei->next;
    }

    return lver;
}

/******************************************************************************/
void g3dweightgroup_empty ( G3DWEIGHTGROUP *grp ) {
    LIST *ltmp = grp->lwei;

    while ( ltmp ) {
        G3DWEIGHT *wei = ( G3DWEIGHT  * ) ltmp->data;

        g3dvertex_removeWeight ( wei->ver, wei );

        ltmp = ltmp->next;
    }

    list_free ( &grp->lwei, NULL );
    /*list_free ( &grp->lver, NULL );*/

    grp->nbwei = 0x00;
}

/******************************************************************************/
void g3dweightgroup_removeWeight ( G3DWEIGHTGROUP *grp, G3DWEIGHT *wei ) {
    list_remove ( &grp->lwei, wei );
    /*list_remove ( &grp->lver, wei->ver );*/

    g3dvertex_removeWeight ( wei->ver, wei );

    grp->nbwei--;
}

/******************************************************************************/
G3DWEIGHT *g3dweightgroup_removeVertex ( G3DWEIGHTGROUP *grp, G3DVERTEX *ver ) {
    G3DWEIGHT *wei = g3dweightgroup_seekVertex ( grp, ver );

    list_remove ( &ver->lwei, wei );

    g3dweightgroup_removeWeight ( grp, wei );


    return wei;
}

/******************************************************************************/
G3DWEIGHT *g3dweightgroup_seekVertex ( G3DWEIGHTGROUP *grp, G3DVERTEX *ver ) {
    LIST *ltmp = grp->lwei;

    while ( ltmp ) {
        G3DWEIGHT *wei = ( G3DWEIGHT * ) ltmp->data;

        if ( wei->ver == ver ) return wei;

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
void g3dweightgroup_painted ( G3DWEIGHTGROUP *grp ) {
    LIST *ltmp = grp->lwei;

    while ( ltmp ) {
        G3DWEIGHT *wei = ( G3DWEIGHT * ) ltmp->data;

        wei->ver->flags |= VERTEXPAINTED;
        wei->ver->weight = wei->weight;

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dweightgroup_unpainted ( G3DWEIGHTGROUP *grp ) {
    LIST *ltmp = grp->lwei;

    while ( ltmp ) {
        G3DWEIGHT *wei = ( G3DWEIGHT * ) ltmp->data;

        wei->ver->flags &= (~VERTEXPAINTED);
        wei->ver->weight = 0.0f;

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dweightgroup_name ( G3DWEIGHTGROUP *grp, char *name ) {
    if ( name ) {
        uint32_t len = strlen ( name ) + 0x01;

        grp->name = realloc ( grp->name, len );

        memset ( grp->name, 0x00, len );
        memcpy ( grp->name, name, len );
    }
}

/******************************************************************************/
G3DWEIGHTGROUP *g3dweightgroup_new ( G3DMESH *mes, char *name ) {
    uint32_t structsize = sizeof ( G3DWEIGHTGROUP );
    G3DWEIGHTGROUP *grp = ( G3DWEIGHTGROUP * ) calloc ( 0x01, structsize );


    if ( grp == NULL ) {
        fprintf ( stderr, "g3dweightgroup_new: calloc failed\n" );
    }

    g3dweightgroup_name ( grp, name );

    grp->mes = mes;


    return grp;
}

/******************************************************************************/
void g3dweightgroup_free ( G3DWEIGHTGROUP *grp ) {
    list_free ( &grp->lwei, g3dweight_free );
    /*list_free ( &grp->lver, NULL );*/

    free ( grp );
}

/******************************************************************************/
void g3dweight_free ( G3DWEIGHT *wei ) {
    free ( wei );
}

/******************************************************************************/
G3DWEIGHT *g3dweight_new ( G3DVERTEX *ver, float weight ) {
    G3DWEIGHT *wei = ( G3DWEIGHT * ) calloc ( 0x01, sizeof ( G3DWEIGHT ) );

    if ( wei == NULL ) {
        fprintf ( stderr, "g3dweight_new: calloc failed\n" );
    }

    wei->ver    = ver;
    wei->weight = weight;


    return wei;
}

/******************************************************************************/
void g3dweightgroup_fix ( G3DWEIGHTGROUP *grp, 
                          G3DRIG         *rig ) {
    LIST *ltmpwei = grp->lwei;

    while ( ltmpwei ) {
        G3DWEIGHT *wei = ( G3DWEIGHT * ) ltmpwei->data;

        wei->rig = rig;

        ltmpwei = ltmpwei->next;
    }
}

/******************************************************************************/
void g3dweightgroup_unfix ( G3DWEIGHTGROUP *grp ) {
    LIST *ltmpwei = grp->lwei;

    while ( ltmpwei ) {
        G3DWEIGHT *wei = ( G3DWEIGHT * ) ltmpwei->data;

        wei->rig = NULL;

        ltmpwei = ltmpwei->next;
    }
}
