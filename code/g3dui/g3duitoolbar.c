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
#include <unistd.h>

/******************************************************************************/
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>

/******************************************************************************/
#include <sys/types.h>
#include <sys/wait.h>

/******************************************************************************/
#include <pthread.h>
 
/******************************************************************************/
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/cursorfont.h>

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
#include <Xm/FileSB.h>
#include <Xm/MessageB.h>
#include <Xm/ScrolledW.h>
#include <Xm/DrawingA.h>
#include <Xm/CascadeB.h>

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
#include <g3dcom.h>
#include <r3d.h>
#include <r3dnet.h>
#include <g3durmanager.h>
#include <g3dimport.h>
#include <g3dexport.h>
#include <g3dmouse.h>
#include <g3dui.h>

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
void newscenecbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    /*** todo - clear the old scene ***/

    gui->sce = g3dscene_new ( 0x00, "SCENE" );
}

/******************************************************************************/
void undocbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DURMANAGER *urm = gui->urm;
    uint32_t return_value;

    return_value = g3durmanager_undo ( urm, gui->flags );

    if ( return_value & REDRAWVIEW ) {
        redrawGLViews ( gui );
    }

    if ( return_value & REDRAWLIST ) {
        redrawObjectList ( gui );
    }

    if ( return_value & REDRAWCURRENTOBJECT ) {
        updateAllCurrentEdit ( gui );
    }
}

/******************************************************************************/
void redocbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DURMANAGER *urm = gui->urm;
    uint32_t return_value;

    return_value = g3durmanager_redo ( urm, gui->flags );

    if ( return_value & REDRAWVIEW ) {
        redrawGLViews ( gui );
    }

    if ( return_value & REDRAWLIST ) {
        redrawObjectList ( gui );
    }

    if ( return_value & REDRAWCURRENTOBJECT ) {
        updateAllCurrentEdit ( gui );
    }
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

    g3duiSetFileName ( gui, filename );

    g3duiSaveG3DFile ( gui );

    XtUnmanageChild ( w );

    XmStringFree ( str );
    free ( filename );
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
    free ( filename );
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

    filename = ( char * ) XmStringUnparse ( cbs->value,
                                            XmFONTLIST_DEFAULT_TAG,
                                            XmCHARSET_TEXT,
                                            XmCHARSET_TEXT,
                                            NULL, 0x00, XmOUTPUT_ALL );

    if ( filename && access ( filename, F_OK ) == 0x00 ) {
        char buf[0x100];
        XmString str;
        Widget mdw;

        snprintf ( buf, 0x100, "Overwrite %s ?", basename ( filename ) );
        str = XmStringCreateLocalized ( buf );

        mdw = XmCreateMessageDialog ( gui->top, "Confirm", NULL, 0x00 );

        XtVaSetValues ( mdw, XmNuserData, filename,
                             XmNmessageString, str,
                             NULL );

        XtAddCallback ( mdw, XmNokCallback    , overwritefileokcbk    , gui );
        XtAddCallback ( mdw, XmNcancelCallback, overwritefilecancelcbk, gui );

        XtManageChild ( mdw );
        XtPopup ( XtParent ( mdw ), XtGrabNone);

        /*free ( str );*/
    } else {
        g3duiSetFileName ( gui, filename );

        printf ( "saving as:%s\n", gui->filename );

        g3duiSaveG3DFile ( gui );

        free ( filename );
    }

    XtUnmanageChild ( w );
}

/******************************************************************************/
void saveascbk ( Widget w, XtPointer client, XtPointer call ) {
    Widget dialog;
    G3DUI *gui = ( G3DUI * ) client;

    dialog = XmCreateFileSelectionDialog ( w, "Save File", 
                                           NULL, 0x00 );

    XtAddCallback ( dialog, XmNokCallback    , savefileasokcbk    , gui );
    XtAddCallback ( dialog, XmNcancelCallback, savefileascancelcbk, gui );

    XtManageChild ( dialog );
}

