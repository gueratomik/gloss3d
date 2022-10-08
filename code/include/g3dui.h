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
#include <X11/Xlib.h>
#ifdef WITH_MOTIF
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#endif
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
#include <g3dengine/g3dengine.h>


#ifdef __linux__
/*#include <r3dnet.h>*/
#endif

#include <g3durmanager.h>
#include <g3dmouse.h>
#include <g3dcom.h>
#include <g3dexport.h>
#include <g3dexportv2.h>
#include <g3dexportv3.h>
#include <g3dimport.h>
#include <g3dimportv2.h>
#include <g3dimportv3.h>
#include <qiss3d/q3d.h>

/******************************************************************************/
#define GLOSS3DLICENSE \
"  GLOSS3D is free software: you can redistribute it and/or modify \n \
it under the terms of the GNU General Public License as published by \n \
the Free Software Foundation, either version 3 of the License, or \n \
(at your option) any later version. \n \
\n \
  GLOSS3D is distributed in the hope that it will be useful, \n \
but WITHOUT ANY WARRANTY; without even the implied warranty of \n \
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \n \
GNU General Public License for more details. \n \
\n\
  You should have received a copy of the GNU General Public License \n \
along with GLOSS3D.  If not, see http://www.gnu.org/licenses/." \

/*********************** Export to g3d file ***********************************/
#define SIG_G3DUI                           0x3D3D0000
#define SIG_G3DUI_OBJECTS                       0x20000000
#define SIG_G3DUI_OBJECT_ENTRY                      0x21000000
#define SIG_G3DUI_OBJECT_ID                             0x21100000 /*      uint32_t*/
#define SIG_G3DUI_OBJECT_COLLAPSED                      0x21200000 /*(bool)uint32_t*/
#define SIG_G3DUI_VIEWS                         0x10000000
#define SIG_G3DUI_VIEW_ENTRY                        0x11000000
/*Deprecated: #define SIG_G3DUI_VIEW_FLAGS              0x11100000 */
#define SIG_G3DUI_VIEW_FLAGS                            0x11400000
#define SIG_G3DUI_VIEW_FLAGS_SHOWTEXTURES                   0x11401000 /*(bool)uint32_t*/
#define SIG_G3DUI_VIEW_FLAGS_SHOWNORMALS                    0x11402000 /*(bool)uint32_t*/
#define SIG_G3DUI_VIEW_FLAGS_SHOWBONES                      0x11403000 /*(bool)uint32_t*/
#define SIG_G3DUI_VIEW_FLAGS_SHOWGRID                       0x11404000 /*(bool)uint32_t*/
#define SIG_G3DUI_VIEW_FLAGS_SHOWBACKGROUND                 0x11405000 /*(bool)uint32_t*/
#define SIG_G3DUI_VIEW_FLAGS_SHOWLIGHTING                   0x11406000
#define SIG_G3DUI_VIEW_CAMERA                           0x11200000
#define SIG_G3DUI_VIEW_CAMERA_POSITION                      0x11210000 /* float-float-float */
#define SIG_G3DUI_VIEW_CAMERA_ROTATION                      0x11220000 /* float-float-float */
#define SIG_G3DUI_VIEW_CAMERA_SCALING                       0x11230000 /* float-float-float */
#define SIG_G3DUI_VIEW_CAMERA_FOCAL                         0x11240000 /* float */
#define SIG_G3DUI_VIEW_USECAMERA                        0x1130000 /* uint32_t */

/******************************************************************************/
#define G3DUIAPPNAME "Gloss3D"

#define FILEDESC_3DS "3D Studio .3ds"
#define FILEDESC_OBJ "Wavefront .obj"
#define FILEDESC_DAE "Collada .dae"
#define FILEDESC_C4D "Cinema4D .c4d"
#define FILEDESC_STA "Ascii STL .stl"
#define FILEDESC_POV "Povray Scene .pov"
#define FILEDESC_V2  "Gloss3D V2"
#define FILEDESC_V1  "Gloss3D V1"

#define OBJECTMENUNAME          "Object Menu"

#define VERTEXMODEMESHMENUNAME  "Vertex_Mode_Mesh_Menu"
#define EDGEMODEMESHMENUNAME    "Edge_Mode_Mesh_Menu"
#define FACEMODEMESHMENUNAME    "Face_Mode_Mesh_Menu"
#define SCULPTMODEMESHMENUNAME  "Sculpt_Mode_Mesh_Menu"

#define VERTEXMODESPLINEMENUNAME  "Vertex_Mode_Spline_Menu"
#define VERTEXMODEMORPHERMENUNAME "Vertex_Mode_Mopher_Menu"

#define OPTIONMENUNAME  "Option_Menu"
#define SHADINGMENUNAME "Shading_Menu"

#define PROCPERLINNOISE "Perlin Noise"
#define PROCCHESSBOARD  "Chess board"
#define PROCBRICK       "Brick"
#define PROCGRADIENT    "Gradient"

#define PROCRES128  "128x128"
#define PROCRES256  "256x256"
#define PROCRES512  "512x512"
#define PROCRES1024 "1024x1024"
#define PROCRES2048 "2048x2048"

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
#define RENDERWINDOWSTATUSBARNAME "RENDERWINDOWSTATUSBAR"

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
#define MESHTOOL       ( 1 << 6 )
#define SPLINETOOL     ( 1 << 7 )
#define MORPHERTOOL    ( 1 << 8 )

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
#define EDITABSOLUTE  "Absolute"

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

#define EDITTEXTUREMAPPING   "Choose UVW Map"
#define EDITTEXTURERESTRICT  "Restrict to selection"
#define EDITTEXTUREREPEAT    "Repeat texture"
 





#define EDITFFD         "Edit FFD"
#define EDITFFDXSLICES  "X Slices"
#define EDITFFDYSLICES  "Y Slices"
#define EDITFFDZSLICES  "Z Slices"
#define EDITFFDXRADIUS  "X Radius"
#define EDITFFDYRADIUS  "Y Radius"
#define EDITFFDZRADIUS  "Z Radius"

/**** Widget names for LightEdit TextField widget ***/
#define EDITLIGHT                  "Light"





/**** Widget names for Pick Tool widget ***/
#define EDITPICKTOOLVISIBLE        "Only visible"
#define EDITPICKTOOLWEIGHTFRAME    "Weight Painting"
#define EDITPICKTOOLWEIGHTSTRENGTH "Strength"
#define EDITPICKTOOLWEIGHTRADIUS   "Radius"

/**** Widget names for Cut Mesh Tool widget ***/
#define EDITCUTMESHTOOLRESTRICT    "Restrict to selection"

/**** Widget names for MeshEdit TextField widget ***/
#define EDITMESH             "Mesh"
#define EDITMESHGENERAL      "General"
#define EDITMESHWEIGHTGROUPS "Weight Groups"
#define EDITMESHFACEGROUPS   "Face Groups"
#define EDITMESHPOSE         "Poses"
#define EDITMESHGOURAUDLIMIT "Gouraud Shading limit"
#define EDITMESHISOLINES     "Use isoparms (Slower)"
#define EDITMESHSHADING      "Disable shading"

/**** Widget names for BoneEdit TextField widget ***/
#define EDITBONE            "Bone"
#define EDITBONEVERTEXGROUP "Vertex Groups (Double-Click to link)"

#define EDITCAMERA            "Camera"
#define EDITCAMERADOF         "Depth of field"
#define EDITCAMERADOFENABLE   "Enable"
#define EDITCAMERADOFNEARBLUR "Near blur"
#define EDITCAMERADOFNOBLUR   "No blur"
#define EDITCAMERADOFFARBLUR  "Far blur"
#define EDITCAMERADOFRADIUS   "Radius"

#define EDITSYMMETRY      "Symmetry"
#define EDITSYMMETRYPLANE "Symmetry Plane"
#define EDITSYMMETRYLIMIT "Merge limit"

#define EDITWEIGHTGROUPNAME "Group name"
#define EDITWEIGHTGROUPLIST "Group list"

#define EDITMESHPOSENAME    "Pose name"
#define EDITMESHPOSELIST    "Pose list"

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

#define EDITCHANNELCOLOR           "Color"
#define EDITCHANNELIMAGE           "Image"
#define EDITCHANNELPROCEDURAL      "Procedural"
#define EDITCHANNELPROCEDURALTYPE  "Procedural Type"
#define EDITCHANNELPROCEDURALRES   "Procedural Res"

#define EDITDIFFUSE                "Diffuse"
#define EDITDIFFUSECOLOR           EDITCHANNELCOLOR
#define EDITDIFFUSEIMAGE           EDITCHANNELIMAGE
#define EDITDIFFUSEPROCEDURAL      EDITCHANNELPROCEDURAL
#define EDITDIFFUSEPROCEDURALTYPE  EDITCHANNELPROCEDURALTYPE
#define EDITDIFFUSEPROCEDURALRES   EDITCHANNELPROCEDURALRES
#define EDITDIFFUSEPROCEDURALEDIT  "Procedural diffuse settings"

#define EDITSPECULAR            "Specularity"
#define EDITSPECULARSHININESS   "Shininess"
#define EDITSPECULARLEVEL       "Level"
#define EDITSPECULARCOLOR       EDITCHANNELCOLOR
#define EDITSPECULARIMAGE       EDITCHANNELIMAGE

#define EDITDISPLACEMENT           "Displacement"
#define EDITDISPLACEMENTENABLED    "Enable Displacement Channel"
#define EDITDISPLACEMENTCHANNEL    "Displacement Channel"
#define EDITDISPLACEMENTIMAGE      EDITCHANNELIMAGE
#define EDITDISPLACEMENTPROCEDURAL EDITCHANNELPROCEDURAL
#define EDITDISPLACEMENTSTRENGTH   "Strength"
#define EDITDISPLACEMENTPROCEDURALEDIT  "Procedural displacement settings"

#define EDITBUMP                   "Bump"
#define EDITBUMPENABLED            "Enable Bump Channel"
#define EDITBUMPCHANNEL            "Bump Channel"
#define EDITBUMPIMAGE               EDITCHANNELIMAGE
#define EDITBUMPPROCEDURAL          EDITCHANNELPROCEDURAL
#define EDITBUMPSTRENGTH           "Strength"
#define EDITBUMPPROCEDURALEDIT  "Procedural bump settings"

#define EDITALPHA                   "Alpha"
#define EDITALPHAENABLED            "Enable Alpha Channel"
#define EDITALPHACHANNEL            "Alpha Channel"
#define EDITALPHAIMAGE               EDITCHANNELIMAGE
#define EDITALPHAPROCEDURAL          EDITCHANNELPROCEDURAL
#define EDITALPHASTRENGTH           "Opacity"
#define EDITALPHAPROCEDURALEDIT  "Procedural alpha settings"

#define EDITREFLECTION             "Reflection"
#define EDITREFLECTIONSTRENGTH     "Strength"

#define EDITREFRACTION             "Refraction"
#define EDITREFRACTIONTRANSPARENCY "Transparency"
#define EDITREFRACTIONSTRENGTH     "Strength"

#define EDITCOLORSOLID             EDITCHANNELCOLOR
#define EDITCOLORIMAGE             EDITCHANNELIMAGE
#define EDITCOLORPROCEDURAL        EDITCHANNELPROCEDURAL

#define EDITUVMAP             "UVMap"
#define EDITUVMAPPROJECTION   "Projection"
#define EDITUVMAPNAME         "Name"
#define EDITUVMAPFIXED        "Fixed"
#define EDITUVMAPEDITOR       "Edit texture and UVs in LIPS3D"
#define CYLINDRICALPROJECTION "Cylindrical"
#define SPHERICALPROJECTION   "Spherical"
#define FLATPROJECTION        "Flat"
#define BACKGROUNDPROJECTION  "Background"

