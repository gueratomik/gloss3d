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

#define VOXTREESIZE  0x40
#define VOXTREEDEPTH 0x02

#define VOXEL_CELL_FACES   0x04
#define VOXEL_IS_EMPTY     0x00
#define VOXEL_HAS_1_FACE   0x01
#define VOXEL_HAS_2_FACES  0x02
#define VOXEL_HAS_3_FACES  0x03
#define VOXEL_HAS_4_FACES  0x04
#define VOXEL_HAS_VOXTREE  0xFF

#define MARKXY  ( 1 << 0 )
#define MARKYZ  ( 1 << 1 )
#define MARKZX  ( 1 << 2 )
#define MARKALL ( MARKXY | MARKYZ | MARKZX )

typedef struct _R3DVOXTREE R3DVOXTREE;

/******************************************************************************/
typedef struct _R3DUVPOINT {
   int32_t u, v;
   float w;
} R3DUVPOINT;

/******************************************************************************/
typedef struct _R3DUVHLINE {
    uint32_t inited;
    float u1, u2;
    float w1, w2;
} R3DUVHLINE;

/******************************************************************************/
typedef struct _R3DVOXEL {
    uint32_t type;
    union {
        uint32_t    rfcID[VOXEL_CELL_FACES];
        R3DVOXTREE *voxtree;
    } cell;
} R3DVOXEL;

/******************************************************************************/
struct _R3DVOXTREE {
    uint32_t depth;
    float xmin;
    float xmax;
    float ymin;
    float ymax;
    float zmin;
    float zmax;
    R3DVOXEL voxel[VOXTREESIZE][VOXTREESIZE][VOXTREESIZE];
};

/******************************************************************************/
typedef struct _G3DVOXTREE {
    G3DOBJECT obj;
    R3DVOXTREE *voxtree;
} G3DVOXTREE;

R3DVOXTREE *r3dvoxtree_new ( uint32_t, float xmin, float xmax,
                             float ymin, float ymax,
                             float zmin, float zmax );

void r3dvoxtree_importFace ( R3DVOXTREE *vxt,
                             R3DVERTEX  *rver,
                             R3DFACE    *rfc, 
                             uint32_t    rfcID );

/*
ax + by + cz + d = 0
cz = -d - ax - by
z = ( -d - ax - by ) / c
*/

/******************************************************************************/
void r3duvharray_init ( R3DUVHLINE  *hlines,
                        R3DUVPOINT *srcPoint,
                        R3DUVPOINT *dstPoint ) {
    int32_t du  = ( dstPoint->u - srcPoint->u ),
            ddu = ( du == 0x00 ) ? 0x01 : abs ( du ),
            dv  = ( dstPoint->v - srcPoint->v ),
            ddv = ( dv == 0x00 ) ? 0x01 : abs ( dv ),
            dd  = ( ddu > ddv ) ? ddu : ddv;
    float dw  = ( dstPoint->w - srcPoint->w );
    float stepw = ( float ) dw / dd;
    int pu = ( du > 0x00 ) ? 1 : -1, 
        pv = ( dv > 0x00 ) ? 1 : -1;
    int32_t u = srcPoint->u, 
            v = srcPoint->v;
    float   w = srcPoint->w;
    int i, cumul = 0x00;

    if ( ddu > ddv ) {
        for ( i = 0x00; i <= ddu; i++ ) {
            if ( cumul >= ddu ) {
                cumul -= ddu;
                v     += pv;
            }

            if ( ( v >= 0 ) && ( v < VOXTREESIZE ) ) {
                if ( hlines[v].inited == 0x00 ) {
                    hlines[v].inited = 0x01;

                    hlines[v].u1 = hlines[v].u2 = u;
                    hlines[v].w1 = hlines[v].w2 = w;
                } else {
                    if ( u < hlines[v].u1 ) {
                        hlines[v].u1 = u;
                        hlines[v].w1 = w;
                    }

                    if ( u > hlines[v].u2 ) {
                        hlines[v].u2 = u;
                        hlines[v].w2 = w;
                    }

                    hlines[v].inited = 0x02;
        	    }
            }

            cumul += ddv;
            u     += pu;
            w     += stepw;
        }
    } else {
        for ( i = 0x00; i <= ddv; i++ ) {
            if ( cumul >= ddv ) {
                cumul -= ddv;
                u     += pu;
            }

            if ( ( v >= 0 ) && ( v < VOXTREESIZE ) ) {
                if ( hlines[v].inited == 0x00 ) {
                    hlines[v].inited = 0x01;

                    hlines[v].u1 = hlines[v].u2 = u;
                    hlines[v].w1 = hlines[v].w2 = w;
                } else {
                    if ( u < hlines[v].u1 ) {
                        hlines[v].u1 = u;
                        hlines[v].w1 = w;
                    }

                    if ( u > hlines[v].u2 ) {
                        hlines[v].u2 = u;
                        hlines[v].w2 = w;
                    }

                    hlines[v].inited = 0x02;
                }
            }

            cumul += ddu;
            v     += pv;
            w     += stepw;
        }
    }
}

