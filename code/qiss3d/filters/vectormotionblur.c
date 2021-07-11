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
typedef struct _VMBZPIXEL {
    uint32_t vobjID;
    uint32_t vtriID;
    float    z;
} VMBZPIXEL;

/******************************************************************************/
typedef struct _VMBZPOINT {
    int32_t x;
    int32_t y;
    float   z;
    float ratio[0x03]; /*** possible improvement for lower memory footprint :**/
                       /*** use unsigned chars for ratios ***/
} VMBZPOINT;

/******************************************************************************/
typedef struct _VMBZHLINE {
    VMBZPOINT p1;
    VMBZPOINT p2;
    int       inited;
} VMBZHLINE;

/******************************************************************************/
typedef struct _VMBOBJECT {
    Q3DOBJECT *qobj;
    uint32_t   id;
} VMBOBJECT;

/******************************************************************************/
typedef struct _VMBVECTOR2S1D {
    int16_t x;
    int16_t y;
    float   z;
} VMBVECTOR2S1D;

/******************************************************************************/
typedef struct _VMBVECTOR2S {
    int16_t x;
    int16_t y;
} VMBVECTOR2S;

/******************************************************************************/
typedef struct _VMBTRIANGLE {
    VMBVECTOR2S pnt[0x03];
    float       surface;
} VMBTRIANGLE;

/******************************************************************************/
typedef struct _VMBMESH {
    VMBOBJECT    vobj;
    VMBTRIANGLE *vtri;    /*** triangles on frame ***/
    float      (*zval)[0x03];
    VMBTRIANGLE *vtribef; /*** triangles before frame ***/
    VMBTRIANGLE *vtriaft; /*** triangles after frame ***/
} VMBMESH;

/******************************************************************************/
typedef struct _FILTERVMB {
    LIST          *lvobj;
    uint32_t       vobjID;
    float          strength;
    uint32_t       nbSamples;
    float          subSamplingRate;
    VMBZPIXEL     *zbuffer;
    uint32_t       height;
    uint32_t       width;
    uint32_t       bpp;
    unsigned char *img;
    VMBZHLINE     *hlines;
    uint32_t     (*abuffer)[0x04]; /**** accumulation buffer ***/
} FILTERVMB;

/******************************************************************************/
static void filtervmb_fillAbuffer ( Q3DFILTER    *qfil,
                                    VMBMESH      *vmes,
                                    uint32_t      vtriID,
                                    int32_t       y,
                                    float         strength );

static void filtervmb_import_r    ( Q3DFILTER *qfil, 
                                    Q3DCAMERA *qcam,
                                    Q3DOBJECT *qobj,
                                    double    *MVX,
                                    int32_t    subframeID,
                                    float      frame );

/******************************************************************************/
static VMBOBJECT *filtervmb_getObjectbyID ( Q3DFILTER *fil, 
                                            uint32_t   id ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) fil->data;
    LIST *ltmpvobj = fvmb->lvobj;

    while ( ltmpvobj ) {
        VMBOBJECT *vobj = ( VMBOBJECT * ) ltmpvobj->data;

        if ( vobj->id == id ) return vobj;

        ltmpvobj = ltmpvobj->next;
    }

    return NULL;
}

/******************************************************************************/
static void filtervmb_merge ( Q3DFILTER     *qfil,
                              unsigned char *img,
                              uint32_t       bpp  ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    uint32_t i, j;

    for ( i = 0x00; i < fvmb->height; i++ ) {
        for ( j = 0x00; j < fvmb->width; j++ ) {
            switch ( bpp ) {
                case 0x18 : {
                    uint32_t offset = ( i * fvmb->width ) + j;
                    unsigned char (*srcimg)[0x03] = img;

                    if ( fvmb->abuffer[offset][0x03] ) {
                        unsigned char a0 = fvmb->abuffer[offset][0x00] / fvmb->abuffer[offset][0x03],
                                      a1 = fvmb->abuffer[offset][0x01] / fvmb->abuffer[offset][0x03],
                                      a2 = fvmb->abuffer[offset][0x02] / fvmb->abuffer[offset][0x03];

                        srcimg[offset][0x00] = ( ( 1.0f - fvmb->strength ) * srcimg[offset][0x00] ) + ( fvmb->strength * a0 );
                        srcimg[offset][0x01] = ( ( 1.0f - fvmb->strength ) * srcimg[offset][0x01] ) + ( fvmb->strength * a1 );
                        srcimg[offset][0x02] = ( ( 1.0f - fvmb->strength ) * srcimg[offset][0x02] ) + ( fvmb->strength * a2 );
                    }
                } break;

                default :
                break;
            }
        }
    }
}

