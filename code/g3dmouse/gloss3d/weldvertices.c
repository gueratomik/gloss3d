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
#include <g3dmouse.h>

/******************************************************************************/
/* each function must return FALSE for redrawing the OGL Widget it belongs to */
/* only or TRUE to redraw all OGL Widgets                                     */
/******************************************************************************/

static uint32_t weldVertices_init  ( G3DMOUSETOOL *mou, 
                                     G3DSCENE     *sce, 
                                     G3DURMANAGER *urm, 
                                     uint64_t      engine_flags );
static uint32_t weldNeighbours_init ( G3DMOUSETOOL *mou, 
                                      G3DSCENE     *sce, 
                                      G3DURMANAGER *urm, 
                                      uint64_t      engine_flags );

/******************************************************************************/
G3DMOUSETOOLWELDVERTICES *g3dmousetoolweldvertices_new ( ) {
    void *memarea = calloc ( 0x01, sizeof ( G3DMOUSETOOLWELDVERTICES ) );
    G3DMOUSETOOLWELDVERTICES *wv = ( G3DMOUSETOOLWELDVERTICES * ) memarea;

    if ( wv == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    g3dmousetool_init ( wv,
                        WELDVERTICESTOOL,
                        's',
                        NULL,
                        weldVertices_init,
                        NULL,
                        NULL,
                        MOUSETOOLNOCURRENT );

    return wv;
}

/******************************************************************************/
G3DMOUSETOOLWELDNEIGHBOURS *g3dmousetoolweldneighbours_new ( ) {
    void *memarea = calloc ( 0x01, sizeof ( G3DMOUSETOOLWELDNEIGHBOURS ) );
    G3DMOUSETOOLWELDNEIGHBOURS *wn = ( G3DMOUSETOOLWELDNEIGHBOURS * ) memarea;

    if ( wn == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    g3dmousetool_init ( wn,
                        WELDNEIGHBOURSTOOL,
                        's',
                        NULL,
                        weldNeighbours_init,
                        NULL,
                        NULL,
                        MOUSETOOLNOCURRENT );

    return wn;
}

/******************************************************************************/
static uint32_t weldVertices_init  ( G3DMOUSETOOL *mou,
                                     G3DSCENE     *sce,
                                     G3DURMANAGER *urm,
                                     uint64_t      engine_flags ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( ( obj ) && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3durm_mesh_weldSelectedVertices ( urm, 
                                           sce,
                                           mes,
                                           0x01, 
                                           engine_flags,
                                           REDRAWVIEW );

        return REDRAWVIEW;
    }

    return 0x00;
}

/******************************************************************************/
static uint32_t weldNeighbours_init ( G3DMOUSETOOL *mou,
                                      G3DSCENE     *sce,
                                      G3DURMANAGER *urm,
                                      uint64_t      engine_flags ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( ( obj ) && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3durm_mesh_weldNeighbourVertices ( urm, 
                                            sce,
                                            mes,
                                            0x01,
                                            0.1f,
                                            engine_flags,
                                            REDRAWVIEW );

        return REDRAWVIEW;
    }

    return 0x00;
}
