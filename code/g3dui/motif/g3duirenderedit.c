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
void g3duirendersettings_rendertoimage ( G3DUIRENDERSETTINGS *rsg, 
                                         G3DSCENE *sce,
                                         G3DCAMERA *cam,
                                         uint32_t engine_flags,
                                         Widget area ) {

}

/******************************************************************************/
void g3duirendersettings_rendertovideo ( G3DUIRENDERSETTINGS *rsg, 
                                         G3DSCENE *sce,
                                         G3DCAMERA *cam,
                                         uint32_t engine_flags,
                                         Widget area ) {

}


/******************************************************************************/
static void previewCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;


}

/******************************************************************************/
static void startFrameCbk ( Widget w, XtPointer client, XtPointer call ) {
    char *value = XmTextGetString ( w );
    float frame = strtof ( value, NULL );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duirenderedit_startFrameCbk ( gui, frame );

    XtFree ( value );
}

/******************************************************************************/
static void fpsCbk ( Widget w, XtPointer client, XtPointer call ) {
    char *value = XmTextGetString ( w );
    float fps = strtof ( value, NULL );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duirenderedit_fpsCbk ( gui, fps );

    XtFree ( value );
}

/******************************************************************************/
static void endFrameCbk ( Widget w, XtPointer client, XtPointer call ) {
    char *value = XmTextGetString ( w );
    float frame = strtof ( value, NULL );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duirenderedit_endFrameCbk ( gui, frame );

    XtFree ( value );
}

/******************************************************************************/
static void ratioCbk ( Widget w, XtPointer client, XtPointer call ) {
    char *value = XmTextGetString ( w );
    float ratio = strtof ( value, NULL );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duirenderedit_ratioCbk ( gui, ratio );

    XtFree ( value );
}

/******************************************************************************/
static void widthCbk ( Widget w, XtPointer client, XtPointer call ) {
    char *value = XmTextGetString ( w );
    int width = strtol ( value, NULL, 10 );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duirenderedit_widthCbk ( gui, width );

    XtFree ( value );
}

/******************************************************************************/
static void heightCbk ( Widget w, XtPointer client, XtPointer call ) {
    char *value = XmTextGetString ( w );
    int height = strtol ( value, NULL, 10 );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duirenderedit_heightCbk ( gui, height );

    XtFree ( value );
}

/******************************************************************************/
static void outputCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;


}

/******************************************************************************/
static void motionBlurCbk ( Widget w, XtPointer client, XtPointer call ) {
    char *value = XmTextGetString ( w );
    int nbstep = strtol ( value, NULL, 10 );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duirenderedit_motionBlurCbk ( gui, nbstep );

    XtFree ( value );
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
        char buf[0x10];

        if ( XtClass ( child ) == xmToggleButtonWidgetClass ) {
            /*if ( strcmp ( name, EDITKEYLOOP ) == 0x00 ) {

            }*/
        }

        if ( XtClass ( child ) == xmPushButtonWidgetClass ) {
            if ( strcmp ( name, EDITRENDERBACKGROUND ) == 0x00 ) {
                uint32_t pixel = rsg->background;

                XtVaSetValues ( child, XmNbackground, pixel, NULL );
            }
        }

        if ( XtClass ( child ) == xmTextWidgetClass ) {
            /****************** Start Frame *****************/
            if ( strcmp ( name, EDITRENDERSTART ) == 0x00 ) {
                snprintf ( buf, 0x10, "%d", rsg->startframe );

                XmTextSetString ( child, buf );
            }

            /****************** End Frame *****************/
            if ( strcmp ( name, EDITRENDEREND ) == 0x00 ) {
                snprintf ( buf, 0x10, "%d", rsg->endframe );

                XmTextSetString ( child, buf );
            }

            /****************** End Frame *****************/
            if ( strcmp ( name, EDITRENDERMBLUR ) == 0x00 ) {
                snprintf ( buf, 0x10, "%d", rsg->mblur );

                XmTextSetString ( child, buf );
            }

            /****************** End Frame *****************/
            if ( strcmp ( name, EDITRENDERRATIO ) == 0x00 ) {
                float ratio = ( rsg->ratio ) ? rsg->ratio : gui->curcam->ratio;

                snprintf ( buf, 0x10, "%f", ratio );

                XmTextSetString ( child, buf );
            }

            /****************** FPS *****************/
            if ( strcmp ( name, EDITRENDERFPS ) == 0x00 ) {
                snprintf ( buf, 0x10, "%d", rsg->fps );

                XmTextSetString ( child, buf );
            }

            /****************** FPS *****************/
            if ( strcmp ( name, EDITRENDERWIDTH ) == 0x00 ) {
                float ratio = ( rsg->ratio ) ? rsg->ratio : gui->curcam->ratio;
                uint32_t rsgwidth = rsg->height * ratio;

                snprintf ( buf, 0x10, "%d", rsgwidth );

                XmTextSetString ( child, buf );
            }

            /****************** FPS *****************/
            if ( strcmp ( name, EDITRENDERHEIGHT ) == 0x00 ) {
                snprintf ( buf, 0x10, "%d", rsg->height );

                XmTextSetString ( child, buf );
            }

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
    }

    gui->lock = 0x00;
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

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    frm = XmVaCreateManagedForm ( parent, name,
                                  XmNx, x,
                                  XmNy, y,
                                  XmNwidth , width,
                                  XmNheight, height,
                                  XmNforeground, foreground,
                                  XmNbackground, background,
                                  NULL );

    createToggleLabel ( frm, gui, EDITRENDERPREVIEW,
                               0,   0, 104, 20, previewCbk );

    createIntegerText ( frm, gui, EDITRENDERSTART,
                               0,  24, 96,  32, startFrameCbk );

    createIntegerText ( frm, gui, EDITRENDEREND,
                               0,  48, 96,  32, endFrameCbk );

    createIntegerText ( frm, gui, EDITRENDERFPS,
                               0,  72, 96,  32, fpsCbk );

    createIntegerText ( frm, gui, EDITRENDERWIDTH,
                               0,  96, 96,  32, widthCbk );

    createIntegerText ( frm, gui, EDITRENDERHEIGHT,
                               0, 120, 96,  32, heightCbk );

    createFloatText   ( frm, gui, EDITRENDERRATIO,
                               0, 144, 96,  64, ratioCbk );

    createCharText    ( frm, gui, EDITRENDEROUTPUT,
                               0, 168, 96, 200, outputCbk );

    createIntegerText ( frm, gui, EDITRENDERMBLUR,
                               0, 192, 96,  64, motionBlurCbk );

    createSimpleLabel ( frm, gui, EDITRENDERBACKGROUND,
                               0, 216, 96, 20 );

    createColorButton ( frm, gui, EDITRENDERBACKGROUND,
                              96, 216, 96, 18, backgroundCbk );

    /*createPushButton  ( frm, gui, EDITRENDERRUN,
                              96, 240, 48, 20, g3dui_runRenderCbk );*/


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
