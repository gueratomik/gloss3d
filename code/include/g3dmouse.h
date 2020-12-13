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

#ifndef _G3DMOUSETOOL_H_
#define _G3DMOUSETOOL_H_

/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/******************************************************************************/
#include <string.h>
#include <math.h>

/******************************************************************************/
#include <list.h>
#include <g3dengine/g3dengine.h>
#include <lips3d/lips3d.h>
#include <g3durmanager.h>

/******************************************************************************/
#define PICKTOOL                     "Pick"
#define SCULPTTOOL                   "Sculpt"
#define SMOOTHTOOL                   "Smooth"
#define MOVETOOL                     "Move"
#define ROTATETOOL                   "Rotate"
#define SCALETOOL                    "Scale"
#define EXTRUDEFACETOOL              "Extrude face"
#define EXTRUDEINNERTOOL             "Extrude Inner"
#define CREATESPHERETOOL             "Create Sphere"
#define CREATECUBETOOL               "Create Cube"
#define CREATETUBETOOL               "Create Tube"
#define CREATECYLINDERTOOL           "Create Cylinder"
#define CREATETORUSTOOL              "Create Torus"
#define CREATEBONETOOL               "Create Bone"
#define CREATEPLANETOOL              "Create Plane"
#define CUTMESHTOOL                  "Cutter"
#define CREATEVERTEXTOOL             "Create Vertex (ctrl+click)"
#define REVERTSPLINETOOL             "Revert Spline"
#define REMOVEVERTEXPOSETOOL         "Remove from pose"
#define BRIDGETOOL                   "Bridge"
#define PAINTWEIGHTTOOL              "Paint Weight"
#define UNTRIANGULATETOOL            "Untriangulate"
#define TRIANGULATETOOL              "Triangulate"
#define CREATEFACEGROUPTOOL          "Create Facegroup"
#define WELDVERTICESTOOL             "Weld vertices"
#define WELDNEIGHBOURSTOOL           "Weld neighbour vertices"
#define INVERTNORMALTOOL             "Invert Normal"
#define ROUNDSPLINEPOINTTOOL         "Round selection"

#define PICKUVTOOL                   "Pick UV"
#define MOVEUVTOOL                   "Move UV"
#define ROTATEUVTOOL                 "Rotate UV"
#define SCALEUVTOOL                  "Scale UV"

/******************************************************************************/
#define SELECTTOOL                   "Select"
#define PENTOOL                      "Pen"
#define BUCKETTOOL                   "Bucket"
#define ERASERTOOL                   "Eraser"
#define COLORPICKERTOOL              "Color Picker"

/*** dont set the tool as the current one ***/
#define MOUSETOOLNOCURRENT ( 1  << 0  )
#define MOUSETOOLREADONLY  ( 1  << 1  )
#define MOUSETOOLREADWRITE ( 1  << 2  )

#define G3DKeyPress      0x01
#define G3DKeyRelease    0x02
#define G3DButtonPress   0x03
#define G3DButtonRelease 0x04
#define G3DMotionNotify  0x05

#define G3DControlMask   ( 1  << 0  )
#define G3DShiftMask     ( 1  << 1  )
#define G3DButton1Mask   ( 1  << 2  )
#define G3DButton2Mask   ( 1  << 3  )
#define G3DButton3Mask   ( 1  << 4  )

#define REDRAWVIEW            ( 1       )
#define REDRAWUVMAPEDITOR     ( 1 <<  1 )
#define REDRAWLIST            ( 1 <<  2 )
#define REDRAWCOORDS          ( 1 <<  3 )
#define REDRAWPARAMS          ( 1 <<  4 )
#define REDRAWCURRENTOBJECT   ( 1 <<  5 )
#define REDRAWTIMELINE        ( 1 <<  6 )
#define REDRAWMATERIALLIST    ( 1 <<  7 )
#define REBUILDMATERIALLIST   ( 1 <<  8 )
#define NOBUFFEREDSUBDIVISION ( 1 <<  9 )
#define BUFFEREDSUBDIVISIONOK ( 1 << 10 )
#define REDRAWALL             ( REDRAWVIEW | REDRAWLIST | REDRAWCOORDS | \
                                REDRAWUVMAPEDITOR |\
                                REDRAWPARAMS | REDRAWCURRENTOBJECT | \
                                REDRAWTIMELINE )

