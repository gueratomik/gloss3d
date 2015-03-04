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
#include <Xm/VaSimpleP.h>

/******************************************************************************/
#include <list.h>

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
static XtProc ClassInitialize ( void );
static XtInitProc Initialize  ( Widget, Widget, ArgList, Cardinal * );
static XtRealizeProc Realize  ( Widget, XtValueMask *, XSetWindowAttributes * );
static XtWidgetProc Destroy   ( Widget );
static XtWidgetProc Resize    ( Widget );
static XtExposeProc Redisplay ( Widget, XEvent *, Region );
static void XmMaterialListMouseEvent ( Widget, XEvent *, String *, Cardinal * );

/******************************************************************************/
static XtResource widgetRes[] = {
    { XtNpreviewSize, XtNpreviewSize, XtRInt, sizeof ( int ),
      XtOffsetOf ( XmMaterialListRec, materialList.imgsize ),
      XtRImmediate, ( XtPointer ) 0x48 },
    { XtNpreviewBpp, XtNpreviewBpp, XtRInt, sizeof ( int ),
      XtOffsetOf ( XmMaterialListRec, materialList.imgbpp ),
      XtRImmediate, ( XtPointer ) 0x20 },
    { XmNmaterialSelectedCallback,
      XmNmaterialSelectedCallback, XtRCallback, sizeof ( XtPointer ),
      XtOffsetOf ( XmMaterialListRec, materialList.materialSelectedCallback ),
      XtRImmediate, ( XtPointer ) NULL } };

/******************************************************************************/
static char defaultTranslations[] = 
"<PtrMoved>: XmMaterialListMouseEvent()\n\
<BtnDown>:   XmMaterialListMouseEvent()\n\
<BtnUp>:     XmMaterialListMouseEvent()\n";

/******************************************************************************/
static XtActionsRec Actions[] = { {"XmMaterialListMouseEvent",
                                    XmMaterialListMouseEvent} };

/******************************************************************************/
static void chgcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmMaterialListWidget xmw = ( XmMaterialListWidget ) client;

    XmMaterialListUpdateSelectedPreview ( ( Widget ) xmw );
}

/******************************************************************************/
static void XmMaterialListMouseEvent ( Widget w, XEvent *event,
                                       String *params, Cardinal *num_params ) {
    XmMaterialListWidget xmw = ( XmMaterialListWidget ) w;
    XMMATPREVIEW *selmpv = xmw->materialList.selmpv;
    static Time click;

    switch ( event->type ) {
        case ButtonRelease : {
            XButtonEvent *bev = ( XButtonEvent * ) event;
            /*** Click on preview tile ***/
            XMMATPREVIEW *mpv = selectPreview ( w, bev->x, bev->y );

            /*** if we clicked on a tile ***/
            if ( mpv ) {
                XmMaterialListCallbackStruct mls = { XmMATERIALSELECTED,
                                                     event,
                                                     mpv->mat };

                XtCallCallbackList ( w,
                                     xmw->materialList.materialSelectedCallback,
                                     &mls );

                /*** if we got a first click, check the difference time ***/
                if ( click ) {
                    Time dift = ( bev->time - click );

                    /*** double-click on the same tile as before ***/
                    if ( ( dift < 500 ) && ( selmpv == mpv ) ) {
                        /*Display *dis = XtDisplay ( w );
                        Window root = XDefaultRootWindow ( dis );
                        XWindowAttributes wat;
                        Widget shell, edit;

                        XGetWindowAttributes ( dis, root, &wat );

                        shell = XtVaAppCreateShell ( NULL, "Class",
                                                     topLevelShellWidgetClass,
                                                     dis, 
                                                     XtNtitle,"Material Editor",
                                                     XtNx, ( wat.width/2) -200,
                                                     XtNy, ( wat.height/2) -160,
                                                     XtNwidth, 400,
                                                     XtNheight, 320,
                                                     NULL );

                        edit = XtVaCreateWidget ( "Material Edit",
                                                  xmMaterialEditWidgetClass,
                                                  shell,
                                                  XtNx, 0x00,
                                                  XtNy, 0x00,
                                                  XtNwidth, 400,
                                                  XtNheight, 320,
                                                  XmNmaterial, selmpv->mat,
                                                  NULL );

                        XtAddCallback ( edit, XmNdestroyCallback, chgcbk, xmw );

                        XtManageChild ( edit );

                        XtRealizeWidget ( shell );*/
                    }

                    /*** no double-click ***/          
                    click = 0x00;
                /*** or else, set the initial values ***/
                } else {
                    click = bev->time;
                    xmw->materialList.selmpv = mpv;

                    XClearArea ( XtDisplay ( w ),
                                 XtWindow  ( w ), mpv->x,
                                                  mpv->y,
                                                  mpv->width,
                                                  mpv->height, True );
                }
            }
        } break;

        case MotionNotify :
            click = 0x00;
        break;

        default :
        break;
    }
}

