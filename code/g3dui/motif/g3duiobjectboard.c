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
Widget createObjectBoard ( Widget parent, G3DUI *gui, char *name,
                           Dimension x    , Dimension y,
                           Dimension width, Dimension height ) {
    Pixel white = XWhitePixel ( XtDisplay ( parent ), 0x00 );
    Pixel background, foreground;
    Widget curedit, objlist, tab, frm;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    frm = XmVaCreateForm ( parent, name,
                                   XtNx, x,
                                   XtNy, y,
                                   XtNwidth , width,
                                   XtNheight, height,
                                   XmNforeground, foreground,
                                   XmNbackground, background,
                                   XmNborderWidth, 0x00,
                                   NULL );

    createObjectList ( frm, gui, "Objects", 0x00, 0x00, 320, 320 );

    /**** BOTTOM TABs ****/
    tab =  XmVaCreateManagedTabbedBook ( frm, "Tabs",
                                              XmNx, 0x00,
                                              XmNy, 0x140,
                                              XmNwidth, 0x140,
                                              XmNheight, 0x140,
                                              XmNfontList, gmt->fontlist,
                                              NULL );
/*
    createCurrentMouseTool ( tab, gui, "Tool"    , 0x00, 0x00, 0x140, 0x90 );
    createCoordinatesEdit  ( tab, gui, "Coords"  , 0x00, 0x00, 0x140, 0x90 );*/
    createCurrentEdit      ( tab, gui, "Object"    , 0, 0, 310, 192 );


    XtManageChild ( frm );


    return frm;
}

