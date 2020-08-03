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

#ifndef _G3DUI_MOTIF_H_
#define _G3DUI_MOTIF_H_

/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

/******************************************************************************/
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

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
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/Frame.h>
#include <Xm/ComboBox.h>
#include <Xm/TextF.h>
#include <Xm/Text.h>
#include <Xm/CascadeB.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/FileSB.h>
#include <Xm/MessageB.h>
#include <Xm/ScrolledW.h>

/******************************************************************************/
#include <X11/xpm.h>
#include <X11/cursorfont.h>

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/GLwMDrawA.h>

/******************************************************************************/
#include <list.h>
#include <g3dengine/g3dengine.h>
#include <r3d.h>
#include <r3dnet.h>
#include <g3durmanager.h>
#include <g3dmouse.h>
#include <g3dexport.h>
#include <g3dimport.h>
#include <g3dui.h>

/******************************************************************************/
#include <TabbedBook.h>
#include <ColorEdit.h>
#include <SpinButton.h>

/******************************************************************************/
#define G3DUIWIDGET Widget

/******************************************************************************/
/******************************** QUAD WIDGET *********************************/

#define XtNxpos      "Xpos"
#define XtNypos      "Ypos"
#define XtNxratio    "Xratio"
#define XtNyratio    "Yratio"
#define XtNsegment   "Segment"
#define XtNmax       "Max"
#define XtNscene     "Scene"
#define XtNloadFile  "LoadFile"

/****************************** Toolkit Data **********************************/
typedef struct _G3DUIMOTIF {
    XFontStruct  *nrmlft;
    XFontStruct  *boldft;
    XFontStruct  *tinyft;
    XmFontList    fontlist;
    XmRenderTable renTable;
    Widget top;
    Widget main;
    Widget curogl; /*** current OpenGL Widget - the one we used last ***/
    Widget curmou; /*** store the current pressed toggle button      ***/
    Widget curmat; /*** current material ***/
    Cursor watchCursor;
} G3DUIMOTIF;

/*****************************  Class record pointer **************************/
extern WidgetClass guiWidgetClass;

/*************************** Full instance record declaration *****************/
typedef struct _GUiRec {
    CorePart core;
    CompositePart composite;
    ConstraintPart constraint;
    XmManagerPart manager;
    G3DUI gui;
} GUiRec, *GUiWidget;

/***************************** GQuad Class full record ************************/
typedef struct _GUiClassRec {
    CoreClassPart core;
    CompositeClassPart composite;
    ConstraintClassPart constraint;
    XmManagerClassPart manager;
} GUiClassRec;

/****************************** Class record variable *************************/
extern GUiClassRec guiClassRec;

/******************************************************************************/
/******************************************************************************/



/*****************************  Class record pointer **************************/
extern WidgetClass gquadWidgetClass;

/*************************** Full instance record declaration *****************/
typedef struct _GQuadRec {
    CorePart       core;
    CompositePart  composite;
    ConstraintPart constraint;
    XmManagerPart  manager;
    G3DUIQUAD      quad;            /*** this can be shared across UI Toolkits ***/
    Widget         maximized_view; /*** This can't ***/
} GQuadRec, *GQuadWidget;

/************************** Types for GQuad class methods *********************/
typedef void (*GQuadMoveGlider) ( Widget w );

/***************************** GQuad Class full record ************************/
typedef struct _GQuadClassRec {
    CoreClassPart core;
    CompositeClassPart composite;
    ConstraintClassPart constraint;
    XmManagerClassPart manager;
} GQuadClassRec;

/****************************** Class record variable *************************/
extern GQuadClassRec gquadClassRec;

/******************************************************************************/
/******************************************************************************/








/******************************************************************************/
/******************************** VIEW WIDGET *********************************/

#define XtNcamera        "Camera"
#define XmNcamera        XtNcamera

#define XtNdefaultCamera "Default Camera"
#define XmNdefaultCamera XtNdefaultCamera

#define XtNdefaultCameraPosition "Default XPosition"
#define XmNdefaultCameraPosition XtNdefaultPosition
#define XtNdefaultCameraRotation "Default XRotation"
#define XmNdefaultCameraRotation XtNdefaultRotation
#define XtNdefaultCameraScaling  "Default XScaling"
#define XmNdefaultCameraScaling  XtNdefaultScaling
#define XtNdefaultCameraFocal    "Default Focal"
#define XmNdefaultCameraFocal    XtNdefaultFocal


