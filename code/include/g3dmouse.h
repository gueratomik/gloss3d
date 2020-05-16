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
#include <g3durmanager.h>

/******************************************************************************/
#define PICKTOOL                     "Pick"
#define SCULPTTOOL                   "Sculpt"
#define SMOOTHTOOL                   "Smooth"
#define MOVETOOL                     "Move"
#define ROTATETOOL                   "Rotate"
#define SCALETOOL                    "Scale"
#define EXTRUDETOOL                  "Extrude"
#define EXTRUDINNERTOOL              "Extrude Inner"
#define CREATESPHERETOOL             "Create Sphere"
#define CREATECUBETOOL               "Create Cube"
#define CREATECYLINDERTOOL           "Create Cylinder"
#define CREATETORUSTOOL              "Create Torus"
#define CREATEBONETOOL               "Create Bone"
#define CREATEPLANETOOL              "Create Plane"
#define CUTMESHTOOL                  "Cutter"
#define ADDVERTEXTOOL                "Add Vertex"
#define REVERTSPLINETOOL             "Revert Spline"
#define BRIDGETOOL                   "Bridge"
#define PAINTWEIGHTTOOL              "Paint Weight"
#define UNTRIANGULATETOOL            "Untriangulate"
#define TRIANGULATETOOL              "Triangulate"
#define CREATEFACEGROUPTOOL          "Create Facegroup"
#define WELDVERTICESTOOL             "Weld vertices"
#define WELDNEIGHBOURVERTICESTOOL    "Weld neighbour vertices"
#define INVERTNORMALTOOL             "Invert Normal"
#define ROUNDSPLINEPOINTTOOL         "Round selection"

#define PICKUVTOOL                   "Pick UV"
#define MOVEUVTOOL                   "Move UV"
#define ROTATEUVTOOL                 "Rotate UV"
#define SCALEUVTOOL                  "Scale UV"

/*** dont set the tool as the current one ***/
#define MOUSETOOLNOCURRENT ( 1  << 0  )

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

#define REDRAWVIEW            ( 1      )
#define REDRAWUVMAPEDITOR     ( 1 << 1 )
#define REDRAWLIST            ( 1 << 2 )
#define REDRAWCOORDS          ( 1 << 3 )
#define REDRAWPARAMS          ( 1 << 4 )
#define REDRAWCURRENTOBJECT   ( 1 << 5 )
#define REDRAWTIMELINE        ( 1 << 6 )
#define NOBUFFEREDSUBDIVISION ( 1 << 7 )
#define BUFFEREDSUBDIVISIONOK ( 1 << 8 )
#define REDRAWALL             ( REDRAWVIEW | REDRAWLIST | REDRAWCOORDS | \
                                REDRAWUVMAPEDITOR |\
                                REDRAWPARAMS | REDRAWCURRENTOBJECT | \
                                REDRAWTIMELINE )

/******************************************************************************/
typedef struct {
	int type;             /* G3DKeyPress or G3DKeyRelease */
	int x, y;             /* pointer x, y coordinates in event window */
	unsigned int state;   /* key or button mask */
	unsigned int keycode; /* detail */
} G3DKeyEvent;

/******************************************************************************/
typedef struct {
	int type;            /* G3DButtonPress or G3DButtonRelease */
	int x, y;            /* pointer x, y coordinates in event window */
	unsigned int state;  /* key or button mask */
	unsigned int button; /* detail */
} G3DButtonEvent;

/******************************************************************************/
typedef struct {
	int type;            /* G3DMotionNotify */
	int x, y;            /* pointer x, y coordinates in event window */
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
    /*** bitmap must be 0x10 x 0x10 ***/
    const char **icon;
    /*** init function is called when tool is selected ***/
    uint32_t (*init) ( struct _G3DMOUSETOOL *,
                              G3DSCENE *,
                              G3DCAMERA *,
                              G3DURMANAGER *,
                              uint32_t );
    void (*draw) ( struct _G3DMOUSETOOL *, G3DSCENE *, uint32_t ); /* drawing */
    int  (*tool) ( struct _G3DMOUSETOOL *,
                           G3DSCENE *,
                           G3DCAMERA *,
                           G3DURMANAGER *,
                           uint32_t, G3DEvent * ); /** mouse event callback **/
    void *data; /*** private datas ***/
    uint32_t flags;
} G3DMOUSETOOL;

/******************************************************************************/
typedef struct _G3DBRIDGETOOL {
    uint32_t      draw; /*** true or false ***/
    G3DVERTEX    *ver[0x04];
    G3DCURVEPOINT *pt[0x02];
    G3DOBJECT     *obj; /*** the mesh that gest the new face ****/
} G3DBRIDGETOOL;

