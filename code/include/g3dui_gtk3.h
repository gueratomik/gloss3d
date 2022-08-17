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
typedef struct _G3DUIMENU {
    unsigned char     *name;
    uint32_t           type;
    uint32_t         (*condition) ( G3DUI *gui );
    void              *callback;
    GtkWidget         *item;
    GtkWidget         *menu;
    struct _G3DUIMENU *nodes[];
} G3DUIMENU;

/******************************************************************************/
typedef struct _G3DUIWIDGETGROUP {
    G3DUI *gui;
} G3DUIWIDGETGROUP;

/******************************************************************************/
typedef struct _GTK3G3DUI {
    G3DUI gui;
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
    LIST         *lview;
    LIST         *lglview;
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
} GTK3G3DUI;

/******************************************************************************/
typedef struct _GTK3G3DUITOOLBAR {
    G3DUITOOLBAR grp;
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
typedef struct _GTK3G3DUIVIEW {
    G3DUIVIEW  grp;
    GtkWidget *layout;
    GtkWidget *btnarea;
    GtkWidget *glarea;
    GtkWidget *optmenu;
    GtkWidget *shdmenu;
    LIST      *lmenu;
} GTK3G3DUIVIEW;

/******************************************************************************/
typedef struct _GTK3G3DUIMAIN {
    G3DUIMAIN         grp;
    GTK3G3DUITOOLBAR *toolBar;

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
    /*G3DUIMENU *menuBar;*/
} GTK3G3DUIMAIN;

/******************************************************************************/
typedef struct _G3DUIRENDERWINDOW {
    G3DUIWIDGETGROUP    grp;
    GtkWidget          *menuBar;
    GtkWidget          *fileMenu;
    GtkWidget          *drawingArea;
    GtkWidget          *scrolledWindow;
    GtkWidget          *statusBar;
    GtkWidget          *topLevel;
    Q3DFILTER          *tostatus;
    G3DUIRENDERPROCESS *rps;
    G3DUIRENDERBUFFER   rbuf;
} G3DUIRENDERWINDOW;

/******************************************************************************/
typedef struct _G3DUITIMELINE {
    G3DUIWIDGETGROUP grp;
    TIMELINEDATA    *tdata;
    GtkWidget       *menu;
} G3DUITIMELINE;

/******************************************************************************/
/****************************** Toolkit Data **********************************/
typedef struct _G3DUIGTK3 {
    GtkWidget *top;
    GtkWidget *mainView; /*** Main OpenGL View ***/
    GtkWidget *curogl; /*** current OpenGL Widget - the one we used last ***/
    GtkWidget *curmou; /*** store the current pressed toggle button      ***/
    GtkWidget *curmat; /*** current material ***/
    GtkWidget *main;
    GdkWindow *winAtPosition; /*** window at mouse position (for hourGlass)***/
    GtkWidget *currentUVMouseToolButton;
    GtkWidget *currentUVMapEditor;
    G3DUIMENU *menuBar;
} G3DUIGTK3;

/***************************** Main Window Widget *****************************/
typedef struct _GtkGlossUI {
    GtkFixed parent_instance;
    G3DUI gui;
} GtkGlossUI;

/******************************************************************************/
typedef struct _GtkGlossUIClass {
  GtkFixedClass parent_class;
   /* private signal */
   void (*action) (GtkGlossUI * gui );
} GtkGlossUIClass;

/******************************************************************************/
GType      gtk_glossui_get_type    (void) G_GNUC_CONST;
GtkWidget *gtk_glossui_new ( const char *filename );




/******************************************************************************/
/****************************** GTK Quad Widget *******************************/
typedef struct _GtkQuad {
    GtkFixed   parent_instance;
    G3DUIQUAD  quad;         /*** this can be shared across UI Toolkits ***/
    GtkWidget *maximized_view; /*** This can't ***/
} GtkQuad;

/******************************************************************************/
typedef struct _GtkQuadClass {
    GtkFixedClass parent_class;
} GtkQuadClass;

/******************************************************************************/
GType      gtk_quad_get_type    (void) G_GNUC_CONST;
GtkWidget *gtk_quad_new ( );
void       gtk_quad_size_allocate ( GtkWidget *, GtkAllocation * );

#define GTK_TYPE_QUAD    (gtk_quad_get_type ())
#define GTK_IS_QUAD(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_QUAD))

/******************************************************************************/
/******************************************************************************/




/******************************************************************************/
/****************************** GTK View Widget *******************************/
typedef struct _GtkView {
    GtkFixed  parent_instance;
    G3DUIVIEW view;
    cairo_surface_t *surface;
} GtkView;

/******************************************************************************/
typedef struct _GtkViewClass {
    GtkFixedClass parent_class;
    GdkPixbuf *icon[NBVIEWBUTTON];
} GtkViewClass;

/******************************************************************************/
GType      gtk_view_get_type    (void) G_GNUC_CONST;
GtkWidget *gtk_view_new     ( G3DCAMERA *, G3DUI *gui );
GtkWidget *gtk_view_getGLArea ( GtkWidget *widget );
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/************************** GTK UVMap Editor Widget ***************************/
typedef struct _GtkUVMapEditor {
    GtkFixed  parent_instance;
    M3DUI mui;
    cairo_surface_t *surface;
} GtkUVMapEditor;

/******************************************************************************/
typedef struct _GtkUVMapEditorClass {
    GtkFixedClass parent_class;
    GdkPixbuf *icon[NBUVMAPBUTTON];
} GtkUVMapEditorClass;

/******************************************************************************/
GType      gtk_uvmapeditor_get_type       (void) G_GNUC_CONST;
GtkWidget *gtk_uvmapeditor_new            ( );
GtkWidget *gtk_uvmapeditor_getDrawingArea ( GtkWidget *widget );
/******************************************************************************/
/******************************************************************************/

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

GTK3MATERIALPREVIEW *gtk3materialpreview_new    ( G3DMATERIAL *,
                                                  uint32_t width,
                                                  uint32_t height );
void                 gtk3materialpreview_free   ( GTK3MATERIALPREVIEW * );
void                 gtk3materialpreview_update ( GTK3MATERIALPREVIEW * );



/******************************************************************************/
G3DUIGTK3 *g3duigtk3_new  ( GtkWidget * );
void       g3duigtk3_free ( G3DUIGTK3 * );

void gdkevent_to_g3devent ( GdkEvent *gdkev, G3DEvent *g3dev );

/******************************************************************************/
GtkWidget *createModeBar          ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

GtkWidget *createToolBar          ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void g3dui_updateMenuBar ( G3DUI *gui );
void createMenuBar ( G3DUI     *gui,
                     char      *name,
                     gint       x,
                     gint       y,
                     gint       width,
                     gint       height );

GtkWidget *createQuad             ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

GtkWidget *createView             ( GtkWidget *, G3DUI *, char *, G3DCAMERA *,
                                                                  gint, gint );

GtkWidget *createUVMapEditor ( GtkWidget *parent,
                               G3DUI     *gui,
                               char      *name,
                               gint       width,
                               gint       height );

GtkWidget *createMainBoard        ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

GtkWidget *createObjectBoard      ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

GtkWidget *createObjectList       ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );
GtkWidget *createMaterialBoard    ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