/*****************************  Class record pointer **************************/
extern WidgetClass gviewWidgetClass;



/*************************** Full instance record declaration *****************/
typedef struct _GViewRec {
    CorePart core;
    CompositePart composite;
    ConstraintPart constraint;
    XmManagerPart manager;
    G3DUIVIEW gview;
    Pixmap icon_nor[NBVIEWBUTTON];
    Pixmap icon_arm[NBVIEWBUTTON];
} GViewRec, *GViewWidget;

/******************************* GView class fields ***************************/
typedef struct _GViewClassPart {
    int dummy;
} GViewClassPart;

/***************************** GView Class full record ************************/
typedef struct _GViewClassRec {
    CoreClassPart core;
    CompositeClassPart composite;
    ConstraintClassPart constraint;
    XmManagerClassPart manager;
    GViewClassPart gview;
} GViewClassRec;

/****************************** Class record variable *************************/
extern GViewClassRec gviewClassRec;

/******************************************************************************/
/******************************************************************************/


/******************************************************************************/
/******************************** TAB WIDGET **********************************/
#define XtNtabitem     "TabItem"
#define XtNtabWidth    "TabWidth"
#define XtNtabHeight   "TabHeight"
#define XtNtabStart    "TabStart"
#define XtNtabType     "TabType"
#define XtNselectedTab "selectedTab"

/*****************************  Class record pointer **************************/
extern WidgetClass gtabWidgetClass;

/******************************************************************************/
typedef struct _GTABITEM {
    char **xpm; /*** XPM Image ***/
    XmString str;
    Pixmap pixnor;
    Pixmap pixarm;
    Widget w;
    uint32_t x, y, width, height;
} GTABITEM;

/******************************* Fields ***************************************/
typedef struct _GTabPart {
    LIST *ltab;
    uint32_t orientation;      /*** XmHORIZONTAL or XmVERTICAL ***/
    uint32_t tabtype;          /*** XmSTRING or XmPIXMAP       ***/
    uint32_t tabstart;         /*** Starting tab               ***/
    uint32_t tabwidth;         /*** Tabs width                 ***/
    uint32_t tabheight;        /*** Tabs height                ***/
    uint32_t seltab;           /*** index of selected tab      ***/
} GTabPart;

/*************************** Full instance record declaration *****************/
typedef struct _GTabRec {
    CorePart core;
    CompositePart composite;
    ConstraintPart constraint;
    XmManagerPart manager;
    GTabPart tab;
} GTabRec, *GTabWidget;

/******************************* GTab class fields ****************************/
typedef struct _GTabClassPart {
    int dummy;
} GTabClassPart;

/***************************** GTab Class full record *************************/
typedef struct _GTabClassRec {
    CoreClassPart core;
    CompositeClassPart composite;
    ConstraintClassPart constraint;
    XmManagerClassPart manager;
    GTabClassPart gtab;
} GTabClassRec;

/****************************** Class record variable *************************/
extern GTabClassRec gtabClassRec;


/******************************************************************************/
/******************************** LIST WIDGET *********************************/

/*****************************  Class record pointer **************************/
extern WidgetClass glistWidgetClass;

/******************************* Fields ***************************************/
typedef struct _GListPart {
    Pixmap bonico;
    Pixmap ligico;
    Pixmap mesico;
    Pixmap defico;
    Pixmap symico;
    Pixmap cubico;
    Pixmap sphico;
    Pixmap torico;
    Pixmap conico;
    Pixmap plnico;
    Pixmap cylico;
    Pixmap ffdico;
    Pixmap camico;
    Pixmap scnico;
    Pixmap mapico;
} GListPart;

/*************************** Full instance record declaration *****************/
typedef struct _GListRec {
    CorePart         core;
    XmPrimitivePart  primitive;
    GListPart        glist;
} GListRec, *GListWidget;

/******************************* GView class fields ***************************/
typedef struct _GListClassPart {
    int dummy;
} GListClassPart;

/***************************** GList Class full record ************************/
typedef struct _GListClassRec {
    CoreClassPart         core;
    XmPrimitiveClassPart  primitive;
    GListClassPart        glist;
} GListClassRec;

/****************************** Class record variable *************************/
extern GListClassRec glistClassRec;

