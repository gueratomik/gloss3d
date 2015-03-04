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
#include <Xm/Label.h>
#include <Xm/Text.h>
#include <Xm/ComboBox.h>
#include <Xm/Frame.h>
#include <Xm/BulletinB.h>

/******************************************************************************/
#include <X11/xpm.h>
#include <X11/cursorfont.h>

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/GLwMDrawA.h>

/******************************************************************************/
#include <config.h>

/******************************************************************************/
/***************************** SAX XML Library ********************************/
#ifdef HAVE_EXPAT_H
#include <expat.h>
#endif

/******************************************************************************/
#include <list.h>
#include <g3d.h>
#include <r3d.h>
#include <r3dnet.h>
#include <g3durmanager.h>
#include <g3dexport.h>
#include <g3dimport.h>
#include <g3dmouse.h>
#include <g3dcom.h>

/******************************************************************************/
#include <TabbedBook.h>
#include <g3dui.h>

/******************************************************************************/
#include <xpm/matedit.xpm>
#include <xpm/sphere.xpm>
#include <xpm/cube.xpm>
#include <xpm/cylinder.xpm>
#include <xpm/knife.xpm>
#include <xpm/addvertex.xpm>
#include <xpm/bridge.xpm>
#include <xpm/extrude.xpm>

/******************************************************************************/
static XtProc ClassInitialize ( void );
static XtInitProc Initialize ( Widget, Widget, ArgList, Cardinal * );
static XtRealizeProc Realize ( Widget, XtValueMask *, XSetWindowAttributes * );
static XtWidgetProc Destroy ( Widget );
static XtWidgetProc Resize ( Widget );
static XtExposeProc Redisplay ( Widget, XEvent *, Region );

/******************************************************************************/
static XtResource widgetRes[] = {
    { XtNscene, XtNscene, XtRPointer, sizeof ( XtPointer ),
      XtOffsetOf ( GUiRec, gui.sce ),
      XtRImmediate, ( XtPointer ) 0x00 },
    { XtNloadFile, XtNloadFile, XtRPointer, sizeof ( XtPointer ),
      XtOffsetOf ( GUiRec, gui.loadFile ),
      XtRImmediate, ( XtPointer ) 0x00 } };

/******************************************************************************/
static void createRenderTable ( Widget w ) {
    GUiWidget guw = ( GUiWidget ) w;
    G3DUI *gui = &guw->gui;
    XmRendition rendition;
    Cardinal n = 0x00;
    Arg args[] = { { XmNfontName , ( XtArgVal ) G3DFONT         },
                 /*{ XmNloadModel, ( XtArgVal ) XmLOAD_DEFERRED },*/
                   { XmNfontType , ( XtArgVal ) XmFONT_IS_FONT  } };

    rendition = XmRenditionCreate ( gui->top, "Gloss3DFont", args, XtNumber ( args ) );

    gui->renTable = XmRenderTableAddRenditions ( NULL, &rendition,
                                                 0x01,
                                                 XmMERGE_REPLACE );
}

/******************************************************************************/
static void addRenderSettings ( Widget w, G3DUIRENDERSETTINGS *rsg ) {
    GUiWidget guw = ( GUiWidget ) w;
    G3DUI *gui = &guw->gui;

    list_insert ( &gui->lrsg, rsg );
}

/******************************************************************************/
void g3dui_sighandler ( int signo, siginfo_t *info, void *extra ) {
    static G3DUI *gui;

    if ( info == NULL ) {
        gui  = ( G3DUI * ) extra;
    } else {
        G3DCOM *com = ( G3DCOM * ) info->si_value.sival_ptr;

        /*printf ( "Communication type %d received\n", com->type );*/

        switch ( com->type ) {
            case COMGOTOFRAME : {
                G3DCOMGOTOFRAME *gtf = ( G3DCOMGOTOFRAME * ) com;
                G3DUITIMELINE *tim = gui->tim;

                g3dobject_anim_r ( ( G3DOBJECT * ) gtf->sce,
                                                   gtf->frame, gui->flags );

                /*** COMMENTED: DO NOT CALL ANY XLIB FUNCTION IN A  SIGNAL  ***/
                /*** The signal may be started in the middle of a XLib call ***/
                /*** leading to a deadlock. The program may hang. More info:***/
                /*http://www-h.eng.cam.ac.uk/help/tpl/graphics/X/signals.html */
                /*redrawGLViews ( gui );*/
                /*redrawTimeline ( gui );*/

                g3dcomgotoframe_resume ( gtf );
            } break;

            default :
            break;
        }
    }
}

/******************************************************************************/
static void createFonts ( Widget w ) {
    Display *dis = XtDisplay ( w );
    GUiWidget guw = ( GUiWidget ) w;
    G3DUI *gui = &guw->gui;

    /*** Create the normal font ***/
    if ( ( gui->nrmlft = XLoadQueryFont ( dis, G3DFONT ) ) == NULL ) {
        fprintf ( stderr, "Cannot create %s, switching to 6x13\n", G3DFONT );

        /*** Switch to the default font ***/ 
        if ( ( gui->nrmlft = XLoadQueryFont ( dis, "6x13" ) ) == NULL ) {
            fprintf ( stderr, "Cannot create 6x13 font\n" );

            return;
        }
    }

    /*** I create a Motif font here because the other ones will be used ***/
    /*** by Xlib calls, so I'll directly use the XFontStruct, not the   ***/
    /*** Motif fontlist ( that I don't fully understand by the way ).   ***/
    XmFontList fontlist = XmFontListCreate ( gui->nrmlft,
                                             XmSTRING_DEFAULT_CHARSET );
    if ( fontlist == NULL ) {
        fprintf ( stderr, "XmFontListCreate failed\n", G3DFONT );

        return;
    }

    gui->fontlist = fontlist;

    /*** Create the bold font ***/
    if ( ( gui->boldft = XLoadQueryFont ( dis, G3DBOLDFONT ) ) == NULL ) {
        fprintf ( stderr, "Cannot create %s, switching to 6x13bold\n", G3DBOLDFONT);

        /*** switch to default font ***/
        if ( ( gui->boldft = XLoadQueryFont ( dis, "6x13bold" ) ) == NULL ) {
            XFreeFont ( dis, gui->nrmlft );

            fprintf ( stderr, "Cannot create 6x13bold font\n" );

            return;
        }
    }

    /*** Create the tiny font **/
    if ( ( gui->tinyft = XLoadQueryFont ( dis, G3DTINYFONT ) ) == NULL ) {
        fprintf ( stderr, "Cannot create %s, switching to 6x9\n", G3DTINYFONT );

        /*** switch to the default font ***/
        if ( ( gui->tinyft = XLoadQueryFont ( dis, "6x9" ) ) == NULL ) {
            XFreeFont ( dis, gui->nrmlft );
            XFreeFont ( dis, gui->boldft );

            fprintf ( stderr, "Cannot create 6x9 font\n" );

            return;
        }
    }
}

/******************************************************************************/
void verifycharcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmTextVerifyCallbackStruct *vcs = ( XmTextVerifyCallbackStruct * ) call;
    int num;

    if ( vcs->text->length ) {
        int i;

        for ( i = 0x00; i < vcs->text->length; i++ ) {
            char c = vcs->text->ptr[i];

            if ( (  c <  'A' || c > 'Z' ) &&
                 (  c <  'a' || c > 'z' ) &&
                 (  c <  '0' || c > '9' ) &&
                 (  c != ' ' ) &&
                 (  c != '-' ) &&
                 (  c != '_' ) && 
                 (  c != '.' ) ) {

                vcs->doit = False;

                return;
            }
        }
    }

    vcs->doit = True;
}

/******************************************************************************/
void verifyfloatcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmTextVerifyCallbackStruct *vcs = ( XmTextVerifyCallbackStruct * ) call;
    int num;

    if ( vcs->text->length ) {
        int i;

        for ( i = 0x00; i < vcs->text->length; i++ ) {
            switch ( vcs->text->ptr[i] ) {
                case '0' :
                case '1' :
                case '2' :
                case '3' :
                case '4' :
                case '5' :
                case '6' :
                case '7' :
                case '8' :
                case '9' :
                case '-' :
                case '.' :
                break;

                default :
                    vcs->doit = False;

                    return;
                break;
            }
        }
    }

    vcs->doit = True;
}

