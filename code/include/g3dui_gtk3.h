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
typedef struct _GTK3M3DUIVIEW {
    M3DUIVIEW  core;
    GtkWidget *layout;
    GtkWidget *navbar;
    GtkWidget *glarea;
    GdkPixbuf *pix[0x04];
} GTK3M3DUIVIEW;

/******************************************************************************/
typedef struct _GTK3M3DUITOOLBAR {
    M3DUITOOLBAR         core;

    GtkToolbar           *bar;

    GtkToolButton       *createImageButton;
    GtkToolButton       *openFileButton;
    GtkToolButton       *saveAsButton;
    GtkToolButton       *saveButton;
    GtkToolButton       *saveImageAsButton;
    GtkToolButton       *saveImageButton;
    GtkToolButton       *undoButton;
    GtkToolButton       *redoButton;
    GtkToggleToolButton *pickUVToggle;
    GtkToggleToolButton *moveUVToggle;
    GtkToggleToolButton *scaleUVToggle;
    GtkToggleToolButton *rotateUVToggle;
    GtkToggleToolButton *selectSquareToggle;
    GtkToggleToolButton *selectRandomToggle;
    GtkToolButton       *unselectButton;
    GtkToggleToolButton *penToggle;
    GtkToggleToolButton *bucketToggle;
    GtkToggleToolButton *eraserToggle;
} GTK3M3DUITOOLBAR;

/******************************************************************************/
typedef struct _GTK3M3DUIMODEBAR {
    M3DUIMODEBAR        core;

    GtkToolbar         *bar;

    GtkRadioToolButton *vertexMode;
    GtkRadioToolButton *faceMode;
} GTK3M3DUIMODEBAR;

/******************************************************************************/
typedef struct _GTK3M3DUICHANNELIMAGECREATOR {
    M3DUICHANNELIMAGECREATOR core;
    GtkFixed                *fixed;
    GtkSpinButton           *widthEntry;
    GtkSpinButton           *heightEntry;
    GtkLabel                *infoLabel;
} GTK3M3DUICHANNELIMAGECREATOR;

/******************************************************************************/
typedef struct _GTK3M3DUIBOARD {
    M3DUIBOARD core;
    GtkWidget *layout;
} GTK3M3DUIBOARD;

/******************************************************************************/
typedef struct _GTK3M3DUI {
    M3DUI      core;
    GtkWindow *topWin;
    GtkWidget *currentMouseToolButton;
} GTK3M3DUI;

/******************************************************************************/
typedef struct _GTK3M3DUIPATTERNPREVIEW {
    M3DUIPATTERNPREVIEW core;
    GdkPixbuf          *img;
} GTK3M3DUIPATTERNPREVIEW;

/******************************************************************************/
typedef struct _GTK3M3DUIPATTERNLIST {
    M3DUIPATTERNLIST core;
    GtkDrawingArea  *area;
    GtkScrolledWindow *scrolled;
} GTK3M3DUIPATTERNLIST;

/******************************************************************************/
typedef struct _GTK3M3DUICOLORPICKER {
    M3DUICOLORPICKER core;
    GtkDrawingArea  *area;
} GTK3M3DUICOLORPICKER;

/******************************************************************************/
typedef struct _GTK3M3DUIBUCKETTOOLEDIT {
    M3DUIBUCKETTOOLEDIT core;
    GtkFixed           *fixed;
    GtkSpinButton      *toleranceEntry;
} GTK3M3DUIBUCKETTOOLEDIT;

/******************************************************************************/
typedef struct _GTK3M3DUIPENTOOLEDIT {
    M3DUIPENTOOLEDIT core;
    GtkFixed        *fixed;
    GtkScale        *pressureScale;
    GtkSpinButton   *radiusEntry;
    GtkCheckButton  *incrementalToggle;
} GTK3M3DUIPENTOOLEDIT;

/******************************************************************************/
typedef struct _GTK3M3DUIMOUSETOOLEDIT {
    M3DUIMOUSETOOLEDIT core;
    GtkScrolledWindow *scrolled;
    GtkFixed          *mousetoolFixed;
} GTK3M3DUIMOUSETOOLEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUI {
    G3DUI           core;
    LIST          *lmtools; /*** list of mousetools widget ***/

    GdkWindow     *winAtPosition; /*** window at mouse position (for hourGlass)***/

    GtkWindow     *topWin;
    GtkWidget     *currentMouseToolButton; /*** store the current pressed toggle button      ***/
    GtkWidget     *curMaterialWidget; /*** current material ***/
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
    GtkWidget *invisible;
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
} GTK3G3DUIMAIN;

