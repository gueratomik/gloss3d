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
    GtkWidget *currentMouseToolButton; /*** store the current pressed toggle button      ***/
    GtkWidget *curMaterialWidget; /*** current material ***/
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
    G3DUIMODEBAR   core;
    GtkWidget     *bar;
    GtkWidget     *objectMode;
    GtkWidget     *vertexMode;
    GtkWidget     *edgeMode;
    GtkWidget     *faceMode;
    GtkWidget     *sculptMode;
    GtkWidget     *skinMode;
    GtkWidget     *uvmapMode;
    GtkWidget     *axisMode;
    GtkWidget     *pathMode;
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
    GtkWidget        *timeboard;
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

    GtkLayout          *layout;
    GtkWidget          *drawingArea;
    GtkWidget          *scrolledWindow;
    GtkWidget          *statusBar;
    GtkWidget          *topWin;
} GTK3G3DUIRENDERWINDOW;

/******************************************************************************/
typedef struct _GTK3G3DUIMATERIALBOARD {
    G3DUIMATERIALBOARD core;
    GtkWidget        *layout;
} GTK3G3DUIMATERIALBOARD;

/******************************************************************************/
typedef struct _GTK3G3DUIOBJECTLIST {
    G3DUIOBJECTLIST core;
    GtkWidget      *area;
    GtkWidget      *scrolled;
} GTK3G3DUIOBJECTLIST;

/******************************************************************************/
typedef struct _GTK3G3DUIMATERIALLIST {
    G3DUIMATERIALLIST core;
    GtkWidget        *area;
    GtkWidget        *scrolled;
} GTK3G3DUIMATERIALLIST;

/******************************************************************************/
typedef struct _GTK3G3DUIMATERIALEDIT {
    G3DUIMATERIALEDIT core;

    GtkNotebook      *scrolled;
    GtkNotebook      *notebook;

    /*GtkFixed         *diffusePanel;*/
    GtkToggleButton  *diffuseEnabledToggle;
    GtkToggleButton  *diffuseSolidToggle;
    GtkColorButton   *diffuseColorButton;
    GtkToggleButton  *diffuseImageToggle;
    GtkButton        *diffuseImageButton;
    GtkToggleButton  *diffuseProcToggle;
    GtkComboBoxText  *diffuseProcTypeCombo;
    GtkButton        *diffuseProcSettingsButton;

    /*GtkFixed         *specularPanel;*/
    GtkToggleButton  *specularEnabledToggle;
    GtkScale         *specularShininessScale;
    GtkScale         *specularLevelScale;
    GtkToggleButton  *specularSolidToggle;
    GtkColorButton   *specularColorButton;
    GtkToggleButton  *specularImageToggle;
    GtkButton        *specularImageButton;
    GtkToggleButton  *specularProcToggle;
    GtkComboBoxText  *specularProcTypeCombo;
    GtkButton        *specularProcSettingsButton;

    GtkSpinButton    *bumpStrengthEntry;
    GtkToggleButton  *bumpEnabledToggle;
    GtkScale         *bumpStrengthScale;
    GtkToggleButton  *bumpImageToggle;
    GtkButton        *bumpImageButton;
    GtkToggleButton  *bumpProcToggle;
    GtkComboBoxText  *bumpProcTypeCombo;
    GtkButton        *bumpProcSettingsButton;

    GtkSpinButton    *displacementStrengthEntry;
    GtkToggleButton  *displacementEnabledToggle;
    GtkScale         *displacementStrengthScale;
    GtkToggleButton  *displacementImageToggle;
    GtkButton        *displacementImageButton;
    GtkToggleButton  *displacementProcToggle;
    GtkComboBoxText  *displacementProcTypeCombo;
    GtkButton        *displacementProcSettingsButton;
} GTK3G3DUIMATERIALEDIT;

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
    G3DUIBOARD   core;
    GtkWidget   *layout;
    GtkNotebook *notebook;
} GTK3G3DUIBOARD;

