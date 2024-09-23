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
#include <list.h>
#include <g3dengine/g3dengine.h>

/******************************************************************************/
#define G3DURMCOMMIT 0x01

/******************************************************************************/
typedef struct _URMADDOBJECT {
    G3DSCENE *sce;
    LIST      *lobj; /*** we can add several objects. e.g when Copy-Pasting ***/
    G3DOBJECT *oldpar;
    G3DOBJECT *newpar;
    LIST      *lsel;
} URMADDOBJECT;


/******************************************************************************/
typedef struct _URMADDWEIGHTGROUP {
    G3DSCENE *sce;
    G3DMESH *mes;
    LIST    *lgrp; /*** list of added or deleted weightgroups ***/
} URMADDWEIGHTGROUP, URMDELWEIGHTGROUP;

/******************************************************************************/
typedef struct _URMCONVERTPRIMITIVE {
    G3DSCENE     *sce;
    G3DPRIMITIVE *pri;
    G3DMESH      *mes;
    G3DOBJECT    *par;
} URMCONVERTPRIMITIVE;

/******************************************************************************/
typedef struct _URMCUTMESH {
    G3DSCENE *sce;
    G3DMESH *mes;
    LIST *loldfac;
    LIST *loldedg;
    LIST *lnewver;
    LIST *lnewfac;
    LIST *lnewedg;
} URMCUTMESH;


/******************************************************************************/
typedef struct _URMFILLHEIGTHMAP {
    G3DSCENE *sce;
    G3DSUBDIVIDER *sdr;
    G3DVECTOR3F     *pos;
    uint32_t       nbver;
} URMFILLHEIGTHMAP;


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
#define UTOSAVETRANSLATION ( 1 << 0 )
#define UTOSAVEROTATION    ( 1 << 1 )
#define UTOSAVESCALING     ( 1 << 2 )
#define UTOSAVEAXIS        ( 1 << 3 )

#define UTOSAVESTATEBEFORE ( 0x01 )
#define UTOSAVESTATEAFTER  ( 0x02 )

typedef struct _URMTRANSFORMOBJECT {
    G3DSCENE *sce;
    LIST      *lobj;
    G3DVECTOR3F *oldpos;
    G3DVECTOR3F *oldrot;
    G3DVECTOR3F *oldsca;
    G3DVECTOR3F *newpos;
    G3DVECTOR3F *newrot;
    G3DVECTOR3F *newsca;
    uint32_t   restoreAxis;
} URMTRANSFORMOBJECT;

/******************************************************************************/
#define UMPSAVESELECTEDPOINTS ( 1 << 0 )
#define UMPSAVECURRENTHANDLE  ( 1 << 1 )

#define UMPSAVESTATEBEFORE ( 0x01 )
#define UMPSAVESTATEAFTER  ( 0x02 )

/******************************************************************************/
typedef struct _URMSELECTITEM {
    G3DSCENE *sce;
    G3DOBJECT *obj;
    LIST *loldobjsel;
    LIST *loldversel;
    LIST *loldfacsel;
    LIST *loldedgsel;
    LIST *lnewobjsel;
    LIST *lnewversel;
    LIST *lnewfacsel;
    LIST *lnewedgsel;
    uint64_t engine_flags;
} URMSELECTITEM;

/******************************************************************************/
typedef struct _URMCREATEFACEGROUP {
    G3DSCENE *sce;
    G3DMESH      *mes;
    G3DFACEGROUP *facgrp;
} URMCREATEFACEGROUP;

/******************************************************************************/
typedef struct _URMOBJECTPOSE {
    G3DSCENE *sce;
    G3DOBJECT *obj;
    float      frame;
    G3DKEY    *key;
    G3DKEY    *overwrittenKey;
    G3DVECTOR3F  keypos;
    G3DVECTOR3F  keyrot;
    G3DVECTOR3F  keysca;
    LIST      *lremovedPosSegments;
    LIST      *lremovedRotSegments;
    LIST      *lremovedScaSegments;
    LIST      *laddedPosSegments;
    LIST      *laddedRotSegments;
    LIST      *laddedScaSegments;
} URMOBJECTPOSE;

