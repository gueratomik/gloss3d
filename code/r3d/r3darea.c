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
#include <r3d.h>

/******************************************************************************/
/*** This is responsible for RAY interpolation. We are not going to compute ***/
/*** each ray from calls to gluProject, we just need the boundary rays, then **/
/*** we build the other rays from those ones. It should be faster this way. ***/
void r3darea_viewport ( R3DAREA *area, uint32_t x1, uint32_t y1, 
                                       uint32_t x2, uint32_t y2,
                                       uint32_t width, uint32_t height ) {
    uint32_t coords[0x04][0x02] = { { x1, y1 }, { x2, y1 },
                                    { x2, y2 }, { x1, y2 } };
    /*double *MVX = ((G3DOBJECT*)cam)->iwmatrix;*/
    GLint   VPX[0x04] = { 0x00, 0x00, width, height };
    R3DINTERPOLATION vip[0x04];
    uint32_t i;

    for ( i = 0x00; i < 0x04; i++ ) {
        double rx, ry, rz;
        uint32_t x = coords[i][0x00],
                 y = coords[i][0x01];

        /*** Get ray's foreground coordinates ***/
        /*** Don't forget OpenGL coords are inverted in Y-Axis ***/
        gluUnProject ( x, height - y, 0.000001f, area->rcam->MVX, 
                                                 area->rcam->PJX,
                                                 VPX, &rx, &ry, &rz );

        vip[i].src.x = ( rx );
        vip[i].src.y = ( ry );
        vip[i].src.z = ( rz );
        /*vip[i].src.w = 1.0f;*/

        /*** Get ray's background coordinates ***/
        /*** Don't forget OpenGL coords are inverted in Y-Axis ***/
        gluUnProject ( x, height - y, 0.999999f, area->rcam->MVX, 
                                                 area->rcam->PJX,
                                                 VPX, &rx, &ry, &rz );

        vip[i].dst.x = ( rx );
        vip[i].dst.y = ( ry );
        vip[i].dst.z = ( rz );
        /*vip[i].dst.w = 1.0f;*/
    }

    /*** Viewport vertical interpolation ***/
    r3dinterpolation_build ( &vip[0x00], &vip[0x03], y2 - y1 );
    r3dinterpolation_build ( &vip[0x01], &vip[0x02], y2 - y1 );

    memcpy ( &area->pol[0x00], &vip[0x00], sizeof ( R3DINTERPOLATION ) );
    memcpy ( &area->pol[0x01], &vip[0x01], sizeof ( R3DINTERPOLATION ) );
}