/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/************************** GTK MaterialList Widget ***************************/
typedef struct _MOTIFMATERIALPREVIEW {
    XImage *img;
    G3DMATERIAL *mat;
    G3DUIRECTANGLE rec;
} MOTIFMATERIALPREVIEW;

MOTIFMATERIALPREVIEW *motifmaterialpreview_new    ( Display *, G3DMATERIAL *,
                                                    G3DUIMATERIALMAP * );
void                  motifmaterialpreview_free   ( MOTIFMATERIALPREVIEW * );
void                  motifmaterialpreview_update ( Display *, MOTIFMATERIALPREVIEW *,
                                                    G3DUIMATERIALMAP * );



/******************************************************************************/
void createTransparentPixmap ( Display *, /* Display                    */
                               Pixmap *,  /* Return value:Normal Pixmap */
                               Pixmap *,  /* Return value:Armed Pixmap  */
                               char **,   /* Color values               */
                               int ,      /* Color depth                */
                               Pixel ,    /* Background col when normal */
                               Pixel );   /* Background col when armed  */

XImage *createXImageFromData ( Display *, uint32_t, uint32_t, uint32_t, unsigned char (*)[3] );

Widget createModeBar        ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createMenuBar        ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createQuad           ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createTimeboard      ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createToolBar        ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createTab            ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createMaterialForm   ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createObjectList     ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createObjectEdit     ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createSphereEdit     ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createMeshEdit       ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createCubeEdit       ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createCylinderEdit   ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createCurrentEdit    ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createMaterialEdit   ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createLightEdit      ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createSymmetryEdit   ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createFFDEdit        ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createPlaneEdit      ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createTorusEdit      ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createRenderWindow   ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );
Widget createCoordinatesEdit( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

void   updateG3DMouseTool ( Widget, G3DUI * );
Widget createG3DMouseToolEdit ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                         Dimension, Dimension );

void   updateRenderEdit     ( Widget, G3DUI * );
Widget createRenderEdit     ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createMaterialEdit   ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createKeyEdit        ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );
Widget createTextureEdit    ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createRenderSettings ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createFrame          ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createBoard          ( Widget, G3DUI *, char *, Dimension, Dimension,
                                                       Dimension, Dimension );

Widget createColorButton    ( Widget, G3DUI *, char *, Dimension, Dimension, 
                                                       Dimension, Dimension,
                                                       void (*)( Widget, 
                                                                 XtPointer,
                                                                 XtPointer ) );

void createRadioLabel       ( Widget, G3DUI *, char *, Dimension, Dimension, 
                                                       Dimension, Dimension,
                                                       void (*)( Widget, 
                                                                 XtPointer,
                                                                 XtPointer ) );

Widget createView           ( Widget, G3DUI *, char *, G3DCAMERA *,
                                                       Dimension, Dimension,
                                                       Dimension, Dimension );
Widget createGLView         ( Widget, G3DUI *, Dimension, Dimension,
                                               Dimension, Dimension );

void arrowSetValue ( Widget, int  );

Widget createMaterialListMenu ( Widget, G3DUI *, char * );


Widget createMaterialColorEditForm ( Widget, G3DUI *, char *,
                                     Dimension, Dimension,
                                     Dimension, Dimension );

void createTitleLabel ( Widget, char *, Dimension, Dimension );
void createToggleButton ( Widget, G3DUI *, char *, Dimension, Dimension,
                                          Dimension, Dimension,
                                          void (*armcbk)( Widget, XtPointer,
                                                                  XtPointer ) );
void createRadioButton ( Widget, G3DUI *, char *, Dimension, Dimension,
                                         Dimension, Dimension,
                                         void (*armcbk)( Widget, XtPointer,
                                                                 XtPointer ) );
void createToggleLabel  ( Widget, G3DUI *, char *, Dimension, Dimension,
                                          Dimension, Dimension,
                                          void (*armcbk)( Widget, XtPointer,
                                                                  XtPointer ) );

Widget createHorizontalScale ( Widget, G3DUI *, 
                                              char *,
                                              Dimension, 
                                              Dimension,
                                              Dimension, 
                                              Dimension,
                                              int, int, int,
                                              void (*)( Widget, XtPointer, 
                                                                   XtPointer ) );

/*** Creates a Label + a text field for typing an integer value ***/
Widget createIntegerText          ( Widget, G3DUI *, char *, Dimension, Dimension, 
                                                  Dimension, Dimension,
                                                  void (*)( Widget, 
                                                            XtPointer,
                                                            XtPointer ) );

