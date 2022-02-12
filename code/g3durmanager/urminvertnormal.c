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
static URMINVERTNORMAL *urminvertnormal_new ( G3DMESH *mes ) {
    uint32_t structsize = sizeof ( URMINVERTNORMAL );

    URMINVERTNORMAL *ins = ( URMINVERTNORMAL * ) calloc ( 0x01, structsize );

    if ( ins == NULL ) {
        fprintf ( stderr, "urminvertnormal_new: memory allocation falied\n" );

        return NULL;
    }

    ins->mes  = mes;
    ins->lfac = list_copy ( mes->lselfac );


    return ins;
}

/******************************************************************************/
static void urminvertnormal_free ( URMINVERTNORMAL *ins ) {
    list_free ( &ins->lfac, NULL );

    free ( ins );
}

/******************************************************************************/
static void invertNormal_free ( void    *data, 
                                uint32_t commit ) {
    URMINVERTNORMAL *ins = ( URMINVERTNORMAL * ) data;

    urminvertnormal_free ( ins );
}

/******************************************************************************/
static void invertNormal_undo ( G3DURMANAGER *urm, 
                                void         *data,
                                uint64_t      engine_flags ) {
    URMINVERTNORMAL *ins = ( URMINVERTNORMAL * ) data;
    G3DMESH *mes = ins->mes;

    list_exec ( ins->lfac, (void (*)(void *)) g3dface_invertNormal );

    mes->obj.update_flags |= ( UPDATEFACEPOSITION |
                               UPDATEFACENORMAL   |
                               UPDATEVERTEXNORMAL |
                               RESETMODIFIERS );

    /*** Rebuild the mesh with modifiers ***/
    g3dmesh_update ( mes, engine_flags );
}

/******************************************************************************/
static void invertNormal_redo ( G3DURMANAGER *urm, 
                                void         *data,
                                uint64_t      engine_flags ) {

    invertNormal_undo ( urm, data, engine_flags );
}

/******************************************************************************/
void g3durm_mesh_invertNormal ( G3DURMANAGER *urm, 
                                G3DMESH      *mes,
                                uint64_t      engine_flags,
                                uint32_t      return_flags ) {
    URMINVERTNORMAL *ins;

    /*** Triagulate and unTriagulate feature use ***/
    /*** the same functions and data structures. ***/
    ins = urminvertnormal_new ( mes );

    g3dmesh_invertNormal ( mes );

    mes->obj.update_flags |= ( UPDATEFACEPOSITION |
                               UPDATEFACENORMAL   |
                               UPDATEVERTEXNORMAL |
                               RESETMODIFIERS );

    /*** Rebuild the mesh with modifiers ***/
    g3dmesh_update ( mes, engine_flags );

    g3durmanager_push ( urm, invertNormal_undo,
                             invertNormal_redo,
                             invertNormal_free, ins, return_flags );
}