/******************************************************************************/
void savefilecbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    if ( gui->filename ) {
        printf ( "saving as:%s\n", gui->filename );

        g3duiSaveG3DFile ( gui );
    } else {
        saveascbk ( w, client, call );
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

    gui->sce = g3duiOpenG3DFile ( gui, filename );

    XtFree ( filename );

    XtUnmanageChild ( w );

    redrawGLViews ( gui );
    updateCoords ( gui );
    redrawObjectList ( gui );
    updateAllCurrentEdit ( gui );
}

/******************************************************************************/
void openfilecancelcbk ( Widget w, XtPointer client, XtPointer call ) {
    XtUnmanageChild ( w );
}

/******************************************************************************/
/*** http://linux.die.net/man/2/pipe                                        ***/
/******************************************************************************/

/******************************************************************************/
void renderviewcbk  ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    Display *dis = XtDisplay ( gui->curogl );
    Window win   = XtWindow ( gui->curogl );
    XWindowAttributes wat;

    XGetWindowAttributes ( dis, win, &wat );

    g3dui_setHourGlass ( gui );

    g3duirendersettings_rendertoview ( gui->currsg, 
                                       gui->sce,
                                       gui->curcam,
                                       0x00, 0x00,
                                       wat.width - 1, wat.height - 1,
                                       gui->curogl );

    g3dui_unsetHourGlass ( gui );
}

