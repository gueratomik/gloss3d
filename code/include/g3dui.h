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
 
/**** Widget names for SphereEdit TextField widgets ***/
#define EDITSPHERE        "Edit Sphere"
#define EDITSPHERECAPS    "Caps"
#define EDITSPHERESLICES  "Slices"
#define EDITSPHERERADIUS  "Radius"
#define EDITSPHEREPERFECT "Render Perfect"

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

/**** Widget names for LightEdit TextField widget ***/
#define EDITLIGHT                  "Light"

/**** Widget names for CylinderEdit TextField widget ***/
#define EDITCYLINDER       "Edit Cylinder"
#define EDITCYLINDERXCAPS  "X Caps"
#define EDITCYLINDERYCAPS  "Y Caps" 
#define EDITCYLINDERSLICES "Slices"
#define EDITCYLINDERLENGTH "Length"
#define EDITCYLINDERRADIUS "Radius"

/**** Widget names for TubeEdit TextField widget ***/
#define EDITTUBE       "Edit Tube"
#define EDITTUBEXCAPS  "X Caps"
#define EDITTUBEYCAPS  "Y Caps" 
#define EDITTUBESLICES "Slices"
#define EDITTUBELENGTH "Length"
#define EDITTUBERADIUS "Radius"
#define EDITTUBETHICKNESS "Thickness"

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

#define EDITWIREFRAME          "Wireframe"
#define EDITWIREFRAMETHICKNESS "Thickness"

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

typedef struct _G3DUIMENU    G3DUIMENU;
typedef struct _G3DUIVIEW    G3DUIVIEW;
typedef struct _G3DUIMODEBAR G3DUIMODEBAR;
typedef struct _G3DUIQUAD    G3DUIQUAD;
typedef struct _G3DUIMAIN    G3DUIMAIN;