/******************************************************************************/
typedef struct _GTK3M3DUIMAIN {
    M3DUIMAIN         core;
    GtkLayout        *layout;
} GTK3M3DUIMAIN;

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

    GtkFixed          *fixed;
    GtkEntry          *nameEntry;
    GtkScrolledWindow *scrolled;
    GtkNotebook       *notebook;

    /*GtkFixed          *diffusePanel;*/
    GtkCheckButton    *diffuseEnabledToggle;
    GtkCheckButton    *diffuseSolidToggle;
    GtkColorButton    *diffuseColorButton;
    GtkCheckButton    *diffuseImageToggle;
    GtkButton         *diffuseImageButton;
    GtkCheckButton    *diffuseProcToggle;
    GtkComboBoxText   *diffuseProcTypeCombo;
    GtkButton         *diffuseProcSettingsButton;

    /*GtkFixed          *specularPanel;*/
    GtkCheckButton    *specularEnabledToggle;
    GtkScale          *specularShininessScale;
    GtkScale          *specularLevelScale;
    GtkCheckButton    *specularSolidToggle;
    GtkColorButton    *specularColorButton;
    GtkCheckButton    *specularImageToggle;
    GtkButton         *specularImageButton;
    GtkCheckButton    *specularProcToggle;
    GtkComboBoxText   *specularProcTypeCombo;
    GtkButton         *specularProcSettingsButton;

    GtkSpinButton     *bumpStrengthEntry;
    GtkCheckButton    *bumpEnabledToggle;
    GtkScale          *bumpStrengthScale;
    GtkCheckButton    *bumpImageToggle;
    GtkButton         *bumpImageButton;
    GtkCheckButton    *bumpProcToggle;
    GtkComboBoxText   *bumpProcTypeCombo;
    GtkButton         *bumpProcSettingsButton;

    GtkSpinButton     *displacementStrengthEntry;
    GtkCheckButton    *displacementEnabledToggle;
    GtkScale          *displacementStrengthScale;
    GtkCheckButton    *displacementImageToggle;
    GtkButton         *displacementImageButton;
    GtkCheckButton    *displacementProcToggle;
    GtkComboBoxText   *displacementProcTypeCombo;
    GtkButton         *displacementProcSettingsButton;

    GtkSpinButton     *alphaStrengthEntry;
    GtkCheckButton   *alphaEnabledToggle;
    GtkScale          *alphaStrengthScale;
    GtkCheckButton   *alphaImageToggle;
    GtkButton         *alphaImageButton;
    GtkCheckButton   *alphaProcToggle;
    GtkComboBoxText   *alphaProcTypeCombo;
    GtkButton         *alphaProcSettingsButton;

    GtkCheckButton   *reflectionEnabledToggle;
    GtkCheckButton   *reflectionSolidToggle;
    GtkCheckButton   *reflectionImageToggle;
    GtkButton         *reflectionImageButton;
    GtkCheckButton   *reflectionProcToggle;
    GtkComboBoxText   *reflectionProcTypeCombo;
    GtkButton         *reflectionProcSettingsButton;
    GtkScale          *reflectionStrengthScale;

    GtkCheckButton    *refractionEnabledToggle;
    GtkCheckButton    *refractionSolidToggle;
    GtkCheckButton    *refractionImageToggle;
    GtkButton         *refractionImageButton;
    GtkCheckButton    *refractionProcToggle;
    GtkComboBoxText   *refractionProcTypeCombo;
    GtkButton         *refractionProcSettingsButton;
    GtkScale          *refractionStrengthScale;
} GTK3G3DUIMATERIALEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUIOBJECTEDIT {
    G3DUIOBJECTEDIT    core;
    GtkFixed          *fixed;
    GtkEntry          *nameEntry;
    GtkScrolledWindow *scrolled;
    GtkFixed          *objectFixed;
} GTK3G3DUIOBJECTEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUICOORDINATESEDIT {
    G3DUICOORDINATESEDIT core;
    GtkFixed            *fixed;
    GtkSpinButton       *xTranslationEntry;
    GtkSpinButton       *yTranslationEntry;
    GtkSpinButton       *zTranslationEntry;
    GtkSpinButton       *xRotationEntry;
    GtkSpinButton       *yRotationEntry;
    GtkSpinButton       *zRotationEntry;
    GtkSpinButton       *xScalingEntry;
    GtkSpinButton       *yScalingEntry;
    GtkSpinButton       *zScalingEntry;
    GtkToggleButton     *absoluteToggle;
} GTK3G3DUICOORDINATESEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUIOBJECTBOARD {
    G3DUIOBJECTBOARD core;
    GtkWidget       *layout;
} GTK3G3DUIOBJECTBOARD;

/******************************************************************************/
typedef struct _GTK3G3DUIBOARD {
    G3DUIBOARD   core;
    GtkLayout   *layout;
    GtkNotebook *notebook;
    GtkNotebook *toolbook;
} GTK3G3DUIBOARD;

/******************************************************************************/
typedef struct _GTK3G3DUIRENDEREDIT {
    G3DUIRENDEREDIT core;

    GtkNotebook     *fixed;
    GtkNotebook     *notebook;

    GtkCheckButton  *renderPreviewToggle;
    GtkSpinButton   *fromFrameEntry;
    GtkSpinButton   *toFrameEntry;
    GtkSpinButton   *framerateEntry;

    GtkFixed        *generalTab;
    GtkFixed        *effectsTab;

    GtkSpinButton   *renderWidthEntry;
    GtkSpinButton   *renderHeightEntry;
    GtkSpinButton   *renderRatioEntry;

    GtkCheckButton  *blurEnabledToggle;
    GtkCheckButton  *blurVectorEnabledToggle;
    GtkCheckButton  *blurSceneEnabledToggle;
    GtkSpinButton   *blurSamplesEntry;
    GtkSpinButton   *blurSubSamplingEntry;
    GtkSpinButton   *blurStrengthEntry;
    GtkSpinButton   *blurIterationsEntry;

    GtkCheckButton  *backgroundColorToggle;
    GtkColorButton  *backgroundColorButton;
    GtkCheckButton  *backgroundImageToggle;
    GtkButton       *backgroundImageButton;

    GtkCheckButton  *outputEnabledToggle;
    GtkButton       *outputCodecButton;
    GtkEntry        *outputFileEntry;
    GtkComboBoxText *outputFormatSelector;

    GtkCheckButton  *wireframeEnabledToggle;
    GtkCheckButton  *wireframeAffectedToggle;
    GtkSpinButton   *wireframeThicknessEntry;
    GtkColorButton  *wireframeColorButton;

    GtkCheckButton  *fogAffectsBackgroundToggle;
    GtkCheckButton  *fogEnabledToggle;
    GtkSpinButton   *fogNearEntry;
    GtkSpinButton   *fogFarEntry;
    GtkColorButton  *fogColorButton;
    GtkSpinButton   *fogStrengthEntry;

    GtkCheckButton  *texturingDisabledToggle;
    GtkWidget       *texturingColorButton;

    GtkCheckButton  *aliasingEnabledToggle;
    GtkCheckButton  *aliasingEdgeToggle;
    GtkCheckButton  *aliasingFullToggle;
    GtkCheckButton  *aliasingS5Toggle;
    GtkCheckButton  *aliasingS9Toggle;
} GTK3G3DUIRENDEREDIT;

