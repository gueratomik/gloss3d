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
#ifndef _G3DIMPORT_H_
#define _G3DIMPORT_H_

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
#include <GL/gl.h>
#include <GL/glu.h>

/******************************************************************************/
#include <list.h>
#include <g3dengine/g3dengine.h>
#include <g3dexportv2.h>

#define PRINT_CHUNK_INFO(sig,size,level) \
        fprintf( stderr, "%*s SIG:%08X LEN:%d - %s\n", level, "|-",\
                                                       sig,\
                                                       size, __func__ );

/******************************************************************************/
typedef struct _G3DIMPORTDATA {
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
} G3DIMPORTDATA;

/******************************************************************************/
typedef struct _G3DIMPORTEXTENSION {
    uint32_t signature;
    void (*read)( G3DIMPORTDATA *gid,
                  uint32_t       chunkEnd,
                  FILE          *fsrc,
                  void          *data );
    void *data;
} G3DIMPORTEXTENSION;

/******************************************************************************/
G3DSCENE *g3dscene_importv2 ( const char *filename,
                              G3DSCENE   *mergedScene,
                              LIST       *lextension,
                              uint32_t    flags );

uint32_t g3dimport_freadd ( double *d, FILE *stream );
uint32_t g3dimport_freadll ( uint64_t *ll, FILE *stream );
uint32_t g3dimport_freadl ( uint32_t *l, FILE *stream );
uint32_t g3dimport_freadf ( float *f, FILE *stream );
uint32_t g3dimport_fread ( void   *ptr,
                           size_t  size,
                           size_t  count,
                           FILE   *stream );

void g3dimportscene     ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportobject    ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportprimitive ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportmaterial ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportchannel ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportuvmap ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportcamera ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportlight ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportbone ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportsymmetry ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimporttext ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportsubdivider ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportwireframe ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportspline ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportffd ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportmesh ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportsplinerevolver ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportdata_incrementIndentLevel ( G3DIMPORTDATA *gid );
void g3dimportdata_decrementIndentLevel ( G3DIMPORTDATA *gid );
void g3dimportmorpher ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc );
void g3dimportkey ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc );

G3DIMPORTEXTENSION *g3dimportextension_new ( uint32_t signature,
                                             void (*read)( G3DIMPORTDATA *gid,
                                                           uint32_t       chunkEnd,
                                                           FILE          *fsrc,
                                                           void          *data ),
                                             void      *data );
void g3dimportextension_free ( G3DIMPORTEXTENSION *ext );
G3DIMPORTEXTENSION *g3dimportextension_getFromList ( uint32_t signature,
                                                     LIST    *lext );
void g3dimportextension ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc );

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