/********************************* G3DUI **************************************/
typedef struct _G3DUI {
    G3DMOUSETOOL *mou; /*** current mouse tool ***/
    G3DMOUSETOOL *uvmou; /*** current mouse tool for UV editor ***/
    G3DUICLIPBOARD *cli;
    G3DSCENE     *sce;
    G3DUICONF     conf;
    uint64_t      engine_flags;

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

typedef struct _G3DUIMENU {
    G3DUI             *gui;
    unsigned char     *name;
    uint32_t           type;
    uint32_t         (*condition) ( struct _G3DUIMENU *menu, void *data );
    uint64_t          (*callback) ( struct _G3DUIMENU *menu, void *data );
    LIST               *lchildren;
    void               *data;
    struct _G3DUIMENU *nodes[];
} G3DUIMENU;

/******************************************************************************/
typedef struct _G3DUITOOLBAR {
    G3DUI *gui;
} G3DUITOOLBAR;

/******************************************************************************/
typedef struct _G3DUITIMEBOARD {
    G3DUI *gui;
} G3DUITIMEBOARD;

/******************************************************************************/
typedef struct _G3DUIMESHPOSELIST {
    G3DUI *gui;
} G3DUIMESHPOSELIST;

/******************************************************************************/
typedef struct _G3DUIOBJECTLIST {
    G3DUI *gui;
    uint32_t oldWidth,
             oldHeight;
} G3DUIOBJECTLIST;

/******************************************************************************/
typedef struct _G3DUITIMELINE {
    G3DUI *gui;
    int32_t midframe; /*** the frame in the middle of the widget     ***/
    uint32_t nbpix;   /*** Number of pixels between frame indicators ***/
    float (*funcKey)(G3DKEY *key, void *data );
    void   *funcData;
    uint32_t tool;
} G3DUITIMELINE;

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
typedef struct _G3DUIRENDERWINDOW {
    G3DUI              *gui;
    Q3DFILTER          *tostatus;
    G3DUIRENDERPROCESS *rps;
    G3DUIRENDERBUFFER   rbuf;
} G3DUIRENDERWINDOW;

/******************************************************************************/
typedef struct _G3DUILIGHTEDIT {
    G3DUI *gui;
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
    G3DUI *gui;
} G3DUIFFDEDIT;

/******************************************************************************/
typedef struct _G3DUICOORDINATESEDIT {
    G3DUI *gui;
} G3DUICOORDINATESEDIT;

/******************************************************************************/
typedef struct _G3DUICAMERAEDIT {
    G3DUI *gui;
} G3DUICAMERAEDIT;

/******************************************************************************/
typedef struct _G3DUIWIREFRAMEEDIT {
    G3DUI *gui;
} G3DUIWIREFRAMEEDIT;

/******************************************************************************/
typedef struct _G3DUITEXTEDIT {
    G3DUI *gui;
} G3DUITEXTEDIT;

/******************************************************************************/
typedef struct _G3DUISUBDIVIDEREDIT {
    G3DUI *gui;
} G3DUISUBDIVIDEREDIT;

/******************************************************************************/
typedef struct _G3DUITEXTUREEDIT {
    G3DUI *gui;
} G3DUITEXTUREEDIT;

/******************************************************************************/
typedef struct _G3DUICUBEEDIT {
    G3DUI *gui;
} G3DUICUBEEDIT;

/******************************************************************************/
typedef struct _G3DUITUBEEDIT {
    G3DUI *gui;
} G3DUITUBEEDIT;

/******************************************************************************/
typedef struct _G3DUITORUSEDIT {
    G3DUI *gui;
} G3DUITORUSEDIT;

/******************************************************************************/
typedef struct _G3DUITRACKERTAGEDIT {
    G3DUI *gui;
} G3DUITRACKERTAGEDIT;

/******************************************************************************/
typedef struct _G3DUISPHEREEDIT {
    G3DUI *gui;
} G3DUISPHEREEDIT;

/******************************************************************************/
typedef struct _G3DUIMESHEEDIT {
    G3DUI *gui;
} G3DUIMESHEDIT;

/******************************************************************************/
typedef struct _G3DUISYMMETRYEDIT {
    G3DUI *gui;
} G3DUISYMMETRYEDIT;

/******************************************************************************/
typedef struct _G3DUISPLINEREVOLVEREDIT {
    G3DUI *gui;
} G3DUISPLINEREVOLVEREDIT;

/******************************************************************************/
typedef struct _G3DUICYLINDEREDIT {
    G3DUI *gui;
} G3DUICYLINDEREDIT;

/******************************************************************************/
typedef struct _G3DUIINSTANCEEDIT {
    G3DUI *gui;
} G3DUIINSTANCEEDIT;

/******************************************************************************/
typedef struct _G3DUIKEYEDIT {
    G3DUI *gui;
} G3DUIKEYEDIT;

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
typedef struct _G3DUIOBJECTEDIT {
    G3DUI *gui;
} G3DUIOBJECTEDIT;

/******************************************************************************/
typedef struct _G3DUIPLANEEDIT {
    G3DUI *gui;
} G3DUIPLANEEDIT;

/******************************************************************************/
typedef struct _G3DUIRENDEREDIT {
    G3DUI *gui;
} G3DUIRENDEREDIT;

/******************************************************************************/
typedef struct _G3DUIPARTICLEEMITTEREDIT {
    G3DUI *gui;
} G3DUIPARTICLEEMITTEREDIT;

/******************************************************************************/
typedef struct _G3DUIMAIN {
    G3DUI          *gui;
    G3DUIMENU      *menuBar;
    G3DUIMODEBAR   *modeBar;
    G3DUITOOLBAR   *toolBar;
    G3DUIQUAD      *quad;

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
    uint32_t wait;
    uint32_t type;
    pthread_mutex_t done;
    G3DUI *gui;
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

#define MAXIMIZEBUTTON  0x00
#define ROTATEBUTTON    0x01
#define TRANSLATEBUTTON 0x02
#define ZOOMBUTTON      0x03

typedef struct _G3DUIVIEW {
    G3DUI         *gui;
    G3DUIRECTANGLE rec[NBVIEWBUTTON];       /*** pixmaps position ***/
    G3DUIRECTANGLE glrec;
    G3DUIRECTANGLE optrec;
    G3DUIRECTANGLE shdrec;
    G3DUIRECTANGLE btnrec;
    G3DUIRECTANGLE menurec;
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
    /*Widget ogl;*/ /*** OpenGL Widget ***/
    uint32_t       mode;   /*** wireframe, flat, fill ***/
    uint64_t       engine_flags;
    void         (*grid)( uint32_t );
#ifdef __linux__
    Display       *dpy;
    Window         win;
    GLXContext     glctx;
    pthread_t      render_tid; /*** current rendering thread - Used for canceling***/
#endif
#ifdef __MINGW32__
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
typedef struct _OBJECTLISTPRIVATEDATA {
    G3DUI *gui;
    uint32_t oldWidth, oldHeight;
} OBJECTLISTPRIVATEDATA;


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
    uint32_t image_width;
    uint32_t image_height;
    uint32_t preview_width;
    uint32_t preview_height;
    uint32_t preview_border;
    uint32_t preview_name_height;
} MATERIALLISTDATA;

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

uint64_t            g3dui_undo ( G3DUI *gui );
uint64_t            g3dui_redo ( G3DUI *gui );
void                g3dui_init ( G3DUI     *gui,
                                 G3DUIMAIN *main );
uint64_t            g3dui_openG3DFile ( G3DUI      *gui, 
                                        const char *filename );
void                g3dui_createDefaultCameras ( G3DUI *gui );
void                g3dui_resetDefaultCameras ( G3DUI *gui );
void                g3dui_addMouseTool ( G3DUI        *gui, 
                                         G3DMOUSETOOL *mou );
G3DMOUSETOOL       *g3dui_getMouseTool ( G3DUI      *gui, 
                                         const char *name );
uint32_t            g3dui_setMouseTool ( G3DUI        *gui, 
                                         G3DCAMERA    *cam, 
                                         G3DMOUSETOOL *mou );
void                g3dui_setFileName ( G3DUI      *gui, 
                                        const char *filename );
void                g3dui_saveG3DFile ( G3DUI *gui );
G3DSCENE           *g3dui_importfileokcbk ( G3DUI      *gui, 
                                            const char *filedesc, 
                                            const char *filename );
uint32_t            g3dui_exportfileokcbk ( G3DUI      *gui,
                                            const char *filedesc, 
                                            const char *filename );
void                g3dui_exitokcbk ( G3DUI *gui );
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
uint32_t            g3dui_selectAllCbk ( G3DUI *gui );
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

uint64_t            g3dui_addVibratorTagCbk ( G3DUI *gui );
uint64_t            g3dui_addTrackerTagCbk ( G3DUI *gui );
uint64_t            g3dui_removeSelectedTagCbk ( G3DUI *gui );
uint64_t            g3duimenubar_addUVMapCbk ( G3DUI *gui );
uint64_t            g3duimenubar_fitUVMapCbk ( G3DUI *gui );
uint64_t            g3duimenubar_alignUVMapCbk ( G3DUI      *gui,
                                                 const char *option );
uint64_t            g3dui_mergeMeshCbk ( G3DUI *gui );
uint64_t            g3dui_splitMeshCbk ( G3DUI      *gui,
                                         const char *option );
uint64_t            g3dui_mirrorHeightmapCbk ( G3DUI      *gui,
                                               const char *option );
uint64_t            g3dui_mirrorWeightGroupCbk ( G3DUI      *gui,
                                                 const char *option );
uint64_t            g3dui_resetBoneTreeCbk ( G3DUI *gui );
uint64_t            g3dui_resetBoneCbk ( G3DUI *gui );
uint64_t            g3dui_fixBoneTreeCbk ( G3DUI *gui );
uint64_t            g3dui_fixBoneCbk ( G3DUI *gui );
uint64_t            g3dui_addBoneCbk ( G3DUI *gui );
uint64_t            g3dui_addSkinCbk ( G3DUI *gui );
uint64_t            g3dui_addMorpherCbk ( G3DUI *gui );
uint64_t            g3dui_addFFDBoxCbk ( G3DUI *gui );
uint64_t            g3dui_addSymmetryCbk ( G3DUI *gui );
uint64_t            g3dui_addInstanceCbk ( G3DUI *gui );
uint64_t            g3dui_addEmitterCbk ( G3DUI *gui );
uint64_t            g3dui_addSubdividerCbk ( G3DUI *gui );
uint64_t            g3dui_addWireframeCbk ( G3DUI *gui );
uint64_t            g3dui_addEmptyMeshCbk ( G3DUI *gui );
uint64_t            g3dui_addSplineCbk ( G3DUI *gui );
uint64_t            g3dui_addSplineRevolverCbk ( G3DUI *gui );
uint64_t            g3dui_addTextCbk ( G3DUI *gui );
uint64_t            g3dui_addCameraCbk ( G3DUI     *gui );
uint64_t            g3dui_addLightCbk ( G3DUI *gui );
uint64_t            g3dui_addCylinderCbk ( G3DUI *gui );
uint64_t            g3dui_addTubeCbk ( G3DUI *gui );
uint64_t            g3dui_addTorusCbk ( G3DUI *gui );
uint64_t            g3dui_addSphereCbk ( G3DUI *gui );
uint64_t            g3dui_addCubeCbk ( G3DUI *gui );
uint64_t            g3dui_addPlaneCbk ( G3DUI *gui );
uint64_t            g3dui_addNullCbk ( G3DUI *gui );
uint64_t            g3dui_selectTreeCbk ( G3DUI      *gui,
                                          const char *option );
uint64_t            g3dui_triangulateCbk ( G3DUI      *gui,
                                           const char *option );
uint64_t            g3dui_invertNormalCbk ( G3DUI *gui );
uint64_t            g3dui_alignNormalsCbk ( G3DUI *gui );
uint64_t            g3dui_untriangulateCbk ( G3DUI *gui );
uint64_t            g3dui_weldVerticesCbk ( G3DUI *gui );
uint64_t            g3dui_deleteLoneVerticesCbk ( G3DUI *gui );
uint64_t            g3dui_invertSelectionCbk ( G3DUI *gui );
uint64_t            g3dui_getObjectStatsCbk ( G3DUI   *gui, 
                                              char    *buffer, 
                                              uint32_t bufferlen );
uint64_t            g3dui_makeEditableCbk ( G3DUI *gui, 
                                            void  *data );
uint64_t            g3dui_addUVMapCbk ( G3DUI *gui );
uint64_t            g3dui_fitUVMapCbk ( G3DUI *gui );
uint64_t            g3dui_alignUVMapCbk ( G3DUI      *gui, 
                                          const char *option );
uint64_t g3dui_setMaterialCbk ( G3DUI *gui );

/******************************* g3duiboneedit.c ******************************/


/***************************** g3duibonelinker.c ******************************/

/***************************** g3duicameraedit.c ******************************/

uint64_t g3duicameraedit_dofRadiusCbk ( G3DUICAMERAEDIT *camedit,
                                        uint32_t         radius );
uint64_t g3duicameraedit_dofNearBlurCbk ( G3DUICAMERAEDIT *camedit,
                                          float            nearBlur );
uint64_t g3duicameraedit_dofNoBlurCbk ( G3DUICAMERAEDIT *camedit,
                                        float            noBlur );
uint64_t g3duicameraedit_dofFarBlurCbk ( G3DUICAMERAEDIT *camedit,
                                         float            farBlur );
uint64_t g3duicameraedit_dofEnableCbk ( G3DUICAMERAEDIT *camedit );

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

uint64_t g3duicoordinatesedit_posCbk ( G3DUICOORDINATESEDIT *coordedit, 
                                       G3DUIAXIS             axis, 
                                       uint32_t              absolute,
                                       float                 val );
uint64_t g3duicoordinatesedit_rotCbk ( G3DUICOORDINATESEDIT *coordedit, 
                                       G3DUIAXIS             axis, 
                                       float                 val );
uint64_t g3duicoordinatesedit_scaCbk ( G3DUICOORDINATESEDIT *coordedit, 
                                       G3DUIAXIS             axis, 
                                       float                 val );

/****************************** g3duicubeedit.c *******************************/

uint64_t g3duicubeedit_sliceCbk ( G3DUICUBEEDIT *cubedit,
                                  G3DUIAXIS      axis,
                                  int            slice );
uint64_t g3duicubeedit_radiusCbk ( G3DUICUBEEDIT *cubedit,
                                   float          radius );

/****************************** g3duicurrentedit.c ****************************/


/****************************** g3duicylinderedit.c ***************************/

uint64_t g3duicylinderedit_lengthCbk ( G3DUICYLINDEREDIT *cyledit, 
                                       float              length );
uint64_t g3duicylinderedit_radiusCbk ( G3DUICYLINDEREDIT *cyledit, 
                                       float              radius );
uint64_t g3duicylinderedit_capCbk ( G3DUICYLINDEREDIT *cyledit, 
                                    G3DUIAXIS          axis,
                                    int                cap );
uint64_t g3duicylinderedit_sliceCbk ( G3DUICYLINDEREDIT *cyledit, 
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

uint64_t g3duiffdedit_radiusCbk ( G3DUIFFDEDIT *ffdedit,
                                  G3DUIAXIS     axis,
                                  float         radius );
uint64_t g3duiffdedit_sliceCbk ( G3DUIFFDEDIT *ffdedit,
                                 G3DUIAXIS     axis,
                                 int           slice );

/***************************** g3duiinstanceedit.c ****************************/

uint64_t g3duiinstanceedit_setReferenceCbk ( G3DUIINSTANCEEDIT *insedit, 
                                             uint32_t           rank );
uint64_t g3duiinstanceedit_mirroredToggleCbk ( G3DUIINSTANCEEDIT *insedit );
uint64_t g3duiinstanceedit_orientationCbk ( G3DUIINSTANCEEDIT *insedit,
                                            char              *str );

/******************************** g3duikeyedit.c ******************************/

uint64_t g3duikeyedit_setKeyTransformationsCbk ( G3DUIKEYEDIT *keyedit,
                                                 uint32_t      flag );
uint64_t g3duikeyedit_unsetKeyTransformationsCbk ( G3DUIKEYEDIT *keyedit,
                                                   uint32_t      flag );
uint64_t g3duikeyedit_loopFrameCbk ( G3DUIKEYEDIT *keyedit,
                                     float         frame );
uint64_t g3duikeyedit_loopCbk ( G3DUIKEYEDIT *keyedit,
                                uint32_t      loop );
uint64_t g3duikeyedit_keyCbk ( G3DUIKEYEDIT *keyedit,
                               const char   *field,
                               float         val );

/******************************* g3duilightedit.c *****************************/

uint64_t g3duilightedit_setSpecularityCbk ( G3DUILIGHTEDIT *ligedit,
                                            uint32_t        red,
                                            uint32_t        green,
                                            uint32_t        blue );
uint64_t g3duilightedit_unsetSpotCbk ( G3DUILIGHTEDIT *ligedit );
uint64_t g3duilightedit_setSpotCbk ( G3DUILIGHTEDIT *ligedit,
                                     float           spotLength,
                                     float           spotAngle,
                                     float           spotFadeAngle );
uint64_t g3duilightedit_castShadowsCbk ( G3DUILIGHTEDIT *ligedit );
uint64_t g3duilightedit_setSoftShadowsCbk ( G3DUILIGHTEDIT *ligedit );
uint64_t g3duilightedit_shadowRadiusCbk ( G3DUILIGHTEDIT *ligedit,
                                          float           shadowRadius );
uint64_t g3duilightedit_shadowSampleCbk ( G3DUILIGHTEDIT *ligedit,
                                          uint32_t        shadowSample );
uint64_t g3duilightedit_setDiffuseCbk ( G3DUILIGHTEDIT *ligedit,
                                        uint32_t        red,
                                        uint32_t        green,
                                        uint32_t        blue );

/******************************* g3duiobjectlist.c ****************************/



/********************************* g3duimain.c ********************************/

void g3duimain_sizeAllocate ( G3DUIMAIN *gmn, 
                              uint32_t   width, 
                              uint32_t   height );

/***************************** g3duimaterialboard.c ***************************/

/****************************** g3duimaterialedit.c ***************************/

uint64_t g3duimaterialedit_setDiffuseColorCbk ( G3DUIMATERIALEDIT *matedit, 
                                                float              R,
                                                float              G,
                                                float              B,
                                                float              A );
uint64_t g3duimaterialedit_setSpecularColorCbk ( G3DUIMATERIALEDIT *matedit, 
                                                 float              R,
                                                 float              G,
                                                 float              B,
                                                 float              A );
uint64_t g3duimaterialedit_setDisplacementStrengthCbk ( G3DUIMATERIALEDIT *matedit,
                                                        float              strength );
uint64_t g3duimaterialedit_setBumpStrengthCbk ( G3DUIMATERIALEDIT *matedit, 
                                                float              strength );
uint64_t g3duimaterialedit_setReflectionStrengthCbk ( G3DUIMATERIALEDIT *matedit,
                                                      float              strength );
uint64_t g3duimaterialedit_setRefractionStrengthCbk ( G3DUIMATERIALEDIT *matedit, 
                                                      float              strength );
uint64_t g3duimaterialedit_setAlphaStrengthCbk ( G3DUIMATERIALEDIT *matedit, 
                                                 float              strength );
uint64_t g3duimaterialedit_setNameCbk ( G3DUIMATERIALEDIT *matedit,
                                        const char        *name );
uint64_t g3duimaterialedit_setSpecularLevelCbk ( G3DUIMATERIALEDIT *matedit,
                                                 float              val );
uint64_t g3duimaterialedit_setSpecularShininessCbk ( G3DUIMATERIALEDIT *matedit,
                                                     float              val );
uint64_t g3duimaterialedit_channelChooseImageCbk ( G3DUIMATERIALEDIT *matedit,
                                                   uint32_t           channelID,
                                                   char              *filename,
                                                   uint32_t           bindGL );
uint64_t g3duimaterialedit_enableProceduralCbk ( G3DUIMATERIALEDIT *matedit,
                                                 uint32_t           channelID );
uint64_t g3duimaterialedit_enableSolidColorCbk ( G3DUIMATERIALEDIT *matedit,
                                                 uint32_t           channelID );
uint64_t g3duimaterialedit_enableImageCbk ( G3DUIMATERIALEDIT *matedit,
                                            uint32_t           channelID );
uint64_t g3duimaterialedit_chooseProceduralCbk ( G3DUIMATERIALEDIT *matedit,
                                                 uint32_t           channelID,
                                                 const char        *procType,
                                                 const char        *procRes,
                                                 uint32_t           bindGL );
uint64_t g3duimaterialedit_toggleDisplacementCbk ( G3DUIMATERIALEDIT *matedit );
uint64_t g3duimaterialedit_toggleBumpCbk ( G3DUIMATERIALEDIT *matedit );
uint64_t g3duimaterialedit_toggleAlphaCbk ( G3DUIMATERIALEDIT *matedit );

/****************************** g3duimateriallist.c ***************************/

/*************************** g3duimaterialnameedit.c **************************/

/********************************* g3duimenu.c ********************************/

/******************************** g3duimeshedit.c *****************************/

uint64_t g3duimeshedit_useIsoLinesCbk ( G3DUIMESHEDIT *mesedit );
uint64_t g3duimeshedit_gouraudCbk ( G3DUIMESHEDIT *mesedit,
                                    float          scalarLimit );
uint64_t g3duimeshedit_toggleShadingCbk ( G3DUIMESHEDIT *mesedit );

/****************************** g3duimeshposelist.c ***************************/

/********************************* g3duimodebar.c *****************************/

uint64_t g3duimodebar_setModeCbk ( G3DUIMODEBAR *gmb,
                                   const char   *modename );
uint64_t m3duimodebar_setModeCbk ( M3DUIMODEBAR *mmb, 
                                   const char   *modename );

/******************************** g3duiobjectedit.c ***************************/

uint64_t g3duiobjectedit_nameCbk ( G3DUIOBJECTEDIT *goe,
                                   const char      *name );

/*************************** g3duiparticleemitteredit.c ***********************/

uint64_t g3duiparticleemitteredit_gravityForceXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     g );
uint64_t g3duiparticleemitteredit_gravityForceYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     g );
uint64_t g3duiparticleemitteredit_gravityForceZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     g );
uint64_t g3duiparticleemitteredit_initialVarTranspCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                        float                     initialVarTransp );
uint64_t g3duiparticleemitteredit_initialVarRotXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                      float                     initialVarRotX );
uint64_t g3duiparticleemitteredit_initialVarRotYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                      float                     initialVarRotY );
uint64_t g3duiparticleemitteredit_initialVarRotZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                      float                     initialVarRotZ );
uint64_t g3duiparticleemitteredit_initialVarScaXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                      float                     initialVarScaX );
uint64_t g3duiparticleemitteredit_initialVarScaYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                      float                     initialVarScaY );
uint64_t g3duiparticleemitteredit_initialVarScaZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                      float                     initialVarScaZ );
uint64_t g3duiparticleemitteredit_initialVarSpeedXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                        float                     initialVarSpeedX );
uint64_t g3duiparticleemitteredit_initialVarSpeedYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                        float                     initialVarSpeedY );
uint64_t g3duiparticleemitteredit_initialVarSpeedZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                        float                     initialVarSpeedZ );
uint64_t g3duiparticleemitteredit_initialVarAccelXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                        float                     initialVarAccelX );
uint64_t g3duiparticleemitteredit_initialVarAccelYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                        float                     initialVarAccelY );
uint64_t g3duiparticleemitteredit_initialVarAccelZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                        float                     initialVarAccelZ );
uint64_t g3duiparticleemitteredit_finalTranspCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalTransp );
uint64_t g3duiparticleemitteredit_finalRotXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                 float                     finalRotX );
uint64_t g3duiparticleemitteredit_finalRotYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                 float                     finalRotY );
uint64_t g3duiparticleemitteredit_finalRotZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                 float                     finalRotZ );
uint64_t g3duiparticleemitteredit_finalScaXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                 float                     finalScaX );
uint64_t g3duiparticleemitteredit_finalScaYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                 float                     finalScaY );
uint64_t g3duiparticleemitteredit_finalScaZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                 float                     finalScaZ );
uint64_t g3duiparticleemitteredit_finalSpeedXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalSpeedX );
uint64_t g3duiparticleemitteredit_finalSpeedYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalSpeedY );
uint64_t g3duiparticleemitteredit_finalSpeedZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalSpeedZ );
uint64_t g3duiparticleemitteredit_finalAccelXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalAccelX );
uint64_t g3duiparticleemitteredit_finalAccelYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalAccelY );
uint64_t g3duiparticleemitteredit_finalAccelZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalAccelZ );
uint64_t g3duiparticleemitteredit_initialTranspCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialTransp );
uint64_t g3duiparticleemitteredit_initialRotXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialRotX );
uint64_t g3duiparticleemitteredit_initialRotYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialRotY );
uint64_t g3duiparticleemitteredit_initialRotZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialRotZ );
uint64_t g3duiparticleemitteredit_initialScaXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialScaX );
uint64_t g3duiparticleemitteredit_initialScaYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialScaY );
uint64_t g3duiparticleemitteredit_initialScaZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialScaZ );
uint64_t g3duiparticleemitteredit_initialSpeedXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialSpeedX );
uint64_t g3duiparticleemitteredit_initialSpeedYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialSpeedY );
uint64_t g3duiparticleemitteredit_initialSpeedZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialSpeedZ );
uint64_t g3duiparticleemitteredit_initialAccelXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialAccelX );
uint64_t g3duiparticleemitteredit_initialAccelYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialAccelY );
uint64_t g3duiparticleemitteredit_initialAccelZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialAccelZ );

