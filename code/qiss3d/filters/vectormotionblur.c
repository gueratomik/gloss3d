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
    int16_t x;
    int16_t y;
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
    uint32_t   id;
} VMBOBJECT;

/******************************************************************************/
typedef struct _VMBVECTOR2S1D {
    int16_t x;
    int16_t y;
    float   z;
} VMBVECTOR2S1D;

/******************************************************************************/
typedef struct _VMBTRIANGLE {
    VMBVECTOR2S1D pnt[0x03];
} VMBTRIANGLE;

/******************************************************************************/
typedef struct _VMBMESH {
    VMBOBJECT    vobj;
    VMBTRIANGLE *vtri;    /*** triangles on frame ***/
    uint32_t     nbvtri;
} VMBMESH;

/******************************************************************************/
#define NBCOMPONENT 0x05
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
    uint16_t     (*abuffer)[NBCOMPONENT]; /**** accumulation buffer ***/
} FILTERVMB;

/******************************************************************************/
static void filtervmb_fillAbuffer ( Q3DFILTER    *qfil,
                                    uint32_t      vobjID,
                                    uint32_t      vtriID,
                                    VMBTRIANGLE  *vtri,
                                    int32_t       y,
                                    float         strength );

static void filtervmb_import_r    ( Q3DFILTER *qfil, 
                                    Q3DCAMERA *qcam,
                                    Q3DOBJECT *qobj,
                                    double    *MVX,
                                    int32_t    subframeID,
                                    float      frame,
                                    uint32_t   matrixmul );

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
                        float a0 = ( float ) fvmb->abuffer[offset][0x00] / fvmb->abuffer[offset][0x03],
                              a1 = ( float ) fvmb->abuffer[offset][0x01] / fvmb->abuffer[offset][0x03], 
                              a2 = ( float ) fvmb->abuffer[offset][0x02] / fvmb->abuffer[offset][0x03],
                              /*** get the average opacity ***/
                              op = ( float ) fvmb->abuffer[offset][0x03] / ( fvmb->abuffer[offset][0x04] * 0xFF ),
                              invop = 1.0f - op;

                            srcimg[offset][0x00] = ( 0xFF * a0 * op ) + ( srcimg[offset][0x00] * invop );
                            srcimg[offset][0x01] = ( 0xFF * a1 * op ) + ( srcimg[offset][0x01] * invop );
                            srcimg[offset][0x02] = ( 0xFF * a2 * op ) + ( srcimg[offset][0x02] * invop );

                            /*srcimg[offset][0x00] = fvmb->abuffer[offset][0x00];
                            srcimg[offset][0x01] = fvmb->abuffer[offset][0x01];
                            srcimg[offset][0x02] = fvmb->abuffer[offset][0x02];*/
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
            ddx = abs ( dx ),
            dy  = ( dstPoint->y - srcPoint->y ),
            ddy = abs ( dy ),
            dd  = ( ddx > ddy ) ? ddx : ddy;
    float   dz  = dstPoint->z  - srcPoint->z, pz = ( dd ) ? ( dz / dd ) : 0.0f;
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
    float pratio[0x03] = { ( dd ) ? ( dratio[0x00] / dd ) : 0.0f,
                           ( dd ) ? ( dratio[0x01] / dd ) : 0.0f,
                           ( dd ) ? ( dratio[0x02] / dd ) : 0.0f };
    int i, cumul = 0x00;

    if ( ddx > ddy ) {
        for ( i = 0x00; i <= ddx; i++ ) {
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

            if ( cumul >= ddx ) {
                cumul -= ddx;
                y     += py;
            }
        }
    } else {
        for ( i = 0x00; i <= ddy; i++ ) {
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

            if ( cumul >= ddy ) {
                    cumul -= ddy;
                    x     += px;
            }
        }
    }
}

