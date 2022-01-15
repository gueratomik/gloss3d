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

typedef struct _URMSELECTVERTEXPOSE {
    G3DSCENE           *sce;
    G3DMORPHER         *mpr;
    G3DMORPHERMESHPOSE *mpose;
    /*** morpher vertices before the selection occurs. This will allow us to **/
    /*** tell which vertices were added to the morpher and which one already **/
    /*** belonged to it. Remeber, a vertex that does not belong to a pose    **/
    /*** cannot belong to mpr->lver. If it does, there is a bug somewhere.   **/
    LIST               *lmprver;
    /*** new selection ***/
    LIST               *lselnew;
    /*** old selection ***/
    LIST               *lselold;
} URMSELECTVERTEXPOSE;

/******************************************************************************/
static URMSELECTVERTEXPOSE *urmselectvertexpose_new ( G3DSCENE      *sce,
                                                      G3DMORPHER    *mpr,
                                                 G3DMORPHERMESHPOSE *mpose,
                                                      LIST          *lmprver,
                                                      LIST          *lselold,
                                                      LIST          *lselnew ) {
    uint32_t ssize = sizeof ( URMSELECTVERTEXPOSE );

    URMSELECTVERTEXPOSE *avp = ( URMSELECTVERTEXPOSE * ) calloc ( 0x01, ssize );

    if ( avp == NULL ) {
        fprintf ( stderr, "%s: memory allocation falied\n", __func__ );

        return NULL;
    }

    avp->sce     = sce;
    avp->mpr     = mpr;
    avp->mpose   = mpose;
    avp->lmprver = lmprver;
    avp->lselnew = lselnew;
    avp->lselold = lselold;

    return avp;
}

/******************************************************************************/
static void urmselectvertexpose_free ( URMSELECTVERTEXPOSE *avp ) {
    list_free ( &avp->lmprver, NULL );
    list_free ( &avp->lselnew, NULL );
    list_free ( &avp->lselold, NULL );

    free ( avp );
}

/******************************************************************************/
static void selectVertexPose_free ( void *data, uint32_t commit ) {
    URMSELECTVERTEXPOSE *avp = ( URMSELECTVERTEXPOSE * ) data;

    urmselectvertexpose_free ( avp );
}

/******************************************************************************/
static void selectVertexPose_undo ( G3DURMANAGER *urm, 
                                    void         *data, 
                                    uint64_t      engine_flags ) {
    URMSELECTVERTEXPOSE *avp = ( URMSELECTVERTEXPOSE * ) data;

    if ( ((G3DOBJECT*)avp->mpr)->parent->type == G3DMESHTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) ((G3DOBJECT*)avp->mpr)->parent;
        LIST *ltmpver = avp->lselnew;

        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

            if ( list_seek ( avp->lmprver, ver ) == NULL ) {
                g3dmorpher_removeVertexPose ( avp->mpr, 
                                              ver, 
                                              avp->mpose );
            }

            g3dmesh_unselectVertex ( mes, ver );

            ltmpver = ltmpver->next;
        }

        list_execargdata ( avp->lselold, LIST_FUNCARGDATA(g3dmesh_selectVertex), mes );
    }

    g3dscene_updatePivot ( avp->sce, engine_flags );
}

/******************************************************************************/
static void selectVertexPose_redo ( G3DURMANAGER *urm, 
                                    void         *data, 
                                    uint64_t      engine_flags ) {
    URMSELECTVERTEXPOSE *avp = ( URMSELECTVERTEXPOSE * ) data;

    if ( ((G3DOBJECT*)avp->mpr)->parent->type == G3DMESHTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) ((G3DOBJECT*)avp->mpr)->parent;
        LIST *ltmpver = avp->lselnew;

        list_execargdata ( avp->lselold, LIST_FUNCARGDATA(g3dmesh_unselectVertex), mes );

        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

            if ( list_seek ( avp->lmprver, ver ) == NULL ) {
                g3dmorpher_addVertexPose ( avp->mpr, 
                                           ver, 
                                           avp->mpose,
                                          &ver->pos );
            }

            g3dmesh_selectVertex ( mes, ver );

            ltmpver = ltmpver->next;
        }
    }

    g3dscene_updatePivot ( avp->sce, engine_flags );
}

/******************************************************************************/
void g3durm_morpher_selectVertexPose ( G3DURMANAGER       *urm,
                                       G3DSCENE           *sce,
                                       G3DMORPHER         *mpr,
                                       G3DMORPHERMESHPOSE *mpose,
                                       LIST               *lmprver,
                                       LIST               *lselold,
                                       LIST               *lselnew,
                                       uint32_t            return_flags ) {
    URMSELECTVERTEXPOSE *avp;

    avp = urmselectvertexpose_new ( sce, 
                                    mpr,
                                    mpose,
                                    lmprver,
                                    lselold,
                                    lselnew );

    g3durmanager_push ( urm, selectVertexPose_undo,
                             selectVertexPose_redo,
                             selectVertexPose_free, avp, return_flags );
}