/******************************************************************************/
static void filtervmb_line ( Q3DFILTER *qfil,
                             VMBZPOINT *srcPoint,
                             VMBZPOINT *dstPoint ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
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
    float ratio[0x03] = { srcPoint->ratio[0x00],
                          srcPoint->ratio[0x01],
                          srcPoint->ratio[0x02] };
    float dratio[0x03] = { dstPoint->ratio[0x00] - srcPoint->ratio[0x00],
                           dstPoint->ratio[0x01] - srcPoint->ratio[0x01],
                           dstPoint->ratio[0x02] - srcPoint->ratio[0x02] };
    float pratio[0x03] = { ( dratio[0x00] / (dd+1) ),
                           ( dratio[0x01] / (dd+1) ),
                           ( dratio[0x02] / (dd+1) ) };
    int i, cumul = 0x00;

    if ( ddx > ddy ) {
        for ( i = 0x00; i <= ddx; i++ ) {
            if ( cumul >= ddx ) {
                cumul -= ddx;
                y     += py;
            }

            if ( ( y >= 0x00 ) && ( y < fvmb->height ) ) {
                uint32_t offset = ( y * fvmb->width ) + x;

                if ( fvmb->hlines[y].inited == 0x00 ) {
                    fvmb->hlines[y].inited = 0x01;

                    fvmb->hlines[y].p1.x = fvmb->hlines[y].p2.x = x;
                    fvmb->hlines[y].p1.z = fvmb->hlines[y].p2.z = z;

                    fvmb->hlines[y].p1.ratio[0] = fvmb->hlines[y].p2.ratio[0] = ratio[0];
                    fvmb->hlines[y].p1.ratio[1] = fvmb->hlines[y].p2.ratio[1] = ratio[1];
                    fvmb->hlines[y].p1.ratio[2] = fvmb->hlines[y].p2.ratio[2] = ratio[2];
                } else {
                    if ( x < fvmb->hlines[y].p1.x ) {
                        fvmb->hlines[y].p1.x = x;
                        fvmb->hlines[y].p1.z = z;

                        fvmb->hlines[y].p1.ratio[0] = ratio[0];
                        fvmb->hlines[y].p1.ratio[1] = ratio[1];
                        fvmb->hlines[y].p1.ratio[2] = ratio[2];
                    }

                    if ( x > fvmb->hlines[y].p2.x ) {
                        fvmb->hlines[y].p2.x = x;
                        fvmb->hlines[y].p2.z = z;

                        fvmb->hlines[y].p2.ratio[0] = ratio[0];
                        fvmb->hlines[y].p2.ratio[1] = ratio[1];
                        fvmb->hlines[y].p2.ratio[2] = ratio[2];
                    }

                    fvmb->hlines[y].inited = 0x02;
                }
            }

            cumul += ddy;
            x     += px;
            z     += pz;
            ratio[0x00] += pratio[0x00];
            ratio[0x01] += pratio[0x01];
            ratio[0x02] += pratio[0x02];
        }
    } else {
        for ( i = 0x00; i <= ddy; i++ ) {
            if ( cumul >= ddy ) {
                    cumul -= ddy;
                    x     += px;
            }

            if ( ( y >= 0x00 ) && ( y < fvmb->height ) ) {
                uint32_t offset = ( y * fvmb->width ) + x;

                if ( fvmb->hlines[y].inited == 0x00 ) {
                     fvmb->hlines[y].inited = 0x01;

                    fvmb->hlines[y].p1.x = fvmb->hlines[y].p2.x = x;
                    fvmb->hlines[y].p1.z = fvmb->hlines[y].p2.z = z;

                    fvmb->hlines[y].p1.ratio[0] = fvmb->hlines[y].p2.ratio[0] = ratio[0];
                    fvmb->hlines[y].p1.ratio[1] = fvmb->hlines[y].p2.ratio[1] = ratio[1];
                    fvmb->hlines[y].p1.ratio[2] = fvmb->hlines[y].p2.ratio[2] = ratio[2];
                } else {
                    if ( x < fvmb->hlines[y].p1.x ) {
                        fvmb->hlines[y].p1.x = x;
                        fvmb->hlines[y].p1.z = z;

                        fvmb->hlines[y].p1.ratio[0] = ratio[0];
                        fvmb->hlines[y].p1.ratio[1] = ratio[1];
                        fvmb->hlines[y].p1.ratio[2] = ratio[2];
                    }

                    if ( x > fvmb->hlines[y].p2.x ) {
                        fvmb->hlines[y].p2.x = x;
                        fvmb->hlines[y].p2.z = z;

                        fvmb->hlines[y].p2.ratio[0] = ratio[0];
                        fvmb->hlines[y].p2.ratio[1] = ratio[1];
                        fvmb->hlines[y].p2.ratio[2] = ratio[2];
                    }

                    fvmb->hlines[y].inited = 0x02;
                }
            }

            cumul += ddx;
            y     += py;
            z     += pz;
            ratio[0x00] += pratio[0x00];
            ratio[0x01] += pratio[0x01];
            ratio[0x02] += pratio[0x02];
        }
    }
}