GtkWidget *createMaterialList     ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

GtkWidget *createTimeBoard        ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

GtkWidget *createRenderWindow     ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

GtkWidget *createComboBox         ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

GtkWidget *createWeightgroupList  ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

GtkWidget *createFrame            ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       createSimpleLabel      ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

GtkWidget *createBoneLinker       ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

GtkWidget *createPanel            ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

/******************************************************************************/
void       updateUVMapEdit        ( GtkWidget *, G3DUI * );
GtkWidget *createUVMapEdit        ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateSphereEdit       ( GtkWidget *, G3DUI * );
GtkWidget *createSphereEdit       ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );
 
void       updateCubeEdit         ( GtkWidget *, G3DUI * );
GtkWidget *createCubeEdit         ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateCylinderEdit     ( GtkWidget *, G3DUI * );
GtkWidget *createCylinderEdit     ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updatePlaneEdit        ( GtkWidget *, G3DUI * );
GtkWidget *createPlaneEdit        ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );
 
void       updateTorusEdit        ( GtkWidget *, G3DUI * );
GtkWidget *createTorusEdit        ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateMeshEdit         ( GtkWidget *, G3DUI * );
GtkWidget *createMeshEdit         ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateFFDEdit          ( GtkWidget *, G3DUI * );
GtkWidget *createFFDEdit          ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateSymmetryEdit     ( GtkWidget *, G3DUI * );
GtkWidget *createSymmetryEdit     ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateBoneEdit         ( GtkWidget *, G3DUI * );
GtkWidget *createBoneEdit         ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateObjectEdit       ( GtkWidget *, G3DUI * );
GtkWidget *createObjectEdit       ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateCurrentEdit      ( GtkWidget *, G3DUI * );
GtkWidget *createCurrentEdit      ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateLightEdit        ( GtkWidget *, G3DLIGHT * );
GtkWidget *createLightEdit        ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateCameraEdit       ( GtkWidget *, G3DUI * );
GtkWidget *createCameraEdit       ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateMaterialEdit     ( GtkWidget *, G3DUI * );
GtkWidget *createMaterialEdit     ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateTextEdit         ( GtkWidget *, G3DUI * );
GtkWidget *createTextEdit         ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateCoordinatesEdit  ( GtkWidget *, G3DUI * );
GtkWidget *createCoordinatesEdit  ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateKeyEdit          ( GtkWidget *, G3DUI * );
GtkWidget* createKeyEdit          ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateTextureEdit      ( GtkWidget *, G3DUI * );
GtkWidget *createTextureEdit      ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateTimeline         ( GtkWidget *, G3DUI * );
GtkWidget *createTimeline         ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void timeline_setTool ( GtkWidget *timeline, uint32_t tool );
void timeline_incZoom ( GtkWidget *timeline, uint32_t nbpix );
void timeline_decZoom ( GtkWidget *timeline, uint32_t nbpix );