/******************************************************************************/
typedef struct _GTK3G3DUIRENDEREDIT {
    G3DUIRENDEREDIT core;

    GtkNotebook     *fixed;
    GtkNotebook     *notebook;

    GtkToggleButton *renderPreviewToggle;
    GtkSpinButton   *fromFrameEntry;
    GtkSpinButton   *toFrameEntry;
    GtkSpinButton   *framerateEntry;

    GtkFixed        *generalTab;
    GtkFixed        *effectsTab;

    GtkSpinButton   *renderWidthEntry;
    GtkSpinButton   *renderHeightEntry;
    GtkSpinButton   *renderRatioEntry;

    GtkToggleButton *blurEnabledToggle;
    GtkToggleButton *blurVectorEnabledToggle;
    GtkToggleButton *blurSceneEnabledToggle;
    GtkSpinButton   *blurSamplesEntry;
    GtkSpinButton   *blurSubSamplingEntry;
    GtkSpinButton   *blurStrengthEntry;
    GtkSpinButton   *blurIterationsEntry;

    GtkToggleButton *backgroundColorToggle;
    GtkColorButton  *backgroundColorButton;
    GtkToggleButton *backgroundImageToggle;
    GtkButton       *backgroundImageButton;

    GtkToggleButton *outputEnabledToggle;
    GtkButton       *outputCodecButton;
    GtkEntry        *outputFileEntry;
    GtkComboBoxText *outputFormatSelector;

    GtkToggleButton *wireframeEnabledToggle;
    GtkToggleButton *wireframeAffectedToggle;
    GtkSpinButton   *wireframeThicknessEntry;
    GtkColorButton  *wireframeColorButton;

    GtkToggleButton *fogAffectsBackgroundToggle;
    GtkToggleButton *fogEnabledToggle;
    GtkSpinButton   *fogNearEntry;
    GtkSpinButton   *fogFarEntry;
    GtkColorButton  *fogColorButton;
    GtkSpinButton   *fogStrengthEntry;

    GtkToggleButton *texturingDisabledToggle;
    GtkWidget       *texturingColorButton;

    GtkToggleButton *aliasingEnabledToggle;
    GtkToggleButton *aliasingEdgeToggle;
    GtkToggleButton *aliasingFullToggle;
    GtkToggleButton *aliasingS5Toggle;
    GtkToggleButton *aliasingS9Toggle;
} GTK3G3DUIRENDEREDIT;

/******************************************************************************/
typedef struct _GTK3G3DUILIGHTEDIT {
    G3DUILIGHTEDIT core;

    GtkNotebook     *notebook;
    GtkColorButton  *diffuseColorButton;
    GtkColorButton  *specularColorButton;
    GtkSpinButton   *intensityEntry;
    GtkToggleButton *castShadowsToggle;
    GtkToggleButton *spotToggle;
    GtkSpinButton   *spotLengthEntry;
    GtkSpinButton   *spotAngleEntry;
    GtkSpinButton   *spotFadeAngleEntry; 
    GtkToggleButton *softShadowsToggle;
    GtkSpinButton   *shadowRadiusEntry;
    GtkSpinButton   *shadowSampleEntry;
} GTK3G3DUILIGHTEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUICUBEEDIT {
    G3DUICUBEEDIT core;

    GtkNotebook   *notebook;
    GtkSpinButton *xSlicesEntry;
    GtkSpinButton *ySlicesEntry;
    GtkSpinButton *zSlicesEntry;
    GtkSpinButton *radiusEntry;
} GTK3G3DUICUBEEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUIPLANEEDIT {
    G3DUIPLANEEDIT   core;

    GtkNotebook     *notebook;
    GtkSpinButton   *divUEntry;
    GtkSpinButton   *divVEntry;
    GtkSpinButton   *radiusUEntry;
    GtkSpinButton   *radiusVEntry;
    GtkComboBoxText *axisSelector;
} GTK3G3DUIPLANEEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUITUBEEDIT {
    G3DUITUBEEDIT core;

    GtkNotebook   *notebook;
    GtkSpinButton *capsEntry;
    GtkSpinButton *slicesEntry;
    GtkSpinButton *lengthEntry;
    GtkSpinButton *radiusEntry;
    GtkSpinButton *thicknessEntry;
} GTK3G3DUITUBEEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUITORUSEDIT {
    G3DUITORUSEDIT   core;

    GtkNotebook     *notebook;
    GtkSpinButton   *capsEntry;
    GtkSpinButton   *slicesEntry;
    GtkSpinButton   *extradEntry;
    GtkSpinButton   *intradEntry;
    GtkComboBoxText *axisSelector;
} GTK3G3DUITORUSEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUISPHEREEDIT {
    G3DUISPHEREEDIT  core;

    GtkNotebook     *notebook;
    GtkSpinButton   *capsEntry;
    GtkSpinButton   *slicesEntry;
    GtkSpinButton   *radiusEntry;
    GtkToggleButton *perfectToggle;
} GTK3G3DUISPHEREEDIT;

