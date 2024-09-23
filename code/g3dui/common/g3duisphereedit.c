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
uint64_t g3duisphereedit_togglePerfect ( G3DUISPHEREEDIT *sphedit ) {
    G3DUI *gui = sphedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DSPHERETYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;
            SPHEREDATASTRUCT *sds = ( SPHEREDATASTRUCT * ) pri->data;

            if ( sel->flags & SPHEREISPERFECT ) {
                sel->flags &= (~SPHEREISPERFECT);
            } else {
                sel->flags |=   SPHEREISPERFECT;
            }
        }

        ltmpselobj = ltmpselobj->next;
    }

    g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duisphereedit_radius ( G3DUISPHEREEDIT *sphedit,
                                     float            radius ) {
    G3DUI *gui = sphedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DSPHERETYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;
            SPHEREDATASTRUCT *sds = ( SPHEREDATASTRUCT * ) pri->data;

            g3dsphere_build ( pri,
                              sds->slice,
                              sds->cap,
                              radius );
        }

        ltmpselobj = ltmpselobj->next;
    }

    g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duisphereedit_cap ( G3DUISPHEREEDIT *sphedit,
                                  uint32_t         cap ) {
    G3DUI *gui = sphedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DSPHERETYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;
            SPHEREDATASTRUCT *sds = ( SPHEREDATASTRUCT * ) pri->data;

            g3dsphere_build ( pri,
                              sds->slice,
                              cap,
                              sds->radius );
        }

        ltmpselobj = ltmpselobj->next;
    }

    g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duisphereedit_slice ( G3DUISPHEREEDIT *sphedit,
                                    uint32_t         slice ) {
    G3DUI *gui = sphedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DSPHERETYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;
            SPHEREDATASTRUCT *sds = ( SPHEREDATASTRUCT * ) pri->data;

            g3dsphere_build ( pri,
                              slice,
                              sds->cap,
                              sds->radius );
        }

        ltmpselobj = ltmpselobj->next;
    }

    g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );

    return REDRAWVIEW;
}
