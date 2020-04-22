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
GLOSS3D("GLS3")

SCENE(0x01000000)
OBJECT (0x00100000)
--- IDENTITY(0x00110000)
------- NAME(0x00111000)
----------- Name ( char[] )
------- TYPE(0x00112000)
----------- Type ( uint32_t )
------- PARENT(0x00113000)
----------- ID ( uint32_t )
------- ACTIVE(0x00114000)
----------- active ( uint32_t ) /** 1 or 0 **/
--- ORIENTATION(0x00120000)
------- Position ( float-float-float-float )
------- Rotation ( float-float-float-float )
------- Scale    ( float-float-float-float )
--- KEYS (0x00130000)
------- KEY (0x00131000)
----------- TRANSFORMATION (0x00131100)
--------------- frame num ( float )
--------------- Position  ( float-float-float-uint32_t ) /** last int is usage **/
--------------- Rotation  ( float-float-float-uint32_t ) /** last int is usage **/
--------------- Scale     ( float-float-float-uint32_t ) /** last int is usage **/
----------- LOOP (0x00131200)
-----------   LoopFrame ( float )
#endif

#define SIG_SCENE                           0x01000000
#define SIG_OBJECT                          0x00100000
#define SIG_OBJECT_IDENTITY                 0x00110000
#define SIG_OBJECT_IDENTITY_NAME            0x00111000
#define SIG_OBJECT_IDENTITY_TYPE            0x00112000
#define SIG_OBJECT_IDENTITY_PARENT          0x00113000
#define SIG_OBJECT_IDENTITY_ACTIVE          0x00114000
#define SIG_OBJECT_TRANSFORMATION           0x00120000
#define SIG_OBJECT_KEYS                     0x00130000
#define SIG_OBJECT_KEYS_KEY                 0x00131000
#define SIG_OBJECT_KEYS_KEY_TRANSFORMATION  0x00131100
#define SIG_OBJECT_KEYS_KEY_LOOP            0x00131200


uint32_t g3dexportobject ( G3DOBJECT *obj, uint32_t flags, FILE *fdst );
void g3dscene_exportv2 ( G3DSCENE *sce, 
                         char     *filename,
                         char     *comment,
                         LIST     *lextension, 
                         uint32_t  flags );

#endif