/******************************************************************************/
typedef struct _GTK3G3DUILIGHTEDIT {
    G3DUILIGHTEDIT core;

    GtkNotebook     *notebook;
    GtkColorButton  *diffuseColorButton;
    GtkColorButton  *specularColorButton;
    GtkSpinButton   *intensityEntry;
    GtkCheckButton  *castShadowsToggle;
    GtkCheckButton  *spotToggle;
    GtkSpinButton   *spotLengthEntry;
    GtkSpinButton   *spotAngleEntry;
    GtkSpinButton   *spotFadeAngleEntry; 
    GtkCheckButton  *softShadowsToggle;
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
typedef struct _GTK3G3DUIWIREFRAMEEDIT {
    G3DUIWIREFRAMEEDIT core;

    GtkNotebook       *notebook;
    GtkSpinButton     *thicknessEntry;
} GTK3G3DUIWIREFRAMEEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUIMORPHERKEYEDIT {
    G3DUIMORPHERKEYEDIT core;

    GtkFixed           *fixed;
    GtkFixed           *poseList;
} GTK3G3DUIMORPHERKEYEDIT;

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
    GtkCheckButton          *displayPartToggle;          

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
    GtkCheckButton  *scaleButton;
    GtkCheckButton  *panButton;
} GTK3G3DUITIMEBOARD;

/******************************************************************************/
typedef struct _GTK3G3DUIPROCEDURALBRICKEDIT {
    G3DUIPROCEDURALBRICKEDIT core;
    GdkPixbuf               *pix;
    GtkFixed                *fixed;
    GtkDrawingArea          *drawingArea;
    GtkColorButton          *brickColorButton;
    GtkColorButton          *spacingColorButton;
    GtkSpinButton           *nbLinesEntry;
    GtkSpinButton           *nbBricksPerLineEntry;
    GtkSpinButton           *uSpacingEntry;
    GtkSpinButton           *vSpacingEntry;
} GTK3G3DUIPROCEDURALBRICKEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUIPROCEDURALCHESSEDIT {
    G3DUIPROCEDURALCHESSEDIT core;
    GdkPixbuf               *pix;
    GtkFixed                *fixed;
    GtkDrawingArea          *drawingArea;
    GtkColorButton          *color1Button;
    GtkColorButton          *color2Button;
    GtkSpinButton           *nbBricksPerLineEntry;
    GtkSpinButton           *uDivEntry;
    GtkSpinButton           *vDivEntry;
} GTK3G3DUIPROCEDURALCHESSEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUIPROCEDURALGRADIENTEDIT {
    G3DUIPROCEDURALGRADIENTEDIT core;
    GdkPixbuf                  *pix;
    GtkFixed                   *fixed;
    GtkDrawingArea             *drawingArea;
    GtkColorButton             *color1Button;
    GtkColorButton             *color2Button;
    GtkCheckButton             *horizontalToggle;
    GtkCheckButton             *verticalToggle;
} GTK3G3DUIPROCEDURALGRADIENTEDIT;

typedef struct _GTK3G3DUIPROCEDURALNOISEEDIT {
    G3DUIPROCEDURALNOISEEDIT core;
    GdkPixbuf               *pix;
    GtkFixed                *fixed;
    GtkDrawingArea          *drawingArea;
    GtkSpinButton           *uGradientsEntry;
    GtkSpinButton           *vGradientsEntry;
    GtkSpinButton           *octavesEntry;
    GtkSpinButton           *nbpairsEntry;
    struct GTK3G3DUIPROCEDURALNOISEPAIR {
        GtkFixed       *fixed;
        GtkSpinButton  *thresholdEntry;
        GtkColorButton *color1Button;
        GtkColorButton *color2Button;
    } pair[MAXNOISECOLORS];
} GTK3G3DUIPROCEDURALNOISEEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUITEXTUREEDIT {
    G3DUITEXTUREEDIT core;
    GtkFixed        *fixed;
    GtkFixed        *facegroupFrame;
    GtkFixed        *facegroupFixed;
    GtkCheckButton  *restrictToggle;
    GtkCheckButton  *repeatToggle;
    GtkButton       *editButton;
    GtkComboBoxText *uvmapSelector;
} GTK3G3DUITEXTUREEDIT ;

/******************************************************************************/
typedef struct _GTK3G3DUICAMERAEDIT {
    G3DUICAMERAEDIT  core;
    GtkNotebook     *notebook;
    GtkSpinButton   *focalEntry;
    GtkCheckButton  *dofEnableToggle;
    GtkSpinButton   *dofNearBlurEntry;
    GtkSpinButton   *dofNoBlurEntry;
    GtkSpinButton   *dofFarBlurEntry;
    GtkSpinButton   *dofRadiusEntry;
} GTK3G3DUICAMERAEDIT ;

