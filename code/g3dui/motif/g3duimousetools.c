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
void restrictcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DMOUSETOOL *mou = common_g3dui_getMouseTool ( gui, CUTMESHTOOL );
    G3DCUTMESH   *cm = mou->data;
    XmToggleButtonCallbackStruct *tbs = ( XmToggleButtonCallbackStruct * ) call;

    if ( gui->lock ) return;

    if ( tbs->set ) cm->restrict_to_selection = 0x00;
    else            cm->restrict_to_selection = 0x01;
}

/******************************************************************************/
void updateCutMeshToolEdit ( Widget w ) {
    WidgetList children;
    G3DMOUSETOOL *mou;
    Cardinal nc;
    G3DUI *gui;
    int i;

    XtVaGetValues ( w, XmNuserData, &gui,
                       XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    mou = common_g3dui_getMouseTool ( gui, CUTMESHTOOL );

    gui->lock = 0x01;

    if ( mou ) {
        G3DCUTMESH *cm = mou->data;

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
 
            if ( XtClass ( child ) == xmToggleButtonWidgetClass ) {
                if ( strcmp ( XtName ( child ), "Restrict to selection" ) == 0x00 ) {
                    if ( cm->restrict_to_selection ) {
                        XtVaSetValues ( child, XmNset, True, NULL );
                    } else {
                        XtVaSetValues ( child, XmNset, False, NULL );
                    }
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
Widget createKnifeToolEdit ( Widget parent, G3DUI *gui, char *name,
                             Dimension x    , Dimension y,
                             Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget lab, frm;


    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    frm = XmVaCreateManagedForm ( parent, name,
                                  XtNx, 0x00,
                                  XtNy, 0x00,
                                  XmNwidth , 0x140,
                                  XmNheight, 0x140,
                                  XmNforeground, foreground,
                                  XmNbackground, background,
                                  XmNborderWidth, 0x00,
                                  XmNuserData, gui,
                                  NULL );

    lab = XmVaCreateManagedToggleButton ( frm, "Restrict to selection",
                                          XtNx, 0x00,
                                          XtNy, 0x00,
                                          XmNwidth , 0xC0,
                                          XmNheight, 0x20,
                                          XmNforeground, foreground,
                                          XmNbackground, background,
                                          XmNborderWidth, 0x00,
                                          XmNuserData, gui,
                                          NULL );

    XtAddCallback ( lab, XmNarmCallback, restrictcbk, gui );


    return frm;
}

/******************************************************************************/
void operationcbk ( Widget w, XtPointer client, XtPointer call ) {
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
}

/******************************************************************************/
void setweightcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmScaleCallbackStruct *scs = ( XmScaleCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    G3DMOUSETOOL *mou = common_g3dui_getMouseTool ( gui, PICKTOOL );

    if ( mou ) {
        G3DPICKTOOL *pt = mou->data;

        pt->weight = ( float ) scs->value / 100.0f;
    }
}

/******************************************************************************/
void paintradiuscbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DMOUSETOOL *mou = common_g3dui_getMouseTool ( gui, PICKTOOL );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( mou ) {
        G3DPICKTOOL *pt = mou->data;
        char *value = XmTextGetString ( w );
        int radius = strtol ( value, NULL, 10 );

        pt->paint_radius = radius;

        XtFree ( value );
    }
}

/******************************************************************************/
void onlyvisiblecbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DMOUSETOOL *mou = common_g3dui_getMouseTool ( gui, PICKTOOL );
    G3DPICKTOOL *pt = mou->data;
    XmToggleButtonCallbackStruct *tbs = ( XmToggleButtonCallbackStruct * ) call;

    if ( gui->lock ) return;

    if ( tbs->set ) pt->only_visible = 0x00;
    else            pt->only_visible = 0x01;
}

/******************************************************************************/
void updatePaintWeightFrame ( Widget w ) {
    WidgetList children;
    G3DMOUSETOOL *mou;
    Cardinal nc;
    G3DUI *gui;
    int i;

    XtVaGetValues ( w, XmNuserData, &gui,
                       XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    mou = common_g3dui_getMouseTool ( gui, PICKTOOL );

    if ( mou ) {
        G3DPICKTOOL *pt = mou->data;

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];

            if ( XtClass ( child ) == xmScaleWidgetClass ) {
                if ( strcmp ( XtName ( child ), EDITWEIGHTVALUE ) == 0x00 ) {
                    float weight = ( pt->weight * 100.0f );

                    XtVaSetValues ( child, XmNvalue, ( uint32_t ) weight, NULL );
                }
            }

            if ( XtClass ( child ) == xmTextWidgetClass ) {
                if ( strcmp ( XtName ( child ), EDITWEIGHTRADIUS ) == 0x00 ) {
                    char buf[0x10];
                    snprintf ( buf, 0x10, "%d", pt->paint_radius );

                    XmTextSetString ( child, buf );
                }
            }
        }
    }
}

/******************************************************************************/
void updatePickToolEdit ( Widget w ) {
    WidgetList children;
    G3DMOUSETOOL *mou;
    Cardinal nc;
    G3DUI *gui;
    int i;

    XtVaGetValues ( w, XmNuserData, &gui,
                       XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    mou = common_g3dui_getMouseTool ( gui, PICKTOOL );

    gui->lock = 0x01;

    if ( mou ) {
        G3DPICKTOOL *pt = mou->data;

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
 
            if ( XtClass ( child ) == xmToggleButtonWidgetClass ) {
                if ( strcmp ( XtName ( child ), "Only Visible" ) == 0x00 ) {
                    if ( pt->only_visible ) {
                        XtVaSetValues ( child, XmNset, True, NULL );
                    } else {
                        XtVaSetValues ( child, XmNset, False, NULL );
                    }
                }
            }

            if ( XtClass ( child ) == xmFrameWidgetClass ) {
                updatePaintWeightFrame ( child );
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
    Widget lab, brd, frm;
    XmStringTable dmlist;
    Widget sel, sca;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    brd = XmVaCreateManagedForm ( parent, name,
                                  XtNx, 0x00,
                                  XtNy, 0x00,
                                  XmNwidth , 0x140,
                                  XmNheight, 0x140,
                                  XmNforeground, foreground,
                                  XmNbackground, background,
                                  XmNborderWidth, 0x00,
                                  NULL );

    lab = XmVaCreateManagedToggleButton ( brd, "Only Visible",
                                          XtNx, 0x00,
                                          XtNy, 0x00,
                                          XmNwidth , 0x60,
                                          XmNheight, 0x20,
                                          XmNforeground, foreground,
                                          XmNbackground, background,
                                          XmNborderWidth, 0x00,
                                          NULL );

    XtAddCallback ( lab, XmNarmCallback, onlyvisiblecbk, gui );

    frm = createFrame ( brd, gui, "Weight Painting", 0x00, 0x20, 0x120, 0x0A0 );

    sca = XmVaCreateManagedScale ( frm, EDITWEIGHTVALUE,
                                   XtNx, 0x10,
                                   XtNy, 0x40,
                                   XmNwidth , 0xFF,
                                   XmNheight, 0x20,
                                   XmNmaximum, 100,
                                   XmNminimum, 0x0,
                                   XmNshowValue, True,
                                   XmNhighlightThickness, 0x00,
                                   XmNshadowThickness, 0x01,
                                   XmNorientation, XmHORIZONTAL,
                                   XmNfontList, gmt->fontlist,
                                   XmNframeChildType, XmFRAME_GENERIC_CHILD,
                                   XmNforeground, foreground,
                                   XmNbackground, background,
                                   NULL );

    XtAddCallback ( sca, XmNvalueChangedCallback, setweightcbk, gui );

    createIntegerText ( frm, EDITWEIGHTRADIUS, 0x10, 0x70, 0x50, 0x20, paintradiuscbk );


    return brd;
}

/******************************************************************************/
void updateCurrentMouseTool ( Widget w, G3DUI *gui ) {
    G3DMOUSETOOL *mou = gui->mou;;
    WidgetList children;
    G3DOBJECT *obj;
    G3DSCENE *sce;
    Cardinal nc;
    int i;

    XtVaGetValues ( w, XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    if ( mou ) {
        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];

            if ( strcmp ( XtName ( child ), mou->name ) == 0x00 ) {
                if ( strcmp ( mou->name, PICKTOOL    ) == 0x00 ) updatePickToolEdit  ( child );
                if ( strcmp ( mou->name, CUTMESHTOOL ) == 0x00 ) updateCutMeshToolEdit ( child );

                XtMapWidget ( child );
            } else {
                XtUnmapWidget   ( child );
            }
        }
    /*** Hide all children widgets if no object is selected ***/
    } else {
        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];

            if ( XtIsRealized ( child ) ) {
                XtUnmapWidget ( child );
            } else {
                /*** start with children made invisible, because no ***/
                /*** object is selected when application starts.  ***/
                XtVaSetValues ( child, XmNmappedWhenManaged, False, NULL );
            }
        }
    }
}

/******************************************************************************/
Widget createCurrentMouseTool ( Widget parent, G3DUI *gui, 
                                               char *name,
                                               Dimension x,
                                               Dimension y,
                                               Dimension width,
                                               Dimension height ) {
    Pixel background, foreground;
    Widget scr, frm;


    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    scr = XmVaCreateManagedScrolledWindow ( parent, name,
                                            XmNx, x,
                                            XmNy, y,
                                            XmNwidth , width,
                                            XmNheight, height,
                                            XmNscrollingPolicy, XmAUTOMATIC,
                                            XmNspacing, 0x00,
                                            XmNforeground, foreground,
                                            XmNbackground, background,
                                            XmNtopShadowColor, background,
                                            XmNbottomShadowColor, background,
                                            XmNshadowThickness, 0x01,
                                            NULL );

    frm = XmVaCreateManagedForm ( scr, name,
                                  XtNx, 0x00,
                                  XtNy, 0x00,
                                  XmNwidth , 0x140,
                                  XmNheight, 0x140,
                                  XmNforeground, foreground,
                                  XmNbackground, background,
                                  XmNborderWidth, 0x00,
                                  NULL );

    createPickToolEdit  ( frm, gui, PICKTOOL , 0x00, 0x00, 0x140, 0x140 );
    createKnifeToolEdit ( frm, gui, CUTMESHTOOL, 0x00, 0x00, 0x140, 0x140 );

    updateCurrentMouseTool ( frm );

    list_insert ( &gui->lmtools, frm );


    return scr;
}
