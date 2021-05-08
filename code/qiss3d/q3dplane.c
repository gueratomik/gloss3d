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
/*** From http://www.siggraph.org/education/materials/HyperGraph/raytrace/  ***/
/*** rayplane_intersection.htm                                              ***/
/******************************************************************************/
uint32_t q3dplane_intersectLine ( Q3DPLANE  *pla, 
                                  Q3DLINE   *lin, 
                                  Q3DVECTOR *pnt ) {
    float vo = ( pla->x * lin->src.x ) +
               ( pla->y * lin->src.y ) +
               ( pla->z * lin->src.z ) + pla->w,
          vd = ( pla->x * lin->dir.x ) + 
               ( pla->y * lin->dir.y ) +
               ( pla->z * lin->dir.z );
    uint32_t ret;
    float t;

    if ( vd == 0.0f ) return 0x00;

    t = - ( vo / vd );

    if ( ( t > 0.0f ) && ( t < 1.0f ) ) {
        pnt->x = lin->src.x + ( lin->dir.x * t );
        pnt->y = lin->src.y + ( lin->dir.y * t );
        pnt->z = lin->src.z + ( lin->dir.z * t );

        return 0x01;
    }

    return 0x00;
}