/****************************************************************************/
static void filtervmb_fillZbuffer ( Q3DFILTER *qfil,
                                    uint32_t   vobjID,
                                    uint32_t   vtriID,
                                    int32_t    y ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    VMBZHLINE *hline = &fvmb->hlines[y];
    int32_t x1 = hline->p1.x, 
            x2 = hline->p2.x;
    float   z1 = hline->p1.z;
    int32_t dx = x2 - x1, ddx = abs ( dx );
    int32_t x = x1;
    float dz  = hline->p2.z - hline->p1.z, pz = ( ddx ) ? ( dz / ddx ) : 0.0f;
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
                fvmb->zbuffer[xoffset].vobjID = vobjID;
                fvmb->zbuffer[xoffset].vtriID = vtriID;
            }
        }

        x += px;
        z += pz;
    }
}

/****************************************************************************/
static void filtervmb_fillAbuffer ( Q3DFILTER    *qfil,
                                    uint32_t      vobjID,
                                    uint32_t      vtriID,
                                    VMBTRIANGLE  *refTri,
                                    int32_t       y,
                                    float         opacity ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    VMBZHLINE *hline = &fvmb->hlines[y];
    int32_t x1 = hline->p1.x, 
            x2 = hline->p2.x;
    float   z1 = hline->p1.z;
    int32_t dx = x2 - x1, ddx = abs ( dx );
    int32_t x = x1;
    float dz  = hline->p2.z - hline->p1.z, pz = ( ddx ) ? ( dz / ddx ) : 0.0f;
    long  px = ( dx > 0x00 ) ? 1 : -1;
    float z = z1;
    float ratio[0x03] = { hline->p1.ratio[0x00],
                          hline->p1.ratio[0x01],
                          hline->p1.ratio[0x02] };
    float dratio[0x03] = { hline->p2.ratio[0x00] - hline->p1.ratio[0x00],
                           hline->p2.ratio[0x01] - hline->p1.ratio[0x01],
                           hline->p2.ratio[0x02] - hline->p1.ratio[0x02] };
    float pratio[0x03] = { ( ddx ) ? ( dratio[0x00] / (ddx) ) : 0.0f,
                           ( ddx ) ? ( dratio[0x01] / (ddx) ) : 0.0f,
                           ( ddx ) ? ( dratio[0x02] / (ddx) ) : 0.0f };

    int i;
    uint32_t offset = ( y * fvmb->width );
    float invopac = 1.0f - opacity;

    for ( i = 0x00; i <= ddx; i++ ) {
        if ( ( x >= 0x00 ) && 
             ( x <  fvmb->width ) ) {

            int32_t xsrc = ( refTri->pnt[0x00].x * ratio[0x00] +
                             refTri->pnt[0x01].x * ratio[0x01] +
                             refTri->pnt[0x02].x * ratio[0x02] ),
                    ysrc = ( refTri->pnt[0x00].y * ratio[0x00] +
                             refTri->pnt[0x01].y * ratio[0x01] +
                             refTri->pnt[0x02].y * ratio[0x02] );

            if ( ( ysrc >= 0x00         ) && 
                 ( ysrc <  fvmb->height ) && 
                 ( xsrc >= 0x00         ) &&
                 ( xsrc <  fvmb->width  ) ) {
                uint32_t aoffset = offset + x;
                uint32_t zoffset = ( ysrc * fvmb->width ) + xsrc;
                unsigned char R, G, B, AR, AG, AB;

                /*** Note: those 2 lines insure that the reference triangle ***/
                /*** is precisely the original triangle, even though this   ***/
                /*** should not be needed because the calculation should be ***/
                /*** precise enough to match the original triangle. However ***/
                /*** when this is commented, there are more artefacts than  ***/
                /*** with it. But the result is not as good as with.        ***/
                /*if ( ( fvmb->zbuffer[zoffset].vobjID == vobjID ) &&
                     ( fvmb->zbuffer[zoffset].vtriID == vtriID ) ) {*/
                    if ( z <= fvmb->zbuffer[aoffset].z ) {
                            switch ( fvmb->bpp ) {
                                case 0x18 : {
                                    unsigned char (*srcimg)[0x03] = fvmb->img;

                                    R = srcimg[zoffset][0x00];
                                    G = srcimg[zoffset][0x01];
                                    B = srcimg[zoffset][0x02];
    /*
                                    AR = srcimg[aoffset][0x00];
                                    AG = srcimg[aoffset][0x01];
                                    AB = srcimg[aoffset][0x02];
    */
                                    AR = fvmb->abuffer[aoffset][0x00];
                                    AG = fvmb->abuffer[aoffset][0x01];
                                    AB = fvmb->abuffer[aoffset][0x02];
                                } break;

                                default :
                                break;
                            }
    /*
                            fvmb->abuffer[aoffset][0x00] += ( ( R * opacity ) + ( AR * invopac ) );
                            fvmb->abuffer[aoffset][0x01] += ( ( G * opacity ) + ( AG * invopac ) );
                            fvmb->abuffer[aoffset][0x02] += ( ( B * opacity ) + ( AB * invopac ) );
                            fvmb->abuffer[aoffset][0x03] = 0x01;
    */
                            fvmb->abuffer[aoffset][0x00] += ( R    * opacity );
                            fvmb->abuffer[aoffset][0x01] += ( G    * opacity );
                            fvmb->abuffer[aoffset][0x02] += ( B    * opacity );
                            fvmb->abuffer[aoffset][0x03] += ( 0xFF * opacity );
                            fvmb->abuffer[aoffset][0x04] ++;
                        }
                /*}*/
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
                                     uint32_t     vobjID,
                                     uint32_t     vtriID,
                                     VMBTRIANGLE *vtri,
                                     VMBTRIANGLE *refTri,
                                     float        strength,
                                     uint32_t     fillZBuffer ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    int32_t ymin = vtri->pnt[0x00].y,
            ymax = ymin;
    uint32_t i;

    for ( i = 0x00; i < 0x03; i++ ) {
        uint32_t n = ( i + 0x01 ) % 0x03;

        VMBZPOINT pt1 = { .x = vtri->pnt[i].x,
                          .y = vtri->pnt[i].y,
                          .z = vtri->pnt[i].z,
                          /*** due ton imprecision, we have to restrain the ***/
                          /*** reference area from where we pick the pixel ***/
                          /*** values, hence values 0.9f and twice 0.05f  ***/
                          .ratio[0x00] = ( i == 0x00 ) ? 0.9f : 0.05f,
                          .ratio[0x01] = ( i == 0x01 ) ? 0.9f : 0.05f, 
                          .ratio[0x02] = ( i == 0x02 ) ? 0.9f : 0.05f },
                  pt2 = { .x = vtri->pnt[n].x,
                          .y = vtri->pnt[n].y,
                          .z = vtri->pnt[n].z,
                          .ratio[0x00] = ( n == 0x00 ) ? 0.9f : 0.05f,
                          .ratio[0x01] = ( n == 0x01 ) ? 0.9f : 0.05f,
                          .ratio[0x02] = ( n == 0x02 ) ? 0.9f : 0.05f };

        if ( vtri->pnt[i].y < ymin ) ymin = vtri->pnt[i].y;
        if ( vtri->pnt[i].y > ymax ) ymax = vtri->pnt[i].y;

        if ( pt1.x < pt2.x ) filtervmb_line ( qfil, &pt1, &pt2 );
        else                 filtervmb_line ( qfil, &pt2, &pt1 );
    }

    /** check if triangle is outside screen space ***/
    if ( ( vtri->pnt[0x00].z != INFINITY ) &&
         ( vtri->pnt[0x01].z != INFINITY ) &&
         ( vtri->pnt[0x02].z != INFINITY ) ) {
        if ( ymin <  0x00         ) ymin = 0x00;
        if ( ymin >= fvmb->height ) ymin = fvmb->height - 1;
        if ( ymax <  0x00         ) ymax = 0x00;
        if ( ymax >= fvmb->height ) ymax = fvmb->height - 1;

        if ( ymin <= ymax ) {
            for ( i = ymin; i <= ymax; i++ ) {
                if ( fvmb->hlines[i].inited == 0x02 ){
                    if ( fillZBuffer ) {
                        filtervmb_fillZbuffer ( qfil, 
                                                vobjID, 
                                                vtriID, 
                                                i );
                    } else {
                        filtervmb_fillAbuffer ( qfil, 
                                                vobjID,  
                                                vtriID,
                                                refTri,
                                                i, 
                                                strength );
                    }
                }

                fvmb->hlines[i].inited = 0x00;
            }
        }
    }
}

/******************************************************************************/
static void filtervmb_drawMesh ( Q3DFILTER *qfil,
                                 VMBMESH   *vmes ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    uint32_t vobjID = vmes->vobj.id;
    VMBTRIANGLE *vtri = vmes->vtri + ( fvmb->nbSamples * vmes->nbvtri );
    uint32_t i, j;

    for ( i = 0x00; i < vmes->nbvtri; i++ ) {
        uint32_t vtriID = i;

        /*** Draw and fill the Z-Buffer ***/
        filtervmb_drawTriangle ( qfil,
                                 vobjID,
                                 vtriID,
                                &vtri[i],
                                 NULL,
                                 0.0f,
                                 0x01 );
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
static void vmbtriangle_print ( VMBTRIANGLE *vtri, 
                                uint32_t     vtriID, 
                                uint32_t     subframeID ) {
    uint32_t i;

    for ( i = 0x00; i < 0x03; i++ ) {
        printf ( "vertex %d -- %d %d %f\n", i, vtri->pnt[i].x,
                                               vtri->pnt[i].y,
                                               vtri->pnt[i].z );
    }
}

/******************************************************************************/
static void filtervmb_drawInterpolatedTriangles ( Q3DFILTER   *qfil,
                                                  uint32_t     vobjID,
                                                  VMBTRIANGLE *srcTris,
                                                  VMBTRIANGLE *refTris,
                                                  uint32_t     nbtris ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    int32_t nbSamples =  fvmb->nbSamples;
    uint32_t totSamples = ( nbSamples * 0x02 ) + 0x01;
    int32_t i, p;

    /** Note: the condition is "p < nbSamples" and not "p <= nbSamples"  **/
    /** because dont forget dstTri is the next tri, so if there are, e.g **/
    /** 1 sample, there are 3 frames, but only 2 interpolations. ***/
    for ( p = - nbSamples; p < nbSamples; p++ ) {
        float step = 1.0f / nbSamples;
        float srcOpacity = 1.0f - ( fabs ( p ) / nbSamples );
        float dstOpacity = ( p < 0x00 ) ? srcOpacity + step :
                                          srcOpacity - step;
        float deltaOpacity = dstOpacity - srcOpacity;

        for ( i = 0x00; i < nbtris; i++ ) {
            VMBTRIANGLE *refTri = &refTris[i];
            VMBTRIANGLE *srcTri = &srcTris[i];
            uint32_t vtriID = i;

            VMBTRIANGLE *dstTri = srcTri + nbtris;

            /** check if triangle is outside screen space ***/
            if ( ( srcTri->pnt[0x00].z != INFINITY ) &&
                 ( srcTri->pnt[0x01].z != INFINITY ) &&
                 ( srcTri->pnt[0x02].z != INFINITY ) &&
                 ( dstTri->pnt[0x00].z != INFINITY ) &&
                 ( dstTri->pnt[0x01].z != INFINITY ) &&
                 ( dstTri->pnt[0x02].z != INFINITY ) ) {
                G2DVECTOR mvec[0x03] = { { .x = dstTri->pnt[0x00].x - 
                                                srcTri->pnt[0x00].x,
                                           .y = dstTri->pnt[0x00].y - 
                                                srcTri->pnt[0x00].y },
                                         { .x = dstTri->pnt[0x01].x - 
                                                srcTri->pnt[0x01].x,
                                           .y = dstTri->pnt[0x01].y - 
                                                srcTri->pnt[0x01].y },
                                         { .x = dstTri->pnt[0x02].x - 
                                                srcTri->pnt[0x02].x,
                                           .y = dstTri->pnt[0x02].y - 
                                                srcTri->pnt[0x02].y } };
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

/*** IMPORTANT NOTE: there are artefacts when the maxLen at its maximum value**/
/*** so the quick fix here is to multiply the sampling rate by 0.25 until I **/
/*** find the issue ***/

                uint32_t iter = maxLen * fvmb->subSamplingRate * 0.25;

                if ( iter ) {
                    uint32_t j;
                    float stepOpacity = ( deltaOpacity ) / iter;



/*printf("max:%d iter:%d\n", maxLen, iter );*/
                    for ( j = 0x00; j < iter; j++ ) {
                        float pos = ( float ) j / iter;
                        VMBTRIANGLE itrTri = { .pnt[0x00].x = srcTri->pnt[0].x + ( mvec[0].x * pos ),
                                               .pnt[0x00].y = srcTri->pnt[0].y + ( mvec[0].y * pos ),
                                               .pnt[0x00].z = 0.0f,
                                               .pnt[0x01].x = srcTri->pnt[1].x + ( mvec[1].x * pos ),
                                               .pnt[0x01].y = srcTri->pnt[1].y + ( mvec[1].y * pos ),
                                               .pnt[0x01].z = 0.0f,
                                               .pnt[0x02].x = srcTri->pnt[2].x + ( mvec[2].x * pos ),
                                               .pnt[0x02].y = srcTri->pnt[2].y + ( mvec[2].y * pos ),
                                               .pnt[0x02].z = 0.0f };
                        float opacity = srcOpacity + ( stepOpacity  * j );
/*printf("%f\n", opacity);*/



                        /*** small trick to increase blending on  ***/
                        /*** frames close to the middle frame ***/
                        if ( opacity > 0.5f ) {
                            opacity = sin ( opacity * M_PI * 0.5 );
                            opacity = sin ( opacity * M_PI * 0.5 );
                            opacity = sin ( opacity * M_PI * 0.5 );
                            opacity = sin ( opacity * M_PI * 0.5 );
                            opacity = sin ( opacity * M_PI * 0.5 );
                        }

                        filtervmb_drawTriangle ( qfil,
                                                 vobjID,
                                                 vtriID,
                                                &itrTri,
                                                 refTri,
                                                 opacity,
                                                 0x00 );
                    }
                }
            }
        }

        srcTris += nbtris;
    }
}

/******************************************************************************/
static void filtervmb_drawInterpolatedObjects ( Q3DFILTER *qfil ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    LIST *ltmpvobj = fvmb->lvobj;

    while ( ltmpvobj ) {
        VMBOBJECT *vobj = ( VMBOBJECT * ) ltmpvobj->data;
        VMBMESH *vmes = ( VMBMESH * ) vobj;

        /*** Triangles from starting point to middle frame ***/
        filtervmb_drawInterpolatedTriangles ( qfil, 
                                              vmes->vobj.id,
                                              vmes->vtri,
                                              vmes->vtri + ( fvmb->nbSamples * vmes->nbvtri ),
                                              vmes->nbvtri );

        ltmpvobj = ltmpvobj->next;
    }
}

/******************************************************************************/
static void vmbtriangle_init ( VMBTRIANGLE *vtri,
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

    /*if ( ( screenmin.x >= qcam->VPX[0x02] ) ||
         ( screenmin.y >= qcam->VPX[0x03] ) ||
         ( screenmax.x <  qcam->VPX[0x00] ) ||
         ( screenmax.y <  qcam->VPX[0x01] ) ) {
            vtri->pnt[0x00].z = INFINITY;
            vtri->pnt[0x01].z = INFINITY;
            vtri->pnt[0x02].z = INFINITY;
    } else {
        vtri->pnt[0x00].x = screenpos[0x00].x;
        vtri->pnt[0x00].y = qcam->VPX[0x03] - 1 - screenpos[0x00].y;
        vtri->pnt[0x00].z = screenpos[0x00].z;

        vtri->pnt[0x01].x = screenpos[0x01].x;
        vtri->pnt[0x01].y = qcam->VPX[0x03] - 1 - screenpos[0x01].y;
        vtri->pnt[0x01].z = screenpos[0x01].z;

        vtri->pnt[0x02].x = screenpos[0x02].x;
        vtri->pnt[0x02].y = qcam->VPX[0x03] - 1 - screenpos[0x02].y;
        vtri->pnt[0x02].z = screenpos[0x02].z;
    }*/

    /*** draw only triangles that entirely fit the screen ***/
    if ( ( screenmin.x >= qcam->VPX[0x00] ) &&
         ( screenmin.y >= qcam->VPX[0x00] ) &&
         ( screenmax.x <  qcam->VPX[0x02] ) &&
         ( screenmax.y <  qcam->VPX[0x03] ) ) {
        vtri->pnt[0x00].x = screenpos[0x00].x;
        vtri->pnt[0x00].y = qcam->VPX[0x03] - 1 - screenpos[0x00].y;
        vtri->pnt[0x00].z = screenpos[0x00].z;

        vtri->pnt[0x01].x = screenpos[0x01].x;
        vtri->pnt[0x01].y = qcam->VPX[0x03] - 1 - screenpos[0x01].y;
        vtri->pnt[0x01].z = screenpos[0x01].z;

        vtri->pnt[0x02].x = screenpos[0x02].x;
        vtri->pnt[0x02].y = qcam->VPX[0x03] - 1 - screenpos[0x02].y;
        vtri->pnt[0x02].z = screenpos[0x02].z;
    } else {
        vtri->pnt[0x00].z = INFINITY;
        vtri->pnt[0x01].z = INFINITY;
        vtri->pnt[0x02].z = INFINITY;
    }
}

/******************************************************************************/
static VMBMESH *vmbmesh_free ( VMBMESH *vmes ) {
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
    VMBTRIANGLE *vtri = vmes->vtri + ( subframeID * vmes->nbvtri );
    uint32_t i;

    for ( i = 0x00; i < vmes->nbvtri; i++ ) {
        vmbtriangle_init ( &vtri[i],
                           &qmes->qtri[i], 
                            qver,
                            MVX,
                            qcam );

        /*vmbtriangle_print ( &vtri[i], i, subframeID );*/
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

                vmes->vtri = ( VMBTRIANGLE * ) calloc ( ( qmes->nbqtri             ) +
                                                        ( qmes->nbqtri * nbSamples ) +
                                                        ( qmes->nbqtri * nbSamples ) , triSize );

            }

            vmes->vobj.id = id;
            vmes->nbvtri = qmes->nbqtri;
        }
    }

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
                             frame,
                             0x01 );

        ltmpchildren = ltmpchildren->next;
    }
}

/******************************************************************************/
static void filtervmb_import_r ( Q3DFILTER *qfil, 
                                 Q3DCAMERA *qcam,
                                 Q3DOBJECT *qobj,
                                 double    *MVX,
                                 int32_t    subframeID,
                                 float      frame,
                                 uint32_t   matrixmul ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    LIST *ltmpchildren = qobj->lchildren;
    Q3DMESH *qmes = NULL;
    double WMVX[0x10];


    /*** The recursive nature insures objects always have the same ID in ***/
    /*** subframes ***/
    fvmb->vobjID++;

    if ( matrixmul ) {
        g3dcore_multmatrix ( qobj->obj->lmatrix, MVX, WMVX );
    } else {
        memcpy ( WMVX, MVX, sizeof ( WMVX ) );
    }

    if ( qobj->obj->type == G3DPARTICLEEMITTERTYPE ) {
        Q3DPARTICLEEMITTER *qpem = qobj;
        G3DPARTICLEEMITTER *pem = qpem->qobj.obj;

        if ( pem->maxParticles ) {
           if ( pem->maxParticlesPerFrame ) {
               G3DCAMERA *cam = q3dobject_getObject ( qcam );
               G3DOBJECT *objcam = ( G3DOBJECT * ) cam;
               uint32_t i, j;

               for ( i = 0x00; i < pem->particleLifetime; i++ ) {
                   G3DPARTICLE  *prt =  pem->particles + ( pem->maxParticlesPerFrame * i );
                   Q3DPARTICLE *qprt = qpem->qprt      + ( pem->maxParticlesPerFrame * i );

                   for ( j = 0x00; j < pem->maxParticlesPerFrame; j++ ) {
                       if ( prt[j].flags & PARTICLE_ISALIVE ) {
                           if ( qprt[j].qref ) {
                               g3dcore_multmatrix ( qprt[j].MVX,
                                                    objcam->iwmatrix, 
                                                     WMVX );

                               filtervmb_import_r ( qfil, 
                                                    qcam, 
                                                    qprt[j].qref,
                                                    WMVX,
                                                    subframeID,
                                                    frame,
                                                    0x00 );
                           }
                       }
                   }
               }
            }
        }
    }

    if ( qobj->obj->type == G3DINSTANCETYPE ) {
        Q3DINSTANCE *qins = qobj;

        if ( qins->qref->obj->type & MESH ) {
            qmes = ( Q3DMESH * ) qins->qref;
        }
    }

    if ( qobj->obj->type & MESH ) {
        qmes = ( Q3DMESH * ) qobj;
    }

    if ( qmes ) {
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
                             frame,
                             0x01 );

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
                         frame,
                         0x01 );
}

