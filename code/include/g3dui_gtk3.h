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

#ifndef _G3DUI_GTK3_H_
#define _G3DUI_GTK3_H_

/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/******************************************************************************/
#include <libgen.h>

/******************************************************************************/
#ifdef __linux__
#include <X11/X.h>
#include <X11/Xlib.h>
#endif

/******************************************************************************/
#ifdef __linux__
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#endif

/******************************************************************************/
#ifdef __linux__
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#endif

#ifdef __MINGW32__
#include <GL/gl.h>
#include <GL/glu.h>
#endif

/******************************************************************************/
#include <glib.h>
#include <glib-object.h>
#include <cairo/cairo.h>
#include <pango/pango.h>
#include <gdk/gdk.h>
#ifdef __linux__
    #include <gdk/gdkx.h>
#endif
#ifdef __MINGW32__
    #include <gdk/gdkwin32.h>
#endif
#include <gtk/gtk.h>

/******************************************************************************/
#include <list.h>
#include <g3dengine/g3dengine.h>
#include <qiss3d/q3d.h>

#ifdef __linux__
/*#include <r3dnet.h>*/
#endif

#include <g3durmanager.h>
#include <g3dexport.h>
#include <g3dimport.h>
#include <g3dmouse.h>
#include <g3dui.h>

/******************************************************************************/
#define GTK3WIDGETDATA "GTK3WIDGETDATA"

#define GDKRGBA_MULTI (GdkRGBA){.red   = 1.0f, \
                                .green = 1.0f, \
                                .blue  = 1.0f }

/******************************************************************************/
/****************************** GTK Form Widget *******************************/
typedef struct _GtkForm {
    GtkFixed   parent_instance;
} GtkForm;

/******************************************************************************/
typedef struct _GtkFormClass {
    GtkFixedClass parent_class;
} GtkFormClass;

GType      gtk_form_get_type    (void) G_GNUC_CONST;
GtkWidget *gtk_form_new ( );

/******************************************************************************/
typedef struct _GTK3G3DUIMENU {
    G3DUIMENU  core;
    GtkWidget *item;
    GtkWidget *menu;
} GTK3G3DUIMENU;

/******************************************************************************/
typedef struct _G3DUIWIDGETGROUP {
    G3DUI *gui;
} G3DUIWIDGETGROUP;

/******************************************************************************/
typedef struct _GTK3G3DUI {
    G3DUI          core;
    /* context menus for all objects */
    LIST         *lObjectModeMenu;
    /* context menus for meshes */
    LIST         *lVertexModeMeshMenu;
    LIST         *lEdgeModeMeshMenu;
    LIST         *lFaceModeMeshMenu;
    LIST         *lSculptModeMeshMenu;
    /* context menu for splines */
    LIST         *lVertexModeSplineMenu;
    /* context menu for morphers */
    LIST         *lVertexModeMorpherMenu;
    /**********************/
    LIST         *lmtools; /*** list of mousetools widget ***/
    LIST         *lobjlist;
    LIST         *lmatlist;
    LIST         *lmatedit;
    LIST         *lcoordedit; /*** list of Coordinates widget ***/
    LIST         *lcuredit; /*** current object edit widget ***/
    LIST         *ltimeline; /*** List of timelines ***/
    LIST         *lkeyedit;
    LIST         *ltexedit;
    LIST         *lligedit;
    LIST         *lweightgroup;
    LIST         *lmeshpose;
    LIST         *luvmapeditor;
    GdkWindow    *winAtPosition; /*** window at mouse position (for hourGlass)***/

    GtkWidget *topWin;
    GtkWidget *curmou; /*** store the current pressed toggle button      ***/
    GtkWidget *curmat; /*** current material ***/
    GtkWidget *currentUVMouseToolButton;
    GtkWidget *currentUVMapEditor;
} GTK3G3DUI;

/******************************************************************************/
typedef struct _GTK3G3DUITOOLBAR {
    G3DUITOOLBAR core;
    GtkWidget   *bar;
    GtkWidget   *newScene;
    GtkWidget   *openFile;
    GtkWidget   *saveFile;
    GtkWidget   *saveFileAs;
    GtkWidget   *undo;
    GtkWidget   *redo;
    GtkWidget   *delete;
    GtkWidget   *pickTool;
    GtkWidget   *moveTool;
    GtkWidget   *scaleTool;
    GtkWidget   *rotateTool;
    GtkWidget   *renderView;
    GtkWidget   *renderFinal;
    GtkWidget   *makeEditable;
    GtkWidget   *xAxis;
    GtkWidget   *yAxis;
    GtkWidget   *zAxis;
} GTK3G3DUITOOLBAR;

/******************************************************************************/
typedef struct _GTK3G3DUIMODEBAR {
    G3DUIRENDERWINDOW   core;
    GtkWidget          *bar;
    GtkWidget          *objectMode;
    GtkWidget          *vertexMode;
    GtkWidget          *edgeMode;
    GtkWidget          *faceMode;
    GtkWidget          *sculptMode;
    GtkWidget          *skinMode;
    GtkWidget          *uvmapMode;
    GtkWidget          *axisMode;
    GtkWidget          *pathMode;
} GTK3G3DUIMODEBAR;

