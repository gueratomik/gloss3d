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
void q3dinterpolation_build ( Q3DINTERPOLATION *qpol,
                              Q3DSEGMENT       *qone,
                              Q3DSEGMENT       *qtwo,
                              uint32_t          step ) {
    /*** we store interpolation factors ***/
    if ( step ) {
        qpol->srcdif.x = ( qtwo->src.x - qone->src.x ) / step;
        qpol->srcdif.y = ( qtwo->src.y - qone->src.y ) / step;
        qpol->srcdif.z = ( qtwo->src.z - qone->src.z ) / step;

        qpol->dstdif.x = ( qtwo->dst.x - qone->dst.x ) / step;
        qpol->dstdif.y = ( qtwo->dst.y - qone->dst.y ) / step;
        qpol->dstdif.z = ( qtwo->dst.z - qone->dst.z ) / step;
    }
}

/******************************************************************************/
void q3dsegment_interpolate ( Q3DSEGMENT       *qseg, 
                              Q3DINTERPOLATION *qpol, 
                              float             factor ) {
    qseg->src.x += ( qpol->srcdif.x * factor );
    qseg->src.y += ( qpol->srcdif.y * factor );
    qseg->src.z += ( qpol->srcdif.z * factor );

    qseg->dst.x += ( qpol->dstdif.x * factor );
    qseg->dst.y += ( qpol->dstdif.y * factor );
    qseg->dst.z += ( qpol->dstdif.z * factor );
}