/****************************************************************************/
static void filtervmb_fillZbuffer ( Q3DFILTER *qfil,
                                    VMBMESH   *vmes,
                                    uint32_t   vtriID,
                                    int32_t    y ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    VMBZHLINE *hline = &fvmb->hlines[y];
    int32_t x1 = hline->p1.x, 
            x2 = hline->p2.x;
    float   z1 = hline->p1.z;
    int32_t dx = x2 - x1, ddx = ( dx == 0x00 ) ? 0x01 : abs ( dx );
    int32_t x = x1;
    float dz  = hline->p2.z - hline->p1.z, pz = ( dz / ddx );
    long  px = ( dx > 0x00 ) ? 1 : -1;
    float z = z1;
    int i;
    uint32_t offset = ( y * fvmb->width );

    for ( i = 0x00; i <= ddx; i++ ) {
        uint32_t xoffset = ( offset + x );
        float depth = fvmb->zbuffer[xoffset].z;

        if ( ( x >= 0x00 ) && 
             ( x <  fvmb->width ) ) {

            if  ( z <= depth ) {
                fvmb->zbuffer[xoffset].z      = z;
                fvmb->zbuffer[xoffset].vobjID = vmes->vobj.id;
                fvmb->zbuffer[xoffset].vtriID = vtriID;
            }
        }

        x += px;
        z += pz;
    }
}

/****************************************************************************/
static void filtervmb_fillAbuffer ( Q3DFILTER    *qfil,
                                    VMBMESH      *vmes,
                                    uint32_t      vtriID,
                                    int32_t       y,
                                    float         strength ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    VMBZHLINE *hline = &fvmb->hlines[y];
    int32_t x1 = hline->p1.x, 
            x2 = hline->p2.x;
    float   z1 = hline->p1.z;
    int32_t dx = x2 - x1, ddx = ( dx == 0x00 ) ? 0x01 : abs ( dx );
    int32_t x = x1;
    float dz  = hline->p2.z - hline->p1.z, pz = ( dz / ddx );
    long  px = ( dx > 0x00 ) ? 1 : -1;
    float z = z1;
    float ratio[0x03] = { hline->p1.ratio[0x00],
                          hline->p1.ratio[0x01],
                          hline->p1.ratio[0x02] };
    float dratio[0x03] = { hline->p2.ratio[0x00] - hline->p1.ratio[0x00],
                           hline->p2.ratio[0x01] - hline->p1.ratio[0x01],
                           hline->p2.ratio[0x02] - hline->p1.ratio[0x02] };
    float pratio[0x03] = { ( dratio[0x00] / (ddx) ),
                           ( dratio[0x01] / (ddx) ),
                           ( dratio[0x02] / (ddx) ) };

    int i;
    uint32_t offset = ( y * fvmb->width );

    for ( i = 0x00; i <= ddx; i++ ) {
        if ( ( x >= 0x00 ) && 
             ( x <  fvmb->width ) ) {

            /** check if triangle is outside screen space ***/
            if ( ( vmes->zval[vtriID][0x00] != INFINITY ) &&
                 ( vmes->zval[vtriID][0x01] != INFINITY ) &&
                 ( vmes->zval[vtriID][0x02] != INFINITY ) ) {

                int32_t xsrc = ( vmes->vtri[vtriID].pnt[0x00].x * ratio[0x00] +
                                 vmes->vtri[vtriID].pnt[0x01].x * ratio[0x01] +
                                 vmes->vtri[vtriID].pnt[0x02].x * ratio[0x02] ),
                        ysrc = ( vmes->vtri[vtriID].pnt[0x00].y * ratio[0x00] +
                                 vmes->vtri[vtriID].pnt[0x01].y * ratio[0x01] +
                                 vmes->vtri[vtriID].pnt[0x02].y * ratio[0x02] );

                if ( ( ysrc >= 0x00         ) && 
                     ( ysrc <  fvmb->height ) && 
                     ( xsrc >= 0x00         ) &&
                     ( xsrc <  fvmb->width  ) ) {
                    uint32_t aoffset = offset + x;
                    uint32_t zoffset = ( ysrc * fvmb->width ) + xsrc;
                    unsigned char R, G, B;

                    if ( ( fvmb->zbuffer[zoffset].vobjID == vmes->vobj.id ) &&
                         ( fvmb->zbuffer[zoffset].vtriID == vtriID        ) ) {


                        switch ( fvmb->bpp ) {
                            case 0x18 : {
                                unsigned char (*srcimg)[0x03] = fvmb->img;

                                R = srcimg[zoffset][0x00];
                                G = srcimg[zoffset][0x01];
                                B = srcimg[zoffset][0x02];

/*printf("%d %d %d %d %d\n", ysrc, xsrc, R, G, B);*/
                                /*srcimg[zoffset][0x00] = 0xFF;
                                srcimg[zoffset][0x01] = 0xFF;
                                srcimg[zoffset][0x02] = 0xFF;*/
                            } break;

                            default :
                            break;
                        }

                        fvmb->abuffer[aoffset][0x00] += ( R * strength );
                        fvmb->abuffer[aoffset][0x01] += ( G * strength );
                        fvmb->abuffer[aoffset][0x02] += ( B * strength );
                        fvmb->abuffer[aoffset][0x03]++;

/*
                        fvmb->abuffer[aoffset][0x00] = ( ( 1.0f - strength ) * fvmb->abuffer[aoffset][0x00] ) + ( R * strength );
                        fvmb->abuffer[aoffset][0x01] = ( ( 1.0f - strength ) * fvmb->abuffer[aoffset][0x01] ) + ( G * strength );
                        fvmb->abuffer[aoffset][0x02] = ( ( 1.0f - strength ) * fvmb->abuffer[aoffset][0x02] ) + ( B * strength );
*/
                    }
                }
            }
        }

        x += px;
        z += pz;
        ratio[0] += pratio[0x00];
        ratio[1] += pratio[0x01];
        ratio[2] += pratio[0x02];
    }
}