#define EDITSUBDIVIDER         "Subdivider"

#define EDITSPLINEREVOLVER      "Spline Revolver"
#define EDITSPLINEREVOLVERSTEPS "Steps"
#define EDITSPLINEREVOLVERDIVIS "Divisions"

#define EDITTEXT               "Text Editor"
#define EDITTEXTTEXT           "Text"
#define EDITTEXTSIZE           "Size"
#define EDITTEXTFONT           "Font"
#define EDITTEXTROUNDNESS      "Roundness"
#define EDITTEXTTHICKNESS      "Thickness"

#define EDITMORPHER            "Morpher"

#define EDITINSTANCE           "Instance"

#define EDITPARTICLEEMITTER    "Particle Emitter"

#define MODE_VIEWOBJECT "Object mode"
#define MODE_VIEWVERTEX "Vertex mode"
#define MODE_VIEWEDGE   "Edge mode"
#define MODE_VIEWFACE   "Face mode"
#define MODE_VIEWSKIN   "Skin mode"
#define MODE_VIEWPATH   "Path mode"
#define MODE_VIEWSCULPT "Subdivided displacement mode"
#define MODE_VIEWAXIS   "Axis mode"
#define MODE_VIEWUVWMAP "UVW mode"

#define MENU_XAXIS "X axis"
#define MENU_YAXIS "Y axis"
#define MENU_ZAXIS "Z axis"

#define MENU_NEWSCENE    "New scene"
#define MENU_OPENFILE    "Open file"
#define MENU_MERGESCENE  "Merge scene"
#define MENU_IMPORTFILE  "_Import"
#define MENU_SAVEFILE    "Save"
#define MENU_SAVEFILEAS  "Save as"
#define MENU_EXPORTSCENE "_Export"
#define MENU_EXIT        "Exit"
#define MENU_SAVEIMAGE    "Save image"
#define MENU_SAVEIMAGEAS  "Save image as"

#define MENU_UNDO                 "Undo"
#define MENU_REDO                 "Redo"
#define MENU_DELETE               "Delete"
#define MENU_INVERTNORMALS        "Invert Normals"
#define MENU_ALIGNNORMALS         "Align Faces"
#define MENU_TRIANGULATE          "Triangulate"
#define MENU_OPTION_CLOCKWISE     "Clockwise"
#define MENU_OPTION_ANTICLOCKWISE "Counter-Clockwise"
#define MENU_TRIANGULATE          "Triangulate"
#define MENU_UNTRIANGULATE        "Untriangulate"
#define MENU_WELDVERTICES         "Weld Selected Vertices"
#define MENU_DELETELONEVERTICES   "Delete Lone Vertices"
#define MENU_CONVERTSYMMETRY      "Convert Symmetry"
#define MENU_SELECTALL            "Select All"
#define MENU_SELECTTREE           "Select Tree"
#define MENU_OPTION_ALLTYPES      "All Types"
#define MENU_OPTION_SAMETYPE      "Same Type"
#define MENU_INVERTSELECTION      "Invert Selection"
#define MENU_GETOBJECTSTATS       "Object statistics"

#define MENU_ADDPLANE             "Add Plane"
#define MENU_ADDCUBE              "Add Cube"
#define MENU_ADDSPHERE            "Add Sphere"
#define MENU_ADDTORUS             "Add Torus"
#define MENU_ADDCYLINDER          "Add Cylinder"
#define MENU_ADDTUBE              "Add Tube"
#define MENU_ADDLIGHT             "Add Light"
#define MENU_ADDCAMERA            "Add Camera"
#define MENU_ADDEMPTYMESH         "Add Empty Mesh"
#define MENU_ADDSPLINE            "Add Spline"
#define MENU_ADDTEXT              "Add Text"
#define MENU_ADDNULL              "Add Null Object"

#define MENU_COMMITMODIFIER       "Commit up to modifier"
#define MENU_ADDBONE              "Add Bone"
#define MENU_ADDSYMMETRY          "Add Symmetry"
#define MENU_ADDINSTANCE          "Add Instance"
#define MENU_ADDEMITTER           "Add Particle emitter"
#define MENU_ADDWIREFRAME         "Add Wireframe"
#define MENU_ADDSPLINEREVOLVER    "Add Spline Revolver"
#define MENU_ADDSUBDIVIDER        "Add Subdivider"
#define MENU_ADDFFDBOX            "Add FFD Box"
#define MENU_ADDMORPHER           "Add Morpher"
#define MENU_ADDSKIN              "Add Skin"
#define MENU_ASSIGNFFD            "Assign FFD"
#define MENU_RESETFFD             "Reset FFD"
#define MENU_FIXBONE              "Fix Bone"
#define MENU_FIXBONETREE          "Whole tree"
#define MENU_FIXBONEONLY          "This bone only"
#define MENU_RESETBONE            "Reset Bone"
#define MENU_RESETBONETREE        "Whole tree"
#define MENU_RESETBONEONLY        "This bone only"
#define MENU_UNSELECTAREA         "Unselect area"

#define MENU_ADDVIBRATORTAG       "Add Vibrator tag"
#define MENU_ADDEMITTERTAG        "Add Emitter tag"
#define MENU_ADDTRACKERTAG        "Add Tracker tag"
#define MENU_REMOVESELTAG         "Remove selected tag"

#define MENU_ADDUVWMAP            "Add UVW Map"

#define MENU_MIRRORWEIGHTGROUP    "Mirror WeightGroup"
#define MENU_MIRRORHEIGHTMAP      "Mirror Heightmap"
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

#define MENU_ABOUT                "About"

#define MENU_ADDUVMAP             "Add UV Map"
#define MENU_FITUVMAP             "Fit to object"
#define MENU_ALIGNUVMAP           "Align UV Map"
#define MENU_ALIGNUVMAPXY         "Along XY plane"
#define MENU_ALIGNUVMAPYZ         "Along YZ plane"
#define MENU_ALIGNUVMAPZX         "Along ZX plane"

#define MENU_UVFROMVERTEX         "Select UVs from selected vertices"
#define MENU_VERTEXFROMUV         "Select vertices from selected UVs"
#define MENU_UVSETFROMFACE        "Select UV Set from selected faces"
#define MENU_FACEFROMUVSET        "Select faces from selected UV Set"

#define MENU_CREATECHANNELIMAGE   "Create image for channel"
#define MENU_OPENCHANNELIMAGE     "Load image in channel"
#define MENU_SAVECHANNELIMAGE     "Save image"
#define MENU_SAVECHANNELIMAGEAS   "Save image as..."
#define MENU_LEAVELIPS3D          "Exit LIPS3D"

#define MENU_FGFILL               "Fill with foreground color"
#define MENU_BGFILL               "Fill with background color"
#define MENU_RESIZEIMAGE          "Resize image"

#define RWMENU_SAVEJPG           "Save as JPEG"
#define RWMENU_SAVEPNG           "Save as PNG"
#define RWMENU_CLOSE             "Close window"

#define RWMENU_VIEW_FIT  "Fit to window"
#define RWMENU_VIEW_FULL "Full size"

#define VIEWMENU_LIGHTING        "Lights on"
#define VIEWMENU_BACKGROUND      "Show background"
#define VIEWMENU_TEXTURES        "Show textures"
#define VIEWMENU_GRID            "Show grid"
#define VIEWMENU_BONES           "Show bones"
#define VIEWMENU_NORMALS         "Show normals"
#define VIEWMENU_DEFAULTCAMERA   "Use default camera"
#define VIEWMENU_SELECTEDCAMERA  "Use selected camera"
#define VIEWMENU_RESET           "Reset view"
#define VIEWMENU_UNDO            "Undo  view"
#define VIEWMENU_REDO            "Redo  view"

#define TIMEMENU_DELETE          "Delete selected keys"
#define TIMEMENU_SCALE           "Scale keys"
#define TIMEMENU_SELECT          "Select keys"

/******************************************************************************/
#define MARGIN 0x02
#define MARGDB ( MARGIN << 0x01 )

/******************************************************************************/
/*** The Copy-Paste manager ***/
#define CLIPBOARDEMPTY          0x00
#define CLIPBOARDCOPYOBJECT     0x01
#define CLIPBOARDCOPYKEY        0x02
#define CLIPBOARDCOPYFACESCULPT 0x03

/******************************************************************************/
typedef struct _M3DUI M3DUI;
typedef struct _G3DUIRENDERPROCESS G3DUIRENDERPROCESS;
typedef struct _G3DUIRENDERBUFFER G3DUIRENDERBUFFER;

/******************************************************************************/
typedef enum _G3DUIAXIS {
    G3DUIXAXIS,
    G3DUIYAXIS,
    G3DUIZAXIS,
    G3DUIUAXIS,
    G3DUIVAXIS
} G3DUIAXIS;

typedef struct _G3DUICOPIEDITEM {
    G3DOBJECT              *obj;
    G3DKEY                 *key;
    G3DFACESCULPTEXTENSION *fse;
/*** We nee the world matrix for parent swapping ***/
    double                  wmatrix[0x10]; 
} G3DUICOPIEDITEM;

typedef struct _G3DUICLIPBOARD {
    uint32_t operation;
    G3DSCENE  *sce;
    LIST     *lcpy; /*** list of copied objects ***/
    LIST     *lkey;
} G3DUICLIPBOARD;

/******************************************************************************/
typedef struct _G3DUICONF {
    uint32_t undolevel;
} G3DUICONF;

#define RENDERTOIMAGENAME "Render to image"
#define RENDERTOVIDEONAME "Render to video"

/******************************************************************************/
/*** This structure is passed when confirming the subdivision level after a ***/
/*** warning window has been raised against the amount of required memory   ***/
typedef struct _G3DUICONFIRMSUBDIV {
    G3DMESH *mes;
    uint32_t level;
} G3DUICONFIRMSUBDIV;

/********************* a "portable" version of XRectangle *********************/
typedef G2DRECTANGLE G3DUIRECTANGLE;

/********************* a "portable" version of XSegment *********************/
typedef struct _G3DUISEGMENT {
    short x1, x2, y1, y2;
} G3DUISEGMENT; 

typedef struct _G3DUIMENU         G3DUIMENU;
typedef struct _G3DUIVIEW         G3DUIVIEW;
typedef struct _G3DUIMODEBAR      G3DUIMODEBAR;
typedef struct _G3DUIQUAD         G3DUIQUAD;
typedef struct _G3DUIMAIN         G3DUIMAIN;
typedef struct _G3DUIBOARD        G3DUIBOARD;
typedef struct _G3DUIRENDERWINDOW G3DUIRENDERWINDOW;
typedef struct _G3DUIMOUSETOOL    G3DUIMOUSETOOL;

