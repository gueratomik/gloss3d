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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2020         */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/

#ifndef _G3D_EXPORTV3_H_
#define _G3D_EXPORTV3_H_

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

#include "signaturesv3.h"

/******************** Wavefront .OBJ Exporter Module **************************/
#define EXPORTOBJTRIANGULATE ( 1 << 1 )

#define EXPORTV3CHANNEL_SOLIDCOLOR ( 1 << 0 )
#define EXPORTV3CHANNEL_IMAGECOLOR ( 1 << 1 )
#define EXPORTV3CHANNEL_PROCEDURAL ( 1 << 2 )

#define EXPORTV3_CALLBACK(f) ((uint32_t(*)(G3DEXPORTV3DATA*,void*,uint32_t,FILE *))f)

typedef struct _G3DEXPORTV3DATA {
    G3DSCENE  *currentScene;
    G3DOBJECT *currentObject;
    G3DFACE   *currentFace;
    uint32_t   objectID;
    LIST     *lext;
} G3DEXPORTV3DATA;

/******************************************************************************/
typedef struct _G3DEXPORTV3EXTENSION {
    uint32_t signature;
    uint32_t (*write)( G3DEXPORTV3DATA *ged,
                       void            *data,
                       uint32_t         flags,
                       FILE            *fdst );
    void      *data;
} G3DEXPORTV3EXTENSION;

void g3dscene_exportv3 ( G3DSCENE *sce, 
                         char     *filename,
                         char     *comment,
                         LIST     *lextension, 
                         uint32_t  flags );

uint32_t g3dexportv3_writeChunk ( uint32_t   chunkSignature,
                                uint32_t (*writeChunk) ( G3DEXPORTV3DATA *ged,
                                                         void          *data,
                                                         uint32_t       flags,
                                                         FILE          *fdst ),
                                G3DEXPORTV3DATA *ged,
                                void          *data,
                                uint32_t       flags,
                                FILE          *fdst );
uint32_t g3dexportv3_fwrite ( void   *ptr,
                            size_t  size,
                            size_t  count,
                            FILE   *stream );

uint32_t g3dexportv3root ( G3DEXPORTV3DATA *ged, 
                         G3DSCENE      *sce, 
                         uint32_t       flags, 
                         FILE          *fdst );

uint32_t g3dexportv3object ( G3DEXPORTV3DATA *ged, 
                           G3DOBJECT     *obj,
                           uint32_t       flags, 
                           FILE          *fdst );
uint32_t g3dexportv3scene ( G3DEXPORTV3DATA *ged, 
                          G3DSCENE      *sce, 
                          uint32_t       flags, 
                          FILE          *fdst );
uint32_t g3dexportv3primitive ( G3DEXPORTV3DATA *ged, 
                              G3DOBJECT     *obj, 
                              uint32_t       flags, 
                              FILE          *fdst );
uint32_t g3dexportv3mesh ( G3DEXPORTV3DATA *ged, 
                         G3DMESH       *mes, 
                         uint32_t       flags, 
                         FILE          *fdst );
uint32_t g3dexportv3channel  ( G3DEXPORTV3DATA *ged, 
                             G3DCHANNEL    *cha,
                             uint32_t       flags, 
                             FILE          *fdst );
uint32_t g3dexportv3material  ( G3DEXPORTV3DATA *ged, 
                              G3DMATERIAL   *mat, 
                              uint32_t       flags, 
                              FILE          *fdst );
uint32_t g3dexportv3uvmap ( G3DEXPORTV3DATA  *ged, 
                          G3DUVMAP       *uvmap, 
                          uint32_t        flags, 
                          FILE           *fdst );
uint32_t g3dexportv3camera ( G3DEXPORTV3DATA *ged, 
                           G3DCAMERA     *cam, 
                           uint32_t       flags, 
                           FILE          *fdst );
