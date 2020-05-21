/******************************************************************************/
/*    GLOSS is free software: you can redistribute it and/or modify           */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation, either version 3 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*    GLOSS is distributed in the hope that it will be useful,                */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with Foobar.  If not, see http://www.gnu.org/licenses/.             */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/

#ifndef _G3DURMANAGER_H_
#define _G3DURMANAGER_H_

/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/******************************************************************************/
#include <time.h>

/******************************************************************************/
#include <string.h>
#include <math.h>

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>

/******************************************************************************/
#include <list.h>
#include <g3dengine/g3dengine.h>

/******************************************************************************/
#define G3DURMCOMMIT 0x01

/******************************************************************************/
typedef struct _URMADDOBJECT {
    LIST      *lobj; /*** we can add several objects. e.g when Copy-Pasting ***/
    G3DOBJECT *oldpar;
    G3DOBJECT *newpar;
    G3DSCENE  *sce;
    LIST      *lsel;
} URMADDOBJECT;

/******************************************************************************/
typedef struct _URMINVERTNORMAL {
    G3DMESH *mes;
    LIST      *lfac;
} URMINVERTNORMAL;

/******************************************************************************/
typedef struct _URMADDWEIGHTGROUP {
    G3DMESH *mes;
    LIST    *lgrp; /*** list of added or deleted weightgroups ***/
} URMADDWEIGHTGROUP, URMDELWEIGHTGROUP;

/******************************************************************************/
typedef struct _URMCONVERTPRIMITIVE {
    G3DPRIMITIVE *pri;
    G3DMESH      *mes;
    G3DOBJECT    *par;
    G3DSCENE     *sce;
} URMCONVERTPRIMITIVE;

/******************************************************************************/
typedef struct _URMDELSELITEMS {
    G3DSCENE *sce;
    uint32_t flags;
    G3DMESH *mes;
    LIST *loldselfac, *lnewselfac;
    LIST *loldseledg, *lnewseledg;
    LIST *loldselver, *lnewselver;
    LIST *loldselobj, *lnewselobj;
} URMDELSELITEMS;

/******************************************************************************/
typedef struct _URMWELDVERTICES {
    G3DMESH *mes;
    LIST *loldver;
    LIST *loldfac;
    LIST *lnewfac;
    LIST *lnewver;
} URMWELDVERTICES;

/******************************************************************************/
typedef struct _URMSPLITMESH {
    G3DMESH *mes;
    G3DMESH *splmes;
    LIST    *loldver;
    LIST    *loldfac;
} URMSPLITMESH;

/******************************************************************************/
typedef struct _URMMOVEVERTICES {
    G3DMESH *mes;
    LIST *lver;
    LIST *ledg;
    LIST *lfac;
    LIST *lsub;
    G3DVECTOR *oldpos;
    G3DVECTOR *newpos;
} URMMOVEVERTICES;

/******************************************************************************/
typedef struct _URMMOVEUVS {
    G3DUVMAP *uvmap;
    LIST     *luv;
    G3DUV    *olduv;
    G3DUV    *newuv;
} URMMOVEUVS;

/******************************************************************************/
typedef struct _URMCUTMESH {
    G3DMESH *mes;
    LIST *loldfac;
    LIST *lnewver;
    LIST *lnewfac;
} URMCUTMESH;

/******************************************************************************/
typedef struct _URMCUTSPLINE {
    G3DSPLINE *spline;
    LIST      *laddedPoints;
    LIST      *laddedSegments;
    LIST      *lremovedSegments;
} URMCUTSPLINE;

/******************************************************************************/
typedef struct _URMUNTRIANGULATE {
    G3DMESH *mes;
    LIST *loldfac;
    LIST *lnewfac;
} URMUNTRIANGULATE, URMTRIANGULATE;