/********************************* G3DUI **************************************/
typedef struct _G3DUI {
    G3DUIMOUSETOOL *curmou; /*** current mouse tool ***/
    G3DUIMOUSETOOL *curuvmou; /*** current mouse tool for UV editor ***/
    G3DUICLIPBOARD *cli;
    G3DSCENE     *sce;
    G3DUICONF     conf;
    uint64_t      engine_flags;

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

    G3DUIMENU *objectModeMenu;
    G3DUIMENU *vertexModeMeshMenu;
    G3DUIMENU *edgeModeMeshMenu;
    G3DUIMENU *faceModeMeshMenu;
    G3DUIMENU *sculptModeMeshMenu;
    G3DUIMENU *vertexModeSplineMenu;
    G3DUIMENU *vertexModeMorpherMenu;


    LIST         *limg; /*** List of images (among them are textures) ***/
    LIST         *lrps; /*** list of render process ***/
    LIST         *lmou; /*** list of mousetools ***/
    LIST         *lselmat;
    /*G3DMATERIAL   *selmat;*/
    G3DURMANAGER  *urm;
    /*R3DNETSERVER  *srv;
    R3DNETCLIENT  *clt;*/
    G3DUIRECTANGLE  menurec;
    G3DUIRECTANGLE  tbarrec;
    G3DUIRECTANGLE  mbarrec;
    G3DUIRECTANGLE  mbrdrec;
    G3DUIRECTANGLE  proprec;
    G3DUIRECTANGLE  quadrec;
    G3DUIRECTANGLE  timerec;
    G3DUIRECTANGLE  inforec;
    G3DCAMERA     **defaultCameras;
#ifdef __linux__
    GLXContext     sharedCtx; /* shared context for textures */
    pthread_t      playthreadid;
#endif
#ifdef __MINGW32__
    HGLRC          sharedCtx; /* shared context for textures */
    HANDLE         playthreadid;
    COMPVARS       cvars;
#endif
    LIST *lrsg; /*** list of render settings ***/
    Q3DSETTINGS *currsg; /*** current render settings ***/
    char *filename;
    int lock; /*** I use this for preventing loops on XmText fields ***/
    char *loadFile;
    void *toolkit_data; /*** Toolkit related data that cant be shared ***/
    float startframe;
    float curframe;   /*** used by g3duitimeboard.c/play_t() ***/
    float endframe;
    int32_t fps;
    uint32_t playLock;
    LIST *lexportExtensions; /* list of G3DEXPORTEXTENSION */
    Q3DFILTER *toframe;
    uint32_t   inited;

    /*** GUI toolkit extends the following data ***/
    G3DUIVIEW    *currentView;
    G3DUIMAIN    *main;
    G3DUIRENDERWINDOW *renderWindow;
    LIST         *lview;
} G3DUI;

/******************************************************************************/
#define G3DUIMENUTYPE_PUSHBUTTON   0x00
#define G3DUIMENUTYPE_TOGGLEBUTTON 0x01
#define G3DUIMENUTYPE_SEPARATOR    0x02
#define G3DUIMENUTYPE_SUBMENU      0x03
#define G3DUIMENUTYPE_MENUBAR      0x04

#define MENU_CONDITION_SENSITIVE   1
#define MENU_CONDITION_ACTIVE    ( 1 << 1 )

#define CLASS_MAIN              "g3dmain"
#define CLASS_ENTRY             "g3dentry"
#define MENU_CLASS_MAIN         CLASS_MAIN
#define CLASS_VIEW_MENU_BAR     "view_menu_bar"
#define CLASS_VIEW_MENU_DEFAULT "view_menu_default"

typedef struct _G3DUIMENU {
    G3DUI             *gui;
    unsigned char     *name;
    unsigned char     *class;
    uint32_t           type;
    uint32_t         (*condition) ( struct _G3DUIMENU *menu, void *data );
    uint64_t          (*callback) ( struct _G3DUIMENU *menu, void *data );
    LIST               *lchildren;
    void               *data;
    struct _G3DUIMENU **nodes;
} G3DUIMENU;

/******************************************************************************/
typedef struct _G3DUIMOUSETOOL {
    G3DMOUSETOOL *tool;
    G3DUIMENU    *menu;
    uint32_t     flags;
} G3DUIMOUSETOOL;

/******************************************************************************/
typedef struct _G3DUITOOLBAR {
    G3DUI *gui;
} G3DUITOOLBAR;

/******************************************************************************/
typedef struct _G3DUITIMELINE {
    G3DUI     *gui;
    G3DUIMENU *menu;

    int32_t    midframe; /*** the frame in the middle of the widget     ***/
    uint32_t   nbpix;   /*** Number of pixels between frame indicators ***/
    float    (*funcKey)(G3DKEY *key, void *data );
    void      *funcData;
    uint32_t   tool;
} G3DUITIMELINE;

/******************************************************************************/
typedef struct _G3DUITIMEBOARD {
    G3DUI         *gui;

    G3DUIRECTANGLE linerec;
    G3DUIRECTANGLE btnrec;
    G3DUIRECTANGLE funcrec;

    G3DUITIMELINE *timeline;
} G3DUITIMEBOARD;

/******************************************************************************/
typedef struct _G3DUIMESHPOSELIST {
    G3DUI *gui;
} G3DUIMESHPOSELIST;


/******************************************************************************/
typedef struct _G3DUIRENDERBUFFER {
#ifdef __linux__
    Display            *dis; 
    Window              win;
    GC                  gc;
    XImage             *ximg;
    XShmSegmentInfo     ssi;
#endif
#ifdef __MINGW32__
    HWND    hWnd;
    WImage *wimg;
#endif
} G3DUIRENDERBUFFER;

/******************************************************************************/
#define RENDERWINDOW_FIT_SIZE   0x00
#define RENDERWINDOW_FULL_SIZE  0x01
 
typedef struct _G3DUIRENDERWINDOW {
    G3DUI              *gui;
    uint32_t            displayMode;

    G3DUIMENU          *menuBar;
    Q3DFILTER          *tostatus;
    G3DUIRENDERPROCESS *rps;
    G3DUIRENDERBUFFER   rbuf;
    G3DUIRECTANGLE      menurec;
    G3DUIRECTANGLE      statusrec;
    G3DUIRECTANGLE      scrrec;
    G3DUIRECTANGLE      arearec;
} G3DUIRENDERWINDOW;

/******************************************************************************/
typedef struct _G3DUILIGHTEDIT {
    G3DUI    *gui;
    uint32_t  multi;
    G3DLIGHT *editedLight;
} G3DUILIGHTEDIT;

/******************************************************************************/
typedef struct _G3DUIUVMAPEDIT {
    G3DUI *gui;
} G3DUIUVMAPEDIT;

/******************************************************************************/
typedef struct _G3DUIMATERIALEDIT {
    G3DUI *gui;
} G3DUIMATERIALEDIT;

typedef struct _G3DUIWEIGHTGROUPLIST {
    G3DUI *gui;
} G3DUIWEIGHTGROUPLIST;

/******************************************************************************/
typedef struct _G3DUIFFDEDIT {
    G3DUI        *gui;
    uint32_t      multi;
    G3DFFD       *editedFFD;
} G3DUIFFDEDIT;

/******************************************************************************/
typedef struct _G3DUICOORDINATESEDIT {
    G3DUI *gui;
} G3DUICOORDINATESEDIT;

/******************************************************************************/
typedef struct _G3DUICAMERAEDIT {
    G3DUI        *gui;
    uint32_t      multi;
    G3DCAMERA    *editedCamera;
} G3DUICAMERAEDIT;

/******************************************************************************/
typedef struct _G3DUIWIREFRAMEEDIT {
    G3DUI        *gui;
    uint32_t      multi;
    G3DWIREFRAME *editedWireframe;
} G3DUIWIREFRAMEEDIT;

/******************************************************************************/
typedef struct _G3DUITEXTEDIT {
    G3DUI   *gui;
    uint32_t multi;
    G3DTEXT *editedText;
} G3DUITEXTEDIT;

/******************************************************************************/
typedef struct _G3DUISUBDIVIDEREDIT {
    G3DUI         *gui;
    uint32_t       multi;
    G3DSUBDIVIDER *editedSubdivider;
} G3DUISUBDIVIDEREDIT;

/******************************************************************************/
typedef struct _G3DUITEXTUREEDIT {
    G3DUI *gui;
} G3DUITEXTUREEDIT;

/******************************************************************************/
typedef struct _G3DUICUBEEDIT {
    G3DUI        *gui;
    uint32_t      multi;
    G3DPRIMITIVE *editedCube;
} G3DUICUBEEDIT;

/******************************************************************************/
typedef struct _G3DUITUBEEDIT {
    G3DUI *gui;
    uint32_t      multi;
    G3DPRIMITIVE *editedTube;
} G3DUITUBEEDIT;

/******************************************************************************/
typedef struct _G3DUITORUSEDIT {
    G3DUI *gui;
    uint32_t      multi;
    G3DPRIMITIVE *editedTorus;
} G3DUITORUSEDIT;

/******************************************************************************/
typedef struct _G3DUITRACKERTAGEDIT {
    G3DUI *gui;
} G3DUITRACKERTAGEDIT;

/******************************************************************************/
typedef struct _G3DUISPHEREEDIT {
    G3DUI        *gui;
    uint32_t      multi;
    G3DPRIMITIVE *editedSphere;
} G3DUISPHEREEDIT;

/******************************************************************************/
typedef struct _G3DUIMESHEEDIT {
    G3DUI    *gui;
    uint32_t  multi;
    G3DMESH *editedMesh;
} G3DUIMESHEDIT;

/******************************************************************************/
typedef struct _G3DUISYMMETRYEDIT {
    G3DUI        *gui;
    uint32_t      multi;
    G3DSYMMETRY *editedSymmetry;
} G3DUISYMMETRYEDIT;

/******************************************************************************/
typedef struct _G3DUISPLINEREVOLVEREDIT {
    G3DUI             *gui;
    uint32_t           multi;
    G3DSPLINEREVOLVER *editedSplineRevolver;
} G3DUISPLINEREVOLVEREDIT;

/******************************************************************************/
typedef struct _G3DUICYLINDEREDIT {
    G3DUI        *gui;
    uint32_t      multi;
    G3DPRIMITIVE *editedCylinder;
} G3DUICYLINDEREDIT;

/******************************************************************************/
typedef struct _G3DUIINSTANCEEDIT {
    G3DUI        *gui;
    uint32_t      multi;
    G3DINSTANCE *editedInstance;
} G3DUIINSTANCEEDIT;

/******************************************************************************/
typedef struct _G3DUIPLANEEDIT {
    G3DUI        *gui;
    uint32_t      multi;
    G3DPRIMITIVE *editedPlane;
} G3DUIPLANEEDIT;

/******************************************************************************/
typedef struct _G3DUIKEYEDIT {
    G3DUI   *gui;
    uint32_t multi;
    G3DKEY  *editedKey;
} G3DUIKEYEDIT;

/******************************************************************************/
typedef struct _G3DUIPARTICLEEMITTEREDIT {
    G3DUI              *gui;
    uint32_t            multi;
    G3DPARTICLEEMITTER *editedPEmitter;
} G3DUIPARTICLEEMITTEREDIT;


/******************************************************************************/
#define VECNORMMAX 1.001f
#define VECNORMMIN 0.999f
typedef struct _G3DUIMATERIALPIXEL {
    uint32_t used;
    G3DVECTOR pos;
    G3DVECTOR nor;
    float     diff;
    float     spec; /*** specular reflection dot product ***/
    float     u, v;
} G3DUIMATERIALPIXEL;

/******************************************************************************/
typedef struct _G3DUIMATERIALMAP {
    G3DUIMATERIALPIXEL *pixel;
    uint32_t            width;
    uint32_t            height;
} G3DUIMATERIALMAP;

/************************** GTK MaterialList Widget ***************************/
typedef struct _G3DUIMATERIALPREVIEW {
    /*** The vector map that helps us to build the preview sphere ***/
    /*** This is per-preview because we need individual maps in case ***/
    /*** normal vectors are affected by bump maps for instance ***/
    G3DUIMATERIALMAP *map;
    G3DUIRECTANGLE    rec;
    G3DMATERIAL      *mat;
} G3DUIMATERIALPREVIEW;


/******************************************************************************/
typedef struct _G3DUIMATERIALLIST {
    G3DUI   *gui;
    LIST    *lpreview;
    uint32_t image_width;
    uint32_t image_height;
    uint32_t preview_per_line;
    uint32_t preview_width;
    uint32_t preview_height;
    uint32_t preview_border;
    uint32_t preview_name_height;
} G3DUIMATERIALLIST;

