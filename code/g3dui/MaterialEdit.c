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
#include <ColorEdit.h>
#include <TabbedBook.h>

/******************************************************************************/
static XtProc ClassInitialize ( );
static XtInitProc Initialize  ( Widget, Widget, ArgList, Cardinal * );
static XtRealizeProc Realize  ( Widget, XtValueMask *, XSetWindowAttributes * );
static XtWidgetProc Destroy   ( Widget );
static XtWidgetProc Resize    ( Widget );
static XtExposeProc Redisplay ( Widget , XEvent *, Region );
static XtSetValuesFunc set_values ( Widget,
                                    Widget, Widget, ArgList, Cardinal * );

/******************************************************************************/
static XtResource widgetRes[] = {
    { XtNmaterial, XtNmaterial, XtRPointer, sizeof ( XtPointer ),
      XtOffsetOf ( XmMaterialEditRec, materialedit.mat ),
      XtRImmediate, ( XtPointer ) NULL },
    { XmNmaterialChangedCallback,
      XmNmaterialChangedCallback, XtRCallback, sizeof ( XtPointer ),
      XtOffsetOf ( XmMaterialEditRec, materialedit.materialChangedCallback ),
      XtRImmediate, ( XtPointer ) NULL } };

/******************************************************************************/
static XtInitProc Initialize ( Widget request, Widget init,
                               ArgList args, Cardinal *num ) {

    return ( XtInitProc ) NULL;
}

/******************************************************************************/
static XtProc ClassInitialize ( ) {
    uint32_t structsize = sizeof ( XMMATPIXEL );
    uint32_t nbvec = ( MATERIALEDITIMGSIZE * MATERIALEDITIMGSIZE );
    XMMATPIXEL *vecmap = ( XMMATPIXEL * ) calloc ( nbvec, structsize );

    if ( vecmap == NULL ) {
        fprintf ( stderr, "MaterialEditClass: memory allocation failed\n" );

        return;
    }

    /*** build the material sphere vector map ***/
    buildSphere ( vecmap, MATERIALEDITIMGSIZE,
                          MATERIALEDITIMGSIZE, 0.8f );

    xmMaterialEditClassRec.materialedit.vecmap = vecmap;


    return ( XtProc ) NULL;
}

