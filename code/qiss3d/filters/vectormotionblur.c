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
    float z;
} VMBZPIXEL;

/******************************************************************************/
typedef struct _VMBZPOINT {
    int32_t x;
    int32_t y;
    float   z;
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
    LIST       *lvobj;
    uint32_t    vobjID;
    float       strength;
    uint32_t    nbSamples;
    VMBZPIXEL  *zbuffer;
    uint32_t    height;
    uint32_t    width;
    VMBZHLINE  *hlines;
    uint32_t  (*abuffer)[0x04]; /**** accumulation buffer ***/
} FILTERVMB;

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
                    unsigned char (*srcimg)[0x03] = img;
                    uint32_t offset = ( i * fvmb->width ) + j;
                     uint32_t div = fvmb->abuffer[offset][0x03];

                    if ( div ) {
                        srcimg[offset][0x00] = 0xFF/*fvmb->abuffer[offset][0x00] / div*/;
                        srcimg[offset][0x01] = 0xFF/*fvmb->abuffer[offset][0x01] / div*/;
                        srcimg[offset][0x02] = 0xFF/*fvmb->abuffer[offset][0x02] / div*/;
                    } else {
                        srcimg[offset][0x00] = 0x00;
                        srcimg[offset][0x01] = 0x00;
                        srcimg[offset][0x02] = 0x00;
                    }
                } break;

                default :
                break;
            }
        }
    }
}

/******************************************************************************/
static void filtervmb_trace ( Q3DFILTER    *qfil,
                              VMBVECTOR2S  *srcPoint,
                              VMBVECTOR2S  *dstPoint,
                              unsigned char R,
                              unsigned char G,
                              unsigned char B ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    int32_t dx  = ( dstPoint->x - srcPoint->x ),
            ddx = ( dx == 0x00 ) ? 0x01 : abs ( dx ),
            dy  = ( dstPoint->y - srcPoint->y ),
            ddy = ( dy == 0x00 ) ? 0x01 : abs ( dy ),
            dd  = ( ddx > ddy ) ? ddx : ddy;
    int px = ( dx > 0x00 ) ? 1 : -1, 
            py = ( dy > 0x00 ) ? 1 : -1;
    int32_t x = srcPoint->x, 
            y = srcPoint->y;
    int i, cumul = 0x00;

    if ( ddx > ddy ) {
        for ( i = 0x00; i <= ddx; i++ ) {
            if ( cumul >= ddx ) {
                cumul -= ddx;
                y     += py;
            }

            if ( ( y >= 0x00 ) && ( y < fvmb->height ) &&
                 ( x >= 0x00 ) && ( x < fvmb->width  ) ) {
                uint32_t offset = ( y * fvmb->width ) + x;

                fvmb->abuffer[offset][0x00] += R;
                fvmb->abuffer[offset][0x01] += G;
                fvmb->abuffer[offset][0x02] += B;

                fvmb->abuffer[offset][0x03]++;
            }

            cumul += ddy;
            x     += px;
        }
    } else {
        for ( i = 0x00; i <= ddy; i++ ) {
            if ( cumul >= ddy ) {
                    cumul -= ddy;
                    x     += px;
            }

            if ( ( y >= 0x00 ) && ( y < fvmb->height ) &&
                 ( x >= 0x00 ) && ( x < fvmb->width  ) ) {
                uint32_t offset = ( y * fvmb->width ) + x;

                fvmb->abuffer[offset][0x00] += R;
                fvmb->abuffer[offset][0x01] += G;
                fvmb->abuffer[offset][0x02] += B;

                fvmb->abuffer[offset][0x03]++;
            }

            cumul += ddx;
            y     += py;
        }
    }
}

