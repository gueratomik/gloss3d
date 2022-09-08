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
uint64_t g3duiinstanceedit_setReference ( G3DUIINSTANCEEDIT *insedit, 
                                             uint32_t           rank ) {
    G3DUI *gui = insedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DINSTANCETYPE ) {
            G3DINSTANCE *ins = ( G3DINSTANCE * ) sel;
            LIST *ltmpobj, *lobj = NULL;
            uint32_t objRank = 0x00;
            int selected = 0x00;

            /*** flatten the object tree ***/
            g3dobject_treeToList_r ( ( G3DOBJECT * ) sce, &lobj );

            ltmpobj = lobj;

            while ( ltmpobj ) {
                G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

                if ( obj != ( G3DOBJECT * ) ins ) {
                    if ( obj->type != G3DSCENETYPE ) {
                        if ( objRank == rank ) {
                            g3dinstance_setReference ( ins, obj );
                        }

                        objRank++;
                    }
                }

                ltmpobj = ltmpobj->next;
            }

            list_free ( &lobj, NULL );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duiinstanceedit_mirroredToggle ( G3DUIINSTANCEEDIT *insedit ) {
    G3DUI *gui = insedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DINSTANCETYPE ) {
            G3DINSTANCE *ins = ( G3DINSTANCE * ) sel;

            if ( ((G3DOBJECT*)ins)->flags & INSTANCEMIRRORED ) {
                g3dinstance_unsetMirrored ( ins );
            } else {
                g3dinstance_setMirrored ( ins );
            }
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duiinstanceedit_orientation ( G3DUIINSTANCEEDIT *insedit,
                                            char              *str ) {
    G3DUI *gui = insedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DINSTANCETYPE ) {
            G3DINSTANCE *ins = ( G3DINSTANCE * ) sel;

            if ( strcmp ( str, ZXSTR ) == 0x00 ) {
                g3dinstance_setOrientation ( ins, INSTANCEZX );
            }

            if ( strcmp ( str, XYSTR ) == 0x00 ) {
                g3dinstance_setOrientation ( ins, INSTANCEXY );
            }

            if ( strcmp ( str, YZSTR ) == 0x00 ) {
                g3dinstance_setOrientation ( ins, INSTANCEYZ );
            }
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}
