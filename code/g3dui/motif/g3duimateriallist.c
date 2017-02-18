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
static void Resize ( Widget, XtPointer, XtPointer );

/******************************* Draw child border ****************************/
static void DrawTile ( Display *dis, Drawable d, GC gc, G3DUIRECTANGLE *rec,
                       Pixel toppix, Pixel botpix ) {
    /*** retrieve the original rectangle ***/
    int x1 = rec->x,
        y1 = rec->y,
        x2 = rec->x + rec->width - 1,
        y2 = rec->y + rec->height - 1;

    XSetForeground ( dis, gc, toppix );
    XDrawLine ( dis, d, gc, x1, y1, x1, y2 );
    XDrawLine ( dis, d, gc, x1, y1, x2, y1 );

    XSetForeground ( dis, gc, botpix );
    XDrawLine ( dis, d, gc, x1, y2, x2, y2 );
    XDrawLine ( dis, d, gc, x2, y1, x2, y2 );
}

/******************************************************************************/
static MOTIFMATERIALPREVIEW *materiallistdata_getPreview ( MATERIALLISTDATA *mdata,
                                                           G3DMATERIAL *mat ) {
    LIST *ltmppreview = mdata->lpreview;

    while ( ltmppreview ) {
        MOTIFMATERIALPREVIEW *preview = ( MOTIFMATERIALPREVIEW * ) ltmppreview->data;

        if ( preview->mat == mat ) return preview;

        ltmppreview = ltmppreview->next;
    }

    return NULL;
}

/******************************************************************************/
static void materiallistdata_removePreview ( MATERIALLISTDATA *mdata, 
                                             G3DMATERIAL *mat ) {
    MOTIFMATERIALPREVIEW *preview = materiallistdata_getPreview ( mdata, mat );

    list_remove ( &mdata->lpreview, preview );
}

/******************************************************************************/
static void materiallistdata_removeAllPreviews ( MATERIALLISTDATA *mdata ) {
    list_free ( &mdata->lpreview, motifmaterialpreview_free );
}

/******************************************************************************/
void g3duimateriallist_removeAllPreviews ( Widget w ) {
    MATERIALLISTDATA *mdata = NULL;
    Display *dis = XtDisplay ( w );

    XtVaGetValues ( w, XmNuserData, &mdata, NULL );

    if ( mdata ) {
        materiallistdata_removeAllPreviews ( mdata );
    }
}

/******************************************************************************/
static void materiallistdata_addPreview ( Display *dis, MATERIALLISTDATA *mdata, 
                                                        G3DMATERIAL *mat ) {
    MOTIFMATERIALPREVIEW *preview = motifmaterialpreview_new ( dis, mat, mdata->matmap );

    list_insert ( &mdata->lpreview, preview );
}

/******************************************************************************/
void g3duimateriallist_addPreview ( Widget w, G3DUI *gui, G3DMATERIAL *mat ) {
    MATERIALLISTDATA *mdata = NULL;
    Display *dis = XtDisplay ( w );

    XtVaGetValues ( w, XmNuserData, &mdata, NULL );

    if ( mdata ) {
        materiallistdata_addPreview ( dis, mdata, mat );
    }

    /*** ! Resize() should not handle the third parameter ***/
    Resize ( w, gui, NULL );
}

/******************************************************************************/
void g3duimateriallist_importFromScene ( Widget w, G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    LIST *ltmpmat = sce->lmat;

    while ( ltmpmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpmat->data;

        g3duimateriallist_addPreview ( w, gui, mat );

        ltmpmat = ltmpmat->next;
    }
}

/******************************************************************************/
static MOTIFMATERIALPREVIEW *materiallistdata_pickPreview ( MATERIALLISTDATA *mdata,
                                                            uint32_t x,
                                                            uint32_t y ) {
    LIST *ltmppreview = mdata->lpreview;

    while ( ltmppreview ) {
        MOTIFMATERIALPREVIEW *preview = ( MOTIFMATERIALPREVIEW * ) ltmppreview->data;

        if ( ( x > preview->rec.x ) && ( x < ( preview->rec.x + 
                                               preview->rec.width  ) ) &&
             ( y > preview->rec.y ) && ( y < ( preview->rec.y + 
                                               preview->rec.height ) ) ) {
            return preview;
        }

        ltmppreview = ltmppreview->next;
    }

    return NULL;
}

