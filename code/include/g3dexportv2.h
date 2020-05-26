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

#include "signatures.h"

typedef struct _G3DEXPORTDATA {
    G3DSCENE  *currentScene;
    G3DOBJECT *currentObject;
    G3DFACE   *currentFace;
    uint32_t   objectID;
    LIST     *lext;
} G3DEXPORTDATA;

/******************************************************************************/
typedef struct _G3DEXPORTEXTENSION {
    uint32_t signature;
    uint32_t (*write)( G3DEXPORTDATA *ged,
                       void          *data,
                       uint32_t       flags,
                       FILE          *fdst );
    void      *data;
} G3DEXPORTEXTENSION;

void g3dscene_exportv2 ( G3DSCENE *sce, 
                         char     *filename,
                         char     *comment,
                         LIST     *lextension, 
                         uint32_t  flags );

uint32_t g3dexport_writeChunk ( uint32_t   chunkSignature,
                                uint32_t (*writeChunk) ( G3DEXPORTDATA *ged,
                                                         void          *data,
                                                         uint32_t       flags,
                                                         FILE          *fdst ),
                                G3DEXPORTDATA *ged,
                                void          *data,
                                uint32_t       flags,
                                FILE          *fdst );
uint32_t g3dexport_fwrite ( void   *ptr,
                            size_t  size,
                            size_t  count,
                            FILE   *stream );

uint32_t g3dexportroot ( G3DEXPORTDATA *ged, 
                         G3DSCENE      *sce, 
                         uint32_t       flags, 
                         FILE          *fdst );

uint32_t g3dexportobject ( G3DEXPORTDATA *ged, 
                           G3DOBJECT     *obj,
                           uint32_t       flags, 
                           FILE          *fdst );
uint32_t g3dexportscene ( G3DEXPORTDATA *ged, 
                          G3DSCENE      *sce, 
                          uint32_t       flags, 
                          FILE          *fdst );
uint32_t g3dexportprimitive ( G3DEXPORTDATA *ged, 
                              G3DOBJECT     *obj, 
                              uint32_t       flags, 
                              FILE          *fdst );
uint32_t g3dexportmesh ( G3DEXPORTDATA *ged, 
                         G3DMESH       *mes, 
                         uint32_t       flags, 
                         FILE          *fdst );
uint32_t g3dexportchannel  ( G3DEXPORTDATA *ged, 
                             G3DCHANNEL    *cha,
                             uint32_t       flags, 
                             FILE          *fdst );
uint32_t g3dexportmaterial  ( G3DEXPORTDATA *ged, 
                              G3DMATERIAL   *mat, 
                              uint32_t       flags, 
                              FILE          *fdst );
uint32_t g3dexportuvmap ( G3DEXPORTDATA  *ged, 
                          G3DUVMAP       *uvmap, 
                          uint32_t        flags, 
                          FILE           *fdst );
uint32_t g3dexportcamera ( G3DEXPORTDATA *ged, 
                           G3DCAMERA     *cam, 
                           uint32_t       flags, 
                           FILE          *fdst );
uint32_t g3dexportlight ( G3DEXPORTDATA *ged, 
                          G3DLIGHT     *lig, 
                          uint32_t      flags, 
                          FILE         *fdst );
uint32_t g3dexportbone ( G3DEXPORTDATA *ged, 
                         G3DBONE     *cam, 
                         uint32_t       flags, 
                         FILE          *fdst );
uint32_t g3dexportsymmetry ( G3DEXPORTDATA *ged, 
                             G3DSYMMETRY   *sym, 
                             uint32_t       flags, 
                             FILE          *fdst );
uint32_t g3dexporttext ( G3DEXPORTDATA *ged, 
                         G3DTEXT       *txt, 
                         uint32_t       flags, 
                         FILE          *fdst );
uint32_t g3dexportsubdivider ( G3DEXPORTDATA *ged, 
                               G3DSUBDIVIDER   *sub, 
                               uint32_t       flags, 
                               FILE          *fdst );
uint32_t g3dexportwireframe ( G3DEXPORTDATA *ged, 
                              G3DWIREFRAME  *wfm, 
                              uint32_t       flags, 
                              FILE          *fdst );
uint32_t g3dexportsplinerevolver ( G3DEXPORTDATA    *ged, 
                                   G3DSPLINEREVOLVER *srv, 
                                   uint32_t          flags, 
                                   FILE             *fdst );
uint32_t g3dexportspline ( G3DEXPORTDATA*ged, 
                           G3DSPLINE    *spl, 
                           uint32_t      flags, 
                           FILE         *fdst );
uint32_t g3dexportextension ( G3DEXPORTDATA       *ged, 
                              G3DEXPORTEXTENSION  *ext, 
                              uint32_t             flags, 
                              FILE                *fdst );

uint32_t g3dexportffd ( G3DEXPORTDATA *ged, 
                        G3DFFD        *ffd, 
                        uint32_t       flags, 
                        FILE          *fdst );

uint32_t g3dexport_fwrited ( double *d, FILE *stream );
uint32_t g3dexport_fwritef ( float *f, FILE *stream );
uint32_t g3dexport_fwritel ( uint32_t *l, FILE *stream );
uint32_t g3dexport_fwritell ( uint64_t *ll, FILE *stream );

G3DEXPORTEXTENSION *g3dexportextension_new ( uint32_t signature,
                                             uint32_t (*write)( G3DEXPORTDATA      *ged, 
                                                                G3DEXPORTEXTENSION *ext, 
                                                                uint32_t            flags, 
                                                                FILE               *fdst ),
                                             void      *data );
void g3dexportextension_free ( G3DEXPORTEXTENSION *ext );

#endif