/*** Creates a Label + a text field for typing an floating value ***/
void createFloatText            ( Widget, G3DUI *, char *, Dimension, Dimension, 
                                                  Dimension, Dimension,
                                                  void (*)( Widget, 
                                                            XtPointer,
                                                            XtPointer ) );

/*** Creates a Label + a text field for typing a character string ***/
void createCharText             ( Widget, G3DUI *, char *, Dimension, Dimension, 
                                                  Dimension, Dimension,
                                                  void (*)( Widget, 
                                                            XtPointer,
                                                            XtPointer ) );

/*** Creates a Label + a text field for selecting an orientation plane ***/
void createOrientationSelection ( Widget, G3DUI *, char *, Dimension, Dimension, 
                                                  Dimension, Dimension,
                                                  void (*)( Widget, 
                                                            XtPointer,
                                                            XtPointer ) );

void createUVMappingSelection   ( Widget, G3DUI *, char *, Dimension, Dimension, 
                                                  Dimension, Dimension,
                                                  void (*)( Widget, 
                                                            XtPointer,
                                                            XtPointer ) );

void createSimpleLabel          ( Widget, G3DUI *, char *, Dimension, Dimension, 
                                                  Dimension, Dimension );

void createSimplePushButton     ( Widget, G3DUI *, char *, Dimension, Dimension, 
                                                  Dimension, Dimension,
                                                  void (*)( Widget, 
                                                            XtPointer,
                                                            XtPointer ) );

void createPushButton     ( Widget, G3DUI *, char *, Dimension, Dimension, 
                                                  Dimension, Dimension,
                                                  void (*)( Widget, 
                                                            XtPointer,
                                                            XtPointer ) );
Widget createMenuBarButton ( Widget, G3DUI *, char *,
                             Dimension , void (*) ( Widget, XtPointer, 
                                                            XtPointer ) );

Widget g3dui_addMenuButton ( Widget, G3DUI *, char *, Dimension,
                                           void (*) ( Widget, XtPointer, 
                                                              XtPointer ) );
Widget g3dui_addMenuSeparator ( Widget );

void redrawTimeline ( G3DUI * );

unsigned long XmColorEditGetPixel ( Widget );

void addmaterialcbk ( Widget, XtPointer, XtPointer );
void delmaterialcbk ( Widget, XtPointer, XtPointer );
void setmaterialcbk ( Widget, XtPointer, XtPointer );

void bigpreviewscbk ( Widget, XtPointer, XtPointer );
void normalpreviewscbk ( Widget, XtPointer, XtPointer );
void smallpreviewscbk ( Widget, XtPointer,  XtPointer );

void updateSelectedMaterialPreview ( G3DUI * );

void resizePreviews ( Widget );

void updateGLViews ( G3DUI * );

Widget g3duiMaterialBoard_getListWidget ( Widget );

void g3dui_updateAllCurrentEdit ( G3DUI * );
void g3dui_updateCoords         ( G3DUI * );
void g3dui_redrawObjectList     ( G3DUI * );

void updateSphereEdit ( Widget, G3DUI * );
void updateCylinderEdit ( Widget, G3DUI * );
void updateCubeEdit ( Widget, G3DUI * );
void updateCurrentEdit ( Widget, G3DUI * );

void updateMeshEdit ( Widget, G3DUI * );
void updateLightEdit ( Widget, G3DUI * );
void updatePlaneEdit ( Widget, G3DUI * );
void updateTorusEdit ( Widget, G3DUI * );
void updateAllCurrentMouseTools ( G3DUI *gui );
void updateCoordinatesEdit ( Widget, G3DUI * );

/****************************** Callbacks *************************************/
void g3dui_deleteSelectionCbk ( Widget, XtPointer, XtPointer );
void g3dui_makeEditableCbk    ( Widget, XtPointer, XtPointer );
void g3dui_undoCbk            ( Widget, XtPointer, XtPointer );
void g3dui_redoCbk            ( Widget, XtPointer, XtPointer );
void g3dui_setMouseTool       ( Widget, XtPointer, XtPointer );
void g3dui_newSceneCbk        ( Widget, XtPointer, XtPointer );
void g3dui_openFileCbk        ( Widget, XtPointer, XtPointer );
void g3dui_saveFileCbk        ( Widget, XtPointer, XtPointer );
void g3dui_saveAsCbk          ( Widget, XtPointer, XtPointer );
void g3dui_exitCbk            ( Widget, XtPointer, XtPointer );
void g3dui_runRenderCbk       ( Widget, XtPointer, XtPointer );

