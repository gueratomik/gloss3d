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
void g3duirenderwindow_resize ( G3DUIRENDERWINDOW *rwin,
                                uint32_t           width, 
                                uint32_t           height ) {
    G3DUI *gui = rwin->gui;

    rwin->menurec.x      = 0;
    rwin->menurec.y      = 0;
    rwin->menurec.width  = width;
    rwin->menurec.height = 32;

    rwin->statusrec.x      = 0;
    rwin->statusrec.y      = height - 32;
    rwin->statusrec.width  = width;
    rwin->statusrec.height = 32;

    rwin->scrrec.x      = 0;
    rwin->scrrec.y      = 32;
    rwin->scrrec.width  = width;
    rwin->scrrec.height = height - 
                          rwin->menurec.height - 
                          rwin->statusrec.height;

    if ( rwin->displayMode == RENDERWINDOW_FIT_SIZE ) {
        rwin->arearec.x      = 0;
        rwin->arearec.y      = 0;
        rwin->arearec.width  = rwin->scrrec.width;
        rwin->arearec.height = rwin->scrrec.height;
    }


    if ( rwin->displayMode == RENDERWINDOW_FULL_SIZE ) {
        Q3DSETTINGS *rsg = gui->currsg;

        rwin->arearec.x      = 0;
        rwin->arearec.y      = 0;
        rwin->arearec.width  = rsg->output.width;
        rwin->arearec.height = rsg->output.height;
    }
}
