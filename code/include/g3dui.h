#ifndef _G3DUI_H_
#define _G3DUI_H_

#ifndef True
#define True TRUE
#endif

#ifndef False
#define False FALSE
#endif

/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/******************************************************************************/
#ifdef __linux__
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#endif

/******************************************************************************/
#ifdef __linux__
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
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
#include <GL/glxext.h>
#include <GL/glu.h>
#endif

#ifdef __MINGW32__
#include <windows.h>
#include <winbase.h>
#include <shlwapi.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

/******************************************************************************/
#include <list.h>
#include <g3d.h>
#include <r3d.h>

#ifdef __linux__
#include <r3dnet.h>
#endif

#include <g3durmanager.h>
#include <g3dmouse.h>
#include <g3dcom.h>
#include <g3dexport.h>
#include <g3dimport.h>

/******************************************************************************/
#define G3DUIAPPNAME "Gloss3D"

#define FILEDESC_3DS "3D Studio .3ds"
#define FILEDESC_OBJ "Wavefront .obj"
#define FILEDESC_DAE "Collada .dae"
#define FILEDESC_C4D "Cinema4D .c4d"
#define FILEDESC_STA "Ascii STL .stl"

#define OBJECTMENUNAME  "Object Menu"
#define VERTEXMENUNAME  "Vertex Menu"
#define EDGEMENUNAME    "Edge Menu"
#define FACEMENUNAME    "Face Menu"
#define SCULPTMENUNAME  "Sculpt Menu"
#define OPTIONMENUNAME  "Option_Menu"
#define SHADINGMENUNAME "Shading_Menu"

#define SHADINGMENU_GOURAUD   "Gouraud Shading"
#define SHADINGMENU_FLAT      "Flat Shading"
#define SHADINGMENU_WIREFRAME "Wireframe"

#define TOOLBARNAME       "TOOLBAR"
#define MODEBARNAME       "MODEBAR"
#define MENUBARNAME       "MENUBAR"
#define CENTRALTABNAME    "CENTRALTAB"
#define QUADNAME          "QUAD"
#define TIMEBOARDNAME     "TIMEBOARD"
#define TIMELINENAME      "TIMELINE"
#define OBJECTBOARDNAME   "Object Board"
#define BOTTOMBOARDNAME   "BOTTOMBOARD"
#define MAINBOARDNAME     "MAINBOARD"
#define MATERIALBOARDNAME "Material Board"

#define RENDERWINDOWMENUBARNAME "RENDERWINDOWMENUBAR"
#define RENDERWINDOWMENUWORKAREANAME "RENDERWINDOWMENUWORKAREA"

/*
#define G3DFONT     "-*-lucida-medium-r-*-*-12-*-*-*-*-*-iso8859-1"
#define G3DTINYFONT "-*-lucida-medium-r-*-*-08-*-*-*-*-*-iso8859-1"
#define G3DBOLDFONT "-*-lucida-bold-r-*-*-12-*-*-*-*-*-iso8859-1"
*/

#define G3DFONT     "-*-helvetica-medium-r-normal-*-12-120-*-*-*-*-iso8859-1"
#define G3DTINYFONT "-*-helvetica-medium-r-normal-*-08-120-*-*-*-*-iso8859-1"
#define G3DBOLDFONT "-*-helvetica-medium-r-normal-*-12-120-*-*-*-*-iso8859-1"

#define BACKGROUNDCOLOR "gray80"
#define FOREGROUNDCOLOR "gray80"
#define ARMEDCOLOR      "gray20"

#define TOOLBARBUTTONSIZE 0x20
#define MODEBARBUTTONSIZE 0x30
#define OBJTOOLBUTTONSIZE 0x20
#define VERTOOLBUTTONSIZE 0x20
#define FACTOOLBUTTONSIZE 0x20

#define GLVIEWGOURAUD    0x00 /*** Default viewing mode ***/
#define GLVIEWFLAT       0x01
#define GLVIEWWIREFRAME  0x02

/*** object extra flags - uses the last 4 bits ***/
#define OBJECTCOLLAPSED ( 1 << 28 )

#define OBJECTMODETOOL ( 1      )
#define VERTEXMODETOOL ( 1 << 1 )
#define FACEMODETOOL   ( 1 << 2 )
#define EDGEMODETOOL   ( 1 << 3 )
#define GLMENUTOOL     ( 1 << 4 )
#define SCULPTMODETOOL ( 1 << 5 )

#define EDITGLVIEWMODE   "Display Mode"
#define EDITGLVIEWCAMERA "Use camera"

/**** Widget names for ObjectEdit TextField widgets ***/
#define EDITOBJECT     "Edit Object"
#define EDITOBJECTNAME "Object Name"

#define EDITXPOSITION "EDITXPOSITION"
#define EDITYPOSITION "EDITYPOSITION"
#define EDITZPOSITION "EDITZPOSITION"
#define EDITXROTATION "EDITXROTATION"
#define EDITYROTATION "EDITYROTATION"
#define EDITZROTATION "EDITZROTATION"
#define EDITXSCALING  "EDITXSCALING"
#define EDITYSCALING  "EDITYSCALING"
#define EDITZSCALING  "EDITZSCALING"

/*************************** Key Edit Widget constants ************************/
#define EDITKEYPOSITION  "Position"
#define EDITKEYXPOSITION "EDITKEYXPOSITION"
#define EDITKEYYPOSITION "EDITKEYYPOSITION"
#define EDITKEYZPOSITION "EDITKEYZPOSITION"

#define EDITKEYROTATION  "Rotation"
#define EDITKEYXROTATION "EDITKEYXROTATION"
#define EDITKEYYROTATION "EDITKEYYROTATION"
#define EDITKEYZROTATION "EDITKEYZROTATION"

#define EDITKEYSCALING   "Scaling"
#define EDITKEYXSCALING  "EDITKEYXSCALING"
#define EDITKEYYSCALING  "EDITKEYYSCALING"
#define EDITKEYZSCALING  "EDITKEYZSCALING"

#define EDITKEYLOOP      "Loop from frame"
#define EDITKEYLOOPFRAME "EDITKEYLOOPFRAME"

/******************************************************************************/