/******************************************************************************/
static void filtervmb_drawTriangle ( Q3DFILTER   *qfil,
                                     VMBMESH     *vmes,
                                     uint32_t     vtriID,
                                     VMBTRIANGLE *vtri,
                                     float        zval[0x03],
                                     float        strength ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    int32_t ymin = vtri->pnt[0x00].y,
            ymax = ymin;
    uint32_t i;

    for ( i = 0x00; i < 0x03; i++ ) {
        uint32_t n = ( i + 0x01 ) % 0x03;
        VMBZPOINT pt1 = { .x = vtri->pnt[i].x,
                          .y = vtri->pnt[i].y,
                          .z = ( zval ) ? zval[i] : 0.0f,
                          .ratio[0x00] = ( i == 0x00 ) ? 1.0f : 0.0f,
                          .ratio[0x01] = ( i == 0x01 ) ? 1.0f : 0.0f,
                          .ratio[0x02] = ( i == 0x02 ) ? 1.0f : 0.0f },
                  pt2 = { .x = vtri->pnt[n].x,
                          .y = vtri->pnt[n].y,
                          .z = ( zval ) ? zval[n] : 0.0f,
                          .ratio[0x00] = ( n == 0x00 ) ? 1.0f : 0.0f,
                          .ratio[0x01] = ( n == 0x01 ) ? 1.0f : 0.0f,
                          .ratio[0x02] = ( n == 0x02 ) ? 1.0f : 0.0f };

        if ( vtri->pnt[i].y < ymin ) ymin = vtri->pnt[i].y;
        if ( vtri->pnt[i].y > ymax ) ymax = vtri->pnt[i].y;

        if ( pt1.x < pt2.x ) filtervmb_line ( qfil, &pt1, &pt2 );
        else                 filtervmb_line ( qfil, &pt2, &pt1 );
    }

    if ( ymin <  0x00         ) ymin = 0x00;
    if ( ymin >= fvmb->height ) ymin = fvmb->height - 1;
    if ( ymax <  0x00         ) ymax = 0x00;
    if ( ymax >= fvmb->height ) ymax = fvmb->height - 1;

    if ( ymin < ymax ) {
        for ( i = ymin; i < ymax; i++ ) {
            if ( fvmb->hlines[i].inited == 0x02 ){

                if ( zval ) {
                    filtervmb_fillZbuffer ( qfil, vmes, vtriID, i );
                } else {
                    filtervmb_fillAbuffer ( qfil, vmes, vtriID, i, strength );
                }
            }
        }

        memset ( fvmb->hlines + ymin, 0x00, sizeof ( VMBZHLINE ) * ( ymax - 
                                                                     ymin ) );
    }
}

/******************************************************************************/
static void filtervmb_drawMesh ( Q3DFILTER *qfil,
                                 VMBMESH   *vmes ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    Q3DMESH *qmes = ( Q3DMESH * ) vmes->vobj.qobj;
    uint32_t vobjID = vmes->vobj.id;
    uint32_t i, j;

    for ( i = 0x00; i < qmes->nbqtri; i++ ) {
        uint32_t vtriID = i;

        /*** Draw and fill the Z-Buffer ***/
        filtervmb_drawTriangle ( qfil,
                                 vmes,
                                 vtriID,
                                &vmes->vtri[i],
                                 vmes->zval[i],
                                 0.0f );
    }
}

/******************************************************************************/
static void filtervmb_drawObjects ( Q3DFILTER *qfil ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    LIST *ltmpvobj = fvmb->lvobj;

    while ( ltmpvobj ) {
        VMBOBJECT *vobj = ( VMBOBJECT * ) ltmpvobj->data;
        VMBMESH *vmes = ( VMBMESH * ) vobj;

        filtervmb_drawMesh ( qfil, vmes );

        ltmpvobj = ltmpvobj->next;
    }
}