/******************************************************************************/
static void filtervmb_empty ( FILTERVMB *fvmb ) {
    uint32_t i;

    fvmb->abuffer = ( uint16_t * ) realloc  ( fvmb->abuffer,
                                              fvmb->width * 
                                              fvmb->height * sizeof ( uint16_t ) * NBCOMPONENT );

    fvmb->zbuffer = ( VMBZPIXEL * ) realloc ( fvmb->zbuffer,
                                              fvmb->width * 
                                              fvmb->height * sizeof ( VMBZPIXEL ) );

    memset ( fvmb->abuffer, 0x00, fvmb->width  * 
                                  fvmb->height * sizeof ( uint16_t ) * NBCOMPONENT );

    memset ( fvmb->zbuffer, 0x00, fvmb->width  * 
                                  fvmb->height * sizeof ( VMBZPIXEL ) );

    for ( i = 0x00; i < fvmb->width * fvmb->height; i++ ) {
        fvmb->zbuffer[i].z = INFINITY;
    }

    list_free ( &fvmb->lvobj, vmbmesh_free );
}

/******************************************************************************/
static void filtervmb_free ( Q3DFILTER *fil ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) fil->data;

    list_free ( &fvmb->lvobj, vmbmesh_free );

    if ( fvmb->abuffer ) free ( fvmb->abuffer );
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

    filtervmb_empty ( fvmb );


    return fvmb;
}

