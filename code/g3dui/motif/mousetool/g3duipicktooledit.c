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
static void paintRadiusCbk ( Widget w, XtPointer client, XtPointer call ) {
    int radius = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duipicktooledit_paintRadiusCbk ( gui, radius );
}

/******************************************************************************/
static void setWeightCbk ( Widget w, XtPointer client, XtPointer call ) {
    XmScaleCallbackStruct *scs = ( XmScaleCallbackStruct * ) call;
    float weight = ( float ) scs->value;
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duipicktooledit_setWeightCbk ( gui, weight / 100.0f );
}

/******************************************************************************/
static void onlyVisibleCbk ( Widget w, XtPointer client, XtPointer call ) {
    XmToggleButtonCallbackStruct *tbs = ( XmToggleButtonCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duipicktooledit_onlyVisibleCbk ( gui, tbs->set );
}

/******************************************************************************/
void updatePickToolWeightForm ( Widget w, G3DUI *gui ) {
    WidgetList children;
    G3DMOUSETOOL *mou = common_g3dui_getMouseTool ( gui, PICKTOOL );
    Cardinal nc;
    int i;

    XtVaGetValues ( w, XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    /*** prevent a loop ***/
    gui->lock = 0x01;

    if ( mou ) {
        G3DPICKTOOL *pt = mou->data;

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];

            if ( gui->flags & VIEWSKIN ) XtSetSensitive ( child, True  );
            else                         XtSetSensitive ( child, False );


            if ( XtClass ( child ) == xmScaleWidgetClass ) {
                if ( strcmp ( XtName ( child ), EDITPICKTOOLWEIGHTSTRENGTH ) == 0x00 ) {
                    float weight = ( pt->weight * 100.0f );

                    XtVaSetValues ( child, XmNvalue, ( uint32_t ) weight, NULL );
                }
            }

            if ( XtClass ( child ) == xmSpinButtonWidgetClass ) {
                if ( strcmp ( XtName ( child ), EDITPICKTOOLWEIGHTRADIUS ) == 0x00 ) {
                    XmSpinButtonSetValue ( child, pt->paint_radius );
                }
            }
        }
    }

    /*** prevent a loop ***/
    gui->lock = 0x00;
}

/******************************************************************************/
void updatePickToolWeightFrame ( Widget w, G3DUI *gui ) {
    WidgetList children;
    G3DMOUSETOOL *mou = common_g3dui_getMouseTool ( gui, PICKTOOL );
    Cardinal nc;
    int i;

    XtVaGetValues ( w, XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];

        if ( XtClass ( child ) == xmFormWidgetClass ) {
            updatePickToolWeightForm ( child, gui  );
        }
    }
}

/******************************************************************************/
/*void operationcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmComboBoxCallbackStruct *cbs = ( XmComboBoxCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    G3DMOUSETOOL *mou = common_g3dui_getMouseTool ( gui, PICKTOOL );
    char *str;

    XmStringGetLtoR ( cbs->item_or_text, XmFONTLIST_DEFAULT_TAG, &str );

    if ( mou ) {
        G3DPICKTOOL *pt = mou->data;

        if ( strcmp ( str, ADDWEIGHTSTR ) == 0x00 ) {
            pt->operation = 0x01;
        }

        if ( strcmp ( str, REMOVEWEIGHTSTR ) == 0x00 ) {
            pt->operation = 0x00;
        }
    }

    XtFree ( str );
}*/

/******************************************************************************/
void updatePickToolEdit ( Widget w, G3DUI *gui ) {
    WidgetList children;
    G3DMOUSETOOL *mou = common_g3dui_getMouseTool ( gui, PICKTOOL );
    Cardinal nc;
    int i;

    XtVaGetValues ( w, XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    /*** prevents a loop ***/
    gui->lock = 0x01;

    if ( mou ) {
        G3DPICKTOOL *pt = mou->data;

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
 
            if ( XtClass ( child ) == xmToggleButtonWidgetClass ) {
                if ( strcmp ( XtName ( child ), EDITPICKTOOLVISIBLE ) == 0x00 ) {
                    if ( pt->only_visible ) {
                        XtVaSetValues ( child, XmNset, True, NULL );
                    } else {
                        XtVaSetValues ( child, XmNset, False, NULL );
                    }
                }
            }

            if ( XtClass ( child ) == xmFrameWidgetClass ) {
                if ( strcmp ( XtName ( child ), EDITPICKTOOLWEIGHTFRAME ) == 0x00 ) {
                    updatePickToolWeightFrame ( child, gui );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
Widget createPickToolEdit ( Widget parent, G3DUI *gui, char *name,
                            Dimension x    , Dimension y,
                            Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget frm, ptf;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    frm = XmVaCreateManagedForm ( parent, name,
                                  XtNx, x,
                                  XtNy, y,
                                  XmNwidth , width,
                                  XmNheight, height,
                                  XmNforeground, foreground,
                                  XmNbackground, background,
                                  XmNborderWidth, 0x00,
                                  NULL );

    createToggleLabel     ( frm, gui, EDITPICKTOOLVISIBLE,
                                  0,   0,  96,  32, onlyVisibleCbk );

    ptf = createFrame     ( frm, gui, EDITPICKTOOLWEIGHTFRAME, 
                                  0,  32, 276, 128 );

    createHorizontalScale ( ptf, gui, EDITPICKTOOLWEIGHTSTRENGTH, 
                                  0,   0, 256,  64,
                                 0.0f, 100.0f, 1.0f, setWeightCbk );

    createIntegerText     ( ptf, gui, EDITPICKTOOLWEIGHTRADIUS, 
                                  0, 64,   96,  32, paintRadiusCbk );

    updatePickToolEdit ( frm, gui );


    return frm;
}