/******************************************************************************/
typedef struct _G3DUIMATERIALBOARD {
    G3DUI              *gui;
    G3DUIRECTANGLE      menurec;
    G3DUIRECTANGLE      listrec;
    G3DUIRECTANGLE      editrec;
    G3DUIMENU          *menuBar;
    G3DUIMATERIALLIST  *matlist;
    G3DUIMATERIALEDIT  *matedit;
} G3DUIMATERIALBOARD;

/******************************************************************************/
typedef struct _G3DUIOBJECTEDIT {
    G3DUI                   *gui;

    G3DUIRECTANGLE           labrec;
    G3DUIRECTANGLE           namerec;
    G3DUIRECTANGLE           editrec;

    G3DUILIGHTEDIT           *ligedit;
    G3DUICUBEEDIT            *cubedit;
    G3DUICYLINDEREDIT        *cyledit;
    G3DUIFFDEDIT             *ffdedit;
    G3DUISPHEREEDIT          *sphedit;
    G3DUITUBEEDIT            *tubedit;
    G3DUITORUSEDIT           *toredit;
    G3DUIPLANEEDIT           *plnedit;
    G3DUITEXTEDIT            *txtedit;
    G3DUIINSTANCEEDIT        *insedit;
    G3DUISPLINEREVOLVEREDIT  *spredit;
    G3DUISUBDIVIDEREDIT      *sdredit;
    G3DUISYMMETRYEDIT        *symedit;
    G3DUIPARTICLEEMITTEREDIT *pemedit;
    G3DUIWIREFRAMEEDIT       *wiredit;
} G3DUIOBJECTEDIT;

/******************************************************************************/
typedef struct _G3DUIOBJECTLIST {
    G3DUI *gui;
    uint32_t oldWidth,
             oldHeight;
} G3DUIOBJECTLIST;

/******************************************************************************/
typedef struct _G3DUIOBJECTBOARD {
    G3DUI            *gui;
    G3DUIRECTANGLE    menurec;
    G3DUIRECTANGLE    listrec;
    G3DUIRECTANGLE    editrec;
    G3DUIMENU        *menuBar;
    G3DUIOBJECTLIST  *objlist;
    G3DUIOBJECTEDIT  *objedit;
} G3DUIOBJECTBOARD;

/******************************************************************************/
typedef struct _G3DUIMOUSETOOLEDIT {
    G3DUI            *gui;
} G3DUIMOUSETOOLEDIT;

/******************************************************************************/
typedef struct _G3DUIBOARD {
    G3DUI              *gui;
    G3DUIRECTANGLE      boardrec;
    G3DUIRECTANGLE      toolrec;
    G3DUIMATERIALBOARD *matboard;
    G3DUIOBJECTBOARD   *objboard;
    G3DUIMOUSETOOLEDIT *tooledit;
} G3DUIBOARD;

/******************************************************************************/
typedef struct _G3DUIMODEBAR {
    G3DUI *gui;
} G3DUIMODEBAR;

/******************************************************************************/
typedef struct _M3DUIMODEBAR {
    M3DUI *mui;
} M3DUIMODEBAR;

/******************************************************************************/
typedef struct _M3DUIBUCKETTOOLEDIT {
    M3DUI *mui;
} M3DUIBUCKETTOOLEDIT;

/******************************************************************************/
typedef struct _G3DUICUTMESHTOOLEDIT {
    G3DUI *gui;
} G3DUICUTMESHTOOLEDIT;

/******************************************************************************/
typedef struct _M3DUIPENTOOLEDIT {
    M3DUI *mui;
} M3DUIPENTOOLEDIT;

/******************************************************************************/
typedef struct _G3DUIPICKTOOLEDIT {
    G3DUI *gui;
} G3DUIPICKTOOLEDIT;

/******************************************************************************/
typedef struct _G3DUIRENDEREDIT {
    G3DUI       *gui;
    Q3DSETTINGS *editedRsg;
} G3DUIRENDEREDIT;

/******************************************************************************/
typedef struct _G3DUIMAIN {
    G3DUI          *gui;
    G3DUIMENU      *menuBar;
    G3DUIMODEBAR   *modeBar;
    G3DUITOOLBAR   *toolBar;
    G3DUIQUAD      *quad;
    G3DUIBOARD     *board;
    G3DUITIMEBOARD *timeboard;

    G3DUIRECTANGLE  menurec;
    G3DUIRECTANGLE  tbarrec;
    G3DUIRECTANGLE  mbarrec;
    G3DUIRECTANGLE  mbrdrec;
    G3DUIRECTANGLE  proprec;
    G3DUIRECTANGLE  quadrec;
    G3DUIRECTANGLE  timerec;
    G3DUIRECTANGLE  inforec;
} G3DUIMAIN;

/******************************************************************************/
#define G3DUIWIDGET_TAB          0x02
#define G3DUIWIDGET_CONTAINER    0x03
#define G3DUIWIDGET_FRAME        0x04
#define G3DUIWIDGET_PUSHBUTTON   0x05
#define G3DUIWIDGET_TOGGLEBUTTON 0x06
#define G3DUIWIDGET_RADIOBUTTON  0x07
#define G3DUIWIDGET_LABEL        0x08
#define G3DUIWIDGET_FLOATENTRY   0x09
#define G3DUIWIDGET_INTENTRY     0x0A
#define G3DUIWIDGET_CHARENTRY    0x0B

/******************************************************************************/
typedef union {
    float      f;
    uint32_t u32;
    uint64_t u64;
    int32_t  i32;
    int64_t  i64;
} G3DUIWIDGETARG;

/******************************************************************************/
typedef struct _G3DUIWIDGET {
    G3DUI       *gui;
    uint32_t     type;
    char        *label;
    uint32_t     x;
    uint32_t     y;
    uint32_t     width;
    uint32_t     height;
    struct _G3DUIWIDGET *children[];
} G3DUIWIDGET;

/******************************************************************************/
typedef struct _G3DUIENTRYWIDGET { 
    G3DUIWIDGET  wid;
    uint64_t   (*get)( G3DUIWIDGET *wid, G3DUIWIDGETARG *arg );
    uint64_t   (*set)( G3DUIWIDGET *wid, G3DUIWIDGETARG *arg );
    uint32_t     labx, laby,
                 labw, labh;
} G3DUIENTRYWIDGET;

/******************************************************************************/
#define ACTION_GOTOFRAME  0x01
#define ACTION_DUMPSCREEN 0x02

typedef struct _G3DUIACTION {
    uint32_t        wait;
    uint32_t        type;
    pthread_mutex_t done;
    G3DUI          *gui;
} G3DUIACTION;

typedef struct _GOTOFRAME {
    G3DUIACTION action;
    float frame;
    uint64_t extra_engine_flags;
} GOTOFRAME;

typedef struct _DUMPSCREEN {
    G3DUIACTION action;
    uint32_t x, y, width, height;
    unsigned char (*buffer)[4];
} DUMPSCREEN;

/******************************************************************************/
/*** this structure is passed to the Drawing Area widget, so we can save the***/
/*** render as jpeg reading the data stored in the filter or stopping the ***/
/*** render thread ***/
typedef struct _G3DUIRENDERPROCESS {
    uint64_t   id;
    Q3DJOB    *qjob;
    G3DUI     *gui;
    Q3DFILTER *towindow; /*** this contains final XImage structure ***/
    Q3DFILTER *toframe; /*** this jumps to frame in the main context ***/
    char      *filename;
    pthread_t  tid; /*** clean thread ID when cancelling e.g ***/
} G3DUIRENDERPROCESS;

/*************************** Quad Widget Structure ****************************/
typedef struct _G3DUIQUAD {
    G3DUI         *gui;
    uint32_t       mx;   /*** Glider X position                            ***/
    uint32_t       my;   /*** Glider Y position                            ***/
    uint32_t       width;
    uint32_t       height;
    /*** The variables below will allow us to keep a coorect Glider         ***/
    /*** position while resizing the entire QUAD                            ***/
    float          xratio;
    float          yratio;
    G3DUIRECTANGLE rect[0x04]; /*** there are 4 3D windows in our modeler        ***/
                          /*** The fith one is the maximized one            ***/
    G3DUIVIEW     *view[0x04];
    G3DUIVIEW     *magnified;
    uint32_t       margin;
} G3DUIQUAD;


/************************** View Widget Structure *****************************/
#define NBVIEWBUTTON    0x04
#define BUTTONSIZE      0x10 /*** 16x16 ***/

#define MAXIMIZEBUTTON  0x03
#define ROTATEBUTTON    0x02
#define TRANSLATEBUTTON 0x01
#define ZOOMBUTTON      0x00

typedef struct _G3DUIVIEW {
    G3DUI         *gui;
    G3DUIRECTANGLE pixrec[NBVIEWBUTTON];       /*** pixmaps position ***/
    G3DUIRECTANGLE glrec;
    G3DUIRECTANGLE optrec;
    G3DUIRECTANGLE shdrec;
    G3DUIRECTANGLE navrec;
    G3DUIRECTANGLE menurec;
    /*Pixmap         curpix[NBVIEWBUTTON];
    Pixmap         norpix[NBVIEWBUTTON];
    Pixmap         armpix[NBVIEWBUTTON];*/
    int32_t        pressedButtonID; /**** Currently clicked button = -1 if none ***/
    G3DCAMERA     *cam;
    G3DCAMERA     *defcam;
    G3DVECTOR      defcampos; /*** Default camera position ***/
    G3DVECTOR      defcamrot; /*** Default camera rotation ***/
    G3DVECTOR      defcamsca; /*** Default camera scaling  ***/
    float          defcamfoc; /*** Default camera focal    ***/
    /*Widget ogl;*/ /*** OpenGL Widget ***/
    uint32_t       mode;   /*** wireframe, flat, fill ***/
    uint64_t       engine_flags;
    void         (*grid)( uint64_t );
#ifdef __linux__
    Display       *dpy;
    Window         win;
    GLXContext     glctx;
    pthread_t      render_tid; /*** current rendering thread - Used for canceling***/
#endif
#ifdef __MINGW32__
    HWND           hWnd
    HGLRC          glctx;
    HANDLE         render_tid;
#endif
    G3DUIRENDERBUFFER rbuf;
    G3DUIMENU        *menuBar;
} G3DUIVIEW;

/******************************************************************************/
#define NBUVMAPBUTTON   0x02
#define BUTTONSIZE      0x10 /*** 16x16 ***/
#define M3DBOARDWIDTH          256
#define M3DPATTERNBOARDWIDTH   256
#define M3DPATTERNBOARDHEIGHT  128
#define M3DMENUBARHEIGHT        32
#define M3DTOOLBARHEIGHT        32
#define UVMAPTRANSLATEBUTTON 0x00
#define UVMAPZOOMBUTTON      0x01

typedef struct _M3DUI {
    uint64_t       engine_flags;
    G3DUIRECTANGLE rec[NBUVMAPBUTTON];       /*** pixmaps position ***/
    G3DUIRECTANGLE arearec;
    G3DUIRECTANGLE patternrec;
    G3DUIRECTANGLE fgbgrec;
    G3DUIRECTANGLE toolrec;
    int32_t        buttonID; /**** Currently clicked button = -1 if none ***/
    LIST          *lmenu;
    uint32_t       mode;
    LIST          *lseluv;
    LIST          *lseluvset;
    G3DCAMERA      cam;
#ifdef __linux__
    GLXContext     glctx;
#endif
#ifdef __MINGW32__
    HGLRC          glctx;
#endif
    /*G3DURMANAGER  *uvurm;*/
    G3DUI         *gui;
    char          *mask;
    char          *zbuffer;
    M3DPATTERN    *selpat;
    LIST         *lmtools; /*** list of mousetools widget ***/
} M3DUI;

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
#define UVMAPRECTHIT   0x07
#define TAGRECTHIT     0x08

