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
#ifndef _G3DIMPORTV3_H_
#define _G3DIMPORTV3_H_

/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/******************************************************************************/
#include <math.h>
#include <libgen.h>

/******************************************************************************/
#ifdef __linux__
#include <endian.h>
#endif

/******************************************************************************/
#include <list.h>
#include <g3dengine/g3dengine.h>
#include <g3dexportv3.h>

#define IMPORTV3_CALLBACK(f) ((void(*)(G3DIMPORTV3DATA*,uint32_t,FILE *,void*))f)


/*#define PRINT_CHUNK_INFO(sig,size,level)*/

#define PRINT_CHUNK_INFO(sig,size,level) \
        fprintf( stderr, "%*s SIG:%08X LEN:%d - %s\n", level, "|-",\
                                                       sig,\
                                                       size, __func__ );

/******************************************************************************/
typedef struct _G3DIMPORTV3DATA {
    G3DSCENE       *currentScene;
    G3DOBJECT      *parentObject;
    G3DOBJECT      *currentObject;
    G3DMATERIAL    *currentMaterial;
    G3DTEXTURE     *currentTexture;
    G3DCHANNEL     *currentChannel;
    G3DUVMAP       *currentUVMap;
    G3DRIG         *currentRig;
    G3DWEIGHTGROUP *currentWeightgroup;
    G3DFACEGROUP   *currentFacegroup;
    G3DVERTEX     **currentVertexArray;
    G3DEDGE       **currentEdgeArray;
    G3DFACE       **currentFaceArray;
    G3DMATERIAL   **currentMaterialArray;
    G3DCURVEPOINT **currentPointArray;
    uint32_t        currentObjectID;
    char            currentObjectName[0x100];
    uint32_t        currentMaterialID;
    uint32_t        currentWeightgroupID;
    uint32_t        currentFacegroupID;
    uint32_t        currentUVMapID;
    uint64_t        engineFlags;
    uint32_t        indentLevel;
    LIST           *lext;
    LIST           *ldec; /*** list of declared objects ***/
    G3DOBJECT     **declaredObjects; /*** array of declared objects ***/
} G3DIMPORTV3DATA;

/******************************************************************************/
typedef struct _G3DIMPORTV3EXTENSION {
    uint32_t signature;
    void (*read)( G3DIMPORTV3DATA *gid,
                  uint32_t       chunkEnd,
                  FILE          *fsrc,
                  void          *data );
    void *data;
} G3DIMPORTV3EXTENSION;

/******************************************************************************/
G3DSCENE *g3dscene_importv3 ( const char *filename,
                              G3DSCENE   *mergedScene,
                              LIST       *lextension,
                              uint32_t    flags );

uint32_t g3dimportv3_freadd ( double *d, FILE *stream );
uint32_t g3dimportv3_freadll ( uint64_t *ll, FILE *stream );
uint32_t g3dimportv3_freadl ( uint32_t *l, FILE *stream );
uint32_t g3dimportv3_freadf ( float *f, FILE *stream );
uint32_t g3dimportv3_fread ( void   *ptr,
                           size_t  size,
                           size_t  count,
                           FILE   *stream );

void g3dimportv3scene     ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3object    ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3primitive ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3material ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3channel ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3uvmap ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3camera ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3light ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3bone ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3symmetry ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3text ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3subdivider ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3wireframe ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3spline ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3ffd ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3mesh ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3splinerevolver ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3data_incrementIndentLevel ( G3DIMPORTV3DATA *gid );
void g3dimportv3data_decrementIndentLevel ( G3DIMPORTV3DATA *gid );
void g3dimportv3morpher ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3key ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3skin ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3tag ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3instance ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3path ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv3particleemitter ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );

G3DIMPORTV3EXTENSION *g3dimportv3extension_new ( uint32_t signature,
                                             void (*read)( G3DIMPORTV3DATA *gid,
                                                           uint32_t       chunkEnd,
                                                           FILE          *fsrc,
                                                           void          *data ),
                                             void      *data );
void g3dimportv3extension_free ( G3DIMPORTV3EXTENSION *ext );
G3DIMPORTV3EXTENSION *g3dimportv3extension_getFromList ( uint32_t signature,
                                                     LIST    *lext );
void g3dimportv3extension ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc );

char *readEntry ( FILE *fsrc );

/******************************************************************************/
G3DSCENE *g3dscene_open ( const char *filename,
                          G3DSCENE   *mergedScene,
                          LIST       *lextensions,
                          uint32_t    flags );

/*****************************< 3DStudio .3ds FILES >**************************/
G3DSCENE *g3dscene_import3ds ( const char *filename, 
                               uint64_t    engine_flags );

/*****************************< Wavefront .obj FILES >*************************/
G3DSCENE *g3dscene_importObj( const char *filename, 
                              uint64_t    engine_flags );

/************************* Collada .DAE FILES *********************************/
G3DSCENE *g3dscene_importCollada ( const char *filename, 
                                   uint64_t    engine_flags );

#endif
