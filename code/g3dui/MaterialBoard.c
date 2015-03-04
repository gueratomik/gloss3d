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
#include <math.h>

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
#include <Xm/Scale.h>
#include <Xm/Text.h>
#include <Xm/VaSimpleP.h>
#include <Xm/ScrolledW.h>
#include <Xm/CascadeB.h>

/******************************************************************************/
#include <list.h>

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/GLwMDrawA.h>

/******************************************************************************/
#include <g3d.h>
#include <r3d.h>
#include <r3dnet.h>
#include <g3durmanager.h>

/******************************************************************************/
#include <g3dmouse.h>

/******************************************************************************/
#include <g3dui.h>

/******************************************************************************/
#include <MaterialEdit.h>
#include <TabbedBook.h>

/******************************************************************************/
void updateeditcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmMaterialListCallbackStruct *mls = ( XmMaterialListCallbackStruct * ) call;
    Widget edit = ( Widget ) client;

    updateMaterialSettings ( edit, mls->mat );
}

/******************************************************************************/
void selmatcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmMaterialListCallbackStruct *mls = ( XmMaterialListCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;

    gui->selmat = mls->mat;

    updateAllMaterialSettings ( gui );
}

/******************************************************************************/
static void addMenuButton ( Widget menu, char *name,
                            XtCallbackProc cbkfunc, XtPointer cbkdata ) {
    Pixel background, foreground;
    G3DUI *gui;

    XtVaGetValues ( menu, XmNbackground, &background,
                          XmNforeground, &foreground,
                          XmNuserData, &gui,
                          NULL );

    Widget btn = XmVaCreatePushButton ( menu, name,
                                              XmNheight, 0x18,
                                              XmNwidth, 0x60,
                                              XmNrecomputeSize, False,
                                              XmNfontList, gui->fontlist,
                                              XmNhighlightThickness, 0x00,
                                              XmNshadowThickness, 0x00,
                                              XmNbackground, background,
                                              XmNforeground, 0x00,
                                              NULL );

    XtAddCallback ( btn, XmNactivateCallback, cbkfunc, cbkdata );


    XtManageChild ( btn );
}

/******************************************************************************/
Widget createMaterialListMenu ( Widget parent, G3DUI *gui, char *name ) {
    Pixel background, foreground;
    Widget wfile, cfile;
    Widget wedit, cedit;
    Widget menubar;
    Widget wnew;
    Widget matlst = g3duiMaterialBoard_getListWidget ( parent );

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    menubar = XmVaCreateSimpleMenuBar ( parent, name,
                                        XmNfontList, gui->fontlist,
                                        XmNheight, 0x18,
                                        XmNhighlightThickness, 0x01,
                                        XmNshadowThickness, 0x01,
                                        XmNbackground, background,
                                        XmNforeground, foreground,

                                        XmNtopAttachment  , XmATTACH_FORM,
                                        XmNleftAttachment , XmATTACH_FORM,
                                        XmNrightAttachment, XmATTACH_FORM,

                                        NULL );

    wfile = XmCreatePulldownMenu ( menubar, "FilePullDown", NULL, 0x00 );

    XtVaSetValues ( wfile, XmNhighlightThickness, 0x00,
                           XmNuserData, gui,
                           XmNshadowThickness, 0x01, NULL );

    cfile = XmVaCreateCascadeButton ( menubar, "File",
                                      XmNwidth, 0x60,
                                      XmNrecomputeSize, False,
                                      XmNfontList, gui->fontlist,
                                      XmNhighlightThickness, 0x01,
                                      XmNshadowThickness, 0x01,
                                      XmNbackground, background,
                                      XmNforeground, 0x00,
                                      XmNsubMenuId, wfile,
                                      NULL );

    /************************************************/
    addMenuButton ( wfile, "Add Material"   , addmaterialcbk, gui );
    addMenuButton ( wfile, "Delete Material", delmaterialcbk, gui );
    addMenuButton ( wfile, "Set Material"   , setmaterialcbk, gui );

    addMenuButton ( wfile, "Small Icons"    , smallpreviewscbk , matlst );
    addMenuButton ( wfile, "Normal Icons"   , normalpreviewscbk, matlst );
    addMenuButton ( wfile, "Big Icons"      , bigpreviewscbk   , matlst );

    XtManageChild ( cfile );

    XtManageChild ( menubar );


    return menubar;
}

/******************************************************************************/
Widget g3duiMaterialBoard_getListWidget ( Widget w ) {
    WidgetList *ichildren;
    Cardinal inc;
    int i, j;

    XtVaGetValues ( w, XmNchildren, &ichildren,
                       XmNnumChildren, &inc,
                       NULL );

    for ( i = 0x00; i < inc; i++ ) {
        Widget ichild = ( Widget ) ichildren[i];

        if ( XtClass ( ichild ) == xmScrolledWindowWidgetClass ) {
            WidgetList *jchildren;
            Cardinal jnc;
            Widget wrk;

            XtVaGetValues ( ichild, XmNworkWindow, &wrk,
                                    NULL );

            if ( XtClass ( wrk ) == xmMaterialListWidgetClass ) {

                return wrk;
            }
        }
    }

    return NULL;
}

/******************************************************************************/
Widget createMaterialBoard ( Widget parent, G3DUI *gui, char *name,
                                            Dimension x    , Dimension y,
                                            Dimension width, Dimension height ) {

    Widget brd = XmVaCreateForm ( parent, name, XmNx, x,
                                                XmNy, y,
                                                XmNwidth, width,
                                                XmNheight, height,
                                                XmNuserData, gui,
                                                NULL );
    Pixel background, foreground;
    Widget scr, matlist, mnu, tab;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    scr = XmVaCreateManagedScrolledWindow ( brd, "Scrolled Window",
                                            XmNx, 0x00,
                                            XmNy, 0x20,
                                            XmNwidth, width,
                                            XmNheight, 0x120,
                                            XmNscrollingPolicy, XmAUTOMATIC,
                                            XmNspacing, 0x00,
                                            XmNforeground, foreground,
                                            XmNbackground, background,
                                            XmNtopShadowColor, background,
                                            XmNbottomShadowColor, background,
                                            XmNshadowThickness, 0x01,
                                            XmNuserData, gui,
                                            NULL );

    matlist = XmVaCreateManagedMaterialList ( scr, "Materials",
                                              XmNx, 0x00,
                                              XmNy, 0x00,
                                              XmNwidth, 0x128,
                                              XmNheight, 0x00,
                                              XmNuserData, gui,
                                              NULL );

    XtAddCallback ( matlist, XmNmaterialSelectedCallback, selmatcbk    , gui  );

    list_insert ( &gui->lmatlist, matlist );

    /*** Menu must be created after the MaterialList because it will also ***/
    /*** use the MaterialList it as an argument for its callbacks. ***/
    mnu = createMaterialListMenu ( brd, gui, "MENU" );

    /**** BOTTOM TABs ****/
    tab =  XmVaCreateManagedTabbedBook ( brd, "Tabs",
                                              XmNx, 0x00,
                                              XmNy, 0x140,
                                              XmNwidth, 0x140,
                                              XmNheight, 0x140,
                                              XmNfontList, gui->fontlist,
                                              XmNuserData, gui,
                                              NULL );

    createMaterialEdit ( tab, gui, "Material", 0x00, 0x00, 0x140, 0x90 );

    XtManageChild ( brd );


    return brd;
}