/********************************* g3duiplaneedit.c ***************************/

uint64_t g3duiplaneedit_divCbk  ( G3DUIPLANEEDIT *plnedit,
                                  G3DUIAXIS       axis,
                                  int             div );
uint64_t g3duiplaneedit_radiusCbk ( G3DUIPLANEEDIT *plnedit,
                                    G3DUIAXIS       axis,
                                    float           radius );
uint64_t g3duiplaneedit_orientationcbk ( G3DUIPLANEEDIT *plnedit,
                                         const char     *oristr );

/*********************************** g3duiquad.c ******************************/

void g3duiquad_resize ( G3DUIQUAD *quad,
                        uint32_t   width, 
                        uint32_t   height );
void g3duiquad_init ( G3DUIQUAD *quad );

/******************************** g3duirenderedit.c ***************************/

uint64_t g3duirenderedit_aaCbk ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_aaFullCbk ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_aaEdgeCbk ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_aaSamplesCbk ( G3DUIRENDEREDIT *redit,
                                        uint32_t         nbsamples );
uint64_t g3duirenderedit_formatCbk ( G3DUIRENDEREDIT *redit,
                                     const char      *format );
uint64_t g3duirenderedit_startFrameCbk ( G3DUIRENDEREDIT *redit,
                                         float            frame );