/******************************************************************************/
static XtProc ClassInitialize ( ) {

    return ( XtProc ) NULL;
}

/******************************************************************************/
static XtInitProc Initialize ( Widget request, Widget init,
                               ArgList args, Cardinal *num ) {
    XmMaterialListWidget xmw = ( XmMaterialListWidget ) init;

    xmw->materialList.imgsize = 72;

    /*** Default preview size ***/
    xmw->materialList.vecmap = buildMap ( 72, 72 );


    return ( XtInitProc ) NULL;
}

/******************************************************************************/
static XtRealizeProc Realize ( Widget w, XtValueMask *value_mask,
                               XSetWindowAttributes *attributes ) {
    XmMaterialListWidget xmw = ( XmMaterialListWidget ) w;

    /*** this is required if we want to inherit the event mask. ***/
    /*** Redisplay procedure would not be called otherwise      ***/
    xmPrimitiveClassRec.core_class.realize ( w, value_mask, attributes );

    resizePreviews ( w );

    return ( XtRealizeProc ) NULL;
}

/******************************************************************************/
static XtWidgetProc Resize ( Widget w ) {
    /*** XmManager doesnot provide a resize function, so we have to do some ***/
    /*** routines by ourselves.                                             ***/
    if ( XtIsRealized ( w ) ) {
        XClearArea ( XtDisplay ( w ), XtWindow ( w ), 0x00, 0x00,
                                                      0x00, 0x00, True );
    }

    /*** And use the parent class of xmManager for the resizing routines ***/
    if ( xmPrimitiveClassRec.core_class.resize ) {
        xmPrimitiveClassRec.core_class.resize ( w );
    }

    resizePreviews ( w );

    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
static XtWidgetProc Destroy ( Widget w ) {
    XmMaterialListWidget xmw = ( XmMaterialListWidget ) w;

    list_free ( &xmw->materialList.lpreview, (void(*)(void*))xmmatpreview_free );

    free ( xmw->materialList.vecmap );


    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
static XtExposeProc Redisplay ( Widget w, XEvent *event, Region region ) {
    XmMaterialListWidget xmw = ( XmMaterialListWidget ) w;
    LIST *ltmp = xmw->materialList.lpreview; 
    Display *dis = XtDisplay ( w );
    Window win = XtWindow ( w );
    Pixel toppix, botpix, whitepix = XWhitePixel ( dis, 0x00 ),
                          blackpix = XBlackPixel ( dis, 0x00 );
    XGCValues values;
    G3DUI *gui;
    GC gc;

    gc = XtGetGC ( w, 0x00, &values );

    XtVaGetValues ( w, XmNuserData, &gui,
                       XmNtopShadowColor, &toppix,
                       XmNbottomShadowColor, &botpix,
                       NULL );

    while ( ltmp ) {
        XMMATPREVIEW *mpv = ( XMMATPREVIEW * ) ltmp->data;

        if ( mpv == xmw->materialList.selmpv ) {
            drawPreview ( mpv, whitepix,
                               blackpix, blackpix, gui->fontlist, dis, win, gc );
        } else {
            drawPreview ( mpv, blackpix,
                               toppix, botpix, gui->fontlist, dis, win, gc );
        }

        ltmp = ltmp->next;
    }

    XtReleaseGC ( w, gc );

    return ( XtExposeProc ) NULL;
}

/******************************************************************************/
XmMaterialListClassRec xmMaterialListClassRec = 
                              { { (WidgetClass) &xmPrimitiveClassRec, /*parent*/
                                  XMMATERIALLISTCLASSNAME,      /* class_name */
                                  sizeof ( XmMaterialListRec ),/* widget size */
                       ( XtProc ) ClassInitialize,        /* class_initialize */
                                  NULL,              /* class_part_initialize */
                                  FALSE,                      /* class_inited */
                   ( XtInitProc ) Initialize,                   /* initialize */
                                  NULL,                    /* initialize_hook */
                ( XtRealizeProc ) Realize,                         /* realize */
                                  Actions,                         /* actions */
                                  XtNumber ( Actions )/*0x00*/,/* num_actions */
                                  widgetRes,                      /* resouces */
                                  XtNumber ( widgetRes ),    /* num_resources */
                                  NULLQUARK,                      /*xrm_class */
                                  TRUE,                    /* compress_motion */
                                  XtExposeCompressMaximal,/*compress_exposure */
                                  TRUE,                /* compress_enterleave */
                                  FALSE,                  /* visible_interest */
                ( XtWidgetProc )  Destroy,                          /*destroy */
                ( XtWidgetProc )  Resize,                           /* resize */
                ( XtExposeProc )  Redisplay,                        /* expose */
             ( XtSetValuesFunc )  NULL,                         /* set_values */
                                  NULL,                    /* set_values_hook */
                                  XtInheritSetValuesAlmost,/*set_values_almost*/
                                  NULL,                    /* get_values_hook */
                                  NULL,                       /* accept_focus */
                                  XtVersion,                       /* version */
                                  NULL,                   /* callback_offsets */
                                  defaultTranslations,            /* tm_table */
                                  XtInheritQueryGeometry,   /* query_geometry */
                                  NULL,                /* display_accelerator */
                                  NULL },                        /* extension */
                                 { /* Primitive Class part */
      /* border_highlight      */ XmInheritBorderHighlight,
      /* border_unhighlight    */ XmInheritBorderUnhighlight,
      /* translations          */ XtInheritTranslations,
      /* arm_and_activate_proc */ NULL,
      /* Synthetic Resources   */ NULL,
      /* num syn res           */ 0x00,
      /* extension             */ (XtPointer) NULL },
                                 { /* Material class part */
                                   0x00 } };

/******************************************************************************/
WidgetClass xmMaterialListWidgetClass = ( WidgetClass ) &xmMaterialListClassRec;


/******************************************************************************/
Widget XmCreateMaterialList ( Widget parent, char *name, ArgList  args,
                                                         Cardinal narg ) {
    Widget mli = XtCreateWidget ( name, xmMaterialListWidgetClass,
                                  parent, args, narg );

    return mli;
}

/******************************************************************************/
Widget XmVaCreateMaterialList ( Widget parent, char *name, ... ) {
    register Widget w;
    va_list var;
    int count;

    va_start ( var,name );
    count = XmeCountVaListSimple ( var );
    va_end ( var );

    va_start ( var, name );
    w = XmeVLCreateWidget ( name, 
                            xmMaterialListWidgetClass, 
                            parent, False, 
                            var, count );
    va_end( var );

    return w;
}

/******************************************************************************/
Widget XmVaCreateManagedMaterialList ( Widget parent, char *name, ... ) {
    register Widget w;
    va_list var;
    int count;

    va_start ( var,name );
    count = XmeCountVaListSimple ( var );
    va_end ( var );

    va_start ( var, name );
    w = XmeVLCreateWidget ( name, 
                            xmMaterialListWidgetClass, 
                            parent, False, 
                            var, count );
    va_end( var );

    XtManageChild ( w );


    return w;
}

/******************************************************************************/
XMMATPREVIEW *seekPreview ( Widget w, G3DMATERIAL *mat ) {
    XmMaterialListWidget gmw = ( XmMaterialListWidget ) w;
    LIST *ltmp = gmw->materialList.lpreview;

    while ( ltmp ) {
        XMMATPREVIEW *mpv = ( XMMATPREVIEW * ) ltmp->data;

        if ( mpv->mat == mat ) {

            return mpv;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
void importPreviews ( Widget w ) {
    XmMaterialListWidget gmw = ( XmMaterialListWidget ) w;
    G3DUI *gui;
    LIST *ltmp;

    XtVaGetValues ( w, XmNuserData, &gui, NULL  );

    ltmp = gui->sce->lmat;

    while ( ltmp ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmp->data;

        addPreview ( w, mat );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void freePreviews ( Widget w ) {
    XmMaterialListWidget gmw = ( XmMaterialListWidget ) w;

    list_free ( &gmw->materialList.lpreview, (void(*)(void*))xmmatpreview_free );
}

/******************************************************************************/
void addPreview ( Widget w, G3DMATERIAL *mat ) {
    XmMaterialListWidget gmw = ( XmMaterialListWidget ) w;
    XMMATPREVIEW *mpv = xmmatpreview_new ( mat->name, mat );
    Display *dis = XtDisplay ( w );
    Window win = XtWindow ( w );
    XImage *img;
    int depth;

    XtVaGetValues ( w, XmNdepth, &depth, NULL );

    /*** XCreateImage crashes if drawable is "win" variable, why ? ***/
    img = XCreateImage ( dis, DefaultVisual ( dis, 0x00 ),
                         depth, ZPixmap, 0x00, NULL,
                         gmw->materialList.imgsize, /*** Square Image ...   ***/
                         gmw->materialList.imgsize, /*** ... Width = Height ***/
                         gmw->materialList.imgbpp , 0x00 );

    img->data = ( char * ) calloc ( img->height * img->bytes_per_line, 0x01 );

    if ( img->data == NULL ) {
        fprintf ( stderr, "addPreview: memory allocation failed\n" );

        return;
    }

    mpv->img = img;

    /*** Draw the first preview image ***/
    fillPreview ( mpv->img, mat, gmw->materialList.vecmap );

    list_insert ( &gmw->materialList.lpreview, mpv );
}

/******************************************************************************/
void updatePreview ( Widget w, G3DMATERIAL *mat ) {
    XmMaterialListWidget gmw = ( XmMaterialListWidget ) w;

    XMMATPREVIEW *mpv = seekPreview ( w, mat );

    if ( mpv ) {
        fillPreview ( mpv->img, mat, gmw->materialList.vecmap );
    } else {
        fprintf ( stderr, "updatePreview: no preview found\n" );
    }
}

/******************************************************************************/
void delPreview ( Widget w, G3DMATERIAL *mat ) {
    XmMaterialListWidget gmw = ( XmMaterialListWidget ) w;
    XMMATPREVIEW *mpv = seekPreview ( w, mat );

    if ( mpv ) {
        list_remove ( &gmw->materialList.lpreview, mpv );

        xmmatpreview_free ( mpv );

        /*** update tiles ***/
        resizePreviews ( w );
    }
}

/******************************************************************************/
XMMATPREVIEW *selectPreview ( Widget w, int x, int y ) {
    XmMaterialListWidget xmw = ( XmMaterialListWidget ) w;
    LIST *ltmp = xmw->materialList.lpreview;

    while ( ltmp ) {
        XMMATPREVIEW *mpv = ( XMMATPREVIEW * ) ltmp->data;

        if ( ( x > mpv->x ) && ( x < mpv->x + mpv->width ) &&
             ( y > mpv->y ) && ( y < mpv->y + mpv->height) ) {

            return mpv;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
void fillPreview ( XImage *img, G3DMATERIAL *mat, XMMATPIXEL *vecmap ) {
    uint32_t x, y;
    G3DRGBA rgba;

    g3dcolor_toRGBA ( &mat->diffuse.solid, &rgba );

    for ( y = 0x00; y < img->height; y++ ) {
        for ( x = 0x00; x < img->width; x++ ) {
            XMMATPIXEL *p = vecmap++;

            if ( p->used ) {
                uint32_t R, G, B;
                float rf, gf, bf;
                uint32_t col = 0x00;

                if ( mat->flags & DIFFUSE_USEIMAGECOLOR ) {
                    if ( mat->diffuse.image ) {
                        G3DIMAGE *colimg = mat->diffuse.image;
                        uint32_t imgx = ((uint32_t)((float)p->u * colimg->width  * colimg->wratio ));
                        uint32_t imgy = ((uint32_t)((float)p->v * colimg->height * colimg->hratio ));
                        uint32_t offset = ( imgy * colimg->bytesperline  ) +
                                          ( imgx * colimg->bytesperpixel );

                        /*** This depth part should be optimized ***/
                        if ( colimg->depth == 0x18 ) {
                            R = colimg->data[offset+0x00];
                            G = colimg->data[offset+0x01];
                            B = colimg->data[offset+0x02];
                        }

                        if ( colimg->depth == 0x08 ) {
                            R = G = B = colimg->data[offset+0x00];
                        }
                    }
                }

                if ( mat->flags & DIFFUSE_USESOLIDCOLOR ) {
                    R = ( uint32_t ) rgba.r,
                    G = ( uint32_t ) rgba.g,
                    B = ( uint32_t ) rgba.b;
                }

                R = ( R * p->dot );
                G = ( G * p->dot );
                B = ( B * p->dot );

                if ( R > 0xFF ) R = 0xFF;
                if ( G > 0xFF ) G = 0xFF;
                if ( B > 0xFF ) B = 0xFF;

                col = (( uint32_t )( R << 0x10 ) | ( G << 0x08 ) | B );

                img->f.put_pixel ( img, x, y, col );
            } else {
                img->f.put_pixel ( img, x, y, 0x00404040 );
            }
        }
    }
}

/******************************************************************************/
void buildSphere ( XMMATPIXEL *pix, int width, int height, float rad ) {
    uint32_t winx, winy;
    /*** 2 lights vector ***/
    float lvx[0x02] = { -0.500000000000,  0.500000000000 },
          lvy[0x02] = { -0.500000000000,  0.500000000000 },
          lvz[0x02] = {  0.70710678118f, -0.70710678118f };

    if ( !width || !height ) return;

    for ( winy = 0x00; winy < height; winy++ ) {
        float objy = ( float ) 2 * ( winy ) / height - 1.0f;

        for ( winx = 0x00; winx < width; winx++ ) {
            float objx = ( float ) 2 * ( winx ) / width - 1.0f;
            float equz =  ( rad * rad ) - ( ( objx * objx ) + ( objy * objy ) );
            XMMATPIXEL *p = pix++;

            if ( equz >= 0.0f ) {
                float objz = sqrt ( equz );
                float u, v;
                uint32_t i;

                /** the normal vector of a point on a sphere is its position **/
                p->xdir = objx;
                p->ydir = objy;
                p->zdir = objz;

        /*** http://en.wikipedia.org/wiki/UV_mapping#Finding_UV_on_a_sphere ***/
                p->u =   ( atan2f( p->zdir, p->xdir ) / ( 2.0f * M_PI ) ) + 0.5f;
                p->v = - ( asin  ( p->ydir )          / ( M_PI        ) ) + 0.5f;

                /*** normalize the vector if needed ***/
                if ( rad < VECNORMMIN || rad > VECNORMMAX ) {
                    p->xdir /= rad;
                    p->ydir /= rad;
                    p->zdir /= rad;
                }

                p->dot = 0.0f;
                for ( i = 0x00; i < /*0x02*/0x02; i++ ) {
                    float dot = ( p->xdir * lvx[i] ) + ( p->ydir * lvy[i] ) +
                                ( p->zdir * lvz[i] );

                    if ( dot < 0.0f ) dot = 0.0f;

                    p->dot += dot;
                }

                p->used = 0x01;
            } else {
                p->used = 0x00;
            }
        }
    }
}

/******************************************************************************/
XMMATPIXEL *buildMap ( uint32_t width, uint32_t height ) {
    uint32_t structsize = sizeof ( XMMATPIXEL );
    uint32_t nbvec = ( width * height );
    XMMATPIXEL *vecmap = ( XMMATPIXEL * ) calloc ( nbvec, structsize );

    if ( vecmap == NULL ) {
        fprintf ( stderr, "buildMap: memory allocation failed\n" );

        return;
    }

    /*** build the material sphere vector map ***/
    buildSphere ( vecmap, width, height, 0.8f );


    return vecmap;
}

/******************************************************************************/
void drawPreview ( XMMATPREVIEW *mpv, Pixel color,
                                      Pixel toppix,
                                      Pixel botpix,
                                      XmFontList fontlist,
                                      Display *dis, Window win, GC gc ) {
    int x1 = mpv->x, y1 = mpv->y,
        x2 = mpv->x + mpv->width,
        y2 = mpv->y + mpv->height;
    XRectangle rec = { mpv->x,
                       mpv->y,
                       mpv->width,
                       mpv->height };

    XPutImage ( dis, win, gc, mpv->img, 0x00, 0x00, mpv->x,
                                                    mpv->y, 
                                                    mpv->img->width,
                                                    mpv->img->height );

    XSetForeground ( dis, gc, color );
    XmStringDraw ( dis, win, fontlist, mpv->name, gc,
                   mpv->x, mpv->y + mpv->img->height,
                   mpv->width, XmALIGNMENT_CENTER,
                   0x00, &rec );

    XSetForeground ( dis, gc, toppix );
    XDrawLine ( dis, win, gc, x1, y1, x2, y1 );
    XDrawLine ( dis, win, gc, x1, y1, x1, y2 );

    XSetForeground ( dis, gc, botpix );
    XDrawLine ( dis, win, gc, x1, y2, x2, y2 );
    XDrawLine ( dis, win, gc, x2, y1, x2, y2 );
}

/******************************************************************************/
XMMATPREVIEW *xmmatpreview_new ( char *name, G3DMATERIAL *mat ) {
    int structsize = sizeof ( XMMATPREVIEW );
    XMMATPREVIEW *mpv = ( XMMATPREVIEW * ) calloc ( 0x01, structsize );

    if ( mpv == NULL ) {
        fprintf ( stderr, "xmmatpreview_new: memory allocation failed\n" );

        return NULL;
    }

    mpv->name = XmStringCreate ( name, XmFONTLIST_DEFAULT_TAG );
    mpv->mat  = mat;


    return mpv;
}

/******************************************************************************/
void xmmatpreview_free ( XMMATPREVIEW *mpv ) {
    if ( mpv->name ) XmStringFree  ( mpv->name );
    if ( mpv->img  ) XDestroyImage ( mpv->img  );

    free ( mpv );
}

/******************************************************************************/
XImage *allocPreviewImage ( int depth, uint32_t width,
                                       uint32_t height,
                                       uint32_t bpp,
                                       Display *dis,
                                       Visual *vis ) {
    XImage *img;

    if ( ( img = XCreateImage ( dis, vis, depth, ZPixmap, 0x00, NULL,
                                    width, height, bpp, 0x00 ) ) == NULL ) {
        fprintf ( stderr, "sizePreview: memory allocation failed\n" );

        return NULL;
    }

    /*** Alloc and reset the Image color data ***/
    if ( ( img->data = ( char * ) calloc ( img->height * img->bytes_per_line,
                                           0x01 ) ) == NULL ) {
        fprintf ( stderr, "sizePreview: data memory allocation failed\n" );

        XDestroyImage ( img );

        return NULL;
    }

    return img;
}

/******************************************************************************/
void sizePreviewImage ( XMMATPREVIEW *mpv, int depth,
                                           uint32_t width,
                                           uint32_t height,
                                           uint32_t bpp,
                                           XMMATPIXEL *vecmap,
                                           Display *dis,
                                           Visual *vis ) {
    /*** Destroy the previous thumbnail image first, ***/
    /*** before recreating it with a different size. ***/
    if ( mpv->img ) {
        XDestroyImage ( mpv->img );
        mpv->img = NULL;
    }

    mpv->img = allocPreviewImage ( depth, width, height, bpp, dis, vis );

    /*** Fill preview image ***/
    fillPreview ( mpv->img, mpv->mat, vecmap );
}

/******************************************************************************/
void resizePreviews ( Widget w ) {
    XmMaterialListWidget xmw = ( XmMaterialListWidget ) w;
    Widget parent = XtParent ( w );
    LIST *ltmp = ( LIST * ) xmw->materialList.lpreview;
    int accux = 0x00, accuy = 0x00;
    Dimension width, height;
    Dimension retw, reth;
    int per_line;
    int i = 0x00;

    XtVaGetValues ( w, XmNwidth, &width, XmNheight, &height, NULL );

    per_line = ( width  / xmw->materialList.imgsize );

    while ( ltmp ) {
        XMMATPREVIEW *mpv = ( XMMATPREVIEW * ) ltmp->data;

        mpv->x      = accux;
        mpv->y      = accuy;
        mpv->width  = xmw->materialList.imgsize;
        mpv->height = xmw->materialList.imgsize + 0x10;

        if ( (++i) == per_line ) {
            accux = 0x00;
            accuy = ( accuy + xmw->materialList.imgsize ) + 0x01 + 0x10;

            i = 0x00;
        } else {
            accux = ( accux + xmw->materialList.imgsize ) + 0x01;
        }

        height = mpv->y + mpv->height + 0x01;

        ltmp = ltmp->next;
    }

    XtMakeResizeRequest ( w, width, height, &retw, &reth );

    XtResizeWidget ( w, retw, reth, 0x00 );
}

/******************************************************************************/
void smallpreviewscbk ( Widget w, XtPointer client, XtPointer call ) {
    XmMaterialListWidget gmw = ( XmMaterialListWidget ) client;

    gmw->materialList.imgsize = 36;

    /*** free vector map ... ***/
    if ( gmw->materialList.vecmap ) {
        free ( gmw->materialList.vecmap );

        gmw->materialList.vecmap = NULL;
    }

    /*** ... before rebuilding it with the correct size ***/
    gmw->materialList.vecmap = buildMap ( 36, 36 );

    /*** rebuild all previews ***/
    freePreviews   ( ( Widget ) gmw );
    importPreviews ( ( Widget ) gmw );
    resizePreviews ( ( Widget ) gmw );
}

/******************************************************************************/
void normalpreviewscbk ( Widget w, XtPointer client, XtPointer call ) {
    XmMaterialListWidget gmw = ( XmMaterialListWidget ) client;

    gmw->materialList.imgsize = 72;

    /*** free vector map ... ***/
    if ( gmw->materialList.vecmap ) {
        free ( gmw->materialList.vecmap );

        gmw->materialList.vecmap = NULL;
    }

    /*** ... before rebuilding it with the correct size ***/
    gmw->materialList.vecmap = buildMap ( 72, 72 );

    /*** rebuild all previews ***/
    freePreviews   ( ( Widget ) gmw );
    importPreviews ( ( Widget ) gmw );
    resizePreviews ( ( Widget ) gmw );
}

/******************************************************************************/
void bigpreviewscbk ( Widget w, XtPointer client, XtPointer call ) {
    XmMaterialListWidget gmw = ( XmMaterialListWidget ) client;

    gmw->materialList.imgsize = 144;

    /*** free vector map ... ***/
    if ( gmw->materialList.vecmap ) {
        free ( gmw->materialList.vecmap );

        gmw->materialList.vecmap = NULL;
    }

    /*** ... before rebuilding it with the correct size ***/
    gmw->materialList.vecmap = buildMap ( 144, 144 );

    /*** rebuild all previews ***/
    freePreviews   ( ( Widget ) gmw );
    importPreviews ( ( Widget ) gmw );
    resizePreviews ( ( Widget ) gmw );
}

/******************************************************************************/
void XmMaterialListAddMaterial ( Widget w, char *name, G3DMATERIAL *mat ) {
    XmMaterialListWidget xmw = ( XmMaterialListWidget ) w;
    XMMATPREVIEW *mpv = xmmatpreview_new ( name, mat );
    Display *dis = XtDisplay ( w );
    Window win = XtWindow ( w );
    /*** XCreateImage crashes if drawable is "win" variable, why ? ***/
    Visual *vis = DefaultVisual ( dis, 0x00 );
    int depth;

    XtVaGetValues ( w, XmNdepth, &depth, NULL );

    sizePreviewImage ( mpv, depth, xmw->materialList.imgsize,
                                   xmw->materialList.imgsize,
                                   xmw->materialList.imgbpp,
                                   xmw->materialList.vecmap,
                                   dis, vis );

    list_insert ( &xmw->materialList.lpreview, mpv );

    resizePreviews ( w );
}

/******************************************************************************/
void XmMaterialListUpdateSelectedPreview( Widget w ) {
    XmMaterialListWidget xmw = ( XmMaterialListWidget ) w;
    XMMATPIXEL *vecmap = xmw->materialList.vecmap;
    XMMATPREVIEW *mpv = xmw->materialList.selmpv;


    if ( mpv ) {
        /*** Fill preview image ***/
        fillPreview ( mpv->img, mpv->mat, vecmap );

        XClearArea ( XtDisplay ( w ),
                     XtWindow ( w ), 0x00, 0x00, 0x01, 0x01, True );

        return;
    }
}

/******************************************************************************/
void XmMaterialListUpdatePreviewFromMaterial ( Widget w, G3DMATERIAL *mat ) {
    XmMaterialListWidget xmw = ( XmMaterialListWidget ) w;
    XMMATPIXEL *vecmap = xmw->materialList.vecmap;
    LIST *ltmp = xmw->materialList.lpreview;

    while ( ltmp ) {
        XMMATPREVIEW *mpv = ( XMMATPREVIEW * ) ltmp->data;

        if ( mpv->mat == mat ) {
            /*** Fill preview image ***/
            fillPreview ( mpv->img, mpv->mat, vecmap );

            XClearArea ( XtDisplay ( w ),
                         XtWindow ( w ), 0x00, 0x00, 0x01, 0x01, True );

            return;
        }

        ltmp = ltmp->next;
    }
}