#define PICKMINRADIUS 0x10

/******************************************************************************/
typedef struct {
	int type;             /* G3DKeyPress or G3DKeyRelease */
    /*** we use floating point because GDK does. I don't know why. ***/
    /*** If we use int, then it's hard to know the direction change of the ***/
    /*** mouse, which is needed for LIPS3D ***/
	double x, y;             /* pointer x, y coordinates in event window */
	unsigned int state;   /* key or button mask */
	unsigned int keycode; /* detail */
} G3DKeyEvent;

/******************************************************************************/
typedef struct {
	int type;            /* G3DButtonPress or G3DButtonRelease */
    /*** we use floating point because GDK does. I don't know why. ***/
    /*** If we use int, then it's hard to know the direction change of the ***/
    /*** mouse, which is needed for LIPS3D ***/
	double x, y;            /* pointer x, y coordinates in event window */
	unsigned int state;  /* key or button mask */
	unsigned int button; /* detail */
} G3DButtonEvent;

/******************************************************************************/
typedef struct {
	int type;            /* G3DMotionNotify */
    /*** we use floating point because GDK does. I don't know why. ***/
    /*** If we use int, then it's hard to know the direction change of the ***/
    /*** mouse, which is needed for LIPS3D ***/
	double x, y;            /* pointer x, y coordinates in event window */
	unsigned int state;  /* key or button mask */
} G3DMotionEvent;

/******************************************************************************/
typedef union _G3DEvent {
	int type;      /* must not be changed */
	G3DKeyEvent    key;
	G3DButtonEvent button;
	G3DMotionEvent motion;
} G3DEvent;

/******************************************************************************/
typedef struct _G3DMOUSETOOL {
    char *name;
    char key;
    /*** bitmap must be 16x16 .XPM ***/
    const char **icon;
    /*** init function is called when tool is selected ***/
    uint32_t (*init) ( struct _G3DMOUSETOOL *,
                               G3DSCENE *,
                               G3DCAMERA *,
                               G3DURMANAGER *,
                               uint64_t );
    void (*draw) ( struct _G3DMOUSETOOL *, G3DSCENE *, uint32_t ); /* drawing */
    int  (*tool) ( struct _G3DMOUSETOOL *,
                           G3DSCENE *,
                           G3DCAMERA *,
                           G3DURMANAGER *,
                           uint64_t,
                           G3DEvent * ); /** mouse event callback **/
    char        *mask;     /*** used by LIPS3D API ***/
    char        *zbuffer;  /*** used by LIPS3D API ***/
    uint32_t     flags;
} G3DMOUSETOOL,
  G3DMOUSETOOLCREATESPHERE,
  G3DMOUSETOOLCREATECUBE,
  G3DMOUSETOOLCREATEPLANE,
  G3DMOUSETOOLCREATECYLINDER,
  G3DMOUSETOOLCREATETORUS,
  G3DMOUSETOOLCREATETUBE,
  G3DMOUSETOOLCREATEBONE,
  G3DMOUSETOOLCREATEVERTEX,
  G3DMOUSETOOLCREATEFACEGROUP,
  G3DMOUSETOOLREVERTSPLINE,
  G3DMOUSETOOLTRIANGULATE,
  G3DMOUSETOOLUNTRIANGULATE,
  G3DMOUSETOOLROUNDSPLINEPOINT,
  G3DMOUSETOOLWELDVERTICES,
  G3DMOUSETOOLWELDNEIGHBOURS,
  G3DMOUSETOOLINVERTNORMAL,
  G3DMOUSETOOLMOVE,
  G3DMOUSETOOLMOVEUV,
  G3DMOUSETOOLSCALE,
  G3DMOUSETOOLSCALEUV,
  G3DMOUSETOOLROTATE,
  G3DMOUSETOOLROTATEUV,
  G3DMOUSETOOLREMOVEVERTEXPOSE;

/******************************************************************************/
typedef struct _G3DMOUSETOOLBRIDGE {
    G3DMOUSETOOL  tool;
    uint32_t      draw; /*** true or false ***/
    G3DVERTEX    *ver[0x04];
    G3DCURVEPOINT *pt[0x02];
    G3DOBJECT     *obj; /*** the mesh that gest the new face ****/
} G3DMOUSETOOLBRIDGE;

