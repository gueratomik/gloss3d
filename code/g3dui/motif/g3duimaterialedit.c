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
static void displacementToggleCbk ( Widget w, XtPointer client, 
                                              XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_materialDisplacementToggleCbk ( gui );
}

/******************************************************************************/
static void setDisplacementImageOkCbk ( Widget w, XtPointer client,
                                                  XtPointer call ) {
    XmFileSelectionBoxCallbackStruct *cbs =
                        ( XmFileSelectionBoxCallbackStruct * ) call;
    char *filename = ( char * ) XmStringUnparse ( cbs->value,
                                                  XmFONTLIST_DEFAULT_TAG,
                                                  XmCHARSET_TEXT,
                                                  XmCHARSET_TEXT,
                                                  NULL, 0x00, XmOUTPUT_ALL );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_materialDisplacementImageCbk ( gui, filename );

    XtFree ( filename );

    XtUnmanageChild ( w );
}

/******************************************************************************/
static void setDisplacementImageCancelCbk ( Widget w, XtPointer client,
                                                      XtPointer call ) {
    XtUnmanageChild ( w );
}

/******************************************************************************/
static void displacementImageCbk ( Widget w, XtPointer client, 
                                             XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    Widget dialog;

    dialog = XmCreateFileSelectionDialog ( w, "Open Displacement Image", NULL, 0x00 );

    XtAddCallback ( dialog, XmNokCallback    , setDisplacementImageOkCbk    , gui );
    XtAddCallback ( dialog, XmNcancelCallback, setDisplacementImageCancelCbk, gui );


    XtManageChild ( dialog );
}

/******************************************************************************/
static void displacementStrengthCbk ( Widget w, XtPointer client, 
                                                XtPointer call ) {
    float strength = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_materialDisplacementStrengthCbk ( gui, strength );
}

