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
#include <g3durmanager.h>

/******************************************************************************/
typedef struct _URMMOVEVERTICES {
    G3DSCENE    *sce;
    G3DMESH     *mes;
    LIST        *lver;
    LIST        *ledg;
    LIST        *lfac;
    LIST        *lsub;
    G3DVECTOR3F *oldpos;
    G3DVECTOR3F *newpos;
} URMMOVEVERTICES;

/******************************************************************************/
static URMMOVEVERTICES *urmmovevertices_new ( G3DSCENE    *sce,
                                              G3DMESH     *mes, 
                                              LIST        *lver,
                                              LIST        *ledg,
                                              LIST        *lfac,
                                              LIST        *lsub,
                                              G3DVECTOR3F *oldpos,
                                              G3DVECTOR3F *newpos ) {
    uint32_t structsize = sizeof ( URMMOVEVERTICES );

    URMMOVEVERTICES *mvs = ( URMMOVEVERTICES * ) calloc ( 0x01, structsize );

    if ( mvs == NULL ) {
        fprintf ( stderr, "urmmovevertices_new: memory allocation falied\n" );

        return NULL;
    }

    mvs->mes    = mes;
    mvs->lver   = list_copy ( lver );
    mvs->ledg   = list_copy ( ledg );
    mvs->lfac   = list_copy ( lfac );
    mvs->lsub   = list_copy ( lsub );
    mvs->oldpos = oldpos;
    mvs->newpos = newpos;


    return mvs;
}

/******************************************************************************/
static void urmmovevertices_free ( URMMOVEVERTICES *mvs ) {
    list_free ( &mvs->lver, NULL );
    list_free ( &mvs->ledg, NULL );
    list_free ( &mvs->lfac, NULL );
    list_free ( &mvs->lsub, NULL );

    free ( mvs->oldpos );
    free ( mvs->newpos );

    free ( mvs );
}

/******************************************************************************/
static void moveVertices_free ( void    *data, 
                                uint32_t commit ) {
    URMMOVEVERTICES *mvs = ( URMMOVEVERTICES * ) data;

    urmmovevertices_free ( mvs );
}

/******************************************************************************/
static void moveVertices_undo ( G3DURMANAGER *urm, 
                                void         *data, 
                                uint64_t      engine_flags ) {
    URMMOVEVERTICES *mvs = ( URMMOVEVERTICES * ) data;
    G3DOBJECT *obj = ( G3DOBJECT * ) mvs->mes;
    G3DMESH *mes = mvs->mes;
    LIST *ltmp = mvs->lver;
    uint32_t i = 0x00;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        memcpy ( &ver->pos, &mvs->oldpos[i++], sizeof ( G3DVECTOR3F ) );

        ltmp = ltmp->next;
    }

    g3dmesh_updateBbox ( mvs->mes );

    g3dobject_update_r ( G3DOBJECTCAST(mes), 0x00, engine_flags );
}

/******************************************************************************/
static void moveVertices_redo ( G3DURMANAGER *urm, 
                                void         *data, 
                                uint64_t      engine_flags ) {
    URMMOVEVERTICES *mvs = ( URMMOVEVERTICES * ) data;
    G3DOBJECT *obj = ( G3DOBJECT * ) mvs->mes;
    G3DMESH *mes = mvs->mes;
    LIST *ltmp = mvs->lver;
    uint32_t i = 0x00;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        memcpy ( &ver->pos, &mvs->newpos[i++], sizeof ( G3DVECTOR3F ) );

        ltmp = ltmp->next;
    }

    g3dmesh_updateBbox ( mvs->mes );

    g3dobject_update_r ( G3DOBJECTCAST(mes), 0x00, engine_flags );
}

/******************************************************************************/
void g3durm_mesh_moveVertexList ( G3DURMANAGER *urm,
                                  G3DSCENE     *sce, 
                                  G3DMESH      *mes, 
                                  LIST         *lver,
                                  LIST         *ledg,
                                  LIST         *lfac,
                                  LIST         *lsub,
                                  G3DVECTOR3F *oldpos,
                                  G3DVECTOR3F *newpos,
                                  uint32_t return_flags ) {
    URMMOVEVERTICES *mvs;

    mvs = urmmovevertices_new ( sce,
                                mes,
                                lver,
                                ledg,
                                lfac,
                                lsub,
                                oldpos,
                                newpos );

    g3durmanager_push ( urm, moveVertices_undo,
                             moveVertices_redo,
                             moveVertices_free, mvs, return_flags );
}
