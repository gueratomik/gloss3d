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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <unistd.h>
/*** For basename() ***/
#include <libgen.h>

/******************************************************************************/
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

/******************************************************************************/
#include <math.h>

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
#include <Xm/FileSB.h> /*** File selection dialog ***/
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/PushB.h>
#include <Xm/Manager.h>
#include <Xm/ManagerP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/Scale.h>
#include <Xm/Text.h>
#include <Xm/Label.h>
#include <Xm/VaSimpleP.h>

/******************************************************************************/
#include <list.h>

/******************************************************************************/
#include <TabbedBook.h>
#include <ColorEdit.h>

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/GLwMDrawA.h>

/******************************************************************************/
#include <g3d.h>
#include <r3d.h>
#include <r3dnet.h>
#include <g3durmanager.h>

/******************************************************************************/
#include <g3dmouse.h>

/******************************************************************************/
#include <g3dui.h>

/******************************************************************************/
#include <MaterialEdit.h>

/******************************************************************************/
static void colorchange ( uint32_t pixel, void *data ) {
    G3DUI *gui = ( G3DUI * ) data;
    G3DMATERIAL *mat = gui->selmat;

    if ( mat ) {
        LIST *ltmp = gui->lmatlist;
        G3DRGBA rgba;

        rgba.r = ( ( pixel & 0x00FF0000 ) >> 0x10 );
        rgba.g = ( ( pixel & 0x0000FF00 ) >> 0x08 );
        rgba.b = ( ( pixel & 0x000000FF )         );
        rgba.a = 0x00;

        g3drgba_toColor ( &rgba, &mat->diffuse.solid );

        while ( ltmp ) {
            Widget matlist = ( Widget ) ltmp->data;

            XmMaterialListUpdatePreviewFromMaterial ( matlist, mat );

            ltmp = ltmp->next;
        }

        redrawGLViews ( gui );
    }
}

/******************************************************************************/
static void specularitychange ( uint32_t pixel, void *data ) {
    G3DUI *gui = ( G3DUI * ) data;
    G3DMATERIAL *mat = gui->selmat;

    if ( mat ) {
        LIST *ltmp = gui->lmatlist;
        G3DRGBA rgba;

        rgba.r = ( ( pixel & 0x00FF0000 ) >> 0x10 );
        rgba.g = ( ( pixel & 0x0000FF00 ) >> 0x08 );
        rgba.b = ( ( pixel & 0x000000FF )         );
        rgba.a = 0x00;

        g3drgba_toColor ( &rgba, &mat->specular.solid );

        while ( ltmp ) {
            Widget matlist = ( Widget ) ltmp->data;

            XmMaterialListUpdatePreviewFromMaterial ( matlist, mat );

            ltmp = ltmp->next;
        }

        redrawGLViews ( gui );
    }
}

/******************************************************************************/
static void updateColorPanel ( Widget w, G3DMATERIAL *mat ) {
    WidgetList children;
    G3DUI *gui = NULL;
    Cardinal nc;
    uint32_t i;

    XtVaGetValues ( w, XmNuserData, &gui,
                       XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];

        if ( strcmp ( "Solid Color Value", XtName ( child ) ) == 0x00 ) {
            uint32_t pixel = 0x00;
            G3DRGBA rgba;

            if ( mat ) {
                g3dcolor_toRGBA ( &mat->diffuse.solid, &rgba );

                pixel = ( rgba.r << 0x10 ) | ( rgba.g << 0x08 ) | rgba.b;
            }

            XtVaSetValues ( child, XmNbackground, pixel, NULL );
        }

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
            if ( strcmp ( XtName ( child ), EDITCOLORIMAGE ) == 0x00 ) {
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
    }

    gui->lock = 0x00;
}