void g3dui_addImage    ( G3DUI *, G3DIMAGE * );
void g3dui_removeImage ( G3DUI *, G3DIMAGE * );
void g3dui_disableTextureImages ( G3DUI * );
void g3dui_enableTextureImages ( G3DUI * );
void g3dui_loadConfiguration ( G3DUI *, char * );
G3DUI *g3dui_new ( XtAppContext, Widget, G3DSCENE *, Dimension, Dimension );
void *g3dui_playScene_t ( G3DUI * );
void g3dui_createDefaultColors ( G3DUI * );
void g3dui_setMouseTool ( Widget, XtPointer, XtPointer );
void g3dui_addMouseTool ( G3DUI *, G3DMOUSETOOL *, uint32_t );
void g3dui_createTransparentPixmap ( G3DUI *, Pixmap *, Pixmap *, char ** );
void g3dui_addToolBarButton ( G3DUI *, char **, void (*)( Widget, XtPointer,
                                                                  XtPointer ),
                                                void * );
void g3dui_addModeBarButton ( G3DUI *, char **, void (*)( Widget, XtPointer,
                                                                  XtPointer ),
                                                void * );
Widget createRenderWindowMenuBar ( Widget, G3DUI *, char *,
                                   Dimension, Dimension,
                                   Dimension, Dimension );

void g3dui_addFaceToolsButton ( G3DUI *, G3DMOUSETOOL * );
void g3dui_createFaceTools ( G3DUI *, Dimension, Dimension,
                                      Dimension, Dimension  );
void g3dui_addObjectToolsButton ( G3DUI *, G3DMOUSETOOL * );
void g3dui_createObjectTools ( G3DUI *, Dimension, Dimension,
                                        Dimension, Dimension  );
void g3dui_addVertexToolsButton ( G3DUI *, G3DMOUSETOOL * );
void g3dui_createVertexTools ( G3DUI *, Dimension, Dimension,
                                        Dimension, Dimension );
void g3dui_addGLMenuButton ( G3DUI *, G3DMOUSETOOL * );

void g3dui_createLeftFrame ( G3DUI *, Dimension, Dimension,
                                      Dimension, Dimension );
void g3dui_createTimeline ( G3DUI *, Dimension, Dimension,
                                     Dimension, Dimension );
void g3dui_createList ( G3DUI *, Dimension, Dimension,
                                 Dimension, Dimension );
void g3dui_createQuad ( G3DUI *, Dimension, Dimension,
                                 Dimension, Dimension );
void g3dui_createTab ( G3DUI *, Dimension, Dimension,
                                Dimension, Dimension );
void g3dui_importMaterials ( G3DUI * );
void gtab_addItem ( Widget, char *, Widget );
void createDefaultViews ( Widget, G3DUI * );
void g3dui_redrawGLViews ( G3DUI * );
void g3dui_redrawList ( G3DUI * );
void g3dui_updateAllKeyEdits ( G3DUI * );
void g3dui_renderViewCbk  ( Widget, XtPointer, XtPointer );

Pixmap createSimplePixmap ( Display *, char **, Pixel );

void g3dui_setHourGlass   ( G3DUI * );
void g3dui_unsetHourGlass ( G3DUI * );

G3DUITIMELINE *g3duitimeline_new ( uint32_t, uint32_t );

G3DUIRENDERSETTINGS *g3duirendersettings_new ( );
void g3duirendersettings_getFfplayPath ( G3DUIRENDERSETTINGS * );
void g3duirendersettings_getFfmpegPath ( G3DUIRENDERSETTINGS * );
void g3duirendersettings_rendertoview ( G3DUIRENDERSETTINGS *, G3DSCENE *,
                                                               G3DCAMERA *,
                                                               uint32_t,
                                                               uint32_t,
                                                               uint32_t,
                                                               uint32_t,
                                                               Widget ); 
void g3duirendersettings_rendertovideo ( G3DUIRENDERSETTINGS *, G3DSCENE *,
                                                                G3DCAMERA *,
                                                                uint32_t,
                                                                Widget );
