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

#ifndef _G3D_EXPORTV2_H_
#define _G3D_EXPORTV2_H_

/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/******************************************************************************/
#include <string.h>
#include <math.h>

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>

/******************************************************************************/
#include <list.h>
#include <g3dengine/g3dengine.h>

#include "signaturesv2.h"

#define EXPORT_CALLBACK(f) ((uint32_t(*)(G3DEXPORTV2DATA*,void*,uint32_t,FILE *))f)

typedef struct _G3DEXPORTV2DATA {
    G3DSCENE  *currentScene;
    G3DOBJECT *currentObject;
    G3DFACE   *currentFace;
    uint32_t   objectID;
    LIST     *lext;
} G3DEXPORTV2DATA;

/******************************************************************************/
typedef struct _G3DEXPORTV2EXTENSION {
    uint32_t signature;
    uint32_t (*write)( G3DEXPORTV2DATA *ged,
                       void          *data,
                       uint32_t       flags,
                       FILE          *fdst );
    void      *data;
} G3DEXPORTV2EXTENSION;

void g3dscene_exportv2 ( G3DSCENE *sce, 
                         char     *filename,
                         char     *comment,
                         LIST     *lextension, 
                         uint32_t  flags );

uint32_t g3dexportv2_writeChunk ( uint32_t   chunkSignature,
                                uint32_t (*writeChunk) ( G3DEXPORTV2DATA *ged,
                                                         void          *data,
                                                         uint32_t       flags,
                                                         FILE          *fdst ),
                                G3DEXPORTV2DATA *ged,
                                void          *data,
                                uint32_t       flags,
                                FILE          *fdst );
uint32_t g3dexportv2_fwrite ( void   *ptr,
                            size_t  size,
                            size_t  count,
                            FILE   *stream );

uint32_t g3dexportv2root ( G3DEXPORTV2DATA *ged, 
                         G3DSCENE      *sce, 
                         uint32_t       flags, 
                         FILE          *fdst );

uint32_t g3dexportv2object ( G3DEXPORTV2DATA *ged, 
                           G3DOBJECT     *obj,
                           uint32_t       flags, 
                           FILE          *fdst );
uint32_t g3dexportv2scene ( G3DEXPORTV2DATA *ged, 
                          G3DSCENE      *sce, 
                          uint32_t       flags, 
                          FILE          *fdst );
uint32_t g3dexportv2primitive ( G3DEXPORTV2DATA *ged, 
                              G3DOBJECT     *obj, 
                              uint32_t       flags, 
                              FILE          *fdst );
uint32_t g3dexportv2mesh ( G3DEXPORTV2DATA *ged, 
                         G3DMESH       *mes, 
                         uint32_t       flags, 
                         FILE          *fdst );
uint32_t g3dexportv2channel  ( G3DEXPORTV2DATA *ged, 
                             G3DCHANNEL    *cha,
                             uint32_t       flags, 
                             FILE          *fdst );
uint32_t g3dexportv2material  ( G3DEXPORTV2DATA *ged, 
                              G3DMATERIAL   *mat, 
                              uint32_t       flags, 
                              FILE          *fdst );
uint32_t g3dexportv2uvmap ( G3DEXPORTV2DATA  *ged, 
                          G3DUVMAP       *uvmap, 
                          uint32_t        flags, 
                          FILE           *fdst );
uint32_t g3dexportv2camera ( G3DEXPORTV2DATA *ged, 
                           G3DCAMERA     *cam, 
                           uint32_t       flags, 
                           FILE          *fdst );
uint32_t g3dexportv2light ( G3DEXPORTV2DATA *ged, 
                          G3DLIGHT     *lig, 
                          uint32_t      flags, 
                          FILE         *fdst );
uint32_t g3dexportv2bone ( G3DEXPORTV2DATA *ged, 
                         G3DBONE     *cam, 
                         uint32_t       flags, 
                         FILE          *fdst );
uint32_t g3dexportv2symmetry ( G3DEXPORTV2DATA *ged, 
                             G3DSYMMETRY   *sym, 
                             uint32_t       flags, 
                             FILE          *fdst );
uint32_t g3dexportv2text ( G3DEXPORTV2DATA *ged, 
                         G3DTEXT       *txt, 
                         uint32_t       flags, 
                         FILE          *fdst );
uint32_t g3dexportv2subdivider ( G3DEXPORTV2DATA *ged, 
                               G3DSUBDIVIDER   *sub, 
                               uint32_t       flags, 
                               FILE          *fdst );
uint32_t g3dexportv2wireframe ( G3DEXPORTV2DATA *ged, 
                              G3DWIREFRAME  *wfm, 
                              uint32_t       flags, 
                              FILE          *fdst );
uint32_t g3dexportv2splinerevolver ( G3DEXPORTV2DATA    *ged, 
                                   G3DSPLINEREVOLVER *srv, 
                                   uint32_t          flags, 
                                   FILE             *fdst );
uint32_t g3dexportv2spline ( G3DEXPORTV2DATA*ged, 
                           G3DSPLINE    *spl, 
                           uint32_t      flags, 
                           FILE         *fdst );
uint32_t g3dexportv2extension ( G3DEXPORTV2DATA       *ged, 
                              G3DEXPORTV2EXTENSION  *ext, 
                              uint32_t             flags, 
                              FILE                *fdst );

uint32_t g3dexportv2ffd ( G3DEXPORTV2DATA *ged, 
                        G3DFFD        *ffd, 
                        uint32_t       flags, 
                        FILE          *fdst );

uint32_t g3dexportv2morpher ( G3DEXPORTV2DATA *ged, 
                            G3DMORPHER    *mpr, 
                            uint32_t       flags, 
                            FILE          *fdst );

uint32_t g3dexportv2skin ( G3DEXPORTV2DATA *ged, 
                         G3DSKIN       *skn, 
                         uint32_t       flags, 
                         FILE          *fdst );

uint32_t g3dexportv2key ( G3DEXPORTV2DATA *ged, 
                        G3DKEY        *key, 
                        uint32_t       flags, 
                        FILE          *fdst );

uint32_t g3dexportv2_fwrited ( double *d, FILE *stream );
uint32_t g3dexportv2_fwritef ( float *f, FILE *stream );
uint32_t g3dexportv2_fwritel ( uint32_t *l, FILE *stream );
uint32_t g3dexportv2_fwritell ( uint64_t *ll, FILE *stream );

G3DEXPORTV2EXTENSION *g3dexportv2extension_new ( uint32_t signature,
                                             uint32_t (*write)( G3DEXPORTV2DATA      *ged, 
                                                                G3DEXPORTV2EXTENSION *ext, 
                                                                uint32_t            flags, 
                                                                FILE               *fdst ),
                                             void      *data );
void g3dexportv2extension_free ( G3DEXPORTV2EXTENSION *ext );

#endif