/******************************************************************************/
void verifyintcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmTextVerifyCallbackStruct *vcs = ( XmTextVerifyCallbackStruct * ) call;
    int num;

    if ( vcs->text->length ) {
        int i;

        for ( i = 0x00; i < vcs->text->length; i++ ) {
            switch ( vcs->text->ptr[i] ) {
                case '0' :
                case '1' :
                case '2' :
                case '3' :
                case '4' :
                case '5' :
                case '6' :
                case '7' :
                case '8' :
                case '9' :
                case '-' :
                break;

                default :
                    vcs->doit = False;

                    return;
                break;
            }
        }
    }

    vcs->doit = True;
}

/******************************************************************************/
void createSimplePushButton  ( Widget parent, char *name,
                               Dimension x, Dimension y,
                               Dimension width, Dimension height,
                               void (*cbk)( Widget, XtPointer, 
                                                    XtPointer ) ) {
    Pixel background, foreground;
    G3DUI *gui;
    Widget btn;

    XtVaGetValues ( parent, XmNuserData, &gui,
                            XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    btn = XmVaCreateManagedPushButton ( parent, name,
                                        XmNx, x,
                                        XmNy, y,
                                        XmNwidth , width,
                                        XmNheight, height,
                                        XmNfontList, gui->fontlist,
                                        XmNmarginLeft, 0x04 ,
                                        XmNforeground, foreground,
                                        XmNbackground, background,
                                        XmNrecomputeSize, False,
                                        XmNhighlightThickness, 0x00,
                                        XmNshadowThickness, 0x01,
                                        XmNmarginHeight, 0x00,
                                        XmNmarginWidth, 0x00,
                                        XmNuserData, gui,
                                        /*** in case it belongs to a frame ***/
                                        XmNframeChildType, XmFRAME_GENERIC_CHILD,
                                        NULL );

    if ( cbk ) XtAddCallback ( btn, XmNactivateCallback, cbk, gui );
}

/******************************************************************************/
void createSimpleLabel ( Widget parent, char *name,
                         Dimension x, Dimension y,
                         Dimension width ) {
    Pixel background, foreground;
    G3DUI *gui;
    Widget lab;

    XtVaGetValues ( parent, XmNuserData, &gui,
                            XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    lab = XmVaCreateManagedLabel ( parent, name,
                                   XmNx, x,
                                   XmNy, y,
                                   XmNwidth , width,
                                   XmNheight, 0x10,
                                   XmNfontList, gui->fontlist,
                                   XmNforeground, foreground,
                                   XmNbackground, background,
                                   /*** in case it belongs to a frame ***/
                                   XmNframeChildType, XmFRAME_GENERIC_CHILD,

                                   NULL );
}

/******************************************************************************/
void arrowSetValue ( Widget w, int value ) {
    WidgetList children;
    Cardinal nc;
    uint32_t i;

    XtVaGetValues ( w, XmNchildren, &children,
                       XmNnumChildren, &nc,
                       NULL );

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];

        if ( XtClass ( child ) == xmTextWidgetClass ) {
            char buf[0x10];
            snprintf ( buf, 0x10, "%d", value );

            XmTextSetString ( child, buf );
        }
    }
}