#define MAXTEXTURES    0x20 /*** 32 textures at most - will be improved ***/
#define MAXUVMAPS      0x20 /*** 32 uvmaps at most - will be improved ***/
#define MAXTAGS        0x20 /*** 32 uvmaps at most - will be improved ***/

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
    G3DUIRECTANGLE uvmap[MAXUVMAPS];
    G3DUIRECTANGLE texture[MAXTEXTURES];
    G3DUIRECTANGLE tag[MAXTAGS];
} LISTEDOBJECT;

/******************************************************************************/
typedef struct _PICKEDOBJECT {
    G3DOBJECT  *obj;
    G3DTEXTURE *tex;
    G3DUVMAP   *uvmap;
    G3DTAG     *tag;
    uint32_t picked; 
} PICKEDOBJECT;



/******************************************************************************/
#define NBPATTERNS 0x0C
typedef struct _M3DUIPATTERNLIST {
    /*** List of pattern previews (those are ToolKit related) ***/
    LIST    *lpreview;
    uint32_t image_width;
    uint32_t image_height;
    uint32_t preview_width;
    uint32_t preview_height;
    uint32_t preview_border;
    M3DPATTERN *patterns[NBPATTERNS];
} M3DUIPATTERNLIST;

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
#define TIMEBOARDMOVE   "TIMEBOARDMOVE"
#define TIMEBOARDPAN    "TIMEBOARDPAN"
#define TIMEBOARDSCALE  "TIMEBOARDSCALE"

#define DEFAULTFRAMEGAP 0x10
#define MINIMUMFRAMEGAP 0x04
#define MAXIMUMFRAMEGAP 0x20

#define TIME_PAN_TOOL   0x00
#define TIME_MOVE_TOOL  0x01
#define TIME_SCALE_TOOL 0x02

/********************************** g3dui.c ***********************************/

G3DCAMERA          *g3dui_getMainViewCamera ( G3DUI *gui );
void g3dui_initDefaultMouseTools ( G3DUI *gui );
uint64_t            g3dui_undo ( G3DUI *gui );
uint64_t            g3dui_redo ( G3DUI *gui );
void                g3dui_init ( G3DUI     *gui,
                                 G3DUIMAIN *main );
uint64_t            g3dui_openG3DFile ( G3DUI      *gui, 
                                        const char *filename );
void                g3dui_createDefaultCameras ( G3DUI *gui );
void                g3dui_resetDefaultCameras ( G3DUI *gui );
void                g3dui_addMouseTool ( G3DUI          *gui, 
                                         G3DUIMOUSETOOL *mou );
G3DUIMOUSETOOL     *g3dui_getMouseTool ( G3DUI      *gui, 
                                         const char *name );
uint32_t            g3dui_setMouseTool ( G3DUI          *gui, 
                                         G3DCAMERA      *cam, 
                                         G3DUIMOUSETOOL *mou );
void                g3dui_setFileName ( G3DUI      *gui, 
                                        const char *filename );
void                g3dui_saveG3DFile ( G3DUI *gui );
G3DSCENE           *g3dui_importFileOk ( G3DUI      *gui, 
                                            const char *filedesc, 
                                            const char *filename );
uint32_t            g3dui_exportFileOk ( G3DUI      *gui,
                                            const char *filedesc, 
                                            const char *filename );
void                g3dui_exitOk ( G3DUI *gui );
uint32_t            g3dui_cancelRenderByID ( G3DUI   *gui, 
                                             uint64_t id );
uint32_t            g3dui_cancelRenderByScene ( G3DUI  *gui, 
                                                Q3DJOB *qjob );
G3DUIRENDERPROCESS *g3dui_getRenderProcessByJob ( G3DUI  *gui,
                                                  Q3DJOB *qjob );
G3DUIRENDERPROCESS *g3dui_getRenderProcessByID ( G3DUI   *gui,
                                                 uint64_t id );
void                g3dui_addRenderSettings ( G3DUI       *gui,
                                              Q3DSETTINGS *rsg );
void                g3dui_useRenderSettings ( G3DUI       *gui,
                                              Q3DSETTINGS *rsg );
uint32_t            g3dui_selectAll ( G3DUI *gui );
G3DUIRENDERPROCESS *g3dui_render_q3d ( G3DUI       *gui, 
                                       Q3DSETTINGS *rsg,
                                       Q3DFILTER   *towindow,
                                       Q3DFILTER   *toframe,
                                       Q3DFILTER   *tostatus,
                                       Q3DFILTER   *makepreview,
                                       G3DCAMERA   *cam,
                                       float        resetFrame,
                                       uint64_t     id,
                                       uint32_t     sequence,
                                       uint64_t     job_flags );
void                g3dui_processAnimatedImages ( G3DUI *gui );

uint64_t            g3dui_addVibratorTag ( G3DUI *gui );
uint64_t            g3dui_addTrackerTag ( G3DUI *gui );
uint64_t            g3dui_removeSelectedTag ( G3DUI *gui );
uint64_t            g3duimenubar_addUVMap ( G3DUI *gui );
uint64_t            g3duimenubar_fitUVMap ( G3DUI *gui );
uint64_t            g3duimenubar_alignUVMap ( G3DUI      *gui,
                                                 const char *option );
uint64_t            g3dui_mergeMesh ( G3DUI *gui );
uint64_t            g3dui_splitMesh ( G3DUI      *gui,
                                         const char *option );
uint64_t            g3dui_mirrorHeightmap ( G3DUI      *gui,
                                               const char *option );
uint64_t            g3dui_mirrorWeightGroup ( G3DUI      *gui,
                                                 const char *option );
uint64_t            g3dui_resetBoneTree ( G3DUI *gui );
uint64_t            g3dui_resetBone ( G3DUI *gui );
uint64_t            g3dui_fixBoneTree ( G3DUI *gui );
uint64_t            g3dui_fixBone ( G3DUI *gui );
uint64_t            g3dui_addBone ( G3DUI *gui );
uint64_t            g3dui_addSkin ( G3DUI *gui );
uint64_t            g3dui_addMorpher ( G3DUI *gui );
uint64_t            g3dui_addFFDBox ( G3DUI *gui );
uint64_t            g3dui_addSymmetry ( G3DUI *gui );
uint64_t            g3dui_addInstance ( G3DUI *gui );
uint64_t            g3dui_addEmitter ( G3DUI *gui );
uint64_t            g3dui_addSubdivider ( G3DUI *gui );
uint64_t            g3dui_addWireframe ( G3DUI *gui );
uint64_t            g3dui_addEmptyMesh ( G3DUI *gui );
uint64_t            g3dui_addSpline ( G3DUI *gui );
uint64_t            g3dui_addSplineRevolver ( G3DUI *gui );
uint64_t            g3dui_addText ( G3DUI *gui );
uint64_t            g3dui_addCamera ( G3DUI     *gui );
uint64_t            g3dui_addLight ( G3DUI *gui );
uint64_t            g3dui_addCylinder ( G3DUI *gui );
uint64_t            g3dui_addTube ( G3DUI *gui );
uint64_t            g3dui_addTorus ( G3DUI *gui );
uint64_t            g3dui_addSphere ( G3DUI *gui );
uint64_t            g3dui_addCube ( G3DUI *gui );
uint64_t            g3dui_addPlane ( G3DUI *gui );
uint64_t            g3dui_addNull ( G3DUI *gui );
uint64_t            g3dui_selectTree ( G3DUI      *gui,
                                          const char *option );
uint64_t            g3dui_triangulate ( G3DUI      *gui,
                                           const char *option );
uint64_t            g3dui_invertNormal ( G3DUI *gui );
uint64_t            g3dui_alignNormals ( G3DUI *gui );
uint64_t            g3dui_untriangulate ( G3DUI *gui );
uint64_t            g3dui_weldVertices ( G3DUI *gui );
uint64_t            g3dui_deleteLoneVertices ( G3DUI *gui );
uint64_t            g3dui_invertSelection ( G3DUI *gui );
uint64_t            g3dui_getObjectStats ( G3DUI   *gui, 
                                              char    *buffer, 
                                              uint32_t bufferlen );
uint64_t            g3dui_makeEditable ( G3DUI *gui );
uint64_t            g3dui_addUVMap ( G3DUI *gui );
uint64_t            g3dui_fitUVMap ( G3DUI *gui );
uint64_t            g3dui_alignUVMap ( G3DUI      *gui, 
                                          const char *option );
uint64_t            g3dui_setMaterial ( G3DUI *gui );
uint64_t            g3dui_redo ( G3DUI *gui );
uint64_t            g3dui_undo ( G3DUI *gui );
uint64_t            g3dui_closeScene ( G3DUI *gui );
uint64_t            g3dui_deleteSelection ( G3DUI *gui );

/******************************* g3duiboard.c ******************************/
void g3duiboard_resize ( G3DUIBOARD *board, 
                         uint32_t    width,
                         uint32_t    height );

/******************************* g3duiboneedit.c ******************************/


/***************************** g3duibonelinker.c ******************************/

/***************************** g3duicameraedit.c ******************************/

uint64_t g3duicameraedit_dofRadius ( G3DUICAMERAEDIT *camedit,
                                        uint32_t         radius );
uint64_t g3duicameraedit_dofNearBlur ( G3DUICAMERAEDIT *camedit,
                                          float            nearBlur );
uint64_t g3duicameraedit_dofNoBlur ( G3DUICAMERAEDIT *camedit,
                                        float            noBlur );
uint64_t g3duicameraedit_dofFarBlur ( G3DUICAMERAEDIT *camedit,
                                         float            farBlur );
uint64_t g3duicameraedit_dofEnable ( G3DUICAMERAEDIT *camedit );

/****************************** g3duiclipboard.c ******************************/

G3DUICOPIEDITEM *g3duicopieditem_new ( G3DOBJECT              *obj,
                                       G3DKEY                 *key,
                                       G3DFACESCULPTEXTENSION *fse,
                                       uint32_t                operation );
void             g3duicopieditem_clear ( G3DUICOPIEDITEM *item, 
                                         uint32_t         operation );
void             g3duicopieditem_free ( G3DUICOPIEDITEM *item );
G3DUICLIPBOARD  *g3duiclipboard_new ( );
void             g3duiclipboard_free ( G3DUICLIPBOARD *cli );
void             g3duiclipboard_clear ( G3DUICLIPBOARD *cli );
void             g3duiclipboard_copyObject ( G3DUICLIPBOARD *cli, 
                                             G3DSCENE       *sce,
                                             LIST           *lobj,
                                             int             recurse, 
                                             uint64_t        engine_flags );
void             g3duiclipboard_paste ( G3DUICLIPBOARD *cli,
                                        G3DURMANAGER   *urm,
                                        G3DSCENE       *sce,
                                        G3DOBJECT      *dst,
                                        uint32_t        flags );
void             g3duiclipboard_copyKey ( G3DUICLIPBOARD *cli, 
                                          G3DSCENE       *sce,
                                          G3DOBJECT      *obj,
                                          LIST           *lkey );
void             g3duiclipboard_copyFaceSculptExtension ( G3DUICLIPBOARD *cli, 
                                                          G3DSCENE       *sce,
                                                          G3DSUBDIVIDER  *sdr,
                                                          G3DFACE        *fac );

/******************************** g3duiconf.c *********************************/

void g3dui_loadConfiguration ( G3DUI *gui, 
                               char  *filename );


/*************************** g3duicoordinatesedit.c ***************************/

