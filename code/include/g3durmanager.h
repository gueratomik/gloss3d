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
#include <g3d.h>

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
    G3DVERTEX *newver;
} URMWELDVERTICES;

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
typedef struct _URMCUTMESH {
    G3DMESH *mes;
    LIST *loldfac;
    LIST *lnewver;
    LIST *lnewfac;
} URMCUTMESH;

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
typedef struct _URMMOVEOBJECT {
    G3DOBJECT *obj;
    G3DVECTOR oldpos;
    G3DVECTOR oldrot;
    G3DVECTOR oldsca;
    G3DVECTOR newpos;
    G3DVECTOR newrot;
    G3DVECTOR newsca;
} URMMOVEOBJECT;

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
                                                          LIST *, G3DVERTEX * );
void urmweldvertices_free ( URMWELDVERTICES * );
void weldVertices_redo ( G3DURMANAGER *, void *, uint32_t );
void weldVertices_undo ( G3DURMANAGER *, void *, uint32_t );
void weldVertices_free ( void *, uint32_t );
void g3durm_mesh_weldSelectedVertices ( G3DURMANAGER *, G3DMESH *, uint32_t,
                                                                   uint32_t,
                                                                   uint32_t );

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
URMMOVEOBJECT *urmmoveobject_new ( G3DOBJECT *obj, G3DVECTOR *,
                                                   G3DVECTOR *,
                                                   G3DVECTOR *,
                                                   G3DVECTOR *,
                                                   G3DVECTOR *,
                                                   G3DVECTOR * );
void urmmoveobject_free ( URMMOVEOBJECT * );
void moveObject_undo ( G3DURMANAGER *, void *, uint32_t );
void moveObject_redo ( G3DURMANAGER *, void *, uint32_t );
void moveObject_free ( void *, uint32_t );
void g3durm_object_move ( G3DURMANAGER *, G3DOBJECT *, G3DVECTOR *,
                                                       G3DVECTOR *,
                                                       G3DVECTOR *, uint32_t );

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

#endif