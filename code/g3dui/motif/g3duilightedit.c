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
#include <config.h>
#include <g3dui_motif.h>

/******************************************************************************/
static void specularityChangeCbk ( Widget w, XtPointer client,
                                             XtPointer call ) {
    XmColorEditCallbackStruct *ces = ( XmColorEditCallbackStruct * ) call;
    uint32_t red   = ( ces->pixel & 0x00FF0000 ) >> 0x10,
             green = ( ces->pixel & 0x0000FF00 ) >> 0x08,
             blue  = ( ces->pixel & 0x000000FF );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_lightSpecularityChangeCbk ( gui, red  ,
                                                  green,
                                                  blue  );
}

/******************************************************************************/
static void updateSpecularityPanel ( Widget w, G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    Display *dis = XtDisplay ( w );
    Window win = XDefaultRootWindow ( dis );
    XWindowAttributes attr;
    WidgetList children;
    Cardinal nc;
    uint32_t i;

    XGetWindowAttributes ( dis, win, &attr );

    XtVaGetValues ( w, XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];

        if ( obj && ( obj->type == G3DLIGHTTYPE ) ) {
            G3DLIGHT *lig = ( G3DLIGHT * ) obj;

            if ( XtClass ( child ) == xmPushButtonWidgetClass ) {
                if ( ( strcmp ( EDITSPECULARITYCOLOR, XtName ( child ) ) == 0x00 ) ) {
                    uint32_t R = ( lig ) ? ( lig->speccol.r ) : 0x00,
                             G = ( lig ) ? ( lig->speccol.g ) : 0x00,
                             B = ( lig ) ? ( lig->speccol.b ) : 0x00;
                    uint32_t pixel = 0x00,
                             color = ( R << 0x10 ) | ( G << 0x08 ) | B;

                    switch ( attr.depth ) {
                        case 0x10 :
                            R = ( R >> 0x03 );
                            G = ( G >> 0x02 );
                            B = ( B >> 0x03 );

                            pixel = color;
                        break;

                        case 0x18 :
                        case 0x20 :
                            pixel = ( R << 0x10 ) | ( G << 0x08 ) | B;
                        break;

                        default :
                        break;
                    }

                    XtVaSetValues ( child, XmNbackground, pixel, NULL );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static Widget createSpecularityPanel ( Widget parent, G3DUI *gui,
                                                      char *name,
                                                      Dimension x,
                                                      Dimension y,
                                                      Dimension width,
                                                      Dimension height ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Widget col, pan;

    pan = XmVaCreateManagedForm ( parent, name,
                                  XmNx, x,
                                  XmNy, y,
                                  XmNwidth, width,
                                  XmNheight, height,
                                  XmNuserData, gui,
                                  NULL );

          /*** Use Solid color as texture ***/
          createSimpleLabel ( pan, gui,
                                   EDITSPECULARITYCOLOR,
                                     0,  0, 96, 18 );
          /*** Pick color button ***/
    col = createColorButton ( pan, gui,
                                   EDITSPECULARITYCOLOR,
                                   104,  0, 96, 18,
                                   specularityChangeCbk );


    return pan;
}

/******************************************************************************/
static void diffuseChangeCbk ( Widget w, XtPointer client,
                                         XtPointer call ) {
    XmColorEditCallbackStruct *ces = ( XmColorEditCallbackStruct * ) call;
    uint32_t red   = ( ces->pixel & 0x00FF0000 ) >> 0x10,
             green = ( ces->pixel & 0x0000FF00 ) >> 0x08,
             blue  = ( ces->pixel & 0x000000FF );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_lightDiffuseChangeCbk ( gui, red  ,
                                              green,
                                              blue  );
}

/******************************************************************************/
static void diffuseIntensityCbk ( Widget w, XtPointer client,
                                            XtPointer call ) {
    float intensity = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DLIGHTTYPE ) ) {
        G3DLIGHT *lig = ( G3DLIGHT * ) obj;

        lig->intensity = intensity;

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
static void updateColorPanel ( Widget w, G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    Display *dis = XtDisplay ( w );
    Window win = XDefaultRootWindow ( dis );
    XWindowAttributes attr;
    WidgetList children;
    Cardinal nc;
    uint32_t i;

    XGetWindowAttributes ( dis, win, &attr );

    XtVaGetValues ( w, XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];

        if ( obj && ( obj->type == G3DLIGHTTYPE ) ) {
            G3DLIGHT *lig = ( G3DLIGHT * ) obj;

            if ( XtClass ( child ) == xmPushButtonWidgetClass ) {
                if ( ( strcmp ( EDITDIFFUSECOLOR, XtName ( child ) ) == 0x00 ) ) {
                    uint32_t R = ( lig ) ? ( lig->diffcol.r ) : 0x00,
                             G = ( lig ) ? ( lig->diffcol.g ) : 0x00,
                             B = ( lig ) ? ( lig->diffcol.b ) : 0x00;
                    uint32_t pixel = 0x00,
                             color = ( R << 0x10 ) | ( G << 0x08 ) | B;

                    switch ( attr.depth ) {
                        case 0x10 :
                            R = ( R >> 0x03 );
                            G = ( G >> 0x02 );
                            B = ( B >> 0x03 );

                            pixel = color;
                        break;

                        case 0x18 :
                        case 0x20 :
                            pixel = ( R << 0x10 ) | ( G << 0x08 ) | B;
                        break;

                        default :
                        break;
                    }

                    XtVaSetValues ( child, XmNbackground, pixel, NULL );
                }
            }

            /*** Crashes if I don't use the if statement below. Don't know why ***/
            if ( XtClass ( child ) == xmSpinButtonWidgetClass ) {
                if ( strcmp ( EDITLIGHTINTENSITY, XtName ( child ) ) == 0x00 ) {
                    XmSpinButtonSetValue ( child, lig->intensity );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static Widget createDiffuseColorPanel ( Widget parent, G3DUI *gui,
                                                       char *name,
                                                       Dimension x,
                                                       Dimension y,
                                                       Dimension width,
                                                       Dimension height ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Widget lab, col, pan;

    pan = XmVaCreateManagedForm ( parent, name,
                                  XmNx, x,
                                  XmNy, y,
                                  XmNwidth, width,
                                  XmNheight, height,
                                  XmNuserData, gui,
                                  NULL );

          /*** Use Solid color as texture ***/
          createSimpleLabel ( pan, gui,
                                   EDITDIFFUSECOLOR,
                                     0,  0, 96, 18 );
          /*** Pick color button ***/
    col = createColorButton ( pan, gui,
                                   EDITDIFFUSECOLOR,
                                   104,  0, 96, 18,
                                   diffuseChangeCbk );

          createFloatText   ( pan, gui,
                                   EDITLIGHTINTENSITY,
                                   0, 48, 96, 96,
                                   diffuseIntensityCbk );


    return pan;
}



/******************************************************************************/
void updateLightEdit ( Widget w, G3DUI *gui ) {
    WidgetList children;
    Cardinal nc;
    uint32_t i;

    XtVaGetValues ( w, XmNchildren, &children, 
                       XmNnumChildren, &nc, NULL );

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];

        if ( strcmp ( EDITSPECULARITY, XtName ( child ) ) == 0x00 ) {
            updateColorPanel ( child, gui );
        }

        if ( strcmp ( EDITDIFFUSE    , XtName ( child ) ) == 0x00 ) {
            updateColorPanel ( child, gui );
        }
    }
}

/******************************************************************************/
Widget createLightEdit ( Widget parent, G3DUI *gui,
                                        char *name,
                                        Dimension x,
                                        Dimension y,
                                        Dimension width,
                                        Dimension height ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Widget tab;

    tab =  XmVaCreateManagedTabbedBook ( parent, name,
                                         XmNx,  x,
                                         XmNy,  y,
                                         XmNwidth, width,
                                         XmNheight, height,
                                         XmNfontList, gmt->fontlist,
                                         NULL );

    createDiffuseColorPanel ( tab, gui, EDITDIFFUSE     , 0, 0, width, height );
    createSpecularityPanel  ( tab, gui, EDITSPECULARITY , 0, 0, width, height );

    updateLightEdit ( tab, gui );


    return tab;
}
