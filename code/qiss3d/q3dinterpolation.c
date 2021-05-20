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
#include <qiss3d/q3d.h>

/******************************************************************************/
/*** Find the interpolation factors between two vectors. R3DINTERPOLATION   ***/
/*** structure receive an original position and interpolation factors       ***/
/******************************************************************************/
void q3dinterpolation_build ( Q3DINTERPOLATION *rone,
                              Q3DINTERPOLATION *rtwo,
                              uint32_t          step ) {
    /*** we store interpolation factors ***/
    if ( step ) {
        rone->srcdif.x = ( rtwo->src.x - rone->src.x ) / step;
        rone->srcdif.y = ( rtwo->src.y - rone->src.y ) / step;
        rone->srcdif.z = ( rtwo->src.z - rone->src.z ) / step;

        rone->dstdif.x = ( rtwo->dst.x - rone->dst.x ) / step;
        rone->dstdif.y = ( rtwo->dst.y - rone->dst.y ) / step;
        rone->dstdif.z = ( rtwo->dst.z - rone->dst.z ) / step;
    }
}

/******************************************************************************/
void q3dinterpolation_step ( Q3DINTERPOLATION *pol ) {
    pol->src.x += pol->srcdif.x;
    pol->src.y += pol->srcdif.y;
    pol->src.z += pol->srcdif.z;

    pol->dst.x += pol->dstdif.x;
    pol->dst.y += pol->dstdif.y;
    pol->dst.z += pol->dstdif.z;
}
