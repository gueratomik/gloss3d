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
uint64_t g3duitrackertagedit_orientation ( G3DUITRACKERTAGEDIT *ttedit, 
                                           char                *str ) {
    G3DUI *gui = ttedit->gui;
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *sel = ttedit->obj;
    G3DTAG *tag = sel->selectedTag;

    if ( sel ) {
        G3DTAG *tag = sel->selectedTag;

        if ( tag ) {
            if ( tag->type & G3DTAGTRACKERTYPE ) {
                G3DTRACKERTAG *ttag = ( G3DTRACKERTAG * ) tag;

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


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duitrackertagedit_setTarget ( G3DUITRACKERTAGEDIT *ttedit, 
                                         G3DOBJECT           *target ) {
    G3DUI *gui = ttedit->gui;
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *sel = ttedit->obj;
    G3DTAG *tag = sel->selectedTag;

    if ( sel ) {
        G3DTAG *tag = sel->selectedTag;

        if ( tag ) {
            if ( tag->type & G3DTAGTRACKERTYPE ) {
                G3DTRACKERTAG *ttag = ( G3DTRACKERTAG * ) tag;

                g3dtrackertag_setTarget ( ttag, 
                                          sel, 
                                          target,
                                          gui->engine_flags );
            }
        }
    }


    return REDRAWVIEW;
}
