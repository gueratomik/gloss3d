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
#include <g3dui.h>
#include <g3dui_motif.h>

/******************************************************************************/
#include <xpm/matedit.xpm>

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
static uint32_t cleanMe ( R3DFILTER *fil, R3DSCENE *rsce, 
                                          unsigned char *img, 
                                          uint32_t from, 
                                          uint32_t to, 
                                          uint32_t depth, 
                                          uint32_t width ) {
    G3DUI *gui = ( G3DUI * ) fil->data;
    G3DUIRENDERPROCESS *rps = common_g3dui_getRenderProcessByScene ( gui, rsce );

    /*** clean renderprocess if any ***/
    if ( rps ) {
        g3duirenderprocess_free ( rps );

        list_remove ( &gui->lrps, rps );

        return 0x01;
    }

    return 0x00;
}

/******************************************************************************/
G3DUIRENDERPROCESS *g3dui_rendertoarea ( G3DUI *gui, uint32_t x1, uint32_t y1,
                                                     uint32_t x2, uint32_t y2,
                                                     Widget area,
                                                     uint32_t free_after ) {
    G3DUIRENDERSETTINGS *rsg = ( G3DUIRENDERSETTINGS * ) gui->currsg;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    G3DCAMERA *cam = gui->curcam;
    Dimension width, height;
    G3DSCENE *sce = gui->sce;
    Display *dis = XtDisplay ( area );
    Window win = XtWindow ( area );

    XtVaGetValues ( area, XmNwidth , &width,
                          XmNheight, &height,
                          NULL );

    /*** Don't start a new render before the current one has finished ***/
    /*if ( rpc == NULL ) {*/
        /*** this filter is used for displaying ***/
        R3DFILTER *towin = r3dfilter_toWindow_new ( dis, win, 0x01 );
        /*** This filter is used for saving images ***/
        R3DFILTER *tobuf = r3dfilter_toBuffer_new ( width, height, 
                                                           rsg->depth, 
                                                           rsg->background );
        /*** Filter to free R3DSCENE, Filters & G3DUIRENDERPROCESS ***/
        R3DFILTER *clean = ( free_after ) ? r3dfilter_new ( FILTERIMAGE, 
                                                            "CLEAN",  
                                                            cleanMe,
                                                            NULL, 
                                                            gui ) : NULL;
        G3DUIRENDERPROCESS *rps;
        LIST *lfilters  = NULL;
        R3DSCENE *rsce  = NULL;
        pthread_attr_t attr;
        pthread_t tid;

        /*r3dfilter_setType ( towin, FILTERLINE );*/

                     list_append ( &lfilters, tobuf );
                     list_append ( &lfilters, towin );
        if ( clean ) list_append ( &lfilters, clean );

        pthread_attr_init ( &attr );

        /*** start thread son all CPUs ***/
        pthread_attr_setscope ( &attr, PTHREAD_SCOPE_SYSTEM );

        rsce = r3dscene_new ( sce, cam, x1, y1,
                                        x2, y2,
                                        width, height,
                                        rsg->background,
                                        rsg->startframe,
                                        rsg->endframe,
                                        lfilters );

        /*** launch rays in a thread ***/
        pthread_create ( &rsce->tid, &attr,(void*(*)(void*))r3dscene_render_frame_t, rsce );

        /*** Remember the thread id for cancelling on mouse input e.g ***/
        /*** We use the widget as an ID ***/
        rps = g3duirenderprocess_new ( ( uint64_t ) area, rsce, towin, tobuf );

        /*** register the renderprocess so that we can cancel it ***/
        list_insert ( &gui->lrps, rps );

        /*** prepare to release resources after thread termination ***/
        /*pthread_detach ( tid );*/
    /*}*/

    return rps;
}

/******************************************************************************/
G3DUIMOTIF *g3duimotif_new ( ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) calloc ( 0x01, sizeof ( G3DUIMOTIF ) );

    if ( gmt == NULL ) { 
        fprintf ( stderr, "g3duimotif_new: malloc failed\n" );

        return NULL;
    }

    return gmt;
}

/******************************************************************************/
void g3duimotif_free ( G3DUIMOTIF *gmt ) {
    free ( gmt );
}

