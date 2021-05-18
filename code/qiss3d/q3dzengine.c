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

#define CLIPPINGPLANES 0x01

/******************************************************************************/
typedef struct _Q3DZHLINE {
    int32_t x1, x2;
    int32_t y1, y2;
    float   z1, z2;
    int     inited;
} Q3DZHLINE;

/******************************************************************************/
typedef struct _Q3DZPOINT {
    int32_t x;
    int32_t y;
    float   z;
} Q3DZPOINT;

/******************************************************************************/
typedef struct _Q3DZBUFFER {
    float    z;
    uint32_t qobjID;
    uint32_t qtriID;
} Q3DZBUFFER;

/******************************************************************************/
typedef struct _Q3DZENGINE {
    Q3DZBUFFER *buffer;
    Q3DZHLINE  *hlines;
} Q3DZENGINE;

/******************************************************************************/
static void q3dzengine_line ( Q3DZENGINE *qzen,
                              int         VPX[0x04],
                              Q3DZPOINT  *srcPoint,
                              Q3DZPOINT  *dstPoint ) {
    int32_t dx  = ( dstPoint->x - srcPoint->x ),
            ddx = ( dx == 0x00 ) ? 0x01 : abs ( dx ),
            dy  = ( dstPoint->y - srcPoint->y ),
            ddy = ( dy == 0x00 ) ? 0x01 : abs ( dy ),
            dd  = ( ddx > ddy ) ? ddx : ddy;
    float   dz  = dstPoint->z  - srcPoint->z, pz = ( dz / dd );
    int px = ( dx > 0x00 ) ? 1 : -1, 
            py = ( dy > 0x00 ) ? 1 : -1;
    int32_t x = srcPoint->x, 
            y = srcPoint->y;
    float   z = srcPoint->z;
    int i, cumul = 0x00;

    if ( ddx > ddy ) {
        for ( i = 0x00; i <= ddx; i++ ) {
            if ( cumul >= ddx ) {
                cumul -= ddx;
                y     += py;
            }

            if ( ( y >= VPX[0x01] ) && ( y <= VPX[0x03] ) ) {
                if ( qzen->hlines[y].inited == 0x00 ) {
                    qzen->hlines[y].inited = 0x01;

                    qzen->hlines[y].x1 = qzen->hlines[y].x2 = x;
                    qzen->hlines[y].z1 = qzen->hlines[y].z2 = z;
                } else {
                    if ( x < qzen->hlines[y].x1 ) {
                        qzen->hlines[y].x1 = x;
                        qzen->hlines[y].z1 = z;
                    }

                    if ( x > qzen->hlines[y].x2 ) {
                        qzen->hlines[y].x2 = x;
                        qzen->hlines[y].z2 = z;
                    }

                    qzen->hlines[y].inited = 0x02;
                }
            }

            cumul += ddy;
            x     += px;
            z     += pz;
        }
    } else {
        for ( i = 0x00; i <= ddy; i++ ) {
            if ( cumul >= ddy ) {
                    cumul -= ddy;
                    x     += px;
            }

            if ( ( y >= VPX[0x01] ) && ( y <= VPX[0x03] ) ) {
                if ( qzen->hlines[y].inited == 0x00 ) {
                     qzen->hlines[y].inited = 0x01;

                    qzen->hlines[y].x1 = qzen->hlines[y].x2 = x;
                    qzen->hlines[y].z1 = qzen->hlines[y].z2 = z;
                } else {
                    if ( x < qzen->hlines[y].x1 ) {
                        qzen->hlines[y].x1 = x;
                        qzen->hlines[y].z1 = z;
                    }

                    if ( x > qzen->hlines[y].x2 ) {
                        qzen->hlines[y].x2 = x;
                        qzen->hlines[y].z2 = z;
                    }

                    qzen->hlines[y].inited = 0x02;
                }
            }

            cumul += ddx;
            y     += py;
            z     += pz;
        }
    }
}