/******************************************************************************/
static void filtervmb_blur ( Q3DFILTER     *qfil,
                             unsigned char *img,
                             uint32_t       bpp ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    uint32_t i, j, k;

    for ( i = 0x00; i < fvmb->height; i++ ) {
        for ( j = 0x00; j < fvmb->width; j++ ) {
            uint32_t offset = ( i * fvmb->width ) + j;

            if ( fvmb->zbuffer[offset].z != INFINITY ) {
                uint32_t vobjID = fvmb->zbuffer[offset].vobjID,
                         vtriID = fvmb->zbuffer[offset].vtriID;
                VMBOBJECT *vobj = filtervmb_getObjectbyID ( qfil, vobjID );
                VMBMESH   *vmes = ( VMBMESH * ) vobj;
                Q3DMESH   *qmes = ( Q3DMESH * ) vobj->qobj;
                uint32_t nbqtri = qmes->nbqtri;
                Q3DVECTOR3F v0v1 = { .x = vmes->vtri[vtriID].pnt[0x01].x -
                                          vmes->vtri[vtriID].pnt[0x00].x,
                                     .y = vmes->vtri[vtriID].pnt[0x01].y -
                                          vmes->vtri[vtriID].pnt[0x00].y, 
                                     .z = 0.0f },
                            v1v2 = { .x = vmes->vtri[vtriID].pnt[0x02].x -
                                          vmes->vtri[vtriID].pnt[0x01].x,
                                     .y = vmes->vtri[vtriID].pnt[0x02].y -
                                          vmes->vtri[vtriID].pnt[0x01].y,
                                     .z = 0.0f },
                            v2v0 = { .x = vmes->vtri[vtriID].pnt[0x00].x -
                                          vmes->vtri[vtriID].pnt[0x02].x,
                                     .y = vmes->vtri[vtriID].pnt[0x00].y -
                                          vmes->vtri[vtriID].pnt[0x02].y,
                                     .z = 0.0f };
                Q3DVECTOR3F vpv0 = { .x = vmes->vtri[vtriID].pnt[0x00].x - j,
                                     .y = vmes->vtri[vtriID].pnt[0x00].y - i,
                                     .z = 0.0f },
                            vpv1 = { .x = vmes->vtri[vtriID].pnt[0x01].x - j,
                                     .y = vmes->vtri[vtriID].pnt[0x01].y - i,
                                     .z = 0.0f },
                            vpv2 = { .x = vmes->vtri[vtriID].pnt[0x02].x - j,
                                     .y = vmes->vtri[vtriID].pnt[0x02].y - i,
                                     .z = 0.0f };
                Q3DVECTOR3F vout0, 
                            vout1, 
                            vout2;
                VMBVECTOR2S psrc = { .x = j,
                                     .y = i },
                            pdst;
                float surface = vmes->vtri[vtriID].surface ;
                float ratio0, 
                      ratio1,
                      ratio2;

                q3dvector3f_cross ( &v0v1, &vpv0, &vout0 );
                q3dvector3f_cross ( &v1v2, &vpv1, &vout1 );
                q3dvector3f_cross ( &v2v0, &vpv2, &vout2 );

                ratio0 = q3dvector3f_length ( &vout0 ) / surface;
                ratio1 = q3dvector3f_length ( &vout1 ) / surface;
                ratio2 = q3dvector3f_length ( &vout2 ) / surface;

                for ( k = 0x00; k < fvmb->nbSamples; k++ ) {
                    VMBTRIANGLE *vtribef = vmes->vtribef + ( k * nbqtri );

                    pdst.x = ( vtribef[vtriID].pnt[0x00].x * ratio0 ) +
                             ( vtribef[vtriID].pnt[0x01].x * ratio1 ) +
                             ( vtribef[vtriID].pnt[0x02].x * ratio2 );

                    pdst.y = ( vtribef[vtriID].pnt[0x00].y * ratio0 ) +
                             ( vtribef[vtriID].pnt[0x01].y * ratio1 ) +
                             ( vtribef[vtriID].pnt[0x02].y * ratio2 );

                    filtervmb_trace ( qfil, &psrc, &pdst, 0x00, 0x00, 0x00 );

                    memcpy ( &pdst, &psrc, sizeof ( VMBVECTOR2S ) );
                }
            }
        }
    }

    filtervmb_merge ( qfil, img, bpp );
}

/******************************************************************************/
static void filtervmb_import_r ( Q3DFILTER *qfil, 
                                 Q3DCAMERA *qcam,
                                 Q3DOBJECT *qobj,
                                 double    *MVX,
                                 int32_t    subframeID,
                                 float      frame );

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
                } else {
                    if ( x < fvmb->hlines[y].p1.x ) {
                        fvmb->hlines[y].p1.x = x;
                        fvmb->hlines[y].p1.z = z;
                    }

                    if ( x > fvmb->hlines[y].p2.x ) {
                        fvmb->hlines[y].p2.x = x;
                        fvmb->hlines[y].p2.z = z;
                    }

                    fvmb->hlines[y].inited = 0x02;
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

            if ( ( y >= 0x00 ) && ( y < fvmb->height ) ) {
                uint32_t offset = ( y * fvmb->width ) + x;

                if ( fvmb->hlines[y].inited == 0x00 ) {
                     fvmb->hlines[y].inited = 0x01;

                    fvmb->hlines[y].p1.x = fvmb->hlines[y].p2.x = x;
                    fvmb->hlines[y].p1.z = fvmb->hlines[y].p2.z = z;
                } else {
                    if ( x < fvmb->hlines[y].p1.x ) {
                        fvmb->hlines[y].p1.x = x;
                        fvmb->hlines[y].p1.z = z;
                    }

                    if ( x > fvmb->hlines[y].p2.x ) {
                        fvmb->hlines[y].p2.x = x;
                        fvmb->hlines[y].p2.z = z;
                    }

                    fvmb->hlines[y].inited = 0x02;
                }
            }

            cumul += ddx;
            y     += py;
            z     += pz;
        }
    }
}