/******************************************************************************/
void colimgcommitcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmFileSelectionBoxCallbackStruct *cbs =
                        ( XmFileSelectionBoxCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    G3DMATERIAL *mat = gui->selmat;
    char *filename;

    if ( mat ) {
        G3DIMAGE *colimg;

        filename = ( char * ) XmStringUnparse ( cbs->value,
                                                XmFONTLIST_DEFAULT_TAG,
                                                XmCHARSET_TEXT,
                                                XmCHARSET_TEXT,
                                                NULL, 0x00, XmOUTPUT_ALL );

        colimg = g3dimage_newFromJpeg ( filename, 0x01 );

        if ( colimg ) {
            LIST *ltmpmat = gui->lmatlist;

            g3dmaterial_addDiffuseImage ( mat, colimg );

            /*** Update Meshes that have this material   ***/
            /*** to rebuild Texture Coordinates. This is ***/
            /*** needed for buffered subdivided meshes ***/
            g3dmaterial_updateMeshes ( mat, gui->flags );

            while ( ltmpmat ) {
                Widget matlist = ( Widget ) ltmpmat->data;

                XmMaterialListUpdatePreviewFromMaterial ( matlist, mat );

                ltmpmat = ltmpmat->next;
            }

            updateAllMaterialSettings ( gui );

            g3dui_addImage ( gui, colimg );

            redrawGLViews ( gui );
        } else {
            fprintf ( stderr, "Could not load image %s\n", filename );
        }

        XtFree ( filename );
    }

    XtUnmanageChild ( w );
}

/******************************************************************************/
void colimgcancelcbk ( Widget w, XtPointer client, XtPointer call ) {
    XtUnmanageChild ( w );
}