void g3duirendersettings_rendertoimage ( G3DUIRENDERSETTINGS *, G3DSCENE *,
                                                                G3DCAMERA *,
                                                                uint32_t,
                                                                Widget );
void g3dui_clearMaterials ( G3DUI * );

void g3dui_createModeBar ( G3DUI *, Dimension, Dimension, Dimension, Dimension);
Widget createMaterialBoard ( Widget, G3DUI *, char *,
                                  Dimension, Dimension,
                                  Dimension, Dimension );
Widget g3duiCreateMaterialSettings ( G3DUI *, Widget, char *,
                                     Dimension, Dimension,
                                     Dimension, Dimension );

Widget g3duiRenderWindow_getWorkArea ( Widget );

void updateMaterialSettings ( Widget, G3DMATERIAL * );

G3DOBJECT *selectObject ( Display *, Window, XmFontList, Dimension *, Dimension *,
                          Dimension, G3DOBJECT *, int, int );
void XmGuiOpenG3DFile ( Widget, char * );
G3DSCENE *g3duiOpenG3DFile ( G3DUI *, char * );
void g3duiSaveG3DFile ( G3DUI * );
void g3duiSetFileName ( G3DUI *, char * );

void renderfinalcbk  ( Widget, XtPointer, XtPointer );
void openfilecancelcbk ( Widget , XtPointer, XtPointer );
void openfileokcbk ( Widget , XtPointer, XtPointer );
void savefilecbk ( Widget , XtPointer, XtPointer );
void saveascbk ( Widget , XtPointer, XtPointer );
void savefileasokcbk ( Widget , XtPointer, XtPointer );
void savefileascancelcbk ( Widget , XtPointer, XtPointer );
void overwritefilecancelcbk ( Widget , XtPointer, XtPointer );
void overwritefileokcbk ( Widget , XtPointer, XtPointer );
void untriangulatecbk ( Widget , XtPointer, XtPointer );
void openfilecbk ( Widget , XtPointer, XtPointer );
void newscenecbk ( Widget , XtPointer, XtPointer );
void undocbk ( Widget, XtPointer, XtPointer );
void redocbk ( Widget, XtPointer, XtPointer );

void rendervideocbk ( Widget, XtPointer, XtPointer );
void renderimagecbk ( Widget, XtPointer, XtPointer );

void *g3duirendersettings_rendertovideo_thread ( G3DUI * );

void g3dui_rendertowindow ( G3DUI *, G3DCAMERA *, Display *, Window );
void g3dui_rendervideo    ( G3DUI *, G3DCAMERA * );

void updateSymmetryEdit ( Widget, G3DUI * );
void updatePickToolEdit ( Widget, G3DUI * );
void updateKeyEdit ( Widget, G3DUI * );

void g3dui_setSkinMode   ( Widget, XtPointer, XtPointer );
void g3dui_setUVWMapMode ( Widget, XtPointer, XtPointer );

G3DSCENE *g3duiOpenDAEFile ( G3DUI *, char * );
G3DSCENE *g3duiOpen3DSFile ( G3DUI *, char * );
G3DSCENE *g3duiOpenOBJFile ( G3DUI *, char * );

R3DFILTER *r3dfilter_toWidget_new  ( Widget, uint32_t );
R3DFILTER *r3dfilter_goToFrame_new ( G3DUI * );

void g3duiclipboard_copyObject ( G3DUICLIPBOARD *, G3DSCENE *, LIST *, int, uint32_t );
void g3duiclipboard_paste      ( G3DUICLIPBOARD *, G3DURMANAGER *, G3DSCENE *, G3DOBJECT *, uint32_t );
G3DUICLIPBOARD *g3duiclipboard_new ( );
void g3duiclipboard_clear ( G3DUICLIPBOARD * );

void dispatchMenuButton ( G3DUI *, G3DMOUSETOOL *, uint32_t );
void addMenuListButton  ( G3DUI *, LIST *, G3DMOUSETOOL * );
void addGLMenuButton    ( Widget , G3DUI *, G3DMOUSETOOL * );

void g3duimateriallist_updatePreview ( Widget, G3DUI *, G3DMATERIAL * );

Widget g3duiview_getGLArea ( Widget );

void g3duicom_goToFrame ( G3DUI *, float );
void g3duicom_communicate ( Widget, XtPointer, XEvent *, Boolean * );

#endif
