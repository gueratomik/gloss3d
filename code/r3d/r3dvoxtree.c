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

#define VOXTREESIZE  0x08
#define VOXTREEDEPTH 0x02

#define VOXEL_HAS_1_FACE  ( 1 << 0 )
#define VOXEL_HAS_2_FACES ( 1 << 1 )
#define VOXEL_HAS_VOXTREE ( 1 << 2 )
#define VOXEL_HAS_LIST    ( 1 << 3 )

typedef struct _R3DVOXTREE R3DVOXTREE;

/******************************************************************************/
typedef struct _R3DVOXPOINT {
    int32_t u, v;
    float w;
} R3DVOXPOINT;

/******************************************************************************/
typedef struct _R3DVOXHLINE {
    uint32_t inited;
    int32_t u1, u2;
    float w1, w2;
} R3DVOXHLINE;

/******************************************************************************/
typedef struct _R3DVOXEL {
    uint16_t flags;
    uint16_t nbrfc;
    union {
        uint32_t rfcID[0x02];
        R3DVOXTREE *voxtree;
    } cell;
} R3DVOXEL;

/******************************************************************************/
struct _R3DVOXTREE {
    G3DOBJECT obj;
    uint32_t depth;
    float xmin;
    float xmax;
    float ymin;
    float ymax;
    float zmin;
    float zmax;
    R3DVOXEL voxel[VOXTREESIZE][VOXTREESIZE][VOXTREESIZE];
    R3DVOXHLINE hlines[VOXTREESIZE];
};

R3DVOXTREE *r3dvoxtree_new ( uint32_t, float xmin, float xmax,
                             float ymin, float ymax,
                             float zmin, float zmax );

void r3dvoxtree_importFace ( R3DVOXTREE *vxt,
                             R3DVERTEX  *rver,
                             R3DFACE    *rfc, 
                             uint32_t    rfcID );

void r3dvoxtree_importFaceSinglePlane ( R3DVOXTREE *vxt,
                                        R3DVERTEX  *rver,
                                        R3DFACE    *rfc, 
                                        uint32_t    rfcID,
                                        uint32_t    plane );

/******************************************************************************/
void r3dvoxtree_fillHLine ( R3DVOXTREE  *vxt,
                            R3DVOXPOINT *srcPoint,
                            R3DVOXPOINT *dstPoint ) {
    int32_t du  = ( dstPoint->u - srcPoint->u ),
            ddu = ( du == 0x00 ) ? 0x01 : abs ( du ),
            dv  = ( dstPoint->v - srcPoint->v ),
            ddv = ( dv == 0x00 ) ? 0x01 : abs ( dv ),
            dd  = ( ddu > ddv ) ? ddu : ddv;
    float dw  = dstPoint->w  - srcPoint->w, pw = ( float ) dw / dd;
    int pu = ( du > 0x00 ) ? 1 : -1, 
        pv = ( dv > 0x00 ) ? 1 : -1;
    int32_t u = srcPoint->u, 
            v = srcPoint->v;
    float w = srcPoint->w;
    int i, cumul = 0x00;

    if ( ddu > ddv ) {
        for ( i = 0x00; i <= ddu; i++ ) {
            if ( cumul >= ddu ) {
                cumul -= ddu;
                v     += pv;
            }

            if ( ( v >= 0 ) && ( v < VOXTREESIZE ) ) {
                if ( vxt->hlines[v].inited == 0x00 ) {
                    vxt->hlines[v].inited = 0x01;

                    vxt->hlines[v].u1 = vxt->hlines[v].u2 = u;
                    vxt->hlines[v].w1 = vxt->hlines[v].w2 = w;
                } else {
                    if ( u < vxt->hlines[v].u1 ) {
                        vxt->hlines[v].u1 = u;
                        vxt->hlines[v].w1 = w;
                    }

                    if ( u > vxt->hlines[v].u2 ) {
                        vxt->hlines[v].u2 = u;
                        vxt->hlines[v].w2 = w;
                    }

                    vxt->hlines[v].inited = 0x02;
        	    }
            }

            cumul += ddv;
            u     += pu;

            w     += pw;
        }
    } else {
        for ( i = 0x00; i <= ddv; i++ ) {
            if ( cumul >= ddv ) {
                cumul -= ddv;
                u     += pu;
            }

            if ( ( v >= 0 ) && ( v < VOXTREESIZE ) ) {
                if ( vxt->hlines[v].inited == 0x00 ) {
                    vxt->hlines[v].inited = 0x01;

                    vxt->hlines[v].u1 = vxt->hlines[v].u2 = u;
                    vxt->hlines[v].w1 = vxt->hlines[v].w2 = w;
                } else {
                    if ( u < vxt->hlines[v].u1 ) {
                        vxt->hlines[v].u1 = u;
                        vxt->hlines[v].w1 = w;
                    }

                    if ( u > vxt->hlines[v].u2 ) {
                        vxt->hlines[v].u2 = u;
                        vxt->hlines[v].w2 = w;
                    }

                    vxt->hlines[v].inited = 0x02;
                }
            }

            cumul += ddu;
            v     += pv;

            w     += pw;
        }
    }
}

