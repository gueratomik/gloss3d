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

#ifdef unused

/******************************************************************************/
static uint32_t g3dexportv3myobjectvar ( G3DEXPORTV3DATA *ged, 
                                       mytype        *myobject, 
                                       uint32_t       flags, 
                                       FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwrite ( &myobject->myvar, sizeof ( uint32_t ), 0x01, fdst );

    return size;
}


/******************************************************************************/
uint32_t g3dexportv3myobject ( G3DEXPORTV3DATA *ged, 
                              mytype        *myobject, 
                              uint32_t       flags, 
                              FILE          *fdst ) {
    uint32_t size = 0x00;


    size += g3dexportv3_writeChunk ( SIG_XXXXXXX,
                   EXPORTV3_CALLBACK(g3dexportv3xxxxx),
                                   ged,
                                   myobject,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

#endif
