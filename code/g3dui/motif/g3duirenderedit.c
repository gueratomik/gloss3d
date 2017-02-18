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
static void formatCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI    *gui = ( G3DUI * ) client;
    XmComboBoxCallbackStruct *cbs = ( XmComboBoxCallbackStruct * ) call;
    char *str;

    XmStringGetLtoR ( cbs->item_or_text, XmFONTLIST_DEFAULT_TAG, &str );

    common_g3duirenderedit_formatCbk ( gui, str );

    XtFree ( str );
}

/******************************************************************************/
void createRenderFormat ( Widget parent, G3DUI *gui, 
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
    XmStringTable dmlist = ( XmStringTable ) XtMalloc ( 0x02 * strsize );
    Widget lab;
    Widget sel;
    G3DUIMOTIF *gmt;

    XtVaGetValues ( parent, XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    dmlist[0x00] = XmStringCreate ( RENDERTOIMAGENAME, XmFONTLIST_DEFAULT_TAG );
    dmlist[0x01] = XmStringCreate ( RENDERTOVIDEONAME, XmFONTLIST_DEFAULT_TAG );

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
                                       XmNitemCount,	0x02,
                                       XmNitems, dmlist,
                                       XmNvisibleItemCount, 0x02,
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

    XtFree ( ( char * ) dmlist );

    if ( cbk ) {
        XtAddCallback ( sel, XmNselectionCallback, cbk, gui );
    }
}

/******************************************************************************/
static void saveCbk ( Widget w, XtPointer client, XtPointer call ) {
    uint32_t save = XmToggleButtonGetState ( w );
    G3DUI    *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( w );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    common_g3duirenderedit_saveCbk ( gui, save );

    updateRenderEdit ( parent, gui );
}

/******************************************************************************/
static void previewCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duirenderedit_previewCbk ( gui );
}

/******************************************************************************/
void g3dui_renderViewCbk  ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI      *gui = ( G3DUI * ) client;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Dimension width, height;
    G3DUIRENDERPROCESS *rps;

    XtVaGetValues ( gmt->curogl, XmNwidth , &width,
                                 XmNheight, &height,
                                 NULL );

    rps = g3dui_rendertoarea ( gui, 0x00, 0x00,
                                    width  - 1, 
                                    height - 1,
                                    gmt->curogl, 0x01 );
}



/******************************************************************************/
static void startFrameCbk ( Widget w, XtPointer client, XtPointer call ) {
    float frame = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( w );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    common_g3duirenderedit_startFrameCbk ( gui, frame );

    updateRenderEdit ( parent, gui );
}

/******************************************************************************/
static void fpsCbk ( Widget w, XtPointer client, XtPointer call ) {
    float fps = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duirenderedit_fpsCbk ( gui, fps );
}

/******************************************************************************/
static void endFrameCbk ( Widget w, XtPointer client, XtPointer call ) {
    float frame = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( w );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    common_g3duirenderedit_endFrameCbk ( gui, frame );

    updateRenderEdit ( parent, gui );
}

/******************************************************************************/
static void ratioCbk ( Widget w, XtPointer client, XtPointer call ) {
    float ratio = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( w );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    common_g3duirenderedit_ratioCbk ( gui, ratio );

    updateRenderEdit ( parent, gui );

    XmSpinButtonTailCursor ( w );
}

/******************************************************************************/
static void widthCbk ( Widget w, XtPointer client, XtPointer call ) {
    int width = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( w );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    common_g3duirenderedit_widthCbk ( gui, width );

    updateRenderEdit ( parent, gui );

    XmSpinButtonTailCursor ( w );
}

/******************************************************************************/
static void heightCbk ( Widget w, XtPointer client, XtPointer call ) {
    int height = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( w );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    common_g3duirenderedit_heightCbk ( gui, height );

    updateRenderEdit ( parent, gui );

    XmSpinButtonTailCursor ( w );
}

/******************************************************************************/
static void outputCbk ( Widget w, XtPointer client, XtPointer call ) {
    char *outfile = XmTextGetString ( w );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duirenderedit_outputCbk ( gui, outfile );

    XtFree ( outfile );
}

/******************************************************************************/
static void motionBlurCbk ( Widget w, XtPointer client, XtPointer call ) {
    int nbstep = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duirenderedit_motionBlurCbk ( gui, nbstep );
}