uint32_t g3dexportv3light ( G3DEXPORTV3DATA *ged, 
                          G3DLIGHT     *lig, 
                          uint32_t      flags, 
                          FILE         *fdst );
uint32_t g3dexportv3bone ( G3DEXPORTV3DATA *ged, 
                         G3DBONE     *cam, 
                         uint32_t       flags, 
                         FILE          *fdst );
uint32_t g3dexportv3symmetry ( G3DEXPORTV3DATA *ged, 
                             G3DSYMMETRY   *sym, 
                             uint32_t       flags, 
                             FILE          *fdst );
uint32_t g3dexportv3text ( G3DEXPORTV3DATA *ged, 
                         G3DTEXT       *txt, 
                         uint32_t       flags, 
                         FILE          *fdst );
uint32_t g3dexportv3subdivider ( G3DEXPORTV3DATA *ged, 
                               G3DSUBDIVIDER   *sub, 
                               uint32_t       flags, 
                               FILE          *fdst );
uint32_t g3dexportv3wireframe ( G3DEXPORTV3DATA *ged, 
                              G3DWIREFRAME  *wfm, 
                              uint32_t       flags, 
                              FILE          *fdst );
uint32_t g3dexportv3splinerevolver ( G3DEXPORTV3DATA    *ged, 
                                   G3DSPLINEREVOLVER *srv, 
                                   uint32_t          flags, 
                                   FILE             *fdst );
uint32_t g3dexportv3spline ( G3DEXPORTV3DATA*ged, 
                           G3DSPLINE    *spl, 
                           uint32_t      flags, 
                           FILE         *fdst );
uint32_t g3dexportv3extension ( G3DEXPORTV3DATA       *ged, 
                              G3DEXPORTV3EXTENSION  *ext, 
                              uint32_t             flags, 
                              FILE                *fdst );

uint32_t g3dexportv3ffd ( G3DEXPORTV3DATA *ged, 
                        G3DFFD        *ffd, 
                        uint32_t       flags, 
                        FILE          *fdst );

uint32_t g3dexportv3morpher ( G3DEXPORTV3DATA *ged, 
                            G3DMORPHER    *mpr, 
                            uint32_t       flags, 
                            FILE          *fdst );

uint32_t g3dexportv3particleemitter ( G3DEXPORTV3DATA    *ged, 
                                      G3DPARTICLEEMITTER *pem, 
                                      uint32_t            flags, 
                                      FILE               *fdst );

uint32_t g3dexportv3skin ( G3DEXPORTV3DATA *ged, 
                         G3DSKIN       *skn, 
                         uint32_t       flags, 
                         FILE          *fdst );

uint32_t g3dexportv3key ( G3DEXPORTV3DATA *ged, 
                        G3DKEY        *key, 
                        uint32_t       flags, 
                        FILE          *fdst );

uint32_t g3dexportv3tag ( G3DEXPORTV3DATA *ged, 
                          G3DTAG          *tag, 
                          uint32_t         flags, 
                          FILE            *fdst );

uint32_t g3dexportv3instance ( G3DEXPORTV3DATA *ged, 
                               G3DINSTANCE     *ins, 
                               uint32_t         flags, 
                               FILE            *fdst );

uint32_t g3dexportv3_fwrited ( double *d, FILE *stream );
uint32_t g3dexportv3_fwritef ( float *f, FILE *stream );
uint32_t g3dexportv3_fwritel ( uint32_t *l, FILE *stream );
uint32_t g3dexportv3_fwritell ( uint64_t *ll, FILE *stream );

G3DEXPORTV3EXTENSION *g3dexportv3extension_new ( uint32_t signature,
                                             uint32_t (*write)( G3DEXPORTV3DATA *ged, 
                                                                void            *data, 
                                                                uint32_t         flags, 
                                                                FILE            *fdst ),
                                             void      *data );
void g3dexportv3extension_free ( G3DEXPORTV3EXTENSION *ext );

#endif