uint64_t g3duirenderedit_endFrameCbk ( G3DUIRENDEREDIT *redit,
                                       float            frame );
uint64_t g3duirenderedit_setMotionBlurCbk ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_setTexturingCbk ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_texturingColorCbk ( G3DUIRENDEREDIT *redit,
                                             unsigned char    R,
                                             unsigned char    G,
                                             unsigned char    B );
uint64_t g3duirenderedit_setWireframeCbk ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_setWireframeLightingCbk ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_wireframeThicknessCbk ( G3DUIRENDEREDIT *redit,
                                                 float  thickness );
uint64_t g3duirenderedit_wireframeColorCbk ( G3DUIRENDEREDIT *redit, 
                                             unsigned char R, 
                                             unsigned char G, 
                                             unsigned char B );
uint64_t g3duirenderedit_setFogStrengthCbk ( G3DUIRENDEREDIT *redit, 
                                             float            strength );
uint64_t g3duirenderedit_setFogCbk ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_setFogAffectsBackgroundCbk ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_fogFarCbk ( G3DUIRENDEREDIT *redit,
                                     float            ffar );
uint64_t g3duirenderedit_fogNearCbk ( G3DUIRENDEREDIT *redit,
                                      float            fnear );
uint64_t g3duirenderedit_fogColorCbk ( G3DUIRENDEREDIT *redit, 
                                       unsigned char    R, 
                                       unsigned char    G, 
                                       unsigned char    B );