#define EDITRENDERSTART      "From Frame"
#define EDITRENDEREND        "To Frame"
#define EDITRENDERFPS        "Framerate"
#define EDITRENDEROUTPUT     "Output file"
#define EDITRENDERFORMAT     "Format"
#define EDITRENDERWIDTH      "Render width"
#define EDITRENDERHEIGHT     "Render height"
#define EDITRENDERRATIO      "Aspect ratio"
#define EDITRENDERMBLUR      "MotionBlur(iter)"
#define EDITRENDERBACKGROUND "Background"
#define EDITRENDERPREVIEW    "Make preview"
#define EDITRENDERRUN        "Run render"
#define EDITRENDERSAVE       "Save result"

#define EDITTEXTUREMAPPING   "Choose UVW Map"

/**** Widget names for SphereEdit TextField widgets ***/
#define EDITSPHERE       "Edit Sphere"
#define EDITSPHERECAPS   "Caps"
#define EDITSPHERESLICES "Slices"
#define EDITSPHERERADIUS "Radius"

/**** Widget names for CubeEdit TextField widget ***/
#define EDITCUBE        "Edit Cube"
#define EDITCUBEXSLICES "X Slices"
#define EDITCUBEYSLICES "Y Slices"
#define EDITCUBEZSLICES "Z Slices"
#define EDITCUBERADIUS  "Radius"

#define EDITTORUS            "Edit Torus"
#define EDITTORUSSLICE       "Slices"
#define EDITTORUSCAP         "Caps"
#define EDITTORUSEXTRAD      "Outer Radius"
#define EDITTORUSINTRAD      "Inner Radius"
#define EDITTORUSORIENTATION "Orientation"

#define EDITPLANE     "Edit Plane"
#define EDITPLANEDIVU "U Divisions"
#define EDITPLANEDIVV "V Divisions"
#define EDITPLANERADU "U Radius"
#define EDITPLANERADV "V Radius"
#define EDITPLANEORIENTATION "Orientation"

#define EDITFFD         "Edit FFD"
#define EDITFFDXSLICES  "X Slices"
#define EDITFFDYSLICES  "Y Slices"
#define EDITFFDZSLICES  "Z Slices"
#define EDITFFDXRADIUS  "X Radius"
#define EDITFFDYRADIUS  "Y Radius"
#define EDITFFDZRADIUS  "Z Radius"

/**** Widget names for CylinderEdit TextField widget ***/
#define EDITCYLINDER       "Edit Cylinder"
#define EDITCYLINDERXCAPS  "X Caps"
#define EDITCYLINDERYCAPS  "Y Caps" 
#define EDITCYLINDERSLICES "Slices"
#define EDITCYLINDERLENGTH "Length"
#define EDITCYLINDERRADIUS "Radius"

/**** Widget names for Pick Tool widget ***/
#define EDITPICKTOOLVISIBLE        "Only visible"
#define EDITPICKTOOLWEIGHTFRAME    "Weight Painting"
#define EDITPICKTOOLWEIGHTSTRENGTH "Strength"
#define EDITPICKTOOLWEIGHTRADIUS   "Radius"

/**** Widget names for Cut Mesh Tool widget ***/
#define EDITCUTMESHTOOLRESTRICT    "Restrict to selection"

/**** Widget names for MeshEdit TextField widget ***/
#define EDITMESH            "Mesh"
#define EDITMESHSUBDIVISION "Subdivision"
#define EDITMESHWEIGHTGROUP "Weight Groups"
#define EDITMESHSUBDIVMODE  "Buffered Subdivision (Faster)"
#define EDITMESHSUBDIVVLAB  "Editing level"
#define EDITMESHSUBDIVVIEW  "Subdivision (view)"
#define EDITMESHSUBDIVSYNC  "Sync"
#define EDITMESHSUBDIVRLAB  "Render level"
#define EDITMESHSUBDIVREND  "Subdivision (render)"
#define EDITMESHADAPTIVE    "Adaptive mode (Experimental)"
#define EDITMESHADVANGLE    "Limit Angle"
#define EDITMESHISOLINES    "Use isoparms (Slower)"

/**** Widget names for BoneEdit TextField widget ***/
#define EDITBONE            "Bone"
#define EDITBONEVERTEXGROUP "Vertex Groups (Double-Click to link)"

/**** Widget names for LightEdit TextField widget ***/
#define EDITLIGHT            "Light"
#define EDITLIGHTINTENSITY   "Intensity"
#define EDITLIGHTCOLOR       "Light RGB Color"
#define EDITLIGHTSPECULARITY "Light RGB Specularity"

#define EDITSYMMETRY      "Symmetry"
#define EDITSYMMETRYPLANE "Symmetry Plane"
#define EDITSYMMETRYLIMIT "Merge limit"

#define EDITWEIGHTGROUPNAME "Group name"
#define EDITWEIGHTGROUPLIST "Group list"

/**** Widget names for MaterialColorEdit widget ***/
#define MATERIALCOLOREDITFORMNAME "Material Color Edit Form"

#define ADDWEIGHTSTR      "Add weight"
#define REMOVEWEIGHTSTR   "Remove weight"
#define RELATIVEWEIGHTSTR "Relative"
#define ABSOLUTEWEIGHTSTR "Absolute"

#define EDITWEIGHTRADIUS  "Paint Radius"
#define EDITWEIGHTVALUE   "Weight Value"

#define EDITMATERIALNAME           "Material Name"
#define EDITMATERIALCHANNELS       "Material Channels"

#define EDITDIFFUSE                "Diffuse"
#define EDITDIFFUSECOLOR           "Color"
#define EDITDIFFUSEIMAGE           "Image"

#define EDITSPECULARITY            "Specularity"
#define EDITSPECULARITYSHININESS   "Shininess"
#define EDITSPECULARITYLEVEL       "Level"
#define EDITSPECULARITYCOLOR       "Color"
#define EDITSPECULARITYIMAGE       "Image"

#define EDITDISPLACEMENT           "Displacement"
#define EDITDISPLACEMENTENABLED    "Enable Displacement Channel"
#define EDITDISPLACEMENTCHANNEL    "Displacement Channel"
#define EDITDISPLACEMENTIMAGE      "Image"
#define EDITDISPLACEMENTSTRENGTH   "Strength"

#define EDITREFLECTION             "Reflection"
#define EDITREFLECTIONSTRENGTH     "Strength"

#define EDITREFRACTION             "Refraction"
#define EDITREFRACTIONTRANSPARENCY "Transparency"
#define EDITREFRACTIONSTRENGTH     "Strength"