/******************************************************************************/
typedef struct _URMEXTRUDEMESH {
    G3DMESH *mes;
    LIST *loriver;
    LIST *loldfac;
    LIST *lnewver;
    LIST *lnewfac;
    G3DVECTOR *oldpos;
    G3DVECTOR *newpos;
} URMEXTRUDEMESH;

/******************************************************************************/
typedef struct _URMREMOVETEXTURE {
    G3DOBJECT   *obj;
    G3DTEXTURE  *tex;
    LIST        *lfacgrp;
} URMREMOVETEXTURE;

/******************************************************************************/
typedef struct _URMADDTEXTURE {
    G3DOBJECT   *obj;
    G3DTEXTURE  *tex;
} URMADDTEXTURE;

/******************************************************************************/
typedef struct _URMADDMATERIAL {
    G3DSCENE    *sce;
    G3DMATERIAL *mat;
} URMADDMATERIAL;

/******************************************************************************/
typedef struct _URMREMOVEMATERIAL {
    G3DSCENE    *sce;
    G3DMATERIAL *mat;
    LIST       *ludt; /*** list of URMDELETETEXTURE ***/
} URMREMOVEMATERIAL;

/******************************************************************************/
typedef struct _URMREMOVEUVMAP {
    G3DMESH  *mes;
    G3DUVMAP *uvmap;
    LIST     *lolduvset;
    LIST     *loldtex;
} URMREMOVEUVMAP;

/******************************************************************************/
#define UTOSAVETRANSLATION ( 1 << 0 )
#define UTOSAVEROTATION    ( 1 << 1 )
#define UTOSAVESCALING     ( 1 << 2 )
#define UTOSAVEAXIS        ( 1 << 3 )

#define UTOSAVESTATEBEFORE ( 0x01 )
#define UTOSAVESTATEAFTER  ( 0x02 )

typedef struct _URMTRANSFORMOBJECT {
    LIST      *lobj;
    G3DVECTOR *oldpos;
    G3DVECTOR *oldrot;
    G3DVECTOR *oldsca;
    G3DVECTOR *newpos;
    G3DVECTOR *newrot;
    G3DVECTOR *newsca;
    uint32_t   save_type;
} URMTRANSFORMOBJECT;

/******************************************************************************/
#define UMPSAVESELECTEDPOINTS ( 1 << 0 )
#define UMPSAVECURRENTHANDLE  ( 1 << 1 )

#define UMPSAVESTATEBEFORE ( 0x01 )
#define UMPSAVESTATEAFTER  ( 0x02 )

typedef struct _URMMOVEPOINT {
    G3DSPLINE     *spl;
    LIST          *lpt;    /*** list of curve points ***/
    G3DCURVEPOINT *curhan; /*** the current selected handle ***/
    G3DVECTOR     *oldpos;
    G3DVECTOR     *newpos;
    G3DVECTOR      oldhanpos;
    G3DVECTOR      newhanpos;
    uint32_t       save_type;
} URMMOVEPOINT;

/******************************************************************************/
typedef struct _URMSELECTITEM {
    G3DOBJECT *obj;
    LIST *loldobjsel;
    LIST *loldversel;
    LIST *loldfacsel;
    LIST *loldedgsel;
    LIST *lnewobjsel;
    LIST *lnewversel;
    LIST *lnewfacsel;
    LIST *lnewedgsel;
    uint32_t engine_flags;
} URMSELECTITEM;

/******************************************************************************/
typedef struct _URMCREATEFACE {
    G3DMESH *mes;
    G3DFACE *fac;
} URMCREATEFACE;

/******************************************************************************/
typedef struct _URMADDVERTEX {
    G3DMESH *mes;
    G3DVERTEX *ver;
} URMADDVERTEX;

/******************************************************************************/
typedef struct _URMCREATEFACEGROUP {
    G3DMESH      *mes;
    G3DFACEGROUP *facgrp;
} URMCREATEFACEGROUP;

