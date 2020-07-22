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
URMCREATEFACEGROUP *urmcreatefacegroup_new ( G3DMESH      *mes, 
                                             G3DFACEGROUP *facgrp ) {
    uint32_t structsize = sizeof ( URMCREATEFACEGROUP );

    URMCREATEFACEGROUP *ucf = ( URMCREATEFACEGROUP * ) calloc ( 0x01, structsize );

    if ( ucf == NULL ) {
        fprintf ( stderr, "%s: memory allocation falied\n", __func__ );

        return NULL;
    }

    ucf->mes    = mes;
    ucf->facgrp = facgrp;

    return ucf;
}

/******************************************************************************/
void urmcreatefacegroup_free ( URMCREATEFACEGROUP *ucf ) {
    free ( ucf );
}

/******************************************************************************/
void createFacegroup_free ( void *data, uint32_t commit ) {
    URMCREATEFACEGROUP *ucf = ( URMCREATEFACEGROUP * ) data;

    if ( commit ) {
        /* */
    } else {
        g3dfacegroup_free ( ucf->facgrp );
    }

    urmcreatefacegroup_free ( ucf );
}

/******************************************************************************/
void createFacegroup_undo ( G3DURMANAGER *urm, void *data, uint64_t engine_flags ) {
    URMCREATEFACEGROUP *ucf = ( URMCREATEFACEGROUP * ) data;

    g3dmesh_removeFacegroup ( ucf->mes, ucf->facgrp );
}

/******************************************************************************/
void createFacegroup_redo ( G3DURMANAGER *urm, void *data, uint64_t engine_flags ) {
    URMCREATEFACEGROUP *ucf = ( URMCREATEFACEGROUP * ) data;

    g3dmesh_addFacegroup ( ucf->mes, ucf->facgrp );
}

/******************************************************************************/
void g3durm_mesh_createFacegroup ( G3DURMANAGER *urm,
                                   G3DMESH      *mes,
                                   const char   *name,
                                   uint64_t engine_flags,
                                   uint32_t      return_flags ) {
    G3DFACEGROUP *facgrp = g3dfacegroup_new ( name, mes->lselfac );
    URMCREATEFACEGROUP *ucf;

    g3dmesh_addFacegroup ( mes, facgrp );

    ucf = urmcreatefacegroup_new ( mes, facgrp );

    g3durmanager_push ( urm, createFacegroup_undo,
                             createFacegroup_redo,
                             createFacegroup_free, ucf, return_flags );
}