/******************************************************************************/
typedef struct GTK3G3DUIWIREFRAMEEDIT {
    G3DUIWIREFRAMEEDIT core;

    GtkNotebook       *notebook;
    GtkSpinButton     *thicknessEntry;
} GTK3G3DUIWIREFRAMEEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUICYLINDEREDIT {
    G3DUICYLINDEREDIT core;

    GtkNotebook      *notebook;
    GtkSpinButton    *capsEntry;
    GtkSpinButton    *slicesEntry;
    GtkSpinButton    *radiusEntry;
    GtkSpinButton    *lengthEntry;
} GTK3G3DUICYLINDEREDIT;

/******************************************************************************/
typedef struct _GTK3G3DUIFFDEDIT {
    G3DUIFFDEDIT core;

    GtkNotebook      *notebook;
    GtkSpinButton    *xSlicesEntry;
    GtkSpinButton    *ySlicesEntry;
    GtkSpinButton    *zSlicesEntry;
    GtkSpinButton    *xRadiusEntry;
    GtkSpinButton    *yRadiusEntry;
    GtkSpinButton    *zRadiusEntry;
} GTK3G3DUIFFDEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUISUBDIVIDEREDIT {
    G3DUISUBDIVIDEREDIT core;

    GtkNotebook        *notebook;
    GtkSpinButton      *previewEntry;
    GtkSpinButton      *renderEntry;
    GtkWidget          *syncToggle;

    GtkToggleButton    *sculptRadio;
    GtkToggleButton    *heightRadio;
} GTK3G3DUISUBDIVIDEREDIT;

/******************************************************************************/
typedef struct _GTK3G3DUIINSTANCEEDIT {
    G3DUIINSTANCEEDIT core;

    GtkNotebook     *notebook;
    GtkComboBoxText *objectSelector;
    GtkToggleButton *mirroredToggle;
    GtkComboBoxText *mirroredPlanSelector;
} GTK3G3DUIINSTANCEEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUIPARTICLEEMITTEREDIT {
    G3DUIPARTICLEEMITTEREDIT core;

    GtkNotebook             *notebook;                   

    GtkSpinButton           *startAtFrameEntry;          
    GtkSpinButton           *endAtFrameEntry;            
    GtkSpinButton           *radiusEntry;                
    GtkComboBoxText         *shapeCombo;                 
    GtkSpinButton           *lifetimeEntry;              
    GtkSpinButton           *ppfEntry;                   
    GtkSpinButton           *maxPreviewsEntry;           
    GtkToggleButton         *displayPartToggle;          

    GtkSpinButton           *initialAccelXEntry;         
    GtkSpinButton           *initialAccelYEntry;         
    GtkSpinButton           *initialAccelZEntry;         
    GtkSpinButton           *initialSpeedXEntry;         
    GtkSpinButton           *initialSpeedYEntry;         
    GtkSpinButton           *initialSpeedZEntry;         
    GtkSpinButton           *initialScaXEntry;           
    GtkSpinButton           *initialScaYEntry;           
    GtkSpinButton           *initialScaZEntry;           
    GtkSpinButton           *initialRotXEntry;           
    GtkSpinButton           *initialRotYEntry;           
    GtkSpinButton           *initialRotZEntry;           
    GtkSpinButton           *initialTranspEntry;         

    GtkSpinButton           *initialVarAngleEntry;       
    GtkSpinButton           *initialVarLifetimeEntry;    

    GtkSpinButton           *initialVarAccelXEntry;      
    GtkSpinButton           *initialVarAccelYEntry;      
    GtkSpinButton           *initialVarAccelZEntry;      
    GtkSpinButton           *initialVarSpeedXEntry;      
    GtkSpinButton           *initialVarSpeedYEntry;      
    GtkSpinButton           *initialVarSpeedZEntry;      
    GtkSpinButton           *initialVarScaXEntry;        
    GtkSpinButton           *initialVarScaYEntry;        
    GtkSpinButton           *initialVarScaZEntry;        
    GtkSpinButton           *initialVarRotXEntry;        
    GtkSpinButton           *initialVarRotYEntry;        
    GtkSpinButton           *initialVarRotZEntry;        
    GtkSpinButton           *initialVarTranspEntry;      

    GtkSpinButton           *finalAccelXEntry;           
    GtkSpinButton           *finalAccelYEntry;           
    GtkSpinButton           *finalAccelZEntry;           
    GtkSpinButton           *finalSpeedXEntry;           
    GtkSpinButton           *finalSpeedYEntry;           
    GtkSpinButton           *finalSpeedZEntry;           
    GtkSpinButton           *finalScaXEntry;             
    GtkSpinButton           *finalScaYEntry;             
    GtkSpinButton           *finalScaZEntry;             
    GtkSpinButton           *finalRotXEntry;             
    GtkSpinButton           *finalRotYEntry;             
    GtkSpinButton           *finalRotZEntry;             
    GtkSpinButton           *finalTranspEntry;           

    GtkSpinButton           *gravityForceXEntry;         
    GtkSpinButton           *gravityForceYEntry;         
    GtkSpinButton           *gravityForceZEntry;         
} GTK3G3DUIPARTICLEEMITTEREDIT;