/******************************************************************************/
static void filtervmb_drawInterpolatedMesh ( Q3DFILTER *qfil,
                                             VMBMESH   *vmes ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    Q3DMESH *qmes = ( Q3DMESH * ) vmes->vobj.qobj;
    uint32_t vobjID = vmes->vobj.id;
    uint32_t i, p;

    for ( p = 0x00; p < fvmb->nbSamples; p++ ) {
        VMBTRIANGLE *dstTri = vmes->vtribef + ( qmes->nbqtri * p ),
                    *srcTri = vmes->vtri;

        for ( i = 0x00; i < qmes->nbqtri; i++ ) {
            uint32_t vtriID = i;
            G2DVECTOR mvec[0x03] = { { .x = dstTri[i].pnt[0x00].x - 
                                            srcTri[i].pnt[0x00].x,
                                       .y = dstTri[i].pnt[0x00].y - 
                                            srcTri[i].pnt[0x00].y },
                                     { .x = dstTri[i].pnt[0x01].x - 
                                            srcTri[i].pnt[0x01].x,
                                       .y = dstTri[i].pnt[0x01].y - 
                                            srcTri[i].pnt[0x01].y },
                                     { .x = dstTri[i].pnt[0x02].x - 
                                            srcTri[i].pnt[0x02].x,
                                       .y = dstTri[i].pnt[0x02].y - 
                                            srcTri[i].pnt[0x02].y } };
            uint32_t len[0x03] = { g2dvector_length ( &mvec[0x00] ), 
                                   g2dvector_length ( &mvec[0x01] ),
                                   g2dvector_length ( &mvec[0x02] ) };
            /*** We draw as many triangles as the maximum length between source ***/
            /*** and destination triangles (if subSamplingRate is 100%). So we ***/
            /*** first have to get the maximum length and then we compute the ***/
            /*** number of iterations from this length and subSamplingRate. ***/
            uint32_t maxLen = ( len[0] > len[1] ) ? 
                            ( ( len[0] > len[2] ) ? len[0] : len[2] ) :
                            ( ( len[1] > len[2] ) ? len[1] : len[2] );
            uint32_t iter = maxLen * fvmb->subSamplingRate;

            if ( maxLen ) {
                uint32_t j;

                for ( j = 0x00; j < iter; j++ ) {
                    float pos = ( float ) j / iter;
                    VMBTRIANGLE itrTri = { .pnt[0x00].x = srcTri[i].pnt[0].x + ( mvec[0].x * pos ),
                                           .pnt[0x00].y = srcTri[i].pnt[0].y + ( mvec[0].y * pos ),
                                           .pnt[0x01].x = srcTri[i].pnt[1].x + ( mvec[1].x * pos ),
                                           .pnt[0x01].y = srcTri[i].pnt[1].y + ( mvec[1].y * pos ),
                                           .pnt[0x02].x = srcTri[i].pnt[2].x + ( mvec[2].x * pos ),
                                           .pnt[0x02].y = srcTri[i].pnt[2].y + ( mvec[2].y * pos ) };
                    float opacity = 1.0f - pos;

                    filtervmb_drawTriangle ( qfil,
                                             vmes,
                                             vtriID,
                                            &itrTri,
                                             NULL,
                                             opacity );
                }
            }
        }
    }
}

/******************************************************************************/
static void filtervmb_drawInterpolatedObjects ( Q3DFILTER *qfil ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    LIST *ltmpvobj = fvmb->lvobj;

    while ( ltmpvobj ) {
        VMBOBJECT *vobj = ( VMBOBJECT * ) ltmpvobj->data;
        VMBMESH *vmes = ( VMBMESH * ) vobj;

        filtervmb_drawInterpolatedMesh ( qfil, vmes );

        ltmpvobj = ltmpvobj->next;
    }
}

/******************************************************************************/
static void vmbtriangle_init ( VMBTRIANGLE *vtri,
                               float       *zval,
                               Q3DTRIANGLE *qtri,
                               Q3DVERTEX   *qver,
                               double      *MVX,
                               Q3DCAMERA   *qcam ) {
    Q3DVECTOR3D screenmax = { .x = FLT_MIN, .y = FLT_MIN, .z = FLT_MIN },
                screenmin = { .x = FLT_MAX, .y = FLT_MAX, .z = FLT_MAX };
    Q3DVECTOR3D screenpos[0x03];
    uint32_t i;

    for ( i = 0x00; i < 0x03; i++ ) {
        gluProject ( qver[qtri->qverID[i]].pos.x,
                     qver[qtri->qverID[i]].pos.y,
                     qver[qtri->qverID[i]].pos.z,
                     MVX,
                     qcam->PJX,
                     qcam->VPX,
                    &screenpos[i].x,
                    &screenpos[i].y,
                    &screenpos[i].z );

        if ( screenpos[i].x < screenmin.x ) screenmin.x = screenpos[i].x;
        if ( screenpos[i].y < screenmin.y ) screenmin.y = screenpos[i].y;
        if ( screenpos[i].x > screenmax.x ) screenmax.x = screenpos[i].x;
        if ( screenpos[i].y > screenmax.y ) screenmax.y = screenpos[i].y;
    }

    if ( ( screenmin.x >= qcam->VPX[0x02] ) ||
         ( screenmin.y >= qcam->VPX[0x03] ) ||
         ( screenmax.x <  qcam->VPX[0x00] ) ||
         ( screenmax.y <  qcam->VPX[0x01] ) ) {
        if ( zval ) {
            zval[0x00] = INFINITY;
            zval[0x01] = INFINITY;
            zval[0x02] = INFINITY;
        }
    } else {
        Q3DVECTOR3D v0v1 = { screenpos[0x01].x - screenpos[0x00].x,
                             screenpos[0x01].y - screenpos[0x00].y, 0.0f },
                    v1v2 = { screenpos[0x02].x - screenpos[0x01].x,
                             screenpos[0x02].y - screenpos[0x01].y, 0.0f },
                    vout;

               vtri->pnt[0x00].x = screenpos[0x00].x;
               vtri->pnt[0x00].y = qcam->VPX[0x03] - 1 - screenpos[0x00].y;
        if ( zval ) zval[0x00]   = screenpos[0x00].z;

               vtri->pnt[0x01].x = screenpos[0x01].x;
               vtri->pnt[0x01].y = qcam->VPX[0x03] - 1 - screenpos[0x01].y;
        if ( zval ) zval[0x01]   = screenpos[0x01].z;

               vtri->pnt[0x02].x = screenpos[0x02].x;
               vtri->pnt[0x02].y = qcam->VPX[0x03] - 1 - screenpos[0x02].y;
        if ( zval ) zval[0x02]   = screenpos[0x02].z;

        q3dvector3d_cross  ( &v0v1, &v1v2, &vout );

        vtri->surface = q3dvector3d_length ( &vout );
    }
}