/****************************************************************************/
static void q3dzengine_fillHLine ( Q3DZENGINE *qzen,
                                   int         VPX[0x04],
                                   int32_t     y ) {
    Q3DZHLINE *hline = &qzen->hlines[y];
    int32_t x1 = hline->x1, x2 = hline->x2;
    int32_t dx = x2 - x1, ddx = ( dx == 0x00 ) ? 0x01 : abs ( dx );
    int32_t x = x1;
    double dz  = hline->z2 - hline->z1, pz = ( dz / ddx );
    long  px = ( dx > 0x00 ) ? 1 : -1;
    double z = hline->z1;
    int i;

    for ( i = 0x00; i < ddx; i++ ) {
        uint32_t offset = ( ( y ) * VPX[0x02] ) + x;
        /*** add some epsilon against Z fghting ***/
        float depth = qzen->buffer[offset].z;

        if ( ( x >= VPX[0x00] ) && 
             ( x <= VPX[0x02] ) ) {
            if  ( z <= depth ) {
                qzen->buffer[offset].z = z;
            }
        }

        x += px;
        z  += pz;
    }
}

/******************************************************************************/
void q3dzengine_drawTriangle ( Q3DZENGINE  *qzen,
                               Q3DTRIANGLE *qtri,
                               Q3DVERTEX   *qver,
                               double      *MVX,
                               double      *PJX,
                               int         *VPX,
                               Q3DPLANE    *frustrum ) {
    uint32_t qverID0 = qtri->qverID[0x00],
             qverID1 = qtri->qverID[0x01],
             qverID2 = qtri->qverID[0x02];
    Q3DVERTEX *qver[0x03] = { &qver[qverID0],
                              &qver[qverID1],
                              &qver[qverID2] };
    int32_t xmin = INT32_MAX,
            xmax = INT32_MIN,
            ymin = INT32_MAX,
            ymax = INT32_MIN,
            bymin, 
            bymax;
    Q3DVECTOR3  pworld[0x03];
    G3DVECTOR3  lworld[0x03][0x02]; /*** line points ***/
    Q3DVECTOR3 lscreen[0x03][0x02];
    Q3DVECTOR3 pclip[0x02]; /*** clipping points ***/
    uint32_t nodraw = 0xFFFFFFFF;
    uint32_t nbClip = 0x00;
    uint32_t i, j;

    q3dvector_matrix ( &qver[0x00]->pos, MVX, &pworld[0x00] );
    q3dvector_matrix ( &qver[0x01]->pos, MVX, &pworld[0x01] );
    q3dvector_matrix ( &qver[0x02]->pos, MVX, &pworld[0x02] );

    /*** Clip first, or else GluProject will return unwanted values ***/
    for ( i = 0x00; i < 0x03; i++ ) {
        uint32_t n = ( i + 0x01 ) % nbver;
        Q3DVECTOR3 *p1 = &pworld[i],
                   *p2 = &pworld[n];
        float distance = FLT_MAX;
        float s1, s2;

        memcpy ( &lworld[i][0x00], p1, sizeof ( Q3DVECTOR3 ) );
        memcpy ( &lworld[i][0x01], p2, sizeof ( Q3DVECTOR3 ) );

        for ( j = 0x00; j < CLIPPINGPLANES; j++ ) {
            /* Check if both points are inside the frustrum */
            s1 = ( frustrum[j].x * p1->x ) +
                 ( frustrum[j].y * p1->y ) +
                 ( frustrum[j].z * p1->z ) + frustrum[j].w,
            s2 = ( frustrum[j].x * p2->x ) +
                 ( frustrum[j].y * p2->y ) +
                 ( frustrum[j].z * p2->z ) + frustrum[j].w;

            /*** line lies entirely on the right side of the plane ***/
            if ( ( s1 > 0.0f ) && ( s2 > 0.0f ) ) {
                nodraw &= (~( 1 << i ));
            }

            if ( ( s1 * s2 ) < 0.0f ) {
                Q3DVECTOR3 it; /*** intersection ***/
                float t;

                nodraw &= (~( 1 << i ));

                /*** if p1 is on the outside of the frustrum, it becomes p2 ***/
                if ( s1 < 0.0f ) {
                    memcpy ( &lworld[i][0x00], p2, sizeof ( Q3DVECTOR3 ) );
                    memcpy ( &lworld[i][0x01], p1, sizeof ( Q3DVECTOR3 ) );
                }

                t = g3dcore_intersect ( &frustrum[j],
                                        &lworld[i][0x00],
                                        &lworld[i][0x01],
                                        &it );

                if (  ( t > 0.0f ) && ( t < distance ) ) {
                    /*** overwrite the outside point ***/
                    memcpy ( &lworld[i][0x01], &it, sizeof ( Q3DVECTOR3 ) );

                    /*** remember clipping point position for ***/
                    /*** drawing the clipping line ***/
                    gluProject ( it.x, 
                                 it.y,
                                 it.z,
                                 MVX, 
                                 PJX,
                                 VPX,
                                &pclip[nbClip].x, 
                                &pclip[nbClip].y, 
                                &pclip[nbClip].z );

                    distance = t;
                }

                /*** only 2 lines at most can clip. Prepare to store the **/
                /*** second clipping point ***/
                nbClip = 0x01;
            }
        }
    }

    for ( i = 0x00; i < 0x03; i++ ) {
        for ( j = 0x00; j < 0x02; j++ ) {
            gluProject ( lworld[i][j].x,
                         lworld[i][j].y,
                         lworld[i][j].z,
                         MVX,
                         PJX,
                         VPX,
                        &lscreen[i][j].x,
                        &lscreen[i][j].y,
                        &lscreen[i][j].z );
        }

        /* prepare 2D clipping */
        if ( ( nodraw & ( 1 << i ) ) == 0x00 ) {
            if ( lscreen[i][0x00].x < xmin ) xmin = lscreen[i][0x00].x;
            if ( lscreen[i][0x00].x > xmax ) xmax = lscreen[i][0x00].x;
            if ( lscreen[i][0x00].y < ymin ) ymin = lscreen[i][0x00].y;
            if ( lscreen[i][0x00].y > ymax ) ymax = lscreen[i][0x00].y;

            if ( lscreen[i][0x01].x < xmin ) xmin = lscreen[i][0x01].x;
            if ( lscreen[i][0x01].x > xmax ) xmax = lscreen[i][0x01].x;
            if ( lscreen[i][0x01].y < ymin ) ymin = lscreen[i][0x01].y;
            if ( lscreen[i][0x01].y > ymax ) ymax = lscreen[i][0x01].y;
        }
    }

    /*** Perform 2D clipping ***/
    if ( ( xmin > VPX[0x02] ) ||
         ( xmax < VPX[0x00] ) ||
         ( ymin > VPX[0x03] ) ||
         ( ymax < VPX[0x01] ) ) {
        return;
    }

    bymin = ( ymin < 0x00      ) ? 0x00      : ymin;
    bymax = ( ymax > VPX[0x03] ) ? VPX[0x03] : ymax;

    memset ( qzen->hlines + ymin, 0x00, sizeof ( Q3DZHLINE ) * ( bymax - 
                                                                 bymin - 1 ) );

    for ( i = 0x00; i < nbver; i++ ) {
        if ( ( nodraw & ( 1 << i ) ) == 0x00 ) {
            uint32_t n = ( i + 0x01 ) % nbver;
            Q3DVECTOR3 pt1 = { .x = lscreen[i][0x00].x,
                               .y = lscreen[i][0x00].y,
                               .z = lscreen[i][0x00].z },
                       pt2 = { .x = lscreen[i][0x01].x,
                               .y = lscreen[i][0x01].y,
                               .z = lscreen[i][0x01].z };

            if ( pt1.x < pt2.x ) q3dzengine_line ( qzen, VPX, &pt1, &pt2 );
            else                 q3dzengine_line ( qzen, VPX, &pt2, &pt1 );
        }
    }

    /*** Draw the new clipping line ***/
    if ( nbClip == 0x01 ) {
        Q3DVECTOR3 pt1 = { .x = pclip[0x00].x,
                           .y = pclip[0x00].y,
                           .z = pclip[0x00].z },
                   pt2 = { .x = pclip[0x01].x,
                           .y = pclip[0x01].y,
                           .z = pclip[0x01].z };

        if ( pt1.x < pt2.x ) q3dzengine_line ( qzen, VPX, &pt1, &pt2 );
        else                 q3dzengine_line ( qzen, VPX, &pt2, &pt1 );
    }

    for ( i = VPX[0x01]; i < VPX[0x03]; i++ ) {
        if ( qzen->hlines[i].inited == 0x02 ){
            q3dzengine_fillHLine ( qzen, VPX, i );
        }
    }
}

/******************************************************************************/
void q3dzengine_init ( Q3DZENGINE *qzen,
                       uint32_t    width,
                       uint32_t    height ) {
    uint32_t i;

    qzen->buffer = ( Q3DZBUFFER * ) calloc ( height *
                                             width, sizeof ( Q3DZBUFFER ) );

    for ( i = 0x00; i < height * width; i++ ) {
        qzen->buffer[i].z = INFINITY;
    }

    qzen->hlines = ( Q3DZHLINE * ) calloc ( height, sizeof ( Q3DZHLINE ) );
}