/******************************************************************************/
typedef struct _G3DMOUSETOOLEXTRUDEFACE {
    G3DMOUSETOOL  tool;
    uint32_t inner; /*** true or false ***/
} G3DMOUSETOOLEXTRUDEFACE;

/******************************************************************************/
#define CTRLCLICK ( 1 << 0 )

typedef struct _G3DMOUSETOOLPICK {
    G3DMOUSETOOL  tool;
    int start; 
    int32_t coord[0x04];   /*** x1, y1, x2, y2                     ***/
    uint32_t only_visible; /*** true or false                      ***/
    float weight;          /*** Paint weight                       ***/
    uint32_t operation;    /*** Add (0x01) or remove (0x00) weight ***/
    uint32_t radius;
} G3DMOUSETOOLPICK, G3DMOUSETOOLPICKUV;

/******************************************************************************/
typedef struct _SCULPTFACE {
    G3DFACE *fac;
    G3DRTVERTEX *rtvermem;
} SCULPTFACE;

/******************************************************************************/
typedef struct _G3DMOUSETOOLSCULPT {
    G3DMOUSETOOL  tool;
    int32_t coord[0x04];   /*** x1, y1, x2, y2                     ***/
    int only_visible;
    int radius;
    float pressure;
    int ctrl_key;
    LIST *lscf; /*** SCULPTFACE list ***/
} G3DMOUSETOOLSCULPT;

/******************************************************************************/
typedef struct _G3DMOUSETOOLSMOOTH {
    G3DMOUSETOOL  tool;
    int32_t coord[0x04];   /*** x1, y1, x2, y2                     ***/
    int only_visible;
    int radius;
    float pressure;
    int ctrl_key;
} G3DMOUSETOOLSMOOTH;

/******************************************************************************/
typedef struct _G3DMOUSETOOLCUTMESH {
    G3DMOUSETOOL  tool;
    int start;
    GLdouble coord[0x04][0x03];
    uint32_t restrict_to_selection; /*** true or false ***/
} G3DMOUSETOOLCUTMESH;

/******************************************************************************/
typedef struct _G3DMESHFAC {
    G3DMESH *mes;
    LIST *lfac;
} G3DMESHFAC;

/******************************************************************************/
typedef struct _L3DMOUSETOOL {
    G3DMOUSETOOL gtool;
    L3DOBJECT   *obj;
} L3DMOUSETOOL, L3DMOUSETOOLSELECTEAREA, L3DMOUSETOOLSELECTOR;

/******************************************************************************/
typedef struct _L3DMOUSETOOLPEN {
    L3DMOUSETOOL ltool;
    uint32_t     incremental;
} L3DMOUSETOOLPEN, L3DMOUSETOOLERASER;

/******************************************************************************/
typedef struct _L3DMOUSETOOLBUCKET {
    L3DMOUSETOOL ltool;
} L3DMOUSETOOLBUCKET;

/******************************************************************************/
void g3dmousetool_init ( G3DMOUSETOOL *gtool,
                         char *name, char key, const char **icon,
                         uint32_t (*init) ( G3DMOUSETOOL *,
                                            G3DSCENE *, 
                                            G3DCAMERA *,
                                            G3DURMANAGER *, 
                                            uint64_t ),
                         void (*draw) ( G3DMOUSETOOL *,
                                        G3DSCENE *, 
                                        uint64_t ),
                         int  (*tool) ( G3DMOUSETOOL *, 
                                        G3DSCENE *,
                                        G3DCAMERA *, 
                                        G3DURMANAGER *,
                                        uint64_t,
                                        G3DEvent * ),
                         uint32_t flags );

void l3dmousetool_init ( L3DMOUSETOOL *ltool,
                         char *name, char key, const char **icon,
                         uint32_t (*init) ( G3DMOUSETOOL *,
                                            G3DSCENE *, 
                                            G3DCAMERA *,
                                            G3DURMANAGER *, 
                                            uint64_t ),
                         void (*draw) ( G3DMOUSETOOL *,
                                        G3DSCENE *, 
                                        uint64_t ),
                         int  (*tool) ( G3DMOUSETOOL *, 
                                        G3DSCENE *,
                                        G3DCAMERA *, 
                                        G3DURMANAGER *,
                                        uint64_t,
                                        G3DEvent * ),
                         uint32_t flags );