/******************************************************************************/
static VMBMESH *vmbmesh_free ( VMBMESH *vmes ) {
    free ( vmes->vtribef );
    free ( vmes->vtriaft );
    free ( vmes->vtri    );
    free ( vmes );
}

/******************************************************************************/
static void vmbmesh_import ( VMBMESH   *vmes,
                             Q3DMESH   *qmes,
                             Q3DCAMERA *qcam,
                             double    *MVX,
                             int32_t    subframeID,
                             float      frame ) {
    Q3DVERTEXSET *qverset = q3dmesh_getVertexSet ( qmes, frame );
    uint32_t triSize = sizeof ( VMBTRIANGLE ),
             fltSize = sizeof ( float ) * 0x03;
    Q3DVERTEX *qver = qverset->qver;
    uint32_t i;

    if ( subframeID == 0x00 ) {
        for ( i = 0x00; i < qmes->nbqtri; i++ ) {
            vmbtriangle_init ( &vmes->vtri[i],
                                vmes->zval[i],
                               &qmes->qtri[i], 
                                qver,
                                MVX,
                                qcam );
        }
    }
    
    /*** fill iterations happening before the current frame ***/
    if ( subframeID < 0x00 ) {
        uint32_t page = abs ( subframeID ) - 0x01;
        VMBTRIANGLE *vtri = vmes->vtribef + ( qmes->nbqtri * page );

        for ( i = 0x00; i < qmes->nbqtri; i++ ) {
            vmbtriangle_init ( &vtri[i],
                                NULL,
                               &qmes->qtri[i], 
                                qver,
                                MVX,
                                qcam );
        }
    }

    /*** fill iterations happening after the current frame ***/
    if ( subframeID > 0x00 ) {
        uint32_t page = abs ( subframeID ) - 0x01;
        VMBTRIANGLE *vtri = vmes->vtriaft + ( qmes->nbqtri * page );

        for ( i = 0x00; i < qmes->nbqtri; i++ ) {
            vmbtriangle_init ( &vtri[i],
                                NULL,
                               &qmes->qtri[i], 
                                qver,
                                MVX,
                                qcam );
        }
    }
}

/******************************************************************************/
static VMBMESH *vmbmesh_new ( Q3DMESH   *qmes,
                              uint32_t   id,
                              Q3DCAMERA *qcam,
                              double    *MVX,
                              float      frame,
                              uint32_t   nbSamples ) {
    VMBMESH *vmes = NULL;

    if ( qmes->nbqtri ) {
        Q3DVERTEXSET *qverset = q3dmesh_getVertexSet ( qmes, frame );
        Q3DVECTOR3F coord[0x08] = { { .x = qverset->min.x, .y = qverset->min.y, .z = qverset->min.z }, 
                                    { .x = qverset->min.x, .y = qverset->min.y, .z = qverset->max.z },
                                    { .x = qverset->min.x, .y = qverset->max.y, .z = qverset->min.z }, 
                                    { .x = qverset->min.x, .y = qverset->max.y, .z = qverset->max.z },
                                    { .x = qverset->max.x, .y = qverset->min.y, .z = qverset->min.z }, 
                                    { .x = qverset->max.x, .y = qverset->min.y, .z = qverset->max.z },
                                    { .x = qverset->max.x, .y = qverset->max.y, .z = qverset->min.z }, 
                                    { .x = qverset->max.x, .y = qverset->max.y, .z = qverset->max.z } };
        Q3DVECTOR3D screenmax = { .x = FLT_MIN, .y = FLT_MIN, .z = FLT_MIN },
                    screenmin = { .x = FLT_MAX, .y = FLT_MAX, .z = FLT_MAX };
        uint32_t i;

        for ( i = 0x00; i < 0x08; i++ ) {
            Q3DVECTOR3D screenpos;

            gluProject ( coord[i].x,
                         coord[i].y,
                         coord[i].z,
                         MVX,
                         qcam->PJX,
                         qcam->VPX,
                        &screenpos.x,
                        &screenpos.y,
                        &screenpos.z );

            if ( screenpos.x < screenmin.x ) screenmin.x = screenpos.x;
            if ( screenpos.y < screenmin.y ) screenmin.y = screenpos.y;
            if ( screenpos.x > screenmax.x ) screenmax.x = screenpos.x;
            if ( screenpos.y > screenmax.y ) screenmax.y = screenpos.y;
        }

        if ( ( screenmin.x >= qcam->VPX[0x02] ) ||
             ( screenmin.y >= qcam->VPX[0x03] ) ||
             ( screenmax.x <  qcam->VPX[0x00] ) ||
             ( screenmax.y <  qcam->VPX[0x01] ) ) {
            return NULL;
        } else {
            vmes = ( VMBMESH * ) calloc ( 0x01, sizeof ( VMBMESH ) );
 
            if ( vmes == NULL ) {
                fprintf ( stderr, "%s: calloc failed\n", __func__);

                return NULL;
            } else {
                uint32_t triSize = sizeof ( VMBTRIANGLE ),
                         fltSize = sizeof ( float ) * 0x03;

                vmes->vtri = ( VMBTRIANGLE * ) calloc ( qmes->nbqtri, triSize );
                vmes->zval = ( float       * ) calloc ( qmes->nbqtri, fltSize );

                vmes->vtribef = ( VMBTRIANGLE * ) calloc ( qmes->nbqtri * nbSamples, triSize );
                vmes->vtriaft = ( VMBTRIANGLE * ) calloc ( qmes->nbqtri * nbSamples, triSize );
            }
        }
    }

    vmes->vobj.id = id;
    vmes->vobj.qobj = qmes;

    vmes->zval = ( float * ) calloc ( qmes->nbqtri, sizeof ( float ) * 0x03 );

    return vmes;
}

