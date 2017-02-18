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

/******************************************************************************/
void g3duicom_goToFrame ( G3DUI *gui, float frame ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Display *dis = XtDisplay ( gmt->main );
    Window win = XtWindow ( gmt->main );
    XClientMessageEvent client ;

    client.type      = ClientMessage ;
    client.display   = dis; /* Could be remote */
    client.window    = win; /* Could be remote */

    /*client.send_event   = True;*/
    client.format    = 32 ; /*** Why is this needed (crash o/w) ? ***/
    client.data.l[0] = COMGOTOFRAME;

    memcpy ( &client.data.l[1], &frame, sizeof ( float ) );

    /* Send the data off to the other process */
    XSendEvent ( dis, win, True, NoEventMask, (XEvent*) &client ) ;

    XFlush ( client.display ) ;
}

/******************************************************************************/
static uint32_t gotoframe_draw ( R3DFILTER *fil, R3DSCENE *rsce,
                                                 float frameID,
                                                 unsigned char *img, 
                                                 uint32_t from, 
                                                 uint32_t to, 
                                                 uint32_t depth, 
                                                 uint32_t width ) {
    G3DUI *gui = ( G3DUI * ) fil->data;
    float nextframe = frameID + 1.0f;

    g3duicom_goToFrame ( gui, nextframe );


    return 0x00;
}

/******************************************************************************/
R3DFILTER *r3dfilter_gotoframe_new ( G3DUI *gui ) {
    R3DFILTER *fil;

    fil = r3dfilter_new ( FILTERIMAGE, GOTOFRAMEFILTERNAME,
                                       gotoframe_draw, /*** Jump to next frame ***/
                                       NULL,           /*** don't free ***/
                                       gui );          /*** userData   ***/

    return fil;
}

