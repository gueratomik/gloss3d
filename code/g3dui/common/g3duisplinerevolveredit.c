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
#include <g3dui.h>

/******************************************************************************/
uint64_t g3duisplinerevolveredit_splineRevolverSteps ( G3DUISPLINEREVOLVEREDIT *srvedit,
                                                          uint32_t                 level ) {
    G3DUI *gui = srvedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DSPLINEREVOLVERTYPE ) {
            G3DSPLINEREVOLVER *srv = ( G3DSPLINEREVOLVER * ) sel;

            srv->nbsteps = level;

            g3dsplinerevolver_reshape ( srv, gui->engine_flags );

        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}


/******************************************************************************/
uint64_t g3duisplinerevolveredit_splineRevolverDivis ( G3DUISPLINEREVOLVEREDIT *srvedit,  
                                                          uint32_t                 level ) {
    G3DUI *gui = srvedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DSPLINEREVOLVERTYPE ) {
            G3DSPLINEREVOLVER *srv = ( G3DSPLINEREVOLVER * ) sel;

            srv->nbdivis = level;

            g3dsplinerevolver_reshape ( srv, gui->engine_flags );

            /*g3dmesh_setSubdivisionLevel ( mes, level, gui->engine_flags );*/
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}
