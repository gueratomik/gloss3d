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
typedef struct _URMMORPHERMOVEVERTICES {
    G3DMORPHER         *mpr;
    G3DMORPHERMESHPOSE *mpose;
    LIST               *lver;
    G3DVECTOR3F          *oldpos;
    G3DVECTOR3F          *newpos;
} URMMORPHERMOVEVERTICES;

/******************************************************************************/
static URMMORPHERMOVEVERTICES *urmMorpherMoveVertices_new ( G3DMORPHER         *mpr,
                                                            G3DMORPHERMESHPOSE *mpose,
                                                            LIST               *lver,
                                                            G3DVECTOR3F          *oldpos,
                                                            G3DVECTOR3F          *newpos ) {
    uint32_t structsize = sizeof ( URMMORPHERMOVEVERTICES );

    URMMORPHERMOVEVERTICES *mmv = ( URMMORPHERMOVEVERTICES * ) calloc ( 0x01, structsize );

    if ( mmv == NULL ) {
        fprintf ( stderr, "%s: memory allocation falied\n", __func__ );

        return NULL;
    }

    mmv->mpr    = mpr;
    mmv->mpose  = mpose;
    mmv->lver   = list_copy ( lver );
    mmv->oldpos = oldpos;
    mmv->newpos = newpos;


    return mmv;
}

/******************************************************************************/
static void urmMorpherMoveVertices_free ( URMMORPHERMOVEVERTICES *mmv ) {
    list_free ( &mmv->lver, NULL );

    free ( mmv->oldpos );
    free ( mmv->newpos );

    free ( mmv );
}

/******************************************************************************/
static void morpherMoveVertices_free ( void    *data, 
                                       uint32_t commit ) {
    URMMORPHERMOVEVERTICES *mmv = ( URMMORPHERMOVEVERTICES * ) data;

    urmMorpherMoveVertices_free ( mmv );
}

/******************************************************************************/
static void morpherMoveVertices_undo ( G3DURMANAGER *urm, 
                                       void         *data, 
                                       uint64_t      engine_flags ) {
    URMMORPHERMOVEVERTICES *mmv = ( URMMORPHERMOVEVERTICES * ) data;
    G3DMORPHER *mpr = mmv->mpr;
    G3DMORPHERMESHPOSE *mpose = mmv->mpose;
    LIST *ltmpver = mmv->lver;
    uint32_t verID = 0x00;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DMORPHERVERTEXPOSE *vpose = g3dmorpher_getVertexPose ( mpr, 
                                                                 ver,
                                                                 mpose,
                                                                 NULL );

        memcpy ( &vpose->pos, &mmv->oldpos[verID++], sizeof ( G3DVECTOR3F ) );

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
static void morpherMoveVertices_redo ( G3DURMANAGER *urm, 
                                       void         *data, 
                                       uint64_t      engine_flags ) {
    URMMORPHERMOVEVERTICES *mmv = ( URMMORPHERMOVEVERTICES * ) data;
    G3DMORPHER *mpr = mmv->mpr;
    G3DMORPHERMESHPOSE *mpose = mmv->mpose;
    LIST *ltmpver = mmv->lver;
    uint32_t verID = 0x00;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DMORPHERVERTEXPOSE *vpose = g3dmorpher_getVertexPose ( mpr, 
                                                                 ver,
                                                                 NULL,
                                                                 NULL );

        memcpy ( &vpose->pos, &mmv->newpos[verID++], sizeof ( G3DVECTOR3F ) );

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
void g3durm_morpher_moveVertices ( G3DURMANAGER       *urm,
                                   G3DMORPHER         *mpr,
                                   G3DMORPHERMESHPOSE *mpose,
                                   LIST               *lver,
                                   G3DVECTOR3F          *oldpos,
                                   G3DVECTOR3F          *newpos,
                                   uint32_t            return_flags ) {
    URMMORPHERMOVEVERTICES *mmv;

    mmv = urmMorpherMoveVertices_new ( mpr,
                                       mpose, 
                                       lver, 
                                       oldpos, 
                                       newpos );

    g3durmanager_push ( urm, morpherMoveVertices_undo,
                             morpherMoveVertices_redo,
                             morpherMoveVertices_free, mmv, return_flags );
}