/******************************************************************************/
void r3dvoxtree_fillCell ( R3DVOXTREE *vxt,
                           uint32_t    x, 
                           uint32_t    y, 
                           uint32_t    z,
                           uint32_t    rfcID ) {
    
    vxt->voxel[x][y][z].type = VOXEL_HAS_1_FACE;
    vxt->voxel[x][y][z].cell.rfcID[0] = rfcID;
}

/******************************************************************************/
void r3dvoxtree_fill ( R3DVOXTREE *vxt,
                       R3DUVHLINE  harray[0x03][VOXTREESIZE],
                       float xdiff, float ydiff, float zdiff, 
                       uint32_t    rfcID ) {
    uint32_t i, x, y, z;
    static int tmp[VOXTREESIZE][VOXTREESIZE][VOXTREESIZE] = { 0 };

    memset ( tmp, 0x00, sizeof ( tmp ) );

    for ( i = 0x00; i < 0x03; i++ ) {
        switch ( i ) {
            case 0x00 : { /* XY */
                for ( y = 0x00; y < VOXTREESIZE; y++ ) {
                    if ( harray[i][y].inited == 0x02 ) {
                        int32_t ddu = ( harray[i][y].u2 - harray[i][y].u1 );
                        float   ddw = ( harray[i][y].w2 - harray[i][y].w1 );
                        float stepz = ( float ) ddw / ddu;
                        float zf = harray[i][y].w1;
/*
printf("%d u1:%f u2:%f w1:%f w2:%f\n", y, harray[i][y].u1, harray[i][y].u2, harray[i][y].w1, harray[i][y].w2 );
*/
                        for ( x = harray[i][y].u1; x <= harray[i][y].u2; x++ ) {
                            z = zf / zdiff * VOXTREESIZE;
/*
printf("%d\n", z );
*/
                            tmp[x][y][z] |= MARKXY;

                            if ( tmp[x][y][z] & MARKXY ) {
                                vxt->voxel[x][y][z].type = VOXEL_HAS_1_FACE;
                                vxt->voxel[x][y][z].cell.rfcID[0] = rfcID;
                            }

                            zf += stepz;
                        }
                    }
                }
            } break;

            case 0x01 : { /* YZ */
                for ( z = 0x00; z < VOXTREESIZE; z++ ) {
                    if ( harray[i][z].inited == 0x02 ) {
                        int32_t ddu = ( harray[i][z].u2 - harray[i][z].u1 );
                        float   ddw = ( harray[i][z].w2 - harray[i][z].w1 );
                        float stepx = ( float ) ddw / ddu;
                        float xf = harray[i][z].w1;
/*
printf("%d u1:%f u2:%f w1:%f w2:%f\n", y, harray[i][y].u1, harray[i][y].u2, harray[i][y].w1, harray[i][y].w2 );
*/
                        for ( y = harray[i][z].u1; y <= harray[i][z].u2; y++ ) {
                            x = xf / xdiff * VOXTREESIZE;
/*
printf("%d\n", z );
*/
                            tmp[x][y][z] |= MARKYZ;

                            if ( tmp[x][y][z] & MARKYZ ) {
                                vxt->voxel[x][y][z].type = VOXEL_HAS_1_FACE;
                                vxt->voxel[x][y][z].cell.rfcID[0] = rfcID;
                            }

                            xf += stepx;
                        }
                    }
                }
            } break;

            case 0x02 : { /* ZX */
                for ( x = 0x00; x < VOXTREESIZE; x++ ) {
                    if ( harray[i][x].inited == 0x02 ) {
                        int32_t ddu = ( harray[i][x].u2 - harray[i][x].u1 );
                        float   ddw = ( harray[i][x].w2 - harray[i][x].w1 );
                        float stepy = ( float ) ddw / ddu;
                        float yf = harray[i][x].w1;
/*
printf("%d u1:%f u2:%f w1:%f w2:%f\n", y, harray[i][y].u1, harray[i][y].u2, harray[i][y].w1, harray[i][y].w2 );
*/
                        for ( z = harray[i][x].u1; z <= harray[i][x].u2; z++ ) {
                            y = yf / ydiff * VOXTREESIZE;
/*
printf("%d\n", z );
*/
                            tmp[x][y][z] |= MARKZX;

                            if ( tmp[x][y][z] & MARKZX ) {
                                vxt->voxel[x][y][z].type = VOXEL_HAS_1_FACE;
                                vxt->voxel[x][y][z].cell.rfcID[0] = rfcID;
                            }

                            yf += stepy;
                        }
                    }
                }
            } break;

            default :

            break;
        }
    }
}

