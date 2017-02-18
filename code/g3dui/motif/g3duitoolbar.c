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
#include <xpm/render.xpm>
#include <xpm/newfile.xpm>
#include <xpm/openfile.xpm>
#include <xpm/undo.xpm>
#include <xpm/redo.xpm>
#include <xpm/save.xpm>
#include <xpm/saveas.xpm>
#include <xpm/stoprender.xpm>
#include <xpm/renderw.xpm>
#include <xpm/makeedit.xpm>
#include <xpm/pick.xpm>
#include <xpm/move.xpm>
#include <xpm/scale.xpm>
#include <xpm/rotate.xpm>
#include <xpm/delete.xpm>
#include <xpm/xaxis.xpm>
#include <xpm/yaxis.xpm>
#include <xpm/zaxis.xpm>

/******************************************************************************/
void newscenecancelcbk ( Widget w, XtPointer client, XtPointer call ) {
    XtUnmanageChild ( w );
}

/******************************************************************************/
void newsceneokcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmAnyCallbackStruct *abs = ( XmAnyCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;

    gui->sce = g3dscene_new ( 0x00, "SCENE" );

    XtUnmanageChild ( w );
}

/******************************************************************************/
void g3dui_newSceneCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Widget dialog;
    char buf[0x100];
    XmString str;

    snprintf ( buf, 0x100, "Close scene and create a new one ?" );
    str = XmStringCreateLocalized ( buf );

    dialog = XmCreateMessageDialog ( gmt->top, "Dialog", 
                                                NULL, 0x00 );

    XtVaSetValues ( dialog, XmNmessageString, str, NULL );

    XtAddCallback ( dialog, XmNokCallback    , newsceneokcbk    , gui );
    XtAddCallback ( dialog, XmNcancelCallback, newscenecancelcbk, gui );

    XtManageChild ( dialog );

    XtPopup ( XtParent ( dialog ), XtGrabNone );
}

/******************************************************************************/
void g3dui_undoCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_undoCbk ( gui );
}

/******************************************************************************/
void g3dui_redoCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_redoCbk ( gui );
}

/******************************************************************************/
void overwritefileokcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmAnyCallbackStruct *abs = ( XmAnyCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    char *filename = NULL;
    XmString str;

    XtVaGetValues ( w, XmNmessageString, &str,
                       XmNuserData, &filename, NULL );

    printf ( "Saving as %s ...\n", filename );

    common_g3dui_setFileName ( gui, filename );

    common_g3dui_saveG3DFile ( gui );

    XtUnmanageChild ( w );

    XmStringFree ( str );
    /*free ( filename );*/
}

/******************************************************************************/
void overwritefilecancelcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmAnyCallbackStruct *abs = ( XmAnyCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    char *filename = NULL;
    XmString str;

    XtVaGetValues ( w, XmNmessageString, &str,
                       XmNuserData, &filename, NULL );

    XtUnmanageChild ( w );

    XmStringFree ( str );
    /*free ( filename );*/
}

/******************************************************************************/
void savefileascancelcbk ( Widget w, XtPointer client, XtPointer call ) {
    XtUnmanageChild ( w );
}

/******************************************************************************/
void savefileasokcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmFileSelectionBoxCallbackStruct *cbs =
                        ( XmFileSelectionBoxCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    char *filename = NULL;
    static char filenameext[0x100] = { 0x00 };
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    filename = ( char * ) XmStringUnparse ( cbs->value,
                                            XmFONTLIST_DEFAULT_TAG,
                                            XmCHARSET_TEXT,
                                            XmCHARSET_TEXT,
                                            NULL, 0x00, XmOUTPUT_ALL );

    if ( strstr ( filename, ".g3d" ) == NULL ) {
        snprintf ( filenameext, 0x100, "%s.g3d", filename );
    } else {
        snprintf ( filenameext, 0x100, "%s", filename );
    }

    if ( access ( filenameext, F_OK ) == 0x00 ) {
        char buf[0x100];
        XmString str;
        Widget mdw;

        snprintf ( buf, 0x100, "Overwrite %s ?", filenameext );
        str = XmStringCreateLocalized ( buf );

        mdw = XmCreateMessageDialog ( gmt->top, "Confirm", NULL, 0x00 );

        XtVaSetValues ( mdw, XmNuserData, filenameext,
                             XmNmessageString, str,
                             NULL );

        XtAddCallback ( mdw, XmNokCallback    , overwritefileokcbk    , gui );
        XtAddCallback ( mdw, XmNcancelCallback, overwritefilecancelcbk, gui );

        XtManageChild ( mdw );
        XtPopup ( XtParent ( mdw ), XtGrabNone);

        /*free ( str );*/
    } else {
        common_g3dui_setFileName ( gui, filenameext );

        printf ( "saving as:%s\n", gui->filename );

        common_g3dui_saveG3DFile ( gui );
    }

    free ( filename );

    XtUnmanageChild ( w );
}

