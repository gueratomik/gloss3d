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
uint64_t g3duiplaneedit_divCbk  ( G3DUIPLANEEDIT *plnedit,
                                  G3DUIAXIS       axis,
                                  int             div ) {
    G3DUI *gui = plnedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DPLANETYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;
            PLANEDATASTRUCT *pds = ( PLANEDATASTRUCT * ) pri->data;

            if ( axis == G3DUIUAXIS ) g3dplane_build ( pri, 
                                                       pds->orientation, 
                                                       div, 
                                                       pds->nbv, 
                                                       pds->radu, 
                                                       pds->radv );

            if ( axis == G3DUIVAXIS ) g3dplane_build ( pri,
                                                       pds->orientation, 
                                                       pds->nbu, 
                                                       div, 
                                                       pds->radu, 
                                                       pds->radv );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duiplaneedit_radiusCbk ( G3DUIPLANEEDIT *plnedit,
                                    G3DUIAXIS       axis,
                                    float           radius ) {
    G3DUI *gui = plnedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DPLANETYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;
            PLANEDATASTRUCT *pds = ( PLANEDATASTRUCT * ) pri->data;

            if ( axis == G3DUIUAXIS ) g3dplane_build ( pri, 
                                                       pds->orientation, 
                                                       pds->nbu, 
                                                       pds->nbv, 
                                                       radius, 
                                                       pds->radv );

            if ( axis == G3DUIVAXIS ) g3dplane_build ( pri, 
                                                       pds->orientation, 
                                                       pds->nbu, 
                                                       pds->nbv, 
                                                       pds->radu, 
                                                       radius );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duiplaneedit_orientationcbk ( G3DUIPLANEEDIT *plnedit,
                                         const char     *oristr ) {
    G3DUI *gui = plnedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DPLANETYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;
            PLANEDATASTRUCT *pds = ( PLANEDATASTRUCT * ) pri->data;
            uint32_t orientation;

            if ( strcmp ( oristr, ZXSTR ) == 0x00 ) {
                orientation = PLANEZX;
            }

            if ( strcmp ( oristr, XYSTR ) == 0x00 ) {
                orientation = PLANEXY;
            }

            if ( strcmp ( oristr, YZSTR ) == 0x00 ) {
                orientation = PLANEYZ;
            }

            g3dplane_build ( pri,
                             orientation,
                             pds->nbu,
                             pds->nbv,
                             pds->radu,
                             pds->radv );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}
