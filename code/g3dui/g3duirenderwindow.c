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
#include <Xm/CascadeB.h>
#include <Xm/DrawingA.h>

/******************************************************************************/
#include <X11/xpm.h>

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/GLwMDrawA.h>

/******************************************************************************/
/***************************** SAX XML Library ********************************/
#include <expat.h>

/******************************************************************************/
#include <list.h>
#include <g3d.h>
#include <r3d.h>
#include <r3dnet.h>
#include <g3durmanager.h>
#include <g3dexport.h>
#include <g3dimport.h>
#include <g3dmouse.h>

/******************************************************************************/
#include <TabbedBook.h>
#include <g3dui.h>

/******************************************************************************/
Widget createRenderWindowMenuBar ( Widget parent, G3DUI *gui, char *name,
                                   Dimension x    , Dimension y,
                                   Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget wfile, cfile;
    Widget wjpg, wpng;
    Widget menubar;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    menubar = XmVaCreateSimpleMenuBar ( parent, name,
                                        XmNfontList, gui->fontlist,
                                        XmNx, x,
                                        XmNy, y,
                                        XmNwidth, width,
                                        XmNheight, height,
                                        XmNhighlightThickness, 0x01,
                                        XmNshadowThickness, 0x01,
                                        XmNbackground, background,
                                        XmNforeground, foreground,
XmNmarginWidth, 0x00,
XmNmarginHeight, 0x00,
XmNresizeWidth, False,
XmNresizeHeight, False,
                                        XmNtopAttachment  , XmATTACH_FORM,
                                        XmNleftAttachment , XmATTACH_FORM,
                                        XmNrightAttachment, XmATTACH_FORM,

                                        NULL );

    wfile = XmCreatePulldownMenu ( menubar, "FilePullDown", NULL, 0x00 );

    XtVaSetValues ( wfile, XmNhighlightThickness, 0x00,
                           XmNshadowThickness   , 0x01, NULL );

    cfile = XmVaCreateCascadeButton ( menubar, "File",
                                      XmNx, 0x00,
                                      XmNy, 0x00,
                                      XmNwidth, 0x60,
                                      XmNheight, height,
                                      XmNrecomputeSize, False,
                                      XmNfontList, gui->fontlist,
                                      XmNhighlightThickness, 0x01,
                                      XmNshadowThickness, 0x01,
                                      XmNbackground, background,
                                      XmNforeground, 0x00,
                                      XmNsubMenuId, wfile,
                                      NULL );

    /************************************************/
    wjpg = XmVaCreatePushButton ( wfile, "Save as JPEG",
                                        XmNwidth, 0x60,
                                        XmNrecomputeSize, False,
                                        XmNfontList, gui->fontlist,
                                        XmNhighlightThickness, 0x00,
                                        XmNshadowThickness, 0x00,
                                        XmNbackground, background,
                                        XmNforeground, 0x00,
                                        NULL );
    /*XtAddCallback ( wjpg, XmNarmCallback, newscenecbk, gui );*/
    XtManageChild ( wjpg );

    /************************************************/
    wpng = XmVaCreatePushButton ( wfile, "Save as PNG",
                                        XmNwidth, 0x60,
                                        XmNrecomputeSize, False,
                                        XmNfontList, gui->fontlist,
                                        XmNhighlightThickness, 0x00,
                                        XmNshadowThickness, 0x00,
                                        XmNbackground, background,
                                        XmNforeground, 0x00,
                                        NULL );
    /*XtAddCallback ( wjpg, XmNarmCallback, newscenecbk, gui );*/
    XtManageChild ( wpng );


    XtManageChild ( cfile );

    XtManageChild ( menubar );


    return menubar;
}

/******************************************************************************/
Widget g3duiRenderWindow_getWorkArea ( Widget mwd ) {
    WidgetList *children;
    Cardinal nc;
    int i;

    XtVaGetValues ( mwd, XmNchildren, &children, XmNnumChildren, &nc, NULL );

    for ( i = 0x00; i < nc; i++ ) {
        char *name = XtName ( ( Widget ) children[i] );

        if ( strcmp ( name, RENDERWINDOWMENUWORKAREANAME ) == 0x00 ) {

            return ( Widget ) children[i];
        }
    }

    return NULL;
}

/******************************************************************************/
Widget createRenderWindow ( Widget parent, G3DUI *gui, char *name,
                            Dimension x    , Dimension y,
                            Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget mwd, wrk, mnu;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );



    mwd = XmVaCreateForm ( parent, name,
                                         XtNx, x,
                                         XtNy, y,
                                         XtNwidth , width,
                                         XtNheight, height,
                                         XmNforeground, foreground,
                                         XmNbackground, background,
                                         XmNborderWidth, 0x00,
                                         XmNuserData, gui,
                                         NULL );

    mnu = createRenderWindowMenuBar ( mwd, gui, RENDERWINDOWMENUBARNAME,
                                      0x00, 0x00, width, 0x18 );

    wrk = XmVaCreateDrawingArea ( mwd, RENDERWINDOWMENUWORKAREANAME,
                                       XtNx, 0x00,
                                       XtNy, 0x18,
                                       XtNwidth , width,
                                       XtNheight, height,
                                       XmNforeground, foreground,
                                       XmNbackground, background,
                                       XmNborderWidth, 0x01,

                                       XmNtopAttachment   , XmATTACH_WIDGET,
                                       XmNtopWidget       , mnu,
                                       XmNleftAttachment  , XmATTACH_FORM,
                                       XmNrightAttachment , XmATTACH_FORM,

                                       NULL );
    XtManageChild ( wrk );


    return mwd;
}