/******************************************************************************/
static void filtervmb_importSymmetry ( Q3DFILTER   *qfil, 
                                       Q3DCAMERA   *qcam,
                                       Q3DSYMMETRY *qsym,
                                       double      *MVX,
                                       int32_t      subframeID,
                                       float        frame ) {
    Q3DOBJECT *qobj = ( Q3DOBJECT * ) qsym;
    LIST *ltmpchildren = qobj->lchildren;
    G3DSYMMETRY *sym  = ( G3DSYMMETRY * ) q3dobject_getObject ( qobj );
    double SMVX[0x10];

    g3dcore_multmatrix ( sym->smatrix, MVX, SMVX );

    while ( ltmpchildren ) {
        Q3DOBJECT *qchild = ( Q3DOBJECT * ) ltmpchildren->data;

        filtervmb_import_r ( qfil, 
                             qcam,
                             qchild,
                             SMVX,
                             subframeID,
                             frame );

        ltmpchildren = ltmpchildren->next;
    }
}

/******************************************************************************/
static void filtervmb_import_r ( Q3DFILTER *qfil, 
                                 Q3DCAMERA *qcam,
                                 Q3DOBJECT *qobj,
                                 double    *MVX,
                                 int32_t    subframeID,
                                 float      frame ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    LIST *ltmpchildren = qobj->lchildren;
    double WMVX[0x10];

    /*** The recursive nature insures objects always have the same ID in ***/
    /*** subframes ***/
    fvmb->vobjID++;

    g3dcore_multmatrix ( qobj->obj->lmatrix, MVX, WMVX );

    if ( qobj->obj->type & MESH ) {
        Q3DMESH *qmes = ( Q3DMESH * ) qobj;
        /*** When the subframes happen, retrieve the on-frame object ***/
        VMBMESH *vmes = filtervmb_getObjectbyID ( qfil, fvmb->vobjID );

        if ( vmes == NULL ) {
            vmes = vmbmesh_new ( qmes,
                                 fvmb->vobjID,
                                 qcam,
                                 WMVX,
                                 frame,
                                 fvmb->nbSamples );
            /*** Note: vmes is NULL if not in screenspace ***/
            if ( vmes ) {
                list_insert ( &fvmb->lvobj, vmes );
            }
        }

        /*** Note: vmes is NULL if not in screenspace ***/
        if ( vmes ) {
            vmbmesh_import ( vmes,
                             qmes,
                             qcam,
                             WMVX,
                             subframeID,
                             frame );
        }
    }

    if ( qobj->obj->type == G3DSYMMETRYTYPE ) {
        filtervmb_importSymmetry ( qfil,
                                   qcam,
                                   qobj,
                                   WMVX,
                                   subframeID,
                                   frame );
    }

    while ( ltmpchildren ) {
        Q3DOBJECT *qchild = ( Q3DOBJECT * ) ltmpchildren->data;

        filtervmb_import_r ( qfil,
                             qcam,
                             qchild,
                             WMVX,
                             subframeID,
                             frame );

        ltmpchildren = ltmpchildren->next;
    }
}

/******************************************************************************/
static void filtervmb_import ( Q3DFILTER *qfil, 
                               Q3DCAMERA *qcam,
                               Q3DOBJECT *qobj,
                               double    *MVX,
                               int32_t    subframeID,
                               float      frame ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;

    fvmb->vobjID = 0x00;

    filtervmb_import_r ( qfil,
                         qcam, 
                         qobj,
                         MVX,
                         subframeID,
                         frame );
}

