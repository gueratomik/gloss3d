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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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
void common_g3duisymmetryedit_limitCbk ( G3DUI *gui, float limit ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DSYMMETRYTYPE ) ) {
        G3DSYMMETRY *sym = ( G3DSYMMETRY * ) obj;
        LIST *ltmpobj = ((G3DOBJECT*)sym)->lchildren;

        g3dsymmetry_setMergeLimit ( sym, limit );

        while ( ltmpobj ) {
            G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

            if ( obj->type == G3DMESHTYPE ) {
                G3DMESH *mes = ( G3DMESH * ) obj;

                g3dsymmetry_meshChildChange ( sym, mes );

                g3dmesh_update ( mes, NULL,
                                      NULL,
                                      NULL,
                                      UPDATEVERTEXNORMAL |
                                      UPDATEFACENORMAL | 
                                      RESETMODIFIERS, gui->flags );
            }

            ltmpobj = ltmpobj->next;
        }

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3duisymmetryedit_planeCbk ( G3DUI *gui, char *orientation ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DSYMMETRYTYPE ) ) {
        G3DSYMMETRY *sym = ( G3DSYMMETRY * ) obj;

        if ( strcmp ( orientation, ZXSTR ) == 0x00 ) {
            g3dsymmetry_setPlane ( sym, SYMMETRYZX );
        }

        if ( strcmp ( orientation, XYSTR ) == 0x00 ) {
            g3dsymmetry_setPlane ( sym, SYMMETRYXY );
        }
        
        if ( strcmp ( orientation, YZSTR ) == 0x00 ) {
            g3dsymmetry_setPlane ( sym, SYMMETRYYZ );
        }

        g3dui_redrawGLViews ( gui );
    }
}