/******************************************************************************/
static XtWidgetProc Destroy ( Widget w ) {
    XmMaterialEditWidget xmw = ( XmMaterialEditWidget ) w;

    if ( xmw->materialedit.img ) XDestroyImage ( xmw->materialedit.img );

    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
static XtWidgetProc Resize ( Widget w ) {

    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
static void colorchange ( uint32_t pixel, void *data ) {
    XmMaterialEditWidget xmw = ( XmMaterialEditWidget ) data;
    G3DMATERIAL *mat = xmw->materialedit.mat;
    XImage *img = xmw->materialedit.img;


    if ( mat ) {
        G3DRGBA rgba;

        rgba.r = ( pixel & 0x00FF0000 ) >> 0x10;
        rgba.g = ( pixel & 0x0000FF00 ) >> 0x08;
        rgba.b = ( pixel & 0x000000FF );
        rgba.a = 0x00;

        g3drgba_toColor ( &rgba, &mat->diffuse.solid );

        fillPreview ( img, mat, xmMaterialEditClassRec.materialedit.vecmap );

        XClearArea ( XtDisplay ( xmw ), XtWindow ( xmw ), 0x00, 0x00,
        /*** Some trick to generate exposure event without redrawing    ***/
        /*** the background, preventing flickering. Needs a better way. ***/
                                                          0x01, 0x01, True );
    }
}

/******************************************************************************/
static Widget createColorPane ( Widget parent ) {
    XmMaterialEditWidget xmw = ( XmMaterialEditWidget ) parent;
    G3DMATERIAL *mat = xmw->materialedit.mat;
    Widget cpan = XmVaCreateManagedForm ( parent, COLORPANENAME,
                                          XmNx, MATERIALEDITIMGSIZE,
                                          XmNy, 0x00,
                                          XmNwidth, MATERIALEDITPANEWIDTH,
                                          XmNheight, MATERIALEDITPANEHEIGHT,
                                          NULL );
    Widget col;
    unsigned long pixel = 0x00;
    G3DRGBA rgba;

    if ( mat ) {
        g3dcolor_toRGBA ( &mat->diffuse.solid, &rgba );
    }

    pixel = ( rgba.r << 0x10 ) | ( rgba.g << 0x08 ) | rgba.b; 

    XmVaCreateManagedToggleButton ( cpan, "Solid Color",
                                    XmNx    , 0x00, XmNy     , 0x00,
                                    XmNwidth, 0x60, XmNheight, 0x20,
                                    XmNhighlightThickness, 0x00,
                                    NULL );

    col = XmCreateColorEditLabel ( cpan, "Color", 0x60, 0x04,
                                                  0x20, 0x12,
                                                  pixel, colorchange, parent );

    XtManageChild ( col );


    return cpan;
}



/******************************************************************************/
static Widget createReflectionPane ( Widget parent ) {
    XmMaterialEditWidget xmw = ( XmMaterialEditWidget ) parent;
    G3DMATERIAL *mat = xmw->materialedit.mat;
    Widget rpan = XmVaCreateManagedForm ( parent, REFLECTIONPANENAME,
                                          XmNx, MATERIALEDITIMGSIZE,
                                          XmNy, 0x00,
                                          XmNwidth, MATERIALEDITPANEWIDTH,
                                          XmNheight, MATERIALEDITPANEHEIGHT,
                                          NULL );
    Widget sca;

    XmVaCreateManagedToggleButton ( rpan, "Reflection",
                                    XmNx    , 0x00, XmNy     , 0x00,
                                    XmNwidth, 0x60, XmNheight, 0x20,
                                    XmNhighlightThickness, 0x00,
                                    NULL );

    /*col = XmVaCreateManagedScale ( w, "SCALE",
                                   XmNx, 0x00,
                                   XmNy, 0x100,
                                   XmNwidth, 0x100,
                                   XmNheight, 0x10,
                                   XmNmaximum, 0xFF,
                                   XmNminimum, 0x00,
                                   XmNhighlightThickness, 0x00,
                                   XmNshadowThickness, 0x01,
                                   XmNorientation, XmHORIZONTAL,
                                   XmNforeground, foreground,
                                   XmNbackground, background,
                                   NULL );*/


    /*XtManageChild ( sca );*/


    return rpan;
}

/******************************************************************************/
static void showpanecbk  ( Widget w, XtPointer client, XtPointer call ) {
    Widget parent = XtParent ( w );
    WidgetList children;
    Cardinal nc;

    XtVaGetValues ( parent, XmNchildren, &children,
                            XmNnumChildren, &nc, NULL );

    /*for ( i = 0x00; i < nc; i++ ) {
        char *kidname = XtName ( children[i] ),
             *widname = 

        if ( strcmp ( name,  )
    }*/
}

/******************************************************************************/
static void addLeftPaneToggleButton ( Widget parent, char *name,
                                      G3DMATERIAL *mat,
                                      void (*armcbk)( Widget, XtPointer,
                                                           XtPointer ) ) {
    Cardinal nc;
    Widget btn;

    XtVaGetValues ( parent, XmNnumChildren, &nc, NULL );

    btn = XmVaCreateManagedToggleButton ( parent, name,
                                          XmNx, 0x00,
                                          XmNy, 0x20 * nc,
                                          XmNwidth , MATERIALEDITIMGSIZE,
                                          XmNheight, 0x20,
                                          XmNindicatorOn, False,
                                          XmNrecomputeSize, False,
                                          XmNhighlightThickness, 0x00,
                                          XmNshadowThickness, 0x01,
                                          XmNmarginWidth, 0x00,
                                          NULL );

    XtAddCallback ( btn, XmNarmCallback, armcbk, mat );
}

/******************************************************************************/
static Widget createLeftPane ( Widget parent ) {
    Pixel background, foreground;
    G3DMATERIAL *mat;
    Widget cpan;
    Widget leftp;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground,
                            /*XmNmaterial, &mat,*/
                            NULL );

    leftp = XmVaCreateManagedRowColumn ( parent, "LEFTPANE",
                                         XmNx, 0x00,
                                         XmNy, MATERIALEDITIMGSIZE,
                                         XmNwidth, MATERIALEDITIMGSIZE,
                                         XmNheight, 0x100,
                                         XmNspacing, 0x00,
                                         XmNmarginWidth, 0x00,
                                         XmNmarginHeight, 0x00,
                                         XmNradioAlwaysOne, True,
                                         XmNradioBehavior, True,
                                         XmNbackground, background,
                                         XmNforeground, foreground, NULL );

    createColorPane      ( parent );
    createReflectionPane ( parent );

    /*addLeftPaneToggleButton ( leftp, "Color"       , mat, colorPanecbk      );
    addLeftPaneToggleButton ( leftp, "Reflection"  , mat, reflectionPanecbk );*/
    /*addLeftPaneToggleButton ( leftp, "Transparency", mat, colorPanecbk );
    addLeftPaneToggleButton ( leftp, "Bump"        , mat, colorPanecbk );
    addLeftPaneToggleButton ( leftp, "Displacement", mat, colorPanecbk );*/


    return leftp;
}

/******************************************************************************/
static XtRealizeProc Realize ( Widget w, XtValueMask *value_mask,
                               XSetWindowAttributes *attributes ) {
    XmMaterialEditWidget xmw = ( XmMaterialEditWidget ) w;
    Display *dis = XtDisplay ( w );
    Visual *vis = DefaultVisual ( dis, 0x00 );
    Dimension width, height;
    int depth;

    /*** this is required if we want to inherit the event mask. ***/
    /*** Redisplay procedure would not be called otherwise      ***/
    xmManagerClassRec.core_class.realize ( w, value_mask, attributes );

    XtVaGetValues ( w, XmNdepth , &depth,
                       XmNwidth , &width,
                       XmNheight, &height, NULL );

    xmw->materialedit.img = allocPreviewImage ( depth,
                   /*** Square Image ...   ***/ MATERIALEDITIMGSIZE, 
                   /*** ... Width = Height ***/ MATERIALEDITIMGSIZE, 
                                                MATERIALEDITIMGDEPTH,
                                                dis, vis );

    if ( xmw->materialedit.mat ) {
        fillPreview ( xmw->materialedit.img,
                      xmw->materialedit.mat,
                      xmMaterialEditClassRec.materialedit.vecmap );
    }

    createLeftPane ( w );

    /*XmVaCreateManagedPushButton ( w, "Apply",
                                  XmNx, 0x100,
                                  XmNy, 0x100,
                                  XmNwidth, 0x20,
                                  XmNheight, 0x20,
                                  NULL );*/

    return ( XtRealizeProc ) NULL;
}

/******************************************************************************/
static XtExposeProc Redisplay ( Widget w, XEvent *event, Region region ) {
    XmMaterialEditWidget xmw = ( XmMaterialEditWidget ) w;
    XImage *img = xmw->materialedit.img;
    Display *dis = XtDisplay ( w );
    Window win = XtWindow ( w );
    XGCValues values;
    GC gc = XtGetGC ( w, 0x00, &values );

    XPutImage ( dis, win, gc, img, 0x00, 0x00,
                                   0x00, 0x00,
                                   MATERIALEDITIMGSIZE,
                                   MATERIALEDITIMGSIZE );

    XtReleaseGC ( w, gc );


    return ( XtExposeProc ) NULL;
}


/******************************************************************************/
static XtSetValuesFunc set_values ( Widget current, Widget request, Widget set,
                                    ArgList args, Cardinal *numargs ) {

    return 0x00;
}

/******************************************************************************/
XmMaterialEditClassRec xmMaterialEditClassRec = 
                              { { (WidgetClass) &xmManagerClassRec,/*parent*/
                                  "MATERIALEDIT",               /* class_name */
                                  sizeof ( XmMaterialEditRec ),/* widget size */
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
                ( XtExposeProc )  Redisplay,                        /* expose */
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

/******************************************************************************/
WidgetClass xmMaterialEditWidgetClass = ( WidgetClass ) &xmMaterialEditClassRec;

/******************************************************************************/
Widget XmCreateMaterialEdit ( Widget parent, char *name, ArgList args,
                                                         Cardinal narg ) {
    Widget med = XtCreateWidget ( name, xmMaterialEditWidgetClass,
                                  parent, args, narg );

    return med;
}

/******************************************************************************/
Widget XmCreateManagedMaterialEdit ( Widget parent, char *name, ArgList args,
                                                                Cardinal narg ){
    Widget med = XtCreateManagedWidget ( name, xmMaterialEditWidgetClass,
                                         parent, args, narg );

    return med;
}

/******************************************************************************/
Widget XmVaCreateMaterialEdit ( Widget parent, char *name, ... ) {
    register Widget w;
    va_list var;
    int count;

    va_start ( var,name );
    count = XmeCountVaListSimple ( var );
    va_end ( var );

    va_start ( var, name );
    w = XmeVLCreateWidget ( name, 
                            xmMaterialEditWidgetClass, 
                            parent, False, 
                            var, count );
    va_end( var );

    return w;
}

/******************************************************************************/
Widget XmVaCreateManagedMaterialEdit ( Widget parent, char *name, ... ) {
    register Widget w;
    va_list var;
    int count;

    va_start ( var,name );
    count = XmeCountVaListSimple ( var );
    va_end ( var );

    va_start ( var, name );
    w = XmeVLCreateWidget ( name, 
                            xmMaterialEditWidgetClass, 
                            parent, False, 
                            var, count );
    va_end( var );

    XtManageChild ( w );


    return w;
}

#ifdef OLDCODE



/******************************************************************************/
static XtRealizeProc Realize ( Widget w, XtValueMask *value_mask,
                               XSetWindowAttributes *attributes ) {
    XmMaterialEditWidget xmw = ( XmMaterialEditWidget ) w;
    Display *dis = XtDisplay ( w );
    Pixel background, foreground;
    Dimension width, height;
    Widget leftp;
    XImage *img;
    G3DUI *gui;
    int depth;

    /*** this is required if we want to inherit the event mask. ***/
    /*** Redisplay procedure would not be called otherwise      ***/
    xmManagerClassRec.core_class.realize ( w, value_mask, attributes );

    XtVaGetValues ( w, XmNuserData, &gui,
                       XmNdepth, &depth,
                       XmNwidth, &width,
                       XmNheight, &height,
                       XmNbackground, &background,
                       XmNforeground, &foreground, NULL );

    img = XCreateImage ( dis, DefaultVisual ( dis, 0x00 ),
                         depth, ZPixmap, 0x00, NULL,
                         MATERIALEDITIMGSIZE, /*** Square Image ...   ***/
                         MATERIALEDITIMGSIZE, /*** ... Width = Height ***/
                         MATERIALEDITIMGDEPTH,
                         0x00 );

    img->data = ( char * ) calloc ( img->height * img->bytes_per_line, 0x01 );

    if ( img->data == NULL ) {
        fprintf ( stderr, "addPreview: memory allocation failed\n" );

        return;
    }

    xmw->materialedit.img = img;

    if ( gui->selmat ) {
        drawSphere ( gmw->materialedit.img,
                     xmMaterialEditClassRec.materialedit.vecmap, gui->selmat );
    }


    return ( XtRealizeProc ) NULL;
}

#endif