/******************************************************************************/
void updateDisplacementPanel ( Widget w, G3DUI *gui ) {
    WidgetList children;
    Cardinal nc;
    uint32_t i;

    XtVaGetValues ( w, XmNchildren, &children, 
                       XmNnumChildren, &nc, NULL );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];

        if ( gui->selmat ) {
            G3DMATERIAL *mat = gui->selmat;
            char buf[0x10];

            /*XtVaSetValues ( child, XmNsensitive, True, NULL );*/

            /*** Update toggle button, the one allowing the user to choose ***/
            /*** if the material color should be solid or from image file. ***/
            if ( XtClass ( child ) == xmSpinButtonWidgetClass ) {
                if ( strcmp ( XtName ( child ), EDITDISPLACEMENTSTRENGTH ) == 0x00 ) {
                    XmSpinButtonSetValue ( child, mat->displacement_strength );
                }
            }

            /*** Update toggle button, the one allowing the user to choose ***/
            /*** if the material color should be solid or from image file. ***/
            if ( XtClass ( child ) == xmToggleButtonWidgetClass ) {
                if ( strcmp ( XtName ( child ), EDITDISPLACEMENTIMAGE ) == 0x00 ) {
                    if ( mat->flags & DISPLACEMENT_USEIMAGECOLOR ) {
                        XmToggleButtonSetState ( child, True, False );
                    } else {
                        XmToggleButtonSetState ( child, False, False );
                    }
                }

                if ( strcmp ( XtName ( child ), EDITDISPLACEMENTENABLED ) == 0x00 ) {
                    if ( mat->flags & DISPLACEMENT_ENABLED ) {
                        XmToggleButtonSetState ( child, True, False );
                    } else {
                        XmToggleButtonSetState ( child, False, False );
                    }
                }
            }

            if ( XtClass ( child ) == xmPushButtonWidgetClass ) {
                /*** The pushButton name should be the image name ***/
                if ( strcmp ( XtName ( child ), EDITDISPLACEMENTIMAGE ) == 0x00 ) {
                    XmString str;

                    if ( mat->displacement.image ) {
                        char *imgpath, *imgname;

                        imgpath = g3dcore_strclone ( mat->displacement.image->name );

                        /*** We just keep the image name, not the whole path and ***/
                        /*** display it as the button label ***/
                        imgname = basename ( imgpath );
  
                        str = XmStringCreate ( imgname, XmFONTLIST_DEFAULT_TAG );
 
                        XtVaSetValues ( child, XmNlabelString, str, NULL );

                        free ( imgpath );
                    } else {
                        str = XmStringCreate ( EDITDISPLACEMENTIMAGE, XmFONTLIST_DEFAULT_TAG );

                        XtVaSetValues ( child, XmNlabelString, str, NULL );
                    }

                    XmStringFree ( str );
                }
            }
        } else {
/*printf("Wudget: %s \n", XtName ( child ) );*/
            /*if ( XtClass ( child ) != xmPushButtonWidgetClass ) {*/
               /*XtSetSensitive ( child, False );*/
            /*}*/
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static Widget createDisplacementPanel  ( Widget parent, G3DUI *gui,
                                                        char *name,
                                                        Dimension x,
                                                        Dimension y,
                                                        Dimension width,
                                                        Dimension height ) {
    Widget lab, col, pan, btn;

    /*pan = XmVaCreateManagedRowColumn ( parent, name,
                                       XmNx, x,
                                       XmNy, y,
                                       XmNwidth, width,
                                       XmNheight, height,
                                       XmNradioBehavior, True,
                                       XmNorientation, XmHORIZONTAL,
                                       XmNresizeWidth, False,
                                       XmNnumColumns, 0x02,
                                       NULL );*/

    pan = XmVaCreateForm ( parent, name,
                                   XtNx, x,
                                   XtNy, y,
                                   XtNwidth , width,
                                   XtNheight, height,
                                   XmNborderWidth, 0x00,
                                   NULL );

    createToggleLabel  ( pan, gui, EDITDISPLACEMENTENABLED,   
                                0,  0,
                              192, 18, displacementToggleCbk );


    createFloatText    ( pan, gui, EDITDISPLACEMENTSTRENGTH,
                                0,  24,
                               96,
                               48, displacementStrengthCbk );

    /*** Use image as displacement texture ***/
    createRadioLabel   ( pan, gui, EDITDISPLACEMENTIMAGE,
                                0, 48, 
                               96, 18, NULL );

    createPushButton   ( pan, gui, EDITDISPLACEMENTIMAGE,
                               96,  48, 
                               96,  18, displacementImageCbk );

    XtManageChild ( pan );


    return pan;
}

/******************************************************************************/
static void setSpecularityColorCbk ( Widget w, XtPointer client,
                                               XtPointer call ) {
    XmColorEditCallbackStruct *ces = ( XmColorEditCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;

    uint32_t red   = ( ces->pixel & 0x00FF0000 ) >> 0x10,
             green = ( ces->pixel & 0x0000FF00 ) >> 0x08,
             blue  = ( ces->pixel & 0x000000FF );

    if ( gui->selmat ) {
        G3DMATERIAL *mat = gui->selmat;

        common_g3dui_materialColorChangeCbk ( gui, &mat->specular, red,
                                                                   green,
                                                                   blue );
    }
}

/******************************************************************************/
static void setSpecularityLevelCbk ( Widget w, XtPointer client,
                                               XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    int val;

    XmScaleGetValue ( w, &val );

    common_g3dui_materialSpecularityLevelCbk ( gui, ( float ) val / 255.0f );
}

/******************************************************************************/
static void setSpecularityShininessCbk ( Widget w, XtPointer client,
                                                   XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    int val;

    XmScaleGetValue ( w, &val );

    common_g3dui_materialSpecularityShininessCbk ( gui, val );
}

/******************************************************************************/
void updateSpecularityPanel ( Widget w, G3DUI *gui ) {
    WidgetList children;
    Cardinal nc;
    uint32_t i;

    XtVaGetValues ( w, XmNchildren, &children, 
                       XmNnumChildren, &nc, NULL );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];

        if ( gui->selmat ) {
            G3DMATERIAL *mat = gui->selmat;

            /*XtVaSetValues ( child, XmNsensitive, True, NULL );*/

            /*** Update toggle button, the one allowing the user to choose ***/
            /*** if the material color should be solid or from image file. ***/
            if ( XtClass ( child ) == xmScaleWidgetClass ) {
                if ( strcmp ( XtName ( child ), EDITSPECULARITYSHININESS ) == 0x00 ) {
                    uint32_t val = ( mat ) ? mat->shininess : 0x00;

                    XtVaSetValues ( child, XmNvalue, val, NULL );
                }

                if ( strcmp ( XtName ( child ), EDITSPECULARITYLEVEL ) == 0x00 ) {
                    uint32_t val = ( mat ) ? mat->specular_level * 255.0f : 0x00;

                    XtVaSetValues ( child, XmNvalue, val, NULL );
                }
            }

            if ( XtClass ( child ) == xmPushButtonWidgetClass ) {
                /*** The pushButton name should be the image name ***/
                if ( strcmp ( XtName ( child ), EDITSPECULARITYCOLOR ) == 0x00 ) {
                    uint32_t pixel = 0x00;
                    G3DRGBA rgba;

                    g3dcolor_toRGBA ( &mat->specular.solid, &rgba );

                    pixel = ( rgba.r << 0x10 ) | ( rgba.g << 0x08 ) | rgba.b;

                    XtVaSetValues ( child, XmNbackground, pixel, NULL );
                }
            }
        } else {
/*printf("Wudget: %s \n", XtName ( child ) );*/
            /*if ( XtClass ( child ) != xmPushButtonWidgetClass ) {*/
               /*XtSetSensitive ( child, False );*/
            /*}*/
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static Widget createSpecularityPanel   ( Widget parent, G3DUI *gui,
                                                        char *name,
                                                        Dimension x,
                                                        Dimension y,
                                                        Dimension width,
                                                        Dimension height ) {
    Widget lab, col, pan, btn;

    /*pan = XmVaCreateManagedRowColumn ( parent, name,
                                       XmNx, x,
                                       XmNy, y,
                                       XmNwidth, width,
                                       XmNheight, height,
                                       XmNradioBehavior, True,
                                       XmNorientation, XmHORIZONTAL,
                                       XmNresizeWidth, False,
                                       XmNnumColumns, 0x02,
                                       NULL );*/

    pan = XmVaCreateForm ( parent, name,
                                   XtNx, x,
                                   XtNy, y,
                                   XtNwidth , width,
                                   XtNheight, height,
                                   XmNborderWidth, 0x00,
                                   NULL );

          createSimpleLabel     ( pan, gui, EDITSPECULARITYSHININESS,
                                         0,  0,  96, 18 );

          createHorizontalScale ( pan, gui, 
                                       EDITSPECULARITYSHININESS,
                                       104,  0, 208, 18,
                                       0.0f, 128.0f, 1.0f,
                                       setSpecularityShininessCbk );

          createSimpleLabel     ( pan, gui, 
                                       EDITSPECULARITYLEVEL,
                                         0, 24,  96, 18 );

          createHorizontalScale ( pan, gui, 
                                       EDITSPECULARITYLEVEL,
                                       104, 24, 208, 18,
                                       0.0f, 255.0f, 1.0f,
                                       setSpecularityLevelCbk );

          createColorButton     ( pan, gui,
                                       EDITSPECULARITYCOLOR,
                                       104, 48, 32, 18,
                                       setSpecularityColorCbk );

    XtManageChild ( pan );


    return pan;
}

/******************************************************************************/
static void setDiffuseImageOkCbk ( Widget w, XtPointer client,
                                             XtPointer call ) {
    XmFileSelectionBoxCallbackStruct *cbs =
                        ( XmFileSelectionBoxCallbackStruct * ) call;
    char *filename = ( char * ) XmStringUnparse ( cbs->value,
                                                  XmFONTLIST_DEFAULT_TAG,
                                                  XmCHARSET_TEXT,
                                                  XmCHARSET_TEXT,
                                                  NULL, 0x00, XmOUTPUT_ALL );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_materialChooseImageCbk ( gui, filename );

    XtFree ( filename );

    XtUnmanageChild ( w );
}

/******************************************************************************/
static void setDiffuseImageCancelCbk ( Widget w, XtPointer client,
                                                 XtPointer call ) {
    XtUnmanageChild ( w );
}

/******************************************************************************/
static void setDiffuseImageCbk ( Widget w, XtPointer client,
                                           XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    Widget dialog;

    dialog = XmCreateFileSelectionDialog ( w, "Open Color Image", NULL, 0x00 );

    XtAddCallback ( dialog, XmNokCallback    , setDiffuseImageOkCbk    , gui );
    XtAddCallback ( dialog, XmNcancelCallback, setDiffuseImageCancelCbk, gui );


    XtManageChild ( dialog );
}

/******************************************************************************/
static void useDiffuseImageCbk ( Widget w, XtPointer client,
                                           XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_materialImageColorCbk ( gui );
}

/******************************************************************************/
static void setDiffuseColorCbk ( Widget w, XtPointer client,
                                           XtPointer call ) {
    XmColorEditCallbackStruct *ces = ( XmColorEditCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;

    uint32_t red   = ( ces->pixel & 0x00FF0000 ) >> 0x10,
             green = ( ces->pixel & 0x0000FF00 ) >> 0x08,
             blue  = ( ces->pixel & 0x000000FF );

    if ( gui->selmat ) {
        G3DMATERIAL *mat = gui->selmat;

        common_g3dui_materialColorChangeCbk ( gui, &mat->diffuse, red, 
                                                                  green,
                                                                  blue );
    }
}

/******************************************************************************/
static void useDiffuseColorCbk ( Widget w, XtPointer client,
                                           XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_materialSolidColorCbk ( gui );
}

/******************************************************************************/
void updateDiffuseColorPanel ( Widget w, G3DUI *gui ) {
    WidgetList children;
    Cardinal nc;
    uint32_t i;

    XtVaGetValues ( w, XmNchildren, &children, 
                       XmNnumChildren, &nc, NULL );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];

        if ( gui->selmat ) {
            G3DMATERIAL *mat = gui->selmat;

            /*XtVaSetValues ( child, XmNsensitive, True, NULL );*/

            /*** Update toggle button, the one allowing the user to choose ***/
            /*** if the material color should be solid or from image file. ***/
            if ( XtClass ( child ) == xmToggleButtonWidgetClass ) {
                if ( strcmp ( XtName ( child ), EDITCOLORSOLID ) == 0x00 ) {
                    if ( mat->flags & DIFFUSE_USESOLIDCOLOR ) {
                        XmToggleButtonSetState ( child, True, False );
                    } else {
                        XmToggleButtonSetState ( child, False, False );
                    }
                }

                if ( strcmp ( XtName ( child ), EDITCOLORIMAGE ) == 0x00 ) {
                    if ( mat->flags & DIFFUSE_USEIMAGECOLOR ) {
                        XmToggleButtonSetState ( child, True, False );
                    } else {
                        XmToggleButtonSetState ( child, False, False );
                    }
                }
            }

            if ( XtClass ( child ) == xmPushButtonWidgetClass ) {
                /*** The pushButton name should be the image name ***/
                if ( strcmp ( XtName ( child ), EDITDIFFUSECOLOR ) == 0x00 ) {
                    uint32_t pixel = 0x00;
                    G3DRGBA rgba;

                    g3dcolor_toRGBA ( &mat->diffuse.solid, &rgba );

                    pixel = ( rgba.r << 0x10 ) | ( rgba.g << 0x08 ) | rgba.b;

                    XtVaSetValues ( child, XmNbackground, pixel, NULL );
                }

                /*** The pushButton name should be the image name ***/
                if ( strcmp ( XtName ( child ), EDITDIFFUSEIMAGE ) == 0x00 ) {
                    XmString str;

                    if ( mat->diffuse.image ) {
                        char *imgpath, *imgname;

                        imgpath = g3dcore_strclone ( mat->diffuse.image->name );

                        /*** We just keep the image name, not the whole path and ***/
                        /*** display it as the button label ***/
                        imgname = basename ( imgpath );
  
                        str = XmStringCreate ( imgname, XmFONTLIST_DEFAULT_TAG );
 
                        XtVaSetValues ( child, XmNlabelString, str, NULL );

                        free ( imgpath );
                    } else {
                        str = XmStringCreate ( EDITCOLORIMAGE, XmFONTLIST_DEFAULT_TAG );

                        XtVaSetValues ( child, XmNlabelString, str, NULL );
                    }

                    XmStringFree ( str );
                }
            }
        } else {
/*printf("Wudget: %s \n", XtName ( child ) );*/
            /*if ( XtClass ( child ) != xmPushButtonWidgetClass ) {*/
               /*XtSetSensitive ( child, False );*/
            /*}*/
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static Widget createDiffuseColorPanel  ( Widget parent, G3DUI *gui,
                                                        char *name,
                                                        Dimension x,
                                                        Dimension y,
                                                        Dimension width,
                                                        Dimension height ) {
    Widget lab, col, pan;
    
    pan = XmVaCreateManagedRowColumn ( parent, name,
                                       XmNx, x,
                                       XmNy, y,
                                       XmNwidth, width,
                                       XmNheight, height,
                                       XmNradioBehavior, True,
                                       XmNorientation, XmHORIZONTAL,
                                       XmNresizeWidth, False,
                                       XmNnumColumns, 0x02,
                                       NULL );

          /*** Use color as texture ***/
    createRadioLabel  ( pan, gui,
                             EDITCOLORSOLID,
                               0,  0, 96, 18,
                             useDiffuseColorCbk );

    /*** Color chooser button ***/
    createColorButton ( pan, gui,
                             EDITDIFFUSECOLOR,
                             104,  0, 96, 18,
                             setDiffuseColorCbk );

          /*** Use image as texture ***/
    createRadioLabel  ( pan, gui,
                             EDITCOLORIMAGE,
                               0, 24, 96, 18,
                             useDiffuseImageCbk );

          /*** Image chooser button **/
    createPushButton  ( pan, gui,
                             EDITDIFFUSEIMAGE,
                             104, 24, 96, 18,
                             setDiffuseImageCbk );


    return pan;
}

/******************************************************************************/
static void setMaterialNameCbk ( Widget w, XtPointer client, XtPointer call ) {
    char *name = XmTextGetString ( w );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_materialNameCbk ( gui, name );

    XtFree ( name );
}

/******************************************************************************/
void updateMaterialChannels ( Widget w, G3DUI *gui ) {
    WidgetList children;
    Cardinal nc;
    uint32_t i;

    XtVaGetValues ( w, XmNchildren, &children, 
                       XmNnumChildren, &nc, NULL );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];

        if ( strcmp ( XtName ( child ), EDITDIFFUSE     ) == 0x00 ) {
            updateDiffuseColorPanel ( child, gui );
        }

        if ( strcmp ( XtName ( child ), EDITSPECULARITY ) == 0x00 ) {
            updateSpecularityPanel ( child, gui );
        }

        if ( strcmp ( XtName ( child ), EDITDISPLACEMENT ) == 0x00 ) {
            updateDisplacementPanel ( child, gui );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
void updateMaterialEdit ( Widget w, G3DUI *gui ) {
    WidgetList children;
    Cardinal nc;
    uint32_t i;

    XtVaGetValues ( w, XmNchildren, &children, 
                       XmNnumChildren, &nc, NULL );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];

        if ( gui->selmat ) {
            G3DMATERIAL *mat = gui->selmat;

            XtVaSetValues ( child, XmNsensitive, True, NULL );

            if ( XtClass ( child ) == xmTextWidgetClass ) {
                if ( strcmp ( XtName ( child ), EDITMATERIALNAME ) == 0x00 ) {
                    char buf[0x100];

                    snprintf ( buf, 0x100, "%s", mat->name );

                    XmTextSetString ( child, buf );
                }
            }
        } else {
            XtVaSetValues ( child, XmNsensitive, False, NULL );
        }

        if ( XtClass ( child ) == xmTabbedBookWidgetClass ) {
           if ( strcmp ( XtName ( child ), EDITMATERIALCHANNELS ) == 0x00 ) {
                updateMaterialChannels ( child, gui );
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
Widget createMaterialEdit ( Widget parent, G3DUI *gui,
                                           char *name,
                                           Dimension x,
                                           Dimension y,
                                           Dimension width,
                                           Dimension height ) {
    Pixel background, foreground;
    Widget frm, tab;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    frm = XmVaCreateForm ( parent, name,
                                   XtNx, x,
                                   XtNy, y,
                                   XtNwidth , width,
                                   XtNheight, height,
                                   XmNforeground, foreground,
                                   XmNbackground, background,
                                   XmNborderWidth, 0x00,
                                   NULL );

    createCharText ( frm, gui, EDITMATERIALNAME, 0, 0, 96, 96, setMaterialNameCbk );

    tab =  XmVaCreateManagedTabbedBook ( frm, EDITMATERIALCHANNELS,
                                         XmNx,  0,
                                         XmNy, 24,
                                         XmNwidth, width,
                                         XmNheight, height,
                                         XmNfontList, gmt->fontlist,
                                         NULL );

    createDiffuseColorPanel ( tab, gui, EDITDIFFUSE     , 0, 0, width, height );
    createSpecularityPanel  ( tab, gui, EDITSPECULARITY , 0, 0, width, height );
    createDisplacementPanel ( tab, gui, EDITDISPLACEMENT, 0, 0, width, height );

    /*createReflectionPanel   ( mst, 0x00, 0x00, 0x100, 0x100 );
    createRefractionPanel   ( mst, 0x00, 0x00, 0x100, 0x100 );*/

    list_insert ( &gui->lmatedit, frm );

    updateMaterialEdit ( frm, gui );

    XtManageChild ( frm );

    return frm;
}