uint64_t g3duicoordinatesedit_pos ( G3DUICOORDINATESEDIT *coordedit, 
                                       G3DUIAXIS             axis, 
                                       uint32_t              absolute,
                                       float                 val );
uint64_t g3duicoordinatesedit_rot ( G3DUICOORDINATESEDIT *coordedit, 
                                       G3DUIAXIS             axis, 
                                       float                 val );
uint64_t g3duicoordinatesedit_sca ( G3DUICOORDINATESEDIT *coordedit, 
                                       G3DUIAXIS             axis, 
                                       float                 val );

/****************************** g3duicubeedit.c *******************************/

uint64_t g3duicubeedit_slice ( G3DUICUBEEDIT *cubedit,
                                  G3DUIAXIS      axis,
                                  int            slice );
uint64_t g3duicubeedit_radius ( G3DUICUBEEDIT *cubedit,
                                   float          radius );

/****************************** g3duicurrentedit.c ****************************/


/****************************** g3duicylinderedit.c ***************************/

uint64_t g3duicylinderedit_length ( G3DUICYLINDEREDIT *cyledit, 
                                       float              length );
uint64_t g3duicylinderedit_radius ( G3DUICYLINDEREDIT *cyledit, 
                                       float              radius );
uint64_t g3duicylinderedit_cap ( G3DUICYLINDEREDIT *cyledit, 
                                    G3DUIAXIS          axis,
                                    int                cap );
uint64_t g3duicylinderedit_slice ( G3DUICYLINDEREDIT *cyledit, 
                                      int                slice );

/********************************* g3duiexport.c ******************************/

uint32_t g3dui_write ( G3DEXPORTV3DATA *ged, 
                       G3DUI         *gui, 
                       uint32_t       flags, 
                       FILE          *fdst );
void     g3dui_read ( G3DIMPORTV3DATA *gid, 
                      uint32_t       chunkEnd, 
                      FILE          *fsrc,
                      G3DUI         *gui );

/********************************* g3duiffdedit.c *****************************/

uint64_t g3duiffdedit_radius ( G3DUIFFDEDIT *ffdedit,
                                  G3DUIAXIS     axis,
                                  float         radius );
uint64_t g3duiffdedit_slice ( G3DUIFFDEDIT *ffdedit,
                                 G3DUIAXIS     axis,
                                 int           slice );

/***************************** g3duiinstanceedit.c ****************************/

uint64_t g3duiinstanceedit_setReference ( G3DUIINSTANCEEDIT *insedit, 
                                             uint32_t           rank );
uint64_t g3duiinstanceedit_mirroredToggle ( G3DUIINSTANCEEDIT *insedit );
uint64_t g3duiinstanceedit_orientation ( G3DUIINSTANCEEDIT *insedit,
                                            char              *str );

/******************************** g3duikeyedit.c ******************************/

uint64_t g3duikeyedit_setKeyTransformations ( G3DUIKEYEDIT *keyedit,
                                                 uint32_t      flag );
uint64_t g3duikeyedit_unsetKeyTransformations ( G3DUIKEYEDIT *keyedit,
                                                   uint32_t      flag );
uint64_t g3duikeyedit_loopFrame ( G3DUIKEYEDIT *keyedit,
                                     float         frame );
uint64_t g3duikeyedit_loop ( G3DUIKEYEDIT *keyedit,
                                uint32_t      loop );
uint64_t g3duikeyedit_key ( G3DUIKEYEDIT *keyedit,
                               const char   *field,
                               float         val );

/******************************* g3duilightedit.c *****************************/

uint64_t g3duilightedit_setSpecularity ( G3DUILIGHTEDIT *ligedit,
                                            uint32_t        red,
                                            uint32_t        green,
                                            uint32_t        blue );
uint64_t g3duilightedit_unsetSpot ( G3DUILIGHTEDIT *ligedit );
uint64_t g3duilightedit_setSpot ( G3DUILIGHTEDIT *ligedit );
uint64_t g3duilightedit_setSpotAngle ( G3DUILIGHTEDIT *ligedit, float spotAngle );
uint64_t g3duilightedit_setSpotFadeAngle ( G3DUILIGHTEDIT *ligedit, float spotFadeAngle );
uint64_t g3duilightedit_setSpotLength ( G3DUILIGHTEDIT *ligedit, float spotLength );
uint64_t g3duilightedit_castShadows ( G3DUILIGHTEDIT *ligedit );
uint64_t g3duilightedit_setSoftShadows ( G3DUILIGHTEDIT *ligedit );
uint64_t g3duilightedit_shadowRadius ( G3DUILIGHTEDIT *ligedit,
                                          float           shadowRadius );
uint64_t g3duilightedit_shadowSample ( G3DUILIGHTEDIT *ligedit,
                                          uint32_t        shadowSample );
uint64_t g3duilightedit_setDiffuse ( G3DUILIGHTEDIT *ligedit,
                                        uint32_t        red,
                                        uint32_t        green,
                                        uint32_t        blue );

/********************************* g3duimain.c ********************************/

void g3duimain_resize ( G3DUIMAIN *gmn, 
                        uint32_t   width, 
                        uint32_t   height );

/***************************** g3duimaterialboard.c ***************************/

void g3duimaterialboard_resize ( G3DUIMATERIALBOARD *matboard, 
                                 uint32_t            width,
                                 uint32_t            height );

/****************************** g3duimaterialedit.c ***************************/

uint64_t g3duimaterialedit_setDiffuseColor ( G3DUIMATERIALEDIT *matedit, 
                                                float              R,
                                                float              G,
                                                float              B,
                                                float              A );
uint64_t g3duimaterialedit_setSpecularColor ( G3DUIMATERIALEDIT *matedit, 
                                                 float              R,
                                                 float              G,
                                                 float              B,
                                                 float              A );
uint64_t g3duimaterialedit_setDisplacementStrength ( G3DUIMATERIALEDIT *matedit,
                                                        float              strength );
uint64_t g3duimaterialedit_setBumpStrength ( G3DUIMATERIALEDIT *matedit, 
                                                float              strength );
uint64_t g3duimaterialedit_setReflectionStrength ( G3DUIMATERIALEDIT *matedit,
                                                      float              strength );
uint64_t g3duimaterialedit_setRefractionStrength ( G3DUIMATERIALEDIT *matedit, 
                                                      float              strength );
uint64_t g3duimaterialedit_setAlphaStrength ( G3DUIMATERIALEDIT *matedit, 
                                                 float              strength );
uint64_t g3duimaterialedit_setName ( G3DUIMATERIALEDIT *matedit,
                                        const char        *name );
uint64_t g3duimaterialedit_setSpecularLevel ( G3DUIMATERIALEDIT *matedit,
                                                 float              val );
uint64_t g3duimaterialedit_setSpecularShininess ( G3DUIMATERIALEDIT *matedit,
                                                     float              val );
uint64_t g3duimaterialedit_channelChooseImage ( G3DUIMATERIALEDIT *matedit,
                                                   uint32_t           channelID,
                                                   char              *filename,
                                                   uint32_t           bindGL );
uint64_t g3duimaterialedit_enableProcedural ( G3DUIMATERIALEDIT *matedit,
                                                 uint32_t           channelID );
uint64_t g3duimaterialedit_enableSolidColor ( G3DUIMATERIALEDIT *matedit,
                                                 uint32_t           channelID );
uint64_t g3duimaterialedit_enableImage ( G3DUIMATERIALEDIT *matedit,
                                            uint32_t           channelID );
uint64_t g3duimaterialedit_chooseProcedural ( G3DUIMATERIALEDIT *matedit,
                                                 uint32_t           channelID,
                                                 const char        *procType,
                                                 const char        *procRes,
                                                 uint32_t           bindGL );
uint64_t g3duimaterialedit_toggleDisplacement ( G3DUIMATERIALEDIT *matedit );
uint64_t g3duimaterialedit_toggleBump ( G3DUIMATERIALEDIT *matedit );
uint64_t g3duimaterialedit_toggleAlpha ( G3DUIMATERIALEDIT *matedit );

/****************************** g3duimateriallist.c ***************************/

void g3duimateriallist_init ( G3DUIMATERIALLIST *matlist,
                              G3DUI             *gui,
                              uint32_t           image_width,
                              uint32_t           image_height,
                              uint32_t           preview_per_line );
G3DUIMATERIALPREVIEW *g3duimateriallist_pickPreview ( G3DUIMATERIALLIST *matlist,
                                                      uint32_t           x,
                                                      uint32_t           y );
uint32_t g3duimateriallist_arrangePreviews ( G3DUIMATERIALLIST *matlist );
G3DUIMATERIALPREVIEW *g3duimateriallist_getPreview ( G3DUIMATERIALLIST *matlist,
                                                     G3DMATERIAL       *mat );
void g3duimateriallist_removePreview ( G3DUIMATERIALLIST    *matlist, 
                                       G3DUIMATERIALPREVIEW *preview );
void g3duimateriallist_removeAllPreviews ( G3DUIMATERIALLIST *matlist );
G3DUIMATERIALMAP *g3duimaterialmap_new ( uint32_t width,
                                         uint32_t height );
void g3duimaterialmap_buildSphere ( G3DUIMATERIALMAP *matmap,
                                    G3DMATERIAL      *mat,
                                    float             radius );
void g3duimaterialmap_fillData ( G3DUIMATERIALMAP *matmap,
                                 G3DMATERIAL      *mat,
                                 unsigned char (*data)[3] );
void g3duimateriallist_removeMaterial ( G3DUIMATERIALLIST *matlist,
                                        G3DSCENE          *sce,
                                        G3DURMANAGER      *urm,
                                        G3DMATERIAL       *mat );
void g3duimateriallist_addPreview ( G3DUIMATERIALLIST    *matlist, 
                                    G3DUIMATERIALPREVIEW *preview );

/*************************** g3duimaterialnameedit.c **************************/

/********************************* g3duimenu.c ********************************/

/******************************** g3duimeshedit.c *****************************/

uint64_t g3duimeshedit_useIsoLines ( G3DUIMESHEDIT *mesedit );
uint64_t g3duimeshedit_gouraud ( G3DUIMESHEDIT *mesedit,
                                    float          scalarLimit );
uint64_t g3duimeshedit_toggleShading ( G3DUIMESHEDIT *mesedit );

/****************************** g3duimeshposelist.c ***************************/

/********************************* g3duimodebar.c *****************************/

uint64_t g3duimodebar_setMode ( G3DUIMODEBAR *gmb,
                                   const char   *modename );
uint64_t m3duimodebar_setMode ( M3DUIMODEBAR *mmb, 
                                   const char   *modename );

/******************************* g3duimousetool.c *****************************/
G3DUIMOUSETOOL *g3duimousetool_new ( G3DMOUSETOOL *tool,
                                     G3DUIMENU    *menu,
                                     uint32_t      uiflags );
void g3duimousetool_free ( G3DUIMOUSETOOL *mou );

/******************************** g3duiobjectedit.c ***************************/

uint64_t g3duiobjectedit_name ( G3DUIOBJECTEDIT *goe,
                                   const char      *name );

/******************************** g3duiobjectboard.c **************************/

void g3duiobjectboard_resize ( G3DUIOBJECTBOARD *objbrd, 
                               uint32_t          width,
                               uint32_t          height );

/******************************* g3duiobjectlist.c ****************************/

LISTEDOBJECT *g3duiobjectlist_sizeListedObject ( G3DUIOBJECTLIST *objlis,
                                                 G3DOBJECT       *obj, 
                                                 uint32_t         x, 
                                                 uint32_t         y,
                                                 uint32_t         xsep,
                                                 uint32_t         strwidth,
                                                 uint32_t         query_flags );