#define EDITCOLORIMAGE             "Color Image"
#define EDITCOLORSOLID             "Solid Color"
#define EDITCOLORPROCEDURAL        "Procedural Texture"

#define EDITUVMAP             "UVMap"
#define EDITUVMAPPROJECTION   "Projection"
#define EDITUVMAPFIXED        "Fixed"
#define CYLINDRICALPROJECTION "Cylindrical"
#define SPHERICALPROJECTION   "Spherical"
#define FLATPROJECTION        "Flat"

#define MODE_VIEWOBJECT "Object mode"
#define MODE_VIEWVERTEX "Vertex mode"
#define MODE_VIEWEDGE   "Edge mode"
#define MODE_VIEWFACE   "Face mode"
#define MODE_VIEWSKIN   "Skin mode"
#define MODE_VIEWSCULPT "Sculpt mode"
#define MODE_VIEWUVWMAP "UVW mode"

#define MENU_XAXIS "X axis"
#define MENU_YAXIS "Y axis"
#define MENU_ZAXIS "Z axis"

#define MENU_NEWSCENE    "New scene"
#define MENU_OPENFILE    "Open file"
#define MENU_IMPORTFILE  "_Import"
#define MENU_SAVEFILE    "Save"
#define MENU_SAVEFILEAS  "Save as"
#define MENU_EXPORTSCENE "_Export"
#define MENU_EXIT        "Exit"

#define MENU_UNDO                 "Undo"
#define MENU_REDO                 "Redo"
#define MENU_DELETE               "Delete"
#define MENU_INVERTNORMALS        "Invert Normals"
#define MENU_ALIGNNORMALS         "Align Faces"
#define MENU_TRIANGULATE          "Triangulate"
#define MENU_OPTION_CLOCKWISE     "Clockwise"
#define MENU_OPTION_ANTICLOCKWISE "Anti Clockwise"
#define MENU_TRIANGULATE          "Triangulate"
#define MENU_UNTRIANGULATE        "Untriangulate"
#define MENU_WELDVERTICES         "Weld Selected Vertices"
#define MENU_DELETELONEVERTICES   "Delete Lone Vertices"
#define MENU_CONVERTSYMMETRY      "Convert Symmetry"
#define MENU_SELECTTREE           "Select Tree"
#define MENU_OPTION_ALLTYPES      "All Types"
#define MENU_OPTION_SAMETYPE      "Same Type"
#define MENU_INVERTSELECTION      "Invert Selection"

#define MENU_ADDPLANE             "Add Plane"
#define MENU_ADDCUBE              "Add Cube"
#define MENU_ADDSPHERE            "Add Sphere"
#define MENU_ADDTORUS             "Add Torus"
#define MENU_ADDCYLINDER          "Add Cylinder"
#define MENU_ADDLIGHT             "Add Light"
#define MENU_ADDCAMERA            "Add Camera"
#define MENU_ADDEMPTYMESH         "Add Empty Mesh"

#define MENU_ADDBONE              "Add Bone"
#define MENU_ADDSYMMETRY          "Add Symmetry"
#define MENU_ADDFFDBOX            "Add FFD Box"
#define MENU_ASSIGNFFD            "Assign FFD"
#define MENU_RESETFFD             "Reset FFD"
#define MENU_FIXBONE              "Fix Bone"
#define MENU_FIXBONETREE          "Whole tree"
#define MENU_FIXBONEONLY          "This bone only"
#define MENU_RESETBONE            "Reset Bone"
#define MENU_RESETBONETREE        "Whole tree"
#define MENU_RESETBONEONLY        "This bone only"

#define MENU_ADDUVWMAP            "Add UVW Map"

#define MENU_MIRRORWEIGHTGROUP    "Mirror WeightGroup"
#define MENU_MIRRORXY             "Along XY plane"
#define MENU_MIRRORYZ             "Along YZ plane"
#define MENU_MIRRORZX             "Along ZX plane"
#define MENU_SPLITMESH            "Split Mesh"
#define MENU_SPLITANDKEEP         "Split and keep"
#define MENU_SPLITANDREMOVE       "Split and remove"
#define MENU_MAKEEDITABLE         "Make editable"
#define MENU_MERGEMESH            "Merge Meshes"

#define MENU_RENDERVIEW           "Render view"
#define MENU_RENDERFINAL          "Render Final"
#define MENU_RENDERSETTINGS       "Render settings"

#define MENU_ADDUVMAP             "Add UV Map"
#define MENU_FITUVMAP             "Fit to object"
#define MENU_ALIGNUVMAP           "Align UV Map"
#define MENU_ALIGNUVMAPXY         "Along XY plane"
#define MENU_ALIGNUVMAPYZ         "Along YZ plane"
#define MENU_ALIGNUVMAPZX         "Along ZX plane"

/******************************************************************************/
#define MARGIN 0x02
#define MARGDB ( MARGIN << 0x01 )

/******************************************************************************/
/*** The Copy-Paste manager ***/
#define CLIPBOARDEMPTY      0x00
#define CLIPBOARDCOPYOBJECT 0x01
#define CLIPBOARDCOPYKEY    0x02

typedef enum _G3DUIAXIS {
    G3DUIXAXIS,
    G3DUIYAXIS,
    G3DUIZAXIS,
    G3DUIUAXIS,
    G3DUIVAXIS
} G3DUIAXIS;

typedef struct _G3DUICOPIEDITEM {
    G3DOBJECT *obj;
    G3DKEY    *key;
    double wmatrix[0x10]; /*** We nee the world matrix for parent swapping ***/
} G3DUICOPIEDITEM;

typedef struct _G3DUICLIPBOARD {
    uint32_t operation;
    G3DSCENE  *sce;
    LIST     *lcpy; /*** list of copied objects ***/
    LIST     *lkey;
} G3DUICLIPBOARD;

/******************************************************************************/
typedef struct _G3DUITIMELINE {
    uint32_t fps; /*** frame per second ***/
    uint32_t nbframes;
    int32_t startframe;
    int32_t endframe;
    int32_t curframe;
} G3DUITIMELINE;

/******************************************************************************/
typedef struct _G3DUICONF {
    uint32_t undolevel;
} G3DUICONF;

/******************************************************************************/
#define RENDERTOIMAGE 0x00
#define RENDERTOVIDEO 0x01

