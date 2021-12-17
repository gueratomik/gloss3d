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
static URMDELSELITEMS *urmdelselitems_new ( G3DSCENE *sce, 
                                            LIST     *loldselobj,
                                            G3DMESH  *mes, 
                                            LIST     *loldselver,
                                            LIST     *loldseledg,
                                            LIST     *loldselfac,
                                            uint64_t  engine_flags ) {
    uint32_t structsize = sizeof ( URMDELSELITEMS );

    URMDELSELITEMS *dsi = ( URMDELSELITEMS * ) calloc ( 0x01, structsize );

    if ( dsi == NULL ) {
        fprintf ( stderr, "urmdelselitems_new: memory allocation falied\n" );

        return NULL;
    }

    dsi->sce          = sce;
    dsi->mes          = mes;
    dsi->engine_flags = engine_flags;

    dsi->loldselobj = loldselobj;
    dsi->loldselver = loldselver;
    dsi->loldseledg = loldseledg;
    dsi->loldselfac = loldselfac;


    return dsi;
}

/******************************************************************************/
static void urmdelselitems_free ( URMDELSELITEMS *dsi ) {
    list_free ( &dsi->loldselobj, NULL );
    list_free ( &dsi->loldselver, NULL );
    /*list_free ( &dsi->loldseledg, NULL );*/
    list_free ( &dsi->loldselfac, NULL );

    free ( dsi );
}

/******************************************************************************/
static void deleteSelectedItems_free ( void    *data, 
                                       uint32_t commit ) {
    URMDELSELITEMS *dsi = ( URMDELSELITEMS * ) data;

    if ( commit ) {
        list_exec ( dsi->loldselobj, (void(*)(void*)) g3dobject_free );
        list_exec ( dsi->loldselver, (void(*)(void*)) g3dvertex_free );
        /*list_exec ( dsi->loldseledg, g3dedge_free   );*/
        list_exec ( dsi->loldselfac, (void(*)(void*)) g3dface_free   );

        list_exec ( dsi->lorphanedEdges, (void(*)(void*)) g3dedge_free   );

    }

    urmdelselitems_free ( dsi );
}

/******************************************************************************/
static void deleteSelectedItems_undo ( G3DURMANAGER *urm, 
                                       void         *data, 
                                       uint64_t      engine_flags ) {
    URMDELSELITEMS *dsi = ( URMDELSELITEMS * ) data;
    G3DMESH *mes = dsi->mes;

    if ( dsi->engine_flags & VIEWOBJECT ) {
        G3DSCENE *sce = ( G3DSCENE * ) dsi->sce;
        LIST *lselobj = dsi->loldselobj;

        g3dscene_unselectAllObjects ( sce, engine_flags );

        /*** Here we cannot just replace sce->lselobj with ***/
        /*** lsel because some flags are turned on in      ***/
        /*** objects structure when added to a scene.      ***/
        while ( lselobj ) {
            G3DOBJECT *child = ( G3DOBJECT * ) lselobj->data;

            g3dscene_selectObject ( sce, child, engine_flags );

            g3dobject_addChild ( child->parent, child, engine_flags );

            lselobj = lselobj->next;
        }

        g3dscene_checkReferredObjects ( sce );

        g3dscene_checkLights ( sce, engine_flags );
    } else {
        if ( mes && ( ( dsi->engine_flags & VIEWFACE   ) ||
                      ( dsi->engine_flags & VIEWVERTEX ) ||
                      ( dsi->engine_flags & VIEWEDGE   ) ) ) {
            LIST *ltmpver = dsi->loldselver;
            LIST *ltmpfac = dsi->loldselfac;

            while ( ltmpver ) {
                G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

                g3dmesh_addSelectedVertex ( mes, ver );


                ltmpver = ltmpver->next;
            }

            while ( ltmpfac ) {
                G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                int i;

                g3dmesh_addSelectedFace ( mes, fac );

                /*** Face vertices where unlinked by the  ***/
                /*** previous call to g3dmesh_removeFace ***/
                g3dface_linkVertices ( fac );


                ltmpfac = ltmpfac->next;
            }

            /*** Rebuild the mesh with modifiers ***/
            g3dmesh_update ( mes, NULL,
                                  NULL,
                                  NULL,
                                  UPDATEFACEPOSITION |
                                  UPDATEFACENORMAL   |
                                  UPDATEVERTEXNORMAL |
                                  RESETMODIFIERS, engine_flags );
        }
    }
}