/******************************************************************************/
typedef struct _GTK3G3DUITEXTEDIT {
    G3DUITEXTEDIT  core;
    GtkNotebook     *notebook;
    GtkButton       *fontFaceButton;
    GtkSpinButton   *fontSizeEntry;
    GtkSpinButton   *fontRoundnessEntry;
    GtkSpinButton   *fontThicknessEntry;
    GtkTextView     *textText;
} GTK3G3DUITEXTEDIT ;

/******************************************************************************/
typedef struct _GTK3G3DUIUVMAPEDIT {
    G3DUIUVMAPEDIT   core;
    GtkFixed        *fixed;
    GtkEntry        *nameEntry;
    GtkCheckButton  *fixedToggle;
    GtkComboBoxText *projectionCombo;
} GTK3G3DUIUVMAPEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUISYMMETRYEDIT {
    G3DUISYMMETRYEDIT core;
    GtkNotebook      *notebook;    
    GtkSpinButton    *limitEntry;
    GtkComboBoxText  *orientationCombo;
} GTK3G3DUISYMMETRYEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUISPLINEREVOLVEREDIT {
    G3DUISPLINEREVOLVEREDIT core;
    GtkNotebook            *notebook;    
    GtkSpinButton          *stepsEntry;
    GtkSpinButton          *divisionsEntry;
} GTK3G3DUISPLINEREVOLVEREDIT;

/******************************************************************************/
typedef struct _GTK3G3DUITRACKERTAGEDIT {
    G3DUITRACKERTAGEDIT core;
    GtkFixed           *fixed;
    GtkCheckButton     *objectCombo;
    GtkComboBoxText    *orientationCombo;
} GTK3G3DUITRACKERTAGEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUIPICKTOOLEDIT {
    G3DUIPICKTOOLEDIT core;
    GtkFixed         *fixed;
    GtkWidget        *visibleToggle;
    GtkWidget        *weightStrengthScale;
    GtkWidget        *weightRadiusEntry;
    GtkWidget        *weightFrame;
} GTK3G3DUIPICKTOOLEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUIMOVETOOLEDIT {
    G3DUIPICKTOOLEDIT core;
    GtkFixed         *fixed;
    GtkWidget        *visibleToggle;
} GTK3G3DUIMOVETOOLEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUISCULPTTOOLEDIT {
    G3DUISCULPTTOOLEDIT core;
    GtkFixed           *fixed;
    GtkCheckButton    *visibleToggle;
    GtkScale           *radiusScale;
    GtkScale           *pressureScale;
    GtkCheckButton     *circularToggle;
    GtkCheckButton     *squaredToggle;
} GTK3G3DUISCULPTTOOLEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUICUTMESHTOOLEDIT {
    G3DUICUTMESHTOOLEDIT core;
    GtkFixed           *fixed;
    GtkWidget          *restrictToggle;
} GTK3G3DUICUTMESHTOOLEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUIMOUSETOOLEDIT {
    G3DUIMOUSETOOLEDIT  core;
    GtkScrolledWindow  *scrolled;
    GtkFixed           *mousetoolFixed;
} GTK3G3DUIMOUSETOOLEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUIBONEEDIT {
    G3DUIBONEEDIT  core;
    GtkNotebook   *notebook;
    GtkSpinButton *lengthEntry;
    GtkFixed      *weightgroupFixed;
} GTK3G3DUIBONEEDIT ;

/******************************************************************************/
typedef struct _GTK3G3DUIMESHEDIT {
    G3DUIMESHEDIT    core;
    GtkNotebook     *notebook;
    GtkCheckButton  *shadingToggle;
    GtkSpinButton   *gouraudEntry;
    GtkEntry        *fgNameEntry;
    GtkFixed        *fgListFixed;
    GtkFixed        *fgPanelFixed;

    GtkEntry        *wgNameEntry;
    GtkFixed        *wgListFixed;
    GtkFixed        *wgPanelFixed;
} GTK3G3DUIMESHEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUIKEYEDIT {
    G3DUIKEYEDIT    core;
    GtkFixed        *fixed;
    GtkCheckButton  *translationToggle;
    GtkCheckButton  *rotationToggle;
    GtkCheckButton  *scalingToggle;
    GtkSpinButton   *XTranslationEntry;
    GtkSpinButton   *YTranslationEntry;
    GtkSpinButton   *ZTranslationEntry;
    GtkSpinButton   *XRotationEntry;
    GtkSpinButton   *YRotationEntry;
    GtkSpinButton   *ZRotationEntry;
    GtkSpinButton   *XScalingEntry;
    GtkSpinButton   *YScalingEntry;
    GtkSpinButton   *ZScalingEntry;
    GtkCheckButton  *loopToggle;
    GtkSpinButton   *loopFrameEntry;
    GtkCheckButton  *keyDataToggle;
    GtkButton       *keyDataButton;
} GTK3G3DUIKEYEDIT;

/******************************************************************************/
typedef struct _GTK3G3DUIMORPHEREDIT {
    G3DUIMORPHEREDIT core;
    GtkNotebook     *notebook;
    GtkEntry        *poseNameEntry;
    GtkFixed        *poseListFixed;
    GtkFixed        *posePanelFixed;
} GTK3G3DUIMORPHEREDIT;

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
gchar *gtk3_getDarkModeCSS();
gchar *gtk3_getLightModeCSS();

/********************************* g3dui.c ************************************/

void g3duirectangle_toGdkRectangle ( G3DUIRECTANGLE *in, 
                                     GdkRectangle   *out  );
uint64_t gtk3_setMouseTool ( GTK3G3DUI *gtk3gui, 
                             GtkWidget *button,
                             char      *toolName );
