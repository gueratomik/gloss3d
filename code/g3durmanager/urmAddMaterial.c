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
URMADDMATERIAL *urmaddmaterial_new ( G3DSCENE    *sce,
                                     G3DMATERIAL *mat,
                                     uint64_t engine_flags ) {
    uint32_t structsize = sizeof ( URMADDMATERIAL );

    URMADDMATERIAL *uam = ( URMADDMATERIAL * ) calloc ( 0x01, structsize );

    if ( uam == NULL ) {
        fprintf ( stderr, "%s: memory allocation falied\n", __func__ );

        return NULL;
    }

    uam->sce = sce;
    uam->mat = mat;

    return uam;
}

/******************************************************************************/
void urmaddmaterial_free ( URMADDMATERIAL *uam ) {
    free ( uam );
}

/******************************************************************************/
void addMaterial_free ( void *data, uint32_t commit ) {
    URMADDMATERIAL *uam = ( URMADDMATERIAL * ) data;

    if ( commit ) {

    } else {
        g3dmaterial_free ( uam->mat );
    }

    urmaddmaterial_free ( uam );
}

/******************************************************************************/
void addMaterial_undo ( G3DURMANAGER *urm, void *data, uint32_t flags ) {
    URMADDMATERIAL *uam = ( URMADDMATERIAL * ) data;

    g3dscene_removeMaterial ( uam->sce, uam->mat );
}

/******************************************************************************/
void addMaterial_redo ( G3DURMANAGER *urm, void *data, uint32_t flags ) {
    URMADDMATERIAL *uam = ( URMADDMATERIAL * ) data;

    g3dscene_addMaterial ( uam->sce, uam->mat );
}

/******************************************************************************/
void g3durm_scene_addMaterial ( G3DURMANAGER *urm,
                                G3DSCENE     *sce,
                                G3DMATERIAL  *mat,
                                uint64_t engine_flags,
                                uint32_t      return_flags ) {
    URMADDMATERIAL *uam = urmaddmaterial_new ( sce, mat, engine_flags );

    g3dscene_addMaterial ( sce, mat );

    g3durmanager_push ( urm, 
                        addMaterial_undo,
                        addMaterial_redo,
                        addMaterial_free, 
                        uam,
                        return_flags );
}