/******************************************************************************/
static void colimgcbk  ( Widget w, XtPointer client, XtPointer call ) {
    XmScaleCallbackStruct *scs = ( XmScaleCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;

    Widget dialog;

    dialog = XmCreateFileSelectionDialog ( w, "Open Color Image", NULL, 0x00 );

    XtAddCallback ( dialog, XmNokCallback    , colimgcommitcbk, gui );
    XtAddCallback ( dialog, XmNcancelCallback, colimgcancelcbk, gui );


    XtManageChild ( dialog );
}

/******************************************************************************/
static void color_usesolidcolorcbk ( Widget w, XtPointer client,
                                               XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DMATERIAL *mat = gui->selmat;

    if ( gui->lock ) return;

    if ( mat ) {
        g3dmaterial_enableDiffuseSolidColor ( mat );

        /*** Redraw Material List widget Previews ***/
        updateSelectedMaterialPreview ( gui );

        redrawGLViews ( gui );
    }
}

/******************************************************************************/
static void color_useimagecolorcbk ( Widget w, XtPointer client,
                                               XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DMATERIAL *mat = gui->selmat;

    if ( gui->lock ) return;

    if ( mat ) {
        g3dmaterial_enableDiffuseImageColor ( mat );

        /*** Redraw Material List widget Previews ***/
        updateSelectedMaterialPreview ( gui );

        redrawGLViews ( gui );
    }
}

/******************************************************************************/
static Widget createColorPanel ( G3DUI *gui, Widget parent,
                                 Dimension x    , Dimension y,
                                 Dimension width, Dimension height ) {
    Widget lab, col, pan;
    
    pan = XmVaCreateManagedRowColumn ( parent, "Color",
                                       XmNx, x,
                                       XmNy, y,
                                       XmNwidth, width,
                                       XmNheight, height,
                                       XmNradioBehavior, True,
                                       XmNorientation, XmHORIZONTAL,
                                       XmNresizeWidth, False,
                                       XmNnumColumns, 0x02,
                                       XmNuserData, gui,
                                       NULL );

    /*** Use color as texture ***/
    createToggleLabel ( pan, EDITCOLORSOLID, 0x00, 0x00,
                                             0x60, 0x12,
                                             color_usesolidcolorcbk );
    /*** Color chooser button ***/
    col = XmCreateColorEditLabel ( pan, "Solid Color Value",
                                        0x60, 0x00,
                                        0x20, 0x12,
                                        0x00, colorchange, gui );

    /*** Use image as texture ***/
    createToggleLabel ( pan, EDITCOLORIMAGE, 0x00, 0x48,
                                             0x60, 0x12,
                                             color_useimagecolorcbk );

    /*** Image chooser button **/
    createSimplePushButton ( pan, EDITCOLORIMAGE, 0x00, 0x04, 
                                                  0x80, 0x10, colimgcbk  );


    XtManageChild ( col );


    return pan;
}

/******************************************************************************/
void disimgcommitcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmFileSelectionBoxCallbackStruct *cbs =
                        ( XmFileSelectionBoxCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    G3DMATERIAL *mat = gui->selmat;
    char *filename;

    if ( mat ) {
        G3DIMAGE *disimg;

        filename = ( char * ) XmStringUnparse ( cbs->value,
                                            XmFONTLIST_DEFAULT_TAG,
                                            XmCHARSET_TEXT,
                                            XmCHARSET_TEXT,
                                            NULL, 0x00, XmOUTPUT_ALL );

        disimg = g3dimage_newFromJpeg ( filename, 0x01 );

        if ( disimg ) {
            g3dmaterial_addDisplacementImage ( mat, disimg );

            g3dmaterial_updateMeshes ( mat, gui->flags );

            updateAllMaterialSettings ( gui );

            g3dui_addImage ( gui, disimg );

            redrawGLViews ( gui );
        } else {
            fprintf ( stderr, "Could not load image %s\n", filename );
        }

        XtFree ( filename );
    }

    XtUnmanageChild ( w );
}

/******************************************************************************/
void disimgcancelcbk ( Widget w, XtPointer client, XtPointer call ) {
    XtUnmanageChild ( w );
}

/******************************************************************************/
static void disimgcbk  ( Widget w, XtPointer client, XtPointer call ) {
    XmScaleCallbackStruct *scs = ( XmScaleCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;

    Widget dialog;

    dialog = XmCreateFileSelectionDialog ( w, "Open Image", NULL, 0x00 );

    XtAddCallback ( dialog, XmNokCallback    , disimgcommitcbk, gui );
    XtAddCallback ( dialog, XmNcancelCallback, disimgcancelcbk, gui );


    XtManageChild ( dialog );
}

/******************************************************************************/
static void updateDisplacementChannel ( Widget w, G3DMATERIAL *mat ) {
    WidgetList children;
    Cardinal nc;
    uint32_t i;
    G3DUI *gui;

    XtVaGetValues ( w, XmNuserData, &gui,
                       XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];
        char buf[0x10];

        if ( XtClass ( child ) == xmTextWidgetClass ) {
            if ( strcmp ( XtName ( child ), EDITDISPLACEMENTSTRENGTH ) == 0x00 ) {
                snprintf ( buf, 0x10, "%f", mat->displacement_strength );

                XmTextSetString ( child, buf );
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
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void updateDisplacementPanel ( Widget w, G3DMATERIAL *mat ) {
    WidgetList children;
    Cardinal nc;
    uint32_t i;
    G3DUI *gui;

    XtVaGetValues ( w, XmNuserData, &gui,
                       XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];
        char buf[0x10];

        if ( XtClass ( child ) == xmToggleButtonWidgetClass ) {
            if ( strcmp ( XtName ( child ), EDITDISPLACEMENTENABLED ) == 0x00 ) {
                if ( mat->flags & DISPLACEMENT_ENABLED ) {
                    XmToggleButtonSetState ( child, True, False );
                } else {
                    XmToggleButtonSetState ( child, False, False );
                }
            }
        }

        if ( XtClass ( child ) == xmRowColumnWidgetClass ) {
            if ( strcmp ( XtName ( child ), EDITDISPLACEMENTCHANNEL ) == 0x00 ) {
                updateDisplacementChannel ( child, mat );
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void disstrengthcbk  ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DMATERIAL *mat = gui->selmat;

    if ( gui->lock ) return;

    if ( mat ) {
        char *value = XmTextGetString ( w );
        float displacement_strength = strtof ( value, NULL );

        mat->displacement_strength = displacement_strength;

        g3dmaterial_updateMeshes ( mat, gui->flags );


        redrawGLViews ( gui );
    }
}

/******************************************************************************/
static void displacement_useimagecolorcbk ( Widget w, XtPointer client,
                                                      XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DMATERIAL *mat = gui->selmat;

    if ( gui->lock ) return;

    if ( mat ) {
        g3dmaterial_enableDisplacementImageColor ( mat );

        g3dmaterial_updateMeshes ( mat, gui->flags );

        /*** Redraw Material List widget Previews ***/
        updateSelectedMaterialPreview ( gui );

        redrawGLViews ( gui );
    }
}

/******************************************************************************/
static void displacement_enablecbk ( Widget w, XtPointer client,
                                               XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DMATERIAL *mat = gui->selmat;

    if ( gui->lock ) return;

    if ( mat ) {
        if ( mat->flags & DISPLACEMENT_ENABLED ) {
            g3dmaterial_disableDisplacement ( mat );
        } else {
            g3dmaterial_enableDisplacement  ( mat );
        }

        g3dmaterial_updateMeshes ( mat, gui->flags );

        redrawGLViews ( gui );
    }
}

/******************************************************************************/
static Widget createDisplacementPanel ( G3DUI *gui, Widget parent,
                                        Dimension x    , Dimension y,
                                        Dimension width, Dimension height ) {
    Widget pan, frm;

    frm = XmVaCreateManagedForm ( parent, EDITDISPLACEMENT,
                                          XtNx, x,
                                          XtNy, y,
                                          XtNwidth , width,
                                          XtNheight, height,
                                          /*XmNforeground, foreground,
                                          XmNbackground, background,*/
                                          XmNborderWidth, 0x00,
                                          XmNuserData, gui,
                                          NULL );

    createToggleLabel ( frm, EDITDISPLACEMENTENABLED, 0x00, 0x00,
                                                      0xC0, 0x12,
                                                      displacement_enablecbk );

    pan = XmVaCreateManagedRowColumn ( frm, EDITDISPLACEMENTCHANNEL,
                                       XmNx, 0x00,
                                       XmNy, 0x14,
                                       XmNwidth, width,
                                       XmNheight, 0x80,
                                       XmNradioBehavior, True,
                                       XmNorientation, XmHORIZONTAL,
                                       XmNresizeWidth, False,
                                       XmNnumColumns, 0x02,
                                       XmNpacking, XmPACK_COLUMN,

                                       XmNuserData, gui,
                                       NULL );


    createFloatText ( pan, EDITDISPLACEMENTSTRENGTH, 0x00, 0x24,
                                                     0x80, 0x30, disstrengthcbk );

    /*** Use image as texture ***/
    createToggleLabel ( pan, EDITDISPLACEMENTIMAGE, 0x00, 0x48,
                                                    0x60, 0x12,
                                                    displacement_useimagecolorcbk );

    /*** Image chooser button **/
    createSimplePushButton ( pan, EDITDISPLACEMENTIMAGE, 0x00, 0x48, 
                                                         0x80, 0x10, disimgcbk  );


    return frm;
}

/******************************************************************************/
static void updateSpecularityPanel ( Widget w, G3DMATERIAL *mat ) {
    WidgetList children;
    Cardinal nc;
    uint32_t i;

    XtVaGetValues ( w, XmNchildren, &children, XmNnumChildren, &nc, NULL );

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];

        if ( strcmp ( "Solid Color Value", XtName ( child ) ) == 0x00 ) {
            uint32_t pixel = 0x00;
            G3DRGBA rgba;

            if ( mat ) {
                g3dcolor_toRGBA ( &mat->diffuse.solid, &rgba );

                pixel = ( rgba.r << 0x10 ) | ( rgba.g << 0x08 ) | rgba.b;
            }

            XtVaSetValues ( child, XmNbackground, pixel, NULL );
        }

        if ( strcmp ( "Shininess Value", XtName ( child ) ) == 0x00 ) {
            uint32_t val = ( mat ) ? mat->shininess : 0x00;

            XtVaSetValues ( child, XmNvalue, val, NULL );
        }
    }
}

/******************************************************************************/
static void shinecbk  ( Widget w, XtPointer client, XtPointer call ) {
    XmScaleCallbackStruct *scs = ( XmScaleCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    G3DMATERIAL *mat = gui->selmat;

    if ( mat ) {
        mat->shininess = ( float ) scs->value;

        redrawGLViews ( gui );
    }
}

/******************************************************************************/
static Widget createSpecularityPanel ( G3DUI *gui, Widget parent,
                                       Dimension x    , Dimension y,
                                       Dimension width, Dimension height ) {
    Widget ref, pan, lab, col;
    
    pan = XmVaCreateManagedForm ( parent, "Specularity",
                                  XmNx, x,
                                  XmNy, y,
                                  XmNwidth, width,
                                  XmNheight, height,
                                  NULL );

    lab = XmVaCreateManagedLabel ( pan, "Solid Color",
                                        XmNx, 0x00,
                                        XmNy, 0x04,
                                        XmNwidth, 0x60,
                                        XmNheight, 0x12,
                                        XmNfontList, gui->fontlist,
                                        NULL );

    col = XmCreateColorEditLabel ( pan, "Solid Color Value",
                                        0x60, 0x04,
                                        0x20, 0x12,
                                        0x00, specularitychange, gui );

    XtManageChild ( col );

    ref = XmVaCreateManagedScale ( pan, "Shininess Value",
                                        XmNx, 0x00,
                                        XmNy, 0x30,
                                        XmNwidth, 0x100,
                                        XmNheight, 0x10,
                                        XmNorientation, XmHORIZONTAL,
                                        XmNshowValue, False,
                                        XmNshadowThickness, 0x01,
                                        XmNdecimalPoints, 0x02,
                                        XmNhighlightOnEnter, False,
                                        XmNminimum, 0,
                                        XmNmaximum, 128,
                                        NULL );

    XtAddCallback ( ref, XmNdragCallback,shinecbk, gui );

    XtManageChild ( ref );


    return pan;
}

/******************************************************************************/
static void updateReflectionPanel ( Widget w, G3DMATERIAL *mat ) {
    WidgetList children;
    Cardinal nc;
    uint32_t i;

    XtVaGetValues ( w, XmNchildren, &children, XmNnumChildren, &nc, NULL );

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];

        if ( strcmp ( "Reflection Value", XtName ( child ) ) == 0x00 ) {
            uint32_t val = ( mat ) ? mat->reflection_strength * 100 : 0x00;

            XtVaSetValues ( child, XmNvalue, val, NULL );
        }
    }
}

/******************************************************************************/
static void refdragcbk  ( Widget w, XtPointer client, XtPointer call ) {
    XmScaleCallbackStruct *scs = ( XmScaleCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    G3DMATERIAL *mat = gui->selmat;

    if ( mat ) {
        mat->reflection_strength = ( float ) scs->value / 100.0f;

        redrawGLViews ( gui );
    }
}

/******************************************************************************/
static Widget createReflectionPanel ( G3DUI *gui, Widget parent,
                                      Dimension x    , Dimension y,
                                      Dimension width, Dimension height ) {
    Widget ref, pan;
    
    pan = XmVaCreateManagedForm ( parent, "Reflection",
                                  XmNx, x,
                                  XmNy, y,
                                  XmNwidth, width,
                                  XmNheight, height,
                                  NULL );

    ref = XmVaCreateManagedScale ( pan, "Reflection Value",
                                        XmNx, 0x00,
                                        XmNy, 0x00,
                                        XmNwidth, 0x100,
                                        XmNheight, 0x10,
                                        XmNorientation, XmHORIZONTAL,
                                        XmNshowValue, False,
                                        XmNshadowThickness, 0x01,
                                        XmNdecimalPoints, 0x02,
                                        XmNhighlightOnEnter, False,
                                        XmNminimum, 0,
                                        XmNmaximum, 100,
                                        NULL );

    XtAddCallback ( ref, XmNdragCallback, refdragcbk, gui );

    XtManageChild ( ref );


    return pan;
}

/******************************************************************************/
void updateMaterialSettings ( Widget w, G3DMATERIAL *mat ) {
    WidgetList children;
    Cardinal nc;
    uint32_t i;

    XtVaGetValues ( w, XmNchildren, &children, XmNnumChildren, &nc, NULL );

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];

        if ( strcmp ( "Color", XtName ( child ) ) == 0x00 ) {
            updateColorPanel ( child, mat );
        }

        if ( strcmp ( "Displacement", XtName ( child ) ) == 0x00 ) {
            updateDisplacementPanel ( child, mat );
        }

        if ( strcmp ( "Reflection", XtName ( child ) ) == 0x00 ) {
            updateReflectionPanel ( child, mat );
        }

        if ( strcmp ( "Specularity", XtName ( child ) ) == 0x00 ) {
            updateSpecularityPanel ( child, mat );
        }
    }
}

/******************************************************************************/
Widget createMaterialEdit ( Widget parent, G3DUI *gui,
                                           char *name,
                                           Dimension x,
                                           Dimension y,
                                           Dimension width,
                                           Dimension height ) {
    Widget mst;

    mst =  XmVaCreateManagedTabbedBook ( parent, name,
                                         XmNx, x,
                                         XmNy, y,
                                         XmNwidth, width,
                                         XmNheight, height,
                                         XmNfontList, gui->fontlist,
                                         NULL );

    createDisplacementPanel ( gui, mst, 0x00, 0x00, 0x100, 0x100 );
    createSpecularityPanel  ( gui, mst, 0x00, 0x00, 0x100, 0x100 );
    /*createReflectionPanel   ( gui, mst, 0x00, 0x00, 0x100, 0x100 );*/
    createColorPanel        ( gui, mst, 0x00, 0x00, 0x100, 0x100 );
 
    list_insert ( &gui->lmatsett, mst );


    return mst;
}
