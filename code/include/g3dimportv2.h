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
#ifndef _G3DIMPORTV2_H_
#define _G3DIMPORTV2_H_

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
#include <g3dexportv2.h>

#define PRINT_CHUNK_INFO(sig,size,level) \
        fprintf( stderr, "%*s SIG:%08X LEN:%d - %s\n", level, "|-",\
                                                       sig,\
                                                       size, __func__ );

/******************************************************************************/
typedef struct _G3DIMPORTV2DATA {
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
} G3DIMPORTV2DATA;

/******************************************************************************/
typedef struct _G3DIMPORTV2EXTENSION {
    uint32_t signature;
    void (*read)( G3DIMPORTV2DATA *gid,
                  uint32_t       chunkEnd,
                  FILE          *fsrc,
                  void          *data );
    void *data;
} G3DIMPORTV2EXTENSION;

/******************************************************************************/
G3DSCENE *g3dscene_importv2 ( const char *filename,
                              G3DSCENE   *mergedScene,
                              LIST       *lextension,
                              uint32_t    flags );

uint32_t g3dimportv2_freadd ( double *d, FILE *stream );
uint32_t g3dimportv2_freadll ( uint64_t *ll, FILE *stream );
uint32_t g3dimportv2_freadl ( uint32_t *l, FILE *stream );
uint32_t g3dimportv2_freadf ( float *f, FILE *stream );
uint32_t g3dimportv2_fread ( void   *ptr,
                           size_t  size,
                           size_t  count,
                           FILE   *stream );

void g3dimportv2scene     ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv2object    ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv2primitive ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv2material ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv2channel ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv2uvmap ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv2camera ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv2light ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv2bone ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv2symmetry ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv2text ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv2subdivider ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv2wireframe ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv2spline ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv2ffd ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv2mesh ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv2splinerevolver ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv2data_incrementIndentLevel ( G3DIMPORTV2DATA *gid );
void g3dimportv2data_decrementIndentLevel ( G3DIMPORTV2DATA *gid );
void g3dimportv2morpher ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv2key ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportv2skin ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc );

G3DIMPORTV2EXTENSION *g3dimportv2extension_new ( uint32_t signature,
                                             void (*read)( G3DIMPORTV2DATA *gid,
                                                           uint32_t       chunkEnd,
                                                           FILE          *fsrc,
                                                           void          *data ),
                                             void      *data );
void g3dimportv2extension_free ( G3DIMPORTV2EXTENSION *ext );
G3DIMPORTV2EXTENSION *g3dimportv2extension_getFromList ( uint32_t signature,
                                                     LIST    *lext );
void g3dimportv2extension ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc );

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
