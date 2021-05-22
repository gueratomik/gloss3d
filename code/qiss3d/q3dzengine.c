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
static void q3dzengine_buildFrustrum ( Q3DZENGINE *qzen,
                                       double     *MVX,
                                       double     *PJX,
                                       int        *VPX ) {
    Q3DVECTOR3D scr2D[0x04] = { { .x = VPX[0x00], .y = VPX[0x01] },
                                { .x = VPX[0x02], .y = VPX[0x01] },
                                { .x = VPX[0x02], .y = VPX[0x03] },
                                { .x = VPX[0x00], .y = VPX[0x03] } };
    Q3DVECTOR3D scr3Dnear[0x04];
    Q3DVECTOR3D scr3Dfar[0x04];

    double xDouble, yDouble, zDouble;
    int i;

    memset ( qzen->frustrum, 0x00, sizeof ( qzen->frustrum ) );

    for ( i = 0x00; i < 0x04; i++ ) {
        gluUnProject ( ( double ) scr2D[i].x,
                       ( double ) scr2D[i].y,
                       ( double ) 0.0f, /* zNear */
                       MVX,
                       PJX,
                       VPX,
                      &scr3Dnear[i].x,
                      &scr3Dnear[i].y,
                      &scr3Dnear[i].z );

        gluUnProject ( ( double ) scr2D[i].x,
                       ( double ) scr2D[i].y,
                       ( double ) 0.4f, /* zFar */
                       MVX,
                       PJX,
                       VPX,
                      &scr3Dfar[i].x,
                      &scr3Dfar[i].y,
                      &scr3Dfar[i].z );
    }

    gluUnProject ( 0.0f,
                   0.0f,
                   0.0f, /* zNear */
                   MVX,
                   PJX,
                   VPX,
                  &xDouble,
                  &yDouble,
                  &zDouble );

    qzen->frustrum[0x00].z = -1.0f;
    qzen->frustrum[0x00].w = zDouble;

    /*pick->frustrum[0x01].z =  1.0f;
    pick->frustrum[0x01].w =  pick->zFar;*/
}

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

            if ( ( y >= VPX[0x01] ) && ( y < VPX[0x03] ) ) {
                if ( qzen->hlines[y].inited == 0x00 ) {
                    qzen->hlines[y].inited = 0x01;

                    qzen->hlines[y].p1.x = qzen->hlines[y].p2.x = x;
                    qzen->hlines[y].p1.z = qzen->hlines[y].p2.z = z;
                } else {
                    if ( x < qzen->hlines[y].p1.x ) {
                        qzen->hlines[y].p1.x = x;
                        qzen->hlines[y].p1.z = z;
                    }

                    if ( x > qzen->hlines[y].p2.x ) {
                        qzen->hlines[y].p2.x = x;
                        qzen->hlines[y].p2.z = z;
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

            if ( ( y >= VPX[0x01] ) && ( y < VPX[0x03] ) ) {
                if ( qzen->hlines[y].inited == 0x00 ) {
                     qzen->hlines[y].inited = 0x01;

                    qzen->hlines[y].p1.x = qzen->hlines[y].p2.x = x;
                    qzen->hlines[y].p1.z = qzen->hlines[y].p2.z = z;
                } else {
                    if ( x < qzen->hlines[y].p1.x ) {
                        qzen->hlines[y].p1.x = x;
                        qzen->hlines[y].p1.z = z;
                    }

                    if ( x > qzen->hlines[y].p2.x ) {
                        qzen->hlines[y].p2.x = x;
                        qzen->hlines[y].p2.z = z;
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
                                   uint32_t    qobjID,
                                   uint32_t    qtriID,
                                   int         VPX[0x04],
                                   int32_t     y ) {
    Q3DZHLINE *hline = &qzen->hlines[y];
    int32_t x1 = hline->p1.x, 
            x2 = hline->p2.x;
    float   z1 = hline->p1.z;
    int32_t dx = x2 - x1, ddx = ( dx == 0x00 ) ? 0x01 : abs ( dx );
    int32_t x = x1;
    double dz  = hline->p2.z - hline->p1.z, pz = ( dz / ddx );
    long  px = ( dx > 0x00 ) ? 1 : -1;
    double z = z1;
    int i;
    uint32_t offset = ( ( VPX[0x03] - 1 - y ) * VPX[0x02] );

    for ( i = 0x00; i < ddx; i++ ) {
        /*** add some epsilon against Z fghting ***/
        float depth = qzen->buffer[offset+x].z;

        if ( ( x >= VPX[0x00] ) && 
             ( x <  VPX[0x02] ) ) {
            if  ( z <= depth ) {
                qzen->buffer[offset+x].z      = z;
                qzen->buffer[offset+x].qobjID = qobjID;
                qzen->buffer[offset+x].qtriID = qtriID;
            }
        }

        x += px;
        z  += pz;
    }
}

/******************************************************************************/
static void q3dzengine_drawTriangle ( Q3DZENGINE  *qzen,
                                      uint32_t     qobjID,
                                      uint32_t     qtriID,
                                      Q3DTRIANGLE *qtri,
                                      Q3DVERTEX   *qver,
                                      double      *MVX,
                                      double      *PJX,
                                      int         *VPX ) {
    static double IDX[0x10] = { 1.0f, 0.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, 0.0f, 1.0f };
    uint32_t qverID0 = qtri->qverID[0x00],
             qverID1 = qtri->qverID[0x01],
             qverID2 = qtri->qverID[0x02];
    int32_t xmin = INT32_MAX,
            xmax = INT32_MIN,
            ymin = INT32_MAX,
            ymax = INT32_MIN,
            bymin, 
            bymax;
    Q3DVECTOR3F  pworld[0x03];
    Q3DVECTOR3F  lworld[0x03][0x02]; /*** line points ***/
    Q3DVECTOR3D lscreen[0x03][0x02];
    Q3DVECTOR3D pclip[0x02]; /*** clipping points ***/
    uint32_t nodraw = 0xFFFFFFFF;
    uint32_t nbClip = 0x00;
    uint32_t i, j;

    /*** the clipping planes are in world coordinates, so we work in world ***/
    /*** coordinates. That's why we need the identity matrix further below ***/
    q3dvector3f_matrix ( &qver[qverID0].pos, MVX, &pworld[0x00] );
    q3dvector3f_matrix ( &qver[qverID1].pos, MVX, &pworld[0x01] );
    q3dvector3f_matrix ( &qver[qverID2].pos, MVX, &pworld[0x02] );

    /*** Clip first, or else GluProject will return unwanted values ***/
    for ( i = 0x00; i < 0x03; i++ ) {
        uint32_t n = ( i + 0x01 ) % 0x03;
        Q3DVECTOR3F *p1 = &pworld[i],
                    *p2 = &pworld[n];
        float distance = FLT_MAX;
        float s1, s2;

        memcpy ( &lworld[i][0x00], p1, sizeof ( Q3DVECTOR3F ) );
        memcpy ( &lworld[i][0x01], p2, sizeof ( Q3DVECTOR3F ) );

        for ( j = 0x00; j < CLIPPINGPLANES; j++ ) {
            /* Check if both points are inside the frustrum */
            s1 = ( qzen->frustrum[j].x * p1->x ) +
                 ( qzen->frustrum[j].y * p1->y ) +
                 ( qzen->frustrum[j].z * p1->z ) + qzen->frustrum[j].w,
            s2 = ( qzen->frustrum[j].x * p2->x ) +
                 ( qzen->frustrum[j].y * p2->y ) +
                 ( qzen->frustrum[j].z * p2->z ) + qzen->frustrum[j].w;

            /*** line lies entirely on the right side of the plane ***/
            if ( ( s1 > 0.0f ) && ( s2 > 0.0f ) ) {
                nodraw &= (~( 1 << i ));
            }

            if ( ( s1 * s2 ) < 0.0f ) {
                Q3DVECTOR3F it; /*** intersection ***/
                float t;

                nodraw &= (~( 1 << i ));

                /*** if p1 is on the outside of the frustrum, it becomes p2 ***/
                if ( s1 < 0.0f ) {
                    memcpy ( &lworld[i][0x00], p2, sizeof ( Q3DVECTOR3F ) );
                    memcpy ( &lworld[i][0x01], p1, sizeof ( Q3DVECTOR3F ) );
                }

                t = q3dplane_intersectSegment ( &qzen->frustrum[j],
                                                &lworld[i][0x00],
                                                &lworld[i][0x01],
                                                &it );

                if (  ( t > 0.0f ) && ( t < distance ) ) {
                    /*** overwrite the outside point ***/
                    memcpy ( &lworld[i][0x01], &it, sizeof ( Q3DVECTOR3F ) );

                    /*** remember clipping point position for ***/
                    /*** drawing the clipping line ***/
                    gluProject ( it.x, 
                                 it.y,
                                 it.z,
                                 IDX, 
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
                         IDX,
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

    if ( bymin != bymax ) {
        memset ( qzen->hlines + ymin, 0x00, sizeof ( Q3DZHLINE ) * ( bymax - 
                                                                     bymin - 1 ) );

        for ( i = 0x00; i < 0x03; i++ ) {
            if ( ( nodraw & ( 1 << i ) ) == 0x00 ) {
                uint32_t n = ( i + 0x01 ) % 0x03;
                Q3DZPOINT pt1 = { .x = lscreen[i][0x00].x,
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
            Q3DZPOINT pt1 = { .x = pclip[0x00].x,
                              .y = pclip[0x00].y,
                              .z = pclip[0x00].z },
                      pt2 = { .x = pclip[0x01].x,
                              .y = pclip[0x01].y,
                              .z = pclip[0x01].z };

            if ( pt1.x < pt2.x ) q3dzengine_line ( qzen, VPX, &pt1, &pt2 );
            else                 q3dzengine_line ( qzen, VPX, &pt2, &pt1 );
        }

        /*** Much faster if we restrict to useful Y-axis boundaries ***/
        for ( i = /*VPX[0x01]*/bymin; i < /*VPX[0x03]*/bymax; i++ ) {
            if ( qzen->hlines[i].inited == 0x02 ){
                q3dzengine_fillHLine ( qzen, qobjID, qtriID, VPX, i );
            }
        }
    }
}

/******************************************************************************/
static void q3dzengine_drawSymmetry ( Q3DZENGINE  *qzen, 
                                      Q3DSYMMETRY *qsym,
                                      double      *MVX,
                                      double      *PJX,
                                      int         *VPX,
                                      float        frame ) {
    Q3DOBJECT *qobj = ( Q3DOBJECT * ) qsym;
    LIST *ltmpchildren = qobj->lchildren;
    G3DSYMMETRY *sym  = ( G3DSYMMETRY * ) q3dobject_getObject ( qobj );
    double SMVX[0x10];

    g3dcore_multmatrix ( sym->smatrix, MVX, SMVX );

    while ( ltmpchildren ) {
        Q3DOBJECT *qchild = ( Q3DOBJECT * ) ltmpchildren->data;

        q3dzengine_drawObject_r ( qzen, 
                                  qchild,
                                  SMVX,
                                  PJX,
                                  VPX,
                                  frame );

        ltmpchildren = ltmpchildren->next;
    }
}

/******************************************************************************/
static void q3dzengine_drawMesh ( Q3DZENGINE *qzen, 
                                  Q3DMESH    *qmes,
                                  double     *MVX,
                                  double     *PJX,
                                  int        *VPX,
                                  float       frame ) {
    Q3DVERTEX *qver = q3dmesh_getVertices ( qmes, frame );

    if ( qver ) {
        uint32_t i;

        for ( i = 0x00; i < qmes->nbqtri; i++ ) {
            q3dzengine_drawTriangle ( qzen,
                                      q3dobject_getID ( ( Q3DOBJECT * ) qmes ),
                                      i,
                                     &qmes->qtri[i],
                                      qver,
                                      MVX,
                                      PJX,
                                      VPX );
        }
    }
}

/******************************************************************************/
void q3dzengine_drawObject_r ( Q3DZENGINE *qzen, 
                               Q3DOBJECT  *qobj,
                               double     *MVX,
                               double     *PJX,
                               int        *VPX,
                               float       frame ) {
    LIST *ltmpchildren = qobj->lchildren;
    double WMVX[0x10];

    g3dcore_multmatrix ( qobj->obj->lmatrix, MVX, WMVX );

    if ( qobj->obj->type & MESH ) {
        Q3DMESH *qmes = ( Q3DMESH * ) qobj;

        q3dzengine_drawMesh ( qzen, 
                              qmes,
                              WMVX,
                              PJX,
                              VPX,
                              frame );
    }

    if ( qobj->obj->type == G3DSYMMETRYTYPE ) {
        Q3DSYMMETRY *qsym = ( Q3DSYMMETRY * ) qobj;

        q3dzengine_drawSymmetry ( qzen, 
                                  qsym,
                                  WMVX,
                                  PJX,
                                  VPX,
                                  frame );
    }

    while ( ltmpchildren ) {
        Q3DOBJECT *qchild = ( Q3DOBJECT * ) ltmpchildren->data;

        q3dzengine_drawObject_r ( qzen,
                                  qchild,
                                  WMVX,
                                  PJX,
                                  VPX,
                                  frame );

        ltmpchildren = ltmpchildren->next;
    }
}

/******************************************************************************/
void q3dzengine_reset ( Q3DZENGINE *qzen ) {
    free ( qzen->buffer );
    free ( qzen->hlines );
}

/******************************************************************************/
void q3dzengine_init ( Q3DZENGINE *qzen,
                       double     *MVX,
                       double     *PJX,
                       int        *VPX,
                       uint32_t    width,
                       uint32_t    height ) {
    uint32_t i;

    if ( width && height ) {
        qzen->buffer = ( Q3DZBUFFER * ) calloc ( height *
                                                 width, sizeof ( Q3DZBUFFER ) );

        for ( i = 0x00; i < height * width; i++ ) {
            qzen->buffer[i].z = INFINITY;
        }

        qzen->hlines = ( Q3DZHLINE * ) calloc ( height, sizeof ( Q3DZHLINE ) );

        q3dzengine_buildFrustrum ( qzen, MVX, PJX, VPX );
    }
}
