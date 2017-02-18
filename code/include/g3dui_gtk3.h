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
#include <r3d.h>

#ifdef __linux__
#include <r3dnet.h>
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


/******************************************************************************/
/****************************** Toolkit Data **********************************/
typedef struct _G3DUIGTK3 {
    GtkWidget *top;
    GtkWidget *curogl; /*** current OpenGL Widget - the one we used last ***/
    GtkWidget *curmou; /*** store the current pressed toggle button      ***/
    GtkWidget *curmat; /*** current material ***/
    GtkWidget *main;
    GdkWindow *winAtPosition; /*** window at mouse position (for hourGlass)***/
} G3DUIGTK3;

/******************************************************************************/
#define ACTION_GOTOFRAME  0x01
#define ACTION_DUMPSCREEN 0x02

typedef struct _G3DUIACTION {
    uint32_t wait;
    uint32_t type;
    pthread_mutex_t done;
    G3DUI *gui;
} G3DUIACTION;

typedef struct _GOTOFRAME {
    G3DUIACTION action;
    float frame;
} GOTOFRAME;

typedef struct _DUMPSCREEN {
    G3DUIACTION action;
    uint32_t x, y, width, height;
    unsigned char (*buffer)[4];
} DUMPSCREEN;

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
GtkWidget *gtk_glossui_new ( );




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
GtkWidget *gtk_view_new     ( G3DCAMERA * );
GtkWidget *gtk_view_getGLArea ( GtkWidget *widget );
/******************************************************************************/
/******************************************************************************/


/******************************************************************************/
/************************** GTK MaterialList Widget ***************************/
typedef struct _GTK3MATERIALPREVIEW {
    GdkPixbuf *img;
    G3DMATERIAL *mat;
    G3DUIRECTANGLE rec;
} GTK3MATERIALPREVIEW;

GTK3MATERIALPREVIEW *gtk3materialpreview_new    ( G3DMATERIAL *,
                                                  G3DUIMATERIALMAP * );
void                 gtk3materialpreview_free   ( GTK3MATERIALPREVIEW * );
void                 gtk3materialpreview_update ( GTK3MATERIALPREVIEW *,
                                                  G3DUIMATERIALMAP * );



/******************************************************************************/
G3DUIGTK3 *g3duigtk3_new  ( GtkWidget * );
void       g3duigtk3_free ( G3DUIGTK3 * );

/******************************************************************************/
void   g3dui_init  ( G3DUI *, G3DSCENE * );
G3DUI *g3dui_new   ( G3DSCENE * );

/******************************************************************************/
GtkWidget *createModeBar          ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

GtkWidget *createToolBar          ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

GtkWidget *createMenuBar          ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

GtkWidget *createQuad             ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

GtkWidget *createView             ( GtkWidget *, G3DUI *, char *, G3DCAMERA *,
                                                                  gint, gint );

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

void       updateLightEdit        ( GtkWidget *, G3DUI * );
GtkWidget *createLightEdit        ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateMaterialEdit     ( GtkWidget *, G3DUI * );
GtkWidget *createMaterialEdit     ( GtkWidget *, G3DUI *, char *, gint, gint, 
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

void       updatePickToolEdit     ( GtkWidget *, G3DUI * );
GtkWidget *createPickToolEdit     ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateSculptToolEdit   ( GtkWidget *, G3DUI * );
GtkWidget *createSculptToolEdit   ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateRenderEdit       ( GtkWidget *, G3DUI * );
GtkWidget *createRenderEdit       ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateCurrentMouseTool ( GtkWidget *, G3DUI * );
GtkWidget *createCurrentMouseTool ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

void       updateCutMeshToolEdit  ( GtkWidget *, G3DUI * );
GtkWidget *createCutMeshToolEdit  ( GtkWidget *, G3DUI *, char *, gint, gint, 
                                                                  gint, gint );

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

void       createIntegerText     ( GtkWidget *, G3DUI *,
                                                char *,
                                                gint, gint,
                                                gint, gint,
                                                void (*)( GtkWidget *, 
                                                          gpointer ) );

void       createFloatText       ( GtkWidget *, G3DUI *,
                                                char *,
                                                gint, gint,
                                                gint, gint,
                                                void (*)( GtkWidget *, 
                                                          gpointer ) );

void       createCharText        ( GtkWidget *, G3DUI *,
                                                char *,
                                                gint, gint,
                                                gint, gint,
                                                void (*)( GtkWidget *, 
                                                          GdkEvent  *,
                                                          gpointer ) );

void createOrientationSelection  ( GtkWidget *, G3DUI *,
                                                char *,
                                                gint, gint,
                                                gint, gint,
                                                void (*)( GtkWidget *, 
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

uint32_t g3dui_cancelRenderByScene ( G3DUI *, R3DSCENE * );
uint32_t g3dui_cancelRenderByID    ( G3DUI *, uint64_t   );
void g3dui_redrawGLViews ( G3DUI * );
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

void gtk3_g3dui_setMode ( GtkWidget *, gpointer );
void g3dui_setMouseTool ( GtkWidget *, gpointer );
gboolean gtk3_showGL    ( GtkWidget *, cairo_t *, gpointer );
void gtk3_initGL        ( GtkWidget *, gpointer );
void gtk3_sizeGL        ( GtkWidget *, GdkRectangle *, gpointer );
void gtk3_inputGL       ( GtkWidget *, GdkEvent *, gpointer );

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

G3DUIRENDERPROCESS *g3dui_getRenderProcessByScene ( G3DUI *, R3DSCENE * );
G3DUIRENDERPROCESS *g3dui_getRenderProcessByID    ( G3DUI *, uint64_t   );

uint32_t g3dui_renderClean ( R3DFILTER *, R3DSCENE *, float,
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

GtkWidget *g3dui_addMenuSeparator ( GtkWidget * );
GtkWidget *g3dui_addMenuButton    ( GtkWidget *, G3DUI *,
                                                 const gchar *,
                                                 gint,
                                                 GCallback );

void g3duicom_handleAction ( GtkWidget *, gpointer, gpointer );
void g3duicom_gotoframe    ( G3DUI *, GOTOFRAME  * );
void g3duicom_dumpscreen   ( G3DUI *, DUMPSCREEN * );

/******************************************************************************/
#define TOGDKWINDOWFILTERNAME "TOGDKWINDOWFILTERNAME"
typedef struct _FILTERTOGDKWINDOW {
    cairo_surface_t *crsurf;
    cairo_t         *cr;
    GdkWindow       *gdkwin;
} FILTERTOGDKWINDOW;

R3DFILTER *r3dfilter_toGtkWidget_new ( GtkWidget *, uint32_t );
void       filtertogdkwindow_free ( FILTERTOGDKWINDOW * );
void       filtertogdkwindow_draw ( void *, R3DSCENE *, unsigned char *,
                                                        uint32_t, 
                                                        uint32_t, 
                                                        uint32_t, 
                                                        uint32_t );
FILTERTOGDKWINDOW *filtertogdkwindow_new ( GdkWindow * );

#endif
