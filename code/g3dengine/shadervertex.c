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
/**
 * This function initialize the G3DRTVERTEX structure depending on the
 * G3DVERTEX structure passed as  the second argument. The facsel argument 
 * allows one to skip (or not) the vertex colorizing when the G3DRTVERTEX 
 * belongs to a face that has been selected for example.
 */
/******************************************************************************/
void shadervertex_init ( SHADERVERTEX *shdver,
                         G3DVERTEX    *ver,
                         uint32_t      facsel,
                         uint64_t      engine_flags ) {
    if ( engine_flags & VIEWSKIN ) {
        shdver->col.r = 1.0f;
        shdver->col.g = /*( ver->flags & VERTEXPAINTED ) ? */ver->weight/* : 0.0f*/;
        shdver->col.b = 0.0f;
    } else {
        if ( ver->nbfac == 0x00 ) {
            if ( ver->flags & VERTEXSELECTED ) {
                shdver->col.r = 1.0f;
                shdver->col.g = 0.0f;
                shdver->col.b = 0.0f;
            } else {
                shdver->col.r = 0.0f;
                shdver->col.g = 0.0f;
                shdver->col.b = 1.0f;
            }
        } else {
            if ( facsel ) {
                shdver->col.r = 1.0f;
                shdver->col.g = 0.5f;
                shdver->col.b = 0.0f;
            } else {
                shdver->col.r = shdver->col.g = shdver->col.b = MESHCOLORF;
            }
        }
    }

    shdver->flags = ver->flags;

    memcpy ( &shdver->nor, &ver->nor, sizeof ( G3DVECTOR3F ) );
    memcpy ( &shdver->pos, &ver->pos, sizeof ( G3DVECTOR3F ) );
}
