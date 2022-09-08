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
uint64_t g3duitorusedit_slice ( G3DUITORUSEDIT *toredit,
                                   uint32_t        slice ) {
    G3DUI *gui = toredit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DTORUSTYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;
            TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;

            g3dtorus_build ( pri, 
                             tds->orientation, 
                             slice,
                             tds->cap,
                             tds->extrad, 
                             tds->intrad );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duitorusedit_cap ( G3DUITORUSEDIT *toredit,
                                 uint32_t        cap ) {
    G3DUI *gui = toredit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DTORUSTYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;
            TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;

            g3dtorus_build ( pri,
                             tds->orientation, 
                             tds->slice,
                             cap,
                             tds->extrad, 
                             tds->intrad );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duitorusedit_extRadius ( G3DUITORUSEDIT *toredit,
                                       float           extrad ) {
    G3DUI *gui = toredit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DTORUSTYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;
            TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;

            g3dtorus_build ( pri, 
                             tds->orientation, 
                             tds->slice,
                             tds->cap,
                             extrad, 
                             tds->intrad );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duitorusedit_intRadius ( G3DUITORUSEDIT *toredit,
                                       float           intrad ) {
    G3DUI *gui = toredit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DTORUSTYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;
            TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;

            g3dtorus_build ( pri,
                             tds->orientation, 
                             tds->slice,
                             tds->cap,
                             tds->extrad, 
                             intrad );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duitorusedit_orientation ( G3DUITORUSEDIT *toredit,
                                         const char     *oristr ) {
    G3DUI *gui = toredit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DTORUSTYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;
            TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;
            uint32_t orientation;

            if ( strcmp ( oristr, ZXSTR ) == 0x00 ) {
                orientation = TORUSZX;
            }

            if ( strcmp ( oristr, XYSTR ) == 0x00 ) {
                orientation = TORUSXY;
            }

            if ( strcmp ( oristr, YZSTR ) == 0x00 ) {
                orientation = TORUSYZ;
            }

            g3dtorus_build ( pri, 
                             orientation,
                             tds->slice,
                             tds->cap,
                             tds->extrad,
                             tds->intrad );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