void       updatePickToolEdit     ( GtkWidget *, G3DMOUSETOOL * );
GtkWidget *createPickToolEdit     ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateSculptToolEdit   ( GtkWidget *, G3DMOUSETOOL *tool );
GtkWidget *createSculptToolEdit   ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateRenderEdit       ( GtkWidget *, Q3DSETTINGS *rsg );
GtkWidget *createRenderEdit       ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateG3DMouseTool ( GtkWidget *, G3DUI * );
GtkWidget *createG3DMouseToolEdit ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateCutMeshToolEdit  ( GtkWidget *, G3DUI * );
GtkWidget *createCutMeshToolEdit  ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );
void updateTubeEdit ( GtkWidget *widget, G3DUI *gui );
GtkWidget *createTubeEdit ( GtkWidget *parent, G3DUI *gui, 
                                                   char *name,
                                                   gint x, 
                                                   gint y,
                                                   gint width,
                                                   gint height );

void updateSubdividerEdit ( GtkWidget     *w, 
                            G3DSUBDIVIDER *sdr );

GtkWidget *createSubdividerEdit ( GtkWidget *parent, G3DUI *gui,
                                                     char *name,
                                                     gint x,
                                                     gint y,
                                                     gint width,
                                                     gint height );

void updateSplineRevolverEdit ( GtkWidget *widget, G3DUI *gui );
GtkWidget *createSplineRevolverEdit ( GtkWidget *parent, G3DUI *gui,
                                                     char *name,
                                                     gint x,
                                                     gint y,
                                                     gint width,
                                                     gint height );

void updateWireframeEdit ( GtkWidget *widget, G3DUI *gui );
GtkWidget *createWireframeEdit ( GtkWidget *parent, G3DUI *gui,
                                                     char *name,
                                                     gint x,
                                                     gint y,
                                                     gint width,
                                                     gint height );

void createProceduralResSelection ( GtkWidget *parent, 
                                           G3DUI     *gui, 
                                           char      *name,
                                           gint       x, 
                                           gint       y,
                                           gint       labwidth,
                                           gint       txtwidth,
                                           void     (*cbk)( GtkWidget *, 
                                                            gpointer ) );

GtkWidget* createProceduralBrickEdit ( GtkWidget           *parent,
                                       G3DUI               *gui,
                                       G3DPROCEDURALBRICK *brick,
                                       char               *name,
                                       gint                x,
                                       gint                y,
                                       gint                width,
                                       gint                height,
                                       uint32_t            bindGL );

GtkWidget* createProceduralGradientEdit ( GtkWidget             *parent,
                                          G3DUI                 *gui,
                                          G3DPROCEDURALGRADIENT *gradient,
                                          char                  *name,
                                          gint                   x,
                                          gint                   y,
                                          gint                   width,
                                          gint                   height,
                                          uint32_t               bindGL );

