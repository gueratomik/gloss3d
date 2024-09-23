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
uint64_t g3duicylinderedit_length ( G3DUICYLINDEREDIT *cyledit, 
                                       float              length ) {
    G3DUI *gui = cyledit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DCYLINDERTYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;
            CYLINDERDATASTRUCT *cds = ( CYLINDERDATASTRUCT * ) pri->data;

            g3dcylinder_build ( pri, 
                                cds->slice,
                                cds->capx,
                                cds->capy,
                                cds->radius,
                                length );
        }

        ltmpselobj = ltmpselobj->next;
    }

    g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duicylinderedit_radius ( G3DUICYLINDEREDIT *cyledit, 
                                       float              radius ) {
    G3DUI *gui = cyledit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DCYLINDERTYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;
            CYLINDERDATASTRUCT *cds = ( CYLINDERDATASTRUCT * ) pri->data;

            g3dcylinder_build ( pri,
                                cds->slice,
                                cds->capx,
                                cds->capy,
                                radius,
                                cds->length );

        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duicylinderedit_cap ( G3DUICYLINDEREDIT *cyledit, 
                                    G3DUIAXIS          axis,
                                    int                cap ) {
    G3DUI *gui = cyledit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DCYLINDERTYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;
            CYLINDERDATASTRUCT *cds = ( CYLINDERDATASTRUCT * ) pri->data;

            if ( axis == G3DUIXAXIS ) g3dcylinder_build ( pri, cds->slice, 
                                                               cap,
                                                               cds->capy,
                                                               cds->radius,
                                                               cds->length );

            if ( axis == G3DUIYAXIS ) g3dcylinder_build ( pri, cds->slice, 
                                                               cds->capy,
                                                               cap,
                                                               cds->radius,
                                                               cds->length );

        }

        ltmpselobj = ltmpselobj->next;
    }

    g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duicylinderedit_slice ( G3DUICYLINDEREDIT *cyledit, 
                                      int                slice ) {
    G3DUI *gui = cyledit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DCYLINDERTYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;
            CYLINDERDATASTRUCT *cds = ( CYLINDERDATASTRUCT * ) pri->data;

            g3dcylinder_build ( pri, slice, cds->capx, 
                                            cds->capy, 
                                            cds->radius,
                                            cds->length );
        }

        ltmpselobj = ltmpselobj->next;
    }

    g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );

    return REDRAWVIEW;
}