void gtk3_setHourGlass ( GTK3G3DUI *gtk3gui );
void gtk3_unsetHourGlass ( GTK3G3DUI *gtk3gui );
static void gtk3_updateAllCurrentMouseTools ( GTK3G3DUI *gtk3gui );
static void gtk3_updateKeyEdit ( GTK3G3DUI *gtk3gui );
void gtk3_initDefaultMouseTools ( GTK3G3DUI *gtk3gui );
void gtk3_interpretUIReturnFlags ( GTK3G3DUI *gtk3gui,
                                   uint64_t   msk );

uint64_t gtk3_newScene ( GTK3G3DUI *gtk3gui );
uint64_t gtk3_openFile ( GTK3G3DUI *gtk3gui );
uint64_t gtk3_saveAs ( GTK3G3DUI *gtk3gui );
uint64_t gtk3_saveFile ( GTK3G3DUI *gtk3gui );
uint64_t gtk3_mergeFile ( GTK3G3DUI *gtk3gui );
uint64_t gtk3_exportFile ( GTK3G3DUI   *gtk3gui,
                           const gchar *filedesc );
void gtk3_createRenderEdit ( GTK3G3DUI *gtk3gui );
uint64_t gtk3_importFile ( GTK3G3DUI   *gtk3gui,
                           const gchar *filedesc );
void gtk3_renderView ( GTK3G3DUI *gtk3gui );
void gtk3_runRender ( GTK3G3DUI *gtk3gui );
gint gtk3_exit ( GTK3G3DUI *gtk3gui );
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