/******************************************************************************/
static void createRenderTable ( Widget w ) {
    GUiWidget guw = ( GUiWidget ) w;
    G3DUI *gui = &guw->gui;
    XmRendition rendition;
    Cardinal n = 0x00;
    Arg args[] = { { XmNfontName , ( XtArgVal ) G3DFONT         },
                 /*{ XmNloadModel, ( XtArgVal ) XmLOAD_DEFERRED },*/
                   { XmNfontType , ( XtArgVal ) XmFONT_IS_FONT  } };
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    rendition = XmRenditionCreate ( gmt->top, "Gloss3DFont", args, XtNumber ( args ) );

    gmt->renTable = XmRenderTableAddRenditions ( NULL, &rendition,
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
static void createFonts ( Widget w ) {
    Display *dis = XtDisplay ( w );
    GUiWidget guw = ( GUiWidget ) w;
    G3DUI *gui = &guw->gui;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    /*** Create the normal font ***/
    if ( ( gmt->nrmlft = XLoadQueryFont ( dis, G3DFONT ) ) == NULL ) {
        fprintf ( stderr, "Cannot create %s, switching to 6x13\n", G3DFONT );

        /*** Switch to the default font ***/ 
        if ( ( gmt->nrmlft = XLoadQueryFont ( dis, "6x13" ) ) == NULL ) {
            fprintf ( stderr, "Cannot create 6x13 font\n" );

            return;
        }
    }

    /*** I create a Motif font here because the other ones will be used ***/
    /*** by Xlib calls, so I'll directly use the XFontStruct, not the   ***/
    /*** Motif fontlist ( that I don't fully understand by the way ).   ***/
    XmFontList fontlist = XmFontListCreate ( gmt->nrmlft,
                                             XmSTRING_DEFAULT_CHARSET );
    if ( fontlist == NULL ) {
        fprintf ( stderr, "XmFontListCreate failed\n", G3DFONT );

        return;
    }

    gmt->fontlist = fontlist;

    /*** Create the bold font ***/
    if ( ( gmt->boldft = XLoadQueryFont ( dis, G3DBOLDFONT ) ) == NULL ) {
        fprintf ( stderr, "Cannot create %s, switching to 6x13bold\n", G3DBOLDFONT);

        /*** switch to default font ***/
        if ( ( gmt->boldft = XLoadQueryFont ( dis, "6x13bold" ) ) == NULL ) {
            XFreeFont ( dis, gmt->nrmlft );

            fprintf ( stderr, "Cannot create 6x13bold font\n" );

            return;
        }
    }

    /*** Create the tiny font **/
    if ( ( gmt->tinyft = XLoadQueryFont ( dis, G3DTINYFONT ) ) == NULL ) {
        fprintf ( stderr, "Cannot create %s, switching to 6x9\n", G3DTINYFONT );

        /*** switch to the default font ***/
        if ( ( gmt->tinyft = XLoadQueryFont ( dis, "6x9" ) ) == NULL ) {
            XFreeFont ( dis, gmt->nrmlft );
            XFreeFont ( dis, gmt->boldft );

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
void createPushButton  ( Widget parent, G3DUI *gui,
                                        char *name,
                                        Dimension x, 
                                        Dimension y,
                                        Dimension width, 
                                        Dimension height,
                                        void (*cbk)( Widget, XtPointer, 
                                                             XtPointer ) ) {
    createSimplePushButton ( parent, gui, name, x, y, width, height, cbk );
}

/******************************************************************************/
void createSimplePushButton  ( Widget parent, G3DUI *gui,
                                              char *name,
                                              Dimension x,
                                              Dimension y,
                                              Dimension width, 
                                              Dimension height,
                                              void (*cbk)( Widget, XtPointer, 
                                                                   XtPointer ) ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget btn;

    XtVaGetValues ( parent, XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    btn = XmVaCreateManagedPushButton ( parent, name,
                                        XmNx, x,
                                        XmNy, y,
                                        XmNwidth , width,
                                        XmNheight, height,
                                        XmNfontList, gmt->fontlist,
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
                                        NULL );

    if ( cbk ) XtAddCallback ( btn, XmNactivateCallback, cbk, gui );
}

/******************************************************************************/
Widget createImageButton ( Widget parent, G3DUI *gui,
                                          char *name, 
                                          char **data,
                                          Dimension x, 
                                          Dimension y,
                                          Dimension width,
                                          Dimension height,
                                          void (*func)( Widget, XtPointer,
                                                                XtPointer ) ){
    Pixel background, foreground;
    Pixmap pixnor, pixarm;
    Widget btn;
    int depth;

    XtVaGetValues ( parent, XmNdepth, &depth,
                            XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    createTransparentPixmap ( XtDisplay ( parent ), &pixnor,
                                                    &pixarm,
                                                    data,
                                                    depth,
                                                    background, 
                                                    foreground );

    btn = XmVaCreateManagedPushButton ( parent, name,
                                        XtNx, x,
                                        XtNy, y,
                                        XtNwidth , width,
                                        XtNheight, height,
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


    return btn;
}


/******************************************************************************/
void createSimpleLabel ( Widget parent, G3DUI *gui, char *name,
                         Dimension x, Dimension y,
                         Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget lab;
    G3DUIMOTIF *gmt;

    XtVaGetValues ( parent, XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    lab = XmVaCreateManagedLabel ( parent, name,
                                   XmNx, x,
                                   XmNy, y,
                                   XmNwidth , width,
                                   XmNheight, height,
                                   XmNfontList, gmt->fontlist,
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
XImage *createXImageFromData ( Display *dis, uint32_t width, 
                                             uint32_t height,
                                             uint32_t depth,
                                             unsigned char (*data)[3] ) {
    XImage *img = XCreateImage ( dis, CopyFromParent, 
                                      depth,
                                      ZPixmap, 
                                      0, 
                                      NULL, 
                                      width, 
                                      height, 
                                      0x20, 
                                      0x00 );
    uint32_t i, j;

    if ( img == NULL ) {
        fprintf ( stderr, "createXImageFromData failed\n" );

        return NULL;
    }

    img->data = calloc ( img->height, img->bytes_per_line );

    XInitImage ( img );

    for ( i = 0x00; i < img->height; i++ ) {
        for ( j = 0x00; j < img->width; j++ ) {
            uint32_t r = data[(i*width)+j][0],
                     g = data[(i*width)+j][1],
                     b = data[(i*width)+j][2];

                     img->f.put_pixel ( img, j, i, ( r << 0x10 ) | ( g << 0x08 ) | b );
        }
    }

    return img;
}

/******************************************************************************/
static void colorButtonDestroyCbk ( Widget w, XtPointer client, XtPointer call ) {

}

/******************************************************************************/
static void colorButtonSetColorCbk ( Widget w, XtPointer client, 
                                               XtPointer call ) {
    XmColorEditCallbackStruct *ces = ( XmColorEditCallbackStruct * ) call;
    Widget btn = ( Widget ) client;

    XtVaSetValues ( btn, XmNbackground, ces->pixel, NULL );
}

/******************************************************************************/
static void colorButtonPushCbk ( Widget w, XtPointer client, XtPointer call ) {
    Display *dis = XtDisplay ( w );
    Window root = XDefaultRootWindow ( dis );
    XWindowAttributes wat;
    Widget shell, edit;
    void *userData;
    void (*cbk)( Widget, XtPointer, XtPointer );
    Pixel color;

    XtVaGetValues ( w, XmNbackground, &color, 
                       XmNuserData, &cbk, NULL );

    XGetWindowAttributes ( dis, root, &wat );

    shell = XtVaAppCreateShell ( NULL, "Class",
                                 topLevelShellWidgetClass,
                                 dis, 
                                 XtNtitle, "Choose Color",
                                 XtNx, ( wat.width  / 2 ) -200,
                                 XtNy, ( wat.height / 2 ) -160,
                                 XtNwidth, 400,
                                 XtNheight, 320,
                                 NULL );

    edit = XtVaCreateWidget ( "Color Edit",
                              xmColorEditWidgetClass,
                              shell,
                              XtNx, 0x00,
                              XtNy, 0x00,
                              XtNwidth, 400,
                              XtNheight, 320,
                              XtNcolor, color,
                              NULL );

    if ( cbk ) {
        XtAddCallback ( edit, XmNcolorChangingCallback, cbk, client );
        XtAddCallback ( edit, XmNcolorChangingCallback, colorButtonSetColorCbk, w      );
    }

    XtManageChild ( edit );

    XtRealizeWidget ( shell );
}

/******************************************************************************/
Widget createColorButton ( Widget parent, G3DUI *gui, 
                                          char *name, 
                                          Dimension x, 
                                          Dimension y,
                                          Dimension width,
                                          Dimension height,
                                          void (*cbk)( Widget, XtPointer, 
                                                               XtPointer ) ) {
    Pixel black = BlackPixel ( XtDisplay ( parent ), 0x00 );
    Arg args[] = { { XmNx                 , ( XtArgVal ) x        },
                   { XmNy                 , ( XtArgVal ) y        },
                   { XmNwidth             , ( XtArgVal ) width    },
                   { XmNheight            , ( XtArgVal ) height   },
                   { XmNlabelType         , ( XtArgVal ) XmPIXMAP },
                   { XmNuserData          , ( XtArgVal ) cbk      },
                   { XmNhighlightThickness, ( XtArgVal ) 0x00     },
                   { XmNshadowThickness   , ( XtArgVal ) 0x00     },
                   { XmNborderWidth       , ( XtArgVal ) 0x01     },
                   { XmNborderColor       , ( XtArgVal ) black    } };

    Widget btn = XmCreatePushButton ( parent, name, args, XtNumber ( args ) );

    XtAddCallback ( btn, XmNactivateCallback, colorButtonPushCbk   , gui );
    XtAddCallback ( btn, XmNdestroyCallback , colorButtonDestroyCbk, gui );


    XtManageChild ( btn );


    return btn;
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
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

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
                                   NULL );

    lessbtn = XmVaCreateManagedPushButton ( frm, "-",
                                                 XmNx, 0x00,
                                                 XmNy, 0x00,
                                                 XmNwidth,  0x20,
                                                 XmNheight, height,
                                                 XmNfontList, gmt->fontlist,
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
                                                 NULL );

    textbtn = XmVaCreateManagedText ( frm, name,
                                           XmNx, 0x20,
                                           XmNy, 0x00,
                                           XmNwidth , 0x20,
                                           XmNheight, height,
                                           XmNfontList, gmt->fontlist,
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

    /*** check input ***/
    XtAddCallback ( textbtn, XmNmodifyVerifyCallback, verifyintcbk, NULL );

    morebtn = XmVaCreateManagedPushButton ( frm, "+",
                                                 XmNx, 0x40,
                                                 XmNy, 0x00,
                                                 XmNwidth,  0x20,
                                                 XmNheight, height,
                                                 XmNfontList, gmt->fontlist,
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
                                                 NULL );

    if ( txtcbk ) XtAddCallback ( textbtn, XmNvalueChangedCallback, txtcbk, gui );
    if ( deccbk ) XtAddCallback ( lessbtn, XmNarmCallback         , deccbk, gui );
    if ( inccbk ) XtAddCallback ( morebtn, XmNarmCallback         , inccbk, gui );

    XtManageChild ( frm );


    return frm;
}

/******************************************************************************/
void createRadioLabel ( Widget parent, G3DUI *gui,
                                        char *name,
                                        Dimension x, 
                                        Dimension y,
                                        Dimension width, 
                                        Dimension height,
                                        void (*cbk)( Widget, XtPointer, 
                                                             XtPointer ) ) {
    createToggleLabel ( parent, gui, name, x, y, width, height, cbk );
}

/******************************************************************************/
void createToggleLabel ( Widget parent, G3DUI *gui,
                                        char *name,
                                        Dimension x, 
                                        Dimension y,
                                        Dimension width, 
                                        Dimension height,
                                        void (*cbk)( Widget, XtPointer, 
                                                             XtPointer ) ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget btn;

    XtVaGetValues ( parent, XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    btn = XmVaCreateManagedToggleButton ( parent, name,
                                          XmNx, x,
                                          XmNy, y,
                                          XmNwidth,  width,
                                          XmNheight, height,
                                          XmNfontList, gmt->fontlist,
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
                                          NULL );

    if ( cbk ) XtAddCallback ( btn, XmNarmCallback, cbk, gui );
}

/******************************************************************************/
Widget createFrame ( Widget parent, G3DUI *gui,
                                    char *name,
                                    Dimension x, Dimension y,
                                    Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget frm, form;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

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
                                           NULL );

    XmVaCreateManagedLabel ( frm, name,
                                  XtNx, 0x00,
                                  XtNy, 0x00,
                                  XmNwidth , 0x60,
                                  XmNheight, 0x20,
                                  XmNfontList, gmt->fontlist,
                                  XmNframeChildType, XmFRAME_TITLE_CHILD,
                                  XmNforeground, foreground,
                                  XmNbackground, background,
                                  NULL );

    form = XmVaCreateManagedForm ( frm, name,
                                   XtNx, 0,
                                   XtNy, 0,
                                   XtNwidth , width,
                                   XtNheight, height,
                                   XmNforeground, foreground,
                                   XmNbackground, background,
                                   XmNborderWidth, 0x00,
                                   NULL );

    return form;
}

/******************************************************************************/
Widget createBoard ( Widget parent, G3DUI *gui,
                                    char *name,
                                    Dimension x, Dimension y,
                                    Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget brd;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    XtVaGetValues ( parent, XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    brd = XmVaCreateBulletinBoard ( parent, name,
                                            XtNx, x,
                                            XtNy, y,
                                            XmNwidth , width,
                                            XmNheight, height,
                                            XmNfontList, gmt->fontlist,
                                            XmNframeChildType, XmFRAME_GENERIC_CHILD,
                                            XmNforeground, foreground,
                                            XmNbackground, background,
XmNresizePolicy, XmRESIZE_NONE,
XmNdefaultPosition, False,
                                            NULL );

    XtManageChild ( brd );


    return brd;
}

/******************************************************************************/
Widget createHorizontalScale ( Widget parent, G3DUI *gui, 
                                              char *name,
                                              Dimension x, 
                                              Dimension y,
                                              Dimension width, 
                                              Dimension height,
                                              int min, int max, int step,
                                              void (*cbk)( Widget, XtPointer, 
                                                                   XtPointer ) ) {
    Widget sca;
    XmString title = XmStringCreateLocalized ( name );
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    sca = XmVaCreateManagedScale ( parent, name,
                                        XmNx, x,
                                        XmNy, y,
                                        XmNwidth, width,
                                        XmNheight, height,
                                        XmNorientation, XmHORIZONTAL,
                                        XmNshowValue, True,
                                        XmNshadowThickness, 0x01,
                                        XmNdecimalPoints, 0x00,
                                        XmNhighlightOnEnter, False,
                                        XmNminimum, min,
                                        XmNmaximum, max,
                                        XmNtitleString, title,
                                        XmNfontList, gmt->fontlist,
                                        NULL );

    if ( cbk ) {
        XtAddCallback ( sca, XmNdragCallback, cbk, gui );
    }

    XmStringFree ( title );

    XtManageChild ( sca );

    return sca;
}

/******************************************************************************/
void createRadioButton ( Widget parent, G3DUI *gui, char *name,
                         Dimension x, Dimension y,
                         Dimension width, Dimension height,
                         void (*armcbk)( Widget, XtPointer, XtPointer ) ) {
    Pixel background, foreground;
    Widget btn;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    XtVaGetValues ( parent, XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    btn = XmVaCreateManagedToggleButton ( parent, name,
                                          XmNx, x,
                                          XmNy, y,
                                          XmNwidth,  width,
                                          XmNheight, height,
                                          XmNfontList, gmt->fontlist,
                                          XmNindicatorOn, True,
                                          XmNrecomputeSize, False,
                                          XmNhighlightThickness, 0x00,
                                          XmNshadowThickness, 0x01,
                                          XmNmarginHeight, 0x00,
                                          XmNmarginWidth, 0x00,
                                          XmNbackground, background,
                                          XmNforeground, foreground,
                                          NULL );

    if ( armcbk ) XtAddCallback ( btn, XmNarmCallback, armcbk, gui );
}

/******************************************************************************/
void createToggleButton ( Widget parent, G3DUI *gui, char *name,
                          Dimension x, Dimension y,
                          Dimension width, Dimension height,
                          void (*armcbk)( Widget, XtPointer, XtPointer ) ) {
    Pixel background, foreground;
    Widget btn;
    G3DUIMOTIF *gmt;

    XtVaGetValues ( parent, XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    btn = XmVaCreateManagedToggleButton ( parent, name,
                                          XmNx, x,
                                          XmNy, y,
                                          XmNwidth,  width,
                                          XmNheight, height,
                                          XmNfontList, gmt->fontlist,
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
                                          NULL );

    if ( armcbk ) XtAddCallback ( btn, XmNarmCallback, armcbk, gui );
}

/******************************************************************************/
void createOrientationSelection ( Widget parent, G3DUI *gui, 
                                                 char *name,
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
    Widget lab;
    Widget sel;
    G3DUIMOTIF *gmt;

    XtVaGetValues ( parent, XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    dmlist[0x00] = XmStringCreate ( ZXSTR, XmFONTLIST_DEFAULT_TAG );
    dmlist[0x01] = XmStringCreate ( XYSTR, XmFONTLIST_DEFAULT_TAG );
    dmlist[0x02] = XmStringCreate ( YZSTR, XmFONTLIST_DEFAULT_TAG );

    lab = XmVaCreateManagedLabel ( parent, name,
                                   XmNx, x,
                                   XmNy, y,
                                   XmNwidth , labwidth,
                                   XmNheight, 0x12,
                                   XmNfontList, gmt->fontlist,
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
                                       XmNfontList, gmt->fontlist,
                                       XmNarrowSize, 0x0C,
                                       XmNcomboBoxType, XmDROP_DOWN_LIST,
                                       XmNrenderTable, gmt->renTable,
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
void createFloatText ( Widget parent, G3DUI *gui,
                                      char *name,
                                      Dimension x, Dimension y,
                                      Dimension labwidth,
                                      Dimension txtwidth,
                                      void (*cbk)( Widget, XtPointer,
                                                           XtPointer ) ) {
    Pixel white = XWhitePixel ( XtDisplay ( parent ), 0x00 );
    Pixel background, foreground;
    Widget txt; /*** textfield ***/
    Widget lab; /*** Label     ***/
    G3DUIMOTIF *gmt;

    XtVaGetValues ( parent, XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    if ( labwidth ) {
        lab = XmVaCreateManagedLabel ( parent, name,
                                       XmNx, x,
                                       XmNy, y,
                                       XmNwidth , labwidth,
                                       XmNheight, 0x12,
                                       XmNalignment, XmALIGNMENT_END,
                                       XmNframeChildType, XmFRAME_GENERIC_CHILD,
                                       XmNfontList, gmt->fontlist,
                                       XmNforeground, foreground,
                                       XmNbackground, background,
                                       NULL );
    }

    txt = XmVaCreateManagedText ( parent, name,
                                  XmNx, x + labwidth,
                                  XmNy, y,
                                  XmNwidth , txtwidth,
                                  XmNheight, 0x12,
                                  XmNfontList, gmt->fontlist,
                                  XmNframeChildType, XmFRAME_GENERIC_CHILD,
                                  XmNmaxLength, 0x10,
                                  XmNmarginWidth, 0x00,
                                  XmNmarginHeight, 0x00,
                                  XmNshadowThickness, 0x01,
                                  XmNhighlightOnEnter, True,
                                  XmNhighlightThickness, 0x01,
                                  XmNforeground, foreground,
                                  XmNbackground, white,
                                  NULL );

    XtAddCallback ( txt, XmNmodifyVerifyCallback, verifyfloatcbk, NULL );

    if ( cbk ) {
        XtAddCallback ( txt, XmNvalueChangedCallback, cbk  , gui  );
    }
}

/******************************************************************************/
Widget createIntegerText ( Widget parent, G3DUI *gui,
                                          char *name,
                                          Dimension x, Dimension y,
                                          Dimension labwidth,
                                          Dimension txtwidth,
                                          void (*cbk)( Widget, XtPointer, 
                                                               XtPointer ) ) {
    Pixel white = XWhitePixel ( XtDisplay ( parent ), 0x00 );
    Pixel background, foreground;
    Widget txt; /*** textfield ***/
    Widget lab; /*** Label     ***/
    G3DUIMOTIF *gmt;

    XtVaGetValues ( parent, XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    if ( labwidth ) {
        lab = XmVaCreateManagedLabel ( parent, name,
                                       XmNx, x,
                                       XmNy, y,
                                       XmNwidth , labwidth,
                                       XmNheight, 0x12,
                                       XmNfontList, gmt->fontlist,
                                       XmNforeground, foreground,
                                       XmNbackground, background,
                                       XmNalignment, XmALIGNMENT_END,
                                      /*** in case it belongs to a frame ***/
                                       XmNframeChildType, XmFRAME_GENERIC_CHILD,
                                       NULL );
    }

    txt = XmVaCreateManagedSpinButton ( parent, name,
                                  XmNx, x + labwidth,
                                  XmNy, y,
                                  XmNwidth ,
                                  txtwidth + 0x20, /* 32 pix for "+-" buttons */
                                  XmNheight, 0x12,
                                  XmNfontList, gmt->fontlist,
                                  XmNmaxLength, 0x04,
                                  XmNmarginWidth, 0x00,
                                  XmNmarginHeight, 0x00,
                                  XmNshadowThickness, 0x01,
                                  XmNhighlightOnEnter, True,
                                  XmNhighlightThickness, 0x01,
                                  XmNforeground, foreground,
                                  /*XmNbackground, white,*/
                                  /*** in case it belongs to a frame ***/
                                  XmNframeChildType, XmFRAME_GENERIC_CHILD,
                                  NULL );

#ifdef dfdsfdsfdsf
    txt = XmVaCreateManagedText ( parent, name,
                                  XmNx, x + labwidth,
                                  XmNy, y,
                                  XmNwidth , txtwidth,
                                  XmNheight, 0x12,
                                  XmNfontList, gmt->fontlist,
                                  XmNmaxLength, 0x04,
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
#endif

    /*XtAddCallback ( txt, XmNmodifyVerifyCallback, verifyintcbk, NULL );*/

    if ( cbk ) {
        /*XtAddCallback ( txt, XmNvalueChangedCallback, cbk, gui );*/
    }

    return txt;
}

/******************************************************************************/
void createCharText ( Widget parent, G3DUI *gui, 
                                     char *name,
                                     Dimension x, Dimension y,
                                     Dimension labwidth,
                                     Dimension txtwidth,
                                     void (*cbk)( Widget, XtPointer, 
                                                          XtPointer ) ) {
    Pixel white = XWhitePixel ( XtDisplay ( parent ), 0x00 );
    Pixel background, foreground;
    Widget txt; /*** textfield ***/
    Widget lab; /*** Label     ***/
    G3DUIMOTIF *gmt;

    XtVaGetValues ( parent, XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    if ( labwidth ) {
        lab = XmVaCreateManagedLabel ( parent, name,
                                       XmNx, x,
                                       XmNy, y,
                                       XmNwidth , labwidth,
                                       XmNheight, 0x12,
                                       XmNfontList, gmt->fontlist,
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
                                  XmNfontList, gmt->fontlist,
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
Pixmap createSimplePixmap ( Display *dis, char **xpm_data, Pixel bg ) {
    XpmColorSymbol bgmask = { NULL, "none", bg };
    /*** We use the root window because using the Widget's window would not ***/
    /*** usually work as this is called when the window is not realized yet ***/
    Window win = RootWindow ( dis, 0x00 );
    int depth = XDefaultDepth ( dis, 0x00 );
    XpmAttributes atr;
    Pixmap pixmap;

    atr.valuemask  = XpmColorSymbols | XpmCloseness | XpmDepth;
    atr.numsymbols = 0x0001;
    atr.closeness  = 0xFFFF;
    atr.depth      = depth;

    atr.colorsymbols = &bgmask;

    XpmCreatePixmapFromData ( dis, win, xpm_data, &pixmap, NULL, &atr );


    return pixmap;
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
void g3dui_redrawWidgetList ( G3DUI *gui, LIST *lwidget ) {
    LIST *ltmpwidget = lwidget;

    while ( ltmpwidget ) {
        Widget w = ( Widget ) ltmpwidget->data;

        XClearArea ( XtDisplay ( w ),
                     XtWindow  ( w ), 0x00, 0x00, 0x00, 0x00, True );

        ltmpwidget = ltmpwidget->next;
    }
}

/******************************************************************************/
void g3dui_redrawObjectList ( G3DUI *gui ) {
    g3dui_redrawWidgetList ( gui, gui->lobjlist );
}

/******************************************************************************/
void g3dui_redrawAllWeightGroupList ( G3DUI *gui ) {
    g3dui_redrawWidgetList ( gui, gui->lweightgroup );
}

/******************************************************************************/
void g3dui_redrawTimeline ( G3DUI *gui ) {
    LIST *ltmp = gui->ltimeline;

    while ( ltmp ) {
        Widget w = ( Widget ) ltmp->data;

        XClearArea ( XtDisplay ( w ),
                     XtWindow  ( w ), 0x00, 0x00, 0x00, 0x00, True );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dui_setMouseTool ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    char *toolname = XtName ( w );
    G3DMOUSETOOL *mou;
    G3DUIMOTIF *gmt;

    mou = common_g3dui_getMouseTool ( gui, toolname );

    gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    common_g3dui_setMouseTool ( gui, mou );

    if ( ( mou->flags & MOUSETOOLNOCURRENT ) == 0x00 ) {
        /*** Remember that widget ID, for example to be unset when a toggle button 
        from another parent widget is called (because XmNradioBehavior won't talk
        to other parent widget ***/
        if ( gmt->curmou && w != gmt->curmou ) {
            XtVaSetValues ( gmt->curmou, XmNset, False, NULL );
        }

        gmt->curmou = w;

        g3dui_updateAllCurrentMouseTools ( gui );
    }
}

/******************************************************************************/
void g3dui_updateKeyEdit ( G3DUI *gui ) {
    LIST *ltmp = gui->lkeyedit;

    while ( ltmp ) {
        Widget w = ( Widget ) ltmp->data;

        /*updateKeyEdit ( w );*/


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
        /*updateGLView ( w );*/

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dui_redrawGLViews ( G3DUI *gui ) {
    LIST *ltmp = gui->lglview;

    while ( ltmp ) {
        Widget glview = ( Widget ) ltmp->data;

        if ( XtIsManaged ( glview ) ) {
            Widget glarea = g3duiview_getGLArea ( glview );
            Display *dis  = XtDisplay ( glarea );

            XClearArea ( XtDisplay ( glarea ),
                         XtWindow  ( glarea ), 0x00, 0x00, 0x00, 0x00, True );

            XSync ( XtDisplay ( glarea ), False );
        }

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dui_updateMaterialEdit ( G3DUI *gui ) {
    LIST *ltmpmatedit = gui->lmatedit;

    while ( ltmpmatedit ) {
        Widget widget = ( Widget ) ltmpmatedit->data;

        updateMaterialEdit ( widget, gui );

        ltmpmatedit = ltmpmatedit->next;
    }
}

/******************************************************************************/
void g3dui_redrawMaterialList ( G3DUI *gui ) {
    LIST *ltmp = gui->lmatlist;

    while ( ltmp ) {
        Widget w = ( Widget ) ltmp->data;

        XClearArea ( XtDisplay ( w ),
                     XtWindow  ( w ), 0x00, 0x00, 0x00, 0x00, True );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dui_updateSelectedMaterialPreview ( G3DUI *gui ) {
    LIST *ltmpmatlist = gui->lmatlist;
    G3DMATERIAL *mat = gui->selmat;

    if ( mat ) {
        while ( ltmpmatlist ) {
            Widget *matlst = ( Widget * ) ltmpmatlist->data;

            g3duimateriallist_updatePreview ( matlst, gui, mat );


            ltmpmatlist = ltmpmatlist->next;
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
void g3dui_updateAllCurrentEdit ( G3DUI *gui ) {
    LIST *ltmp = gui->lcuredit;

    while ( ltmp ) {
        Widget w = ( Widget ) ltmp->data;

        updateCurrentEdit ( w, gui );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dui_updateAllCurrentMouseTools ( G3DUI *gui ) {
    LIST *ltmp = gui->lmtools;

    while ( ltmp ) {
        Widget w = ( Widget ) ltmp->data;

        updateCurrentMouseTool ( w, gui );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dui_updateCoords ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
    LIST *ltmpcoordedit = gui->lcoordedit;

    if ( obj ) {
        while ( ltmpcoordedit ) {
            Widget w = ( Widget ) ltmpcoordedit->data;

            updateCoordinatesEdit ( w, gui );


            ltmpcoordedit = ltmpcoordedit->next;
        }
    }
}

/******************************************************************************/
#ifdef UNUSED
void reloadfilesignal ( int signo, siginfo_t *info, void *extra ) {
    void *ptr_val = info->si_value.sival_ptr;
    /*int int_val = info->si_value.sival_int;*/
    G3DUI *gui = ( G3DUI * ) ptr_val;
    R3DNETCLIENT *clt = gui->clt;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    /* a decommnter apres adaptation ***/
    /*clt->sce = gui->sce = g3duiOpenG3DFile ( gui, clt->filename );*/

    if ( clt->sce ) {
        Dimension width, height;
        R3DSCENE *rsce;

        XtVaGetValues ( gmt->curogl, XmNwidth, &width,
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
#endif

/******************************************************************************/
void g3dui_setHourGlass ( G3DUI *gui ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    XDefineCursor ( XtDisplay ( gmt->top ), 
                    XtWindow  ( gmt->top ), gmt->watchCursor );

    XFlush ( XtDisplay ( gmt->top ) );
}

/******************************************************************************/
void g3dui_unsetHourGlass ( G3DUI *gui ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    XUndefineCursor ( XtDisplay ( gmt->top ), 
                      XtWindow  ( gmt->top ) );
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
    char **fonts;
    int nbfont, nbfontdir = 0x01, i;
    char *home = getenv ( "HOME" );
    char configFileName[0x100];
    G3DUIMOTIF *gmt;

    snprintf ( configFileName, 0x100, "%s/.gloss3d/gloss3d.conf", home );

    /*** this is required if we want to inherit the event mask. ***/
    /*** Redisplay procedure would not be called otherwise      ***/
    xmManagerClassRec.core_class.realize ( w, value_mask, attributes );

    XtVaGetValues ( w, XmNcolormap, &cmap,
                       XmNwidth, &width,
                       XmNheight, &height, NULL );

    XAllocNamedColor ( dis, cmap, BACKGROUNDCOLOR, &scrcol, &exacol );

    XtVaSetValues ( w, XmNbackground, scrcol.pixel, NULL );

    gui->toolkit_data = gmt = g3duimotif_new ( );

    gmt->top  = XtParent ( w );
    gmt->main = w;

    gui->lobjmenu   = NULL;
    gui->lvermenu   = NULL;
    gui->ledgmenu   = NULL;
    gui->lfacmenu   = NULL;
    gui->lmtools    = NULL;
    gui->lglview    = NULL;
    gui->lobjlist   = NULL;
    gui->lmatlist   = NULL;
    gui->lcoordedit = NULL;
    gui->lcuredit   = NULL;

    gmt->watchCursor = XCreateFontCursor ( dis, XC_watch );

    /*fonts = XListFonts ( dis, "*", 2048, &nbfont );

    for ( i = 0x00; i < nbfont; i++ ) {
        printf ( "Font: %s\n", fonts[i] );
    }*/

    common_g3dui_loadConfiguration ( gui, configFileName );

    /*** undo redo manager ***/
    gui->urm = g3durmanager_new ( gui->conf.undolevel );

    /*** copy-paste manager ***/
    /*gui->cli = g3duiclipboard_new ( );*//*a decommenter apres adaptation*/

    gui->flags = ( VIEWOBJECT | XAXIS | YAXIS | ZAXIS );

    gui->currsg = g3duirendersettings_new ( ); /*** default render settings ***/

    createRenderTable ( w );
    createFonts ( w );

    common_g3dui_resizeWidget ( gui, width, height );


    quad = createQuad ( w, gui, QUADNAME,  gui->quadrec.x,
                                           gui->quadrec.y,
                                           gui->quadrec.width,
                                           gui->quadrec.height );

    if ( gui->loadFile ) {
        g3dui_loadFile ( gui, gui->loadFile );

        common_g3dui_setFileName ( gui, gui->loadFile );
    } else {
        gui->sce = g3dscene_new  ( 0x00, "Gloss3D scene" );
    }

    createMenuBar ( w, &guw->gui, MENUBARNAME, gui->menurec.x,
                                               gui->menurec.y,
                                               gui->menurec.width,
                                               gui->menurec.height );

    createToolBar ( w, gui, TOOLBARNAME, gui->tbarrec.x,
                                         gui->tbarrec.y,
                                         gui->tbarrec.width,
                                         gui->tbarrec.height );

    createModeBar ( w, &guw->gui, MODEBARNAME, gui->mbarrec.x,
                                               gui->mbarrec.y,
                                               gui->mbarrec.width,
                                               gui->mbarrec.height );
#ifdef youpitralala
    tab = XmVaCreateManagedTabbedBook ( w, MAINBOARDNAME,
                                           XmNx, gui->mbrdrec.x,
                                           XmNy, gui->mbrdrec.y,
                                           XmNwidth, gui->mbrdrec.width,
                                           XmNheight, gui->mbrdrec.height,
                                           XmNfontList, gmt->fontlist,
                                           NULL );


    createMaterialBoard ( tab, &guw->gui, "Material Board", 0x00, 0x00, 0x140, 0x300 );
    createObjectBoard   ( tab, &guw->gui, "Object Board", 0x00, 0x00, 0x140, 0x300 );


    
    /*** Called after material widget is created ***/
    g3dui_importMaterials ( gui );
#endif
    /*** MainBoard uses G3DSCENE, we must created it after scene creation ***/
    createMainBoard ( w, gui, MAINBOARDNAME, gui->mbrdrec.x,
                                             gui->mbrdrec.y,
                                             gui->mbrdrec.width,
                                             gui->mbrdrec.height );

    createTimeboard ( w, gui, TIMEBOARDNAME,  gui->timerec.x,
                                              gui->timerec.y,
                                              gui->timerec.width,
                                              gui->timerec.height );

    /*** Add mouse tools AFTER GLViews creation ***/
    common_g3dui_initDefaultMouseTools ( gui );

    XtAddEventHandler ( w, NoEventMask, True, g3duicom_communicate, gui );


    return ( XtRealizeProc ) NULL;
}

/******************************************************************************/
static XtWidgetProc Destroy ( Widget w ) {
    GUiWidget guw = ( GUiWidget ) w;
    Display *dis = XtDisplay ( w );
    G3DUI *gui = &guw->gui;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    XFreeFont ( dis, gmt->nrmlft );
    XFreeFont ( dis, gmt->boldft );
    XFreeFont ( dis, gmt->tinyft );

    XmRenderTableFree ( gmt->renTable );


    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
/*** This handles children widget resizing. I don't use XmForm Attachment   ***/
/*** because I find it not very convenient for Windows with many children.  ***/
static XtWidgetProc Resize ( Widget w ) {
    GUiWidget guw = ( GUiWidget ) w;
    Dimension width, height;
    WidgetList children;
    G3DUI *gui = &guw->gui;
    Cardinal nc;
    int i;

    XtVaGetValues ( w, XmNchildren, &children,
                       XmNnumChildren, &nc,
                       XmNwidth, &width,
                       XmNheight, &height,
                       NULL );

    common_g3dui_resizeWidget ( gui, width, height );

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];
        char *name = XtName ( child );
        G3DUIRECTANGLE *rec = NULL;

        if ( strcmp ( name, MENUBARNAME     ) == 0x00 ) rec = &gui->menurec;
        if ( strcmp ( name, TOOLBARNAME     ) == 0x00 ) rec = &gui->tbarrec;
        if ( strcmp ( name, MODEBARNAME     ) == 0x00 ) rec = &gui->mbarrec;
        if ( strcmp ( name, QUADNAME        ) == 0x00 ) rec = &gui->quadrec;
        if ( strcmp ( name, TIMEBOARDNAME   ) == 0x00 ) rec = &gui->timerec;
        if ( strcmp ( name, MAINBOARDNAME   ) == 0x00 ) rec = &gui->mbrdrec;
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

/******************************************************************************/
G3DSCENE *g3duiLoadFileSignal ( G3DUI *gui, char *filename ) {

}

/******************************************************************************/
/*** Create the material previews, typically after loading a scene file ***/
void g3dui_clearMaterials   ( G3DUI *gui ) {
    LIST *ltmpmatlist = gui->lmatlist;

    while ( ltmpmatlist ) {
        Widget matlst = ( Widget ) ltmpmatlist->data;

        g3duimateriallist_removeAllPreviews ( matlst );


        ltmpmatlist = ltmpmatlist->next;
    }
}

/******************************************************************************/
/*** Create the material previews, typically after loading a scene file ***/
void g3dui_importMaterials ( G3DUI *gui ) {
    LIST *ltmpmatlist = gui->lmatlist;

    while ( ltmpmatlist ) {
        Widget matlst = ( Widget ) ltmpmatlist->data;

        g3duimateriallist_importFromScene ( matlst, gui );

        ltmpmatlist = ltmpmatlist->next;
    }
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