/******************************************************************************/
typedef struct _GTK3G3DUITIMELINE {
    G3DUITIMELINE   core;

    GtkDrawingArea *area;
} GTK3G3DUITIMELINE;

/******************************************************************************/
typedef struct _GTK3G3DUITIMEBOARD {
    G3DUITIMEBOARD  core;

    GtkLayout       *layout;

    GtkFixed        *buttonsFixed;
    GtkButton       *prevButton;
    GtkButton       *nextButton;
    GtkButton       *playButton;
    GtkButton       *zoomButton;
    GtkButton       *unzoomButton;
    GtkButton       *recordButton;

    GtkFixed        *functionsFixed;
    GtkToggleButton *scaleButton;
    GtkToggleButton *panButton;
} GTK3G3DUITIMEBOARD;

/******************************************************************************/
/************************** GTK PatternList Widget ***************************/
typedef struct _GTK3PATTERNPREVIEW {
    GdkPixbuf     *img;
    M3DPATTERN    *pat;
    G3DUIRECTANGLE rec;
} GTK3PATTERNPREVIEW;

/******************************************************************************/
/************************** GTK MaterialList Widget ***************************/
typedef struct _GTK3G3DUIMATERIALPREVIEW {
    G3DUIMATERIALPREVIEW core;
    GdkPixbuf           *img;
} GTK3G3DUIMATERIALPREVIEW;

/******************************************************************************/
#define TOGDKWINDOWFILTERNAME "TOGDKWINDOWFILTERNAME"

typedef struct _FILTERTOGDKWINDOW {
    cairo_surface_t *crsurf;
    cairo_t         *cr;
    GdkWindow       *gdkwin;
} FILTERTOGDKWINDOW;

/******************************** css/default.c *******************************/
gchar *gtk3_getDefaultCSS ( );

/********************************* g3dui.c ************************************/

void g3duirectangle_toGdkRectangle ( G3DUIRECTANGLE *in, 
                                     GdkRectangle   *out  );
uint32_t gtk3_setMouseTool ( GTK3G3DUI *gtk3gui, char *name );
void gtk3_setHourGlass ( GTK3G3DUI *gtk3gui );
void gtk3_unsetHourGlass ( GTK3G3DUI *gtk3gui );
static void gtk3_updateAllCurrentMouseTools ( GTK3G3DUI *gtk3gui );
static void gtk3_updateKeyEdit ( GTK3G3DUI *gtk3gui );
void gtk3_initDefaultMouseTools ( GTK3G3DUI *gtk3gui );
void gtk3_interpretUIReturnFlags ( GTK3G3DUI *gtk3gui,
                                   uint64_t   msk );

void gtk3_newScene ( GTK3G3DUI *gtk3gui );
void gtk3_openFile ( GTK3G3DUI *gtk3gui );
void gtk3_saveAs ( GTK3G3DUI *gtk3gui );
void gtk3_saveFile ( GTK3G3DUI *gtk3gui );
void gtk3_renderView ( GTK3G3DUI *gtk3gui );
void gtk3_runRender ( GTK3G3DUI *gtk3gui );
uint64_t gtk3_loadImageForChannel ( GTK3G3DUI  *gtk3gui,
                                    uint32_t    channelID );

GtkFixed *ui_createTab ( GtkNotebook *parent, 
                         void        *data,
                         char        *name,
                         char        *class,
                         gint         x,
                         gint         y,
                         gint         width,
                         gint         height );