/******************************************************************************/
typedef struct _URMOBJECTPOSE {
    G3DOBJECT *obj;
    float      frame;
    G3DKEY    *key;
    G3DKEY    *overwrittenKey;
    G3DVECTOR  keypos;
    G3DVECTOR  keyrot;
    G3DVECTOR  keysca;
} URMOBJECTPOSE;

/******************************************************************************/
typedef struct _URMOBJECTREMOVEKEYS {
    G3DOBJECT *obj;
    LIST      *lkey;
} URMOBJECTREMOVEKEYS;

/******************************************************************************/
typedef struct _URMADDSPLINEPOINT {
    G3DSPLINE        *spline;
    G3DSPLINEPOINT   *pt;
    G3DSPLINESEGMENT *seg;
} URMADDSPLINEPOINT;

/******************************************************************************/
typedef struct _URMROUNDSPLINEPOINT {
    G3DSPLINE  *spline;
    LIST       *lselectedPoints;
    LIST       *lsegments;
    G3DVECTOR (*pos)[0x02];
} URMROUNDSPLINEPOINT;

/******************************************************************************/
typedef struct _URMADDSPLINESEGMENT {
    G3DSPLINE        *spline;
    G3DSPLINESEGMENT *seg;
} URMADDSPLINESEGMENT;

/******************************************************************************/
typedef struct _URMDELETESPLINEPOINTS {
    G3DSPLINE *spline;
    LIST      *lremovedPoints;
    LIST      *lremovedSegments;
} URMDELETESPLINEPOINTS;

/******************************************************************************/
typedef struct _G3DURMANAGER {
    LIST *lprocs;
    LIST *lcurprocs;
    uint32_t level; /*** undo level - 0 = infinite ***/
    /*LIST *lgarbobj;
    LIST *lgarbver;
    LIST *lgarbedg;
    LIST *lgarbfac;*/
} G3DURMANAGER;

/******************************************************************************/
typedef struct _G3DURITEM {
    uint32_t commit;
    void (*undo) ( G3DURMANAGER *, void *, uint32_t ); /*** called if g3durmanager_undo is called  ***/
    void (*redo) ( G3DURMANAGER *, void *, uint32_t ); /*** called if g3durmanager_redo is called  ***/
    void (*free) ( void *, uint32_t );
    void *data;
    uint32_t return_flags;
} G3DURITEM;

/******************************************************************************/
/*typedef struct _G3DURMTRASH {
    void *data;
} G3DURMTRASH;*/

/******************************************************************************/
G3DURMANAGER *g3durmanager_new ( uint32_t );
void g3durmanager_init ( G3DURMANAGER *, uint32_t );
uint32_t g3durmanager_undo ( G3DURMANAGER *, uint32_t );
uint32_t g3durmanager_redo ( G3DURMANAGER *, uint32_t );
void g3durmanager_push ( G3DURMANAGER *, void (*) ( G3DURMANAGER *, void *, uint32_t ),
                                         void (*) ( G3DURMANAGER *, void *, uint32_t ),
                                         void (*) ( void *, uint32_t ),
                                         void *, 
                                         uint32_t  );
void g3duritem_free ( G3DURITEM * );
void g3durmanager_clearFrom ( G3DURMANAGER *, LIST ** );
void g3durmanager_clear     ( G3DURMANAGER * );
void g3durmanager_free ( G3DURMANAGER *urm );

/******************************************************************************/
G3DURITEM *g3duritem_new ( void (*) ( G3DURMANAGER *,void *, uint32_t ),/* undo func    */
                           void (*) ( G3DURMANAGER *,void *, uint32_t ),/* redo func    */
                           void (*) ( void *, uint32_t ),     /* free func    */
                           void *,                            /* user data    */
                           uint32_t );                        /* return flags */

/******************************************************************************/
URMADDOBJECT *urmaddobject_new          ( LIST *, G3DOBJECT *,
                                                  G3DOBJECT *, G3DSCENE * );
