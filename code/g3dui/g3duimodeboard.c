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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2014         */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/******************************************************************************/
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

/******************************************************************************/
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

/******************************************************************************/
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

/******************************************************************************/
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/PushB.h>
#include <Xm/Manager.h>
#include <Xm/ManagerP.h>
#include <Xm/PrimitiveP.h>

/******************************************************************************/
#include <X11/xpm.h>

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/GLwMDrawA.h>

/******************************************************************************/
#include <list.h>
#include <g3d.h>
#include <r3d.h>
#include <r3dnet.h>
#include <g3durmanager.h>
#include <g3dmouse.h>
#include <g3dui.h>

/******************************************************************************/
#include <xpm/objectmode.xpm>
#include <xpm/facemode.xpm>
#include <xpm/vertexmode.xpm>
#include <xpm/edgemode.xpm>
#include <xpm/skinmode.xpm>
#include <xpm/uvwmapmode.xpm>

/******************************************************************************/
static void setMode ( G3DUI *gui, uint32_t newmode ) {
    /*** Discard mode flags ***/
    uint32_t oldmode  = gui->flags & MODEMASK;
    uint32_t curflags = gui->flags & (~MODEMASK);
    G3DSCENE *sce  = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    static float defaultDiffuse[] = { 0.8, 0.8, 0.8, 1.0 },
                 defaultAmbient[] = { 0.2, 0.2, 0.2, 1.0 };

    /*** Restore default behavior ***/
    /*glDisable ( GL_COLOR_MATERIAL );*/

    /*** restore default values ***/
/*
    glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, (GLfloat *) &defaultDiffuse );
    glMaterialfv ( GL_FRONT_AND_BACK, GL_AMBIENT, (GLfloat *) &defaultAmbient );
*/

    gui->flags = ( curflags | newmode );

    /*** Ths is usefull in paintmode for example, to redraw the object in ***/
    /*** the paintmode color (red) ***/

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        /*** updating the mesh might take some time, let's warn the user ***/
        g3dui_setHourGlass ( gui );

        if ( ( newmode & VIEWSKIN ) || ( oldmode & VIEWSKIN ) ) {
            g3dmesh_update ( mes, NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  COMPUTESUBDIVISION, gui->flags );
        }

        /*** The below restores the face ***/
        /*** color when we switch modes ***/
        if ( ( newmode & VIEWFACE ) || ( oldmode & VIEWFACE ) ) {
            if ( mes->lselfac ) {
                g3dmesh_update ( mes, NULL,
                                      NULL,
                                      NULL,
                                  /*** there is no need to rebuild the     ***/
                                  /*** whole mesh, only the selected faces ***/
                                      mes->lselfac,
                                      COMPUTESUBDIVISION, gui->flags );
            }
        }

        g3dui_unsetHourGlass ( gui );
    }

    redrawGLViews ( gui );
}

/******************************************************************************/
void g3dui_setObjectMode ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    /*XtUnmapWidget ( gui->vertool );
    XtUnmapWidget ( gui->factool );

    XtMapWidget   ( gui->objtool );*/

    setMode ( gui, VIEWOBJECT );
}

/******************************************************************************/
void g3dui_setEdgeMode ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    /*XtUnmapWidget ( gui->objtool );
    XtUnmapWidget ( gui->factool );

    XtMapWidget   ( gui->vertool );*/

    setMode ( gui, VIEWEDGE );
}

/******************************************************************************/
void g3dui_setVertexMode ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    /*XtUnmapWidget ( gui->objtool );
    XtUnmapWidget ( gui->factool );

    XtMapWidget   ( gui->vertool );*/

    setMode ( gui, VIEWVERTEX );
}

/******************************************************************************/
void g3dui_setFaceMode ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    /*XtUnmapWidget ( gui->objtool );
    XtUnmapWidget ( gui->vertool );

    XtMapWidget   ( gui->factool );*/

    setMode ( gui, VIEWFACE );
}

/******************************************************************************/
void g3dui_setSkinMode ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    /*XtUnmapWidget ( gui->objtool );
    XtUnmapWidget ( gui->vertool );

    XtMapWidget   ( gui->factool );*/

    setMode ( gui, VIEWSKIN );

    glEnable ( GL_COLOR_MATERIAL );
}

/******************************************************************************/
void g3dui_setUVWMapMode ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    /*XtUnmapWidget ( gui->objtool );
    XtUnmapWidget ( gui->vertool );

    XtMapWidget   ( gui->factool );*/

    setMode ( gui, VIEWUVWMAP );
}

/******************************************************************************/
static void addModeBarButton ( Widget parent, G3DUI *gui, char **data,
                               void ( *armfunc ) ( Widget, XtPointer,
                                                           XtPointer ),
                               void *armdata ) {
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

    btn = XmVaCreateToggleButton ( parent, "MODEBARBUTTON",
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

                                   XmNuserData, gui,
                                   NULL );

    XtAddCallback ( btn, XmNarmCallback, armfunc, armdata );

    XtManageChild ( btn );
}

/******************************************************************************/
Widget createModeBar ( Widget parent, G3DUI *gui, char *name,
                       Dimension x    , Dimension y,
                       Dimension width, Dimension height ) {
    Pixel background, foreground;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    Widget w = XmVaCreateRowColumn ( parent, name,
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

                                     XmNuserData, gui,
                                     NULL );

    addModeBarButton ( w, gui, objectmode_xpm, g3dui_setObjectMode, gui );
    addModeBarButton ( w, gui, vertexmode_xpm, g3dui_setVertexMode, gui );
    addModeBarButton ( w, gui, facemode_xpm  , g3dui_setFaceMode  , gui );
    addModeBarButton ( w, gui, edgemode_xpm  , g3dui_setEdgeMode  , gui );
    addModeBarButton ( w, gui, skinmode_xpm  , g3dui_setSkinMode  , gui );
    addModeBarButton ( w, gui, uvwmapmode_xpm, g3dui_setUVWMapMode, gui );

    XtManageChild ( w );

    return w;
}