GtkWidget* createKeyMorpherEdit ( GtkWidget  *parent,
                                  G3DMORPHER *mpr,
                                  G3DKEY     *key,
                                  G3DUI      *gui,
                                  gint        x,
                                  gint        y,
                                  gint        width,
                                  gint        height );

GtkWidget *createTimeContextMenu ( GtkWidget     *parent,
                                   G3DUITIMELINE *tim,
                                   char          *name,
                                   gint           width,
                                   gint           height );
  
/******************************************************************************/
GtkWidget *createHorizontalScale ( GtkWidget *, G3DUI *,
                                                char *,
                                                gint, gint,
                                                gint, gint,
                                                float, float, float,
                                                void (*)( GtkWidget *, 
                                                          gpointer ) );

GtkWidget *createColorButton     ( GtkWidget *, G3DUI *,
                                                char *,
                                                gint, gint,
                                                gint, gint,
                                                void (*)( GtkWidget *, 
                                                          gpointer ) );

GtkWidget *createImageButton     ( GtkWidget *, G3DUI *,
                                                char *,
                                                const char **,
                                                gint, gint,
                                                gint, gint,
                                                void (*)( GtkWidget *,
                                                          gpointer ) );

GtkWidget *createButton          ( GtkWidget *, G3DUI *,
                                                char *,
                                                const char **,
                                                gint, gint,
                                                gint, gint,
                                                void (*)( GtkWidget *, 
                                                          gpointer ) );

GtkWidget *createPushButton      ( GtkWidget *, G3DUI *,
                                                char *,
                                                gint, gint,
                                                gint, gint,
                                                void (*)( GtkWidget *, 
                                                          gpointer ) );

GtkWidget *createToggleLabel     ( GtkWidget *, G3DUI *,
                                                char *,
                                                gint, gint,
                                                gint, gint,
                                                void (*)( GtkWidget *, 
                                                          gpointer ) );

GtkWidget *createRadioLabel     ( GtkWidget *, G3DUI *,
                                                char *,
                                                GtkWidget *,
                                                gint, gint,
                                                gint, gint,
                                                void (*)( GtkWidget *, 
                                                          gpointer ) );

GtkWidget *createIntegerText     ( GtkWidget *, G3DUI *,
                                                char *,
                                                gint, gint,
                                                gint, gint,
                                                gint, gint,
                                                void (*)( GtkWidget *, 
                                                          gpointer ) );

GtkWidget *createFloatText       ( GtkWidget *, G3DUI *,
                                                char *,
                                                double, double,
                                                gint, gint,
                                                gint, gint,
                                                void (*)( GtkWidget *, 
                                                          gpointer ) );

GtkWidget *createCharText ( GtkWidget *parent, 
                            G3DUI     *gui,
                            char      *name,
                            gint      x, 
                            gint      y,
                            gint      labwidth,
                            gint      txtwidth,
                            void (*cbk)( GtkWidget *,
                                         GdkEvent  *, 
                                         gpointer ) );


void createTextField ( GtkWidget *parent, 
                       G3DUI     *gui,
                       char      *name,
                       gint       x, 
                       gint       y,
                       gint       textWidth,
                       gint       textHeight,
                       void     (*cbk)( GtkTextBuffer *,
                                        gpointer ) );

GtkWidget *createOrientationSelection ( GtkWidget *parent,
                                        G3DUI     *gui, 
                                        char      *name,
                                        char      *str0,
                                        char      *str1,
                                        char      *str2,
                                        gint      x,
                                        gint      y,
                                        gint      labwidth,
                                        gint      txtwidth,
                                        void    (*cbk)( GtkWidget *, 
                                                        gpointer ) );

void createProceduralResolutionSelection ( GtkWidget *, 
                                           G3DUI     *, 
                                           char      *,
                                           gint       , 
                                           gint       ,
                                           gint       ,
                                           gint       ,
                                           void     (*)( GtkWidget *, 
                                                         gpointer ) );
void createProceduralTypeSelection ( GtkWidget *, 
                                     G3DUI     *, 
                                     char      *,
                                     gint       , 
                                     gint       ,
                                     gint       ,
                                     gint       ,
                                     void     (*)( GtkWidget *, 
                                                   gpointer ) );