void          urmaddobject_free         ( URMADDOBJECT * );
void          addObject_undo            ( G3DURMANAGER *, void *, uint32_t );
void          addObject_redo            ( G3DURMANAGER *, void *, uint32_t );
void          addObject_free            ( void *, uint32_t );
void          g3durm_object_addChild    ( G3DURMANAGER *, G3DSCENE *,
                                                          uint32_t   ,
                                                          uint32_t,
                                                          G3DOBJECT *,
                                                          G3DOBJECT *,
                                                          G3DOBJECT * );
void          g3durm_object_addChildren ( G3DURMANAGER *, G3DSCENE  *,
                                                          uint32_t   ,
                                                          uint32_t   ,
                                                          G3DOBJECT *,
                                                          G3DOBJECT *,
                                                          LIST      * );

/******************************************************************************/
URMDELSELITEMS *urmdelselitems_new ( G3DSCENE *, LIST *,
                                     G3DMESH  *, LIST *,
                                                 LIST *,
                                                 LIST *, uint32_t );
void urmdelselitems_free ( URMDELSELITEMS * );
void deleteSelectedItems_undo ( G3DURMANAGER *, void *, uint32_t );
void deleteSelectedItems_redo ( G3DURMANAGER *, void *, uint32_t );
void deleteSelectedItems_free ( void *, uint32_t );
void g3durm_scene_deleteSelectedObjects ( G3DURMANAGER *,
                                          G3DSCENE *, uint32_t, uint32_t );

/******************************************************************************/
URMCONVERTPRIMITIVE *urmconvertprimitive_new ( G3DPRIMITIVE *, 
                                               G3DMESH *, G3DOBJECT *, 
                                                               G3DSCENE  * );
void urmconvertprimitive_free ( URMCONVERTPRIMITIVE * );
void convertPrimitive_undo ( G3DURMANAGER *, void *, uint32_t );
void convertPrimitive_redo ( G3DURMANAGER *, void *, uint32_t );
void convertPrimitive_free (  void *, uint32_t );
void g3durm_primitive_convert ( G3DURMANAGER *, G3DSCENE  *, uint32_t ,
                                G3DPRIMITIVE *, G3DOBJECT *, uint32_t );

/******************************************************************************/
URMWELDVERTICES *urmweldvertices_new ( G3DMESH *, LIST *, LIST *,
                                                          LIST *, LIST * );
void urmweldvertices_free ( URMWELDVERTICES * );
void weldVertices_redo ( G3DURMANAGER *, void *, uint32_t );
void weldVertices_undo ( G3DURMANAGER *, void *, uint32_t );
void weldVertices_free ( void *, uint32_t );
void g3durm_mesh_weldSelectedVertices ( G3DURMANAGER *, G3DMESH *, uint32_t,
                                                                   uint32_t,
                                                                   uint32_t );
void g3durm_mesh_weldNeighbourVertices ( G3DURMANAGER *urm, 
                                         G3DMESH      *mes, 
                                         uint32_t      type,
                                         float         distance,
                                         uint32_t engine_flags,
                                         uint32_t return_flags );

/******************************************************************************/
URMMOVEVERTICES *urmmovevertices_new ( G3DMESH *, 
                                       LIST *, LIST *, LIST *, LIST *,
                                       G3DVECTOR *, G3DVECTOR * );
void urmmovevertices_free ( URMMOVEVERTICES * );
void moveVertices_undo ( G3DURMANAGER *, void *, uint32_t );
void moveVertices_redo ( G3DURMANAGER *, void *, uint32_t );
void moveVertices_free ( void *, uint32_t );
void g3durm_mesh_moveVertexList ( G3DURMANAGER *, G3DMESH *, 
                                  LIST *, LIST *, LIST *, LIST *,
                                  G3DVECTOR *, G3DVECTOR *, uint32_t );