#define RENDERTOIMAGENAME "Render to image"
#define RENDERTOVIDEONAME "Render to video"

#define FFMPEGPATHLEN 0x200

/*** G3DUIRENDERSETTINGS flags ***/
#define RENDERPREVIEW ( 1      )
#define RENDERSAVE    ( 1 << 1 )

typedef struct _G3DUIRENDERSETTINGS {
    uint32_t background;
    uint32_t flags;
    uint32_t fps; /*** frame per second ***/
    uint32_t depth;
    uint32_t mblur; /*** motion blur iterations ***/
    int32_t  startframe;
    int32_t  endframe;
    char    *outfile;
    uint32_t format;
    uint32_t width, height;
    float    ratio;
    char     ffmpegpath[FFMPEGPATHLEN];
    char     ffplaypath[FFMPEGPATHLEN];
    LIST    *lfilter;
    int pipefd[0x02];
} G3DUIRENDERSETTINGS;

/****************************** g3duirendersettings.c *************************/
G3DUIRENDERSETTINGS *g3duirendersettings_new ( );

/******************************************************************************/
/*** this structure is passed to the Drawing Area widget, so we can save the***/
/*** render as jpeg reading the data stored in the filter or stopping the ***/
/*** render thread ***/
typedef struct _G3DUIRENDERPROCESS {
    uint64_t id;
    R3DSCENE  *rsce;
    /*R3DFILTER *filter_to_window;*/ /*** this contains final XImage structure ***/
    /*R3DFILTER *filter_to_buffer;*/ /*** this contains raw image data ***/
    char  *filename;
} G3DUIRENDERPROCESS;

/****************************** g3duirenderprocess.c **************************/
G3DUIRENDERPROCESS *g3duirenderprocess_new      ( uint64_t, R3DSCENE *,
                                                            R3DFILTER *,
                                                            R3DFILTER * );
void                g3duirenderprocess_free     ( G3DUIRENDERPROCESS * );
void                g3duirenderprocess_filename ( G3DUIRENDERPROCESS *, char * );
void                g3duirenderprocess_savejpg  ( G3DUIRENDERPROCESS *, char * );

/******************************************************************************/
/*** This structure is passed when confirming the subdivision level after a ***/
/*** warning window has been raised against the amount of required memory   ***/
typedef struct _G3DUICONFIRMSUBDIV {
    G3DMESH *mes;
    uint32_t level;
} G3DUICONFIRMSUBDIV;

/********************* a "portable" version of XRectangle *********************/
typedef struct _G3DUIRECTANGLE {
    short x, y;
    unsigned short width, height; 
} G3DUIRECTANGLE; 

/********************* a "portable" version of XSegment *********************/
typedef struct _G3DUISEGMENT {
    short x1, x2, y1, y2;
} G3DUISEGMENT; 

/********************************* G3DUI **************************************/
typedef struct _G3DUI {
    G3DMOUSETOOL *mou; /*** current mouse tool ***/
    /*G3DUITIMELINE *tim;*/
    G3DUICLIPBOARD *cli;
    G3DSCENE     *sce;
    G3DUICONF     conf;
    uint32_t      flags;
    LIST         *lobjmenu;
    LIST         *lvermenu;
    LIST         *ledgmenu;
    LIST         *lfacmenu;
    LIST         *lscpmenu;
    LIST         *lmtools; /*** list of mousetools widget ***/
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
    LIST         *limg; /*** List of images (among them are textures) ***/
    LIST         *lrps; /*** list of render process ***/
    LIST *lmou; /*** list of mousetools ***/
    G3DMATERIAL   *selmat;
    G3DURMANAGER  *urm;
    /*R3DNETSERVER  *srv;
    R3DNETCLIENT  *clt;*/
    G3DUIRECTANGLE menurec;
    G3DUIRECTANGLE tbarrec;
    G3DUIRECTANGLE mbarrec;
    G3DUIRECTANGLE mbrdrec;
    G3DUIRECTANGLE proprec;
    G3DUIRECTANGLE quadrec;
    G3DUIRECTANGLE timerec;
    G3DUIRECTANGLE inforec;
    G3DCAMERA     *defcam;
    G3DCAMERA     *curcam;
#ifdef __linux__
    pthread_t      playthreadid;
#endif
#ifdef __MINGW32__
    HANDLE         playthreadid;
#endif
    LIST *lrsce;
    LIST *lrsg; /*** list of render settings ***/
    G3DUIRENDERSETTINGS *currsg; /*** current render settings ***/
    char *filename;
    int lock; /*** I use this for preventing loops on XmText fields ***/
    char *loadFile;
    void *toolkit_data; /*** Toolkit related data that cant be shared ***/
    float startframe;
    float curframe;   /*** used by g3duitimeboard.c/play_t() ***/
    float endframe;
    int32_t fps;
    uint32_t playLock;
} GuiPart, G3DUI;

/********************************* g3dui.c ************************************/
G3DCAMERA   **common_g3dui_createDefaultCameras ( G3DUI * );
void          common_g3dui_initDefaultMouseTools ( G3DUI * );
void          common_g3dui_addMouseTool ( G3DUI *, G3DMOUSETOOL *, uint32_t );
G3DMOUSETOOL *common_g3dui_getMouseTool ( G3DUI *, const char * );

/*************************** Quad Widget Structure ****************************/
typedef struct _G3DUIQUAD {
    uint32_t      Xpos;   /*** Glider X position                            ***/
    uint32_t      Ypos;   /*** Glider Y position                            ***/
    /*** The variables below will allow us to keep a coorect Glider         ***/
    /*** position while resizing the entire QUAD                            ***/
    uint32_t      Xratio; /*** Glider Relative X position (Fixed-Point Math)***/
    uint32_t      Yratio; /*** Glider Relative Y position (Fixed-Point Math)***/
    G3DUISEGMENT  seg[5]; /*** there are 4 3D windows in our modeler        ***/
                          /*** The fith one is the maximized one            ***/
} G3DUIQUAD;

/******************************* g3duiquad.c **********************************/
void common_g3duiquad_createSegments ( G3DUIQUAD *, uint32_t, uint32_t );
void common_g3duiquad_divideSegments ( G3DUISEGMENT *, uint32_t, uint32_t, 
                                                       uint32_t, uint32_t );
void common_g3duiquad_resize         ( G3DUIQUAD *, uint32_t, uint32_t );
void common_g3duiquad_init           ( G3DUIQUAD *, uint32_t, uint32_t );