GtkScale *ui_createHorizontalScale ( GtkWidget *parent,
                                     void      *data,
                                     char      *name,
                                     char      *class,
                                     gint       x, 
                                     gint       y,
                                     gint       labwidth,
                                     gint       width,
                                     gint       height,
                                     float      min,
                                     float      max,
                                     float      step,
                                     void     (*cbk)( GtkWidget *, 
                                                      gpointer ) );
GtkToggleButton *ui_createToggleLabel ( GtkFixed *parent, 
                                        void     *data,
                                        char     *name,
                                        char     *class,
                                        gint      x, 
                                        gint      y,
                                        gint      labwidth,
                                        gint      btnwidth,
                                        gint      height,
                                        void    (*cbk)( GtkWidget *, 
                                                        gpointer ) );
GtkLabel *ui_createSimpleLabel ( GtkFixed *parent,
                                 void     *data,
                                 char     *name,
                                 char     *class,
                                 gint      x, 
                                 gint      y,
                                 gint      width,
                                 gint      height );
GtkColorButton *ui_createColorButton ( GtkFixed *fixed,
                                       void     *data,
                                       char     *name,
                                       char     *class,
                                       gint      x, 
                                       gint      y,
                                       gint      labwidth,
                                       gint      btnwidth,
                                       gint      btnheight,
                                       void    (*cbk)( GtkWidget *, 
                                                       gpointer ) );
GtkSpinButton *ui_createNumericText ( GtkFixed      *parent, 
                                      void          *data,
                                      GtkAdjustment *adj,
                                      char          *name,
                                      char          *class,
                                      gint           x, 
                                      gint           y,
                                      gint           labwidth,
                                      gint           txtwidth,
                                      gint           txtheight,
                                      void (*cbk)( GtkWidget *, 
                                                   gpointer ) );
GtkSpinButton *ui_createIntegerText ( GtkFixed *parent, 
                                      void     *data, 
                                      char     *name,
                                      char     *class,
                                      gdouble   min, 
                                      gdouble   max,
                                      gint      x, 
                                      gint      y,
                                      gint      labwidth,
                                      gint      txtwidth,
                                      gint      txtheight,
                                      void    (*cbk)( GtkWidget *, 
                                                      gpointer ) );
GtkSpinButton *ui_createFloatText ( GtkFixed *parent, 
                                    void      *data, 
                                    char      *name,
                                    char      *class,
                                    gdouble    min, 
                                    gdouble    max,
                                    gint       x, 
                                    gint       y,
                                    gint       labwidth,
                                    gint       txtwidth,
                                    gint       txtheight,
                                    void       (*cbk)( GtkWidget *, 
                                                       gpointer ) );
GtkComboBoxText *ui_createSelector ( GtkFixed *parent, 
                                     void      *data,
                                     char      *name,
                                     char      *class,
                                     gint       x, 
                                     gint       y,
                                     gint       labwidth,
                                     gint       txtwidth,
                                     gint       txtheight,
                                     void       (*cbk)( GtkWidget *, 
                                                        gpointer ) );
GtkComboBoxText *ui_createPlaneSelector ( GtkFixed *parent,
                                          void     *data, 
                                          char     *name,
                                          char     *class,
                                          gint     x,
                                          gint     y,
                                          gint     labwidth,
                                          gint     txtwidth,
                                          gint     txtheight,
                                          void   (*cbk)( GtkWidget *, 
                                                         gpointer ) );
GtkComboBoxText *ui_createAxisSelector ( GtkFixed *parent,
                                          void     *data, 
                                          char     *name,
                                          char     *class,
                                          gint     x,
                                          gint     y,
                                          gint     labwidth,
                                          gint     txtwidth,
                                          gint     txtheight,
                                          void   (*cbk)( GtkWidget *, 
                                                         gpointer ) );
GtkButton *ui_createImageButton ( GtkFixed    *parent,
                                  void        *data, 
                                  char        *name,
                                  char        *class,
                                  char       **xpm,
                                  gint         x,
                                  gint         y,
                                  gint         width,
                                  gint         height,
                                  void       (*cbk)( GtkWidget *, 
                                                     gpointer ) );
GtkButton *ui_createPushButton ( GtkFixed *parent,
                                 void     *data,
                                 char     *name,
                                 char     *class,
                                 gint      x, 
                                 gint      y,
                                 gint      width,
                                 gint      height,
                                 void    (*cbk)( GtkWidget *, 
                                                 gpointer ) );