/******************************************************************************/
static void backgroundCbk ( Widget w, XtPointer client, XtPointer call ) {
    XmColorEditCallbackStruct *ces = ( XmColorEditCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    uint32_t red   = ( ces->pixel & 0x00FF0000 ) >> 0x10,
             green = ( ces->pixel & 0x0000FF00 ) >> 0x08,
             blue  = ( ces->pixel & 0x000000FF );

    common_g3duirenderedit_backgroundCbk ( gui, red, green, blue);
}

/******************************************************************************/
void updateRenderEdit ( Widget w, G3DUI *gui ) {
    G3DUIRENDERSETTINGS *rsg = gui->currsg;;
    WidgetList children;
    G3DOBJECT *obj;
    G3DSCENE *sce;
    Cardinal nc;
    int i;

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    XtVaGetValues ( w, XmNchildren, &children,
                       XmNnumChildren, &nc,
                       NULL );

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];
        char *name = XtName ( child );

        if ( XtClass ( child ) == xmToggleButtonWidgetClass ) {
            if ( strcmp ( name, EDITRENDERSAVE ) == 0x00 ) {
                if ( rsg->flags & RENDERSAVE ) {
                    XmToggleButtonSetState ( child, True , False );
                } else {
                    XmToggleButtonSetState ( child, False, False );
                }
            }

            if ( strcmp ( name, EDITRENDERPREVIEW ) == 0x00 ) {
                if ( rsg->flags & RENDERPREVIEW ) {
                    XmToggleButtonSetState ( child, True , False );
                } else {
                    XmToggleButtonSetState ( child, False, False );
                }
            }
        }

        if ( XtClass ( child ) == xmTextWidgetClass ) {
            if ( strcmp ( name, EDITRENDEROUTPUT ) == 0x00 ) {
                if ( rsg->outfile ) {
                    uint32_t len = strlen ( rsg->outfile ) + 0x01;
                    char *txt = calloc ( len, sizeof ( char ) );

                    snprintf ( txt, len, "%s", rsg->outfile );

                    XmTextSetString ( child, txt );

                    free ( txt );
                }
            }
        }

        if ( XtClass ( child ) == xmSpinButtonWidgetClass ) {
            /****************** Start Frame *****************/
            if ( strcmp ( name, EDITRENDERSTART ) == 0x00 ) {
                XmSpinButtonSetValue ( child, rsg->startframe );
            }

            /****************** End Frame *****************/
            if ( strcmp ( name, EDITRENDEREND ) == 0x00 ) {
                XmSpinButtonSetValue ( child, rsg->endframe );
            }

            /****************** End Frame *****************/
            if ( strcmp ( name, EDITRENDERMBLUR ) == 0x00 ) {
                XmSpinButtonSetValue ( child, rsg->mblur );
            }

            /****************** End Frame *****************/
            if ( strcmp ( name, EDITRENDERRATIO ) == 0x00 ) {
                float ratio = ( rsg->ratio ) ? rsg->ratio : gui->curcam->ratio;

                XmSpinButtonSetValue ( child, ratio );
            }

            /****************** FPS *****************/
            if ( strcmp ( name, EDITRENDERFPS ) == 0x00 ) {
                XmSpinButtonSetValue ( child, rsg->fps );
            }

            /****************** FPS *****************/
            if ( strcmp ( name, EDITRENDERWIDTH ) == 0x00 ) {
               /* float ratio = ( rsg->ratio ) ? rsg->ratio : gui->curcam->ratio;
                uint32_t rsgwidth = rsg->height * ratio;*/

                XmSpinButtonSetValue ( child, rsg->width );
            }

            /****************** FPS *****************/
            if ( strcmp ( name, EDITRENDERHEIGHT ) == 0x00 ) {
                XmSpinButtonSetValue ( child, rsg->height );
            }
        }

         if ( XtClass ( child ) == xmComboBoxWidgetClass ) {
                if ( strcmp ( name, EDITRENDERFORMAT   ) == 0x00 ) {
                    if ( rsg->flags & RENDERSAVE ) {
                        XtSetSensitive ( child, True );
                    } else {
                        XtSetSensitive ( child, False );
                    }

                    XtVaSetValues ( child, XmNselectedPosition,
                                           rsg->format, NULL );
                }
            }

        if ( XtClass ( child ) == xmPushButtonWidgetClass ) {
            if ( strcmp ( name, EDITRENDERBACKGROUND ) == 0x00 ) {
                uint32_t pixel = rsg->background;

                XtVaSetValues ( child, XmNbackground, pixel, NULL );
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
void g3dui_runRenderCbk  ( Widget w, XtPointer client, XtPointer call ) {
    Display *dis = XtDisplay ( w );
    Window  root = XDefaultRootWindow ( dis );
    G3DUI *gui   = ( G3DUI * ) client;
    Pixel background, foreground;
    Widget shell, wren, area = NULL;
    G3DUIRENDERSETTINGS *rsg = ( G3DUIRENDERSETTINGS * ) gui->currsg;
    G3DSCENE  *sce = gui->sce;
    G3DCAMERA *cam = gui->curcam;
    XWindowAttributes wat;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    float ratio = ( rsg->ratio ) ? rsg->ratio : gui->curcam->ratio;
    uint32_t rsgwidth  = rsg->height * ratio;
    uint32_t rsgheight = rsg->height;
    Widget dial;
    Colormap cmap;
    XColor scrcol, exacol;
    int screen = DefaultScreen ( dis );
    Visual *visual = DefaultVisual ( dis, screen );

    /*** This helps up to position the rendering ***/
    /*** window in the middle of the screen. ***/
    XGetWindowAttributes ( dis, root, &wat );

    /*XtVaGetValues ( w, XmNbackground, &background,
                       XmNforeground, &foreground, NULL );*/

    cmap = XCreateColormap ( dis, root, visual, AllocNone );

    XAllocNamedColor ( dis, cmap, BACKGROUNDCOLOR, &scrcol, &exacol );

    /*** Start the rendering in a new window ***/
    dial = XtVaAppCreateShell ( NULL, "Class", topLevelShellWidgetClass,
                                               dis, 
                                               XtNtitle,"Render Shell",
                                               XtNx, ( wat.width  >> 1 ) - ( rsgwidth  >> 1 ),
                                               XtNy, ( wat.height >> 1 ) - ( rsg->height >> 1 ),
                                               XtNwidth,  rsgwidth  + 0x02,
                                               XtNheight, rsg->height + 0x20,
                                               XmNvisual, visual,
                                               XmNcolormap, cmap,
                                               /*XmNforeground, foreground,
                                               XmNbackground, background,*/
                                               NULL );

    createRenderWindow ( dial, gui, "RENDER WINDOW", 0, 0, rsgwidth,
                                                           rsgheight );

    XtRealizeWidget ( dial );
}

/******************************************************************************/
static void backgroundcbk ( uint32_t pixel, void *data ) {
    G3DUI *gui = ( G3DUI * ) data;
    G3DUIRENDERSETTINGS *rsg = gui->currsg;

    if ( rsg ) {
        LIST *ltmp = gui->lmatlist;

        rsg->background = pixel;
    }
}

/******************************************************************************/
Widget createRenderEdit ( Widget parent, G3DUI *gui, 
                                         char *name,
                                         Dimension x,
                                         Dimension y,
                                         Dimension width, 
                                         Dimension height ) {
    Pixel background, foreground;
    Widget frm, col;

    /*** Crashes if attached to a shell widget :

  X Error of failed request:  BadValue (integer parameter out of range for operation)
  Major opcode of failed request:  91 (X_QueryColors)
  Value in failed request:  0xf8fa30ea
  Serial number of failed request:  7329
  Current serial number in output stream:  7329

  I guess that's a colormap issue but I don't know how to fix this yet **/


    /*XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );*/

    frm = XmVaCreateManagedForm ( parent, name,
                                  XmNx, x,
                                  XmNy, y,
                                  XmNwidth , width,
                                  XmNheight, height,
                                  /*XmNforeground, foreground,
                                  XmNbackground, background,*/
                                  NULL );


    createToggleLabel ( frm, gui, EDITRENDERSAVE,
                               0,   0, 104, 20, saveCbk );

    /*createToggleLabel ( frm, gui, EDITRENDERPREVIEW,
                               0,  24, 104, 20, previewCbk );*/

    createIntegerText ( frm, gui, EDITRENDERSTART,
                               0,  48, 96,  32, startFrameCbk );

    createIntegerText ( frm, gui, EDITRENDEREND,
                               0,  72, 96,  32, endFrameCbk );

    createIntegerText ( frm, gui, EDITRENDERFPS,
                               0,  96, 96,  32, fpsCbk );

    createIntegerText ( frm, gui, EDITRENDERWIDTH,
                               0, 120, 96,  32, widthCbk );

    createIntegerText ( frm, gui, EDITRENDERHEIGHT,
                               0, 144, 96,  32, heightCbk );

    createFloatText   ( frm, gui, EDITRENDERRATIO,
                               0, 168, 96,  64, ratioCbk );

    createRenderFormat( frm, gui, EDITRENDERFORMAT,
                               0, 192, 96,  128, formatCbk );

    createCharText    ( frm, gui, EDITRENDEROUTPUT,
                               0, 216, 96, 200, outputCbk );

    createIntegerText ( frm, gui, EDITRENDERMBLUR,
                               0, 240, 96,  64, motionBlurCbk );

    createSimpleLabel ( frm, gui, EDITRENDERBACKGROUND,
                               0, 264, 96, 20 );

    createColorButton ( frm, gui, EDITRENDERBACKGROUND,
                              96, 264, 96, 18, backgroundCbk );

    createPushButton  ( frm, gui, EDITRENDERRUN,
                              96, 288, 96, 20, g3dui_runRenderCbk );


    updateRenderEdit ( frm, gui );

    /*list_insert ( &gui->lrendersettings, frm );*/


    return frm;
}

/******************************************************************************/
/*** This filter is declared in the g3dui layer because of the Widget struct***/
R3DFILTER *r3dfilter_toWidget_new ( Widget w, uint32_t active_fill ) {
    Display *dis = XtDisplay ( w );
    Window win = XtWindow ( w );
    R3DFILTER *fil;

    fil = r3dfilter_new ( FILTERLINE, TOWINDOWFILTERNAME,
                                      filtertowindow_draw,
   /*** When destroyed, the widget is in charge of freeing the private data ***/
                                      NULL, 
                                      filtertowindow_new ( dis, win, active_fill ) );

    return fil;
}