/******************************************************************************/
typedef struct _URMOBJECTREMOVEKEYS {
    G3DSCENE *sce;
    G3DOBJECT *obj;
    LIST      *lkey;
    LIST      *laddedPosSegments;
    LIST      *laddedRotSegments;
    LIST      *laddedScaSegments;
    LIST      *lremovedPosSegments;
    LIST      *lremovedRotSegments;
    LIST      *lremovedScaSegments;
} URMOBJECTREMOVEKEYS;

/******************************************************************************/
typedef struct _URMREMOVETEXTURE {
    G3DSCENE    *sce;
    G3DOBJECT   *obj;
    G3DTEXTURE  *tex;
    LIST        *lfacgrp;
} URMREMOVETEXTURE;

/******************************************************************************/
typedef struct _URMSCULPTFACE {
    G3DSCENE      *sce;
    G3DSUBDIVIDER *sdr;
    LIST          *lusfe;
} URMSCULPTFACE;

/******************************************************************************/
typedef struct _URMSCULPTFACEEXTENSION {
    G3DFACESCULPTEXTENSION *fse;
    G3DFACE                *fac;
    G3DVECTOR4F            *pos;
    G3DHEIGHT              *hei;
    G3DVECTOR3F            *nor;
} URMSCULPTFACEEXTENSION;