GtkEntry *ui_createCharText ( GtkWidget *parent, 
                              void      *data,
                              char      *name,
                              char      *class,
                              gint       x, 
                              gint       y,
                              gint       labwidth,
                              gint       txtwidth,
                              gint       txtheight,
                              void     (*cbk)( GtkWidget *,
                                               GdkEvent  *, 
                                               gpointer ) );

GtkComboBoxText *ui_createProceduralSelector ( GtkFixed *parent,
                                               void     *data, 
                                               char     *name,
                                               char     *class,
                                               gint     x,
                                               gint     y,
                                               gint     labwidth,
                                               gint     txtwidth,
                                               gint     txtheight,
                                               void   (*cbk)( GtkWidget *, 
                                                              gpointer ) );

GtkCheckMenuItem *ui_gtk_check_menu_item_new_with_mnemonic ( char *class,
                                                             const gchar* label );
GtkFixed *ui_gtk_fixed_new ( char *class );
GtkLabel *ui_gtk_label_new ( char *class,
                              char *name );
GtkScale *ui_gtk_scale_new_with_range ( char *class,
                                        GtkOrientation orientation,
                                        double         min,
                                        double         max,
                                        double         step );
GtkEntry *ui_gtk_entry_new ( char *class );
GtkFrame *ui_gtk_frame_new ( char       *class,
                             const char *label );
GtkScrolledWindow *ui_gtk_scrolled_window_new ( char          *class,
                                                GtkAdjustment *hadjustment,
                                                GtkAdjustment *vadjustment );
GtkStatusbar *ui_gtk_statusbar_new ( char *class );
GtkNotebook *ui_gtk_notebook_new ( char *class );
GtkWindow *ui_gtk_window_new ( char *class, GtkWindowType type );
GtkMenu *ui_gtk_menu_new ( char *class );
GtkMenuBar *ui_gtk_menu_bar_new ( char *class );
GtkToolbar *ui_gtk_toolbar_new ( char *class );
GtkMenuItem *ui_gtk_menu_item_new_with_mnemonic ( char *class, 
                                                  const gchar* label );
GtkButton *ui_gtk_button_new ( char *class );
GtkButton *ui_gtk_button_new_with_label ( char *class, char *name );
GtkComboBoxText *ui_gtk_combo_box_text_new ( char *class );
GtkSpinButton *ui_gtk_spin_button_new ( char          *class,
                                    GtkAdjustment *adjustment,
                                    gdouble        climb_rate,
                                    guint          digits );
GtkCheckButton *ui_gtk_check_button_new ( char *class );
GtkLayout *ui_gtk_layout_new ( char          *class, 
                               GtkAdjustment *hadjustment,
                               GtkAdjustment *vadjustment );
GtkDrawingArea *ui_gtk_drawing_area_new ( char *class );

/******************************* g3duiboard.c *********************************/
GTK3G3DUIBOARD *gtk3_g3duiboard_create ( GtkWidget *parent,
                                         GTK3G3DUI *gtk3gui,
                                         char      *name );
void gtk3_g3duiboard_resize ( GTK3G3DUIBOARD *gtk3board,
                              uint32_t        width,
                              uint32_t        height );

/******************************** g3duicom.c **********************************/
Q3DFILTER *q3dfilter_gotoframe_new ( GTK3G3DUI *gtk3gui );
void gtk3_g3duicom_handleAction ( GtkWidget *widget, 
                             gpointer   ptr, 
                             gpointer   user_data );
void gtk3_g3duicom_handleAction ( GtkWidget *widget, 
                                  gpointer   ptr, 
                                  gpointer   user_data );
void gtk3_g3duicom_requestActionFromMainThread ( GTK3G3DUI   *gtk3gui,
                                                 G3DUIACTION *action );

/******************************* g3duicubeedit.c ******************************/
void gtk3_g3duicubeedit_update ( GTK3G3DUICUBEEDIT *gtk3ced );
GTK3G3DUICUBEEDIT *gtk3_g3duicubeedit_create ( GtkWidget *parent,
                                               GTK3G3DUI *gtk3gui,
                                               char      *name );

/**************************** g3duicylinderedit.c *****************************/
void gtk3_g3duicylinderedit_update ( GTK3G3DUICYLINDEREDIT *gtk3ced );
GTK3G3DUICYLINDEREDIT *gtk3_g3duicylinderedit_create ( GtkWidget *parent,
                                                       GTK3G3DUI *gtk3gui,
                                                       char      *name );