/******************************************************************************/
static void filtervmb_free ( Q3DFILTER *fil ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) fil->data;

    list_free ( &fvmb->lvobj, vmbmesh_free );

    if ( fvmb->zbuffer ) free ( fvmb->zbuffer );

    free ( fvmb );
}

/******************************************************************************/
static FILTERVMB *filtervmb_new ( uint32_t width, 
                                  uint32_t height,
                                  float    strength,
                                  uint32_t nbSamples,
                                  float    subSamplingRate ) {

    uint32_t structsize = sizeof ( FILTERVMB );
    FILTERVMB *fvmb = ( FILTERVMB * ) calloc ( 0x01, structsize );
    uint32_t i;

    if ( fvmb == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    fvmb->width     = width;
    fvmb->height    = height;
    fvmb->strength  = strength;
    fvmb->nbSamples = nbSamples;
    fvmb->subSamplingRate = 1.0f;

    fvmb->hlines = ( VMBZHLINE * ) calloc ( height, sizeof ( VMBZHLINE ) );

    fvmb->abuffer = ( uint32_t * ) calloc  ( width * 
                                             height, sizeof ( uint32_t ) * 0x04 );

    fvmb->zbuffer = ( VMBZPIXEL * ) calloc ( width * 
                                             height, sizeof ( VMBZPIXEL ) );

    for ( i = 0x00; i < width * height; i++ ) {
        fvmb->zbuffer[i].z = INFINITY;
    }

    return fvmb;
}

/******************************************************************************/
static uint32_t filtervmb_draw ( Q3DFILTER     *qfil, 
                                 Q3DJOB        *qjob,
                                 float          frameID,
                                 unsigned char *img, 
                                 uint32_t       from, 
                                 uint32_t       to, 
                                 uint32_t       bpp, 
                                 uint32_t       width ) {
    G3DCAMERA *cam = q3dobject_getObject ( qjob->qcam );
    Q3DSCENE  *oriqsce = qjob->qsce;
    G3DSCENE  *sce = q3dobject_getObject ( qjob->qsce );
    G3DOBJECT *objcam = ( G3DOBJECT * ) cam;
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    int32_t i, j;
    Q3DFILTERSET orifilters;
    float middleFrame = frameID     - ( 0.5f * fvmb->strength );
    float step = ( ( 0.5f ) * fvmb->strength ) / fvmb->nbSamples;
    float fromFrame = middleFrame - step;
    float toFrame   = middleFrame + step;

    fvmb->bpp = bpp;
    fvmb->img = img;

    /*** to substract 1.0f ***/
    if ( qjob->filters.toframe ) {
        q3djob_goToFrame ( qjob, middleFrame );

        q3djob_clear   ( qjob );
        q3djob_prepare ( qjob, 
                         qjob->qrsg->input.sce, 
                         qjob->qrsg->input.cam );

        qjob->qsce = q3dscene_import ( sce, middleFrame, 0x00 );

        memcpy ( &orifilters   , &qjob->filters, sizeof ( Q3DFILTERSET ) );
        memset ( &qjob->filters, 0x00          , sizeof ( Q3DFILTERSET ) );

        qjob->filters.toframe  = orifilters.toframe;
        qjob->filters.towindow = orifilters.towindow;

        q3djob_render ( qjob );

        memcpy ( &qjob->filters, &orifilters   , sizeof ( Q3DFILTERSET ) );

        filtervmb_import ( qfil, 
                           qjob->qcam,
                           qjob->qsce,
                           objcam->iwmatrix,
                           0x00,
                           middleFrame );

        for ( i = 0x01; i <= fvmb->nbSamples; i++ ) {
            q3djob_goToFrame ( qjob, fromFrame );

            Q3DSCENE *befqsce = q3dscene_import ( sce, 
                                                  fromFrame, 
                                                  GEOMETRYONLY );

            /*** Before-frames ***/
            filtervmb_import ( qfil, 
                               qjob->qcam,
                               befqsce,
                               objcam->iwmatrix,
                              -i,
                               fromFrame );

            /*** after frames ***/
            /*filtervmb_import ( qfil, 
                               qjob->qcam,
                               qjob->qsce,
                               objcam->iwmatrix,
                               i,
                               frameID );*/


            fromFrame -= step;

            if ( fromFrame <  ( frameID - 1.0f ) ) fromFrame = frameID - 1.0f;
            if ( toFrame   >= ( frameID + 1.0f ) ) toFrame   = frameID + 0.99f;
        }

        filtervmb_drawObjects ( qfil );
        filtervmb_drawInterpolatedObjects ( qfil );

        /*filtervmb_merge ( qfil, img, bpp );*/
    }

    return 0x02;
}

/******************************************************************************/
Q3DFILTER *q3dfilter_vmb_new ( uint32_t width,
                               uint32_t height,
                               float    strength,
                               uint32_t nbSamples,
                               float    subSamplingRate ) {
    Q3DFILTER *fil;

    fil = q3dfilter_new ( FILTERBEFORE, 
                          VECTORMOTIONBLURFILTERNAME,
                          filtervmb_draw,
                          filtervmb_free,
                          filtervmb_new ( width, 
                                          height,
                                          strength,
                                          nbSamples,
                                          subSamplingRate ) );

    return fil;
}