/******************************************************************************/
int pick_tool ( G3DMOUSETOOL *mou, 
                G3DSCENE     *sce, 
                G3DCAMERA    *cam,
                G3DURMANAGER *urm, 
                uint64_t      flags, 
                G3DEvent     *event );
int pickUV_tool ( G3DMOUSETOOL *mou, 
                  G3DSCENE     *sce, 
                  G3DCAMERA    *cam,
                  G3DURMANAGER *urm, 
                  uint64_t      flags, 
                  G3DEvent     *event );
void pick_Item ( G3DMOUSETOOLPICK *pt, 
                 G3DSCENE         *sce, 
                 G3DCAMERA        *cam,
                 uint32_t          ctrlClick,
                 uint64_t engine_flags );
void pick_cursor ( G3DMOUSETOOLPICK *pt, 
                   G3DSCENE         *sce, 
                   G3DCAMERA        *cam,
                   uint64_t engine_flags );

/******************************************************************************/
G3DMOUSETOOLCREATECYLINDER *g3dmousetoolcreatecylinder_new ( );
G3DMOUSETOOLPICK *g3dmousetoolpick_new ( );
G3DMOUSETOOLCREATESPHERE *g3dmousetoolcreatesphere_new ( );
G3DMOUSETOOLCREATECUBE *g3dmousetoolcreatecube_new ( );
G3DMOUSETOOLCREATEPLANE *g3dmousetoolcreateplane_new ( );
G3DMOUSETOOLCREATECYLINDER *g3dmousetoolcreatecylinder_new ( );
G3DMOUSETOOLCREATETUBE *g3dmousetoolcreatetube_new ( );
G3DMOUSETOOLCREATETORUS *g3dmousetoolcreatetorus_new ( );
G3DMOUSETOOLCREATEBONE *g3dmousetoolcreatebone_new ( );
G3DMOUSETOOLCUTMESH *g3dmousetoolcutmesh_new ( );
G3DMOUSETOOLCREATEVERTEX *g3dmousetoolcreatevertex_new ( );
G3DMOUSETOOLREVERTSPLINE *g3dmousetoolrevertspline_new ( );
G3DMOUSETOOLBRIDGE *g3dmousetoolbridge_new ( );
G3DMOUSETOOLEXTRUDEFACE *g3dmousetoolextrudeface_new ( );
G3DMOUSETOOLEXTRUDEFACE *g3dmousetoolextrudeinner_new ( );
G3DMOUSETOOLUNTRIANGULATE *g3dmousetooluntriangulate_new ( );
G3DMOUSETOOLTRIANGULATE *g3dmousetooltriangulate_new ( );
G3DMOUSETOOLROUNDSPLINEPOINT *g3dmousetoolroundsplinepoint_new ( );
G3DMOUSETOOLWELDVERTICES *g3dmousetoolweldvertices_new ( );
G3DMOUSETOOLWELDNEIGHBOURS *g3dmousetoolweldneighbours_new ( );
G3DMOUSETOOLINVERTNORMAL *g3dmousetoolinvertnormal_new ( );
G3DMOUSETOOLCREATEFACEGROUP *g3dmousetoolcreatefacegroup_new ( );
G3DMOUSETOOLMOVE *g3dmousetoolmove_new ( );
G3DMOUSETOOLSCALE *g3dmousetoolscale_new ( );
G3DMOUSETOOLROTATE *g3dmousetoolrotate_new ( );
G3DMOUSETOOLPICKUV *g3dmousetoolpickUV_new ( );
G3DMOUSETOOLMOVEUV *g3dmousetoolmoveUV_new ( );
G3DMOUSETOOLSCALEUV *g3dmousetoolscaleUV_new ( );
G3DMOUSETOOLROTATEUV *g3dmousetoolrotateUV_new ( );
G3DMOUSETOOLREMOVEVERTEXPOSE *g3dmousetoolremovevertexpose_new ( );

/******************************************************************************/
L3DMOUSETOOLSELECTOR *l3dmousetoolselect_new ( );
L3DMOUSETOOLPEN          *l3dmousetoolpen_new ( );
L3DMOUSETOOLERASER       *l3dmousetooleraser_new ( );
L3DMOUSETOOLBUCKET *l3dmousetoolbucket_new ( );

#endif
