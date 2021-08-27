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
#include <config.h>
#include <g3dexportv3.h>

/******************************************************************************/
uint32_t g3dexportv3skin ( G3DEXPORTV3DATA *ged, 
                         G3DSKIN       *skn, 
                         uint32_t       flags, 
                         FILE          *fdst ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) skn;
    uint32_t size = 0x00;

    /*size += g3dexportv3_writeChunk ( SIG_OBJECT_BONE_LENGTH,
                                   g3dexportv3bone_length,
                                   ged,
                                   bon,
                                   0xFFFFFFFF,
                                   fdst );*/

    return size;
}