/****************************** g3duiffdedit.c ********************************/
void gtk3_g3duiffdedit_update ( GTK3G3DUIFFDEDIT *gtk3fed );
GTK3G3DUIFFDEDIT *gtk3_g3duiffdedit_create ( GtkWidget *parent,
                                             GTK3G3DUI *gtk3gui,
                                             char      *name );

/***************************** g3duiinstanceedit.c ****************************/
void gtk3_g3duiinstanceedit_update ( GTK3G3DUIINSTANCEEDIT *gtk3ied );
GTK3G3DUIINSTANCEEDIT *gtk3_g3duiinstanceedit_create ( GtkWidget *parent,
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

/**************************** g3duimaterialboard.c ****************************/
GTK3G3DUIMATERIALBOARD *gtk3_g3duimaterialboard_create ( GtkWidget *parent,
                                                         GTK3G3DUI *gtk3gui,
                                                         char      *name );
void gtk3_g3duimaterialboard_resize ( GTK3G3DUIMATERIALBOARD *gtk3matboard,
                                      uint32_t                width,
                                      uint32_t                height );

/**************************** g3duimaterialedit.c *****************************/
void gtk3_g3duimaterialedit_update ( GTK3G3DUIMATERIALEDIT *gtk3med );
GTK3G3DUIMATERIALEDIT *gtk3_g3duimaterialedit_create ( GtkWidget *parent,
                                                       GTK3G3DUI *gtk3gui,
                                                       char      *name );

/**************************** g3duimateriallist.c *****************************/
GTK3G3DUIMAIN *gtk3_g3duimain_create ( GtkWidget *parent,
                                       GTK3G3DUI *gtk3gui,
                                       char      *name,
                                       gint       x,
                                       gint       y,
                                       gint       width,
                                       gint       height,
                                       char      *filename );
void gtk3_g3duimain_updateMenuBar ( GTK3G3DUIMAIN *gtk3main );
void gtk3_g3duimain_resize ( GTK3G3DUIMAIN *gtk3main,
                             uint32_t       width,
                             uint32_t       height );
void gtk3_g3duimateriallist_update ( GTK3G3DUIMATERIALLIST *gtk3matlist );
void gtk3_g3duimateriallist_updatePreview ( GTK3G3DUIMATERIALLIST *gtk3matlist,
                                            G3DMATERIAL           *mat );
void gtk3_g3duimateriallist_addMaterial ( GTK3G3DUIMATERIALLIST *gtk3matlist,
                                          G3DMATERIAL           *mat );
void gtk3_g3duimateriallist_removeMaterial ( GTK3G3DUIMATERIALLIST *gtk3matlist, 
                                             G3DSCENE              *sce,
                                             G3DURMANAGER          *urm,
                                             G3DMATERIAL           *mat );
GTK3G3DUIMATERIALLIST *gtk3_g3duimateriallist_create ( GtkWidget *parent,
                                                       GTK3G3DUI *gtk3gui,
                                                       char      *name );

/**************************** g3duiobjectlist.c *******************************/
GTK3G3DUIOBJECTLIST *gtk3_g3duiobjectlist_create ( GtkWidget *parent,
                                                   GTK3G3DUI *gtk3gui,
                                                   char      *name );

/************************ g3duiparticleemitter.c ******************************/
void gtk3_g3duiparticleemitteredit_update ( GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped );
GTK3G3DUIPARTICLEEMITTEREDIT *gtk3_g3duiparticleemitteredit_create ( GtkWidget *parent,
                                                                     GTK3G3DUI *gtk3gui,
                                                                     char      *name );
/**************************** g3duiplaneedit.c ********************************/
void gtk3_g3duiplaneedit_update ( GTK3G3DUIPLANEEDIT *gtk3ped );
GTK3G3DUIPLANEEDIT *gtk3_g3duiplaneedit_create ( GtkWidget *parent,
                                               GTK3G3DUI *gtk3gui,
                                               char      *name );

/***************************** g3duirenderedit.c ******************************/
GTK3G3DUIRENDEREDIT* gtk3_g3duirenderedit_create ( GtkWidget   *parent, 
                                                   GTK3G3DUI   *gtk3gui,
                                                   char        *name,
                                                   Q3DSETTINGS *rsg );

/**************************** g3duirenderwindow.c *****************************/
GTK3G3DUIRENDERWINDOW *gtk3_g3duirenderwindow_create ( GtkWindow *parent, 
                                                       GTK3G3DUI *gtk3gui,
                                                       char      *name );
void gtk3_g3duirenderwindow_resize ( GTK3G3DUIRENDERWINDOW *gtk3rwin,
                                     uint32_t               width,
                                     uint32_t               height );
void gtk3_g3duirenderwindow_updateMenuBar ( GTK3G3DUIRENDERWINDOW *gtk3rwin );

/*************************** g3duisubdivideredit.c ****************************/
void gtk3_g3duisubdivideredit_update ( GTK3G3DUISUBDIVIDEREDIT *gtk3sed );
GTK3G3DUISUBDIVIDEREDIT *gtk3_g3duisubdivideredit_create ( GtkWidget *parent,
                                                           GTK3G3DUI *gtk3gui,
                                                           char      *name );
/***************************** g3duitimeline.c ********************************/
GTK3G3DUITIMELINE *gtk3_g3duitimeline_create ( GtkWidget *parent,
                                               GTK3G3DUI *gtk3gui,
                                               char      *name );

/***************************** g3duitimeboard.c *******************************/
void gtk3_g3duitimeboard_update ( GTK3G3DUITIMEBOARD *gtk3timeboard );
void gtk3_g3duitimeboard_resize ( GTK3G3DUITIMEBOARD *gtk3timeboard,
                                  uint32_t              width,
                                  uint32_t              height );
GTK3G3DUITIMEBOARD *gtk3_g3duitimeboard_create ( GtkWidget *parent,
                                                 GTK3G3DUI *gtk3gui,
                                                 char      *name );

/***************************** g3duitoolbar.c *********************************/
GTK3G3DUITOOLBAR *gtk3_g3duitoolbar_create ( GtkWidget *parent,
                                             GTK3G3DUI *gtk3gui,
                                             char      *name );

/***************************** g3duitorusedit.c *******************************/
void gtk3_g3duitorusedit_update ( GTK3G3DUITORUSEDIT *gtk3ted );
GTK3G3DUITORUSEDIT *gtk3_g3duitorusedit_create ( GtkWidget *parent,
                                                 GTK3G3DUI *gtk3gui,
                                                 char      *name );

/***************************** g3duitubeedit.c ********************************/
void gtk3_g3duitubeedit_update ( GTK3G3DUITUBEEDIT *gtk3ted );
GTK3G3DUITUBEEDIT *gtk3_g3duitubeedit_create ( GtkWidget *parent,
                                               GTK3G3DUI *gtk3gui,
                                               char      *name );

/************************** g3duisphereeedit.c ********************************/
void gtk3_g3duisphereedit_update ( GTK3G3DUISPHEREEDIT *gtk3sed );
GTK3G3DUISPHEREEDIT *gtk3_g3duisphereedit_create ( GtkWidget *parent,
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
void gtk3_g3duiobjectboard_resize ( GTK3G3DUIOBJECTBOARD *gtk3objboard,
                                    uint32_t              width,
                                    uint32_t              height );

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
void gtk3_g3duiquad_resize ( GTK3G3DUIQUAD *gtk3quad,
                             uint32_t       width,
                             uint32_t       height );

/****************************** g3duiview.c ***********************************/
GTK3G3DUIVIEW *gtk3_g3duiview_create ( GtkWidget *parent, 
                                       GTK3G3DUI *gtk3gui,
                                       char      *name );
void gtk3_g3duiview_updateMenuBar ( GTK3G3DUIVIEW *gtk3view );
void gtk3_g3duiview_resize ( GTK3G3DUIVIEW *gtk3view,
                             uint32_t       width,
                             uint32_t       height );

/**************************** g3duiwireframeedit.c ****************************/
void gtk3_g3duiwireframeedit_update ( GTK3G3DUIWIREFRAMEEDIT *gtk3wed );
GTK3G3DUIWIREFRAMEEDIT *gtk3_g3duiwireframeedit_create ( GtkWidget *parent,
                                                         GTK3G3DUI *gtk3gui,
                                                         char      *name );

/********************************* q3dfilters.c *******************************/
uint32_t q3dfilter_toStatusBar_getStatus ( Q3DFILTER *fil );
Q3DFILTER *q3dfilter_toStatusBar_new ( GtkWidget *widget, 
                                       float      lastFrame );

#endif
