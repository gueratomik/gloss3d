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
#include <g3dengine/g3dengine.h>

/******************************************************************************/
void g3dobjectextension_init ( G3DOBJECTEXTENSION *ext,
                               uint16_t            name, 
                               uint16_t            unit ) {
    ext->id = ( name << 16 ) | unit;
}

/******************************************************************************/
G3DMESHPOSEEXTENSION *g3dmeshposeextension_new ( ) {
    uint32_t size = sizeof ( G3DMESHPOSEEXTENSION );
    G3DMESHPOSEEXTENSION *ext = ( G3DMESHPOSEEXTENSION * ) calloc ( 0x01, 
                                                                      size );

    if ( ext == NULL ) {
        fprintf ( stderr, "g3dmeshposeextension_new: calloc failed\n");

        return NULL;
    }

    g3dobjectextension_init ( ( G3DOBJECTEXTENSION * ) ext,
                                                       MESHPOSEEXTENSION, 
                                                       0x00 );

    return ext;
}

/******************************************************************************/
void g3dmeshposeextension_selectPose ( G3DMESHPOSEEXTENSION *ext, 
                                       G3DMESHPOSE *mps ) {
    mps->flags |= MESHPOSESELECTED;

    ext->curmps = mps;
}

/******************************************************************************/
void g3dmeshposeextension_unselectPose ( G3DMESHPOSEEXTENSION *ext, 
                                         G3DMESHPOSE *mps ) {
    mps->flags &= (~MESHPOSESELECTED);

    ext->curmps = NULL;
}

/******************************************************************************/
G3DMESHPOSE *g3dmeshposeextension_createPose ( G3DMESHPOSEEXTENSION *ext,
                                               char                 *name ) {
    G3DMESHPOSE *mps = g3dmeshpose_new ( name, ext->mpsid++ );

    list_insert ( &ext->lmps, mps );

    ext->nbmps++;

    return mps;
}

/******************************************************************************/
void *g3dmeshposeextension_removePose ( G3DMESHPOSEEXTENSION *ext,
                                        G3DMESHPOSE          *mps ) {
    list_remove ( &ext->lmps, mps );

    ext->nbmps--;
}

/******************************************************************************/
void g3dvertexextension_init ( G3DVERTEXEXTENSION *ext,
                               uint16_t            name, 
                               uint16_t            unit ) {
    ext->id = ( name << 16 ) | unit;


    return ext;
}

/******************************************************************************/
G3DVERTEXPOSEEXTENSION *g3dvertexposeextension_new ( uint16_t unit ) {
    uint32_t size = sizeof ( G3DVERTEXPOSEEXTENSION );
    G3DVERTEXPOSEEXTENSION *ext = ( G3DVERTEXPOSEEXTENSION * ) calloc ( 0x01, 
                                                                          size );

    if ( ext == NULL ) {
        fprintf ( stderr, "g3dvertexposeextension_new: calloc failed\n");

        return NULL;
    }

    g3dvertexextension_init ( ( G3DVERTEXEXTENSION * ) ext,
                                                       VERTEXPOSEEXTENSION, 
                                                       unit );


    return ext;
}