void g3duiobjectlist_init ( G3DUIOBJECTLIST *objlist, G3DUI *gui );

uint32_t g3duiobjectlist_initListedObject ( G3DUIOBJECTLIST *objlis,
                                            LISTEDOBJECT    *lob,
                                            uint32_t         x, 
                                            uint32_t         y,
                                            uint32_t         strwidth );

PICKEDOBJECT *g3duiobjectlist_pickObject ( G3DUIOBJECTLIST *objlist,
                                           uint32_t         x, 
                                           uint32_t         y,
                                           uint32_t         xsep,
                                           uint32_t         xm,
                                           uint32_t         ym,
                                           uint32_t         strwidth,
                                           G3DOBJECT       *obj,
                                           G3DSCENE        *sce,
                                           G3DURMANAGER    *urm,
                                           uint32_t         pick_flags,
                                           uint64_t         engine_flags );

/*************************** g3duiparticleemitteredit.c ***********************/

uint64_t g3duiparticleemitteredit_startAtFrame ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                    uint32_t                  startAtFrame );
uint64_t g3duiparticleemitteredit_endAtFrame ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                  uint32_t                  endAtFrame );
uint64_t g3duiparticleemitteredit_radius ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                              float                     radius );
uint64_t g3duiparticleemitteredit_lifetime ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                uint32_t                  lifetime );
uint64_t g3duiparticleemitteredit_ppf ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                           float                     ppf );
uint64_t g3duiparticleemitteredit_displayPart ( G3DUIPARTICLEEMITTEREDIT *gpeedit );
uint64_t g3duiparticleemitteredit_maxPreviews ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   uint32_t                  maxPreviews );
uint64_t g3duiparticleemitteredit_gravityForceX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     g );
uint64_t g3duiparticleemitteredit_gravityForceY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     g );
uint64_t g3duiparticleemitteredit_gravityForceZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     g );
uint64_t g3duiparticleemitteredit_initialVarTransp ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                        float                     initialVarTransp );
uint64_t g3duiparticleemitteredit_initialVarRotX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                      float                     initialVarRotX );
uint64_t g3duiparticleemitteredit_initialVarRotY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                      float                     initialVarRotY );
uint64_t g3duiparticleemitteredit_initialVarRotZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                      float                     initialVarRotZ );
uint64_t g3duiparticleemitteredit_initialVarScaX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                      float                     initialVarScaX );
uint64_t g3duiparticleemitteredit_initialVarScaY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                      float                     initialVarScaY );
uint64_t g3duiparticleemitteredit_initialVarScaZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                      float                     initialVarScaZ );
uint64_t g3duiparticleemitteredit_initialVarSpeedX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                        float                     initialVarSpeedX );
uint64_t g3duiparticleemitteredit_initialVarSpeedY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                        float                     initialVarSpeedY );
uint64_t g3duiparticleemitteredit_initialVarSpeedZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                        float                     initialVarSpeedZ );
uint64_t g3duiparticleemitteredit_initialVarAccelX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                        float                     initialVarAccelX );
uint64_t g3duiparticleemitteredit_initialVarAccelY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                        float                     initialVarAccelY );
uint64_t g3duiparticleemitteredit_initialVarAccelZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                        float                     initialVarAccelZ );
uint64_t g3duiparticleemitteredit_finalTransp ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalTransp );
uint64_t g3duiparticleemitteredit_finalRotX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                 float                     finalRotX );
uint64_t g3duiparticleemitteredit_finalRotY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                 float                     finalRotY );
uint64_t g3duiparticleemitteredit_finalRotZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                 float                     finalRotZ );
uint64_t g3duiparticleemitteredit_finalScaX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                 float                     finalScaX );
uint64_t g3duiparticleemitteredit_finalScaY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                 float                     finalScaY );
uint64_t g3duiparticleemitteredit_finalScaZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                 float                     finalScaZ );
uint64_t g3duiparticleemitteredit_finalSpeedX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalSpeedX );
uint64_t g3duiparticleemitteredit_finalSpeedY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalSpeedY );
uint64_t g3duiparticleemitteredit_finalSpeedZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalSpeedZ );
uint64_t g3duiparticleemitteredit_finalAccelX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalAccelX );
uint64_t g3duiparticleemitteredit_finalAccelY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalAccelY );
uint64_t g3duiparticleemitteredit_finalAccelZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalAccelZ );
uint64_t g3duiparticleemitteredit_initialTransp ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialTransp );
uint64_t g3duiparticleemitteredit_initialRotX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialRotX );
uint64_t g3duiparticleemitteredit_initialRotY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialRotY );
uint64_t g3duiparticleemitteredit_initialRotZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialRotZ );
uint64_t g3duiparticleemitteredit_initialScaX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialScaX );
uint64_t g3duiparticleemitteredit_initialScaY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialScaY );
uint64_t g3duiparticleemitteredit_initialScaZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialScaZ );
uint64_t g3duiparticleemitteredit_initialSpeedX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialSpeedX );
uint64_t g3duiparticleemitteredit_initialSpeedY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialSpeedY );
uint64_t g3duiparticleemitteredit_initialSpeedZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialSpeedZ );
uint64_t g3duiparticleemitteredit_initialAccelX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialAccelX );
uint64_t g3duiparticleemitteredit_initialAccelY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialAccelY );
uint64_t g3duiparticleemitteredit_initialAccelZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialAccelZ );

/********************************* g3duiplaneedit.c ***************************/

uint64_t g3duiplaneedit_div  ( G3DUIPLANEEDIT *plnedit,
                                  G3DUIAXIS       axis,
                                  int             div );
uint64_t g3duiplaneedit_radius ( G3DUIPLANEEDIT *plnedit,
                                    G3DUIAXIS       axis,
                                    float           radius );
uint64_t g3duiplaneedit_orientation ( G3DUIPLANEEDIT *plnedit,
                                         const char     *oristr );

/*********************************** g3duiquad.c ******************************/

void g3duiquad_resize ( G3DUIQUAD *quad,
                        uint32_t   width, 
                        uint32_t   height );
void g3duiquad_init ( G3DUIQUAD *quad );

/****************************** g3duirenderbuffer.c ***************************/
#ifdef __MINGW32__
void g3duirenderbuffer_clear ( G3DUIRENDERBUFFER *rbuf );
void g3duirenderbuffer_init ( G3DUIRENDERBUFFER *rbuf,
                              HWND               hWnd,
                              uint32_t           width,
                              uint32_t           height );
#endif
#ifdef __linux__
void g3duirenderbuffer_clear ( G3DUIRENDERBUFFER *rbuf );
void g3duirenderbuffer_init ( G3DUIRENDERBUFFER *rbuf,
                              Display            *dis,
                              Window              win,
                              uint32_t            width,
                              uint32_t            height );
#endif

/******************************** g3duirenderedit.c ***************************/

uint64_t g3duirenderedit_aa ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_aaFull ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_aaEdge ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_aaSamples ( G3DUIRENDEREDIT *redit,
                                        uint32_t         nbsamples );
uint64_t g3duirenderedit_format ( G3DUIRENDEREDIT *redit,
                                     const char      *format );
uint64_t g3duirenderedit_startFrame ( G3DUIRENDEREDIT *redit,
                                         float            frame );
uint64_t g3duirenderedit_endFrame ( G3DUIRENDEREDIT *redit,
                                       float            frame );
uint64_t g3duirenderedit_setMotionBlur ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_setTexturing ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_texturingColor ( G3DUIRENDEREDIT *redit,
                                             unsigned char    R,
                                             unsigned char    G,
                                             unsigned char    B );
uint64_t g3duirenderedit_setWireframe ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_setWireframeLighting ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_wireframeThickness ( G3DUIRENDEREDIT *redit,
                                                 float  thickness );
uint64_t g3duirenderedit_wireframeColor ( G3DUIRENDEREDIT *redit, 
                                             unsigned char R, 
                                             unsigned char G, 
                                             unsigned char B );
uint64_t g3duirenderedit_setFogStrength ( G3DUIRENDEREDIT *redit, 
                                             float            strength );
uint64_t g3duirenderedit_setFog ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_setFogAffectsBackground ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_fogFar ( G3DUIRENDEREDIT *redit,
                                     float            ffar );
uint64_t g3duirenderedit_fogNear ( G3DUIRENDEREDIT *redit,
                                      float            fnear );
uint64_t g3duirenderedit_fogColor ( G3DUIRENDEREDIT *redit, 
                                       unsigned char    R, 
                                       unsigned char    G, 
                                       unsigned char    B );
uint64_t g3duirenderedit_sceneMotionBlur ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_vectorMotionBlur ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_sceneMotionBlurIteration ( G3DUIRENDEREDIT *redit, 
                                                       uint32_t nbstep );
uint64_t g3duirenderedit_motionBlurStrength ( G3DUIRENDEREDIT *redit,
                                                 float            strength );
uint64_t g3duirenderedit_vectorMotionBlurSamples ( G3DUIRENDEREDIT *redit, 
                                                      uint32_t         nbSamples );
uint64_t g3duirenderedit_vectorMotionBlurSubSamplingRate ( G3DUIRENDEREDIT *redit, 
                                                              float            subSamplingRate );
uint64_t g3duirenderedit_fps ( G3DUIRENDEREDIT *redit,
                                  uint32_t         fps );
uint64_t g3duirenderedit_output ( G3DUIRENDEREDIT *redit, 
                                     const char      *outfile );
uint64_t g3duirenderedit_preview ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_save ( G3DUIRENDEREDIT *redit,
                                   uint32_t         save );
uint64_t g3duirenderedit_width ( G3DUIRENDEREDIT *redit, 
                                    uint32_t         width );
uint64_t g3duirenderedit_height ( G3DUIRENDEREDIT *redit,
                                     uint32_t         height );
uint64_t g3duirenderedit_ratio ( G3DUIRENDEREDIT *redit, 
                                    float            ratio );
uint64_t g3duirenderedit_background ( G3DUIRENDEREDIT *redit, 
                                         unsigned char    R, 
                                         unsigned char    G, 
                                         unsigned char    B );
uint64_t g3duirenderedit_setBackgroundColorMode ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_setBackgroundImageMode ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_setBackgroundImage ( G3DUIRENDEREDIT *redit,
                                                 char            *filename );

/****************************** g3duirenderprocess.c **************************/

void g3duirenderprocess_savejpg ( G3DUIRENDERPROCESS *rps, 
                                  char               *filename );
G3DUIRENDERPROCESS *g3duirenderprocess_new ( uint64_t  id );
void g3duirenderprocess_init ( G3DUIRENDERPROCESS *rps,
                               G3DUI              *gui,
                               Q3DJOB             *qjob,
                               Q3DFILTER          *towindow,
                               Q3DFILTER          *toframe );
void g3duirenderprocess_free ( G3DUIRENDERPROCESS *rps );
void *g3duirenderprocess_render_frame_t ( G3DUIRENDERPROCESS *rps );
void *g3duirenderprocess_render_sequence_t ( G3DUIRENDERPROCESS *rps );
void g3duirenderprocess_filename ( G3DUIRENDERPROCESS *rps,
                                   char               *filename );

/****************************** g3duirenderwindow.c ***************************/
void g3duirenderwindow_resize ( G3DUIRENDERWINDOW *rwin,
                                uint32_t           width, 
                                uint32_t           height );

/******************************* g3duisphereedit.c ****************************/

uint64_t g3duisphereedit_togglePerfect ( G3DUISPHEREEDIT *sphedit );
uint64_t g3duisphereedit_radius ( G3DUISPHEREEDIT *sphedit,
                                     float            radius );