/******************************************************************************/
static void deleteSelectedItems_redo ( G3DURMANAGER *urm, 
                                       void         *data, 
                                       uint64_t      engine_flags ) {
    URMDELSELITEMS *dsi = ( URMDELSELITEMS * ) data;
    G3DMESH *mes = dsi->mes;

    if ( dsi->engine_flags & VIEWOBJECT ) {
        G3DSCENE *sce = ( G3DSCENE * ) dsi->sce;
        LIST *lselobj = dsi->lnewselobj;

        /*** Here we cannot just replace sce->lselobj with ***/
        /*** lsel because some flags are turned on in      ***/
        /*** objects structure when added to a scene.      ***/
        g3dscene_deleteSelectedObjects ( sce, NULL, engine_flags );

        g3dscene_checkReferredObjects ( sce );

        g3dscene_checkLights ( sce, engine_flags );
    } else {
        if ( mes && ( ( dsi->engine_flags & VIEWFACE   ) ||
                      ( dsi->engine_flags & VIEWVERTEX ) ||
                      ( dsi->engine_flags & VIEWEDGE   ) ) ) {
            LIST *ltmpver = dsi->loldselver;
            LIST *ltmpfac = dsi->loldselfac;

            while ( ltmpfac ) {
                G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

                if ( fac->flags & FACESELECTED ) g3dmesh_unselectFace ( mes, fac );

                g3dmesh_removeFace ( mes, fac );


                ltmpfac = ltmpfac->next;
            }

            while ( ltmpver ) {
                G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

                if ( ver->flags & VERTEXSELECTED ) g3dmesh_unselectVertex ( mes, ver );

                g3dmesh_removeVertex ( mes, ver );


                ltmpver = ltmpver->next;
            }

            /*** Rebuild the mesh with modifiers ***/
            g3dmesh_update ( mes, NULL,
                                  NULL,
                                  NULL,
                                  UPDATEFACEPOSITION |
                                  UPDATEFACENORMAL   |
                                  UPDATEVERTEXNORMAL |
                                  RESETMODIFIERS, engine_flags );
        }
    }
}

/******************************************************************************/
void g3durm_mesh_deleteGeometry ( G3DURMANAGER *urm,
                                  G3DMESH      *mes,
                                  uint64_t      engine_flags,
                                  uint32_t      return_flags ) {
    LIST *loldselobj = NULL;
    LIST *loldselver = NULL;
    LIST *loldselfac = NULL;
    LIST *loldseledg = NULL;
    URMDELSELITEMS *dsi;

    if ( engine_flags & VIEWVERTEX ) {
        loldselver = list_copy ( mes->lselver );
        loldselfac = g3dmesh_getFaceListFromSelectedVertices ( mes );
    }

    if ( engine_flags & VIEWFACE ) {
        loldselfac = list_copy ( mes->lselfac );
    }

    if ( engine_flags & VIEWEDGE ) {
        loldselver = g3dmesh_getVertexListFromSelectedEdges ( mes );
        loldseledg = list_copy ( mes->lseledg );
        loldselfac = g3dmesh_getFaceListFromSelectedEdges ( mes );
    }

    dsi = urmdelselitems_new ( NULL,
                               NULL,
                               mes, 
                               loldselver,
                               loldseledg,
                               loldselfac,
                               engine_flags );

    g3durmanager_push ( urm, 
                        deleteSelectedItems_undo,
                        deleteSelectedItems_redo,
                        deleteSelectedItems_free,
                        dsi,
                        return_flags );

            /*g3dmesh_unselectAllVertices ( mes );*/
            /*g3dmesh_unselectAllFaces    ( mes );*/

    if ( engine_flags & VIEWVERTEX ) {
        g3dmesh_removeVerticesFromList ( mes, loldselver );
        g3dmesh_removeFacesFromList    ( mes, loldselfac );
        g3dface_getOrphanedEdgesFromList ( loldselfac, &dsi->lorphanedEdges );
    }

    if ( engine_flags & VIEWFACE ) {
        g3dmesh_removeFacesFromList    ( mes, loldselfac );
        g3dface_getOrphanedEdgesFromList ( loldselfac, &dsi->lorphanedEdges );
    }

    if ( engine_flags & VIEWEDGE ) {
        g3dmesh_removeVerticesFromList ( mes, loldselver );
        g3dmesh_removeFacesFromList    ( mes, loldselfac );
    }

    /*** Rebuild the subdivided mesh ***/
    g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          UPDATEFACEPOSITION |
                          UPDATEFACENORMAL   |
                          UPDATEVERTEXNORMAL |
                          RESETMODIFIERS, engine_flags );
}

/******************************************************************************/
uint32_t g3durm_scene_deleteSelectedObjects ( G3DURMANAGER *urm,
                                              G3DSCENE     *sce, 
                                              uint64_t      engine_flags,
                                              uint32_t      return_flags ) {
    LIST *lremovedObjects = NULL;
    URMDELSELITEMS *dsi;
    uint32_t ret;

    /*** perform action ***/
    ret = g3dscene_deleteSelectedObjects ( sce, &lremovedObjects, engine_flags );

    g3dscene_checkReferredObjects ( sce );

    g3dscene_checkLights ( sce, engine_flags );

    /*** save state ***/
    dsi = urmdelselitems_new ( sce,
                               lremovedObjects,
                               NULL,
                               NULL,
                               NULL,
                               NULL,
                               engine_flags );

    g3durmanager_push ( urm, deleteSelectedItems_undo,
                             deleteSelectedItems_redo,
                             deleteSelectedItems_free, dsi,
                             return_flags );

    return ret;
}