GtkWidget *createDrawingArea ( GtkWidget *parent, G3DUI *gui,
                                                  char *name,
                                                  gint x, 
                                                  gint y,
                                                  gint width,
                                                  gint height,
                                                  void (*cbk)( GtkWidget *, 
                                                               cairo_t *cr,
                                                               gpointer ) );

void dispatchMenuButton  ( G3DUI *, G3DMOUSETOOL *, uint32_t );

/**************************** Toolbar or menu Callbacks ***********************/
void g3dui_openfilecbk        ( GtkWidget *, gpointer );
void g3dui_newscenecbk        ( GtkWidget *, gpointer );
void g3dui_deleteSelectionCbk ( GtkWidget *, gpointer );
void g3dui_makeEditableCbk    ( GtkWidget *, gpointer );
void g3dui_saveascbk          ( GtkWidget *, gpointer );
void g3dui_savefilecbk        ( GtkWidget *, gpointer );
void g3dui_undoCbk            ( GtkWidget *, gpointer );
void g3dui_redoCbk            ( GtkWidget *, gpointer );
void g3dui_renderViewCbk      ( GtkWidget *, gpointer );
void g3dui_stopCbk            ( GtkWidget *, gpointer );
void g3dui_playCbk            ( GtkWidget *, gpointer );
void g3dui_runRenderCbk       ( GtkWidget *, gpointer );
void g3dui_mergeSceneCbk ( GtkWidget *widget, gpointer user_data );
gboolean g3dui_exitEventCbk ( GtkWidget *widget, 
                              GdkEvent  *event, 
                              gpointer   user_data );

uint32_t g3dui_cancelRenderByScene ( G3DUI *, Q3DJOB * );
uint32_t g3dui_cancelRenderByID    ( G3DUI *, uint64_t   );
void g3dui_redrawGLViews ( G3DUI * );
void gtk_view_updateMenu ( GtkWidget * );
void g3dui_updateGLViewsMenu ( G3DUI *gui );
void addGLMenuButton ( G3DUI *, GtkWidget *, G3DMOUSETOOL * );
GtkWidget *createViewMenu ( GtkWidget *, G3DUI *, char * );

void g3duimateriallist_removeAllPreviews ( GtkWidget * );
void g3duimateriallist_importFromScene   ( GtkWidget *, G3DSCENE *    );
void g3duimateriallist_updatePreview     ( GtkWidget *, G3DMATERIAL * );
void g3duimateriallist_addPreview        ( GtkWidget *, G3DMATERIAL * );
void g3duimateriallist_removePreview     ( GtkWidget *, G3DMATERIAL * );
void g3duimateriallist_removeMaterial    ( GtkWidget *, G3DSCENE *,
                                                        G3DURMANAGER *,
                                                        G3DMATERIAL * );
void g3duimateriallist_size              ( GtkWidget *, GdkRectangle *, 
                                                        gpointer );

void gtk3_g3dui_setMode   ( GtkWidget *, gpointer );
void gtk3_g3dui_setMouseTool   ( GtkWidget *, gpointer );
void m3dui_setUVMouseTool ( GtkWidget *, gpointer );
gboolean gtk3_showGL      ( GtkWidget *, cairo_t *, gpointer );
void gtk3_initGL          ( GtkWidget *, gpointer );
void gtk3_sizeGL          ( GtkWidget *, GdkRectangle *, gpointer );
gboolean gtk3_inputGL     ( GtkWidget *, GdkEvent *, gpointer );

void g3dui_redrawUVMapEditors ( G3DUI *gui );
void g3dui_resizeUVMapEditors ( G3DUI *gui );
GtkWidget *gtk_uvmapeditor_getGLArea ( GtkWidget *widget );