uint64_t g3duisphereedit_cap ( G3DUISPHEREEDIT *sphedit,
                                  uint32_t         cap );
uint64_t g3duisphereedit_slice ( G3DUISPHEREEDIT *sphedit,
                                    uint32_t         slice );

/************************** g3duisplinerevolveredit.c *************************/

uint64_t g3duisplinerevolveredit_splineRevolverSteps ( G3DUISPLINEREVOLVEREDIT *srvedit,
                                                          uint32_t                 level );
uint64_t g3duisplinerevolveredit_splineRevolverDivis ( G3DUISPLINEREVOLVEREDIT *srvedit,  
                                                          uint32_t                 level );

/**************************** g3duisubdivideredit.c ***************************/

uint64_t g3duisubdivideredit_displacementHeight ( G3DUISUBDIVIDEREDIT *subedit );
uint64_t g3duisubdivideredit_displacementSculpt ( G3DUISUBDIVIDEREDIT *subedit );
uint64_t g3duisubdivideredit_subdivSync ( G3DUISUBDIVIDEREDIT *subedit );
uint64_t g3duisubdivideredit_useIsoLines ( G3DUISUBDIVIDEREDIT *subedit );
uint64_t g3duisubdivideredit_subdivRender ( G3DUISUBDIVIDEREDIT *subedit,
                                               uint32_t             level );
uint64_t g3duisubdivideredit_subdivPreview ( G3DUISUBDIVIDEREDIT *subedit,
                                                uint32_t             level );

/***************************** g3duisymmetryedit.c ****************************/

uint64_t g3duisymmetryedit_limit ( G3DUISYMMETRYEDIT *symedit,
                                      float              limit );
uint64_t g3duisymmetryedit_plane ( G3DUISYMMETRYEDIT *symedit,
                                      char              *orientation );

/******************************* g3duitextedit.c ******************************/

uint64_t g3duitextedit_setText ( G3DUITEXTEDIT *txtedit,
                                    char          *newText );
uint64_t g3duitextedit_thickness ( G3DUITEXTEDIT *txtedit,
                                      float          thickness );
uint64_t g3duitextedit_roundness ( G3DUITEXTEDIT *txtedit,
                                      uint32_t       roundness );
uint64_t g3duitextedit_size ( G3DUITEXTEDIT *txtedit,
                                 uint32_t       size );

/****************************** g3duitextureedit.c ****************************/

uint64_t g3duitextureedit_toggleRestrict ( G3DUITEXTUREEDIT *texedit );
uint64_t g3duitextureedit_toggleRepeat ( G3DUITEXTUREEDIT *texedit );
uint64_t g3duitextureedit_setUVMap ( G3DUITEXTUREEDIT *texedit,
                                        uint32_t         rank );

/******************************* g3duitimeboard.c *****************************/

uint64_t g3duitimeboard_stop ( G3DUITIMEBOARD *tbrd );
uint64_t g3duitimeboard_gotoFrame ( G3DUITIMEBOARD *tbrd );
uint64_t g3duitimeboard_recordFrame ( G3DUITIMEBOARD *tbrd,
                                         uint32_t        key_flags );
void g3duitimeboard_resize ( G3DUITIMEBOARD *tbrd, 
                             uint32_t        width, 
                             uint32_t        height );

/******************************* g3duitimeline.c ******************************/

uint64_t g3duitimeline_scaleSelectedKeys ( G3DUITIMELINE *tim, 
                                              float  factor, 
                                              float  reference );
uint64_t g3duitimeline_deleteSelectedKeys ( G3DUITIMELINE *tim );
uint64_t g3duitimeline_selectAllKeys ( G3DUITIMELINE *tim );
uint32_t g3duitimeline_selectKey ( G3DUITIMELINE *tim,
                                   int           frame,
                                   int           keep,
                                   int           range,
                                   int           width );
uint32_t g3duitimeline_isOnKey ( G3DUITIMELINE *tim, 
                                 int            frame );
int32_t g3duitimeline_getFramePos ( G3DUITIMELINE *tim, 
                                    float          frame, 
                                    int            width );
int32_t g3duitimeline_getFrame ( G3DUITIMELINE *tim,
                                 int            x, 
                                 int            y, 
                                 int            width );
uint32_t g3duitimeline_onFrame ( G3DUITIMELINE *tim,
                                 float          curframe,
                                 int            x, 
                                 int            y, 
                                 int            width );
void g3duitimeline_init ( G3DUITIMELINE *tim,
                          G3DUI         *gui );
void g3duitimeline_decZoom ( G3DUITIMELINE *tim,
                             uint32_t       nbpix );
void g3duitimeline_incZoom ( G3DUITIMELINE *tim,
                             uint32_t       nbpix );
void g3duitimeline_setTool ( G3DUITIMELINE *tim,
                             uint32_t       tool );


/******************************* g3duitorusedit.c *****************************/

uint64_t g3duitorusedit_slice ( G3DUITORUSEDIT *toredit,
                                   uint32_t        slice );
uint64_t g3duitorusedit_cap ( G3DUITORUSEDIT *toredit,
                                 uint32_t        cap );
uint64_t g3duitorusedit_extRadius ( G3DUITORUSEDIT *toredit,
                                       float           extrad );
uint64_t g3duitorusedit_intRadius ( G3DUITORUSEDIT *toredit,
                                       float           intrad );
uint64_t g3duitorusedit_orientation ( G3DUITORUSEDIT *toredit,
                                         const char     *oristr );

/***************************** g3duitrackertagedit.c **************************/

uint64_t g3duitrackertagedit_orientation ( G3DUITRACKERTAGEDIT *ttedit, 
                                              char                *str );
uint64_t g3duitrackertagedit_setTarget ( G3DUITRACKERTAGEDIT *ttedit, 
                                            uint32_t             rank );

/******************************** g3duitubeedit.c *****************************/

uint64_t g3duitubeedit_length ( G3DUITUBEEDIT *tubedit,
                                   float          length );
uint64_t g3duitubeedit_radius ( G3DUITUBEEDIT *tubedit,
                                   float          radius );
uint64_t g3duitubeedit_thickness ( G3DUITUBEEDIT *tubedit,
                                      float          thickness );
uint64_t g3duitubeedit_cap ( G3DUITUBEEDIT *tubedit,
                                G3DUIAXIS      axis,
                                uint32_t       cap );
uint64_t g3duitubeedit_slice ( G3DUITUBEEDIT *tubedit,
                                  uint32_t       slice );

/******************************** g3duiuvmapedit.c ****************************/

uint64_t g3duiuvmapedit_projection ( G3DUIUVMAPEDIT *uvedit,
                                        const char     *projection );
uint64_t g3duiuvmapedit_lockUVMap ( G3DUIUVMAPEDIT *uvedit );

/*********************************** g3duiview.c ******************************/

uint64_t g3duiview_orbit ( G3DUIVIEW *view,
                           G3DPIVOT  *piv,
                           float      diffx, 
                           float      diffy );
uint64_t g3duiview_spin ( G3DUIVIEW *view, 
                          float      diffx );
uint64_t g3duiview_zoom ( G3DUIVIEW *view,
                          float      diffx );
uint64_t g3duiview_moveSideward ( G3DUIVIEW *view,
                                  float diffx, 
                                  float diffy );
uint64_t g3duiview_moveForward ( G3DUIVIEW *view, 
                                 float      diffx );

void g3duiview_pressButton ( G3DUIVIEW *view, 
                             int        x,
                             int        y );
void g3duiview_releaseButton ( G3DUIVIEW *view );
void g3duiview_init ( G3DUIVIEW *view,
                      uint32_t   width,
                      uint32_t   height,
                      uint32_t   menuHeight );
void g3duiview_resize ( G3DUIVIEW *view, 
                        uint32_t   width, 
                        uint32_t   height,
                        uint32_t   menuHeight );
void g3duiview_sizeGL ( G3DUIVIEW *view, 
                        uint32_t   width, 
                        uint32_t   height );
void g3duiview_initGL ( G3DUIVIEW *view );
void g3duiview_useSelectedCamera ( G3DUIVIEW *view, 
                                   G3DCAMERA *cam );
void g3duiview_showRenderingArea ( G3DUIVIEW *view,
                                   uint64_t engine_flags );
void g3duiview_showGL ( G3DUIVIEW    *view,
                        uint64_t      engine_flags );

uint64_t g3duiview_setShading ( G3DUIVIEW *view,
                                   char      *shading );

/*************************** g3duiweightgrouplist.c ***************************/

G3DWEIGHTGROUP *g3duiweightgrouplist_getWeightGroup ( G3DUIWEIGHTGROUPLIST *wgrplist,
                                                      G3DMESH              *mes, 
                                                      float                mouse_x,
                                                      float                mouse_y );
uint64_t g3duiweightgrouplist_deleteWeightGroup ( G3DUIWEIGHTGROUPLIST *wgrplist );
uint64_t g3duiweightgrouplist_addWeightGroup ( G3DUIWEIGHTGROUPLIST *wgrplist );
uint64_t g3duiweightgrouplist_name ( G3DUIWEIGHTGROUPLIST *wgrplist,
                                        const char           *grpname );
uint64_t g3duiweightgrouplist_deleteSelected ( G3DUIWEIGHTGROUPLIST *wgrplist );
uint64_t g3duiweightgrouplist_select ( G3DUIWEIGHTGROUPLIST *wgrplist,
                                          G3DWEIGHTGROUP       *grp );

/******************************* g3duiwireframeedit.c ****************************/

uint64_t g3duiwireframeedit_thickness ( G3DUIWIREFRAMEEDIT *wfmedit,
                                           float               thickness );

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

/**************************** menu/g3duimainmenu.c ****************************/
G3DUIMENU *g3duimenu_getMainMenuNode ( );

/************************ menu/g3duimaterialboardmenu.c ***********************/
G3DUIMENU *g3duimenu_getMaterialBoardMenuNode ( );

/************************* menu/g3duiobjectboardmenu.c ************************/
G3DUIMENU *g3duimenu_getObjectBoardMenuNode ( );

/************************* menu/g3duirenderwindowmenu.c ***********************/
G3DUIMENU *g3duimenu_getRenderWindowMenuNode ( );

/************************* menu/g3duirenderwindowmenu.c ***********************/
G3DUIMENU *g3duimenu_getRenderWindowMenuNode ( );

/************************** menu/g3duitimelinemenu.c **************************/
G3DUIMENU *g3duimenu_getTimelineMenuNode ( );

/************************** menu/g3duiuveditormenu.c **************************/
G3DUIMENU *g3duimenu_getUVEditorMenuNode ( );

/************************** menu/g3duiviewmenu.c **************************/
G3DUIMENU *g3duimenu_getViewMenuNode ( );

/******************************************************************************/
Q3DFILTER *q3dfilter_preview_new ( G3DUI *gui );


















/********************************* M3DUI **************************************/
uint64_t m3dui_redo ( M3DUI *mui );
uint64_t m3dui_undo ( M3DUI *mui );
uint64_t m3dui_fillWithColor ( M3DUI   *mui, 
                               uint32_t color );
uint64_t m3dui_fac2uvset ( M3DUI *mui );
uint64_t m3dui_uv2ver ( M3DUI *mui );
uint64_t m3dui_ver2uv ( M3DUI *mui );
uint64_t m3dui_uvset2fac ( M3DUI *mui );
G3DCHANNEL *m3dui_getWorkingChannel ( M3DUI *mui );
void m3dui_resizeBuffers ( M3DUI *mui );
void m3dui_setCanevas ( M3DUI *mui );
void m3duiview_showGL ( M3DUI        *mui,
                        M3DMOUSETOOL *mou,
                        uint64_t      engine_flags );

#endif