/******************************************************************************/
void g3dui_saveAsCbk ( Widget w, XtPointer client, XtPointer call ) {
    Widget dialog;
    G3DUI *gui = ( G3DUI * ) client;

    dialog = XmCreateFileSelectionDialog ( w, "Save File", 
                                           NULL, 0x00 );

    XtAddCallback ( dialog, XmNokCallback    , savefileasokcbk    , gui );
    XtAddCallback ( dialog, XmNcancelCallback, savefileascancelcbk, gui );

    XtManageChild ( dialog );
}

/******************************************************************************/
void g3dui_saveFileCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    if ( gui->filename ) {
        printf ( "saving as:%s\n", gui->filename );

        common_g3dui_saveG3DFile ( gui );
    } else {
        g3dui_saveAsCbk ( w, client, call );
    }
}

/******************************************************************************/
void openfileokcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmFileSelectionBoxCallbackStruct *cbs =
                        ( XmFileSelectionBoxCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = NULL;
    char *filename;

    filename = ( char * ) XmStringUnparse ( cbs->value,
                                            XmFONTLIST_DEFAULT_TAG,
                                            XmCHARSET_TEXT,
                                            XmCHARSET_TEXT,
                                            NULL, 0x00, XmOUTPUT_ALL );

    gui->sce = common_g3dui_openG3DFile ( gui, filename );

    XtFree ( filename );

    XtUnmanageChild ( w );

    g3dui_redrawGLViews ( gui );
    g3dui_updateCoords ( gui );
    g3dui_redrawObjectList ( gui );
    g3dui_updateAllCurrentEdit ( gui );
}

/******************************************************************************/
void openfilecancelcbk ( Widget w, XtPointer client, XtPointer call ) {
    XtUnmanageChild ( w );
}

/******************************************************************************/
void renderdestroycbk  ( Widget w, XtPointer client, XtPointer call ) {
    G3DUIRENDERPROCESS *rps = NULL;

    XtVaGetValues ( w, XmNuserData, &rps, NULL );

    if ( rps ) {
        /*g3duirenderprocess_free ( rps );

        list_remove ( &gui->lrps, rps );*/
    }
}

/******************************************************************************/
void renderexposecbk  ( Widget w, XtPointer client, XtPointer call ) {
    G3DUIRENDERPROCESS *rps = NULL;
    Dimension width, height;

    XtVaGetValues ( w, XmNuserData, &rps, 
                       XmNwidth   , &width,
                       XmNheight  , &height, NULL );

    if ( rps ) {
        /*R3DFILTER *fil = rps->filter_to_window;
        FILTERTOWINDOW *ftw = ( FILTERTOWINDOW * ) fil->data;

        XShmPutImage ( ftw->dis, ftw->win, ftw->gc, ftw->ximg,
                       0x00, 0x00,
                       0x00, 0x00,
                       width, height, False );*/
    }
}

/******************************************************************************/
void overwritejpgokcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmAnyCallbackStruct *abs = ( XmAnyCallbackStruct * ) call;
    Widget area = ( Widget ) client;
    G3DUIRENDERPROCESS *rps;
    char *filename;

    /*** retrieve the stored raw pixel values ***/
    XtVaGetValues ( area, XmNuserData, &rps, NULL );

    if ( rps ) {
        g3duirenderprocess_savejpg ( rps, rps->filename );
    }

    XtUnmanageChild ( w );
}