void g3dui_updateAllCurrentEdit          ( G3DUI * );
void g3dui_updateCoords                  ( G3DUI * );
void g3dui_redrawObjectList              ( G3DUI * );
void g3dui_clearMaterials                ( G3DUI * );
void g3dui_importMaterials               ( G3DUI * );
void g3dui_redrawMaterialList            ( G3DUI * );
void g3dui_redrawWidgetList              ( G3DUI *, LIST * );
void g3dui_updateSelectedMaterialPreview ( G3DUI * );
void g3dui_updateMaterialEdit            ( G3DUI * );
void g3dui_redrawTimeline                ( G3DUI * );
void g3dui_updateKeyEdit                 ( G3DUI * );
void g3dui_redrawAllWeightGroupList      ( G3DUI * );
void g3dui_redrawAllMeshPoseList         ( G3DUI * );
void g3dui_redrawUVMapEditors ( G3DUI *gui );
void g3dui_updateMenuBar ( G3DUI *gui );
void updateCurrentMouseTool ( GtkWidget *widget, G3DUI *gui );
void g3dui_loadImageForChannel ( G3DUI      *gui,
                                 G3DCHANNEL *chan );

G3DUIRENDERPROCESS *g3dui_getRenderProcessByScene ( G3DUI *, Q3DJOB * );
G3DUIRENDERPROCESS *g3dui_getRenderProcessByID    ( G3DUI *, uint64_t   );

uint32_t g3dui_renderClean ( Q3DFILTER *, Q3DJOB *, float,
                                                      unsigned char *, 
                                                      uint32_t, 
                                                      uint32_t, 
                                                      uint32_t, 
                                                      uint32_t );

G3DUIRENDERPROCESS *g3dui_render ( G3DUI *, uint64_t,
                                            uint32_t, 
                                            uint32_t,
                                            uint32_t, 
                                            uint32_t,
                                            uint32_t, 
                                            uint32_t,
                                            LIST *, 
                                            uint32_t );
void addMenuListButton ( G3DUI *gui, LIST *lmenu, G3DMOUSETOOL *mou );
void g3dui_updateGLViewsMenu ( G3DUI *gui );
GtkWidget *g3dui_addMenuSeparator ( GtkWidget * );
GtkWidget *g3dui_addMenuButton    ( GtkWidget *, G3DUI *,
                                                 const gchar *,
                                                 gint,
                                                 GCallback );
GtkWidget *g3dui_addMenuToggle ( GtkWidget *menu, G3DUI *gui,
                                                  const gchar *name,
                                                  gint width,
                                                  GCallback callback );

void g3dui_aboutCbk ( GtkWidget *widget, gpointer user_data );

G3DCAMERA *g3dui_getCurrentViewCamera ( G3DUI *gui );
G3DCAMERA *g3dui_getMainViewCamera ( G3DUI *gui );

void g3duicom_handleAction ( GtkWidget *, gpointer, gpointer );
void g3duicom_gotoframe    ( G3DUI *, GOTOFRAME  * );
void g3duicom_dumpscreen   ( G3DUI *, DUMPSCREEN * );
void g3duicom_requestActionFromMainThread ( G3DUI *gui, G3DUIACTION *action );

/******************************************************************************/
#define TOGDKWINDOWFILTERNAME "TOGDKWINDOWFILTERNAME"


typedef struct _FILTERTOGDKWINDOW {
    cairo_surface_t *crsurf;
    cairo_t         *cr;
    GdkWindow       *gdkwin;
} FILTERTOGDKWINDOW;

Q3DFILTER *q3dfilter_toGtkWidget_new ( GtkWidget *, uint32_t );
Q3DFILTER *r3dfilter_toGtkWidget_new ( GtkWidget *, uint32_t );
void       filtertogdkwindow_free ( FILTERTOGDKWINDOW * );
void       filtertogdkwindow_draw ( void *, Q3DJOB *, unsigned char *,
                                                        uint32_t, 
                                                        uint32_t, 
                                                        uint32_t, 
                                                        uint32_t );
FILTERTOGDKWINDOW *filtertogdkwindow_new ( GdkWindow * );

typedef struct _FILTERTOSTATUSBAR {
    GtkWidget *widget;
    float lastFrame;
    uint32_t done;
} FILTERTOSTATUSBAR;

Q3DFILTER *q3dfilter_toStatusBar_new ( GtkWidget *widget, float lastFrame );
uint32_t filtertostatusbar_getStatus ( Q3DFILTER *fil );

/******************************************************************************/
void updateSaveOutputForm ( GtkWidget *widget, G3DUI *gui );
void updateMotionBlurForm ( GtkWidget *widget, G3DUI *gui );