/******************************************************************************/
void renderdestroycbk  ( Widget w, XtPointer client, XtPointer call ) {
    G3DUIRENDERPROCESS *rps = NULL;

    XtVaGetValues ( w, XmNuserData, &rps, NULL );

    if ( rps ) {
        pthread_cancel ( rps->tid );
        pthread_join   ( rps->tid, NULL );

        g3duirenderprocess_free ( rps );
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
        FILTERTOWINDOW *ftw = rps->ftw;

        XShmPutImage ( ftw->dis, ftw->win, ftw->gc, ftw->ximg,
                       0x00, 0x00,
                       0x00, 0x00,
                       width, height, False );
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

    r3draw_to_jpg ( rps->ftb->rawimg, rps->ftb->width,
                                      rps->ftb->height,
                                      rps->ftb->depth, rps->filename );

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

    XtVaGetValues ( w, XmNuserData, &gui, NULL );

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

        mdw = XmCreateMessageDialog ( gui->top, "Confirm", NULL, 0x00 );

        XtVaSetValues ( mdw, XmNmessageString, str, NULL );

        XtAddCallback ( mdw, XmNokCallback    , overwritejpgokcbk    , area );
        XtAddCallback ( mdw, XmNcancelCallback, overwritejpgcancelcbk, area );

        XtManageChild ( mdw );
        XtPopup ( XtParent ( mdw ), XtGrabNone );

        /*free ( str );*/
    } else {
        r3draw_to_jpg ( rps->ftb->rawimg, rps->ftb->width,
                                          rps->ftb->height,
                                          rps->ftb->depth, rps->filename );
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
void renderfinalcbk  ( Widget w, XtPointer client, XtPointer call ) {
    Display *dis = XtDisplay ( w );
    Window  root = XDefaultRootWindow ( dis );
    G3DUI *gui   = ( G3DUI * ) client;
    Pixel background, foreground;
    Widget shell, wren, area = NULL;
    G3DUIRENDERSETTINGS *rsg;
    XWindowAttributes wat;
    G3DSCENE  *sce;
    G3DCAMERA *cam;
    Widget menubar;
    Widget frm;
    Widget wfile, cfile, wbtn;

    /*** Get current render settings ***/
    rsg = gui->currsg;
    sce = gui->sce;
    cam = gui->curcam;

    float ratio = ( rsg->ratio ) ? rsg->ratio : cam->ratio;
    uint32_t rsgwidth = rsg->height * ratio;

    /*** This helps up to position the rendering ***/
    /*** window in the middle of the screen. ***/
    XGetWindowAttributes ( dis, root, &wat );

    XtVaGetValues ( w, XmNbackground, &background,
                       XmNforeground, &foreground, NULL );

    /*** Start the rendering in a new window ***/
    shell = XtVaAppCreateShell ( NULL, "Class", topLevelShellWidgetClass,
                                                dis, 
                                                XtNtitle,"Render Shell",
                                                XtNx, ( wat.width  >> 1 ) - ( rsgwidth  >> 1 ),
                                                XtNy, ( wat.height >> 1 ) - ( rsg->height >> 1 ),
                                                XtNwidth,  rsgwidth  + 0x02,
                                                XtNheight, rsg->height + 0x20,
                                                XmNforeground, foreground,
                                                XmNbackground, background,
                                                NULL );

    frm = XmVaCreateManagedForm ( shell, "Form",
                                          XmNforeground, foreground,
                                          XmNbackground, background,
                                          XmNborderWidth, 0x00,
                                          XmNuserData, gui,
                                          NULL );

    /************************************************/
    menubar = XmVaCreateSimpleMenuBar ( frm, "Menubar",
                                        XmNfontList, gui->fontlist,
                                        XmNheight, 0x20,
                                        XmNwidth, wat.width,
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
                           XmNshadowThickness   , 0x01, NULL );

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

    /*** I dont use the createMenuBarButton convenience function because ***/
    /*** I dont need to pass GUI as an argument, I need to pass the drawing ***/
    /*** area as an argument to the activateCallback in order to retrieve ***/
    /*** the raw image data ***/
    wbtn = XmVaCreatePushButton ( wfile, "Save as JPG", 
                                          XmNwidth, 0x60,
                                          XmNrecomputeSize, False,
                                          XmNfontList, gui->fontlist,
                                          XmNhighlightThickness, 0x00,
                                          XmNshadowThickness, 0x00,
                                          XmNbackground, background,
                                          XmNforeground, 0x00,
                                          XmNuserData, gui,
                                          NULL );

    /************************************************/

    /*** This function create a scrolled window ***/
    wren = XmVaCreateManagedScrolledWindow ( frm, "Render",
                                             XmNscrollingPolicy, XmAUTOMATIC,
                                             XmNspacing, 0x00,
                                             XmNtopShadowColor, background,
                                             XmNbottomShadowColor, background,
                                             XmNshadowThickness, 0x01,
                                             XmNforeground, foreground,
                                             XmNbackground, background,

                                             XmNtopAttachment   , XmATTACH_WIDGET,
                                             XmNtopWidget, menubar,
                                             XmNleftAttachment  , XmATTACH_FORM,
                                             XmNrightAttachment , XmATTACH_FORM,
                                             XmNbottomAttachment, XmATTACH_FORM,

                                             XmNuserData, gui,
                                             NULL );

    area = XmVaCreateManagedDrawingArea ( wren, "Drawing Area",
                                          XtNx, 0x00,
                                          XtNy, 0x00,
                                          XtNwidth , rsgwidth,
                                          XtNheight, rsg->height,
                                          XmNforeground, foreground,
                                          XmNbackground, background,
                                          XmNborderWidth, 0x00,
                                          NULL );

    /*** If the rendering window is closed while the raytracer is running,  ***/
    /*** this will allow us to cleanly terminate the rendering thread using ***/
    /*** the DrawingArea Widget's destroywindow callback. The R3DSCENE ptr  ***/
    /*** will be stored in XmNuserData structure member. ***/
    XtAddCallback ( area, XmNdestroyCallback, renderdestroycbk, NULL );
    XtAddCallback ( area, XmNexposeCallback, renderexposecbk, NULL );

    /*** now that the drawing area is created, I can create the menu callback */
    XtAddCallback ( wbtn, XmNactivateCallback, savejpgcbk, area );
    XtManageChild ( wbtn );

    XtManageChild ( cfile );

    XtManageChild ( menubar );


    XtRealizeWidget ( shell );

    if ( rsg->startframe == rsg->endframe ) {
        g3duirendersettings_rendertoimage ( rsg, sce, cam, area );
    } else {
        g3duirendersettings_rendertovideo ( rsg, sce, cam, area );
    }
}

/******************************************************************************/
void deleteSelectedItems ( Widget w, XtPointer client,
                                     XtPointer call ) {
    G3DSCENE  *sce = NULL;
    G3DOBJECT *obj = NULL;
    URMDELSELITEMS *dsi;
    G3DURMANAGER *urm = NULL;
    G3DMESH   *mes = NULL;
    LIST *loldselobj = NULL;
    LIST *loldselver = NULL;
    LIST *loldselfac = NULL;
    LIST *loldseledg = NULL;
    G3DUI *gui = NULL;

    /*** retrieve the global GUI structure ***/
    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    urm = gui->urm;
    sce = gui->sce;

    obj = g3dscene_getLastSelected ( sce );

    mes = ( G3DMESH * ) obj;

    if ( gui->flags & VIEWOBJECT ) {
        g3durm_scene_deleteSelectedObjects ( urm, sce, gui->flags, REDRAWVIEW |
                                                                   REDRAWLIST );

        updateAllCurrentEdit ( gui );
        updateCoords         ( gui );
        redrawObjectList     ( gui );
    } else {
        if ( obj && ( obj->type == G3DMESHTYPE ) ) {
            if ( gui->flags & VIEWVERTEX ) {
                loldselver = list_copy ( mes->lselver );
                loldselfac = g3dmesh_getFaceListFromSelectedVertices ( mes );
            }

            if ( gui->flags & VIEWFACE ) {
                loldselfac = list_copy ( mes->lselfac );
            }

            if ( gui->flags & VIEWEDGE ) {
                loldselver = g3dmesh_getVertexListFromSelectedEdges ( mes );
                loldseledg = list_copy ( mes->lseledg );
                loldselfac = g3dmesh_getFaceListFromSelectedEdges ( mes );
            }

            /*** save state ***/
            dsi = urmdelselitems_new ( sce, NULL,
                                       mes, loldselver,
                                            loldseledg,
                                            loldselfac,
                                       gui->flags );

            g3durmanager_push ( gui->urm, deleteSelectedItems_undo,
                                          deleteSelectedItems_redo,
                                          deleteSelectedItems_free, dsi,
                                          REDRAWVIEW | REDRAWLIST );
  
            /*g3dmesh_unselectAllVertices ( mes );*/
            /*g3dmesh_unselectAllFaces    ( mes );*/

            if ( gui->flags & VIEWVERTEX ) {
                g3dmesh_removeVerticesFromList ( mes, loldselver );
                g3dmesh_removeFacesFromList    ( mes, loldselfac );
            }

            if ( gui->flags & VIEWFACE ) {
                g3dmesh_removeFacesFromList    ( mes, loldselfac );
            }

            if ( gui->flags & VIEWEDGE ) {
                g3dmesh_removeVerticesFromList ( mes, loldselver );
                g3dmesh_removeFacesFromList    ( mes, loldselfac );
            }

            /*** Rebuild the subdivided mesh ***/
            g3dmesh_update ( mes, NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  COMPUTEFACEPOSITION |
                                  COMPUTEFACENORMAL   |
                                  COMPUTEEDGEPOSITION |
                                  COMPUTEVERTEXNORMAL |
                                  REALLOCSUBDIVISION  |
                                  COMPUTESUBDIVISION, gui->flags );
        }
    }

    redrawGLViews ( gui );
}
/******************************************************************************/
/******************************************************************************/


/******************************************************************************/
void openfilecbk ( Widget w, XtPointer client, XtPointer call ) {
    Widget dialog;
    G3DUI *gui = ( G3DUI * ) client;

    dialog = XmCreateFileSelectionDialog ( w, "Open File", 
                                           NULL, 0x00 );

    XtAddCallback ( dialog, XmNokCallback    , openfileokcbk    , gui );
    XtAddCallback ( dialog, XmNcancelCallback, openfilecancelcbk, gui );

    XtManageChild ( dialog );
}

/******************************************************************************/
static void makeeditcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type & PRIMITIVE ) ) {
        g3dui_setHourGlass ( gui );

        /*uint32_t id = g3dscene_getNextObjectID ( sce );*/
        g3durm_primitive_convert ( gui->urm, 
                                   gui->sce,
                                   gui->flags, ( G3DPRIMITIVE * ) obj,
                                               ( G3DOBJECT    * ) obj->parent,
                                               ( REDRAWCURRENTOBJECT | 
                                                 REDRAWVIEW          | 
                                                 REDRAWLIST ) );

        g3dui_unsetHourGlass ( gui );

        /*g3dscene_unselectAllObjects ( sce );
        g3dscene_selectObject ( sce, ( G3DOBJECT * ) mes );*/

        redrawObjectList ( gui );
        updateAllCurrentEdit ( gui );
        redrawGLViews ( gui );
    }
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
static void timerProc ( XtPointer client_data, XtIntervalId *id ) {
    G3DUI *gui = ( G3DUI * ) client_data;
    G3DUITIMELINE *tim = gui->tim;

    if ( tim->curframe < tim->endframe ) {
        /*XtAppAddTimeOut ( gui->app, 25, timerProc, gui );*/

        tim->curframe++;
        printf("Playing animation frame %d\n", tim->curframe );
    } else {
        tim->curframe = 0x00;
        printf("Stopping animation\n");
    }

    g3dobject_anim_r ( ( G3DOBJECT * ) gui->sce, tim->curframe, gui->flags );
    redrawGLViews ( gui );
}

