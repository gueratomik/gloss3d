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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/

/**
 * @file
 */

/******************************************************************************/
#ifndef _G3VERTEX_H_
#define _G3VERTEX_H_

#include <stdint.h>

#include "g3dvector.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
typedef struct _SHADERVERTEX   SHADERVERTEX;
typedef struct _LIST           LIST;
typedef struct _G3DOBJECT      G3DOBJECT;
typedef struct _G3DINNERVERTEX G3DSUBVERTEX;
typedef struct _G3DWEIGHTGROUP G3DWEIGHTGROUP;
typedef struct _G3DWEIGHT      G3DWEIGHT;
typedef struct _G3DFACE        G3DFACE;
typedef struct _G3DEDGE        G3DEDGE;
typedef struct _G3DUVMAP       G3DUVMAP;

/******************************************************************************/
typedef struct _G3DVERTEX {
    G3DOBJECT    *owner;
    uint32_t      flags;
    uint32_t      id;              /* Vertex ID - Should never be trusted !   */
    uint32_t      geoID;           /* geomtry ID, all types included          */
    G3DVECTOR3F   pos;             /* Vertex position                         */
    G3DVECTOR3F   nor;             /* Vertex normal vector                    */
    LIST         *faceList;        /* list of faces connected to this vertex  */
    LIST         *edgeList;        /* list of connected edges                 */
    LIST         *weightList;      /* List of weight                          */
    uint32_t      faceCount;       /* number of connected faces               */
    uint32_t      edgeCount;       /* number of connected edges               */
    uint32_t      weightCount;     /* number of weights                       */
    float         weight;          /* weight value used when editing groups   */
    SHADERVERTEX *rtvermem;        /* Vertex buffer in buffered mode          */
    LIST         *extensionList;   /* list of vertex extensions               */
    struct _G3DSUBVERTEX *subver;
} G3DVERTEX;

/******************************************************************************/
typedef struct _G3DVERTEXEXTENSION {
    uint64_t name;
} G3DVERTEXEXTENSION;

/******************************************************************************/
typedef struct _G3DSPLITVERTEX {
    G3DVERTEX *oriver;
    G3DVERTEX *newver;
} G3DSPLITVERTEX;

/******************************************************************************/
void                g3dvertex_addExtension ( G3DVERTEX          *ver,
                                             G3DVERTEXEXTENSION *ext );
void                g3dvertex_addEdge ( G3DVERTEX *ver, 
                                        G3DEDGE   *edg );
void                g3dvertex_addFace ( G3DVERTEX *ver,
                                        G3DFACE   *fac );
void                g3dvertex_addWeight ( G3DVERTEX *ver,
                                          G3DWEIGHT *wei );
uint32_t            g3dvertex_belongsToSelectedFacesOnly ( G3DVERTEX *ver );
void                g3dvertex_computeNormal ( G3DVERTEX   *ver, 
                                              G3DVECTOR3F *nor,
                                              uint64_t     engine_flags );
G3DVERTEX          *g3dvertex_copy ( G3DVERTEX *ver, 
                                     uint64_t   engine_flags );
void                g3dvertex_copyPosition ( G3DVERTEX *dst,
                                             G3DVERTEX *src );
uint32_t            g3dvertex_copyPositionFromList ( LIST         *vertexList, 
                                                     G3DVECTOR3F **vec );
void                g3dvertex_displace ( G3DVERTEX    *ver,
                                         LIST        *textureList,
                                         G3DVECTOR3F *pos );
void                g3dvertex_free ( G3DVERTEX *ver );
LIST               *g3dvertex_getAreaFacesFromList ( LIST *vertexList );
uint32_t            g3dvertex_getAverageFacePoint ( G3DVERTEX   *ver, 
                                                    G3DVECTOR3F *facavg );
uint32_t            g3dvertex_getAverageMidPoint ( G3DVERTEX   *ver,
                                                   G3DVECTOR3F *midavg );
uint32_t            g3dvertex_getAverageModifiedFacePoint ( G3DVERTEX   *ver, 
                                                            G3DVECTOR3F *verpos,
                                                            G3DVECTOR3F *facavg );
uint32_t            g3dvertex_getAverageModifiedMidPoint ( G3DVERTEX   *ver, 
                                                           G3DVECTOR3F *verpos,
                                                           G3DVECTOR3F *midavg );