/******************************************************************************/
Widget createArrow ( Widget parent, G3DUI *gui,
                                    char *name,
                                    Dimension x, Dimension y,
                                    Dimension width, Dimension height,
                                    void (*inccbk) ( Widget, XtPointer,
                                                             XtPointer ),
                                    void (*deccbk) ( Widget, XtPointer,
                                                             XtPointer ),
                                    void (*txtcbk) ( Widget, XtPointer,
                                                             XtPointer ) ) {
    Widget morebtn, lessbtn, textbtn, frm;
    Pixel white = XWhitePixel ( XtDisplay ( parent ), 0x00 );
    Pixel background, foreground;

    XtVaGetValues ( parent, XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    frm = XmVaCreateForm ( parent, name,
                                   XtNx, x,
                                   XtNy, y,
                                   XtNwidth , 0x60,
                                   XtNheight, height,
                                   XmNforeground, foreground,
                                   XmNbackground, 0x00,
                                   XmNborderWidth, 0x00,
                                   /*** in case it belongs to a frame ***/
                                   XmNframeChildType, XmFRAME_GENERIC_CHILD,

                                   XmNuserData, gui,
                                   NULL );

    lessbtn = XmVaCreateManagedPushButton ( frm, "-",
                                                 XmNx, 0x00,
                                                 XmNy, 0x00,
                                                 XmNwidth,  0x20,
                                                 XmNheight, height,
                                                 XmNfontList, gui->fontlist,
                                                 XmNmarginLeft, 0x04 ,
                                                 XmNforeground, foreground,
                                                 XmNbackground, background,
                                                 XmNrecomputeSize, False,
                                                 XmNhighlightThickness, 0x00,
                                                 XmNshadowThickness, 0x01,
                                                 XmNmarginHeight, 0x00,
                                                 XmNmarginWidth, 0x00,
                                                 /*** in case it belongs to a frame ***/
                                                 XmNframeChildType, XmFRAME_GENERIC_CHILD,

                                                 XmNuserData, gui,
                                                 NULL );

    textbtn = XmVaCreateManagedText ( frm, name,
                                           XmNx, 0x20,
                                           XmNy, 0x00,
                                           XmNwidth , 0x20,
                                           XmNheight, height,
                                           XmNfontList, gui->fontlist,
                                           XmNmaxLength, 0x03,
                                           XmNmarginWidth, 0x00,
                                           XmNmarginHeight, 0x00,
                                           XmNshadowThickness, 0x01,
                                           XmNhighlightOnEnter, True,
                                           XmNhighlightThickness, 0x01,
                                           XmNforeground, foreground,
                                           XmNbackground, white,
                                           /*** in case it belongs to a frame ***/
                                           XmNframeChildType, XmFRAME_GENERIC_CHILD,

                                           XmNuserData, gui,
                                           NULL );

    /*** check input ***/
    XtAddCallback ( textbtn, XmNmodifyVerifyCallback, verifyintcbk, NULL );

    morebtn = XmVaCreateManagedPushButton ( frm, "+",
                                                 XmNx, 0x40,
                                                 XmNy, 0x00,
                                                 XmNwidth,  0x20,
                                                 XmNheight, height,
                                                 XmNfontList, gui->fontlist,
                                                 XmNmarginLeft, 0x04 ,
                                                 XmNforeground, foreground,
                                                 XmNbackground, background,
                                                 XmNrecomputeSize, False,
                                                 XmNhighlightThickness, 0x00,
                                                 XmNshadowThickness, 0x01,
                                                 XmNmarginHeight, 0x00,
                                                 XmNmarginWidth, 0x00,
                                                 /*** in case it belongs to a frame ***/
                                                 XmNframeChildType, XmFRAME_GENERIC_CHILD,

                                                 XmNuserData, gui,
                                                 NULL );

    if ( txtcbk ) XtAddCallback ( textbtn, XmNvalueChangedCallback, txtcbk, gui );
    if ( deccbk ) XtAddCallback ( lessbtn, XmNarmCallback         , deccbk, gui );
    if ( inccbk ) XtAddCallback ( morebtn, XmNarmCallback         , inccbk, gui );

    XtManageChild ( frm );


    return frm;
}

/******************************************************************************/
void createToggleLabel ( Widget parent, char *name,
                          Dimension x, Dimension y,
                          Dimension width, Dimension height,
                          void (*armcbk)( Widget, XtPointer, XtPointer ) ) {
    Pixel background, foreground;
    G3DUI *gui;
    Widget btn;

    XtVaGetValues ( parent, XmNuserData, &gui,
                            XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    btn = XmVaCreateManagedToggleButton ( parent, name,
                                          XmNx, x,
                                          XmNy, y,
                                          XmNwidth,  width,
                                          XmNheight, height,
                                          XmNfontList, gui->fontlist,
                                          XmNindicatorOn, True,
                                          XmNrecomputeSize, False,
                                          XmNhighlightThickness, 0x00,
                                          XmNshadowThickness, 0x00,
                                          XmNmarginHeight, 0x00,
                                          XmNmarginWidth, 0x00,
                                          XmNbackground, background,
                                          XmNforeground, foreground,
                                  /*** in case it belongs to a frame ***/
                                  XmNframeChildType, XmFRAME_GENERIC_CHILD,

                                          XmNuserData, gui,
                                          NULL );

    if ( armcbk ) XtAddCallback ( btn, XmNarmCallback, armcbk, gui );
}

/******************************************************************************/
Widget createFrame ( Widget parent, G3DUI *gui,
                                    char *name,
                                    Dimension x, Dimension y,
                                    Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget frm;

    XtVaGetValues ( parent, XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    frm = XmVaCreateManagedFrame ( parent, name,
                                           XmNx, x,
                                           XmNy, y,
                                           XmNwidth,  width,
                                           XmNheight, height,
                                           XmNforeground, foreground,
                                           XmNbackground, background,
                                           XmNborderWidth, 0x00,

                                           XmNuserData, gui,
                                           NULL );

    XmVaCreateManagedLabel ( frm, name,
                                  XtNx, 0x00,
                                  XtNy, 0x00,
                                  XmNwidth , 0x60,
                                  XmNheight, 0x20,
                                  XmNfontList, gui->fontlist,
                                  XmNframeChildType, XmFRAME_TITLE_CHILD,
                                  XmNforeground, foreground,
                                  XmNbackground, background,
                                  NULL );

    return frm;
}

/******************************************************************************/
Widget createBoard ( Widget parent, G3DUI *gui,
                                    char *name,
                                    Dimension x, Dimension y,
                                    Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget brd;

    XtVaGetValues ( parent, XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    brd = XmVaCreateBulletinBoard ( parent, name,
                                            XtNx, x,
                                            XtNy, y,
                                            XmNwidth , width,
                                            XmNheight, height,
                                            XmNfontList, gui->fontlist,
                                            XmNframeChildType, XmFRAME_GENERIC_CHILD,
                                            XmNforeground, foreground,
                                            XmNbackground, background,
XmNresizePolicy, XmRESIZE_NONE,
XmNdefaultPosition, False,
                                            XmNuserData, gui,
                                            NULL );

    XtManageChild ( brd );


    return brd;
}

/******************************************************************************/
void createRadioButton ( Widget parent, char *name,
                         Dimension x, Dimension y,
                         Dimension width, Dimension height,
                         void (*armcbk)( Widget, XtPointer, XtPointer ) ) {
    Pixel background, foreground;
    G3DUI *gui;
    Widget btn;

    XtVaGetValues ( parent, XmNuserData, &gui,
                            XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    btn = XmVaCreateManagedToggleButton ( parent, name,
                                          XmNx, x,
                                          XmNy, y,
                                          XmNwidth,  width,
                                          XmNheight, height,
                                          XmNfontList, gui->fontlist,
                                          XmNindicatorOn, True,
                                          XmNrecomputeSize, False,
                                          XmNhighlightThickness, 0x00,
                                          XmNshadowThickness, 0x01,
                                          XmNmarginHeight, 0x00,
                                          XmNmarginWidth, 0x00,
                                          XmNbackground, background,
                                          XmNforeground, foreground,

                                          XmNuserData, gui,
                                          NULL );

    if ( armcbk ) XtAddCallback ( btn, XmNarmCallback, armcbk, gui );
}

/******************************************************************************/
void createToggleButton ( Widget parent, char *name,
                          Dimension x, Dimension y,
                          Dimension width, Dimension height,
                          void (*armcbk)( Widget, XtPointer, XtPointer ) ) {
    Pixel background, foreground;
    G3DUI *gui;
    Widget btn;

    XtVaGetValues ( parent, XmNuserData, &gui,
                            XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    btn = XmVaCreateManagedToggleButton ( parent, name,
                                          XmNx, x,
                                          XmNy, y,
                                          XmNwidth,  width,
                                          XmNheight, height,
                                          XmNfontList, gui->fontlist,
                                          XmNindicatorOn, False,
                                          XmNrecomputeSize, False,
                                          XmNhighlightThickness, 0x00,
                                          XmNshadowThickness, 0x01,
                                          XmNmarginHeight, 0x00,
                                          XmNmarginWidth, 0x00,
                                          XmNbackground, background,
                                          XmNforeground, foreground,
                                          /*** in case it belongs to a frame ***/
                                          XmNframeChildType, XmFRAME_GENERIC_CHILD,

                                          XmNuserData, gui,
                                          NULL );

    if ( armcbk ) XtAddCallback ( btn, XmNarmCallback, armcbk, gui );
}

/******************************************************************************/
void createUVMappingSelection ( Widget parent, char *name,
                                               Dimension x, Dimension y,
                                               Dimension labwidth,
                                               Dimension txtwidth,
                                               void (*cbk)( Widget, 
                                                            XtPointer,
                                                            XtPointer ) ) {
    Pixel white = XWhitePixel ( XtDisplay ( parent ), 0x00 );
    Pixel background, foreground;
    uint32_t strsize = sizeof ( XmString );
    XmStringTable dmlist = ( XmStringTable ) XtMalloc ( 0x03 * strsize );
    G3DUI *gui;
    Widget lab;
    Widget sel;

    XtVaGetValues ( parent, XmNuserData, &gui,
                            XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    dmlist[0x00] = XmStringCreate ( FLATPROJECTION       , XmFONTLIST_DEFAULT_TAG );
    dmlist[0x01] = XmStringCreate ( SPHERICALPROJECTION  , XmFONTLIST_DEFAULT_TAG );
    dmlist[0x02] = XmStringCreate ( CYLINDRICALPROJECTION, XmFONTLIST_DEFAULT_TAG );

    lab = XmVaCreateManagedLabel ( parent, name,
                                   XmNx, x,
                                   XmNy, y,
                                   XmNwidth , labwidth,
                                   XmNheight, 0x12,
                                   XmNfontList, gui->fontlist,
                                   XmNforeground, foreground,
                                   XmNbackground, background,
                                   NULL );

    sel = XmVaCreateManagedComboBox ( parent, name, 
                                       XmNx, x + labwidth,
                                       XmNy, y,
                                       XmNwidth , txtwidth,
                                       XmNhighlightThickness, 0x00,
                                       XmNshadowThickness, 0x01,
                                       XmNmarginHeight, 0x00,
                                       XmNmarginWidth, 0x00,
                                       XmNitemCount,	0x03,
                                       XmNitems, dmlist,
                                       XmNvisibleItemCount, 0x03,
                                       XmNfontList, gui->fontlist,
                                       XmNarrowSize, 0x0C,
                                       XmNcomboBoxType, XmDROP_DOWN_LIST,
                                       XmNrenderTable, gui->renTable,
                                       XmNtraversalOn, False,
                                       XtNbackground, background,
                                       XtNforeground, foreground,
                                       NULL );

    /*** OpenMotif prior to 2.3.4 crashes if this is set before managing ***/
    XtVaSetValues ( sel, XmNheight, 0x12, NULL );

    XmComboBoxSelectItem ( sel, dmlist[0x00] );

    XmStringFree ( dmlist[0x00] );
    XmStringFree ( dmlist[0x01] );
    XmStringFree ( dmlist[0x02] );

    XtFree ( ( char * ) dmlist );

    if ( cbk ) {
        XtAddCallback ( sel, XmNselectionCallback, cbk, gui );
    }
}

/******************************************************************************/
void createOrientationSelection ( Widget parent, char *name,
                                                 Dimension x, Dimension y,
                                                 Dimension labwidth,
                                                 Dimension txtwidth,
                                                 void (*cbk)( Widget, 
                                                              XtPointer,
                                                              XtPointer ) ) {
    Pixel white = XWhitePixel ( XtDisplay ( parent ), 0x00 );
    Pixel background, foreground;
    uint32_t strsize = sizeof ( XmString );
    XmStringTable dmlist = ( XmStringTable ) XtMalloc ( 0x03 * strsize );
    G3DUI *gui;
    Widget lab;
    Widget sel;

    XtVaGetValues ( parent, XmNuserData, &gui,
                            XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    dmlist[0x00] = XmStringCreate ( ZXSTR, XmFONTLIST_DEFAULT_TAG );
    dmlist[0x01] = XmStringCreate ( XYSTR, XmFONTLIST_DEFAULT_TAG );
    dmlist[0x02] = XmStringCreate ( YZSTR, XmFONTLIST_DEFAULT_TAG );

    lab = XmVaCreateManagedLabel ( parent, name,
                                   XmNx, x,
                                   XmNy, y,
                                   XmNwidth , labwidth,
                                   XmNheight, 0x12,
                                   XmNfontList, gui->fontlist,
                                   XmNforeground, foreground,
                                   XmNbackground, background,
                                   NULL );

    sel = XmVaCreateManagedComboBox ( parent, name, 
                                       XmNx, x + labwidth,
                                       XmNy, y,
                                       XmNwidth , txtwidth,
                                       XmNhighlightThickness, 0x00,
                                       XmNshadowThickness, 0x01,
                                       XmNmarginHeight, 0x00,
                                       XmNmarginWidth, 0x00,
                                       XmNitemCount,	0x03,
                                       XmNitems, dmlist,
                                       XmNvisibleItemCount, 0x03,
                                       XmNeditable, False,/*
                                       XmNverticalMargin, 0x01,
                                       XmNhorizontalMargin, 0x01,*/
                                       XmNfontList, gui->fontlist,
                                       XmNarrowSize, 0x0C,
                                       XmNcomboBoxType, XmDROP_DOWN_LIST,
                                       XmNrenderTable, gui->renTable,
                                       XmNtraversalOn, False,
                                       XtNbackground, background,
                                       XtNforeground, foreground,
                                       NULL );

    /*** OpenMotif prior to 2.3.4 crashes if this is set before managing ***/
    XtVaSetValues ( sel, XmNheight, 0x12, NULL );

    XmComboBoxSelectItem ( sel, dmlist[0x00] );

    XmStringFree ( dmlist[0x00] );
    XmStringFree ( dmlist[0x01] );
    XmStringFree ( dmlist[0x02] );

    XtFree ( ( char * ) dmlist );

    if ( cbk ) {
        XtAddCallback ( sel, XmNselectionCallback, cbk, gui );
    }
}

/******************************************************************************/
void createFloatText ( Widget parent, char *name,
                                      Dimension x, Dimension y,
                                      Dimension labwidth,
                                      Dimension txtwidth,
                                      void (*cbk)( Widget, XtPointer,
                                                           XtPointer ) ) {
    Pixel white = XWhitePixel ( XtDisplay ( parent ), 0x00 );
    Pixel background, foreground;
    G3DUI *gui;
    Widget txt; /*** textfield ***/
    Widget lab; /*** Label     ***/

    XtVaGetValues ( parent, XmNuserData, &gui,
                            XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    if ( labwidth ) {
        lab = XmVaCreateManagedLabel ( parent, name,
                                       XmNx, x,
                                       XmNy, y,
                                       XmNwidth , labwidth,
                                       XmNheight, 0x12,
                                       XmNalignment, XmALIGNMENT_END,
                                       XmNframeChildType, XmFRAME_GENERIC_CHILD,
                                       XmNfontList, gui->fontlist,
                                       XmNforeground, foreground,
                                       XmNbackground, background,
                                       XmNuserData, gui,
                                       NULL );
    }

    txt = XmVaCreateManagedText ( parent, name,
                                  XmNx, x + labwidth,
                                  XmNy, y,
                                  XmNwidth , txtwidth,
                                  XmNheight, 0x12,
                                  XmNfontList, gui->fontlist,
                                  XmNframeChildType, XmFRAME_GENERIC_CHILD,
                                  XmNmaxLength, 0x10,
                                  XmNmarginWidth, 0x00,
                                  XmNmarginHeight, 0x00,
                                  XmNshadowThickness, 0x01,
                                  XmNhighlightOnEnter, True,
                                  XmNhighlightThickness, 0x01,
                                  XmNforeground, foreground,
                                  XmNbackground, white,
                                  XmNuserData, gui,
                                  NULL );

    XtAddCallback ( txt, XmNmodifyVerifyCallback, verifyfloatcbk, NULL );

    if ( cbk ) {
        XtAddCallback ( txt, XmNvalueChangedCallback, cbk  , gui  );
    }
}

/******************************************************************************/
Widget createIntegerText ( Widget parent, char *name,
                                        Dimension x, Dimension y,
                                        Dimension labwidth,
                                        Dimension txtwidth,
                                        void (*cbk)( Widget, XtPointer, 
                                                             XtPointer ) ) {
    Pixel white = XWhitePixel ( XtDisplay ( parent ), 0x00 );
    Pixel background, foreground;
    G3DUI *gui;
    Widget txt; /*** textfield ***/
    Widget lab; /*** Label     ***/

    XtVaGetValues ( parent, XmNuserData, &gui,
                            XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    if ( labwidth ) {
        lab = XmVaCreateManagedLabel ( parent, name,
                                       XmNx, x,
                                       XmNy, y,
                                       XmNwidth , labwidth,
                                       XmNheight, 0x12,
                                       XmNfontList, gui->fontlist,
                                       XmNforeground, foreground,
                                       XmNbackground, background,
                                       XmNalignment, XmALIGNMENT_END,
                                      /*** in case it belongs to a frame ***/
                                       XmNframeChildType, XmFRAME_GENERIC_CHILD,
                                       NULL );
    }

    txt = XmVaCreateManagedText ( parent, name,
                                  XmNx, x + labwidth,
                                  XmNy, y,
                                  XmNwidth , txtwidth,
                                  XmNheight, 0x12,
                                  XmNfontList, gui->fontlist,
                                  XmNmaxLength, 0x03,
                                  XmNmarginWidth, 0x00,
                                  XmNmarginHeight, 0x00,
                                  XmNshadowThickness, 0x01,
                                  XmNhighlightOnEnter, True,
                                  XmNhighlightThickness, 0x01,
                                  XmNforeground, foreground,
                                  XmNbackground, white,
                                  /*** in case it belongs to a frame ***/
                                  XmNframeChildType, XmFRAME_GENERIC_CHILD,
                                  NULL );

    XtAddCallback ( txt, XmNmodifyVerifyCallback, verifyintcbk, NULL );

    if ( cbk ) {
        XtAddCallback ( txt, XmNvalueChangedCallback, cbk, gui );
    }

    return txt;
}

/******************************************************************************/
void createCharText ( Widget parent, char *name,
                                     Dimension x, Dimension y,
                                     Dimension labwidth,
                                     Dimension txtwidth,
                                     void (*cbk)( Widget, XtPointer, 
                                                          XtPointer ) ) {
    Pixel white = XWhitePixel ( XtDisplay ( parent ), 0x00 );
    Pixel background, foreground;
    G3DUI *gui;
    Widget txt; /*** textfield ***/
    Widget lab; /*** Label     ***/

    XtVaGetValues ( parent, XmNuserData, &gui,
                            XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    if ( labwidth ) {
        lab = XmVaCreateManagedLabel ( parent, name,
                                       XmNx, x,
                                       XmNy, y,
                                       XmNwidth , labwidth,
                                       XmNheight, 0x12,
                                       XmNfontList, gui->fontlist,
                                       XmNalignment, XmALIGNMENT_END,
                                      /*** in case it belongs to a frame ***/
                                       XmNframeChildType, XmFRAME_GENERIC_CHILD,
                                       XmNforeground, foreground,
                                       XmNbackground, background,
                                       NULL );
    }

    txt = XmVaCreateManagedText ( parent, name,
                                  XmNx, x + labwidth + 0x04,
                                  XmNy, y,
                                  XmNwidth , txtwidth,
                                  XmNheight, 0x12,
                                  XmNfontList, gui->fontlist,
                                  XmNmaxLength, 0xFF,
                                  XmNmarginWidth, 0x00,
                                  XmNmarginHeight, 0x00,
                                  XmNshadowThickness, 0x01,
                                  XmNhighlightOnEnter, True,
                                  XmNhighlightThickness, 0x01,
                                  XmNforeground, foreground,
                                  /*** in case it belongs to a frame ***/
                                  XmNframeChildType, XmFRAME_GENERIC_CHILD,
                                  XmNbackground, white,
                                  NULL );

    XtAddCallback ( txt, XmNmodifyVerifyCallback, verifycharcbk, NULL );

    if ( cbk ) {
        XtAddCallback ( txt, XmNvalueChangedCallback, cbk, gui );
    }
}

/******************************************************************************/
void createTransparentPixmap ( Display *dis,    /* Display                    */
                               Pixmap *pixnor,  /* Return value:Normal Pixmap */
                               Pixmap *pixarm,  /* Return value:Armed Pixmap  */
                               char **data,     /* Color values               */
                               int depth,       /* Color depth                */
                               Pixel norpix,    /* Background col when normal */
                               Pixel armpix ) { /* Background col when armed  */

    XpmColorSymbol normask = { NULL, "none", norpix },
                   armmask = { NULL, "none", armpix };
    XpmAttributes atr;
    /*** We use the root window because using the Widget's window would not ***/
    /*** usually work as this is called when the window is not realized yet ***/
    Window win = RootWindow ( dis, 0x00 );

    /*XtVaGetValues ( gui->mainfrm, XmNdepth, &depth, NULL );*/

    atr.valuemask  = XpmColorSymbols | XpmCloseness | XpmDepth;
    atr.numsymbols = 0x0001;
    atr.closeness  = 0xFFFF;
    atr.depth      = depth;

    /*** XPM when button is in normal state ***/
    if ( pixnor ) {
        atr.colorsymbols = &normask;

        XpmCreatePixmapFromData ( dis, win, data, pixnor, NULL, &atr );
    }

    /*** XPM when button is in armed state ***/
    if ( pixarm ) {
        atr.colorsymbols = &armmask;

        XpmCreatePixmapFromData ( dis, win, data, pixarm, NULL, &atr );
    }
}

/******************************************************************************/
static XtProc ClassInitialize ( ) {

    return ( XtProc ) NULL;
}

/******************************************************************************/
static XtInitProc Initialize ( Widget request, Widget init,
                               ArgList args, Cardinal *num ) {

    return ( XtInitProc ) NULL;
}

/******************************************************************************/
void redrawObjectList ( G3DUI *gui ) {
    LIST *ltmp = gui->lobjlist;

    while ( ltmp ) {
        Widget w = ( Widget ) ltmp->data;

        XClearArea ( XtDisplay ( w ),
                     XtWindow  ( w ), 0x00, 0x00, 0x00, 0x00, True );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void redrawTimeline ( G3DUI *gui ) {
    LIST *ltmp = gui->ltimeline;

    while ( ltmp ) {
        Widget w = ( Widget ) ltmp->data;

        XClearArea ( XtDisplay ( w ),
                     XtWindow  ( w ), 0x00, 0x00, 0x00, 0x00, True );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
G3DMOUSETOOL *g3dui_getMouseTool ( G3DUI *gui, char *name ) {
    LIST *ltmp = gui->lmou;

    while ( ltmp ) {
        G3DMOUSETOOL *mou = ( G3DMOUSETOOL * ) ltmp->data;

        if ( strcmp ( mou->name, name ) == 0x00 ) {

            return mou;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
void addMouseTool ( G3DUI *gui, G3DMOUSETOOL *mou, uint32_t tool_flags ) {
    /*if ( flags & OBJECTMODETOOL ) {
        addObjectToolsButton ( gui, mou );
    }*/

    /*if ( flags & VERTEXMODETOOL ) {
        addVertexToolsButton ( gui, mou );
    }*/

    /*if ( flags & FACEMODETOOL ) {
        addFaceToolsButton ( gui, mou );
    }*/

    /*if ( flags & EDGEMODETOOL ) {
        g3dui_addEdgeToolsButton ( gui, mou );
    }*/

    if ( tool_flags & GLMENUTOOL ) {
        dispatchMenuButton ( gui, mou, tool_flags );
    }

    list_insert ( &gui->lmou, mou );
}

/******************************************************************************/
static void setDefaultMouseTools ( G3DUI *gui ) {
    G3DMOUSETOOL *mou;

    /********************************/
    mou = g3dmousetool_new ( CREATESPHERETOOL, 'a', sphere_xpm,
                             NULL, NULL, createSphere, VIEWOBJECT );
    addMouseTool ( gui, mou, OBJECTMODETOOL | GLMENUTOOL );

    /********************************/
    mou = g3dmousetool_new ( CREATECUBETOOL, 'a', cube_xpm,
                             NULL, NULL, createCube, VIEWOBJECT );
    addMouseTool ( gui, mou, OBJECTMODETOOL | GLMENUTOOL );

    /********************************/
    mou = g3dmousetool_new ( CREATEPLANETOOL, 'a', sphere_xpm,
                             NULL, NULL, createPlane, VIEWOBJECT );
    addMouseTool ( gui, mou, OBJECTMODETOOL | GLMENUTOOL );

    /********************************/
    mou = g3dmousetool_new ( CREATECYLINDERTOOL, 'a', cylinder_xpm,
                             NULL, NULL, createCylinder, VIEWOBJECT );
    addMouseTool ( gui, mou, OBJECTMODETOOL | GLMENUTOOL );

    /********************************/
    mou = g3dmousetool_new ( CREATETORUSTOOL, 'a', cylinder_xpm,
                             NULL, NULL, createTorus, VIEWOBJECT );
    addMouseTool ( gui, mou, OBJECTMODETOOL | GLMENUTOOL );

    /********************************/
    mou = g3dmousetool_new ( CREATEBONETOOL, 'a', cylinder_xpm,
                             NULL, NULL, createBone, VIEWOBJECT );
    addMouseTool ( gui, mou, OBJECTMODETOOL | GLMENUTOOL );

    /********************************/
    mou = g3dmousetool_new ( CUTMESHTOOL, 'a', knife_xpm,
                             cutMesh_init,
                             cutMesh_draw,
                             cutMesh_tool, VIEWVERTEX | VIEWFACE );
    addMouseTool ( gui, mou, VERTEXMODETOOL |
                             FACEMODETOOL   |
                             EDGEMODETOOL   | GLMENUTOOL );

    /********************************/
    mou = g3dmousetool_new ( ADDVERTEXTOOL, 'a', addvertex_xpm,
                             NULL, NULL, createVertex, VIEWVERTEX );
    addMouseTool ( gui, mou, VERTEXMODETOOL | GLMENUTOOL );

    /********************************/
    mou = g3dmousetool_new ( BRIDGETOOL, 'a', bridge_xpm,
                             createFace_init,
                             createFace_draw, createFace_tool, VIEWVERTEX );
    addMouseTool ( gui, mou, VERTEXMODETOOL | GLMENUTOOL );

    /********************************/
    mou = g3dmousetool_new ( PAINTWEIGHTTOOL, 'x', extrude_xpm,
                             paintWeight_init,
                             NULL, paintWeight_tool, VIEWSKIN );
    addMouseTool ( gui, mou, VERTEXMODETOOL );

    /********************************/
    mou = g3dmousetool_new ( EXTRUDETOOL, 'x', extrude_xpm,
                             extrudeFace_init,
                             NULL, extrudeFace_tool, VIEWVERTEX );
    addMouseTool ( gui, mou, FACEMODETOOL | GLMENUTOOL );

    /********************************/
    mou = g3dmousetool_new ( EXTRUDINNERTOOL, 'x', extrude_xpm,
                             extrudeInner_init,
                             NULL, extrudeFace_tool, VIEWVERTEX );
    addMouseTool ( gui, mou, FACEMODETOOL | GLMENUTOOL );

    /********************************/
    mou = g3dmousetool_new ( UNTRIANGULATETOOL, 'a', NULL,
                             untriangulate_init,
                             NULL, NULL, VIEWVERTEX );
    addMouseTool ( gui, mou, FACEMODETOOL | GLMENUTOOL );

    /********************************/
    mou = g3dmousetool_new ( TRIANGULATETOOL, 'a', NULL,
                             triangulate_init,
                             NULL, NULL, VIEWVERTEX );
    addMouseTool ( gui, mou, FACEMODETOOL | GLMENUTOOL );

    /********************************/
    mou = g3dmousetool_new ( WELDVERTICESTOOL, 'a', NULL,
                             weldvertices_init,
                             NULL, NULL, VIEWVERTEX );
    addMouseTool ( gui, mou, VERTEXMODETOOL | GLMENUTOOL );

    /********************************/
    mou = g3dmousetool_new ( INVERTNORMALTOOL, 'a', NULL,
                             invertNormal_init,
                             NULL, NULL, VIEWVERTEX );
    addMouseTool ( gui, mou, FACEMODETOOL | GLMENUTOOL );
}

/******************************************************************************/
void setMouseTool ( Widget w, XtPointer client, XtPointer call ) {
    G3DMOUSETOOL *mou = ( G3DMOUSETOOL * ) client;
    G3DUI *gui = NULL;

    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    /*** Remember that widget ID, for example to be unset when a toggle button 
    from another parent widget is called (because XmNradioBehavior won't talk
    to other parent widget ***/
    if ( gui->curmou && w != gui->curmou ) {
        XtVaSetValues ( gui->curmou, XmNset, False, NULL );
    }

    gui->curmou = w;

    /*** We don't check this variable is non-NULL because it must never be ***/
    gui->mou = mou;

    /*** Call the mouse tool initialization function once. This ***/
    /*** can be used by this function to initialize some values ***/
    if ( mou && mou->init ) {
        mou->init ( mou, gui->sce, gui->curcam, gui->urm, gui->flags );
    }

    updateAllCurrentMouseTools ( gui );
}

/******************************************************************************/
void g3dui_updateAllKeyEdits ( G3DUI *gui ) {
    LIST *ltmp = gui->lkeyedit;

    while ( ltmp ) {
        Widget w = ( Widget ) ltmp->data;

        updateKeyEdit ( w );


        ltmp = ltmp->next;
    }
}

/******************************************************************************/
/*** Update the widget bar but do not redraw the openGL window ***/
void updateGLViews ( G3DUI *gui ) {
    LIST *ltmp = gui->lglview;

    while ( ltmp ) {
        Widget w = ( Widget ) ltmp->data;

        /*** Update comboboxes ***/
        updateGLView ( w );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void redrawGLViews ( G3DUI *gui ) {
    LIST *ltmp = gui->lglview;

    while ( ltmp ) {
        Widget w = ( Widget ) ltmp->data;

        if ( XtIsManaged ( w ) ) {
            GViewWidget gw = ( GViewWidget ) w;
            Display *dis = XtDisplay ( gw->gview.ogl );

            XClearArea ( XtDisplay ( gw->gview.ogl ),
                         XtWindow  ( gw->gview.ogl ), 0x00, 0x00, 0x00, 0x00, True );

            XSync ( XtDisplay ( gw->gview.ogl ), False );
        }

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void updateAllMaterialSettings ( G3DUI *gui ) {
    LIST *ltmp = gui->lmatsett;

    while ( ltmp ) {
        Widget w = ( Widget ) ltmp->data;

        updateMaterialSettings ( w, gui->selmat );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void redrawMaterialLists ( G3DUI *gui ) {
    LIST *ltmp = gui->lmatlist;

    while ( ltmp ) {
        Widget w = ( Widget ) ltmp->data;

        XClearArea ( XtDisplay ( w ),
                     XtWindow  ( w ), 0x00, 0x00, 0x00, 0x00, True );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void updateSelectedMaterialPreview ( G3DUI *gui ) {
    LIST *ltmp = gui->lmatlist;
    G3DMATERIAL *mat = gui->selmat;

    if ( mat ) {
        while ( ltmp ) {
            Widget w = ( Widget ) ltmp->data;

            /*updatePreview ( w, mat );*/
XmMaterialListUpdateSelectedPreview ( w );
            /*XClearArea ( XtDisplay ( w ),
                         XtWindow  ( w ), 0x00, 0x00, 0x00, 0x00, True );*/

            ltmp = ltmp->next;
        }
    }
}

/******************************************************************************/
void g3dui_addImage ( G3DUI *gui, G3DIMAGE *img ) {
    list_insert ( &gui->limg, img  );
}

/******************************************************************************/
void g3dui_removeImage ( G3DUI *gui, G3DIMAGE *img ) {
    list_remove ( &gui->limg, img  );
}

/******************************************************************************/
void g3dui_disableTextureImages ( G3DUI *gui ) {
    LIST *ltmpimg = gui->limg;

    while ( ltmpimg ) {
        G3DIMAGE *img = ( G3DIMAGE * ) ltmpimg->data;

        /*** img->id is set to 0x00 is it is not an OpenGL texture ***/
        if ( img->id ) {
            glBindTexture ( GL_TEXTURE_2D, img->id );
            glDisable     ( GL_TEXTURE_2D );
        }

        ltmpimg = ltmpimg->next;
    }
}

/******************************************************************************/
void updateAllCurrentEdit ( G3DUI *gui ) {
    LIST *ltmp = gui->lcuredit;

    while ( ltmp ) {
        Widget w = ( Widget ) ltmp->data;

        updateCurrentEdit ( w );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void updateAllCurrentMouseTools ( G3DUI *gui ) {
    LIST *ltmp = gui->lmtools;

    while ( ltmp ) {
        Widget w = ( Widget ) ltmp->data;

        updateCurrentMouseTool ( w );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void updateCoords ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
    LIST *ltmpcoord = gui->lcoord;

    if ( obj ) {
        while ( ltmpcoord ) {
            Widget w = ( Widget ) ltmpcoord->data;

            updateCoordinatesEdit ( w );


            ltmpcoord = ltmpcoord->next;
        }
    }
}

/******************************************************************************/
static void resizeWidget ( Widget w ) {
    Dimension width, height;
    G3DUI *gui;

    XtVaGetValues ( w, XmNuserData, &gui,
                       XmNwidth, &width,
                       XmNheight, &height,
                       NULL );

    /*** Menu ***/
    gui->menurec.x      = 0x00;
    gui->menurec.y      = 0x00;
    gui->menurec.width  = width;
    gui->menurec.height = 0x20;

    /*** Tool Bar ***/
    gui->tbarrec.x      = 0x00;
    gui->tbarrec.y      = 0x20;
    gui->tbarrec.width  = width;
    gui->tbarrec.height = 0x28;

    /*** Mode Bar ***/
    gui->mbarrec.x      = 0x00;
    gui->mbarrec.y      = 0x48;
    gui->mbarrec.width  = 0x30;
    gui->mbarrec.height = height - 0x48;

    /*** Upper Right Panel ***/
    gui->listrec.x      = width  - 0x140;
    gui->listrec.y      = 0x48;
    gui->listrec.width  = 0x140;
    gui->listrec.height = height - 0x48;

    /*** Quad View Panel ***/
    gui->quadrec.x      = 0x30;
    gui->quadrec.y      = 0x48;
    gui->quadrec.width  = width  - 0x30 - 0x140;
    gui->quadrec.height = height - 0x68;

    /*** Timeboard Panel ***/
    gui->timerec.x      = 0x30;
    gui->timerec.y      = height - 0x20;
    gui->timerec.width  = width  - 0x30 - 0x140;
    gui->timerec.height = 0x18;
}

/******************************************************************************/
void reloadfilesignal ( int signo, siginfo_t *info, void *extra ) {
    void *ptr_val = info->si_value.sival_ptr;
    /*int int_val = info->si_value.sival_int;*/
    G3DUI *gui = ( G3DUI * ) ptr_val;
    R3DNETCLIENT *clt = gui->clt;

    clt->sce = gui->sce = g3duiOpenG3DFile ( gui, clt->filename );

    if ( clt->sce ) {
        Dimension width, height;
        R3DSCENE *rsce;

        XtVaGetValues ( gui->curogl, XmNwidth, &width,
                                     XmNheight, &height,
                                     NULL );

        g3dcamera_view    ( clt->cam, gui->flags );
        g3dcamera_project ( clt->cam, gui->flags );

        /*rsce = r3dscene_render ( clt->sce, clt->cam, 0x00, clt->from,
                                                     clt->resx-1, clt->to,
                                                     width, height,
                                                     NULL );*/

        /*r3dnetclient_sendimage ( clt, rsce->rayimg );*/

        printf ( "Job done\n" );

        /*pthread_detach ( clt->tid );*/
    }
}

/******************************************************************************/
void g3dui_setHourGlass ( G3DUI *gui ) {
    XDefineCursor ( XtDisplay ( gui->top ), 
                    XtWindow  ( gui->top ), gui->watchCursor );

    XFlush ( XtDisplay ( gui->top ) );
}

/******************************************************************************/
void g3dui_unsetHourGlass ( G3DUI *gui ) {
    XUndefineCursor ( XtDisplay ( gui->top ), 
                      XtWindow  ( gui->top ) );
}

/******************************************************************************/
void g3dui_loadFile ( G3DUI *gui, char *filename ) {
    switch ( g3dimport_detectFormat ( filename ) ) {
        case G3DTYPEG3D :
                gui->sce = g3dscene_open ( filename, 0x00 );
        break;

#ifdef HAVE_C4D_H
        case G3DTYPEC4D :
                gui->sce = g3dscene_importC4D ( filename, 0x00 );
        break;
#endif
        case G3DTYPE3DS :
                gui->sce = g3dscene_import3ds ( filename, 0x00 );
        break;
#ifdef HAVE_EXPAT_H
        case G3DTYPEDAE :
                gui->sce = g3dscene_importCollada ( filename, 0x00 );
        break;
#endif
        default :
                gui->sce = g3dscene_new  ( 0x00, "Gloss3D scene" );
        break;
    }
}

/******************************************************************************/
static XtRealizeProc Realize ( Widget w, XtValueMask *value_mask,
                               XSetWindowAttributes *attributes ) {
    GUiWidget guw = ( GUiWidget ) w;
    Display *dis = XtDisplay ( w );
    Pixel background, foreground;
    Dimension width, height;
    G3DUI *gui = &guw->gui;
    XColor scrcol, exacol;
    Widget tab, quad, matlist;
    GTABITEM tabitem;
    Colormap cmap;
    int depth;
    struct sigaction action;
    G3DUIRENDERSETTINGS *rsg;
    char **fonts;
    int nbfont, nbfontdir = 0x01, i;

    action.sa_flags     = SA_SIGINFO; 
    action.sa_sigaction = g3dui_sighandler;

    g3dui_sighandler ( 0x00, NULL, gui ); /*** Dirty - init static variable ***/

    if ( sigaction ( SIGUSR1, &action, NULL ) == -1 ) { 
        perror ( "sigusr: sigaction" );

        _exit(1);
    }

    /*** this is required if we want to inherit the event mask. ***/
    /*** Redisplay procedure would not be called otherwise      ***/
    xmManagerClassRec.core_class.realize ( w, value_mask, attributes );

    XtVaGetValues ( w, XmNcolormap, &cmap, NULL );
    XAllocNamedColor ( dis, cmap, BACKGROUNDCOLOR, &scrcol, &exacol );

    XtVaSetValues ( w, XmNbackground, scrcol.pixel,
                       XmNuserData, gui,
                       NULL );

    gui->top = XtParent ( w );

    gui->lobjmenu  = NULL;
    gui->lvermenu  = NULL;
    gui->ledgmenu  = NULL;
    gui->lfacmenu  = NULL;
    gui->lmtools   = NULL;
    gui->lglview   = NULL;
    gui->lobjlist  = NULL;
    gui->lmatlist  = NULL;
    gui->lcoord    = NULL;
    gui->lcuredit  = NULL;

    gui->watchCursor = XCreateFontCursor ( dis, XC_watch );

    /*fonts = XListFonts ( dis, "*", 2048, &nbfont );

    for ( i = 0x00; i < nbfont; i++ ) {
        printf ( "Font: %s\n", fonts[i] );
    }*/

    /*** undo redo manager ***/
    gui->urm = g3durmanager_new ( );

    /*** copy-paste manager ***/
    gui->cli = g3duiclipboard_new ( );

    gui->tim = g3duitimeline_new ( 24, 24 );

    gui->flags = ( VIEWOBJECT | XAXIS | YAXIS | ZAXIS );

    rsg = g3duirendersettings_new ( );

    addRenderSettings ( w, rsg );

    gui->currsg = rsg; /*** default render settings ***/

    createRenderTable ( w );
    createFonts ( w );

    resizeWidget ( w );

    quad = createQuad ( w, &guw->gui, QUADNAME,  gui->quadrec.x,
                                                 gui->quadrec.y,
                                                 gui->quadrec.width,
                                                 gui->quadrec.height );

    createDefaultViews ( quad, &guw->gui );

    if ( gui->loadFile ) {
        g3dui_loadFile ( gui, gui->loadFile );

        g3duiSetFileName ( gui, gui->loadFile );
    } else {
        gui->sce = g3dscene_new  ( 0x00, "Gloss3D scene" );
    }

    createMenuBar ( w, &guw->gui, MENUBARNAME, gui->menurec.x,
                                               gui->menurec.y,
                                               gui->menurec.width,
                                               gui->menurec.height );

    createToolBar ( w, &guw->gui, TOOLBARNAME, gui->tbarrec.x,
                                               gui->tbarrec.y,
                                               gui->tbarrec.width,
                                               gui->tbarrec.height );

    createModeBar ( w, &guw->gui, MODEBARNAME, gui->mbarrec.x,
                                               gui->mbarrec.y,
                                               gui->mbarrec.width,
                                               gui->mbarrec.height );

    tab = XmVaCreateManagedTabbedBook ( w, OBJECTBOARDNAME,
                                           XmNx, gui->listrec.x,
                                           XmNy, gui->listrec.y,
                                           XmNwidth, gui->listrec.width,
                                           XmNheight, gui->listrec.height,
                                           XmNfontList, gui->fontlist,
                                           NULL );


    createMaterialBoard ( tab, &guw->gui, "Material Board", 0x00, 0x00, 0x140, 0x300 );
    createObjectBoard   ( tab, &guw->gui, "Object Board", 0x00, 0x00, 0x140, 0x300 );

    createTimeboard ( w, &guw->gui, TIMEBOARDNAME,  gui->timerec.x,
                                                    gui->timerec.y,
                                                    gui->timerec.width,
                                                    gui->timerec.height );
 


    /*** Add mouse tools AFTER GLViews creation ***/
    setDefaultMouseTools ( &guw->gui );
    
    /*** Called after material widget is created ***/
    g3dui_importMaterials ( gui );


    return ( XtRealizeProc ) NULL;
}

/******************************************************************************/
static XtWidgetProc Destroy ( Widget w ) {
    GUiWidget guw = ( GUiWidget ) w;
    Display *dis = XtDisplay ( w );
    G3DUI *gui = &guw->gui;

    XFreeFont ( dis, gui->nrmlft );
    XFreeFont ( dis, gui->boldft );
    XFreeFont ( dis, gui->tinyft );

    XmRenderTableFree ( gui->renTable );


    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
/*** This handles children widget resizing. I don't use XmForm Attachment   ***/
/*** because I find it not very convenient for Windows with many children.  ***/
static XtWidgetProc Resize ( Widget w ) {
    Dimension width, height;
    WidgetList children;
    G3DUI *gui;
    Cardinal nc;
    int i;

    XtVaGetValues ( w, XmNchildren, &children,
                       XmNnumChildren, &nc,
                       XmNwidth, &width,
                       XmNheight, &height,
                       XmNuserData, &gui,
                       NULL );

    resizeWidget ( w );

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];
        char *name = XtName ( child );
        XRectangle *rec = NULL;

        if ( strcmp ( name, MENUBARNAME     ) == 0x00 ) rec = &gui->menurec;
        if ( strcmp ( name, TOOLBARNAME     ) == 0x00 ) rec = &gui->tbarrec;
        if ( strcmp ( name, MODEBARNAME     ) == 0x00 ) rec = &gui->mbarrec;
        if ( strcmp ( name, QUADNAME        ) == 0x00 ) rec = &gui->quadrec;
        if ( strcmp ( name, TIMEBOARDNAME   ) == 0x00 ) rec = &gui->timerec;
        if ( strcmp ( name, OBJECTBOARDNAME ) == 0x00 ) rec = &gui->listrec;
        if ( strcmp ( name, BOTTOMBOARDNAME ) == 0x00 ) rec = &gui->proprec;

        if ( rec && rec->width && rec->height ) {
            XtConfigureWidget ( child, rec->x    , rec->y,
                                       rec->width, rec->height, 0x00 );
        }
    }

    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
static XtSetValuesFunc set_values ( Widget current, Widget request, Widget set,
                                    ArgList args, Cardinal *numargs ) {

    return 0x00;
}

/*****************************************************************************/
void g3duiSetFileName( G3DUI *gui, char *filename ) {
    int len;

    if ( gui->filename ) {
        free ( gui->filename );

        gui->filename = NULL;
    }

    len = strlen ( filename );

    gui->filename = ( char * ) calloc ( len + 0x01, sizeof ( char ) );

    if ( gui->filename == NULL ) {
        fprintf ( stderr, "g3duiSaveG3DFile: calloc failed\n" );
    } else {
        memcpy ( gui->filename, filename, len );
    }
}

/******************************************************************************/
void g3duiSaveG3DFile ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;

    g3dscene_write ( sce, gui->filename, "Made with GLOSS-3D", 0x00 );
}

/******************************************************************************/
G3DSCENE *g3duiLoadFileSignal ( G3DUI *gui, char *filename ) {

}

#ifdef HAVE_EXPAT_H
/******************************************************************************/
G3DSCENE *g3duiOpenDAEFile ( G3DUI *gui, char *filename ) {
    if ( access( filename, F_OK ) == 0x00 ) {
        printf ( "Opening %s ...\n", filename );

        /*** free memory the previous scene ***/
        if ( gui->sce ) {
            g3dobject_free ( ( G3DOBJECT * ) gui->sce );
        }

        g3dui_setHourGlass ( gui );

        gui->sce = g3dscene_importCollada ( filename, gui->flags );

        g3dui_unsetHourGlass ( gui );

        if ( gui->sce ) {
            g3duiSetFileName ( gui, filename );

            printf ( "...Done!\n", gui->filename );

            return gui->sce;
        } else {
            return g3dscene_new ( 0x00, "Gloss3D scene" );
        }
    }

    return NULL;
}
#endif

/******************************************************************************/
G3DSCENE *g3duiOpenOBJFile ( G3DUI *gui, char *filename ) {
    if ( access( filename, F_OK ) == 0x00 ) {
        printf ( "Opening %s ...\n", filename );

        g3durmanager_clear ( gui->urm );

        /*** free memory the previous scene ***/
        if ( gui->sce ) {
            g3dobject_free ( ( G3DOBJECT * ) gui->sce );
        }

        g3dui_setHourGlass ( gui );

        gui->sce = g3dscene_importObj ( filename, gui->flags );

        g3dui_unsetHourGlass ( gui );

        if ( gui->sce  ) {
            g3duiSetFileName ( gui, filename );

            printf ( "...Done!\n", gui->filename );

            return gui->sce;
        } else {
            return g3dscene_new ( 0x00, "Gloss3D scene" );
        }
    }

    return NULL;
}

/******************************************************************************/
G3DSCENE *g3duiOpen3DSFile ( G3DUI *gui, char *filename ) {
    if ( access( filename, F_OK ) == 0x00 ) {
        printf ( "Opening %s ...\n", filename );

        g3durmanager_clear ( gui->urm );

        /*** free memory the previous scene ***/
        if ( gui->sce ) {
            g3dobject_free ( ( G3DOBJECT * ) gui->sce );
        }

        g3dui_setHourGlass ( gui );

        gui->sce = g3dscene_import3ds ( filename, gui->flags );

        g3dui_unsetHourGlass ( gui );

        if ( gui->sce  ) {
            g3duiSetFileName ( gui, filename );

            printf ( "...Done!\n", gui->filename );

            return gui->sce;
        } else {
            return g3dscene_new ( 0x00, "Gloss3D scene" );
        }
    }

    return NULL;
}

/******************************************************************************/
#ifdef HAVE_C4D_H
G3DSCENE *g3duiOpenC4DFile ( G3DUI *gui, char *filename ) {
    if ( access( filename, F_OK ) == 0x00 ) {
        printf ( "Opening %s ...\n", filename );

        g3durmanager_clear ( gui->urm );

        /*** free memory the previous scene ***/
        if ( gui->sce ) {
            g3dobject_free ( gui->sce );
        }

        g3dui_setHourGlass ( gui );

        gui->sce = g3dscene_importC4D ( filename );

        g3dui_unsetHourGlass ( gui );

        if ( gui->sce ) {
            g3duiSetFileName ( gui, filename );

            printf ( "...Done!\n", gui->filename );

            return gui->sce;
        } else {
            return g3dscene_new ( 0x00, "Gloss3D scene" );
        }
    }

    return NULL;
}
#endif
/******************************************************************************/
/*** Create the material previews, typically after loading a scene file ***/
void g3dui_importMaterials ( G3DUI *gui ) {
    LIST *ltmpmat = gui->sce->lmat;

    while ( ltmpmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpmat->data;
        LIST *ltmpmatlist = gui->lmatlist;

        while ( ltmpmatlist ) {
            Widget w = ( Widget ) ltmpmatlist->data;

            XmMaterialListAddMaterial ( w, mat->name, mat );

            ltmpmatlist = ltmpmatlist->next;
        }

        ltmpmat = ltmpmat->next;
    }
}

/******************************************************************************/
/*** Create the material previews, typically after loading a scene file ***/
void g3dui_clearMaterials ( G3DUI *gui ) {
    LIST *ltmpmat = gui->sce->lmat;

    while ( ltmpmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpmat->data;
        LIST *ltmpmatlist = gui->lmatlist;

        while ( ltmpmatlist ) {
            Widget w = ( Widget ) ltmpmatlist->data;

            freePreviews ( w );

            ltmpmatlist = ltmpmatlist->next;
        }

        ltmpmat = ltmpmat->next;
    }
}

/******************************************************************************/
G3DSCENE *g3duiOpenG3DFile ( G3DUI *gui, char *filename ) {
    if ( access( filename, F_OK ) == 0x00 ) {
        printf ( "Opening %s ...\n", filename );

        g3durmanager_clear ( gui->urm );

        /*** free memory the previous scene ***/
        if ( gui->sce ) {
            g3dobject_free ( ( G3DOBJECT * ) gui->sce );
        }

        g3dui_setHourGlass ( gui );

        gui->sce = g3dscene_open ( filename, gui->flags );

        g3dui_unsetHourGlass ( gui );

        if ( gui->sce ) {
            g3duiSetFileName ( gui, filename );

            g3dui_clearMaterials ( gui );

            g3dui_importMaterials ( gui );

            printf ( "...Done!\n", gui->filename );

            return gui->sce;
        } else {
            return g3dscene_new ( 0x00, "Gloss3D scene" );
        }
    }

    return NULL;
}

/******************************************************************************/
/*void XmGuiOpenG3DFile ( Widget w, char *filename ) {
    GUiWidget guw = ( GUiWidget ) w;
    G3DUI *gui = &guw->gui;
    G3DSCENE *sce = NULL;

    g3duiOpenG3DFile ( gui, filename );
}*/

/******************************************************************************/
GUiClassRec guiClassRec = { { ( WidgetClass ) &xmManagerClassRec,/*parent*/
                                  "G3DUI",                      /* class_name */
                                  sizeof ( GUiRec ),           /* widget size */
                       ( XtProc ) ClassInitialize,        /* class_initialize */
                                  NULL,              /* class_part_initialize */
                                  FALSE,                      /* class_inited */
                   ( XtInitProc ) Initialize,                   /* initialize */
                                  NULL,                    /* initialize_hook */
                ( XtRealizeProc ) Realize,                         /* realize */
                                  NULL,                            /* actions */
                                  0x00,                        /* num_actions */
                                  widgetRes,                      /* resouces */
                                  XtNumber ( widgetRes ),    /* num_resources */
                                  NULLQUARK,                      /*xrm_class */
                                  TRUE,                    /* compress_motion */
                                  XtExposeCompressMaximal,/*compress_exposure */
                                  TRUE,                /* compress_enterleave */
                                  FALSE,                  /* visible_interest */
                ( XtWidgetProc )  Destroy,                          /*destroy */
                ( XtWidgetProc )  Resize,                           /* resize */
                ( XtExposeProc )  NULL,                             /* expose */
             ( XtSetValuesFunc )  set_values,                   /* set_values */
                                  NULL,                    /* set_values_hook */
                                  XtInheritSetValuesAlmost,/*set_values_almost*/
                                  NULL,                    /* get_values_hook */
                                  NULL,                       /* accept_focus */
                                  XtVersion,                       /* version */
                                  NULL,                   /* callback_offsets */
                                  NULL,                           /* tm_table */
                                  XtInheritQueryGeometry,   /* query_geometry */
                                  NULL,                /* display_accelerator */
                                  NULL },                        /* extension */
                                  /* Composite class part */
                                { XtInheritGeometryManager,
                                  XtInheritChangeManaged,
                                  XtInheritInsertChild,
                                  XtInheritDeleteChild,
                                  NULL }
                                  /* Constraint class part */
                                ,{ NULL,
                                  0x00,
                                  0x00,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL },
                                  /* XmManager class part */
                                {
/* translations                 */ XtInheritTranslations,
/* syn_resources                */ NULL,
/* num_syn_resources            */ 0x00,
/* syn_constraint_resources     */ NULL,
/* num_syn_constraint_resources */ 0x00,
/* parent_process               */ XmInheritParentProcess,
/* extension                    */ NULL } };

WidgetClass guiWidgetClass = ( WidgetClass ) &guiClassRec;