/******************************************************************************/
URMCUTMESH *urmcutmesh_new ( G3DMESH *, LIST *, LIST *, LIST * );
void urmcutmesh_free ( URMCUTMESH * );
void cutMesh_undo ( G3DURMANAGER *, void *, uint32_t );
void cutMesh_redo ( G3DURMANAGER *, void *, uint32_t );
void cutMesh_free ( void *, uint32_t );
void g3durm_mesh_cut ( G3DURMANAGER *, G3DMESH *, G3DFACE *, int, uint32_t, uint32_t );

/******************************************************************************/
URMEXTRUDEMESH *urmextrudemesh_new ( G3DMESH *, LIST *,
                                                LIST *,
                                                LIST *,
                                                LIST *,
                                                G3DVECTOR *,
                                                G3DVECTOR * );

void urmextrudemesh_free ( URMEXTRUDEMESH * );
void extrudeMesh_free ( void *, uint32_t  );
void extrudeMesh_undo ( G3DURMANAGER *, void *, uint32_t );
void extrudeMesh_redo ( G3DURMANAGER *, void *, uint32_t );
void g3durm_mesh_extrude ( G3DURMANAGER *, G3DMESH *, LIST *,
                                                      LIST *, 
                                                      LIST *, 
                                                      LIST *,
                                                      G3DVECTOR *,
                                                      G3DVECTOR *, uint32_t );

/******************************************************************************/
URMTRANSFORMOBJECT *urmtransformobject_new ( LIST      *lobj,
                                             uint32_t   save_flags );
void urmtransformobject_free ( URMTRANSFORMOBJECT *uto );
void transformObject_free ( void *data, uint32_t commit );
void transformObject_undo ( G3DURMANAGER *urm, void *data, uint32_t flags );
void transformObject_redo ( G3DURMANAGER *urm, void *data, uint32_t flags );
void urmtransform_saveState ( URMTRANSFORMOBJECT *uto, uint32_t save_time );
URMTRANSFORMOBJECT *g3durm_object_transform ( G3DURMANAGER *urm, 
                                              LIST         *lobj,
                                              uint32_t      save_type,
                                              uint32_t      return_flags );
URMTRANSFORMOBJECT *g3durm_uvmap_transform ( G3DURMANAGER *urm, 
                                             G3DUVMAP     *uvmap,
                                             uint32_t      save_type,
                                             uint32_t      return_flags );

/******************************************************************************/
URMUNTRIANGULATE *urmuntriangulate_new ( G3DMESH *, LIST *, LIST * );
void urmuntriangulate_free     ( URMUNTRIANGULATE * );
void unTriangulate_undo        ( G3DURMANAGER *, void *, uint32_t );
void unTriangulate_redo        ( G3DURMANAGER *, void *, uint32_t );
void unTriangulate_free        ( void *, uint32_t );
void g3durm_mesh_untriangulate ( G3DURMANAGER *, G3DMESH *, uint32_t, uint32_t );
void g3durm_mesh_triangulate   ( G3DURMANAGER *, G3DMESH *, int, uint32_t, uint32_t );

/******************************************************************************/
URMCREATEFACE *urmcreateface_new ( G3DMESH *, G3DFACE * );
void urmcreateface_free ( URMCREATEFACE * );
void createFace_free ( void *, uint32_t  );
void createFace_undo ( G3DURMANAGER *, void *, uint32_t );
void createFace_redo ( G3DURMANAGER *, void *, uint32_t );
void g3durm_mesh_createFace ( G3DURMANAGER *, G3DMESH *, G3DFACE *, uint32_t );

/******************************************************************************/
URMSELECTITEM *urmselectitem_new ( G3DOBJECT *, LIST *, LIST *, LIST *, LIST *,
                                                LIST *, LIST *, LIST *, LIST *,
                                                uint32_t );
void urmselectitem_free ( URMSELECTITEM * );
void selectItem_free ( void *, uint32_t );
void selectItem_undo ( G3DURMANAGER *, void *, uint32_t );
void selectItem_redo ( G3DURMANAGER *, void *, uint32_t );
void g3durm_mesh_pickVertices ( G3DURMANAGER *, G3DMESH *,
                                                LIST *,
                                                LIST *,
                                                uint32_t,
                                                uint32_t );