/******************************************************************************/
static void playcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DUITIMELINE *tim = gui->tim;

    tim->curframe = tim->startframe;

    /*XtAppAddTimeOut ( gui->app, 25, timerProc, gui );*/
}

/******************************************************************************/
static void addToolbarAxis ( Widget parent ) {
    Pixel background, foreground;
    G3DUI *gui;
    int depth;

    XtVaGetValues ( parent, XmNuserData, &gui,
                            XmNdepth, &depth,
                            XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    Widget axis = XmVaCreateRowColumn ( parent, "G3DAXIS",
                                        /*** arguments ***/
                                        /*** There are 3 Axis, so ... ***/
                                        XmNwidth, ( TOOLBARBUTTONSIZE * 0x03 ),
                                        XmNheight, TOOLBARBUTTONSIZE,
                                        XmNorientation, XmHORIZONTAL,
                                        XmNspacing, 0x00,
                                        XmNpacking, 0x00,
                                        XmNmarginWidth, 0x00,
                                        XmNmarginHeight, 0x00,
                                        XmNbackground, background,
                                        XmNforeground, foreground,
                                        XmNborderWidth,0x00,
                                        XmNuserData, gui,
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
                                   XmNhighlightThickness, 0x00,
                                   XmNshadowThickness, 0x01,
                                   XmNmarginHeight, 0x00,
                                   XmNmarginWidth, 0x00,
                                   XmNbackground, background,
                                   XmNforeground, foreground,
                                   XmNset, True,
                                   XmNuserData, gui,
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
                                   XmNhighlightThickness, 0x00,
                                   XmNshadowThickness, 0x01,
                                   XmNmarginHeight, 0x00,
                                   XmNmarginWidth, 0x00,
                                   XmNbackground, background,
                                   XmNforeground, foreground,
                                   XmNset, True,
                                   XmNuserData, gui,
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
                                   XmNhighlightThickness, 0x00,
                                   XmNshadowThickness, 0x01,
                                   XmNmarginHeight, 0x00,
                                   XmNmarginWidth, 0x00,
                                   XmNbackground, background,
                                   XmNforeground, foreground,
                                   XmNset, True,
                                   XmNuserData, gui,
                                   NULL );

    /*XtAddCallback ( btn, XmNarmCallback   , armZAxiscbk   , gui );*/
    XtAddCallback ( btn, XmNdisarmCallback, disarmZAxiscbk, gui );
    XtManageChild ( btn );

    XtManageChild ( axis );
}

/******************************************************************************/
void g3dui_addToolBarPushButton ( Widget parent, char **data,
                                  void ( *armfunc ) ( Widget, XtPointer,
                                                                XtPointer ),
                                  void *armdata ) {
    Pixel background, foreground;
    Pixmap pixnor, pixarm;
    Widget btn;
    G3DUI *gui;
    int depth;

    XtVaGetValues ( parent, XmNuserData, &gui,
                            XmNdepth, &depth,
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
                                 XmNhighlightThickness, 0x00,
                                 XmNshadowThickness, 0x01,
                                 XmNmarginHeight, 0x00,
                                 XmNmarginWidth, 0x00,
                                 XmNlabelPixmap , pixnor,
                                 XmNselectPixmap, pixarm,
                                 XmNbackground, background,
                                 XmNforeground, foreground,
                                 XmNuserData, gui,
                                 NULL );

    XtAddCallback ( btn, XmNarmCallback, armfunc, armdata );

    XtManageChild ( btn );
}

/******************************************************************************/
void g3dui_addToolBarToggleButton ( Widget parent, char **data,
                                    void ( *armfunc ) ( Widget, XtPointer,
                                                                XtPointer ),
                                    void *armdata ) {
    Pixel background, foreground;
    Pixmap pixnor, pixarm;
    Widget btn;
    G3DUI *gui;
    int depth;

    XtVaGetValues ( parent, XmNuserData, &gui,
                            XmNdepth, &depth,
                            XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    createTransparentPixmap (  XtDisplay ( parent ), &pixnor,
                                                     &pixarm,
                                                     data,
                                                     depth,
                                                     background,
                                                     foreground );

    btn = XmVaCreateToggleButton ( parent, "G3DBUTTON",
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
                                    XmNorientation, XmHORIZONTAL,
                                    XmNspacing, 0x00,
                                    XmNpacking, 0x00,
                                    XmNmarginWidth, 0x00,
                                    XmNmarginHeight, 0x00,
                                    XmNradioBehavior, 0x01,
                                    XmNbackground, background,
                                    XmNforeground, foreground,
                                    XmNborderWidth, 0x00,
                                    XmNpacking, XmPACK_TIGHT,

                                    XmNuserData, gui,
                                    NULL );
    G3DMOUSETOOL *mou;

    g3dui_addToolBarPushButton ( toolbar, newfile_xpm  , newscenecbk, gui );
    g3dui_addToolBarPushButton ( toolbar, openfile_xpm , openfilecbk, gui );
    g3dui_addToolBarPushButton ( toolbar, saveas_xpm   , saveascbk  , gui );
    g3dui_addToolBarPushButton ( toolbar, save_xpm     , savefilecbk, gui );
    g3dui_addToolBarPushButton ( toolbar, undo_xpm     , undocbk    , gui );
    g3dui_addToolBarPushButton ( toolbar, redo_xpm     , redocbk    , gui );
    g3dui_addToolBarPushButton ( toolbar, delete_xpm   , deleteSelectedItems, gui );

    /********************************/
    mou = g3dmousetool_new ( PICKTOOL, 's', NULL,
                            pickTool_init, pick_draw, pick_tool, VIEWOBJECT | VIEWEDGE |
                                                        VIEWVERTEX | VIEWFACE );
    g3dui_addToolBarToggleButton ( toolbar, pick_xpm, setMouseTool, mou );
    addMouseTool ( gui, mou, 0x00 );

    /********************************/
    mou = g3dmousetool_new ( MOVETOOL, 's', NULL,
                             NULL, NULL, move_tool, VIEWOBJECT | VIEWEDGE |
                                                    VIEWVERTEX | VIEWFACE );
    g3dui_addToolBarToggleButton ( toolbar, move_xpm, setMouseTool, mou );
    addMouseTool ( gui, mou, 0x00 );

    /********************************/
    mou = g3dmousetool_new ( SCALETOOL, 's', NULL,
                             NULL, NULL, scale_tool, VIEWOBJECT | VIEWEDGE |
                                                     VIEWVERTEX | VIEWFACE );
    g3dui_addToolBarToggleButton ( toolbar, scale_xpm, setMouseTool, mou );
    addMouseTool ( gui, mou, 0x00 );

    /********************************/
    mou = g3dmousetool_new ( ROTATETOOL, 'r', NULL,
                             NULL, NULL, rotate_tool, VIEWOBJECT | VIEWEDGE |
                                                      VIEWVERTEX | VIEWFACE );
    g3dui_addToolBarToggleButton ( toolbar, rotate_xpm, setMouseTool, mou );
    addMouseTool ( gui, mou, 0x00 );

    g3dui_addToolBarPushButton ( toolbar, renderw_xpm , renderviewcbk, gui );
    g3dui_addToolBarPushButton ( toolbar, render_xpm  , renderfinalcbk,gui );
    g3dui_addToolBarPushButton ( toolbar, makeedit_xpm, makeeditcbk, gui );

    addToolbarAxis ( toolbar );


    XtManageChild ( toolbar );

    return toolbar;
}
