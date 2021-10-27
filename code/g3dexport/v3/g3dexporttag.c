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
static uint32_t g3dexportv3tag_trackerTarget ( G3DEXPORTV3DATA *ged, 
                                               G3DTRACKERTAG   *ttag,
                                               uint32_t         flags, 
                                               FILE            *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel( &ttag->target->id, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3tag_trackerOrientation ( G3DEXPORTV3DATA *ged, 
                                                    G3DTRACKERTAG   *ttag,
                                                    uint32_t         flags, 
                                                    FILE            *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel( &ttag->orientation, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3tag_tracker ( G3DEXPORTV3DATA *ged, 
                                         G3DTRACKERTAG   *ttag, 
                                         uint32_t         flags, 
                                         FILE            *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_TAG_TRACKER_ORIENTATION,
                                     g3dexportv3tag_trackerOrientation,
                                     ged,
                                     ttag,
                                     flags,
                                     fdst );

    if ( ttag->target ) {
        if ( g3dscene_isObjectReferred ( ttag->sce, ttag->target ) ) {
            /*** useful check in case the target has been removed but not freed ***/
            if ( ( ttag->target->flags & OBJECTORPHANED ) == 0x00 ) {
                size += g3dexportv3_writeChunk ( SIG_OBJECT_TAG_TRACKER_TARGET,
                                                 g3dexportv3tag_trackerTarget,
                                                 ged,
                                                 ttag,
                                                 flags,
                                                 fdst );
            }
        }
    }

    return size;
}

/******************************************************************************/
uint32_t g3dexportv3tag ( G3DEXPORTV3DATA *ged, 
                          G3DTAG          *tag, 
                          uint32_t         flags, 
                          FILE            *fdst ) {
    uint32_t size = 0x00;

    switch ( tag->type ) {
        case G3DTAGTRACKERTYPE : {
            size += g3dexportv3_writeChunk ( SIG_OBJECT_TAG_TRACKER,
                                             g3dexportv3tag_tracker,
                                             ged,
                                             tag,
                                             0xFFFFFFFF,
                                             fdst );
        } break;
 
        default :
        break;
    }


    return size;
}