void g3durm_mesh_pickFaces    ( G3DURMANAGER *, G3DMESH *,
                                                LIST *,
                                                LIST *,
                                                uint32_t,
                                                uint32_t );
void g3durm_scene_pickObject  ( G3DURMANAGER *, G3DSCENE *,
                                                LIST *,
                                                LIST *,
                                                uint32_t,
                                                uint32_t );
void g3durm_uvmap_pickUVs ( G3DURMANAGER *urm, 
                            G3DUVMAP     *uvmap,
                            LIST        *loldseluv,
                            LIST        *lnewseluv,
                            uint32_t     engine_flags,
                            uint32_t     return_flags );

void g3durm_uvmap_pickUVSets ( G3DURMANAGER *urm, 
                               G3DUVMAP     *uvmap,
                               LIST        *loldseluv,
                               LIST        *lnewseluv,
                               uint32_t     engine_flags,
                               uint32_t     return_flags );

/******************************************************************************/
URMADDVERTEX *urmaddvertex_new ( G3DMESH *, G3DVERTEX * );
void urmaddvertex_free ( URMADDVERTEX * );
void addVertex_free ( void *, uint32_t  );
void addVertex_undo ( G3DURMANAGER *, void *, uint32_t );
void addVertex_redo ( G3DURMANAGER *, void *, uint32_t );
void g3durm_mesh_addVertex ( G3DURMANAGER *, G3DMESH *, G3DVERTEX *, uint32_t );

/******************************************************************************/
URMINVERTNORMAL *urminvertnormal_new ( G3DMESH *mes );
void urminvertnormal_free ( URMINVERTNORMAL * );
void invertNormal_free ( void *, uint32_t  );
void invertNormal_undo ( G3DURMANAGER *, void *, uint32_t );
void invertNormal_redo ( G3DURMANAGER *, void *, uint32_t );
void g3durm_mesh_invertNormal ( G3DURMANAGER *, G3DMESH *, uint32_t, uint32_t );

/******************************************************************************/
void g3durm_spline_deletePoints ( G3DURMANAGER *urm,
                                  G3DSPLINE    *spline, 
                                  LIST         *lremovedPoints,
                                  uint32_t      engine_flags,
                                  uint32_t      return_flags );
void deleteSplinePoints_redo ( G3DURMANAGER *urm, 
                               void         *data, 
                               uint32_t      engine_flags );
void deleteSplinePoints_undo ( G3DURMANAGER *urm, 
                               void         *data, 
                               uint32_t      engine_flags );
void deleteSplinePoints_free ( void    *data, 
                               uint32_t commit );
void urmDeleteSplinePoints_free ( URMDELETESPLINEPOINTS *dsp );
URMDELETESPLINEPOINTS *urmDeleteSplinePoints_new ( G3DSPLINE *spline, 
                                                   LIST      *lremovedPoints,
                                                   LIST      *lremovedSegments );

/******************************************************************************/
void g3durm_spline_cut ( G3DURMANAGER *urm,
                         G3DSPLINE    *spline, 
                         G3DFACE      *knife,
                         uint32_t      engine_flags,
                         uint32_t      return_flags );
void cutSpline_redo ( G3DURMANAGER *urm, 
                      void         *data, 
                      uint32_t      engine_flags );
void cutSpline_undo ( G3DURMANAGER *urm, 
                      void         *data, 
                      uint32_t      engine_flags );
void cutSpline_free ( void    *data, 
                      uint32_t commit );
void urmCutSpline_free ( URMCUTSPLINE *csp );
URMCUTSPLINE *urmCutSpline_new ( G3DSPLINE *spline, 
                                 LIST      *laddedPoints,
                                 LIST      *laddedSegments,
                                 LIST      *lremovedSegments );