/****************************************************************************/
void r3dvoxtree_interpolateHLine ( R3DVOXTREE *vxt,
                                   R3DVERTEX  *rver,
                                   R3DFACE    *rfc,
                                   uint32_t    rfcID,
                                   int32_t     v,
                                   int32_t     plane ) {
    R3DVOXHLINE *hline = &vxt->hlines[v];
    int32_t u1 = hline->u1, u2 = hline->u2;
    int32_t w1 = hline->w1, w2 = hline->w2;
    int32_t du = u2 - u1, ddu = ( du == 0x00 ) ? 0x01 : abs ( du );
    float dw = w2 - w1, pw = ( float ) dw / ddu;
    long  pu = ( du > 0x00 ) ? 1 : -1;
    float w = w1;
    int i;

    for ( i = 0x00; i < ddu; i++ ) {
        if ( u1 >= 0 && u1 < VOXTREESIZE ) {
            if ( w >= 0 && w < VOXTREESIZE ) {
                float xmin, ymin, zmin, xmax, ymax, zmax;
                float xstep = ( vxt->xmax - vxt->xmin ) / VOXTREESIZE,
                      ystep = ( vxt->ymax - vxt->ymin ) / VOXTREESIZE,
                      zstep = ( vxt->zmax - vxt->zmin ) / VOXTREESIZE;
                R3DVOXEL *voxel = NULL;

                switch ( plane ) {
                    case 0x00 : /* XY */
                        voxel = &vxt->voxel[u1][v][(int)w];

                        xmin = ((int)u1) * xstep;
                        ymin = ((int)v ) * ystep;
                        zmin = ((int)w ) * zstep;
                    break;

                    case 0x01 : /* YZ */
                        voxel = &vxt->voxel[(int)w][u1][v];

                        xmin = ((int)w ) * xstep;
                        ymin = ((int)u1) * ystep;
                        zmin = ((int)v ) * zstep;
                    break;

                    case 0x02 : /* ZX */
                        voxel = &vxt->voxel[v][(int)w][u1];

                        xmin = ((int)v ) * xstep;
                        ymin = ((int)w ) * ystep;
                        zmin = ((int)u1) * zstep;
                    break;

                    default :
                    break;
                }

                if ( voxel ) {
                    if ( voxel->flags == VOXEL_HAS_2_FACES ) {
                        if ( vxt->depth < VOXTREEDEPTH ) {
                            R3DVOXTREE *voxtree = r3dvoxtree_new ( vxt->depth + 1,
                                                                   xmin, xmin + xstep,
                                                                   ymin, ymin + ystep,
                                                                   zmin, zmin + zstep );
printf("%f %f %f %f %f %f\n", xmin, ymin, zmin, xmin + xstep, ymin + ystep, zmin + zstep);
                            r3dvoxtree_importFace ( voxtree,
                                                    rver,
                                                    rfc,
                                                    voxel->cell.rfcID[0] );

                            r3dvoxtree_importFace ( voxtree,
                                                    rver,
                                                    rfc,
                                                    voxel->cell.rfcID[1] );

                            /* voxel already has 2 faces, subdivide */
                            voxel->cell.voxtree = voxtree;

                            voxel->flags = VOXEL_HAS_VOXTREE;
                        }
                    }

                    if ( voxel->flags == VOXEL_HAS_VOXTREE ) {
                        r3dvoxtree_importFaceSinglePlane ( voxel->cell.voxtree, 
                                                           rver,
                                                           rfc,
                                                           rfcID,
                                                           plane );
                    }

                    if ( voxel->flags == VOXEL_HAS_1_FACE ) {
                        /* Face mybe drawm multiple times at the same voxel */
                        /* because it is drawn on each 2D plane (XY,YZ, ZX) */
                        if ( voxel->cell.rfcID[0x00] != rfcID ) {
                            voxel->cell.rfcID[0x01] = rfcID;

                            voxel->flags = VOXEL_HAS_2_FACES;
                        }
                    }

                    if ( voxel->flags == 0x00 ) {
                        voxel->cell.rfcID[0x00] = rfcID;

                        voxel->flags = VOXEL_HAS_1_FACE;
                    }
                }
            }
        }

        u1 += pu;

        w  += pw;
    }
}

