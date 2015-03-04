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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
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
#include <g3dui_motif.h>

/******************************************************************************/
/****************** Event handler for interprocess communication **************/
void g3duicom_communicate ( Widget w, XtPointer client, 
                                      XEvent *event, 
                                      Boolean *c ) {
    G3DUI *gui = ( G3DUI * ) client;

    /*** Force disabling real time subdivision ***/
    /*** otherwise g3dobject_anim_r() may call some buffered subdivision ***/
    /*** updates and that;s gonna be really slow. We prefer to rely ***/
    /*** On real time stuff ***/
    gui->flags |= ONGOINGANIMATION;

    if ( event->type == ClientMessage) {
        XClientMessageEvent *cme = (XClientMessageEvent *) event ;

        if ( cme->data.l[0x00] == COMGOTOFRAME ) {
            float frame;

            memcpy ( &frame, &cme->data.l[0x01], sizeof ( float ) );

            g3dobject_anim_r ( ( G3DOBJECT * ) gui->sce, frame, gui->flags );

            g3dui_redrawGLViews ( gui );

            gui->curframe = frame;
        }
    }

    /*** Re-enable real time subdivision ***/
    gui->flags &= (~ONGOINGANIMATION);
}
