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
#include <r3d.h>

/******************************************************************************/
/*** Find the interpolation factors between two vectors. R3DINTERPOLATION   ***/
/*** structure receive an original position and interpolation factors       ***/
/******************************************************************************/
void r3dinterpolation_build ( R3DINTERPOLATION *rone, R3DINTERPOLATION *rtwo,
                              uint32_t step ) {
    /*** we store interpolation factors ***/
    if ( step ) {
        rone->srcdifx = ( rtwo->src.x - rone->src.x ) / step;
        rone->srcdify = ( rtwo->src.y - rone->src.y ) / step;
        rone->srcdifz = ( rtwo->src.z - rone->src.z ) / step;

        rone->dstdifx = ( rtwo->dst.x - rone->dst.x ) / step;
        rone->dstdify = ( rtwo->dst.y - rone->dst.y ) / step;
        rone->dstdifz = ( rtwo->dst.z - rone->dst.z ) / step;
    }
}

/******************************************************************************/
void r3dinterpolation_step ( R3DINTERPOLATION *pol ) {
    pol->src.x += pol->srcdifx;
    pol->src.y += pol->srcdify;
    pol->src.z += pol->srcdifz;

    pol->dst.x += pol->dstdifx;
    pol->dst.y += pol->dstdify;
    pol->dst.z += pol->dstdifz;
}