/************************** View Widget Structure *****************************/
#define NBVIEWBUTTON    0x04
#define BUTTONSIZE      0x10 /*** 16x16 ***/

#define MAXIMIZEBUTTON  0x00
#define ROTATEBUTTON    0x01
#define TRANSLATEBUTTON 0x02
#define ZOOMBUTTON      0x03

typedef struct _G3DUIVIEW {
    G3DUIRECTANGLE rec[NBVIEWBUTTON];       /*** pixmaps position ***/
    G3DUIRECTANGLE glrec;
    /*Pixmap         curpix[NBVIEWBUTTON];
    Pixmap         norpix[NBVIEWBUTTON];
    Pixmap         armpix[NBVIEWBUTTON];*/
    int32_t        buttonID; /**** Currently clicked button = -1 if none ***/
    G3DCAMERA     *cam;
    G3DCAMERA     *defcam;
    G3DVECTOR      defcampos; /*** Default camera position ***/
    G3DVECTOR      defcamrot; /*** Default camera rotation ***/
    G3DVECTOR      defcamsca; /*** Default camera scaling  ***/
    float          defcamfoc; /*** Default camera focal    ***/
    LIST          *lmenu;
    /*Widget ogl;*/ /*** OpenGL Widget ***/
    uint32_t       mode;   /*** wireframe, flat, fill ***/
    uint32_t       flags;
#ifdef __linux__
    GLXContext     glctx;
    pthread_t      render_tid; /*** current rendering thread - Used for canceling***/
#endif
#ifdef __MINGW32__
    HGLRC          glctx;
    HANDLE         render_tid;
#endif
} G3DUIVIEW;

/******************************* g3duiview.c **********************************/
void common_g3duiview_resize ( G3DUIVIEW *, uint32_t, uint32_t );
void common_g3duiview_init   ( G3DUIVIEW *, uint32_t, uint32_t );
                   /**** Drawing Area callbacks ***/
void common_g3duiview_sizeGL ( G3DUIVIEW *, uint32_t, uint32_t );

void common_g3duiview_initGL ( G3DUIVIEW * );

void common_g3duiview_showGL ( G3DUI *, G3DSCENE *, G3DCAMERA *, 
                                                    G3DMOUSETOOL *,
                                                        uint32_t,
                                                        uint32_t );
int  common_g3duiview_getCurrentButton ( G3DUIVIEW *, int, int );
void common_g3duiview_moveForward  ( G3DUIVIEW *, int32_t, int32_t );
void common_g3duiview_moveSideward ( G3DUIVIEW *, int32_t, int32_t, 
                                                  int32_t, int32_t );
void common_g3duiview_zoom         ( G3DUIVIEW *, int32_t, int32_t );
void common_g3duiview_spin         ( G3DUIVIEW *, int32_t, int32_t );
void common_g3duiview_orbit        ( G3DUIVIEW *, G3DPIVOT *,
                                                  int32_t, int32_t,
                                                  int32_t, int32_t );

/*************************** List widget Structures ***************************/
#define LISTINDENT 0x10
#define GETLISTSEPARATORS (  1         )
#define GETLISTMAXWIDTH   (  1  <<  1  )

#define EXPANDRECTHIT  0x01
#define ICONRECTHIT    0x02
#define NAMERECTHIT    0x03
#define ACTIVERECTHIT  0x04
#define VISIBLERECTHIT 0x05
#define TEXTURERECTHIT 0x06

#define MAXTEXTURES    0x20 /*** 32 textures at most - will be improved ***/

#define PICKEDOBJECTNOPARSE (  1         )
#define PICKEOBJECTKEEPALL  (  1  <<  1  )

/******************************************************************************/
typedef struct _LISTEDOBJECT {
    G3DUIRECTANGLE expand;
    G3DUIRECTANGLE icon;
    G3DUIRECTANGLE name;
    G3DUIRECTANGLE active;
    G3DUIRECTANGLE visible;
    G3DOBJECT     *obj;
    uint32_t       hit;
    uint32_t       endx;
    G3DUIRECTANGLE texture[MAXTEXTURES];
} LISTEDOBJECT;

/******************************************************************************/
typedef struct _PICKEDOBJECT {
    G3DOBJECT  *obj;
    G3DTEXTURE *tex;
    uint32_t picked; 
} PICKEDOBJECT;

PICKEDOBJECT *pickobject ( uint32_t, uint32_t,
                                     uint32_t, 
                                     uint32_t,
                                     uint32_t,
                                     uint32_t,
                                     G3DOBJECT *,
                                     G3DSCENE *,
                                     G3DURMANAGER *,
                                     uint32_t,
                                     uint32_t );

uint32_t listedObject_sizeInit ( LISTEDOBJECT *, uint32_t, uint32_t, uint32_t );
LISTEDOBJECT *common_g3duilist_sizeListedObject ( G3DOBJECT *, uint32_t, 
                                                        uint32_t,
                                                        uint32_t,
                                                        uint32_t,
                                                        uint32_t );



/****************************** Material Previews *****************************/
/******************************************************************************/
#define VECNORMMAX 1.001f
#define VECNORMMIN 0.999f
/******************************************************************************/
typedef struct _G3DUIMATERIALPIXEL {
    uint32_t used;
    G3DVECTOR pos;
    G3DVECTOR nor;
    float diff;
    float spec; /*** specular reflection dot product ***/
    float u, v;
} G3DUIMATERIALPIXEL;

/******************************************************************************/
typedef struct _G3DUIMATERIALMAP {
    G3DUIMATERIALPIXEL *pixel;
    uint32_t width;
    uint32_t height;
} G3DUIMATERIALMAP;

/******************************************************************************/
typedef struct _MATERIALLISTDATA {
    /*** List of material previews (those are ToolKit related) ***/
    LIST *lpreview;
    /*** The vector map that helps us to build the preview sphere ***/
    G3DUIMATERIALMAP *matmap;
    uint32_t preview_width;
    uint32_t preview_height;
    uint32_t preview_border;
    uint32_t preview_name_height;
} MATERIALLISTDATA;

/******************************************************************************/
G3DUIMATERIALMAP *common_g3duimaterialmap_new         ( uint32_t, uint32_t );
void              common_g3duimaterialmap_buildSphere ( G3DUIMATERIALMAP *, 
                                                        uint32_t, 
                                                        uint32_t,
                                                        float );