void                g3dvertex_getAveragePositionFromList ( LIST        *vertexList,
                                                           G3DVECTOR3F *pos );
void                g3dvertex_getAverageUV ( G3DVERTEX *ver,
                                             G3DUVMAP  *map,
                                             float     *u,
                                             float     *v );
LIST               *g3dvertex_getEdgesFromList ( LIST *vertexList );
G3DVERTEXEXTENSION *g3dvertex_getExtension ( G3DVERTEX *ver,
                                             uint32_t   name );
LIST               *g3dvertex_getFacesFromList ( LIST *vertexList );
G3DVECTOR3F        *g3dvertex_getModifiedPosition ( G3DVERTEX   *ver,
                                                    G3DVECTOR3F *stkpos );
G3DVECTOR3F        *g3dvertex_getModifiedNormal ( G3DVERTEX   *ver,
                                                  G3DVECTOR3F *stknor );
void                g3dvertex_getNormalFromSelectedFaces ( G3DVERTEX   *ver,
                                                           G3DVECTOR3F *vout );
void                g3dvertex_getPositionFromEdges ( G3DVERTEX   *ver,
                                                     G3DVECTOR3F *pos );
int32_t             g3dvertex_getRankFromList ( LIST      *vertexList,
                                                G3DVERTEX *ver );
LIST               *g3dvertex_getUnselectedFacesFromList ( LIST *vertexList );
void                g3dvertex_getVectorFromSelectedFaces ( G3DVERTEX *ver,
                                                           G3DVECTOR3F *vout );
G3DWEIGHT          *g3dvertex_getWeight ( G3DVERTEX      *ver, 
                                          G3DWEIGHTGROUP *grp );
void                g3dvertex_init ( G3DVERTEX *ver,
                                     uint32_t   flags,
                                     float      x,
                                     float      y,
                                     float      z );
uint32_t            g3dvertex_isBorder ( G3DVERTEX *ver );
uint32_t            g3dvertex_isBoundary ( G3DVERTEX *ver );
G3DVERTEX          *g3dvertex_new ( float x,
                                    float y,
                                    float z );
void                g3dvertex_print ( G3DVERTEX *ver );
void                g3dvertex_removeEdge ( G3DVERTEX *ver,
                                           G3DEDGE   *edg );
void                g3dvertex_removeExtension ( G3DVERTEX          *ver,
                                                G3DVERTEXEXTENSION *ext );
void                g3dvertex_removeFace ( G3DVERTEX *ver,
                                           G3DFACE   *fac );
void                g3dvertex_removeWeight ( G3DVERTEX *ver,
                                             G3DWEIGHT *wei );
void                g3dvertex_renumberList ( LIST    *vertexList,
                                             uint32_t id );
G3DVERTEX          *g3dvertex_seekVertexByPosition ( LIST *vertexList,
                                                     float  x,
                                                     float  y,
                                                     float  z,
                                                     float  epsilon );
void                g3dvertex_setNormal ( G3DVERTEX *ver, 
                                          float      x,
                                          float      y,
                                          float      z );
void                g3dvertex_setPosition ( G3DVERTEX *ver, 
                                            float      x,
                                            float      y,
                                            float      z );
void                g3dvertex_setPositionFromList ( LIST        *vertexList,
                                                    G3DVECTOR3F *pos );
void                g3dvertex_setSelected ( G3DVERTEX *ver );
void                g3dvertex_unsetSelected ( G3DVERTEX *ver );
void                g3dvertex_updateFaces ( G3DVERTEX *ver );
void                g3dvertex_updateFacesPosition ( G3DVERTEX *ver );
void                g3dvertex_updateNormal ( G3DVERTEX *ver, 
                                             uint64_t   engine_flags );
G3DVERTEX          *g3dvertex_weldList ( LIST *vertexList );

/******************************************************************************/
void g3dvertexextension_init ( G3DVERTEXEXTENSION *ext,
                               uint64_t            name );

/******************************************************************************/
void            g3dsplitvertex_free ( G3DSPLITVERTEX *spv );
G3DSPLITVERTEX *g3dsplitvertex_new ( G3DVERTEX *ver );
G3DSPLITVERTEX *g3dsplitvertex_seek ( LIST *lis,
                                      G3DVERTEX *ver );

#ifdef __cplusplus
}
#endif

#endif