/****************************************************************************/
static void filtervmb_fillHLine ( Q3DFILTER *qfil,
                                  uint32_t   vobjID,
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
    uint32_t offset = ( ( fvmb->height - 1 - y ) * fvmb->width );

    for ( i = 0x00; i <= ddx; i++ ) {
        /*** add some epsilon against Z fghting ***/
        float depth = fvmb->zbuffer[offset+x].z;

        if ( ( x >= 0x00 ) && 
             ( x <  fvmb->width ) ) {

            if  ( z <= depth ) {
                fvmb->zbuffer[offset+x].z      = z;
                fvmb->zbuffer[offset+x].vobjID = vobjID;
                fvmb->zbuffer[offset+x].vtriID = vtriID;
            }
        }

        x += px;
        z += pz;
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
        int32_t ymin = vmes->vtri[i].pnt[0x00].y,
                ymax = ymin;

        for ( j = 0x00; j < 0x03; j++ ) {
            uint32_t n = ( j + 0x01 ) % 0x03;
            VMBZPOINT pt1 = { .x = vmes->vtri[i].pnt[j].x,
                              .y = vmes->vtri[i].pnt[j].y,
                              .z = vmes->zval[i][j] },
                      pt2 = { .x = vmes->vtri[i].pnt[n].x,
                              .y = vmes->vtri[i].pnt[n].y,
                              .z = vmes->zval[i][n] };

            if ( vmes->vtri[i].pnt[j].y < ymin ) ymin = vmes->vtri[i].pnt[j].y;
            if ( vmes->vtri[i].pnt[j].y > ymax ) ymax = vmes->vtri[i].pnt[j].y;

            if ( pt1.x < pt2.x ) filtervmb_line ( qfil, &pt1, &pt2 );
            else                 filtervmb_line ( qfil, &pt2, &pt1 );
        }

        if ( ymin <  0x00         ) ymin = 0x00;
        if ( ymin >= fvmb->height ) ymin = fvmb->height - 1;
        if ( ymax <  0x00         ) ymax = 0x00;
        if ( ymax >= fvmb->height ) ymax = fvmb->height - 1;

        if ( ymin < ymax ) {
            for ( j = ymin; j < ymax; j++ ) {
                if ( fvmb->hlines[j].inited == 0x02 ){

                    filtervmb_fillHLine ( qfil, vobjID, vtriID, j );
                }
            }

            memset ( fvmb->hlines + ymin, 0x00, sizeof ( VMBZHLINE ) * ( ymax - 
                                                                         ymin ) );
        }
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
               vtri->pnt[0x00].y = screenpos[0x00].y;
        if ( zval ) zval[0x00]   = screenpos[0x00].z;

               vtri->pnt[0x01].x = screenpos[0x01].x;
               vtri->pnt[0x01].y = screenpos[0x01].y;
        if ( zval ) zval[0x01]   = screenpos[0x01].z;

               vtri->pnt[0x02].x = screenpos[0x02].x;
               vtri->pnt[0x02].y = screenpos[0x02].y;
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
                             MVX,
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
    uint32_t i;
    Q3DFILTERSET orifilters;
    float middleFrame = frameID     - ( 0.5f * fvmb->strength );
    float step = ( ( 0.5f ) * fvmb->strength ) / fvmb->nbSamples;
    float fromFrame = middleFrame - step;
    float toFrame   = middleFrame + step;




    /*** Because gotonextframe_draw will jump to frame + 1.0f, we have ***/
    /*** to substract 1.0f ***/
    if ( qjob->filters.toframe ) {
        qjob->filters.toframe->draw ( qjob->filters.toframe, 
                                      NULL, 
                                      middleFrame - 1.0f,
                                      NULL, 
                                      0x00, 
                                      0x00, 
                                      0x00, 
                                      0x00 );

        q3dobject_free ( qjob->qsce );

        qjob->curframe = middleFrame;

        qjob->qsce = q3dscene_import ( sce, middleFrame, 0x00 );

        memcpy ( &orifilters   , &qjob->filters, sizeof ( Q3DFILTERSET ) );
        memset ( &qjob->filters, 0x00          , sizeof ( Q3DFILTERSET ) );

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
            qjob->filters.toframe->draw ( qjob->filters.toframe, 
                                          NULL, 
                                          fromFrame - 1.0f,
                                          NULL, 
                                          0x00, 
                                          0x00, 
                                          0x00, 
                                          0x00 );

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

        filtervmb_blur ( qfil, img, bpp );
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
