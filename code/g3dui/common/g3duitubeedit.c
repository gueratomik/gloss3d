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
uint64_t g3duitubeedit_length ( G3DUITUBEEDIT *tubedit,
                                   float          length ) {
    G3DUI *gui = tubedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DTUBETYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;
            TUBEDATASTRUCT *cds = ( TUBEDATASTRUCT * ) pri->data;

            g3dtube_build ( pri, 
                            cds->slice, 
                            cds->capx,
                            cds->capy, 
                            cds->radius, 
                            cds->thickness,
                            length );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duitubeedit_radius ( G3DUITUBEEDIT *tubedit,
                                   float          radius ) {
    G3DUI *gui = tubedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DTUBETYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;
            TUBEDATASTRUCT *cds = ( TUBEDATASTRUCT * ) pri->data;

            g3dtube_build ( pri, 
                            cds->slice, 
                            cds->capx,
                            cds->capy, 
                            radius, 
                            cds->thickness,
                            cds->length );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duitubeedit_thickness ( G3DUITUBEEDIT *tubedit,
                                      float          thickness ) {
    G3DUI *gui = tubedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DTUBETYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;
            TUBEDATASTRUCT *cds = ( TUBEDATASTRUCT * ) pri->data;

            g3dtube_build ( pri,
                            cds->slice, 
                            cds->capx,
                            cds->capy, 
                            cds->radius, 
                            thickness, 
                            cds->length );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duitubeedit_cap ( G3DUITUBEEDIT *tubedit,
                                G3DUIAXIS      axis,
                                uint32_t       cap ) {
    G3DUI *gui = tubedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DTUBETYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;
            TUBEDATASTRUCT *cds = ( TUBEDATASTRUCT * ) pri->data;

            if ( axis == G3DUIXAXIS ) g3dtube_build ( pri, 
                                                      cds->slice, 
                                                      cap,
                                                      cds->capy,
                                                      cds->radius,
                                                      cds->thickness,
                                                      cds->length );

            if ( axis == G3DUIYAXIS ) g3dtube_build ( pri, 
                                                      cds->slice, 
                                                      cds->capy,
                                                      cap,
                                                      cds->radius,
                                                      cds->thickness,
                                                      cds->length );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duitubeedit_slice ( G3DUITUBEEDIT *tubedit,
                                  uint32_t       slice ) {
    G3DUI *gui = tubedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DTUBETYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;
            TUBEDATASTRUCT *cds = ( TUBEDATASTRUCT * ) pri->data;

            g3dtube_build ( pri, 
                            slice, 
                            cds->capx, 
                            cds->capy, 
                            cds->radius,
                            cds->thickness,
                            cds->length );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}