/******************************************************************************/
URMADDSPLINEPOINT *urmAddSplinePoint_new ( G3DSPLINE        *spline, 
                                           G3DSPLINEPOINT   *pt,
                                           G3DSPLINESEGMENT *seg );
void urmAddSplinePoint_free ( URMADDSPLINEPOINT *asp );
void addSplinePoint_free ( void *data, uint32_t commit );
void addSplinePoint_undo ( G3DURMANAGER *urm, 
                           void         *data,
                           uint32_t      engine_flags );
void addSplinePoint_redo ( G3DURMANAGER *urm, 
                           void         *data,
                           uint32_t      engine_flags );
void g3durm_spline_addPoint ( G3DURMANAGER     *urm,
                              G3DSPLINE        *spline, 
                              G3DSPLINEPOINT   *pt,
                              G3DSPLINESEGMENT *seg,
                              uint32_t          engine_flags,
                              uint32_t          return_flags );

/******************************************************************************/
void g3durm_spline_addSegment ( G3DURMANAGER     *urm,
                                G3DSPLINE        *spline,
                                G3DSPLINESEGMENT *seg,
                                uint32_t          engine_flags,
                                uint32_t          return_flags );
void addSplineSegment_redo ( G3DURMANAGER *urm, 
                             void         *data,
                             uint32_t      engine_flags );
void addSplineSegment_undo ( G3DURMANAGER *urm, 
                             void         *data,
                             uint32_t      engine_flags );
void addSplineSegment_free ( void *data, uint32_t commit );
void urmAddSplineSegment_free ( URMADDSPLINESEGMENT *ass );
URMADDSPLINESEGMENT *urmAddSplineSegment_new ( G3DSPLINE        *spline,
                                               G3DSPLINESEGMENT *seg );

/******************************************************************************/
void g3durm_spline_roundSelectedPoints ( G3DURMANAGER     *urm,
                                         G3DSPLINE        *spline,
                                         uint32_t          engine_flags,
                                         uint32_t          return_flags );
void roundSplinePoint_redo ( G3DURMANAGER *urm, 
                             void         *data,
                             uint32_t      engine_flags );
void roundSplinePoint_undo ( G3DURMANAGER *urm, 
                             void         *data,
                             uint32_t      engine_flags );
void roundSplinePoint_free ( void *data, uint32_t commit );
void urmRoundSplinePoint_free ( URMROUNDSPLINEPOINT *rsp );
URMROUNDSPLINEPOINT *urmRoundSplinePoint_new ( G3DSPLINE  *spline,
                                               LIST       *lselectedPoints,
                                               LIST       *lsegments,
                                               G3DVECTOR (*pos)[0x02] );

/******************************************************************************/
void g3durm_mesh_split ( G3DURMANAGER *urm, 
                         G3DMESH *mes,
                         uint32_t splID,
                         uint32_t keep,
                         uint32_t engine_flags,
                         uint32_t return_flags );
void splitMesh_redo ( G3DURMANAGER *urm, void *data, uint32_t engine_flags );
void splitMesh_undo ( G3DURMANAGER *urm, void *data, uint32_t engine_flags );
void splitMesh_free ( void *data, uint32_t commit );
void urmSplitMesh_free ( URMSPLITMESH *sms );
URMSPLITMESH *urmSplitMesh_new ( G3DMESH *mes, 
                                 G3DMESH *splmes, 
                                 LIST    *loldver,
                                 LIST    *loldfac );

/******************************************************************************/
void g3durm_spline_revert ( G3DURMANAGER *urm, 
                            G3DSPLINE    *spline,
                            uint32_t      engine_flags,
                            uint32_t      return_flags );

/******************************************************************************/
URMOBJECTPOSE *urmObjectPose_new ( G3DOBJECT *obj, 
                                   float      frame,
                                   G3DKEY    *key,
                                   G3DKEY    *overwrittenKey );
void urmObjectPose_free ( URMOBJECTPOSE *op );
void objectPose_free ( void *data, uint32_t commit );
void objectPose_undo ( G3DURMANAGER *urm, 
                       void         *data,
                       uint32_t      engine_flags );