/******************************************************************************/
typedef struct _G3DCREATEVERTEX {
    uint32_t dummy;
} G3DCREATEVERTEX;

/******************************************************************************/
typedef struct _G3DEXTRUDEFACE {
    uint32_t inner; /*** true or false ***/
} G3DEXTRUDEFACE;

/******************************************************************************/
#define CTRLCLICK ( 1 << 0 )

typedef struct G3DPICKTOOL {
    int start; 
    int32_t coord[0x04];   /*** x1, y1, x2, y2                     ***/
    uint32_t only_visible; /*** true or false                      ***/
    float weight;          /*** Paint weight                       ***/
    uint32_t operation;    /*** Add (0x01) or remove (0x00) weight ***/
    uint32_t radius;
} G3DPICKTOOL;

/******************************************************************************/
typedef struct _SCULPTFACE {
    G3DFACE *fac;
    G3DRTVERTEX *rtvermem;
} SCULPTFACE;

/******************************************************************************/
typedef struct G3DSCULPTTOOL {
    int32_t coord[0x04];   /*** x1, y1, x2, y2                     ***/
    int only_visible;
    int radius;
    float pressure;
    int ctrl_key;
    LIST *lscf; /*** SCULPTFACE list ***/
} G3DSCULPTTOOL;

/******************************************************************************/
typedef struct _G3DSMOOTHTOOL {
    int32_t coord[0x04];   /*** x1, y1, x2, y2                     ***/
    int only_visible;
    int radius;
    float pressure;
    int ctrl_key;
} G3DSMOOTHTOOL;

/******************************************************************************/
typedef struct _G3DCUTMESH {
    int start;
    GLdouble coord[0x04][0x03];
    uint32_t restrict_to_selection; /*** true or false ***/
} G3DCUTMESH;

/******************************************************************************/
typedef struct _G3DMESHFAC {
    G3DMESH *mes;
    LIST *lfac;
} G3DMESHFAC;

/******************************************************************************/
G3DMOUSETOOL *g3dmousetool_new ( char *, char , const char **,
                                 uint32_t (*) ( G3DMOUSETOOL *,
                                                G3DSCENE *,
                                                G3DCAMERA *,
                                                G3DURMANAGER *, 
                                                uint32_t ),
                                 void (*) ( G3DMOUSETOOL *,
                                            G3DSCENE *, uint32_t ),
                                 int  (*) ( G3DMOUSETOOL *,
                                            G3DSCENE *,
                                            G3DCAMERA *,
                                            G3DURMANAGER *,
                                            uint32_t, G3DEvent * ),
                                 uint32_t );

/******************************************************************************/
int createSphere   ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *,
                     G3DURMANAGER *, uint32_t, G3DEvent * );
int createCube     ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *,
                     G3DURMANAGER *, uint32_t, G3DEvent * );
int createCylinder ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *,
                     G3DURMANAGER *, uint32_t, G3DEvent * );
int createVertex   ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *,
                     G3DURMANAGER *, uint32_t, G3DEvent * );
int createBone     ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *,
                     G3DURMANAGER *, uint32_t, G3DEvent * );
int createTorus    ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *,
                     G3DURMANAGER *, uint32_t, G3DEvent * );
int createPlane    ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *,
                     G3DURMANAGER *, uint32_t, G3DEvent * );
/******************************************************************************/
uint32_t createVertex_init ( G3DMOUSETOOL *mou, G3DSCENE *sce, 
                                                G3DCAMERA *cam,
                                                G3DURMANAGER *urm, 
                                                uint32_t engine_flags );

/******************************************************************************/
G3DBRIDGETOOL  *bridge_new    ( );
uint32_t        bridge_init   ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *,
                                                                G3DURMANAGER *,
                                                                uint32_t );
void            bridge_draw   ( G3DMOUSETOOL *, G3DSCENE *, uint32_t );
int             bridge_tool   ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *, 
                                                                G3DURMANAGER *, 
                                                                uint32_t, 
                                                                G3DEvent * );

/******************************************************************************/
G3DCUTMESH     *cutMesh_new       ( );
uint32_t        cutMesh_init      ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *,
                                                                G3DURMANAGER *,
                                                                uint32_t );
void            cutMesh_draw      ( G3DMOUSETOOL *, G3DSCENE *, uint32_t );
int             cutMesh_tool      ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *,
                                                                G3DURMANAGER *, 
                                                                uint32_t,
                                                                G3DEvent * );

/******************************************************************************/
uint32_t        paintWeight_init  ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *,
                                                                G3DURMANAGER *,
                                                                uint32_t );
int             paintWeight_tool  ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *,
                                                                G3DURMANAGER *, 
                                                                uint32_t,
                                                                G3DEvent * );
/******************************************************************************/
uint32_t        sculptTool_init  ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *,
                                                                G3DURMANAGER *,
                                                                uint32_t );