/******************************************************************************/
static uint32_t materiallistdata_arrangePreviews ( MATERIALLISTDATA *mdata,
                                                   uint32_t win_x,
                                                   uint32_t win_y,
                                                   uint32_t win_width,
                                                   uint32_t win_height ) {
    LIST *ltmppreview = mdata->lpreview;
    uint32_t maxheight = 0x00;

    while ( ltmppreview ) {
        MOTIFMATERIALPREVIEW *preview = ( MOTIFMATERIALPREVIEW * ) ltmppreview->data;

        preview->rec.x = win_x;
        preview->rec.y = win_y;

        preview->rec.width  = mdata->preview_width;
        preview->rec.height = mdata->preview_height;

        win_x += mdata->preview_width;

        if ( win_y + mdata->preview_height > maxheight ) {
            maxheight = win_y + mdata->preview_height;
        }

        if ( ( win_width - win_x ) < mdata->preview_width ) {
            win_y += mdata->preview_height;

            win_x  = 0x00;
        }

        ltmppreview = ltmppreview->next;
    }

    return maxheight;
}

/******************************************************************************/
void g3duimateriallist_drawPreview ( Display *dis, Window win, 
                                                   GC gc,
                                                   XmFontList ftl,
                                                   Pixel toppix,
                                                   Pixel botpix,
                                                   MATERIALLISTDATA *mdata,
                                                   MOTIFMATERIALPREVIEW *preview,
                                                   uint32_t selected ) {
    XmString str = XmStringCreate ( preview->mat->name, XmFONTLIST_DEFAULT_TAG );

    int strwidth = XmStringWidth  ( ftl, str );
    float text_x, text_y;

    DrawTile ( dis, win, gc, &preview->rec, toppix, botpix );

/*
    XSetForeground ( dis, gc, botpix );
    XDrawLine      ( dis, win, gc, xsep    , 0x00, xsep    , winheight );
    XSetForeground ( dis, gc, toppix );
    XDrawLine      ( dis, win, gc, xsep + 1, 0x00, xsep + 1, winheight );
*/
    text_x = preview->rec.x + ( preview->rec.width  - strwidth ) * 0.5f;
    text_y = preview->rec.y + ( preview->rec.height - mdata->preview_name_height );

    if ( selected ) {
        XSetForeground ( dis, gc, 0x000000FF );
        XDrawRectangle ( dis, win, gc, preview->rec.x,
                                       preview->rec.y,
                                       preview->rec.width - 1,
                                       preview->rec.height -1 );
    }

    XSetForeground ( dis, gc, 0x00000000 );


    /*** Draw the material name ***/
    XmStringDraw ( dis, win, ftl, str, gc, text_x, 
                                           text_y,
                                           preview->rec.width,
                                           XmALIGNMENT_BEGINNING, 
                                           0x00,
                                           NULL );

    /*** Draw the texture preview ***/
    XPutImage ( dis, win, gc, preview->img, 0x00, 0x00, preview->rec.x + mdata->preview_border,
                                                        preview->rec.y + mdata->preview_border,
                                                        preview->img->width,
                                                        preview->img->height );

    XmStringFree ( str );
}

/******************************************************************************/
void motifmaterialpreview_update ( Display *dis, MOTIFMATERIALPREVIEW *preview,
                                                 G3DUIMATERIALMAP *matmap ) {
    int depth = XDefaultDepth ( dis, 0x00 );
    char (*data)[0x03];

    if ( preview->img ) XDestroyImage ( preview->img );

    preview->img = NULL;

    /*** Alloc a buffer that is filled with the material preview ***/
    data = calloc ( 0x03, matmap->width * matmap->height );

    if ( data == NULL ) {
        fprintf ( stderr, "motifmaterialpreview_new: memory allocation failed\n" );

        free ( preview );
    }

    common_g3duimaterialmap_fillData ( matmap, preview->mat, data );

    /*** Convert to a GdkPixbuf image ***/
    preview->img = createXImageFromData ( dis, matmap->width, 
                                               matmap->height,
                                               depth,
                                               data );

    free ( data );
}