void objectPose_redo ( G3DURMANAGER *urm, 
                       void         *data,
                       uint32_t      engine_flags );
void g3durm_object_pose ( G3DURMANAGER *urm,
                          LIST         *lobj,
                          float         frame,
                          uint32_t      key_flags,
                          uint32_t      engine_flags,
                          uint32_t      return_flags );

/******************************************************************************/
URMOBJECTREMOVEKEYS *urmObjectRemoveKeys_new ( G3DOBJECT *obj );
void urmObjectRemoveKeys_free ( URMOBJECTREMOVEKEYS *ork );
void objectRemoveKeys_free ( void *data, uint32_t commit );
void objectRemoveKeys_undo ( G3DURMANAGER *urm, 
                             void         *data,
                             uint32_t      engine_flags );
void objectRemoveKeys_redo ( G3DURMANAGER *urm, 
                             void         *data,
                             uint32_t      engine_flags );
void g3durm_objectList_removeSelectedKeys ( G3DURMANAGER *urm,
                                            LIST         *lobj,
                                            uint32_t      engine_flags,
                                            uint32_t      return_flags );

/******************************************************************************/
URMMOVEPOINT *g3durm_spline_movePoint ( G3DURMANAGER *urm,
                                        G3DSPLINE    *spl,
                                        uint32_t      save_type,
                                        uint32_t      return_flags );
void urmmovepoint_saveState ( URMMOVEPOINT *ump, uint32_t save_time );
void movePoint_redo ( G3DURMANAGER *urm, void *data, uint32_t engine_flags );
void movePoint_undo ( G3DURMANAGER *urm, void *data, uint32_t engine_flags );
void movePoint_free ( void *data, uint32_t commit );
void urmmovepoint_free ( URMMOVEPOINT *ump );
URMMOVEPOINT *urmmovepoint_new ( G3DSPLINE *spl, uint32_t save_type );

/******************************************************************************/
void g3durm_uvmap_moveUVList ( G3DURMANAGER *urm,
                               G3DUVMAP     *uvmap, 
                               LIST         *luv,
                               G3DVECTOR    *olduv,
                               G3DVECTOR    *newuv, 
                               uint32_t      return_flags );

/******************************************************************************/
void g3durm_mesh_createFacegroup ( G3DURMANAGER *urm,
                                   G3DMESH      *mes,
                                   const char   *name,
                                   uint32_t      engine_flags,
                                   uint32_t      return_flags );

/******************************************************************************/
void urmremovetexture_free ( URMREMOVETEXTURE *udt );
void g3durm_mesh_deleteTexture ( G3DURMANAGER *urm,
                                 G3DMESH      *mes,
                                 G3DTEXTURE   *tex, 
                                 uint32_t      engine_flags,
                                 uint32_t      return_flags );

/******************************************************************************/
void g3durm_mesh_addTexture ( G3DURMANAGER *urm,
                              G3DMESH      *mes,
                              G3DTEXTURE   *tex,
                              uint32_t      engine_flags,
                              uint32_t      return_flags );

/******************************************************************************/
void g3durm_scene_addMaterial ( G3DURMANAGER *urm,
                                G3DSCENE     *sce,
                                G3DMATERIAL  *mat,
                                uint32_t      engine_flags,
                                uint32_t      return_flags );

/******************************************************************************/
void g3durm_scene_removeMaterial ( G3DURMANAGER *urm,
                                   G3DSCENE     *sce,
                                   G3DMATERIAL  *mat,
                                   uint32_t      engine_flags,
                                   uint32_t      return_flags );

/******************************************************************************/
void g3durm_mesh_removeUVMap ( G3DURMANAGER *urm,
                               G3DMESH      *mes,
                               G3DUVMAP     *uvmap, 
                               uint32_t      engine_flags,
                               uint32_t      return_flags );

#endif
