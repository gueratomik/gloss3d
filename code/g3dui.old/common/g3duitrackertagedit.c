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
void common_g3duitrackertagedit_orientationCbk ( G3DUI *gui, 
                                                 char  *str ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *sel = g3dscene_getLastSelected ( sce );
    G3DTAG *tag = sel->seltag;

    if ( gui->lock ) return;

    if ( sel ) {
        G3DTAG *tag = sel->seltag;

        if ( tag ) {
            if ( tag->type & G3DTAGTRACKERTYPE ) {
                G3DTRACKERTAG *ttag = tag;

                if ( strcmp ( str, XSTR ) == 0x00 ) {
                    g3dtrackertag_setOrientation ( ttag, 
                                                   TARGET_XAXIS );
                }

                if ( strcmp ( str, YSTR ) == 0x00 ) {
                    g3dtrackertag_setOrientation ( ttag, 
                                                   TARGET_YAXIS );
                }

                if ( strcmp ( str, ZSTR ) == 0x00 ) {
                    g3dtrackertag_setOrientation ( ttag, 
                                                   TARGET_ZAXIS );
                }
            }
        }

        g3dobject_updateMatrix_r ( sel, gui->engine_flags );
    }

    g3dui_redrawGLViews ( gui );
}

/******************************************************************************/
void common_g3duitrackertagedit_setTargetCbk ( G3DUI   *gui, 
                                               uint32_t rank ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *sel = g3dscene_getLastSelected ( sce );
    G3DTAG *tag = sel->seltag;

    if ( gui->lock ) return;

    if ( sel ) {
        G3DTAG *tag = sel->seltag;

        if ( tag ) {
            if ( tag->type & G3DTAGTRACKERTYPE ) {
                G3DTRACKERTAG *ttag = tag;
                LIST *ltmpobj, *lobj = NULL;
                uint32_t objRank = 0x00;

                /*** flatten the object tree ***/
                g3dobject_treeToList_r ( sce, &lobj );

                ltmpobj = lobj;

                while ( ltmpobj ) {
                    G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

                    if ( objRank == rank ) {
                        g3dtrackertag_setTarget ( ttag, 
                                                  sel, 
                                                  obj,
                                                  gui->engine_flags );
                    }

                    objRank++;

                    ltmpobj = ltmpobj->next;
                }

                list_free ( &lobj, NULL );
            }
        }
    }

    g3dui_redrawGLViews ( gui );
}