/******************************************************************************/
void g3duimateriallist_updatePreview ( Widget w, G3DUI *gui, G3DMATERIAL *mat ) {
    MATERIALLISTDATA *mdata = NULL;
    Display *dis = XtDisplay ( w );

    XtVaGetValues ( w, XmNuserData, &mdata, NULL );

    if ( mdata ) {
        MOTIFMATERIALPREVIEW *preview = materiallistdata_getPreview ( mdata, mat );

        if ( preview ) {

            motifmaterialpreview_update ( dis, preview, mdata->matmap );

            /*** bypass background erasing by clearing a tiny rectangle ***/
            XClearArea ( XtDisplay ( w ),
                         XtWindow  ( w ), 0, 
                                          0, 
                                          1, 
                                          1, True );
        }
    }
}

/******************************************************************************/
void g3duimateriallist_removePreview ( Widget w, G3DUI *gui, G3DMATERIAL *mat ) {
    MATERIALLISTDATA *mdata = NULL;
    Dimension width, height;

    XtVaGetValues ( w, XmNuserData, &mdata,
                       XmNwidth, &width,
                       XmNheight, &height, NULL );

    if ( mdata ) {
        materiallistdata_removePreview ( mdata, mat );

        /*** ! Resize () should not handle the third parameter ***/
        Resize ( w, gui, NULL );
    }
}

/******************************************************************************/
void g3duimateriallist_removeMaterial ( Widget w, G3DUI *gui ) {
    g3dscene_delMaterial ( gui->sce, gui->selmat );

    g3duimateriallist_removePreview ( w, gui, gui->selmat );
}

/******************************************************************************/
MOTIFMATERIALPREVIEW *motifmaterialpreview_new ( Display *dis,
                                                 G3DMATERIAL *mat, 
                                                 G3DUIMATERIALMAP *matmap ) {
    uint32_t structsize = sizeof ( MOTIFMATERIALPREVIEW );
    MOTIFMATERIALPREVIEW *preview = ( MOTIFMATERIALPREVIEW * ) calloc ( 0x01, structsize );


    if ( preview == NULL ) {
        fprintf ( stderr, "motifmaterialpreview_new: memory allocation failed\n" );

        return NULL;
    }

    preview->mat = mat;

    motifmaterialpreview_update ( dis, preview, matmap );


    return preview;
}

/******************************************************************************/
void motifmaterialpreview_free ( MOTIFMATERIALPREVIEW *preview ) {
    XDestroyImage ( preview->img );

    /*** TODO : free image data ***/


    free ( preview );
}

/******************************************************************************/
static void Input ( Widget w, XtPointer client, 
                              XEvent *event,
                              Boolean *continue_to_dispatch ) {
    G3DUI    *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    static G3DOBJECT *obj, *dst;
    static PICKEDOBJECT *pob;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    MATERIALLISTDATA *mdata = NULL;

    XtVaGetValues ( w, XmNuserData, &mdata, NULL );

    switch ( event->type ) {
        case KeyPress : {
            XKeyEvent *kev = ( XKeyEvent * ) event;
            KeySym keysym;
            char c;

            if ( XLookupString ( kev, &c, 0x01, &keysym, NULL ) > 0x00 ) {
                switch ( keysym ) {
                    case XK_Delete: {
                        if ( gui->selmat ) {
                            g3duimateriallist_removeMaterial ( w, gui );

                            gui->selmat = NULL;
                        }
                    } break;
                }
            }

            g3dui_redrawMaterialList ( gui );
            g3dui_updateMaterialEdit ( gui );
            g3dui_redrawGLViews      ( gui );
        } break;

        case ButtonPress : {
            XButtonEvent *bev = ( XButtonEvent * ) event;
            MOTIFMATERIALPREVIEW *preview;

            preview = materiallistdata_pickPreview ( mdata, bev->x, bev->y );

            if ( preview ) gui->selmat = preview->mat;

            g3dui_redrawMaterialList ( gui );
            g3dui_updateMaterialEdit ( gui );
            g3dui_redrawGLViews      ( gui );
        } break;

        default:
        break;
    }
}

