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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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
#include <g3dengine/g3dengine.h>

/******************************************************************************/
void g3dobjectextension_init ( G3DOBJECTEXTENSION *ext,
                               uint16_t            name, 
                               uint16_t            unit ) {
    ext->id = ( name << 16 ) | unit;
}

/******************************************************************************/
G3DMESHMORPHEXTENSION *g3dmeshmorphextension_new ( ) {
    uint32_t size = sizeof ( G3DMESHMORPHEXTENSION );
    G3DMESHMORPHEXTENSION *ext = ( G3DMESHMORPHEXTENSION * ) calloc ( 0x01, 
                                                                      size );

    if ( ext == NULL ) {
        fprintf ( stderr, "g3dmeshmorphextension_new: calloc failed\n");

        return NULL;
    }

    g3dobjectextension_init ( ( G3DOBJECTEXTENSION * ) ext,
                                                       MESHMORPHEXTENSION, 
                                                       0 );


    return ext;
}

/******************************************************************************/
void g3dvertexextension_init ( G3DVERTEXEXTENSION *ext,
                               uint16_t            name, 
                               uint16_t            unit ) {
    ext->id = ( name << 16 ) | unit;


    return ext;
}

/******************************************************************************/
G3DVERTEXMORPHEXTENSION *g3dvertexmorphextension_new ( uint16_t unit ) {
    uint32_t size = sizeof ( G3DVERTEXMORPHEXTENSION );
    G3DVERTEXMORPHEXTENSION *ext = ( G3DVERTEXMORPHEXTENSION * ) calloc ( 0x01, 
                                                                          size );

    if ( ext == NULL ) {
        fprintf ( stderr, "g3dvertexmorphextension_new: calloc failed\n");

        return NULL;
    }

    g3dvertexextension_init ( ( G3DVERTEXEXTENSION * ) ext,
                                                       VERRTEXMORPHEXTENSION, 
                                                       unit );


    return ext;
}