/******************************************************************************/
typedef struct _GTK3G3DUIVIEW {
    G3DUIVIEW  core;
    GtkWidget *layout;
    GtkWidget *navbar;
    GtkWidget *glarea;
    LIST      *lmenu;
    GdkPixbuf *pix[0x04];
} GTK3G3DUIVIEW;

/******************************************************************************/
typedef struct _GTK3G3DUIQUAD {
    G3DUIQUAD  core;
    GtkWidget *layout;
} GTK3G3DUIQUAD;

/******************************************************************************/
typedef struct _GTK3G3DUIMAIN {
    G3DUIMAIN         core;
    GtkWidget        *layout;
    GtkWidget        *quad;
    GtkWidget        *timeBoard;
    GtkWidget        *board;

    GtkWidget        *top;
    GtkWidget        *mainView; /*** Main OpenGL View ***/
    GtkWidget        *curogl; /*** current OpenGL Widget - the one we used last ***/
    GtkWidget        *curmou; /*** store the current pressed toggle button      ***/
    GtkWidget        *curmat; /*** current material ***/
    GtkWidget        *main;

    GtkWidget        *currentUVMouseToolButton;
    GtkWidget        *currentUVMapEditor;
} GTK3G3DUIMAIN;

/******************************************************************************/
typedef struct _GTK3G3DUIRENDERWINDOW {
    G3DUIRENDERWINDOW   core;

    GtkWidget          *menuBar;
    GtkWidget          *fileMenu;
    GtkWidget          *drawingArea;
    GtkWidget          *scrolledWindow;
    GtkWidget          *statusBar;
    GtkWidget          *topLevel;
} GTK3G3DUIRENDERWINDOW;

/******************************************************************************/
typedef struct _GTK3G3DUITIMELINE {
    G3DUITIMELINE    core;
    /*GtkWidget       *menu;*/
} GTK3G3DUITIMELINE;

/******************************************************************************/
typedef struct _GTK3G3DUIMATERIALBOARD {
    G3DUIMATERIALBOARD core;
    GtkWidget        *layout;
} GTK3G3DUIMATERIALBOARD;

/******************************************************************************/
typedef struct _GTK3G3DUIOBJECTLIST {
    G3DUIOBJECTLIST core;
    GtkWidget      *drawingArea;
} GTK3G3DUIOBJECTLIST;

/******************************************************************************/
typedef struct _GTK3G3DUIOBJECTEDIT {
    G3DUIOBJECTEDIT core;
    GtkWidget      *fixed;
    GtkWidget      *scrolled;
} GTK3G3DUIOBJECTEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUIOBJECTBOARD {
    G3DUIOBJECTBOARD core;
    GtkWidget       *layout;
} GTK3G3DUIOBJECTBOARD;

/******************************************************************************/
typedef struct _GTK3G3DUIMOUSETOOLEDIT {
    G3DUIMOUSETOOLEDIT core;
    GtkWidget         *layout;
} GTK3G3DUIMOUSETOOLEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUIBOARD {
    G3DUIBOARD core;
    GtkWidget *layout;
    GtkWidget *tab;
} GTK3G3DUIBOARD;

/******************************************************************************/
typedef struct _GTK3G3DUILIGHTEDIT {
    G3DUILIGHTEDIT core;

    GtkWidget     *tab;
    GtkWidget     *diffuseColorButton;
    GtkWidget     *specularColorButton;
    GtkWidget     *intensityEntry;
    GtkWidget     *castShadowsToggle;
    GtkWidget     *spotToggle;
    GtkWidget     *spotLengthEntry;
    GtkWidget     *spotAngleEntry;
    GtkWidget     *spotFadeAngleEntry;
    GtkWidget     *softShadowsToggle;
    GtkWidget     *shadowRadiusEntry;
    GtkWidget     *shadowSampleEntry;

    G3DLIGHT      *editedLight;
} GTK3G3DUILIGHTEDIT;

/******************************************************************************/
/************************** GTK PatternList Widget ***************************/
typedef struct _GTK3PATTERNPREVIEW {
    GdkPixbuf     *img;
    M3DPATTERN    *pat;
    G3DUIRECTANGLE rec;
} GTK3PATTERNPREVIEW;

/******************************************************************************/
/************************** GTK MaterialList Widget ***************************/
typedef struct _GTK3MATERIALPREVIEW {
    GdkPixbuf *img;
    G3DMATERIAL *mat;
    /*** The vector map that helps us to build the preview sphere ***/
    /*** This is per-preview because we need individual maps in case ***/
    /*** normal vectors are affected by bump maps for instance ***/
    G3DUIMATERIALMAP *map;
    G3DUIRECTANGLE rec;
} GTK3MATERIALPREVIEW;

/******************************************************************************/
#define TOGDKWINDOWFILTERNAME "TOGDKWINDOWFILTERNAME"