void m3dui_undoCbk ( GtkWidget *widget, gpointer user_data );
void m3dui_redoCbk ( GtkWidget *widget, gpointer user_data );
void m3dui_uv2verCbk ( GtkWidget *widget, gpointer user_data );
void m3dui_ver2uvCbk ( GtkWidget *widget, gpointer user_data );

GtkWidget *createRenderWindowMenuBar ( GtkWidget         *parent,
                                       G3DUIRENDERWINDOW *rwn,
                                       char              *name,
                                       gint               x,
                                       gint               y,
                                       gint               width,
                                       gint               height );

GtkWidget *createOptionMenu ( GtkWidget *parent, 
                              GtkWidget *gvw,
                              char      *name,
                              gint       x,
                              gint       y,
                              gint       width,
                              gint       height );

GtkWidget *createMaterialMenuBar ( GtkWidget *parent, 
                                   G3DUI     *gui,
                                   char      *name,
                                   gint       x,
                                   gint       y,
                                   gint       width,
                                   gint       height );

GtkWidget *createObjectsMenuBar ( GtkWidget *parent, 
                                  G3DUI     *gui,
                                  char      *name,
                                  gint       x,
                                  gint       y,
                                  gint       width,
                                  gint       height );

GtkWidget *createUVMenuBar ( GtkWidget        *parent,  
                             M3DUI *mui,
                             char             *name,
                             gint              x,
                             gint              y,
                             gint              width,
                             gint              height );

GtkWidget *createFgBgButton ( GtkWidget *parent, 
                              G3DUI     *gui,
                              char      *name,
                              gint       x,
                              gint       y,
                              gint       width,
                              gint       height );

GtkWidget* createChannelImage ( GtkWidget        *parent,
                                M3DUI *mui,
                                G3DCHANNEL       *chn,
                                uint32_t          resize,
                                char             *name,
                                gint              x,
                                gint              y,
                                gint              width,
                                gint              height );

GtkWidget *createUVMapEditorModeBar ( GtkWidget *parent, 
                                      G3DUI     *gui,
                                      char      *name,
                                      gint       x,
                                      gint       y,
                                      gint       width,
                                      gint       height );

GtkWidget *createUVMapEditorToolBar ( GtkWidget *parent, 
                                      G3DUI     *gui,
                                      char      *name,
                                      gint       x,
                                      gint       y,
                                      gint       width,
                                      gint       height );

GtkWidget *createPatternList ( GtkWidget        *parent, 
                               M3DUI *mui,
                               char             *name,
                               gint              x,
                               gint              y,
                               gint              width,
                               gint              height );

GtkWidget* createProceduralChessEdit ( GtkWidget           *parent,
                                       G3DUI               *gui,
                                       G3DPROCEDURALCHESS *chess,
                                       char               *name,
                                       gint                x,
                                       gint                y,
                                       gint                width,
                                       gint                height,
                                       uint32_t            bindGL );

GtkWidget* createProceduralNoiseEdit ( GtkWidget           *parent,
                                       G3DUI               *gui,
                                       G3DPROCEDURALNOISE *noise,
                                       char               *name,
                                       gint                x,
                                       gint                y,
                                       gint                width,
                                       gint                height,
                                       uint32_t            bindGL );

GtkWidget *createScaleKeysDialog ( GtkWidget *parent,
                                   G3DUI     *gui,
                                   char      *name,
                                   gint       x,
                                   gint       y,
                                   gint       width,
                                   gint       height );

uint32_t g3dui_saveAlteredImages ( G3DUI *gui );
uint32_t g3dui_saveChannelAlteredImage ( G3DUI      *gui,
                                         char       *materialName,
                                         G3DCHANNEL *chn,
                                         uint32_t    ask,
                                         uint32_t    rename );

void m3dui_loadImageByChannelIDCbk ( GtkWidget *widget, 
                                                gpointer   user_data );

void m3dui_createChannelImageCbk ( GtkWidget *widget, 
                                              gpointer   user_data );

GtkWidget *createPenToolEdit ( GtkWidget        *parent, 
                               M3DUI *mui,
                               char             *name,
                               gint              x,
                               gint              y,
                               gint              width,
                               gint              height );

void updatePenToolEdit ( GtkWidget        *widget,
                         M3DUI *mui );

GtkWidget *createBucketToolEdit ( GtkWidget        *parent, 
                                  M3DUI *mui,
                                  char             *name,
                                  gint              x,
                                  gint              y,
                                  gint              width,
                                  gint              height );