/******************************************************************************/
void overwritejpgcancelcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmAnyCallbackStruct *abs = ( XmAnyCallbackStruct * ) call;

    XtUnmanageChild ( w );
}

/******************************************************************************/
void savejpgokcbk  ( Widget w, XtPointer client, XtPointer call ) {
    XmFileSelectionBoxCallbackStruct *cbs =
                        ( XmFileSelectionBoxCallbackStruct * ) call;
    Widget area = ( Widget ) client;
    G3DUIRENDERPROCESS *rps;
    char *filename;
    G3DUI *gui;
    G3DUIMOTIF *gmt;

    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    /*** retrieve the stored raw pixel values ***/
    XtVaGetValues ( area, XmNuserData, &rps, NULL );

    filename = ( char * ) XmStringUnparse ( cbs->value,
                                            XmFONTLIST_DEFAULT_TAG,
                                            XmCHARSET_TEXT,
                                            XmCHARSET_TEXT,
                                            NULL, 0x00, XmOUTPUT_ALL );

    g3duirenderprocess_filename ( rps, filename );

    if ( filename && access ( filename, F_OK ) == 0x00 ) {
        char buf[0x100];
        XmString str;
        Widget mdw;

        snprintf ( buf, 0x100, "Overwrite %s ?", basename ( filename ) );
        str = XmStringCreateLocalized ( buf );

        mdw = XmCreateMessageDialog ( gmt->top, "Confirm", NULL, 0x00 );

        XtVaSetValues ( mdw, XmNmessageString, str, NULL );

        XtAddCallback ( mdw, XmNokCallback    , overwritejpgokcbk    , area );
        XtAddCallback ( mdw, XmNcancelCallback, overwritejpgcancelcbk, area );

        XtManageChild ( mdw );
        XtPopup ( XtParent ( mdw ), XtGrabNone );

        /*free ( str );*/
    } else {
        if ( rps ) {
            g3duirenderprocess_savejpg ( rps, rps->filename );
        }
    }

    XtFree ( filename );

    XtUnmanageChild ( w );
}

/******************************************************************************/
void savejpgcancelcbk  ( Widget w, XtPointer client, XtPointer call ) {
    XtUnmanageChild ( w );
}

/******************************************************************************/
void savejpgcbk  ( Widget w, XtPointer client, XtPointer call ) {
    Widget dialog;
    G3DUI *gui;
    /*** In this function we need to retrieve the DrawingArea because its ***/
    /*** userData field contains the G3DUIRENDERPROCESS structure, where our***/
    /*** raw image data are stored. If we want to save as jpg, we need it ***/
    Widget area = ( Widget ) client;

    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    dialog = XmCreateFileSelectionDialog ( w, "Save as JPEG", 
                                           NULL, 0x00 );

    XtVaSetValues ( dialog, XmNuserData, gui, NULL );

    XtAddCallback ( dialog, XmNokCallback    , savejpgokcbk    , area );
    XtAddCallback ( dialog, XmNcancelCallback, savejpgcancelcbk, NULL );

    XtManageChild ( dialog );
}

/******************************************************************************/
void g3dui_deleteSelectionCbk ( Widget w, XtPointer client,
                                          XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_deleteSelectionCbk ( gui );
}
/******************************************************************************/
/******************************************************************************/


/******************************************************************************/
void g3dui_openFileCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    Widget dialog;

    dialog = XmCreateFileSelectionDialog ( w, "Open File", 
                                           NULL, 0x00 );

    XtAddCallback ( dialog, XmNokCallback    , openfileokcbk    , gui );
    XtAddCallback ( dialog, XmNcancelCallback, openfilecancelcbk, gui );

    XtManageChild ( dialog );
}

/******************************************************************************/
void g3dui_makeEditableCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    g3dui_setHourGlass ( gui );

    common_g3dui_makeEditableCbk ( gui );

    g3dui_unsetHourGlass ( gui );
}