typedef struct _FILTERTOGDKWINDOW {
    cairo_surface_t *crsurf;
    cairo_t         *cr;
    GdkWindow       *gdkwin;
} FILTERTOGDKWINDOW;

typedef struct _FILTERTOSTATUSBAR {
    GtkWidget *widget;
    float lastFrame;
    uint32_t done;
} FILTERTOSTATUSBAR;

/******************************** css/default.c *******************************/
gchar *gtk3_getDefaultCSS ( );

/********************************* g3dui.c ************************************/

void g3duirectangle_toGdkRectangle ( G3DUIRECTANGLE *in, 
                                     GdkRectangle   *out  );
void gtk3_setMouseTool ( GtkWidget *widget, 
                         gpointer   user_data );
void gtk3_setHourGlass ( GTK3G3DUI *gtk3gui );
void gtk3_unsetHourGlass ( GTK3G3DUI *gtk3gui );
static void gtk3_updateAllCurrentMouseTools ( GTK3G3DUI *gtk3gui );
static void gtk3_updateKeyEdit ( GTK3G3DUI *gtk3gui );
void gtk3_initDefaultMouseTools ( GTK3G3DUI *gtk3gui );
void gtk3_interpretUIReturnFlags ( GTK3G3DUI *gtk3gui,
                                   uint64_t   msk );

GtkWidget *ui_createColorButton ( GtkWidget *parent,
                                  void      *data,
                                  char      *name,
                                  gint       x, 
                                  gint       y,
                                  gint       width,
                                  gint       height,
                                  void     (*cbk)( GtkWidget *, 
                                                   gpointer ) );

GtkWidget *ui_createSimpleLabel ( GtkWidget *parent, 
                                  void      *data,
                                  char      *name,
                                  gint       x, 
                                  gint       y,
                                  gint       width,
                                  gint       height );

/******************************* g3duiboard.c *********************************/
GTK3G3DUIBOARD *gtk3_g3duiboard_create ( GtkWidget *parent,
                                         GTK3G3DUI *gtk3gui,
                                         char      *name );

/***************************** g3duilightedit.c *******************************/
void gtk3_g3duilightedit_update ( GTK3G3DUILIGHTEDIT *gtk3led );
GTK3G3DUILIGHTEDIT *gtk3_g3duilightedit_create ( GtkWidget *parent,
                                                 GTK3G3DUI *gtk3gui,
                                                 char      *name );

/******************************* g3duimenu.c **********************************/
GTK3G3DUIMENU *gtk3_g3duimenu_parse_r ( G3DUIMENU *node,
                                        G3DUI     *gui,
                                        void      *data );
void gtk3_g3duimenu_update_r ( GTK3G3DUIMENU *gtk3node );

/******************************* g3duimain.c **********************************/
GTK3G3DUIMAIN *gtk3_g3duimain_create ( GtkWidget *parent,
                                       GTK3G3DUI *gtk3gui,
                                       char      *name,
                                       gint       x,
                                       gint       y,
                                       gint       width,
                                       gint       height,
                                       char      *filename );
void gtk3_g3duimain_updateMenuBar ( GTK3G3DUIMAIN *gtk3main );

/***************************** g3duitoolbar.c *********************************/
GTK3G3DUITOOLBAR *gtk3_g3duitoolbar_create ( GtkWidget *parent,
                                             GTK3G3DUI *gtk3gui,
                                             char      *name );

/***************************** g3duimodebar.c *********************************/
GTK3G3DUIMODEBAR *gtk3_g3duimodebar_create ( GtkWidget *parent,
                                             GTK3G3DUI *gtk3gui,
                                             char      *name );

/************************** g3duiobjectboard.c ********************************/
void gtk3_g3duiobjectboard_updateMenuBar ( GTK3G3DUIOBJECTBOARD *gtk3objbrd );
GTK3G3DUIOBJECTBOARD *gtk3_g3duiobjectboard_create ( GtkWidget *parent,
                                                     GTK3G3DUI *gtk3gui,
                                                     char      *name );

/************************** g3duiobjectedit.c ********************************/
void gtk3_g3duiobjectedit_update ( GTK3G3DUIOBJECTEDIT *gtk3objedit );
GTK3G3DUIOBJECTEDIT *gtk3_g3duiobjectedit_create ( GtkWidget *parent, 
                                                   GTK3G3DUI *gtk3gui,
                                                   char      *name );

/****************************** g3duiquad.c ***********************************/
GTK3G3DUIQUAD *gtk3_g3duiquad_create ( GtkWidget *parent,
                                       GTK3G3DUI *gtk3gui,
                                       char      *name );
void gtk3_g3duiquad_rearrange ( GTK3G3DUIQUAD *gtk3quad,
                                GTK3G3DUIVIEW *gtk3view );

/****************************** g3duiview.c ***********************************/
GTK3G3DUIVIEW *gtk3_g3duiview_create ( GtkWidget *parent, 
                                       GTK3G3DUI *gtk3gui,
                                       char      *name );
void gtk3_g3duiview_updateMenuBar ( GTK3G3DUIVIEW *gtk3view );

#endif
