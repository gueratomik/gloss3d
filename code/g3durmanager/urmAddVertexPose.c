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

typedef struct _URMADDVERTEXPOSE {
    G3DSCENE           *sce;
    G3DMORPHER         *mpr;
    G3DMORPHERMESHPOSE *mpose;
    LIST              *lver;
} URMADDVERTEXPOSE;

/******************************************************************************/
static URMADDVERTEXPOSE *urmaddvertexpose_new ( G3DSCENE           *sce,
                                                G3DMORPHER         *mpr,
                                                G3DMORPHERMESHPOSE *mpose,
                                                LIST               *lver ) {
    uint32_t structsize = sizeof ( URMADDVERTEXPOSE );

    URMADDVERTEXPOSE *avp = ( URMADDVERTEXPOSE * ) calloc ( 0x01, structsize );

    if ( avp == NULL ) {
        fprintf ( stderr, "%s: memory allocation falied\n", __func__ );

        return NULL;
    }

    avp->sce   = sce;
    avp->mpr   = mpr;
    avp->mpose = mpose;
    avp->lver  = list_copy  ( lver );

    return avp;
}

/******************************************************************************/
static void urmaddvertexpose_free ( URMADDVERTEXPOSE *avp ) {
    free ( avp );
}

/******************************************************************************/
static void addVertexPose_free ( void *data, uint32_t commit ) {
    URMADDVERTEXPOSE *avp = ( URMADDVERTEXPOSE * ) data;

    if ( commit ) {
        list_free ( &avp->lver, NULL );
    }

    urmaddvertexpose_free ( avp );
}

/******************************************************************************/
static void addVertexPose_undo ( G3DURMANAGER *urm, 
                                 void         *data, 
                                 uint64_t      engine_flags ) {
    URMADDVERTEXPOSE *avp = ( URMADDVERTEXPOSE * ) data;
    LIST *ltmpver = avp->lver;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        g3dmorpher_removeVertexPose ( avp->mpr, ver, avp->mpose );

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
static void addVertexPose_redo ( G3DURMANAGER *urm, 
                                 void         *data, 
                                 uint64_t      engine_flags ) {
    URMADDVERTEXPOSE *avp = ( URMADDVERTEXPOSE * ) data;
    LIST *ltmpver = avp->lver;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        g3dmorpher_addVertexPose ( avp->mpr, ver, avp->mpose, &ver->pos );

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
void g3durm_morpher_addVertexPose ( G3DURMANAGER       *urm,
                                    G3DSCENE           *sce,
                                    G3DMORPHER         *mpr,
                                    G3DMORPHERMESHPOSE *mpose,
                                    LIST               *lver,
                                    uint32_t            return_flags ) {
    URMADDVERTEXPOSE *avp;

    avp = urmaddvertexpose_new ( sce, mpr, mpose, lver );

    g3durmanager_push ( urm, addVertexPose_undo,
                             addVertexPose_redo,
                             addVertexPose_free, avp, return_flags );
}
