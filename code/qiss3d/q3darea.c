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
void q3darea_getZBuffer ( Q3DAREA *qarea, 
                          uint32_t    x, 
                          uint32_t    y,
                          Q3DZBUFFER *zout ) {
    uint32_t offset = ( y * qarea->width ) + x;

    memcpy ( zout, &qarea->qzen.buffer[offset], sizeof ( Q3DZBUFFER ) );
}

/******************************************************************************/
/*** This is responsible for RAY interpolation. We are not going to compute ***/
/*** each ray from calls to gluProject, we just need the boundary rays, then **/
/*** we build the other rays from those ones. It should be faster this way. ***/
static void q3darea_viewport ( Q3DAREA   *qarea, 
                               Q3DCAMERA *qcam ) {
    uint32_t coords[0x04][0x02] = { { qarea->x1, qarea->y1 },
                                    { qarea->x2, qarea->y1 },
                                    { qarea->x2, qarea->y2 },
                                    { qarea->x1, qarea->y2 } };
    G3DCAMERA *cam = ( G3DCAMERA * ) qobject_getObject ( ( Q3DOBJECT * ) qcam );
    G3DOBJECT *objcam = ( G3DOBJECT * ) cam;
    Q3DINTERPOLATION vip[0x04];
    double IWMVX[0x10];
    uint32_t i;

    for ( i = 0x00; i < 0x04; i++ ) {
        double rx, ry, rz;
        uint32_t x = coords[i][0x00],
                 y = coords[i][0x01];

        /*** Get ray's foreground coordinates ***/
        /*** Don't forget OpenGL coords are inverted in Y-Axis ***/
        gluUnProject ( x, qarea->height - y, 0.0f,
                       objcam->iwmatrix,
                       cam->pmatrix,
                       qarea->VPX,
                      &rx, &ry, &rz );

        vip[i].src.x = ( rx );
        vip[i].src.y = ( ry );
        vip[i].src.z = ( rz );
        /*vip[i].src.w = 1.0f;*/

        /*** Get ray's background coordinates ***/
        /*** Don't forget OpenGL coords are inverted in Y-Axis ***/
        gluUnProject ( x, qarea->height - y, 1.0f, 
                       objcam->iwmatrix, 
                       cam->pmatrix,
                       qarea->VPX,
                      &rx, &ry, &rz );

        vip[i].dst.x = ( rx );
        vip[i].dst.y = ( ry );
        vip[i].dst.z = ( rz );
        /*vip[i].dst.w = 1.0f;*/
    }

    /*** Viewport vertical interpolation ***/
    q3dinterpolation_build ( &vip[0x00], &vip[0x03], qarea->y2 - qarea->y1 );
    q3dinterpolation_build ( &vip[0x01], &vip[0x02], qarea->y2 - qarea->y1 );

    memcpy ( &qarea->pol[0x00], &vip[0x00], sizeof ( Q3DINTERPOLATION ) );
    memcpy ( &qarea->pol[0x01], &vip[0x01], sizeof ( Q3DINTERPOLATION ) );
}

/******************************************************************************/
void q3darea_reset ( Q3DAREA *qarea ) {
    q3dzengine_reset ( &qarea->qzen );
}

/******************************************************************************/
void q3darea_init ( Q3DAREA   *qarea,
                    Q3DSCENE  *qsce,
                    Q3DCAMERA *qcam,
                    uint32_t   x1,
                    uint32_t   y1,
                    uint32_t   x2,
                    uint32_t   y2,
                    uint32_t   width,
                    uint32_t   height,
                    uint32_t   depth,
                    float      frame ) {
    G3DCAMERA *cam = ( G3DCAMERA * ) qobject_getObject ( ( Q3DOBJECT * ) qcam );
    G3DOBJECT *objcam = ( G3DOBJECT * ) cam;

    /*** render square area ***/
    qarea->x1     = x1;
    qarea->x2     = x2;
    qarea->y1     = y1;
    qarea->y2     = y2;
    qarea->width  = width;
    qarea->height = height;
    qarea->depth  = depth;

    qarea->VPX[0x00] = 0x00;
    qarea->VPX[0x01] = 0x00;
    qarea->VPX[0x02] = qarea->width;
    qarea->VPX[0x03] = qarea->height;

    /*** first scan line ***/
    qarea->scanline = y1;

    /*** Compute the interpolation factors for rays ***/
    q3darea_viewport ( qarea, qcam );

    q3dzengine_init ( &qarea->qzen,
                       objcam->iwmatrix,
                       cam->pmatrix,
                       qarea->VPX,
                       width,
                       height );

    q3dzengine_drawObject_r ( &qarea->qzen,
                               qsce,
                               objcam->iwmatrix,
                               cam->pmatrix,
                               qarea->VPX,
                               frame );
}