uint64_t g3duirenderedit_sceneMotionBlurCbk ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_vectorMotionBlurCbk ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_sceneMotionBlurIterationCbk ( G3DUIRENDEREDIT *redit, 
                                                       uint32_t nbstep );
uint64_t g3duirenderedit_motionBlurStrengthCbk ( G3DUIRENDEREDIT *redit,
                                                 float            strength );
uint64_t g3duirenderedit_vectorMotionBlurSamplesCbk ( G3DUIRENDEREDIT *redit, 
                                                      uint32_t         nbSamples );
uint64_t g3duirenderedit_vectorMotionBlurSubSamplingRateCbk ( G3DUIRENDEREDIT *redit, 
                                                              float            subSamplingRate );
uint64_t g3duirenderedit_fpsCbk ( G3DUIRENDEREDIT *redit,
                                  uint32_t         fps );
uint64_t g3duirenderedit_outputCbk ( G3DUIRENDEREDIT *redit, 
                                     const char      *outfile );
uint64_t g3duirenderedit_previewCbk ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_saveCbk ( G3DUIRENDEREDIT *redit,
                                   uint32_t         save );
uint64_t g3duirenderedit_widthCbk ( G3DUIRENDEREDIT *redit, 
                                    uint32_t         width );
uint64_t g3duirenderedit_heightCbk ( G3DUIRENDEREDIT *redit,
                                     uint32_t         height );
