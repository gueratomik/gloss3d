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
uint64_t g3duisymmetryedit_limitCbk ( G3DUISYMMETRYEDIT *symedit,
                                      float              limit ) {
    G3DUI *gui = symedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DSYMMETRYTYPE ) {
            G3DSYMMETRY *sym = ( G3DSYMMETRY * ) sel;
            LIST *ltmpobj = ((G3DOBJECT*)sym)->lchildren;

            g3dsymmetry_setMergeLimit ( sym, limit );

            while ( ltmpobj ) {
                G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

                if ( obj->type == G3DMESHTYPE ) {
                    G3DMESH *mes = ( G3DMESH * ) obj;

                    g3dsymmetry_meshChildChange ( sym, mes );

                    mes->obj.update_flags |= ( UPDATEVERTEXNORMAL |
                                               UPDATEFACENORMAL | 
                                               RESETMODIFIERS );

                    g3dmesh_update ( mes, gui->engine_flags );
                }

                ltmpobj = ltmpobj->next;
            }
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duisymmetryedit_planeCbk ( G3DUISYMMETRYEDIT *symedit,
                                      char              *orientation ) {
    G3DUI *gui = symedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DSYMMETRYTYPE ) {
            G3DSYMMETRY *sym = ( G3DSYMMETRY * ) sel;

            if ( strcmp ( orientation, ZXSTR ) == 0x00 ) {
                g3dsymmetry_setPlane ( sym, SYMMETRYZX );
            }

            if ( strcmp ( orientation, XYSTR ) == 0x00 ) {
                g3dsymmetry_setPlane ( sym, SYMMETRYXY );
            }

            if ( strcmp ( orientation, YZSTR ) == 0x00 ) {
                g3dsymmetry_setPlane ( sym, SYMMETRYYZ );
            }
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}