GtkScale *ui_createHorizontalScale ( GtkFixed  *parent,
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
GtkCheckButton *ui_createToggleLabel ( GtkFixed *parent, 
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

GtkTextView *ui_createTextView ( GtkWidget *parent, 
                                 void      *data,
                                 char      *name,
                                 char      *class,
                                 gint       x, 
                                 gint       y,
                                 gint       width,
                                 gint       height,
                                 void     (*cbk)( GtkTextBuffer *,
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
                           const  char       **xpm,
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
GtkComboBoxText *ui_createProjectionSelector ( GtkFixed *parent,
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
GtkDrawingArea *ui_createDrawingArea ( GtkFixed *parent,
                                       void     *data,
                                       char     *name,
                                       char     *class,
                                       gint      x, 
                                       gint      y,
                                       gint      width,
                                       gint      height,
                                       void    (*cbk)( GtkWidget *, 
                                                       cairo_t   *cr,
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
GtkTextView *ui_gtk_text_view_new ( char *class );
GtkScale *ui_gtk_scale_new_with_range ( char *class,
                                        GtkOrientation orientation,
                                        double         min,
                                        double         max,
                                        double         step );
GtkEntry *ui_gtk_entry_new ( char *class );
GtkEventBox *ui_gtk_event_box_new ( char *class );
GtkFrame *ui_gtk_frame_new ( char       *class,
                             const char *label );
GtkToggleToolButton *ui_gtk_toggle_tool_button_new ( char *class );
GtkToolItem *ui_gtk_tool_button_new ( char        *class,
                                      GtkWidget   *icon_widget,
                                      const gchar *label );
GtkDialog *ui_gtk_dialog_new ( char *class );
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
GtkCheckButton *ui_gtk_check_button_new_with_label ( char *class,
                                                     char *label );

uint64_t gtk3_g3dui_saveAlteredImages ( GTK3G3DUI *gtk3gui );
uint64_t gtk3_g3dui_saveChannelAlteredImage ( GTK3G3DUI  *gtk3gui,
                                              char       *materialName,
                                              G3DCHANNEL *chn,
                                              uint32_t    ask,
                                              uint32_t    rename );
uint32_t gtk3_g3dui_saveChannelImageAs ( GTK3G3DUI  *gtk3gui,
                                         G3DIMAGE   *img );

/******************************* g3duiboard.c *********************************/
GTK3G3DUIBOARD *gtk3_g3duiboard_create ( GtkWidget *parent,
                                         GTK3G3DUI *gtk3gui,
                                         char      *name );
void gtk3_g3duiboard_resize ( GTK3G3DUIBOARD *gtk3board,
                              uint32_t        width,
                              uint32_t        height );

/****************************** g3duiboneedit.c *******************************/
GTK3G3DUIBONEEDIT *gtk3_g3duiboneedit_create ( GtkWidget *parent,
                                               GTK3G3DUI *gtk3gui,
                                               char      *name );
void gtk3_g3duiboneedit_update ( GTK3G3DUIBONEEDIT *gtk3bed );

/*************************** g3duicameraedit.c ********************************/
GTK3G3DUICAMERAEDIT *gtk3_g3duicameraedit_create ( GtkWidget *parent,
                                                   GTK3G3DUI *gtk3gui,
                                                   char      *name,
                                                   uint32_t   forKey );
void gtk3_g3duicameraedit_update ( GTK3G3DUICAMERAEDIT *gtk3ced,
                                   G3DCAMERA           *cam );

/******************************** g3duicom.c **********************************/
Q3DFILTER *q3dfilter_gotoframe_new ( GTK3G3DUI *gtk3gui );
Q3DFILTER *q3dfilter_preview_new ( GTK3G3DUI *gtk3gui );
void gtk3_g3duicom_handleAction ( GtkWidget *widget, 
                             gpointer   ptr, 
                             gpointer   user_data );
void gtk3_g3duicom_handleAction ( GtkWidget *widget, 
                                  gpointer   ptr, 
                                  gpointer   user_data );
void gtk3_g3duicom_requestActionFromMainThread ( GTK3G3DUI   *gtk3gui,
                                                 G3DUIACTION *action );

/**************************** g3duicoordinatesedit.c **************************/
void gtk3_g3duicoordinatesedit_update ( GTK3G3DUICOORDINATESEDIT *gtk3cedit );
GTK3G3DUICOORDINATESEDIT *gtk3_g3duicoordinatesedit_create ( GtkWidget *parent, 
                                                             GTK3G3DUI *gtk3gui,
                                                             char      *name );

/******************************* g3duicubeedit.c ******************************/
void gtk3_g3duicubeedit_update ( GTK3G3DUICUBEEDIT *gtk3ced );
GTK3G3DUICUBEEDIT *gtk3_g3duicubeedit_create ( GtkWidget *parent,
                                               GTK3G3DUI *gtk3gui,
                                               char      *name );

/***************************** g3duicutmeshtool.c *****************************/
void gtk3_g3duicutmeshtooledit_update ( GTK3G3DUICUTMESHTOOLEDIT *gtk3cmedit );
GTK3G3DUICUTMESHTOOLEDIT *gtk3_g3duicutmeshtooledit_create ( GtkWidget *parent, 
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

/******************************* g3duikeyedit.c *******************************/
GTK3G3DUIKEYEDIT *gtk3_g3duikeyedit_create ( GtkWidget *parent,
                                             GTK3G3DUI *gtk3gui,
                                             char      *name );
void gtk3_g3duikeyedit_update ( GTK3G3DUIKEYEDIT *gtk3ked );

/***************************** g3duilightedit.c *******************************/
void gtk3_g3duilightedit_update ( GTK3G3DUILIGHTEDIT *gtk3led,
                                  G3DLIGHT           *lig );
GTK3G3DUILIGHTEDIT *gtk3_g3duilightedit_create ( GtkWidget *parent,
                                                 GTK3G3DUI *gtk3gui,
                                                 char      *name,
                                                 uint32_t   forKey );

/******************************* g3duimenu.c **********************************/
GTK3G3DUIMENU *gtk3_g3duimenu_parse_r ( G3DUIMENU *node,
                                        G3DUI     *gui,
                                        void      *data );
void gtk3_g3duimenu_update_r ( GTK3G3DUIMENU *gtk3node );

/***************************** g3duimeshedit.c ********************************/
GTK3G3DUIMESHEDIT *gtk3_g3duimeshedit_create ( GtkWidget *parent,
                                               GTK3G3DUI *gtk3gui,
                                               char      *name );
void gtk3_g3duimeshedit_update ( GTK3G3DUIMESHEDIT *gtk3med );

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
void gtk3_g3duimaterialedit_resize ( GTK3G3DUIMATERIALEDIT *gtk3med,
                                     uint32_t               width,
                                     uint32_t               height );

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

/*************************** g3duimousetooledit.c *****************************/
void gtk3_g3duimousetooledit_update ( GTK3G3DUIMOUSETOOLEDIT *gtk3mtledit );
GTK3G3DUIMOUSETOOLEDIT *gtk3_g3duimousetooledit_create ( GtkWidget *parent, 
                                                         GTK3G3DUI *gtk3gui,
                                                         char      *name );


/**************************** g3duimorpheredit.c ******************************/
GTK3G3DUIMORPHEREDIT *gtk3_g3duimorpheredit_create ( GtkWidget *parent,
                                                     GTK3G3DUI *gtk3gui,
                                                     char      *name );
void gtk3_g3duimorpheredit_update ( GTK3G3DUIMORPHEREDIT *gtk3med );

/*************************** g3duimorpherkeyedit.c ****************************/
GTK3G3DUIMORPHERKEYEDIT *gtk3_g3duimorpherkeyedit_create ( GtkWidget *parent,
                                                           GTK3G3DUI *gtk3gui,
                                                           char      *name,
                                                           G3DKEY    *key );

/************************ g3duiparticleemitter.c ******************************/
void gtk3_g3duiparticleemitteredit_update ( GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped,
                                            G3DPARTICLEEMITTER           *pemitter );
GTK3G3DUIPARTICLEEMITTEREDIT *gtk3_g3duiparticleemitteredit_create ( GtkWidget *parent,
                                                                     GTK3G3DUI *gtk3gui,
                                                                     char      *name,
                                                                     uint32_t   forKey );

/**************************** g3duipicktooledit.c *****************************/

GTK3G3DUIPICKTOOLEDIT *gtk3_g3duipicktooledit_create ( GtkWidget *parent, 
                                                       GTK3G3DUI *gtk3gui,
                                                       char      *name );
void gtk3_g3duipicktooledit_update ( GTK3G3DUIPICKTOOLEDIT *gtk3pickedit );

/**************************** g3duiplaneedit.c ********************************/
void gtk3_g3duiplaneedit_update ( GTK3G3DUIPLANEEDIT *gtk3ped );
GTK3G3DUIPLANEEDIT *gtk3_g3duiplaneedit_create ( GtkWidget *parent,
                                               GTK3G3DUI *gtk3gui,
                                               char      *name );

/************************* g3duiproceduralbrickedit.c *************************/
GTK3G3DUIPROCEDURALBRICKEDIT* 
gtk3_g3duiproceduralbrickedit_create ( GtkWidget          *parent,
                                       GTK3G3DUI          *gtk3gui,
                                       G3DPROCEDURALBRICK *brick,
                                       char               *name,
                                       gint                x,
                                       gint                y,
                                       gint                width,
                                       gint                height,
                                       uint32_t            bindGL );

/************************* g3duiproceduralchessedit.c *************************/
GTK3G3DUIPROCEDURALCHESSEDIT* 
gtk3_g3duiproceduralchessedit_create ( GtkWidget          *parent,
                                       GTK3G3DUI          *gtk3gui,
                                       G3DPROCEDURALCHESS *chess,
                                       char               *name,
                                       gint                x,
                                       gint                y,
                                       gint                width,
                                       gint                height,
                                       uint32_t            bindGL );

/************************ g3duiproceduralgradientedit.c ***********************/
GTK3G3DUIPROCEDURALGRADIENTEDIT* 
gtk3_g3duiproceduralgradientedit_create ( GtkWidget             *parent,
                                          GTK3G3DUI             *gtk3gui,
                                          G3DPROCEDURALGRADIENT *gradient,
                                          char                  *name,
                                          gint                   x,
                                          gint                   y,
                                          gint                   width,
                                          gint                   height,
                                          uint32_t               bindGL );

/************************* g3duiproceduralnoiseedit.c *************************/
GTK3G3DUIPROCEDURALNOISEEDIT* 
gtk3_g3duiproceduralnoiseedit_create ( GtkWidget          *parent,
                                       GTK3G3DUI          *gtk3gui,
                                       G3DPROCEDURALNOISE *noise,
                                       char               *name,
                                       gint                x,
                                       gint                y,
                                       gint                width,
                                       gint                height,
                                       uint32_t            bindGL );

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

/************************* g3duisplinerevolveredit.c **************************/
GTK3G3DUISPLINEREVOLVEREDIT *gtk3_g3duisplinerevolveredit_create ( GtkWidget *parent,
                                                                   GTK3G3DUI *gtk3gui,
                                                                   char      *name );
void gtk3_g3duisplinerevolveredit_update ( GTK3G3DUISPLINEREVOLVEREDIT *gtk3sed );

/*************************** g3duisubdivideredit.c ****************************/
void gtk3_g3duisubdivideredit_update ( GTK3G3DUISUBDIVIDEREDIT *gtk3sed );
GTK3G3DUISUBDIVIDEREDIT *gtk3_g3duisubdivideredit_create ( GtkWidget *parent,
                                                           GTK3G3DUI *gtk3gui,
                                                           char      *name );

/*************************** g3duisymmetryedit.c ****************************/
GTK3G3DUISYMMETRYEDIT *gtk3_g3duisymmetryedit_create ( GtkWidget *parent,
                                                       GTK3G3DUI *gtk3gui,
                                                       char      *name );
void gtk3_g3duisymmetryedit_update ( GTK3G3DUISYMMETRYEDIT *gtk3sed );

/****************************** g3duitextedit.c *******************************/
GTK3G3DUITEXTEDIT *gtk3_g3duitextedit_create ( GtkWidget *parent,
                                               GTK3G3DUI *gtk3gui,
                                               char      *name );
void gtk3_g3duitextedit_update ( GTK3G3DUITEXTEDIT *gtk3ted );

/**************************** g3duitextureedit.c ******************************/
GTK3G3DUITEXTUREEDIT* gtk3_g3duitextureedit_create ( GtkWidget *parent,
                                                     GTK3G3DUI *gtk3gui,
                                                     char      *name,
                                                     G3DOBJECT *obj );

void updateTextureEdit ( GTK3G3DUITEXTUREEDIT *gtk3ted );

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

/**************************** g3duitrackertagedit.c ***************************/
GTK3G3DUITRACKERTAGEDIT* gtk3_g3duitrackertagedit_create ( GtkWidget *parent,
                                                           GTK3G3DUI *gtk3gui,
                                                           char      *name,
                                                           G3DOBJECT *obj );


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
void gtk3_g3duiobjectedit_resize ( GTK3G3DUIOBJECTEDIT *gtk3objedit,
                                   uint32_t             width,
                                   uint32_t             height );

/**************************** g3duiobjectlist.c *******************************/
GTK3G3DUIOBJECTLIST *gtk3_g3duiobjectlist_create ( GtkWidget *parent,
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

/************************** g3duisculpttooledit.c *****************************/
void gtk3_g3duisculpttooledit_update ( GTK3G3DUISCULPTTOOLEDIT *gtk3sculptedit );
GTK3G3DUISCULPTTOOLEDIT *gtk3_g3duisculpttooledit_create ( GtkWidget *parent, 
                                                           GTK3G3DUI *gtk3gui,
                                                           char      *name );

/****************************** g3duiuvmapedit.c ******************************/
void gtk3_g3duiuvmapedit_update ( GTK3G3DUIUVMAPEDIT *gtk3uvmed );
GTK3G3DUIUVMAPEDIT* gtk3_g3duiuvmapedit_create ( GtkWidget *parent,
                                                 GTK3G3DUI *gtk3gui,
                                                 char      *name,
                                                 G3DOBJECT *obj );

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





void gdkevent_to_g3devent ( GdkEvent *gdkev, G3DEvent *g3dev );


/******************************************************************************/
/*********************************** MakeUP 3D ********************************/

/*********************************** m3dui.c **********************************/
GTK3M3DUI *gtk3_m3dui_create ( GTK3G3DUI *gtk3gui );
void gtk3_m3dui_display ( GTK3M3DUI *gtk3mui );
uint64_t gtk3_m3dui_loadImageByChannelIDCbk ( GTK3M3DUI *gtk3mui );
uint64_t gtk3_m3dui_setMouseTool ( GTK3M3DUI *gtk3mui,
                                   GtkWidget *button,
                                   char      *toolName );
void gtk3_m3dui_saveimage ( GTK3M3DUI *gtk3mui );

/********************************* m3duiboard.c *******************************/
void gtk3_m3duiboard_resize ( GTK3M3DUIBOARD *gtk3board,
                              uint32_t        width,
                              uint32_t        height );
GTK3M3DUIBOARD *gtk3_m3duiboard_create ( GtkWidget *parent,
                                         GTK3M3DUI *gtk3mui,
                                         char      *name );

/*************************** m3duichannelimagecreator.c************************/
GTK3M3DUICHANNELIMAGECREATOR *gtk3_m3duichannelimagecreator_create ( GtkWidget  *parent,
                                                                     GTK3M3DUI  *gtk3mui,
                                                                     G3DCHANNEL *chn,
                                                                     uint32_t    resize,
                                                                     char       *name );

/*************************** m3duibuckettooledit.c ****************************/
void gtk3_m3duibuckettooledit_update ( GTK3M3DUIBUCKETTOOLEDIT *gtk3buckedit );
GTK3M3DUIBUCKETTOOLEDIT *gtk3_m3duibuckettooledit_create ( GtkWidget *parent, 
                                                           GTK3M3DUI *gtk3mui,
                                                           char      *name );

/***************************** m3duicolorpicker.c *****************************/
GTK3M3DUICOLORPICKER *gtk3_m3duicolorpicker_create ( GtkWidget *parent,
                                                     GTK3M3DUI *gtk3mui,
                                                     char      *name );

/********************************* m3duimain.c ********************************/
void gtk3_m3duimain_resize ( GTK3M3DUIMAIN *gtk3main,
                             uint32_t       width,
                             uint32_t       height );
GTK3M3DUIMAIN *gtk3_m3duimain_create ( GtkWidget *parent,
                                       GTK3M3DUI *gtk3mui,
                                       char      *name );

/******************************** m3duimodebar.c ******************************/
GTK3M3DUIMODEBAR *gtk3_m3duimodebar_create ( GtkWidget *parent,
                                             GTK3M3DUI *gtk3gui,
                                             char      *name );

/***************************** m3duimousetool.c *******************************/
void gtk3_m3duimousetooledit_update ( GTK3M3DUIMOUSETOOLEDIT *gtk3mtledit );
GTK3M3DUIMOUSETOOLEDIT *gtk3_m3duimousetooledit_create ( GtkWidget *parent, 
                                                         GTK3M3DUI *gtk3mui,
                                                         char      *name );

/***************************** m3duipatternlist.c *****************************/
GTK3M3DUIPATTERNLIST *gtk3_m3duipatternlist_create ( GtkWidget *parent,
                                                     GTK3M3DUI *gtk3mui,
                                                     char      *name );

/***************************** m3duipentooledit.c *****************************/
void gtk3_m3duipentooledit_update ( GTK3M3DUIPENTOOLEDIT *gtk3penedit );
GTK3M3DUIPENTOOLEDIT *gtk3_m3duipentooledit_create ( GtkWidget *parent, 
                                                     GTK3M3DUI *gtk3mui,
                                                     char      *name );

/******************************** m3duitoolbar.c ******************************/
GTK3M3DUITOOLBAR *gtk3_m3duitoolbar_create ( GtkWidget *parent,
                                             GTK3M3DUI *gtk3gui,
                                             char      *name );

/********************************* m3duiview.c ********************************/
GTK3M3DUIVIEW *gtk3_m3duiview_create ( GtkWidget *parent,
                                       GTK3M3DUI *gtk3mui,
                                       char      *name );
void gtk3_m3duiview_updateMenuBar ( GTK3M3DUIVIEW *gtk3view );
void gtk3_m3duiview_resize ( GTK3M3DUIVIEW *gtk3view,
                             uint32_t       width,
                             uint32_t       height );



/******************************************************************************/
/********************************* MENUs **************************************/
/*** this was supposed to be in the common code but I need some GTK calls ***/
/*** so I'll figure out that later ***/
/****************************** menu/condition.c ******************************/

uint32_t objectModeOnly ( G3DUIMENU *menu, void *data );
uint32_t vertexModeOnly ( G3DUIMENU *menu, void *data );
uint32_t faceModeOnly ( G3DUIMENU *menu, void *data );
uint32_t sculptModeOnly ( G3DUIMENU *menu, void *data );
uint32_t edgeModeOnly ( G3DUIMENU *menu, void *data );
uint32_t skinModeOnly ( G3DUIMENU *menu, void *data );
uint32_t objectMode_skinSelected ( G3DUIMENU *menu, void *data );
uint32_t objectMode_objectSelected ( G3DUIMENU *menu, void *data );
uint32_t objectMode_boneSelected ( G3DUIMENU *menu, void *data );
uint32_t objectMode_boneOrSkinSelected ( G3DUIMENU *menu, void *data );
uint32_t object_objectSelectedAndUVFixed ( G3DUIMENU *menu,
                                           void      *data );

/**************************** menu/mainmenu.c ****************************/
G3DUIMENU *g3duimenu_getMainMenuNode ( );

/************************ menu/materialboardmenu.c ***********************/
G3DUIMENU *g3duimenu_getMaterialBoardMenuNode ( );

/************************* menu/objectboardmenu.c ************************/
G3DUIMENU *g3duimenu_getObjectBoardMenuNode ( );

/************************* menu/renderwindowmenu.c ***********************/
G3DUIMENU *g3duimenu_getRenderWindowMenuNode ( );

/************************** menu/timelinemenu.c **************************/
G3DUIMENU *g3duimenu_getTimelineMenuNode ( );

/************************** menu/uveditormenu.c **************************/
G3DUIMENU *g3duimenu_getUVEditorMenuNode ( );

/************************** menu/viewmenu.c **************************/
G3DUIMENU *g3duimenu_getViewMenuNode ( );

/************************** menu/uvviewmenu.c **************************/
G3DUIMENU *g3duimenu_getUVViewMenuNode ( );

#endif