void updateBucketToolEdit ( GtkWidget *widget,
                            M3DUI     *mui );

GtkWidget *createM3DMouseToolEdit ( GtkWidget        *parent, 
                                    M3DUI *mui,
                                    char             *name,
                                    gint              x,
                                    gint              y,
                                    gint              width,
                                    gint              height );
void updateM3DMouseTool ( GtkWidget        *widget, 
                          M3DUI *mui );
void m3dui_updateMouseToolEdit ( M3DUI *mui );

GtkWidget *createMorpherEdit ( GtkWidget *parent,
                               G3DUI     *gui,
                               char      *name,
                               gint       x,
                               gint       y,
                               gint       width,
                               gint       height );
void updateMorpherEdit ( GtkWidget *widget, 
                         G3DUI     *gui );
void updateInstanceEdit ( GtkWidget    *w, 
                          G3DINSTANCE  *ins );
GtkWidget* createTrackerTagEdit ( GtkWidget     *parent, 
                                  G3DUI         *gui,
                                  char          *name,
                                  gint           x,
                                  gint           y,
                                  gint           width,
                                  gint           height );

GtkWidget *createInstanceEdit ( GtkWidget *parent,
                                G3DUI     *gui,
                                char      *name,
                                gint       x,
                                gint       y,
                                gint       width,
                                gint       height );

GtkWidget* getChild ( GtkWidget* parent, const gchar* name);

Q3DFILTER *q3dfilter_gotoframe_new ( G3DUI* gui );

G3DCAMERA *g3dui_getCurrentUVMapEditorCamera ( G3DUI *gui );

GtkWidget *addToolBarPushButton ( GtkWidget   *bar,
                                  void        *data,
                                  char        *name,
                                  const char **xpm_data,
                                  void       (*cbk) ( GtkWidget *,
                                                      gpointer ) );

GtkWidget *addToolBarToggleButton ( GtkWidget   *bar,
                                    void        *data,
                                    char        *name,
                                    const char **xpm_data,
                                    void       (*cbk) ( GtkWidget *, 
                                                        gpointer ) );


void updateParticleEmitterEdit ( GtkWidget           *w, 
                                 G3DPARTICLEEMITTER  *pem );
GtkWidget *createParticleEmitterEdit ( GtkWidget *parent,
                                       G3DUI     *gui,
                                       char      *name,
                                       gint       x,
                                       gint       y,
                                       gint       width,
                                       gint       height );

void g3duirenderbuffer_clear ( G3DUIRENDERBUFFER *rbuf );

/******************************************************************************/
void g3duirenderbuffer_init ( G3DUIRENDERBUFFER *rbuf,
                              GtkWidget         *drawingArea );

GtkWidget *gtk3_g3duimain_create ( GtkWidget *parent,
                                   char      *name,
                                   gint       x,
                                   gint       y,
                                   gint       width,
                                   gint       height );

void gtk3_g3dui_interpretMouseToolReturnFlags ( G3DUI   *gui, 
                                                uint32_t msk );
void gtk3_g3dui_dispatchGLMenuButton ( G3DUI        *gui, 
                                       G3DMOUSETOOL *mou, 
                                       uint32_t      tool_flags );

GTK3G3DUITOOLBAR *gtk3_g3duitoolbar_create ( GtkWidget *parent, 
                                             char      *name,
                                             gint       x,
                                             gint       y,
                                             gint       width,
                                             gint       height );

void gtk3_g3dui_redrawGLViews ( );
void gtk3_g3dui_updateGLViewsMenu ( );
void gtk3_g3dui_redrawUVMapEditors ( );
void gtk3_g3dui_resizeUVMapEditors ( );
void gtk3_g3dui_importMaterials ( );
void gtk3_g3dui_clearMaterials ( );
void gtk3_g3dui_updateSelectedMaterialPreview ( );
void gtk3_g3dui_updateKeyEdit ( );
void gtk3_g3dui_updateAllCurrentMouseTools ( );
void gtk3_initDefaultMouseTools ( G3DCAMERA *cam );

GTK3G3DUI *gtk3_getUI ( );

void g3duirectangle_toGdkRectangle ( G3DUIRECTANGLE *in, 
                                     GdkRectangle   *out  );

#endif