int             sculpt_tool      ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *,
                                                                G3DURMANAGER *, 
                                                                uint32_t,
                                                                G3DEvent * );

/******************************************************************************/
uint32_t        smoothTool_init  ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *,
                                                                G3DURMANAGER *,
                                                                uint32_t );
int             smooth_tool      ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *,
                                                                G3DURMANAGER *, 
                                                                uint32_t,
                                                                G3DEvent * );

/******************************************************************************/
int moveUV_tool ( G3DMOUSETOOL *mou, 
                  G3DSCENE     *sce, 
                  G3DCAMERA    *cam,
                  G3DURMANAGER *urm,
                  uint32_t      eflags, 
                  G3DEvent     *event );

int scaleUV_tool ( G3DMOUSETOOL *mou, 
                   G3DSCENE     *sce, 
                   G3DCAMERA    *cam,
                   G3DURMANAGER *urm,
                   uint32_t      eflags, 
                   G3DEvent     *event );

int rotateUV_tool ( G3DMOUSETOOL *mou, 
                    G3DSCENE     *sce, 
                    G3DCAMERA    *cam,
                    G3DURMANAGER *urm,
                    uint32_t      eflags, 
                    G3DEvent     *event );

/******************************************************************************/
G3DPICKTOOL    *pickTool_new      ( );
uint32_t        pickTool_init     ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *,
                                                                G3DURMANAGER *,
                                                                uint32_t );
void            pick_draw         ( G3DMOUSETOOL *, G3DSCENE *, uint32_t );
int             pick_tool         ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *,
                                                                G3DURMANAGER *,
                                                                uint32_t,
                                                                G3DEvent * );
int pickUV_tool ( G3DMOUSETOOL *mou, G3DSCENE *sce, G3DCAMERA *cam,
                  G3DURMANAGER *urm, uint32_t flags, G3DEvent *event );
void pick_Item ( G3DPICKTOOL *pt, 
                 G3DSCENE   *sce, 
                 G3DCAMERA  *cam,
                 uint32_t    ctrlClick,
                 uint32_t    flags );
void pick_cursor ( G3DPICKTOOL *pt, 
                   G3DSCENE   *sce, 
                   G3DCAMERA  *cam,
                   uint32_t    eflags );

/******************************************************************************/
G3DEXTRUDEFACE *extrudeFace_new   ( );
uint32_t        extrudeFace_init  ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *,
                                                                G3DURMANAGER *,
                                                                uint32_t );
uint32_t        extrudeInner_init ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *,
                                                                G3DURMANAGER *,
                                                                uint32_t );
int             extrudeFace_tool  ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *,
                                                                G3DURMANAGER *,
                                                                uint32_t,
                                                                G3DEvent * );

/******************************************************************************/
int move_tool ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *, G3DURMANAGER *, 
                                                         uint32_t,
                                                         G3DEvent * );

/******************************************************************************/
int scale_tool ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *, G3DURMANAGER *, 
                                                          uint32_t,
                                                          G3DEvent * );

/******************************************************************************/
int rotate_tool ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *, G3DURMANAGER *, 
                                                           uint32_t,
                                                           G3DEvent * );

/******************************************************************************/
uint32_t roundSplinePoint_init  ( G3DMOUSETOOL *mou,
                                  G3DSCENE     *sce, 
                                  G3DCAMERA    *cam,
                                  G3DURMANAGER *urm, 
                                  uint32_t      engine_flags );

/******************************************************************************/
uint32_t untriangulate_init ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *, 
                                                          G3DURMANAGER *, 
                                                          uint32_t );

/******************************************************************************/
uint32_t triangulate_init ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *, 
                                                        G3DURMANAGER *, 
                                                        uint32_t );

/******************************************************************************/
uint32_t weldvertices_init ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *, 
                                                         G3DURMANAGER *, 
                                                         uint32_t );

/******************************************************************************/
uint32_t weldneighbourvertices_init ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *, 
                                                         G3DURMANAGER *, 
                                                         uint32_t );

/******************************************************************************/
uint32_t invertNormal_init ( G3DMOUSETOOL *, G3DSCENE *, G3DCAMERA *, 
                                                         G3DURMANAGER *, 
                                                         uint32_t );

/******************************************************************************/
uint32_t revertSpline_init  ( G3DMOUSETOOL *mou, 
                              G3DSCENE     *sce, 
                              G3DCAMERA    *cam,
                              G3DURMANAGER *urm, 
                              uint32_t      engine_flags );

/******************************************************************************/
uint32_t createFacegroup_init  ( G3DMOUSETOOL *mou, G3DSCENE *sce, 
                                                    G3DCAMERA *cam,
                                                    G3DURMANAGER *urm, 
                                                    uint32_t engine_flags );

#endif