/******************************************************************************/
typedef struct _G3DURMANAGER {
    G3DSCENE *sce;
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
    void (*undo) ( G3DURMANAGER *, void *, uint64_t ); /*** called if g3durmanager_undo is called  ***/
    void (*redo) ( G3DURMANAGER *, void *, uint64_t ); /*** called if g3durmanager_redo is called  ***/
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
uint32_t g3durmanager_undo ( G3DURMANAGER *urm, 
                             uint64_t      engine_flags );
uint32_t g3durmanager_redo ( G3DURMANAGER *urm, 
                             uint64_t      engine_flags );
void g3durmanager_push ( G3DURMANAGER *urm, 
                         void   (*undofunc) ( G3DURMANAGER *urm, 
                                              void         *data,
                                              uint64_t      engine_flags ),
                         void   (*redofunc) ( G3DURMANAGER *urm, 
                                              void         *data, 
                                              uint64_t      engine_flags ),
                         void   (*freefunc) ( void *, uint32_t ),
                         void    *data, 
                         uint32_t return_flags );
void g3duritem_free ( G3DURITEM * );
void g3durmanager_clearFrom ( G3DURMANAGER *, LIST ** );
void g3durmanager_clear     ( G3DURMANAGER * );
void g3durmanager_free ( G3DURMANAGER *urm );

/******************************************************************************/
G3DURITEM *g3duritem_new ( void (*undofunc) ( G3DURMANAGER *urm, 
                                              void         *data,
                                              uint64_t      engine_flags ),
                           void (*redofunc) ( G3DURMANAGER *urm, 
                                              void         *data,
                                              uint64_t      engine_flags ),
                           void (*freefunc) ( void    *data, 
                                              uint32_t commit ),
                           void *,                            /* user data    */
                           uint32_t );                        /* return flags */

/******************************************************************************/
void g3durm_object_addChild ( G3DURMANAGER *urm, 
                              G3DSCENE     *sce,
                              uint64_t      engine_flags,
                              uint32_t      return_flags,
                              G3DOBJECT    *oldpar,
                              G3DOBJECT    *newpar,
                              G3DOBJECT    *obj );
void g3durm_object_addChildren ( G3DURMANAGER *urm, 
                                 G3DSCENE  *sce,
                                 uint64_t   engine_flags,
                                 uint32_t   return_flags,
                                 G3DOBJECT *oldpar,
                                 G3DOBJECT *newpar,
                                 LIST      *lobj );

/******************************************************************************/
void g3durm_scene_deleteSelectedObjects ( G3DURMANAGER *urm,
                                          G3DSCENE     *sce, 
                                          uint64_t      engine_flags,
                                          uint32_t      return_flags );
void g3durm_mesh_deleteGeometry ( G3DURMANAGER *urm,
                                  G3DSCENE     *sce, 
                                  G3DMESH      *mes,
                                  uint64_t      engine_flags,
                                  uint32_t      return_flags );

/******************************************************************************/
URMCONVERTPRIMITIVE *urmconvertprimitive_new ( G3DPRIMITIVE *, 
                                               G3DMESH *, G3DOBJECT *, 
                                                               G3DSCENE  * );
void urmconvertprimitive_free ( URMCONVERTPRIMITIVE * );
void convertPrimitive_undo ( G3DURMANAGER *, void *, uint64_t );
void convertPrimitive_redo ( G3DURMANAGER *, void *, uint64_t );
void convertPrimitive_free (  void *, uint32_t );
void g3durm_primitive_convert ( G3DURMANAGER *urm, 
                                G3DSCENE     *sce,
                                uint64_t      engine_flags,
                                G3DPRIMITIVE *pri,
                                G3DOBJECT    *par,
                                uint32_t      return_flags );

/******************************************************************************/
void g3durm_mesh_weldSelectedVertices ( G3DURMANAGER *urm, 
                                        G3DSCENE     *sce, 
                                        G3DMESH      *mes, 
                                        uint32_t      type,
                                        uint64_t      engine_flags,
                                        uint32_t      return_flags );
void g3durm_mesh_weldNeighbourVertices ( G3DURMANAGER *urm,
                                         G3DSCENE     *sce, 
                                         G3DMESH      *mes, 
                                         uint32_t      type,
                                         float         distance,
                                         uint64_t      engine_flags,
                                         uint32_t      return_flags );

/******************************************************************************/
void g3durm_mesh_moveVertexList ( G3DURMANAGER *urm,
                                  G3DSCENE     *sce, 
                                  G3DMESH      *mes, 
                                  LIST         *lver,
                                  LIST         *ledg,
                                  LIST         *lfac,
                                  LIST         *lsub,
                                  G3DVECTOR3F  *oldpos,
                                  G3DVECTOR3F  *newpos,
                                  uint32_t return_flags );

/******************************************************************************/
void g3durm_mesh_cut ( G3DURMANAGER *urm, 
                       G3DSCENE     *sce,
                       G3DMESH      *mes,
                       G3DFACE      *knife,
                       int           restricted,
                       uint64_t      engine_flags,
                       uint32_t      return_flags );

/******************************************************************************/
void g3durm_mesh_extrude ( G3DURMANAGER *urm, 
                           G3DSCENE     *sce,
                           G3DMESH      *mes,
                           LIST         *loriver,
                           LIST         *loldfac,
                           LIST         *lnewver,
                           LIST         *lnewfac,
                           G3DVECTOR3F    *oldpos,
                           G3DVECTOR3F    *newpos,
                           uint32_t      return_flags );

/******************************************************************************/
void urmtransform_saveState ( URMTRANSFORMOBJECT *uto, uint32_t save_time );
URMTRANSFORMOBJECT *g3durm_object_transform ( G3DURMANAGER *urm,
                                              G3DSCENE     *sce,
                                              LIST         *lobj,
                                              uint32_t      return_flags );
URMTRANSFORMOBJECT *g3durm_uvmap_transform ( G3DURMANAGER *urm,
                                             G3DSCENE     *sce,
                                             G3DUVMAP     *uvmap,
                                             uint32_t      return_flags );
URMTRANSFORMOBJECT *g3durm_axis_transform ( G3DURMANAGER *urm,
                                            G3DSCENE     *sce,
                                            LIST         *lobj,
                                            uint32_t      return_flags );

/******************************************************************************/
void g3durm_mesh_untriangulate ( G3DURMANAGER *urm,
                               G3DSCENE     *sce,
                                 G3DMESH      *mes,
                                 uint64_t      engine_flags,
                                 uint32_t      return_flags );
void g3durm_mesh_triangulate ( G3DURMANAGER *urm,
                               G3DSCENE     *sce,
                               G3DMESH      *mes,
                               int           clockwise,
                               uint64_t      engine_flags,
                               uint32_t      return_flags );

/******************************************************************************/
void g3durm_mesh_createFace ( G3DURMANAGER *urm,
                              G3DSCENE     *sce, 
                              G3DMESH      *mes, 
                              G3DFACE      *fac, 
                              uint32_t      return_flags );

/******************************************************************************/
void g3durm_uvmap_pickUVSets ( G3DURMANAGER *urm, 
                               G3DSCENE     *sce,
                               G3DUVMAP     *uvmap,
                               LIST         *loldseluvset,
                               LIST         *lnewseluvset,
                               uint64_t      engine_flags,
                               uint32_t      return_flags );
void g3durm_uvmap_pickUVs ( G3DURMANAGER *urm,
                            G3DSCENE     *sce,
                            G3DUVMAP     *uvmap,
                            LIST         *loldseluv,
                            LIST         *lnewseluv,
                            uint64_t      engine_flags,
                            uint32_t      return_flags );
void g3durm_mesh_pickVertices ( G3DURMANAGER *urm, 
                                G3DSCENE     *sce,
                                G3DMESH      *mes,
                                LIST         *loldversel,
                                LIST         *lnewversel,
                                uint64_t      engine_flags,
                                uint32_t      return_flags );
void g3durm_mesh_pickEdges ( G3DURMANAGER *urm, 
                                G3DSCENE     *sce,
                                G3DMESH      *mes,
                                LIST         *loldversel,
                                LIST         *lnewversel,
                                uint64_t      engine_flags,
                                uint32_t      return_flags );
void g3durm_mesh_pickFaces ( G3DURMANAGER *urm, 
                             G3DSCENE     *sce,
                             G3DMESH      *mes,
                             LIST         *loldversel,
                             LIST         *lnewversel,
                             uint64_t      engine_flags,
                             uint32_t      return_flags );
void g3durm_scene_pickObject ( G3DURMANAGER *urm, 
                               G3DSCENE     *sce,
                               LIST         *loldobjsel,
                               LIST         *lnewobjsel,
                               uint64_t      engine_flags,
                               uint32_t      return_flags );
void g3durm_spline_pickPoints ( G3DURMANAGER *urm,
                                G3DSCENE     *sce,
                                G3DSPLINE    *spl,
                                LIST         *loldselpt,
                                LIST         *lnewselpt,
                                uint64_t      engine_flags,
                                uint32_t      return_flags );


/******************************************************************************/
void g3durm_mesh_addVertex ( G3DURMANAGER *urm,
                             G3DSCENE     *sce, 
                             G3DMESH      *mes, 
                             G3DVERTEX    *ver, 
                             uint32_t      return_flags );

/******************************************************************************/
void g3durm_mesh_invertNormal ( G3DURMANAGER *urm,
                                G3DSCENE     *sce,
                                G3DMESH      *mes,
                                uint64_t      engine_flags,
                                uint32_t      return_flags );

/******************************************************************************/
void g3durm_spline_deletePoints ( G3DURMANAGER *urm,
                                  G3DSCENE     *sce,
                                  G3DSPLINE    *spline, 
                                  LIST         *lremovedPoints,
                                  uint64_t      engine_flags,
                                  uint32_t      return_flags );

/******************************************************************************/
void g3durm_spline_cut ( G3DURMANAGER *urm,
                         G3DSCENE     *sce, 
                         G3DSPLINE    *spline, 
                         G3DFACE      *knife,
                         uint64_t      engine_flags,
                         uint32_t      return_flags );

/******************************************************************************/
void g3durm_spline_addPoint ( G3DURMANAGER     *urm,
                              G3DSCENE         *sce, 
                              G3DSPLINE        *spline, 
                              G3DSPLINEPOINT   *pt,
                              G3DSPLINESEGMENT *seg,
                              uint64_t engine_flags,
                              uint32_t          return_flags );

/******************************************************************************/
void g3durm_spline_addSegment ( G3DURMANAGER     *urm,
                                G3DSCENE         *sce,
                                G3DSPLINE        *spline,
                                G3DSPLINESEGMENT *seg,
                                uint64_t engine_flags,
                                uint32_t          return_flags );

/******************************************************************************/
void g3durm_spline_roundSelectedPoints ( G3DURMANAGER     *urm,
                                         G3DSCENE         *sce,
                                         G3DSPLINE        *spline,
                                         uint64_t engine_flags,
                                         uint32_t          return_flags );

/******************************************************************************/
void g3durm_mesh_split ( G3DURMANAGER *urm, 
                         G3DSCENE     *sce,
                         G3DMESH      *mes,
                         uint32_t      splID,
                         uint32_t      keep,
                         uint64_t      engine_flags,
                         uint32_t      return_flags );

/******************************************************************************/
void g3durm_spline_revert ( G3DURMANAGER *urm, 
                            G3DSCENE     *sce,
                            G3DSPLINE    *spline,
                            uint64_t      engine_flags,
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
                       uint64_t engine_flags );
void objectPose_redo ( G3DURMANAGER *urm, 
                       void         *data,
                       uint64_t engine_flags );
void g3durm_object_pose ( G3DURMANAGER *urm,
                          LIST         *lobj,
                          float         frame,
                          uint32_t      key_flags,
                          uint64_t engine_flags,
                          uint32_t      return_flags );

/******************************************************************************/
URMOBJECTREMOVEKEYS *urmObjectRemoveKeys_new ( G3DOBJECT *obj );
void urmObjectRemoveKeys_free ( URMOBJECTREMOVEKEYS *ork );
void objectRemoveKeys_free ( void *data, uint32_t commit );
void objectRemoveKeys_undo ( G3DURMANAGER *urm, 
                             void         *data,
                             uint64_t engine_flags );
void objectRemoveKeys_redo ( G3DURMANAGER *urm, 
                             void         *data,
                             uint64_t engine_flags );
void g3durm_objectList_removeSelectedKeys ( G3DURMANAGER *urm,
                                            LIST         *lobj,
                                            uint64_t engine_flags,
                                            uint32_t      return_flags );

/******************************************************************************/
void g3durm_spline_movePoint ( G3DURMANAGER *urm,
                               G3DSCENE     *sce,
                               G3DSPLINE    *spl,
                               uint32_t      save_type,
                               uint32_t      return_flags );

/******************************************************************************/
void g3durm_morpher_moveVertices ( G3DURMANAGER       *urm,
                                   G3DMORPHER         *mpr,
                                   G3DMORPHERMESHPOSE *mpose,
                                   LIST               *lver,
                                   G3DVECTOR3F          *oldpos,
                                   G3DVECTOR3F          *newpos,
                                   uint32_t            return_flags );

/******************************************************************************/
void g3durm_uvmap_moveUVList ( G3DURMANAGER *urm,
                               G3DSCENE     *sce,
                               G3DUVMAP     *uvmap, 
                               LIST         *luv,
                               G3DUV        *olduv,
                               G3DUV        *newuv, 
                               uint32_t      return_flags );

/******************************************************************************/
void g3durm_mesh_createFacegroup ( G3DURMANAGER *urm,
                                   G3DMESH      *mes,
                                   const char   *name,
                                   uint64_t      engine_flags,
                                   uint32_t      return_flags );

/******************************************************************************/
void g3durm_mesh_removeTexture ( G3DURMANAGER *urm,
                                 G3DSCENE     *sce,
                                 G3DMESH      *mes,
                                 G3DTEXTURE   *tex, 
                                 uint64_t      engine_flags,
                                 uint32_t      return_flags );
void urmremovetexture_free ( URMREMOVETEXTURE *urt );

/******************************************************************************/
void g3durm_mesh_addTexture ( G3DURMANAGER *urm,
                              LIST         *lobj,
                              G3DTEXTURE   *tex,
                              uint64_t      engine_flags,
                              uint32_t      return_flags );

/******************************************************************************/
void g3durm_scene_addMaterial ( G3DURMANAGER *urm,
                                G3DSCENE     *sce,
                                G3DMATERIAL  *mat,
                                uint64_t      engine_flags,
                                uint32_t      return_flags );

/******************************************************************************/
void g3durm_scene_removeMaterial ( G3DURMANAGER *urm,
                                   G3DSCENE     *sce,
                                   G3DMATERIAL  *mat,
                                   uint64_t      engine_flags,
                                   uint32_t      return_flags );

/******************************************************************************/
void g3durm_mesh_removeUVMap ( G3DURMANAGER *urm,
                               G3DSCENE     *sce,
                               G3DMESH      *mes,
                               G3DUVMAP     *uvmap, 
                               uint64_t      engine_flags,
                               uint32_t      return_flags );

/******************************************************************************/
void g3durm_mesh_addUVMap ( G3DURMANAGER *urm,
                            G3DSCENE     *sce,
                            G3DMESH      *mes,
                            G3DUVMAP     *uvmap, 
                            uint64_t      engine_flags,
                            uint32_t      return_flags );

/******************************************************************************/
void g3durm_image_paint ( G3DURMANAGER *urm, 
                          G3DIMAGE     *img,
                          G3DIMAGE     *bufimg,
                          int32_t       x1,
                          int32_t       y1,
                          int32_t       x2,
                          int32_t       y2,
                          uint32_t      return_flags );

/******************************************************************************/
void g3durm_morpher_createMeshPose ( G3DURMANAGER *urm,
                                     G3DSCENE     *sce,
                                     G3DMORPHER   *mpr,
                                     uint32_t      engine_flags,
                                     uint32_t      return_flags );

/******************************************************************************/
void g3durm_morpher_removeMeshPose ( G3DURMANAGER       *urm,
                                     G3DSCENE           *sce,
                                     G3DMORPHER         *mpr,
                                     G3DMORPHERMESHPOSE *mpose,
                                     uint32_t            engine_flags,
                                     uint32_t            return_flags );

/******************************************************************************/
void g3durm_morpher_selectVertexPose ( G3DURMANAGER       *urm,
                                       G3DSCENE           *sce,
                                       G3DMORPHER         *mpr,
                                       G3DMORPHERMESHPOSE *mpose,
                                       LIST               *lmprver,
                                       LIST               *lselold,
                                       LIST               *lselnew,
                                       uint32_t            return_flags );

/******************************************************************************/
void g3durm_morpher_moveVertexPose ( G3DURMANAGER       *urm,
                                     G3DSCENE           *sce,
                                     G3DMORPHER         *mpr,
                                     G3DMORPHERMESHPOSE *mpose,
                                     LIST               *lver,
                                     G3DVECTOR3F          *oldpos,
                                     G3DVECTOR3F          *newpos,
                                     uint32_t            return_flags );

/******************************************************************************/
void g3durm_morpher_selectMeshPose ( G3DURMANAGER       *urm,
                                     G3DSCENE           *sce,
                                     G3DMORPHER         *mpr,
                                     G3DMORPHERMESHPOSE *newmpose,
                                     uint32_t            engine_flags,
                                     uint32_t            return_flags );

/******************************************************************************/
void g3durm_morpher_removeVertexPose ( G3DURMANAGER       *urm,
                                       G3DSCENE           *sce,
                                       G3DMORPHER         *mpr,
                                       G3DMORPHERMESHPOSE *mpose,
                                       uint32_t            engine_flags,
                                       uint32_t            return_flags );

/******************************************************************************/
void g3durm_selection_addTag ( G3DURMANAGER *urm,
                               G3DSCENE     *sce,
                               G3DOBJECT    *obj,
                               G3DTAG       *tag,
                               uint64_t      engine_flags,
                               uint32_t      return_flags );

/******************************************************************************/
void g3durm_selection_removeTag ( G3DURMANAGER *urm,
                                  G3DSCENE     *sce,
                                  G3DOBJECT    *obj,
                                  G3DTAG       *tag,
                                  uint64_t      engine_flags,
                                  uint32_t      return_flags );

/******************************************************************************/
void g3durm_objectList_driftSelectedKeys ( G3DURMANAGER *urm,
                                           LIST         *lobj,
                                           float         drift,
                                           uint64_t      engine_flags,
                                           uint32_t      return_flags );

/******************************************************************************/
void g3durm_selection_addTexture ( G3DURMANAGER *urm,
                                   G3DSCENE     *sce,
                                   LIST         *lobj,
                                   LIST         *lmat,
                                   uint64_t      engine_flags,
                                   uint32_t      return_flags );

/******************************************************************************/
void g3durm_objectList_scaleSelectedKeys ( G3DURMANAGER *urm,
                                           LIST         *lobj,
                                           float         factor, 
                                           float         reference,
                                           uint64_t      engine_flags,
                                           uint32_t      return_flags );

/******************************************************************************/
URMSCULPTFACE *g3durm_mesh_sculptFace ( G3DURMANAGER  *urm,
                                        G3DSCENE      *sce,
                                        G3DSUBDIVIDER *sdr,
                                        uint64_t       engine_flags,
                                        uint32_t       return_flags );
URMSCULPTFACEEXTENSION *urmsculptfaceextension_new ( G3DFACESCULPTEXTENSION *fse,
                                                     G3DFACE                *fac );
URMSCULPTFACEEXTENSION *urmsculptfaceextension_seek ( LIST                   *lusf,
                                                      G3DFACESCULPTEXTENSION *fse );

#endif