void              common_g3duimaterialmap_fillData    ( G3DUIMATERIALMAP *,
                                                        G3DMATERIAL *,
                                                        unsigned char (*)[3] );
MATERIALLISTDATA *common_materiallistdata_new ( uint32_t, uint32_t );

/******************************************************************************/
/******************************************************************************/


/********************************** Timelines *********************************/
/******************************************************************************/
#define TIMEBOARDPLAY   "TIMEBOARDPLAY"
#define TIMEBOARDSTOP   "TIMEBOARDSTOP"
#define TIMEBOARDNEXT   "TIMEBOARDNEXT"
#define TIMEBOARDPREV   "TIMEBOARDPREV"
#define TIMEBOARDRECORD "TIMEBOARDRECORD"
#define TIMEBOARDZOOM   "TIMEBOARDZOOM"
#define TIMEBOARDUNZOOM "TIMEBOARDUNZOOM"

#define DEFAULTFRAMEGAP 0x10
#define MINIMUMFRAMEGAP 0x04
#define MAXIMUMFRAMEGAP 0x20

typedef struct _TIMELINEDATA {
    int32_t midframe; /*** the frame in the middle of the widget     ***/
    uint32_t nbpix;   /*** Number of pixels between frame indicators ***/
} TIMELINEDATA;

/******************************************************************************/
TIMELINEDATA *common_timelinedata_new ( );
uint32_t      common_timelinedata_onFrame     ( TIMELINEDATA *, float,
                                                                int,
                                                                int,
                                                                int );
int32_t       common_timelinedata_getFrame    ( TIMELINEDATA *, int, int, int );
int32_t       common_timelinedata_getFramePos ( TIMELINEDATA *, float, int );
void          common_timelinedata_deleteKey   ( TIMELINEDATA *, LIST * );
uint32_t      common_timelinedata_selectKey   ( TIMELINEDATA *, LIST *, 
                                                                int,
                                                                int,
                                                                int );

/******************************************************************************/
/******************************************************************************/

/******************************* ClipBoard ************************************/
G3DUICOPIEDITEM *g3duicopieditem_new   ( G3DOBJECT *, G3DKEY *, uint32_t );
void             g3duicopieditem_clear ( G3DUICOPIEDITEM *, uint32_t );
void             g3duicopieditem_free  ( G3DUICOPIEDITEM * );

G3DUICLIPBOARD *g3duiclipboard_new        ( );
void            g3duiclipboard_clear      ( G3DUICLIPBOARD * );
void            g3duiclipboard_free       ( G3DUICLIPBOARD * );
void            g3duiclipboard_clear      ( G3DUICLIPBOARD * );
void            g3duiclipboard_copyObject ( G3DUICLIPBOARD *, G3DSCENE *,
                                                              LIST *,
                                                              int, 
                                                              uint32_t );
void            g3duiclipboard_paste      ( G3DUICLIPBOARD *, G3DURMANAGER *,
                                                              G3DSCENE *,
                                                              G3DOBJECT *,
                                                              uint32_t );
void            g3duiclipboard_copyKey    ( G3DUICLIPBOARD *, G3DSCENE *,
                                                              G3DOBJECT *,
                                                              LIST * );

/******************************************************************************/
G3DSCENE *common_g3dui_openG3DFile          ( G3DUI *, const char * );
void      common_g3dui_setMouseTool         ( G3DUI *, G3DMOUSETOOL * );
void      common_g3dui_saveG3DFile          ( G3DUI * );
void      common_g3dui_setFileName          ( G3DUI *, const char * );
void      common_g3dui_resizeWidget         ( G3DUI *, uint32_t, 
                                                       uint32_t );
void      common_g3dui_setMode              ( G3DUI *, const char * );
void      common_g3dui_dispatchGLMenuButton ( G3DUI *, G3DMOUSETOOL *,
                                                       uint32_t );
void      common_g3dui_interpretMouseToolReturnFlags ( G3DUI *, uint32_t );

/****************************** Callbacks *************************************/
void      common_g3dui_deleteSelectionCbk     ( G3DUI * );
void      common_g3dui_copySelectionCbk       ( G3DUI * );
void      common_g3dui_pasteSelectionCbk      ( G3DUI * );
void      common_g3dui_makeEditableCbk        ( G3DUI * );
void      common_g3dui_undoCbk                ( G3DUI * );
void      common_g3dui_redoCbk                ( G3DUI * );
G3DSCENE *common_g3dui_importfileokcbk        ( G3DUI *, const char *,
                                                         const char * );
void      common_g3dui_exportfileokcbk        ( G3DUI *, const char *, 
                                                         const char * );
void      common_g3dui_exitokcbk              ( G3DUI * );
void      common_g3dui_triangulateCbk         ( G3DUI *, const char * );
void      common_g3dui_selectTreeCbk          ( G3DUI *, const char * );
void      common_g3dui_addEmptyMeshCbk        ( G3DUI * );
void      common_g3dui_addCameraCbk           ( G3DUI * );
void      common_g3dui_addLightCbk            ( G3DUI * );
void      common_g3dui_addCylinderCbk         ( G3DUI * );
void      common_g3dui_addTorusCbk            ( G3DUI * );
void      common_g3dui_addSphereCbk           ( G3DUI * );
void      common_g3dui_addCubeCbk             ( G3DUI * );
void      common_g3dui_addPlaneCbk            ( G3DUI * );
void      common_g3dui_addBoneCbk             ( G3DUI * );
void      common_g3dui_resetFFDBoxCbk         ( G3DUI * );
void      common_g3dui_assignFFDBoxCbk        ( G3DUI * );
void      common_g3dui_addFFDBoxCbk           ( G3DUI * );
void      common_g3dui_addSymmetryCbk         ( G3DUI * );
void      common_g3dui_resetBoneTreeCbk       ( G3DUI * );
void      common_g3dui_resetBoneCbk           ( G3DUI * );
void      common_g3dui_fixBoneTreeCbk         ( G3DUI * );
void      common_g3dui_fixBoneCbk             ( G3DUI * );
void      common_g3dui_mirrorWeightGroupCbk   ( G3DUI *, const char * );
void      common_g3dui_setMaterialCbk         ( G3DUI * );

