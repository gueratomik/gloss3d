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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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
#include <xpm/objectmode.xpm>
#include <xpm/facemode.xpm>
#include <xpm/vertexmode.xpm>
#include <xpm/edgemode.xpm>
#include <xpm/skinmode.xpm>
#include <xpm/uvwmapmode.xpm>

/******************************************************************************/
void g3dui_setMode ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    uint32_t modename = XtName ( w );

    common_g3dui_setMode ( gui, modename );

    g3dui_redrawGLViews ( gui );
}

/******************************************************************************/
static void addModeBarButton ( Widget parent, G3DUI *gui,
                                              char *name,
                                              char **data,
                                              void (*func)(Widget,XtPointer,
                                                                  XtPointer )){
    Display *dis = XtDisplay ( parent );
    Window   win = XtWindow  ( parent );
    Pixel background, foreground, armedcolor;
    Pixmap pixnor, pixarm;
    Widget btn;
    int depth;

    XtVaGetValues ( parent, XmNdepth, &depth,
                            XmNbackground, &background,
                            XmNforeground, &foreground,
                            XmNforeground, &armedcolor, NULL );

    createTransparentPixmap ( dis, &pixnor, &pixarm, data, depth,
                              background, armedcolor );

    btn = XmVaCreateToggleButton ( parent, name,
                                   /*** arguments ***/
                                   XmNx, 0x00,
                                   XmNy, 0x00,
                                   XmNwidth,  MODEBARBUTTONSIZE,
                                   XmNheight, MODEBARBUTTONSIZE,
                                   XmNindicatorOn, False,
                                   XmNrecomputeSize, False,
                                   XmNlabelType, XmPIXMAP,
                                   XmNlabelPixmap , pixnor,
                                   XmNselectPixmap, pixarm,
                                   XmNhighlightThickness, 0x00,
                                   XmNshadowThickness, 0x01,
                                   XmNmarginHeight, 0x00,
                                   XmNmarginWidth, 0x00,
                                   XmNbackground, background,
                                   XmNforeground, foreground,
                                   NULL );

    if ( func ) {
        XtAddCallback ( btn, XmNarmCallback, func, gui );
    }

    XtManageChild ( btn );
}

/******************************************************************************/
Widget createModeBar ( Widget parent, G3DUI *gui, char *name,
                       Dimension x    , Dimension y,
                       Dimension width, Dimension height ) {
    Pixel background, foreground;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    Widget bar = XmVaCreateRowColumn ( parent, name,
                                       XmNx, x,
                                       XmNy, y,
                                       XmNwidth, width,
                                       XmNheight, height,
                                       XmNspacing, 0x00,
                                       XmNpacking, 0x00,
                                       XmNchildType, XmFRAME_GENERIC_CHILD,
                                       XmNbackground, background,
                                       XmNforeground, foreground,
                                       XmNorientation, XmVERTICAL,
                                       XmNborderWidth, 0x00,
                                       XmNmarginWidth, 0x00,
                                       XmNmarginHeight, 0x00,
                                       XmNradioBehavior, 0x01,
                                       XmNshadowThickness, 0x00,
                                       XmNhighlightThickness, 0x00,
                                       NULL );

    addModeBarButton ( bar, gui, MODE_VIEWOBJECT, objectmode_xpm, g3dui_setMode );
    addModeBarButton ( bar, gui, MODE_VIEWVERTEX, vertexmode_xpm, g3dui_setMode );
    addModeBarButton ( bar, gui, MODE_VIEWFACE  , facemode_xpm  , g3dui_setMode );
    addModeBarButton ( bar, gui, MODE_VIEWEDGE  , edgemode_xpm  , g3dui_setMode );
    addModeBarButton ( bar, gui, MODE_VIEWSKIN  , skinmode_xpm  , g3dui_setMode );
    addModeBarButton ( bar, gui, MODE_VIEWUVWMAP, uvwmapmode_xpm, g3dui_setMode );

    XtManageChild ( bar );

    return bar;
}
