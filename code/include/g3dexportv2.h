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


#ifdef FILE_FORMAT


#define SIG_ROOT
#define SIG_OBJECT
#define SIG_OBJECT_IDENTITY
#define SIG_OBJECT_IDENTITY_NAME
#define SIG_OBJECT_IDENTITY_TYPE            
#define SIG_OBJECT_IDENTITY_PARENT
#define SIG_OBJECT_IDENTITY_ACTIVE
#define SIG_OBJECT_TRANSFORMATION
#define SIG_KEYS
#define SIG_KEY_ENTRY
#define SIG_KEY_TRANSFORMATION
#define SIG_KEY_LOOP
#define SIG_OBJECT_SCENE
#define SIG_OBJECT_PRIMITIVE
#define SIG_OBJECT_PRIMITIVE_SPHERE
#define SIG_OBJECT_PRIMITIVE_TORUS
#define SIG_OBJECT_PRIMITIVE_CUBE
#define SIG_OBJECT_PRIMITIVE_CONE
#define SIG_OBJECT_PRIMITIVE_CYLINDER
#define SIG_OBJECT_PRIMITIVE_PLANE
#define SIG_MATERIALS
#define SIG_MATERIAL_ENTRY
#define SIG_MATERIAL_DIFFUSE
#define SIG_MATERIAL_BUMP
#define SIG_MATERIAL_ALPHA
#define SIG_MATERIAL_SPECULAR
#define SIG_MATERIAL_DISPLACEMENT
#define SIG_MATERIAL_REFLECTION
#define SIG_MATERIAL_REFRACTION
#define SIG_CHANNEL
#define SIG_CHANNEL_SOLID
#define SIG_CHANNEL_IMAGE
#define SIG_CHANNEL_PROCEDURAL
#define SIG_CHANNEL_PROCEDURAL_BRICK
#define SIG_CHANNEL_PROCEDURAL_CHESS
#define SIG_CHANNEL_PROCEDURAL_NOISE
#define SIG_OBJECT_LIGHT
#define SIG_OBJECT_LIGHT_TYPE
#define SIG_OBJECT_LIGHT_SHADOWCASTING
#define SIG_OBJECT_LIGHT_INTENSITY
#define SIG_OBJECT_LIGHT_SPOT
#define SIG_OBJECT_LIGHT_DIFFUSE
#define SIG_OBJECT_LIGHT_SPECULAR
#define SIG_OBJECT_MESH
#define SIG_OBJECT_MESH_GOURAUDLIMIT
#define SIG_OBJECT_MESH_GEOMETRY
#define SIG_OBJECT_MESH_GEOMETRY_VERTICES
#define SIG_OBJECT_MESH_GEOMETRY_POLYGONS
#define SIG_OBJECT_MESH_WEIGHTGROUPS
#define SIG_OBJECT_MESH_WEIGHTGROUP_ENTRY
#define SIG_OBJECT_MESH_WEIGHTGROUP_NAME
#define SIG_OBJECT_MESH_WEIGHTGROUP_WEIGHTS
#define SIG_OBJECT_MESH_UVMAPS
#define SIG_OBJECT_MESH_UVMAP_ENTRY
#define SIG_OBJECT_MESH_UVMAP_NAME
#define SIG_OBJECT_MESH_UVSETS
#define SIG_OBJECT_MESH_UVSET_ENTRY
#define SIG_OBJECT_BONE
#define SIG_OBJECT_BONE_TRANSFORM
#define SIG_OBJECT_BONE_WEIGHTGROUPS
#define SIG_OBJECT_BONE_WEIGHTGROUP
#define SIG_OBJECT_SYMMETRY
#define SIG_OBJECT_SPLINE
#define SIG_OBJECT_SPLINE_QUADRATIC
#define SIG_OBJECT_SPLINE_QUADRATIC_POINTS
#define SIG_OBJECT_SPLINE_QUADRATIC_SEGMENTS
#define SIG_OBJECT_TEXT
#define SIG_OBJECT_TEXT_FONT
#define SIG_OBJECT_TEXT_THICKNESS
#define SIG_OBJECT_TEXT_ROUNDNESS
#define SIG_OBJECT_TEXT_STRING
#define SIG_OBJECT_MODIFIER_FFD
#define SIG_OBJECT_MODIFIER_FFD_GEOMETRY
#define SIG_OBJECT_MODIFIER_FFD_UVW
#define SIG_OBJECT_MODIFIER_FFD_VERTICES
#define SIG_OBJECT_MODIFIER_SUBDIVIDER
#define SIG_OBJECT_MODIFIER_SUBDIVIDER_LEVEL
#define SIG_OBJECT_MODIFIER_WIREFRAME
#define SIG_OBJECT_MODIFIER_WIREFRAME_ALGO
#define SIG_OBJECT_MODIFIER_WIREFRAME_THICKNESS
#define SIG_OBJECT_MODIFIER_SLINEREVOLVER
#define SIG_OBJECT_MODIFIER_SLINEREVOLVER_GEOMETRY
#define SIG_OBJECT_CAMERA
#define SIG_OBJECT_CAMERA_FOCAL
#define SIG_EXTENSION
#define SIG_EXTENSION_NAME
#define SIG_EXTENSION_DATA

#endif

#include "signatures.h"

typedef struct _G3DEXPORTDATA {
    G3DOBJECT *currentObject;
    G3DFACE   *currentFace;
    uint32_t objectID;
} G3DEXPORTDATA;


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

uint32_t g3dexport_fwritef ( float *f, FILE *stream );
uint32_t g3dexport_fwritel ( uint32_t *l, FILE *stream );
uint32_t g3dexport_fwritell ( uint64_t *ll, FILE *stream );

#endif