void      common_g3duimenubar_addUVMapCbk     ( G3DUI * );
void      common_g3duimenubar_alignUVMapCbk   ( G3DUI *, const char * );
void      common_g3duimenubar_fitUVMapCbk     ( G3DUI * );

/************************** g3duimaterialedit.c *******************************/
void common_g3dui_materialSolidColorCbk           ( G3DUI * );
void common_g3dui_materialImageColorCbk           ( G3DUI * );
void common_g3dui_materialColorChangeCbk          ( G3DUI *, G3DCHANNEL *,
                                                             uint32_t ,
                                                             uint32_t ,
                                                             uint32_t );
void common_g3dui_materialChooseImageCbk          ( G3DUI *, char * );
void common_g3dui_materialSpecularityShininessCbk ( G3DUI *, float );
void common_g3dui_materialSpecularityColorCbk     ( G3DUI *, uint32_t,
                                                             uint32_t,
                                                             uint32_t );
void common_g3dui_materialNameCbk                 ( G3DUI *, const char * );
void common_g3dui_materialSpecularityLevelCbk     ( G3DUI *, float );
void common_g3dui_materialDisplacementStrengthCbk ( G3DUI *, float );
void common_g3dui_materialDisplacementImageCbk    ( G3DUI *, const char * );
void common_g3dui_materialDisplacementToggleCbk   ( G3DUI * );
void common_g3dui_materialReflectionStrengthCbk   ( G3DUI *, float );
void common_g3dui_materialRefractionStrengthCbk   ( G3DUI *, float );
void common_g3dui_materialTransparencyStrengthCbk ( G3DUI *, float );

/******************************** g3duitimeline.c *****************************/
void common_g3dui_recordFrameCbk ( G3DUI *, uint32_t );
void common_g3dui_gotoFrameCbk   ( G3DUI * );
void common_g3dui_playCbk        ( G3DUI * );
void common_g3dui_stopCbk        ( G3DUI * );
void common_g3dui_animCbk        ( G3DUI * );

/******************************* Key Edit *************************************/
void common_g3duikeyedit_keyCbk       ( G3DUI *, const char *, float );
void common_g3duikeyedit_loopCbk      ( G3DUI *, uint32_t );
void common_g3duikeyedit_loopFrameCbk ( G3DUI *, float );
void common_g3duikeyedit_setFlagCbk   ( G3DUI *, uint32_t );
void common_g3duikeyedit_unsetFlagCbk ( G3DUI *, uint32_t );

/****************************** Object Edit ***********************************/
void common_g3duiobjectedit_nameCbk ( G3DUI *, const char * );

/****************************** Plane Edit ************************************/
void common_g3duiplaneedit_divCbk         ( G3DUI *, G3DUIAXIS, int    );
void common_g3duiplaneedit_radiusCbk      ( G3DUI *, G3DUIAXIS, float  );
void common_g3duiplaneedit_orientationcbk ( G3DUI *, const char * );

/****************************** Mesh Edit *************************************/
void common_g3duimeshedit_subdivLevelCbk      ( G3DUI *, int );
void common_g3duimeshedit_subdivRenderCbk     ( G3DUI *, int );
void common_g3duimeshedit_useIsoLinesCbk      ( G3DUI * );
void common_g3duimeshedit_useAdaptiveCbk      ( G3DUI * );
void common_g3duimeshedit_setAdaptiveAngleCbk ( G3DUI *, float );
void common_g3duimeshedit_subdivSyncCbk       ( G3DUI * );

/****************************** Sphere Edit ***********************************/
void common_g3duisphereedit_radiusCbk ( G3DUI *, float );
void common_g3duisphereedit_capCbk    ( G3DUI *, int   );
void common_g3duisphereedit_sliceCbk  ( G3DUI *, int   );

/****************************** Torus Edit ************************************/
void common_g3duitorusedit_orientationCbk ( G3DUI *, const char * );
void common_g3duitorusedit_intRadiusCbk   ( G3DUI *, float );
void common_g3duitorusedit_extRadiusCbk   ( G3DUI *, float );
void common_g3duitorusedit_capCbk         ( G3DUI *, int );
void common_g3duitorusedit_sliceCbk       ( G3DUI *, int );

/******************************* Cube Edit ************************************/
void common_g3duicubeedit_radiusCbk ( G3DUI *, float );
void common_g3duicubeedit_sliceCbk  ( G3DUI *, G3DUIAXIS, int );

/**************************** Cylinder Edit ***********************************/
void common_g3duicylinderedit_lengthCbk ( G3DUI *, float );
void common_g3duicylinderedit_radiusCbk ( G3DUI *, float );
void common_g3duicylinderedit_capCbk    ( G3DUI *, G3DUIAXIS, int );
void common_g3duicylinderedit_sliceCbk  ( G3DUI *, int );

/****************************** FFD Edit **************************************/
void common_g3duiffdedit_radiusCbk ( G3DUI *, G3DUIAXIS, float );
void common_g3duiffdedit_sliceCbk  ( G3DUI *, G3DUIAXIS, int   );

/**************************** Symmetry Edit ***********************************/
void common_g3duisymmetryedit_planeCbk ( G3DUI *, char * );
void common_g3duisymmetryedit_limitCbk ( G3DUI *, float  );

/****************************** File opening **********************************/
G3DSCENE *common_g3dui_open3DSFile ( G3DUI *, char * );
G3DSCENE *common_g3dui_openOBJFile ( G3DUI *, char * );
G3DSCENE *common_g3dui_openDAEFile ( G3DUI *, char * );
G3DSCENE *common_g3dui_openC4DFile ( G3DUI *, char * );

/**************************** Coordinates Edit ********************************/
void common_g3duicoordinatesedit_posCbk ( G3DUI *, G3DUIAXIS, float );
void common_g3duicoordinatesedit_rotCbk ( G3DUI *, G3DUIAXIS, float );
void common_g3duicoordinatesedit_scaCbk ( G3DUI *, G3DUIAXIS, float );

/**************************** Weightgroup List ********************************/
void common_g3duiweightgrouplist_selectCbk            ( G3DUI *, G3DWEIGHTGROUP * );
void common_g3duiweightgrouplist_deleteSelectedCbk    ( G3DUI * );
void common_g3duiweightgrouplist_nameCbk              ( G3DUI *, const char * );
void common_g3duiweightgrouplist_addWeightGroupCbk    ( G3DUI * );
void common_g3duiweightgrouplist_deleteWeightGroupCbk ( G3DUI * );
G3DWEIGHTGROUP *common_g3duiweightgrouplist_getWeightGroup ( G3DMESH *, float,
                                                                        float );