/******************************************************************************/
/*static void armXAxiscbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    gui->flags |= XAXIS;
}*/

/******************************************************************************/
static void disarmXAxiscbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    Boolean status;

    XtVaGetValues ( w, XmNset, &status, NULL );

    if ( status ) {
        gui->flags |= XAXIS;
    } else {
        gui->flags &= (~XAXIS);
    }
}

/******************************************************************************/
static void disarmYAxiscbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    Boolean status;

    XtVaGetValues ( w, XmNset, &status, NULL );

    if ( status ) {
        gui->flags |= YAXIS;
    } else {
        gui->flags &= (~YAXIS);
    }
}

/******************************************************************************/
static void disarmZAxiscbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    Boolean status;

    XtVaGetValues ( w, XmNset, &status, NULL );

    if ( status ) {
        gui->flags |= ZAXIS;
    } else {
        gui->flags &= (~ZAXIS);
    }
}

/******************************************************************************/
static void addToolbarAxis ( Widget parent, G3DUI *gui ) {
    Pixel background, foreground;
    int depth;

    XtVaGetValues ( parent, XmNdepth, &depth,
                            XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    Widget axis = XmVaCreateRowColumn ( parent, "G3DAXIS",
                                        /*** arguments ***/
                                        /*** There are 3 Axis, so ... ***/
                                        XmNwidth, ( ( TOOLBARBUTTONSIZE ) * 0x03 ),
                                        XmNheight, TOOLBARBUTTONSIZE,
                                        XmNorientation, XmHORIZONTAL,
                                        XmNspacing, 0x00,
                                        XmNpacking, 0x00,
                                        XmNmarginWidth, 0x00,
                                        XmNmarginHeight, 0x00,
                                        XmNbackground, background,
                                        XmNforeground, foreground,
                                        XmNborderWidth,0x00,
                                        NULL );
    Pixmap pixnor, pixarm;
    Widget btn;

    /********************************/
    createTransparentPixmap ( XtDisplay ( parent ), &pixnor,
                                                    &pixarm,
                                                    xaxis_xpm,
                                                    depth,
                                                    background,
                                                    foreground );

    btn = XmVaCreateToggleButton ( axis, "G3DXAXIS",
                                   /*** arguments ***/
                                   XmNwidth,  TOOLBARBUTTONSIZE,
                                   XmNheight, TOOLBARBUTTONSIZE,
                                   XmNindicatorOn, False,
                                   XmNrecomputeSize, False,
                                   XmNlabelType, XmPIXMAP,
                                   XmNlabelPixmap , pixnor,
                                   XmNselectPixmap, pixarm,
/*XmNalignment, XmALIGNMENT_CENTER,*/
                                   XmNhighlightThickness, 0x00,
                                   XmNshadowThickness, 0x01,
                                   XmNmarginHeight, 0x00,
                                   XmNmarginWidth, 0x00,
                                   XmNbackground, background,
                                   XmNforeground, foreground,
                                   XmNset, True,
                                   NULL );

    /*XtAddCallback ( btn, XmNarmCallback   , armXAxiscbk   , gui );*/
    XtAddCallback ( btn, XmNdisarmCallback, disarmXAxiscbk, gui );
    XtManageChild ( btn );

    /********************************/
    createTransparentPixmap (  XtDisplay ( parent ), &pixnor,
                                                     &pixarm,
                                                     yaxis_xpm,
                                                     depth,
                                                     background,
                                                     foreground );

    btn = XmVaCreateToggleButton ( axis, "G3DYAXIS",
                                   /*** arguments ***/
                                   XmNwidth,  TOOLBARBUTTONSIZE,
                                   XmNheight, TOOLBARBUTTONSIZE,
                                   XmNindicatorOn, False,
                                   XmNrecomputeSize, False,
                                   XmNlabelType, XmPIXMAP,
                                   XmNlabelPixmap , pixnor,
                                   XmNselectPixmap, pixarm,
/*XmNalignment, XmALIGNMENT_CENTER,*/
                                   XmNhighlightThickness, 0x00,
                                   XmNshadowThickness, 0x01,
                                   XmNmarginHeight, 0x00,
                                   XmNmarginWidth, 0x00,
                                   XmNbackground, background,
                                   XmNforeground, foreground,
                                   XmNset, True,
                                   NULL );

    /*XtAddCallback ( btn, XmNarmCallback   , armYAxiscbk   , gui );*/
    XtAddCallback ( btn, XmNdisarmCallback, disarmYAxiscbk, gui );
    XtManageChild ( btn );

    /********************************/
    createTransparentPixmap (  XtDisplay ( parent ), &pixnor,
                                                     &pixarm,
                                                     zaxis_xpm,
                                                     depth,
                                                     background,
                                                     foreground );

    btn = XmVaCreateToggleButton ( axis, "G3DZAXIS",
                                   /*** arguments ***/
                                   XmNwidth,  TOOLBARBUTTONSIZE,
                                   XmNheight, TOOLBARBUTTONSIZE,
                                   XmNindicatorOn, False,
                                   XmNrecomputeSize, False,
                                   XmNlabelType, XmPIXMAP,
                                   XmNlabelPixmap , pixnor,
                                   XmNselectPixmap, pixarm,
/*XmNalignment, XmALIGNMENT_CENTER,*/
                                   XmNhighlightThickness, 0x00,
                                   XmNshadowThickness, 0x01,
                                   XmNmarginHeight, 0x00,
                                   XmNmarginWidth, 0x00,
                                   XmNbackground, background,
                                   XmNforeground, foreground,
                                   XmNset, True,
                                   NULL );

    /*XtAddCallback ( btn, XmNarmCallback   , armZAxiscbk   , gui );*/
    XtAddCallback ( btn, XmNdisarmCallback, disarmZAxiscbk, gui );
    XtManageChild ( btn );

    XtManageChild ( axis );
}

/******************************************************************************/
void g3dui_addToolBarPushButton ( Widget parent, G3DUI *gui, char **data,
                                  void ( *cbk ) ( Widget, XtPointer,
                                                          XtPointer ) ) {
    Pixel background, foreground;
    Pixmap pixnor, pixarm;
    Widget btn;
    int depth;

    XtVaGetValues ( parent, XmNdepth, &depth,
                            XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    createTransparentPixmap (  XtDisplay ( parent ), &pixnor,
                                                     &pixarm,
                                                     data,
                                                     depth,
                                                     background,
                                                     foreground );

    btn = XmVaCreatePushButton ( parent, "BUTTON",
                                 /*** arguments ***/
                                 /*XmNx, 0x00,
                                 XmNy, 0x00,*/
                                 XmNwidth,  TOOLBARBUTTONSIZE,
                                 XmNheight, TOOLBARBUTTONSIZE,
                                 XmNrecomputeSize, False,
                                 XmNlabelType, XmPIXMAP,
/*XmNalignment, XmALIGNMENT_CENTER,*/
                                 XmNhighlightThickness, 0x00,
                                 XmNshadowThickness, 0x01,
                                 XmNmarginHeight, 0x00,
                                 XmNmarginWidth, 0x00,
                                 XmNlabelPixmap , pixnor,
                                 XmNselectPixmap, pixarm,
                                 XmNbackground, background,
                                 XmNforeground, foreground,
                                 NULL );

    if ( cbk ) {
        XtAddCallback ( btn, XmNarmCallback, cbk, gui );
    }

    XtManageChild ( btn );
}

/******************************************************************************/
void g3dui_addToolBarToggleButton ( Widget parent, G3DUI *gui, char *name,
                                                    char **data,
                                    void ( *cbk ) ( Widget, XtPointer,
                                                            XtPointer ) ) {
    Pixel background, foreground;
    Pixmap pixnor, pixarm;
    Widget btn;
    int depth;

    XtVaGetValues ( parent, XmNdepth, &depth,
                            XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    createTransparentPixmap (  XtDisplay ( parent ), &pixnor,
                                                     &pixarm,
                                                     data,
                                                     depth,
                                                     background,
                                                     foreground );

    btn = XmVaCreateToggleButton ( parent, name,
                                   /*** arguments ***/
                                   /*XmNx, 0x00,
                                   XmNy, 0x00,*/
                                   XmNwidth,  TOOLBARBUTTONSIZE,
                                   XmNheight, TOOLBARBUTTONSIZE,
                                   XmNindicatorOn, False,
                                   XmNrecomputeSize, False,
                                   XmNlabelType, XmPIXMAP,
                                   XmNlabelPixmap , pixnor,
                                   XmNselectPixmap, pixarm,
/*XmNalignment, XmALIGNMENT_CENTER,*/
                                   XmNhighlightThickness, 0x00,
                                   XmNshadowThickness, 0x01,
                                   XmNmarginHeight, 0x00,
                                   XmNmarginWidth, 0x00,
                                   XmNbackground, background,
                                   XmNforeground, foreground,
                                   NULL );

    if ( cbk ) {
        XtAddCallback ( btn, XmNarmCallback, cbk, gui );
    }

    XtManageChild ( btn );
}

/******************************************************************************/
Widget createToolBar ( Widget parent, G3DUI *gui, char *name,
                       Dimension x    , Dimension y,
                       Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget toolbar;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    toolbar = XmVaCreateRowColumn ( parent, TOOLBARNAME,
                                         /*** arguments ***/
                                    XmNx, x,
                                    XmNy, y,
                                    XmNwidth, width,
                                    XmNheight, height,
                                   XmNrecomputeSize, False,
/*XmNadjustMargin, False,
XmNresizeHeight, False,
XmNresizeWidth, False,
XmNspacing, 0,*/
                                    XmNorientation, XmHORIZONTAL,
                                    XmNspacing, 0x01,
                                    XmNpacking, 0x00,
                                    XmNmarginWidth, 0x00,
                                    XmNmarginHeight, 0x00,
                                    XmNradioBehavior, 0x01,
                                    XmNbackground, background,
                                    XmNforeground, foreground,
                                    XmNborderWidth, 0x00,
                                    XmNpacking, XmPACK_TIGHT,
                                    NULL );
    G3DMOUSETOOL *mou;

    g3dui_addToolBarPushButton ( toolbar, gui, newfile_xpm  , g3dui_newSceneCbk );
    g3dui_addToolBarPushButton ( toolbar, gui, openfile_xpm , g3dui_openFileCbk );
    g3dui_addToolBarPushButton ( toolbar, gui, saveas_xpm   , g3dui_saveAsCbk   );
    g3dui_addToolBarPushButton ( toolbar, gui, save_xpm     , g3dui_saveFileCbk );
    g3dui_addToolBarPushButton ( toolbar, gui, undo_xpm     , g3dui_undoCbk     );
    g3dui_addToolBarPushButton ( toolbar, gui, redo_xpm     , g3dui_redoCbk     );
    g3dui_addToolBarPushButton ( toolbar, gui, delete_xpm   , g3dui_deleteSelectionCbk );

    /********************************/

    g3dui_addToolBarToggleButton ( toolbar, gui, PICKTOOL, pick_xpm  , g3dui_setMouseTool );

    /********************************/

    g3dui_addToolBarToggleButton ( toolbar, gui, MOVETOOL, move_xpm  , g3dui_setMouseTool );

    /********************************/

    g3dui_addToolBarToggleButton ( toolbar, gui, SCALETOOL, scale_xpm , g3dui_setMouseTool );

    /********************************/

    g3dui_addToolBarToggleButton ( toolbar, gui, ROTATETOOL, rotate_xpm, g3dui_setMouseTool );

    /********************************/

    g3dui_addToolBarPushButton ( toolbar, gui, renderw_xpm , g3dui_renderViewCbk   );
    g3dui_addToolBarPushButton ( toolbar, gui, render_xpm  , g3dui_runRenderCbk        );
    g3dui_addToolBarPushButton ( toolbar, gui, makeedit_xpm, g3dui_makeEditableCbk );

    addToolbarAxis ( toolbar, gui );


    XtManageChild ( toolbar );

    return toolbar;
}