uint64_t g3duirenderedit_ratioCbk ( G3DUIRENDEREDIT *redit, 
                                    float            ratio );
uint64_t g3duirenderedit_backgroundCbk ( G3DUIRENDEREDIT *redit, 
                                         unsigned char    R, 
                                         unsigned char    G, 
                                         unsigned char    B );
uint64_t g3duirenderedit_setBackgroundColorModeCbk ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_setBackgroundImageModeCbk ( G3DUIRENDEREDIT *redit );
uint64_t g3duirenderedit_setBackgroundImageCbk ( G3DUIRENDEREDIT *redit,
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

/******************************* g3duisphereedit.c ****************************/

uint64_t g3duisphereedit_togglePerfectCbk ( G3DUISPHEREEDIT *sphedit );
uint64_t g3duisphereedit_radiusCbk ( G3DUISPHEREEDIT *sphedit,
                                     float            radius );
uint64_t g3duisphereedit_capCbk ( G3DUISPHEREEDIT *sphedit,
                                  uint32_t         cap );
uint64_t g3duisphereedit_sliceCbk ( G3DUISPHEREEDIT *sphedit,
                                    uint32_t         slice );

/************************** g3duisplinerevolveredit.c *************************/

uint64_t g3duisplinerevolveredit_splineRevolverStepsCbk ( G3DUISPLINEREVOLVEREDIT *srvedit,
                                                          uint32_t                 level );
uint64_t g3duisplinerevolveredit_splineRevolverDivisCbk ( G3DUISPLINEREVOLVEREDIT *srvedit,  
                                                          uint32_t                 level );

/**************************** g3duisubdivideredit.c ***************************/

uint64_t g3duisubdivideredit_displacementHeightCbk ( G3DUISUBDIVIDEREDIT *subedit );
uint64_t g3duisubdivideredit_displacementSculptCbk ( G3DUISUBDIVIDEREDIT *subedit );
uint64_t g3duisubdivideredit_subdivSyncCbk ( G3DUISUBDIVIDEREDIT *subedit );
uint64_t g3duisubdivideredit_useIsoLinesCbk ( G3DUISUBDIVIDEREDIT *subedit );
uint64_t g3duisubdivideredit_subdivRenderCbk ( G3DUISUBDIVIDEREDIT *subedit,
                                               uint32_t             level );
uint64_t g3duisubdivideredit_subdivPreviewCbk ( G3DUISUBDIVIDEREDIT *subedit,
                                                uint32_t             level );

/***************************** g3duisymmetryedit.c ****************************/

uint64_t g3duisymmetryedit_limitCbk ( G3DUISYMMETRYEDIT *symedit,
                                      float              limit );
uint64_t g3duisymmetryedit_planeCbk ( G3DUISYMMETRYEDIT *symedit,
                                      char              *orientation );

/******************************* g3duitextedit.c ******************************/

uint64_t g3duitextedit_setTextCbk ( G3DUITEXTEDIT *txtedit,
                                    char          *newText );
uint64_t g3duitextedit_thicknessCbk ( G3DUITEXTEDIT *txtedit,
                                      float          thickness );
uint64_t g3duitextedit_roundnessCbk ( G3DUITEXTEDIT *txtedit,
                                      uint32_t       roundness );
uint64_t g3duitextedit_sizeCbk ( G3DUITEXTEDIT *txtedit,
                                 uint32_t       size );

/****************************** g3duitextureedit.c ****************************/

uint64_t g3duitextureedit_toggleRestrictCbk ( G3DUITEXTUREEDIT *texedit );
uint64_t g3duitextureedit_toggleRepeatCbk ( G3DUITEXTUREEDIT *texedit );
uint64_t g3duitextureedit_setUVMapCbk ( G3DUITEXTUREEDIT *texedit,
                                        uint32_t         rank );

/******************************* g3duitimeboard.c *****************************/

uint64_t g3duitimeboard_stopCbk ( G3DUITIMEBOARD *tbrd );
uint64_t g3duitimeboard_gotoFrameCbk ( G3DUITIMEBOARD *tbrd );
uint64_t g3duitimeboard_recordFrameCbk ( G3DUITIMEBOARD *tbrd,
                                         uint32_t        key_flags );

/******************************* g3duitimeline.c ******************************/

uint64_t g3duitimeline_scaleSelectedKeysCbk ( G3DUITIMELINE *tim, 
                                              float  factor, 
                                              float  reference );
uint64_t g3duitimeline_deleteSelectedKeysCbk ( G3DUITIMELINE *tim );
uint64_t g3duitimeline_selectAllKeysCbk ( G3DUITIMELINE *tim );
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
void g3duitimeline_init ( G3DUITIMELINE *tim );

/******************************* g3duitorusedit.c *****************************/

uint64_t g3duitorusedit_sliceCbk ( G3DUITORUSEDIT *toredit,
                                   uint32_t        slice );
uint64_t g3duitorusedit_capCbk ( G3DUITORUSEDIT *toredit,
                                 uint32_t        cap );
uint64_t g3duitorusedit_extRadiusCbk ( G3DUITORUSEDIT *toredit,
                                       float           extrad );
uint64_t g3duitorusedit_intRadiusCbk ( G3DUITORUSEDIT *toredit,
                                       float           intrad );
uint64_t g3duitorusedit_orientationCbk ( G3DUITORUSEDIT *toredit,
                                         const char     *oristr );

/***************************** g3duitrackertagedit.c **************************/

uint64_t g3duitrackertagedit_orientationCbk ( G3DUITRACKERTAGEDIT *ttedit, 
                                              char                *str );
uint64_t g3duitrackertagedit_setTargetCbk ( G3DUITRACKERTAGEDIT *ttedit, 
                                            uint32_t             rank );

/******************************** g3duitubeedit.c *****************************/

uint64_t g3duitubeedit_lengthCbk ( G3DUITUBEEDIT *tubedit,
                                   float          length );
uint64_t g3duitubeedit_radiusCbk ( G3DUITUBEEDIT *tubedit,
                                   float          radius );
uint64_t g3duitubeedit_thicknessCbk ( G3DUITUBEEDIT *tubedit,
                                      float          thickness );
uint64_t g3duitubeedit_capCbk ( G3DUITUBEEDIT *tubedit,
                                G3DUIAXIS      axis,
                                uint32_t       cap );
uint64_t g3duitubeedit_sliceCbk ( G3DUITUBEEDIT *tubedit,
                                  uint32_t       slice );

/******************************** g3duiuvmapedit.c ****************************/

uint64_t g3duiuvmapedit_projectionCbk ( G3DUIUVMAPEDIT *uvedit,
                                        const char     *projection );
uint64_t g3duiuvmapedit_lockUVMapCbk ( G3DUIUVMAPEDIT *uvedit );

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

int g3duiview_getCurrentButton ( G3DUIVIEW *view, 
                                 int        x,
                                 int        y );
void g3duiview_init ( G3DUIVIEW *view,
                      uint32_t   width,
                      uint32_t   height );
void g3duiview_resize ( G3DUIVIEW *view, 
                        uint32_t   width, 
                        uint32_t   height );
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

uint64_t g3duiview_setShadingCbk ( G3DUIVIEW *view,
                                   char      *shading );

/*************************** g3duiweightgrouplist.c ***************************/

G3DWEIGHTGROUP *g3duiweightgrouplist_getWeightGroup ( G3DUIWEIGHTGROUPLIST *wgrplist,
                                                      G3DMESH              *mes, 
                                                      float                mouse_x,
                                                      float                mouse_y );
uint64_t g3duiweightgrouplist_deleteWeightGroupCbk ( G3DUIWEIGHTGROUPLIST *wgrplist );
uint64_t g3duiweightgrouplist_addWeightGroupCbk ( G3DUIWEIGHTGROUPLIST *wgrplist );
uint64_t g3duiweightgrouplist_nameCbk ( G3DUIWEIGHTGROUPLIST *wgrplist,
                                        const char           *grpname );
uint64_t g3duiweightgrouplist_deleteSelectedCbk ( G3DUIWEIGHTGROUPLIST *wgrplist );
uint64_t g3duiweightgrouplist_selectCbk ( G3DUIWEIGHTGROUPLIST *wgrplist,
                                          G3DWEIGHTGROUP       *grp );

/******************************* g3duiwireframeedit.c ****************************/

uint64_t g3duiwireframeedit_thicknessCbk ( G3DUIWIREFRAMEEDIT *wfmedit,
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
uint64_t m3dui_redoCbk ( M3DUI *mui );
uint64_t m3dui_undoCbk ( M3DUI *mui );
uint64_t m3dui_fillWithColorCbk ( M3DUI   *mui, 
                               uint32_t color );
uint64_t m3dui_fac2uvsetCbk ( M3DUI *mui );
uint64_t m3dui_uv2verCbk ( M3DUI *mui );
uint64_t m3dui_ver2uvCbk ( M3DUI *mui );
uint64_t m3dui_uvset2facCbk ( M3DUI *mui );
G3DCHANNEL *m3dui_getWorkingChannel ( M3DUI *mui );
void m3dui_resizeBuffers ( M3DUI *mui );
void m3dui_setCanevas ( M3DUI *mui );
void m3duiview_showGL ( M3DUI        *mui,
                        M3DMOUSETOOL *mou,
                        uint64_t      engine_flags );

#endif