/******************************************************************************/
static float nextFrame ( float frame, float step, float max ) {
    frame += step;

    return ( frame < max ) ? frame : max;
}

/******************************************************************************/
static uint32_t filtervmb_draw ( Q3DFILTER     *qfil, 
                                 Q3DJOB        *qjob,
                                 uint32_t       cpuID, 
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
    float amplitude = ( 0.5f * fvmb->strength );
    float middleFrame = frameID + 0.5f;
    float step = amplitude / fvmb->nbSamples;
    float fromFrame = middleFrame - amplitude;
    float maxFrame = frameID + 0.99f;
    uint32_t rank = 0x00;

    fvmb->bpp = bpp;
    fvmb->img = img;

    if ( qjob->filters.toframe ) {
        /*** Note: Frame must be called in chronological order ***/
        /*** or else particles will not get blurred correctly. ***/

        for ( i = 0x00; i < fvmb->nbSamples; i++ ) {
            q3djob_goToFrame ( qjob, fromFrame );

            /*** The cmaera may move too ***/
            Q3DCAMERA *befqcam = q3dcamera_new ( qjob->qrsg->input.cam,
                                                 0x00,
                                                 0x00,
                                                 qjob->qrsg->output.width, 
                                                 qjob->qrsg->output.height );

            Q3DSCENE *befqsce = q3dscene_import ( sce, 
                                                  fromFrame, 
                                                  GEOMETRYONLY );

            /*** Before-frames ***/
            filtervmb_import ( qfil, 
                               befqcam,
                               befqsce,
                               objcam->iwmatrix,
                               rank++,
                               fromFrame );

            q3dobject_free_r ( ( Q3DOBJECT * ) befqsce );
            q3dobject_free_r ( ( Q3DOBJECT * ) befqcam );

            fromFrame = nextFrame ( fromFrame, step, maxFrame );
        }

        /*** q3djob_goToFrame() needed just for particles to move correctly ***/
        q3djob_goToFrame ( qjob, fromFrame );
        q3djob_clear   ( qjob );
        q3djob_prepare ( qjob, 
                         qjob->qrsg->input.sce, 
                         qjob->qrsg->input.cam,
                         fromFrame );

        memcpy ( &orifilters   , &qjob->filters, sizeof ( Q3DFILTERSET ) );
        memset ( &qjob->filters, 0x00          , sizeof ( Q3DFILTERSET ) );

        qjob->filters.toframe  = orifilters.toframe;
        qjob->filters.towindow = orifilters.towindow;
        qjob->filters.edgeaa = orifilters.edgeaa;

        q3djob_render ( qjob );

        /*** commeted out : zeroed on creation ***/
        /*for ( i = 0x00; i < fvmb->width * fvmb->height; i++ ) {
            unsigned char (*srcimg)[0x03] = img;

            fvmb->abuffer[i][0x00] = 0x00;
            fvmb->abuffer[i][0x01] = 0x00;
            fvmb->abuffer[i][0x02] = 0x00;
            fvmb->abuffer[i][0x03] = 0x00;
            fvmb->abuffer[i][0x04] = 0x00;
        }*/

        memcpy ( &qjob->filters, &orifilters   , sizeof ( Q3DFILTERSET ) );

        filtervmb_import ( qfil, 
                           qjob->qcam,
                           qjob->qsce,
                           objcam->iwmatrix,
                           rank++,
                           fromFrame );

        fromFrame = nextFrame ( fromFrame, step, maxFrame );

        for ( i = 0x00; i < fvmb->nbSamples; i++ ) {
            q3djob_goToFrame ( qjob, fromFrame );

            /*** The cmaera may move too ***/
            Q3DCAMERA *aftqcam = q3dcamera_new ( qjob->qrsg->input.cam,
                                                 0x00,
                                                 0x00,
                                                 qjob->qrsg->output.width, 
                                                 qjob->qrsg->output.height );

            Q3DSCENE *aftqsce = q3dscene_import ( sce, 
                                                  fromFrame,
                                                  GEOMETRYONLY );

            /*** after frames ***/
            filtervmb_import ( qfil, 
                               aftqcam,
                               aftqsce,
                               objcam->iwmatrix,
                               rank++,
                               fromFrame );

            q3dobject_free_r ( ( Q3DOBJECT * ) aftqsce );
            q3dobject_free_r ( ( Q3DOBJECT * ) aftqcam );

            fromFrame = nextFrame ( fromFrame, step, maxFrame );
        }

        /*** Z-Buffer ***/
        filtervmb_drawObjects ( qfil );

        /*** Geometry Buffer ***/
        filtervmb_drawInterpolatedObjects ( qfil );

        filtervmb_merge ( qfil, img, bpp );
    }

    filtervmb_empty ( fvmb );


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
