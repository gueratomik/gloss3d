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
void g3durm_mesh_triangulate ( G3DURMANAGER *urm, G3DMESH *mes, int clockwise,
                                                  uint32_t engine_flags,
                                                  uint32_t return_flags ) {
    LIST *loldfac = NULL,
         *lnewfac = NULL;
    URMTRIANGULATE *ums;

    g3dmesh_triangulate ( mes, &loldfac, &lnewfac, clockwise );

    /*** Rebuild the mesh with modifiers ***/
    g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          UPDATEFACEPOSITION |
                          UPDATEFACENORMAL   |
                          UPDATEVERTEXNORMAL |
                          RESETMODIFIERS, engine_flags );

    /*** Triagulate and unTriagulate feature use ***/
    /*** the same functions and data structures. ***/
    ums = urmuntriangulate_new ( mes, loldfac, lnewfac );

    g3durmanager_push ( urm, unTriangulate_undo,
                             unTriangulate_redo,
                             unTriangulate_free, ums, return_flags );
}