/**************************** PickTool Edit  **********************************/
void common_g3duipicktooledit_onlyVisibleCbk ( G3DUI *, int   );
void common_g3duipicktooledit_setWeightCbk   ( G3DUI *, float );
void common_g3duipicktooledit_paintRadiusCbk ( G3DUI *, float );

/****************************** UVMap Edit  ***********************************/
void common_g3duiuvmap_lockUVMapCbk  ( G3DUI * );
void common_g3duiuvmap_projectionCbk ( G3DUI *, const char * );

/**************************** Cut Mesh Edit  **********************************/
void common_g3duicutmeshtooledit_restrictCbk ( G3DUI *, int );

/****************************** Light Edit  ***********************************/
void common_g3dui_lightDiffuseChangeCbk ( G3DUI *, uint32_t,
                                                   uint32_t,
                                                   uint32_t );
void common_g3dui_lightSpecularityChangeCbk ( G3DUI *, uint32_t,
                                                       uint32_t,
                                                       uint32_t );

/****************************** Texture Edit  *********************************/
void common_g3duitextureedit_setUVMapCbk ( G3DUI *, uint32_t );

/****************************** Render Edit  **********************************/
void common_g3duirenderedit_startFrameCbk ( G3DUI *, float );
void common_g3duirenderedit_endFrameCbk   ( G3DUI *, float );
void common_g3duirenderedit_motionBlurCbk ( G3DUI *, uint32_t );
void common_g3duirenderedit_fpsCbk        ( G3DUI *, uint32_t );
void common_g3duirenderedit_outputCbk     ( G3DUI *, const char * );
void common_g3duirenderedit_renderCbk     ( G3DUI * );
void common_g3duirenderedit_previewCbk    ( G3DUI * );
void common_g3duirenderedit_widthCbk      ( G3DUI *, uint32_t );
void common_g3duirenderedit_heightCbk     ( G3DUI *, uint32_t );
void common_g3duirenderedit_ratioCbk      ( G3DUI *, float );
void common_g3duirenderedit_backgroundCbk ( G3DUI *, unsigned char, 
                                                     unsigned char, 
                                                     unsigned char );
void common_g3duirenderedit_formatCbk     ( G3DUI *, const char * );
void common_g3duirenderedit_saveCbk       ( G3DUI *, uint32_t );

#ifdef __linux__
void common_g3dui_sighandler ( int, siginfo_t *, void * );
#endif

/******************************* Menu Bar *************************************/
void common_g3dui_splitMeshCbk          ( G3DUI *, const char * );
void common_g3dui_mergeMeshCbk          ( G3DUI * );
void common_g3dui_mirrorWeightGroupCbk  ( G3DUI *, const char * );
void common_g3dui_resetBoneTreeCbk      ( G3DUI * );
void common_g3dui_resetBoneCbk          ( G3DUI * );
void common_g3dui_fixBoneTreeCbk        ( G3DUI * );
void common_g3dui_fixBoneCbk            ( G3DUI * );
void common_g3dui_addBoneCbk            ( G3DUI * );
void common_g3dui_resetFFDBoxCbk        ( G3DUI * );
void common_g3dui_assignFFDBoxCbk       ( G3DUI * );
void common_g3dui_addFFDBoxCbk          ( G3DUI * );
void common_g3dui_convertSymmetryCbk    ( G3DUI * );
void common_g3dui_addSymmetryCbk        ( G3DUI * );
void common_g3dui_addEmptyMeshCbk       ( G3DUI * );
void common_g3dui_addCameraCbk          ( G3DUI * );
void common_g3dui_addLightCbk           ( G3DUI * );
void common_g3dui_addCylinderCbk        ( G3DUI * );
void common_g3dui_addTorusCbk           ( G3DUI * );
void common_g3dui_addSphereCbk          ( G3DUI * );
void common_g3dui_addCubeCbk            ( G3DUI * );
void common_g3dui_addPlaneCbk           ( G3DUI * );
void common_g3dui_selectTreeCbk         ( G3DUI *, const char * );
void common_g3dui_triangulateCbk        ( G3DUI *, const char * );
void common_g3dui_untriangulateCbk      ( G3DUI * );
void common_g3dui_invertNormalCbk       ( G3DUI * );
void common_g3dui_alignNormalsCbk       ( G3DUI * );
void common_g3dui_weldVerticesCbk       ( G3DUI * );
void common_g3dui_deleteLoneVerticesCbk ( G3DUI * );
void common_g3dui_invertSelectionCbk    ( G3DUI * );
void common_g3dui_selectAllCbk          ( G3DUI * );

/************ Functions that shoud be implemented by the toolkit  **************/
void g3dui_clearMaterials                ( G3DUI * );
void g3dui_importMaterials               ( G3DUI * );
void g3dui_redrawAllWeightGroupList      ( G3DUI * );
void g3dui_redrawGLViews                 ( G3DUI * );
void g3dui_redrawMaterialList            ( G3DUI * );
void g3dui_redrawObjectList              ( G3DUI * );
void g3dui_redrawTimeline                ( G3DUI * );
void g3dui_setHourGlass                  ( G3DUI * );
void g3dui_unsetHourGlass                ( G3DUI * );
void g3dui_updateAllCurrentEdit          ( G3DUI * );
void g3dui_updateAllCurrentMouseTools    ( G3DUI * );
void g3dui_updateCoords                  ( G3DUI * );
void g3dui_updateKeyEdit                 ( G3DUI * );
void g3dui_updateMaterialEdit            ( G3DUI * );
void g3dui_updateSelectedMaterialPreview ( G3DUI * );


/******************************************************************************/
void common_g3dui_loadConfiguration ( G3DUI *, char * );

uint64_t getTotalMemory ( );

uint32_t            common_g3dui_cancelRenderByID        ( G3DUI *, uint64_t   );
uint32_t            common_g3dui_cancelRenderByScene     ( G3DUI *, R3DSCENE * );
G3DUIRENDERPROCESS *common_g3dui_getRenderProcessByScene ( G3DUI *, R3DSCENE * );
G3DUIRENDERPROCESS *common_g3dui_getRenderProcessByID    ( G3DUI *, uint64_t   );

#endif