/******************************************************************************/
void r3dvoxtree_importFaceSinglePlane ( R3DVOXTREE *vxt,
                                        R3DVERTEX  *rver,
                                        R3DFACE    *rfc, 
                                        uint32_t    rfcID,
                                        uint32_t    plane ) {
    float xdiff = ( vxt->xmax - vxt->xmin ),
          ydiff = ( vxt->ymax - vxt->ymin ),
          zdiff = ( vxt->zmax - vxt->zmin );
    R3DINTVECTOR pt[0x03] = { { .x = ( ( rver[rfc[rfcID].rverID[0]].pos.x - vxt->xmin ) / xdiff ) * VOXTREESIZE,
                                .y = ( ( rver[rfc[rfcID].rverID[0]].pos.y - vxt->ymin ) / ydiff ) * VOXTREESIZE,
                                .z = ( ( rver[rfc[rfcID].rverID[0]].pos.z - vxt->zmin ) / zdiff ) * VOXTREESIZE },
                              { .x = ( ( rver[rfc[rfcID].rverID[1]].pos.x - vxt->xmin ) / xdiff ) * VOXTREESIZE,
                                .y = ( ( rver[rfc[rfcID].rverID[1]].pos.y - vxt->ymin ) / ydiff ) * VOXTREESIZE,
                                .z = ( ( rver[rfc[rfcID].rverID[1]].pos.z - vxt->zmin ) / zdiff ) * VOXTREESIZE },
                              { .x = ( ( rver[rfc[rfcID].rverID[2]].pos.x - vxt->xmin ) / xdiff ) * VOXTREESIZE,
                                .y = ( ( rver[rfc[rfcID].rverID[2]].pos.y - vxt->ymin ) / ydiff ) * VOXTREESIZE,
                                .z = ( ( rver[rfc[rfcID].rverID[2]].pos.z - vxt->zmin ) / zdiff ) * VOXTREESIZE } };
    R3DVOXPOINT srcPoint, dstPoint;
    uint32_t i;

    memset ( vxt->hlines, 0x00, sizeof ( vxt->hlines ) );

    for ( i = 0x00; i < 0x03; i++ ) {
        uint32_t n = ( i + 0x01 ) % 0x03;

        if ( plane == 0x00 ) {
           srcPoint.u = pt[i].x;
           srcPoint.v = pt[i].y;
           srcPoint.w = pt[i].z;

           dstPoint.u = pt[n].x;
           dstPoint.v = pt[n].y;
           dstPoint.w = pt[n].z;
        }

        if ( plane == 0x01 ) {
           srcPoint.u = pt[i].y;
           srcPoint.v = pt[i].z;
           srcPoint.w = pt[i].x;

           dstPoint.u = pt[n].y;
           dstPoint.v = pt[n].z;
           dstPoint.w = pt[n].x;
        }

        if ( plane == 0x02 ) {
           srcPoint.u = pt[i].z;
           srcPoint.v = pt[i].x;
           srcPoint.w = pt[i].y;

           dstPoint.u = pt[n].z;
           dstPoint.v = pt[n].x;
           dstPoint.w = pt[n].y;
        }

        r3dvoxtree_fillHLine ( vxt, &srcPoint, &dstPoint );
    }

    for ( i = 0x00; i < VOXTREESIZE; i++ ) {
        if ( vxt->hlines[i].inited == 0x02 ){
            r3dvoxtree_interpolateHLine ( vxt, rver, rfc, rfcID, i, plane );
        }
    }
}

/******************************************************************************/
void r3dvoxtree_importFace ( R3DVOXTREE *vxt,
                             R3DVERTEX  *rver,
                             R3DFACE    *rfc, 
                             uint32_t    rfcID ) {
    uint32_t j;

    for ( j = 0x00; j < /*0x03*/0x03; j++ ) {
        r3dvoxtree_importFaceSinglePlane ( vxt, rver, rfc, rfcID, j );
    }
}

