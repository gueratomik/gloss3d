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
uint64_t g3duitextedit_setText ( G3DUITEXTEDIT *txtedit,
                                    char          *newText ) {
    G3DUI *gui = txtedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DTEXTTYPE ) {
            G3DTEXT *txt = ( G3DTEXT * ) sel;

            if ( txt->text && newText ) {
                char *firstOccurence = strstr ( newText, txt->text );

                if ( firstOccurence == newText ) {
                    g3dtext_addText ( txt, newText + strlen ( txt->text ), gui->engine_flags );
                } else {
                    g3dtext_setText ( txt, newText, gui->engine_flags );
                }
            }
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duitextedit_thickness ( G3DUITEXTEDIT *txtedit,
                                      float          thickness ) {
    G3DUI *gui = txtedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DTEXTTYPE ) {
            G3DTEXT *txt = ( G3DTEXT * ) sel;

            g3dtext_setThickness ( txt, thickness, gui->engine_flags );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duitextedit_roundness ( G3DUITEXTEDIT *txtedit,
                                      uint32_t       roundness ) {
    G3DUI *gui = txtedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DTEXTTYPE ) {
            G3DTEXT *txt = ( G3DTEXT * ) sel;

            g3dtext_setRoundness ( txt, roundness, gui->engine_flags );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duitextedit_size ( G3DUITEXTEDIT *txtedit,
                                 uint32_t       size ) {
    G3DUI *gui = txtedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DTEXTTYPE ) {
            G3DTEXT *txt = ( G3DTEXT * ) sel;

            g3dtext_setSize ( txt, size, gui->engine_flags );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}