/******************************************************************************/
void r3dvoxtree_importFace ( R3DVOXTREE *vxt,
                             R3DVERTEX  *rver,
                             R3DFACE    *rfc, 
                             uint32_t    rfcID ) {
    float xdiff = ( vxt->xmax - vxt->xmin ),
          ydiff = ( vxt->ymax - vxt->ymin ),
          zdiff = ( vxt->zmax - vxt->zmin );
    float xscale = 1.0f / xdiff * VOXTREESIZE,
          yscale = 1.0f / ydiff * VOXTREESIZE,
          zscale = 1.0f / zdiff * VOXTREESIZE;
    R3DVECTOR pt[0x03] = { { .x = ( rver[rfc[rfcID].rverID[0]].pos.x - vxt->xmin ),
                             .y = ( rver[rfc[rfcID].rverID[0]].pos.y - vxt->ymin ),
                             .z = ( rver[rfc[rfcID].rverID[0]].pos.z - vxt->zmin ) },
                           { .x = ( rver[rfc[rfcID].rverID[1]].pos.x - vxt->xmin ),
                             .y = ( rver[rfc[rfcID].rverID[1]].pos.y - vxt->ymin ),
                             .z = ( rver[rfc[rfcID].rverID[1]].pos.z - vxt->zmin ) },
                           { .x = ( rver[rfc[rfcID].rverID[2]].pos.x - vxt->xmin ),
                             .y = ( rver[rfc[rfcID].rverID[2]].pos.y - vxt->ymin ),
                             .z = ( rver[rfc[rfcID].rverID[2]].pos.z - vxt->zmin ) } };
    static R3DUVHLINE harray[0x03][VOXTREESIZE];
    R3DUVPOINT srcPoint, dstPoint;
    uint32_t i, j;

    memset ( harray, 0x00, sizeof ( harray ) );

    for ( i = 0x00; i < 0x03; i++ ) {
        for ( j = 0x00; j < 0x03; j++ ) {
            uint32_t n = ( j + 0x01 ) % 0x03;

            switch ( i ) {
                case 0x00 : {
                    srcPoint.u = pt[j].x * xscale;
                    srcPoint.v = pt[j].y * yscale;
                    srcPoint.w = pt[j].z;
                    dstPoint.u = pt[n].x * xscale;
                    dstPoint.v = pt[n].y * yscale;
                    dstPoint.w = pt[n].z;
                } break;

                case 0x01 : {
                    srcPoint.u = pt[j].y * yscale;
                    srcPoint.v = pt[j].z * zscale;
                    srcPoint.w = pt[j].x;
                    dstPoint.u = pt[n].y * yscale;
                    dstPoint.v = pt[n].z * zscale;
                    dstPoint.w = pt[n].x;
                } break;

                case 0x02 : {
                    srcPoint.u = pt[j].z * zscale;
                    srcPoint.v = pt[j].x * xscale;
                    srcPoint.w = pt[j].y;
                    dstPoint.u = pt[n].z * zscale;
                    dstPoint.v = pt[n].x * xscale;
                    dstPoint.w = pt[n].y;
                } break;
            }

            r3duvharray_init ( harray[i], &srcPoint, &dstPoint );
        }
    }

    r3dvoxtree_fill ( vxt, harray, xdiff, ydiff, zdiff, rfcID );
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
uint32_t r3dvoxtree_draw ( R3DVOXTREE *vxt, 
                           G3DCAMERA  *curcam, 
                           uint32_t    flags ) {
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
                int voxel_has_voxtree = 0, voxel_has_faces = 0;

                switch ( voxel->type ) {
                    case VOXEL_HAS_1_FACE :
                        glColor3ub ( voxel->cell.rfcID[0] * 32,
                                     voxel->cell.rfcID[0] * 32,
                                     voxel->cell.rfcID[0] * 32 );

                        voxel_has_faces = 1;
                    break;

                    case VOXEL_HAS_2_FACES :
                        glColor3ub ( 0xFF, 0x00, 0x00 );

                        voxel_has_faces = 1;
                    break;

                    case VOXEL_IS_EMPTY :
                    break;

                    case VOXEL_HAS_VOXTREE :
                        voxel_has_voxtree = 1;
                    break;

                    default :
                    break;
                }

                if ( voxel_has_voxtree ) {
                    R3DVOXTREE *voxtree = voxel->cell.voxtree;

                    r3dvoxtree_draw ( voxtree, curcam, flags );

                    /*glBegin ( GL_LINES );
printf("%f %f %f\n", voxtree->xmin, voxtree->ymin, voxtree->zmin );
                    glVertex3f ( voxtree->xmin, voxtree->ymin, voxtree->zmin );
                    glVertex3f ( voxtree->xmax, voxtree->ymax, voxtree->zmax );
                    glEnd ( );*/
                }

                if ( voxel_has_faces ) {
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

    return 0x00;
}

/******************************************************************************/
uint32_t g3dvoxtree_draw ( G3DOBJECT *obj, 
                           G3DCAMERA *curcam, 
                           uint32_t flags ) {
    G3DVOXTREE *vxt = ( G3DVOXTREE * ) obj;

    r3dvoxtree_draw ( vxt->voxtree, curcam, flags );

    return 0x00;
}


/******************************************************************************/
R3DVOXTREE *r3dvoxtree_new ( uint32_t depth,
                             float xmin, float xmax,
                             float ymin, float ymax,
                             float zmin, float zmax ) {
    R3DVOXTREE *vxt = ( R3DVOXTREE * ) calloc ( 0x01, sizeof ( R3DVOXTREE ) );
static int p;
    if ( vxt == NULL ) {
        fprintf ( stderr, "r3dvoxtree_new: calloc failed\n" );

        return NULL;
    }
printf("%d\n", ++p * sizeof ( R3DVOXTREE ) );
    vxt->depth = depth;
    vxt->xmin = xmin;
    vxt->xmax = xmax;
    vxt->ymin = ymin;
    vxt->ymax = ymax;
    vxt->zmin = zmin;
    vxt->zmax = zmax;

    return vxt;
}

/******************************************************************************/
void g3dvoxtree_importFace ( G3DVOXTREE *vxt,
                             R3DVERTEX  *rver,
                             R3DFACE    *rfc, 
                             uint32_t    rfcID ) {
    r3dvoxtree_importFace ( vxt->voxtree, rver, rfc, rfcID );
}

/******************************************************************************/
G3DVOXTREE *g3dvoxtree_new ( uint32_t depth,
                             float xmin, float xmax,
                             float ymin, float ymax,
                             float zmin, float zmax ) {
    G3DVOXTREE *vxt = ( G3DVOXTREE * ) calloc ( 0x01, sizeof ( G3DVOXTREE ) );

    if ( vxt == NULL ) {
        fprintf ( stderr, "g3dvoxtree_new: calloc failed\n" );

        return NULL;
    }

    vxt->voxtree = r3dvoxtree_new ( depth, xmin, xmax,
                                           ymin, ymax, 
                                           zmin, zmax );

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