/******************************************************************************/
static void drawCube ( float xmin, float ymin, float zmin,
                       float xmax, float ymax, float zmax ) {
    R3DVECTOR pt[0x08] = { { .x = xmin, .y = ymin, .z = zmin },
                           { .x = xmax, .y = ymin, .z = zmin },
                           { .x = xmin, .y = ymax, .z = zmin },
                           { .x = xmax, .y = ymax, .z = zmin },
                           { .x = xmin, .y = ymin, .z = zmax },
                           { .x = xmax, .y = ymin, .z = zmax },
                           { .x = xmin, .y = ymax, .z = zmax },
                           { .x = xmax, .y = ymax, .z = zmax } };
    uint32_t idx[0x06][0x04] = { { 0, 1, 3, 2 },
                                 { 4, 5, 7, 6 },
                                 { 0, 1, 5, 4 },
                                 { 2, 3, 7, 6 },
                                 { 0, 4, 6, 2 },
                                 { 1, 5, 7, 3 } };
    R3DVECTOR nor[0x06] = { { .x =  0.0f, .y =  0.0f, .z = -1.0f },
                            { .x =  0.0f, .y =  0.0f, .z =  1.0f },
                            { .x =  0.0f, .y = -1.0f, .z =  0.0f },
                            { .x =  0.0f, .y =  1.0f, .z =  0.0f },
                            { .x = -1.0f, .y =  0.0f, .z =  0.0f },
                            { .x =  1.0f, .y =  0.0f, .z =  0.0f } };
    uint32_t i;

    glBegin ( GL_QUADS );
    for ( i = 0x00; i < 0x06; i++ ) {
        glNormal3f ( nor[i].x, nor[i].y, nor[i].z );
        glVertex3f ( pt[idx[i][0x00]].x, pt[idx[i][0x00]].y, pt[idx[i][0x00]].z );
        glVertex3f ( pt[idx[i][0x01]].x, pt[idx[i][0x01]].y, pt[idx[i][0x01]].z );
        glVertex3f ( pt[idx[i][0x02]].x, pt[idx[i][0x02]].y, pt[idx[i][0x02]].z );
        glVertex3f ( pt[idx[i][0x03]].x, pt[idx[i][0x03]].y, pt[idx[i][0x03]].z );
    }
    glEnd ( );
}

/******************************************************************************/
uint32_t g3dvoxtree_draw ( G3DOBJECT *obj, 
                           G3DCAMERA *curcam, 
                           uint32_t flags ) {
    R3DVOXTREE *vxt = ( R3DVOXTREE * ) obj;
    float xdiff = ( vxt->xmax - vxt->xmin ),
          ydiff = ( vxt->ymax - vxt->ymin ),
          zdiff = ( vxt->zmax - vxt->zmin );
    float xstep = xdiff / VOXTREESIZE,
          ystep = ydiff / VOXTREESIZE,
          zstep = zdiff / VOXTREESIZE;
    uint32_t i, j, k;


    for ( i = 0x00; i < VOXTREESIZE; i++ ) {
        for ( j = 0x00; j < VOXTREESIZE; j++ ) {
            for ( k = 0x00; k < VOXTREESIZE; k++ ) {
                R3DVOXEL *voxel = &vxt->voxel[i][j][k];

                if ( voxel->flags ) {
                    if ( voxel->flags == VOXEL_HAS_VOXTREE ) {
                        g3dvoxtree_draw ( voxel->cell.voxtree, curcam, flags );
                    }

                    if ( voxel->flags == VOXEL_HAS_1_FACE ) {
                        glColor3ub ( voxel->cell.rfcID[0] * 32,
                                     voxel->cell.rfcID[0] * 32,
                                     voxel->cell.rfcID[0] * 32 );
                    }

                    if ( voxel->flags == VOXEL_HAS_2_FACES ) {
                        glColor3ub ( 0xFF, 0x00, 0x00 );
                    }

                    if ( ( voxel->flags == VOXEL_HAS_1_FACE  ) ||
                         ( voxel->flags == VOXEL_HAS_2_FACES ) ) {
                        drawCube ( vxt->xmin + ( (i+0) * xstep ),
                                   vxt->ymin + ( (j+0) * ystep ),
                                   vxt->zmin + ( (k+0) * zstep ),
                                   vxt->xmin + ( (i+1) * xstep ),
                                   vxt->ymin + ( (j+1) * ystep ),
                                   vxt->zmin + ( (k+1) * zstep ) );
                    }
                }
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
R3DVOXTREE *r3dvoxtree_new ( uint32_t depth,
                             float xmin, float xmax,
                             float ymin, float ymax,
                             float zmin, float zmax ) {
    R3DVOXTREE *vxt = ( R3DVOXTREE * ) calloc ( 0x01, sizeof ( R3DVOXTREE ) );

    if ( vxt == NULL ) {
        fprintf ( stderr, "r3dvoxtree_new: calloc failed\n" );

        return NULL;
    }

    vxt->depth = depth;
    vxt->xmin = xmin;
    vxt->xmax = xmax;
    vxt->ymin = ymin;
    vxt->ymax = ymax;
    vxt->zmin = zmin;
    vxt->zmax = zmax;

    g3dobject_init ( &vxt->obj, G3DOBJECTTYPE, 0x00, "Vox", DRAWBEFORECHILDREN,
                                                   g3dvoxtree_draw,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL );

    return vxt;
}