/******************************************************************************/
static void Resize ( Widget w, XtPointer client, XtPointer call ) {
    MATERIALLISTDATA *mdata = NULL;
    Dimension width, height, maxheight, gotwidth, gotheight;

    XtVaGetValues ( w, XmNuserData, &mdata,
                       XmNwidth, &width,
                       XmNheight, &height, NULL );

    if ( mdata ) {
        maxheight = materiallistdata_arrangePreviews ( mdata, 0, 0, width,
                                                                    height );

        if ( maxheight ) {
            XtMakeResizeRequest ( w, width,
                                     maxheight, 
                                    &gotwidth, 
                                    &gotheight );

            if ( gotwidth && gotheight ) {
                XtResizeWidget ( w, gotwidth, gotheight, 0x00 );
            }
        }
    }
}

/******************************************************************************/
static void Redisplay ( Widget w, XtPointer client, XtPointer call ) {
    Display *dis = XtDisplay ( w );
    Window win = XtWindow ( w );
    XGCValues values;
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Dimension winwidth = 0x0, winheight = 0x00, gotwidth, gotheight;
    uint32_t size;
    Pixel background, toppix, botpix;
    GC gc = XtGetGC ( w, 0x00, &values );
    MATERIALLISTDATA *mdata = NULL;

    XtVaGetValues ( w, XmNuserData, &mdata,
                       XmNtopShadowColor, &toppix,
                       XmNbottomShadowColor, &botpix, NULL );

    if ( mdata ) {
        LIST *ltmppreview = mdata->lpreview;

        while ( ltmppreview ) {
            MOTIFMATERIALPREVIEW *preview = ( MOTIFMATERIALPREVIEW * ) ltmppreview->data;
            uint32_t selected = 0x00;

            if ( preview->mat == gui->selmat ) {
                selected = 0x01;
            }

            g3duimateriallist_drawPreview ( dis, win, 
                                                 gc, 
                                                 gmt->fontlist,
                                                 toppix,
                                                 botpix,
                                                 mdata,
                                                 preview,
                                                 selected );


            ltmppreview = ltmppreview->next;
        }
    }

    XtReleaseGC ( w, gc );
}

/******************************************************************************/
Widget createMaterialList ( Widget parent, G3DUI *gui,
                                           char *name,
                                           Dimension x,
                                           Dimension y,
                                           Dimension width, 
                                           Dimension height ) {
    MATERIALLISTDATA *matlst = common_materiallistdata_new ( 96, 96 );
    Pixel background, foreground;
    Widget drw;
    Widget scr;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    scr = XmVaCreateScrolledWindow ( parent, name,
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

    drw = XmVaCreateDrawingArea ( scr, name,
                                          XtNx, 0,
                                          XtNy, 0,
                                          XtNwidth , width,
                                          XtNheight, height,
                                          /*** needed for keyboard input ***/
                                          XmNtraversalOn,   TRUE,
                                          /*XmNancestorSensitive, TRUE,
                                          XmNsensitive, TRUE,
                                          XmNkeyboardFocusPolicy, XmEXPLICIT,*/
                                          XmNuserData, matlst,
                                          NULL );

    XtAddCallback ( drw, XmNexposeCallback, Redisplay, gui );
    XtAddCallback ( drw, XmNresizeCallback, Resize   , gui );

    XtAddEventHandler ( drw, ButtonPressMask   | 
                             ButtonReleaseMask | 
                             KeyPressMask      | 
                             KeyReleaseMask    | 
                             PointerMotionMask, False, Input, gui );

    XtManageChild ( drw );

    /*** add this widget to the list of ObjectList widget for easy update ***/
    list_insert ( &gui->lmatlist, drw );

    XtManageChild ( scr );


    return drw;
}
