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
URMADDUVMAP *urmadduvmap_new ( G3DMESH  *mes,
                               G3DUVMAP *uvmap,
                               LIST     *lnewuvset,
                               uint64_t engine_flags ) {
    uint32_t structsize = sizeof ( URMADDUVMAP );

    URMADDUVMAP *uau = ( URMADDUVMAP * ) calloc ( 0x01, structsize );

    if ( uau == NULL ) {
        fprintf ( stderr, "%s: memory allocation falied\n", __func__ );

        return NULL;
    }

    uau->mes       = mes;
    uau->uvmap     = uvmap;
    uau->lnewuvset = lnewuvset;

    return uau;
}

/******************************************************************************/
void urmadduvmap_free ( URMADDUVMAP *uau ) {
    free ( uau );
}

/******************************************************************************/
void addUVMap_free ( void *data, uint32_t commit ) {
    URMADDUVMAP *uau = ( URMADDUVMAP * ) data;

    if ( commit ) {
        list_free ( &uau->lnewuvset, NULL );
    } else {
        g3duvmap_free ( ( G3DOBJECT * ) uau->uvmap );

        list_free ( &uau->lnewuvset, LIST_FUNCDATA(g3duvset_free) );
    }

    urmadduvmap_free ( uau );
}

/******************************************************************************/
void addUVMap_undo ( G3DURMANAGER *urm, void *data, uint64_t engine_flags ) {
    URMADDUVMAP *uau = ( URMADDUVMAP * ) data;

    g3dmesh_removeUVMap ( uau->mes, uau->uvmap, NULL, NULL, engine_flags );

    /*** Rebuild the mesh with modifiers (e.g for displacement) ***/
    /* Commented out: this is called in g3dmesh_removeUVMap() ***/
    /*g3dmesh_update ( uau->mes, 
                     NULL,
                     NULL,
                     NULL,
                     RESETMODIFIERS, flags );*/
}

/******************************************************************************/
void addUVMap_redo ( G3DURMANAGER *urm, void *data, uint64_t engine_flags ) {
    URMADDUVMAP *uau = ( URMADDUVMAP * ) data;
    LIST *ltmpnewuvset = uau->lnewuvset;

    /*** this is put BEFORE g3dmesh_addUVMap because g3dmesh_update is called */
    /*** in g3dmesh_addUVMap and e.g displacement would need the uvsets to be */
    /*** attached to the faces. Maybe g3dmesh_update should be taken out of **/
    /*** of g3dmesh_addUVMap ***/
    while ( ltmpnewuvset ) {
        G3DUVSET *uvset = ( G3DUVSET * ) ltmpnewuvset->data;

        g3dface_addUVSet ( uvset->fac, uvset );

        ltmpnewuvset = ltmpnewuvset->next;
    }

    g3dmesh_addUVMap ( uau->mes, 
                       uau->uvmap, 
                       NULL, /*** no need to backp uvsets this time **/
                       engine_flags );

    /*** Rebuild the mesh with modifiers (e.g for displacement) ***/
    /*** Commented out. See comment below ***/
    /*g3dmesh_update ( uau->mes, 
                     NULL,
                     NULL,
                     NULL,
                     RESETMODIFIERS, flags );*/
}

/******************************************************************************/
void g3durm_mesh_addUVMap ( G3DURMANAGER *urm,
                            G3DMESH      *mes,
                            G3DUVMAP     *uvmap, 
                            uint64_t engine_flags,
                            uint32_t      return_flags ) {
    LIST *lnewuvset = NULL;
    URMADDUVMAP *uau;

    g3dmesh_addUVMap ( mes, uvmap, &lnewuvset, engine_flags );

    uau = urmadduvmap_new ( mes, uvmap, lnewuvset, engine_flags );

    /*** Rebuild the mesh with modifiers (e.g for displacement) ***/
    /* Commented out. this is called in g3dmesh_addUVMap() */
    /*g3dmesh_update ( mes, 
                     NULL,
                     NULL,
                     NULL,
                     RESETMODIFIERS, engine_flags );*/

    g3durmanager_push ( urm, 
                        addUVMap_undo,
                        addUVMap_redo,
                        addUVMap_free, 
                        uau,
                        return_flags );
}
