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
static double IDX[0x10] = { 1.0f, 0.0f, 0.0f, 0.0f,
                            0.0f, 1.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 1.0f, 0.0f,
                            0.0f, 0.0f, 0.0f, 1.0f };

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
void q3darea_viewport ( Q3DAREA   *qarea, 
                        Q3DCAMERA *qcam ) {
    uint32_t coords[0x04][0x02] = { { qarea->x1, qarea->y1 },
                                    { qarea->x2, qarea->y1 },
                                    { qarea->x2, qarea->y2 },
                                    { qarea->x1, qarea->y2 } };
    G3DCAMERA *cam = ( G3DCAMERA * ) q3dobject_getObject ( ( Q3DOBJECT * ) qcam );
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
        gluUnProject ( x, qarea->height - y, cam->znear,
                       objcam->iwmatrix,
                       qcam->PJX,
                       qcam->VPX,
                      &rx, &ry, &rz );

        vip[i].src.x = ( rx );
        vip[i].src.y = ( ry );
        vip[i].src.z = ( rz );
        /*vip[i].src.w = 1.0f;*/

        /*** Get ray's background coordinates ***/
        /*** Don't forget OpenGL coords are inverted in Y-Axis ***/
        gluUnProject ( x, qarea->height - y, 1.0f, 
                       objcam->iwmatrix, 
                       qcam->PJX,
                       qcam->VPX,
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
static uint32_t excludePerfectSpheres ( Q3DOBJECT *qobj, void *data ) {
    if ( qobj->obj->type == G3DSPHERETYPE ) {
        if ( qobj->obj->flags & SPHEREISPERFECT ) {
            return 0x00;
        }
    }

    return 0x01;
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
    G3DCAMERA *cam = ( G3DCAMERA * ) q3dobject_getObject ( ( Q3DOBJECT * ) qcam );
    G3DOBJECT *objcam = ( G3DOBJECT * ) cam;
    Q3DOBJECT *qobjcam = ( Q3DOBJECT * ) qcam;

    /*** render square area ***/
    qarea->x1     = x1;
    qarea->x2     = x2;
    qarea->y1     = y1;
    qarea->y2     = y2;
    qarea->width  = width;
    qarea->height = height;
    qarea->depth  = depth;

    /*qarea->VPX[0x00] = 0x00;
    qarea->VPX[0x01] = 0x00;
    qarea->VPX[0x02] = qarea->width;
    qarea->VPX[0x03] = qarea->height;*/

    /*** first scan line ***/
    qarea->scanline = y1;

    qarea->qssh = calloc ( width * height, sizeof ( Q3DSOFTSHADOW ) );

    q3dzengine_init ( &qarea->qzen,
                       cam->znear,
                       IDX,
                       qcam->PJX,
                       qcam->VPX,
                       width,
                       height );

    q3dzengine_drawObjectWithCondition_r ( &qarea->qzen,
                                            qsce,
                                            objcam->iwmatrix,
                                            qcam->PJX,
                                            qcam->VPX,
                                            excludePerfectSpheres,
                                            NULL,
                                            frame );
}

/******************************************************************************/
static void sss ( Q3DSOFTSHADOW *qssh,
                  uint32_t       width,
                  uint32_t       height,
                  uint32_t       x,
                  uint32_t       y ) {
/*    static int32_t block[0x08][0x02] = { { -1,  1 }, {  0,  1 }, {  1,  1 },
                                         { -1,  0 },             {  1,  0 },
                                         { -1, -1 }, {  0, -1 }, {  1, -1 } };
*/
    /*static int32_t block[0x18][0x02] = { { -2,  2 }, { -1,  2 }, {  0,  2 }, {  1,  2 }, {  2,  2 },
                                         { -2,  1 }, { -1,  1 }, {  0,  1 }, {  1,  1 }, {  2,  1 },
                                         { -2,  0 }, { -1,  0 },             {  1,  0 }, {  2,  0 },
                                         { -2, -1 }, { -1, -1 }, {  0, -1 }, {  1, -1 }, {  2, -1 },
                                         { -2, -2 }, { -1, -2 }, {  0, -2 }, {  1, -2 }, {  2, -2 } };*/

    /*static int32_t block[0x50][0x02] = { { -4,  4 }, { -3,  4 }, { -2,  4 }, { -1,  4 }, {  0,  4 }, {  1,  4 }, {  2,  4 }, { 3,  4 }, { 4,  4 },
                                         { -4,  3 }, { -3,  3 }, { -2,  3 }, { -1,  3 }, {  0,  3 }, {  1,  3 }, {  2,  3 }, { 3,  3 }, { 4,  3 },
                                         { -4,  2 }, { -3,  2 }, { -2,  2 }, { -1,  2 }, {  0,  2 }, {  1,  2 }, {  2,  2 }, { 3,  2 }, { 4,  2 },
                                         { -4,  1 }, { -3,  1 }, { -2,  1 }, { -1,  1 }, {  0,  1 }, {  1,  1 }, {  2,  1 }, { 3,  1 }, { 4,  1 },
                                         { -4,  0 }, { -3,  0 }, { -2,  0 }, { -1,  0 },             {  1,  0 }, {  2,  0 }, { 3,  0 }, { 4,  0 },
                                         { -4, -1 }, { -3, -1 }, { -2, -1 }, { -1, -1 }, {  0, -1 }, {  1, -1 }, {  2, -1 }, { 3, -1 }, { 4, -1 },
                                         { -4, -2 }, { -3, -2 }, { -2, -2 }, { -1, -2 }, {  0, -2 }, {  1, -2 }, {  2, -2 }, { 3, -2 }, { 4, -2 },
                                         { -4, -3 }, { -3, -3 }, { -2, -3 }, { -1, -3 }, {  0, -3 }, {  1, -3 }, {  2, -3 }, { 3, -3 }, { 4, -3 },
                                         { -4, -4 }, { -3, -4 }, { -2, -4 }, { -1, -4 }, {  0, -4 }, {  1, -4 }, {  2, -4 }, { 3, -4 }, { 4, -4 }, }; */
    static int32_t block[120][0x02] = { { -5,  5 }, { -4,  5 }, { -3,  5 }, { -2,  5 }, { -1,  5 }, {  0,  5 }, {  1,  5 }, {  2,  5 }, { 3,  5 }, { 4,  5 }, { 5,  5 },
                                        { -5,  4 }, { -4,  4 }, { -3,  4 }, { -2,  4 }, { -1,  4 }, {  0,  4 }, {  1,  4 }, {  2,  4 }, { 3,  4 }, { 4,  4 }, { 5,  4 },
                                        { -5,  3 }, { -4,  3 }, { -3,  3 }, { -2,  3 }, { -1,  3 }, {  0,  3 }, {  1,  3 }, {  2,  3 }, { 3,  3 }, { 4,  3 }, { 5,  3 },
                                        { -5,  2 }, { -4,  2 }, { -3,  2 }, { -2,  2 }, { -1,  2 }, {  0,  2 }, {  1,  2 }, {  2,  2 }, { 3,  2 }, { 4,  2 }, { 5,  2 },
                                        { -5,  1 }, { -4,  1 }, { -3,  1 }, { -2,  1 }, { -1,  1 }, {  0,  1 }, {  1,  1 }, {  2,  1 }, { 3,  1 }, { 4,  1 }, { 5,  1 },
                                        { -5,  0 }, { -4,  0 }, { -3,  0 }, { -2,  0 }, { -1,  0 },             {  1,  0 }, {  2,  0 }, { 3,  0 }, { 4,  0 }, { 5,  0 },
                                        { -5, -1 }, { -4, -1 }, { -3, -1 }, { -2, -1 }, { -1, -1 }, {  0, -1 }, {  1, -1 }, {  2, -1 }, { 3, -1 }, { 4, -1 }, { 5, -1 },
                                        { -5, -2 }, { -4, -2 }, { -3, -2 }, { -2, -2 }, { -1, -2 }, {  0, -2 }, {  1, -2 }, {  2, -2 }, { 3, -2 }, { 4, -2 }, { 5, -2 },
                                        { -5, -3 }, { -4, -3 }, { -3, -3 }, { -2, -3 }, { -1, -3 }, {  0, -3 }, {  1, -3 }, {  2, -3 }, { 3, -3 }, { 4, -3 }, { 5, -3 },
                                        { -5, -4 }, { -4, -4 }, { -3, -4 }, { -2, -4 }, { -1, -4 }, {  0, -4 }, {  1, -4 }, {  2, -4 }, { 3, -4 }, { 4, -4 }, { 5, -4 },
                                        { -5, -5 }, { -4, -5 }, { -3, -5 }, { -2, -5 }, { -1, -5 }, {  0, -5 }, {  1, -5 }, {  2, -5 }, { 3, -5 }, { 4, -5 }, { 5, -5 } };

    uint32_t offset = ( y * width ) + x;
    uint32_t nbs = 0x00;
    uint32_t i;

    qssh[offset].average = 0.0f;

    for ( i = 0x00; i < 120; i++ ) {
        int32_t tx = ( int32_t ) x + block[i][0x00],
                ty = ( int32_t ) y + block[i][0x01];

        if ( ( tx > 0x00 ) && ( tx < width  ) &&
             ( ty > 0x00 ) && ( ty < height ) ) {
            uint32_t toffset = ( ty * width ) + tx;

            qssh[offset].average +=  qssh[toffset].shadow;

            nbs++;
        }
    }

    if ( nbs ) {
        qssh[offset].average /= ( float ) nbs;
    }
}

/******************************************************************************/
void q3darea_averageSoftShadows ( Q3DAREA *qarea ) {
    uint32_t i, j;

    for ( i = 0x00; i < qarea->height; i++ ) {
        for ( j = 0x00; j < qarea->width; j++ ) {
            uint32_t offset = ( i * qarea->width ) + j;

            if ( qarea->qssh[offset].count ) {
                qarea->qssh[offset].shadow /= qarea->qssh[offset].count;
            }
        }
    }

    for ( i = 0x00; i < qarea->height; i++ ) {
        for ( j = 0x00; j < qarea->width; j++ ) {
            uint32_t offset = ( i * qarea->width ) + j;

            /*if ( qarea->qssh[offset].count ) {*/
                sss ( qarea->qssh, 
                      qarea->width, 
                      qarea->height, 
                      j, 
                      i );
            /*}*/
        }
    }
}
