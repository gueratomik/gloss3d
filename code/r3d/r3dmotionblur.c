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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
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

/**
 * The Vector Motion Blur allows faster bluring of an image where motion occurs.
 * It is less accurate than the Scene Motion Blur and have some limitations.
 * It cannot handle blur for objects visible on a reflecting surface.
 * However, it can deal with rigged meshes as well as fixed-topology objects.
 * It is inspired by the way software renderers are implemented. It computes the
 * 2-dimensionnal vector for each vertex of each triangle to be rendered, by 
 * remembering the previous position of these vertices, from the previous
 * rendering (in screen coordinates). Then for each pixel within the triangle,
 * it computes its motion vector by interpolating vertices' motion vectors.
 * The final image is then transformed according to the vector map.
 * 
 */

/******************************************************************************/
uint32_t filtervectormotionblur_draw ( R3DFILTER *fil, R3DSCENE *rsce,
                                                       float frameID,
                                                       unsigned char *img, 
                                                       uint32_t from, 
                                                       uint32_t to, 
                                                       uint32_t depth, 
                                                       uint32_t width ) {
    R3DMOTIONBLUR *rmb = ( R3DMOTIONBLUR * ) fil->data;

    r3dmotionblur_trace ( rmb, rsce );
    r3dmotionblur_blurify ( rmb, img, rsce );

    return 0x00;
}

/******************************************************************************/
void filtervectormotionblur_free ( R3DFILTER *fil ) {
    R3DMOTIONBLUR *rmb = ( R3DMOTIONBLUR * ) fil->data;

    r3dmotionblur_free ( rmb );
}

/******************************************************************************/
R3DFILTER *r3dfilter_VectorMotionBlur_new ( uint32_t width,
                                            uint32_t height,
                                            float    strength ) {
    R3DFILTER *fil;

    fil = r3dfilter_new ( FILTERIMAGE, VECTORMOTIONBLURFILTERNAME,
                                       filtervectormotionblur_draw,
                                       filtervectormotionblur_free,
                                       r3dmotionblur_new ( width, 
                                                           height,
                                                           strength ) );

    return fil;
}

/******************************************************************************/
R3DMOTIONBLUR *r3dmotionblur_new ( uint32_t width,
                                   uint32_t height, 
                                   float    strength ) {
    R3DMOTIONBLUR *rmb = calloc ( 0x01, sizeof ( R3DMOTIONBLUR ) );
    int i;

    if ( rmb == NULL ) {
        fprintf ( stderr, "r3dmotionblur_new: calloc failed\n");

        return NULL;
    }

    rmb->hlines = calloc ( height, sizeof ( R3DMOTIONHLINE ) );

    if ( rmb->hlines == NULL ) {
        fprintf ( stderr, "r3dmotionblur_new: hlines calloc failed\n");

        free ( rmb );

        return NULL;
    }

    rmb->vectors = calloc ( width * height, sizeof ( R3DMOTIONVECTOR ) );

    if ( rmb->vectors == NULL ) {
        fprintf ( stderr, "r3dmotionblur_new: vectorBuffer calloc failed\n");

        free ( rmb->hlines );
        free ( rmb );

        return NULL;
    }

    /*** Store vectors Z position. We don't need to store x and y because ***/
    /*** they are given by the screen coordinates. ***/
    rmb->z = calloc ( width * height, sizeof ( double ) );

    if ( rmb->z == NULL ) {
        fprintf ( stderr, "r3dmotionblur_new: z calloc failed\n");

        free ( rmb->vectors );
        free ( rmb->hlines );
        free ( rmb );

        return NULL;
    }

    for ( i = 0x00; i < width * height; i++ ) {
        rmb->z[i] = 1.0f;
    }

    /*** receives the final image. Used to be needed because we worked with ***/
    /*** uint32_t but now we work with char and we dont compute any average ***/
    /*** value, so we should be able to directly usr the img buffer.        ***/
    rmb->blur = ( unsigned char * ) calloc ( width * height, 
                                             sizeof ( unsigned char ) * 0x03 );

    if ( rmb->blur == NULL ) {
        fprintf ( stderr, "r3dmotionblur_new: blur calloc failed\n");

        free ( rmb->z );
        free ( rmb->vectors );
        free ( rmb->hlines );
        free ( rmb );

        return NULL;
    }

    rmb->width    = width;
    rmb->height   = height;
    rmb->strength = strength;

    return rmb;
}

/******************************************************************************/
void r3dmotionblur_free ( R3DMOTIONBLUR *rmb ) {
    if ( rmb->z       ) free ( rmb->z       );
    if ( rmb->vectors ) free ( rmb->vectors );
    if ( rmb->hlines  ) free ( rmb->hlines  );
    if ( rmb->blur    ) free ( rmb->blur    );

    free ( rmb );
}

/******************************************************************************/
void r3dmotionblur_fillHLineBuffer ( R3DMOTIONBLUR   *rmb,
                                     R3DMOTIONVECTOR *startPos,
                                     R3DMOTIONVECTOR *startVector,
                                     R3DMOTIONVECTOR *endPos,
                                     R3DMOTIONVECTOR *endVector ) {
    int32_t dx  = (endPos->x - startPos->x),
            ddx = ( dx == 0x00 ) ? 0x01 : abs ( dx ),
            dy  = (endPos->y - startPos->y),
            ddy = ( dy == 0x00 ) ? 0x01 : abs ( dy ),
            dd  = ( ddx > ddy ) ? ddx : ddy;
    double dvecx = endVector->x - startVector->x, pvecx = ( dvecx / dd ),
           dvecy = endVector->y - startVector->y, pvecy = ( dvecy / dd ),
           dvecz = endVector->z - startVector->z, pvecz = ( dvecz / dd ),
           dz    = endPos->z    - startPos->z   , pz    = ( dz    / dd );
    int px = ( dx > 0x00 ) ? 1 : -1, 
        py = ( dy > 0x00 ) ? 1 : -1;
    double vecx = startVector->x, 
           vecy = startVector->y,
           vecz = startVector->z;
    int32_t x = startPos->x, 
            y = startPos->y;
    double  z = startPos->z;
    int i, cumul = 0x00;

    if ( ddx > ddy ) {
        for ( i = 0x00; i < ddx; i++ ) {
            if ( cumul >= ddx ) {
                cumul -= ddx;
                y     += py;
            }

            if ( ( y >= 0 ) && ( y < rmb->height ) ) {
                if ( rmb->hlines[y].inited == 0x00 ) {
                    rmb->hlines[y].inited = 0x01;

                    rmb->hlines[y].x1 = rmb->hlines[y].x2 = x;
                    rmb->hlines[y].z1 = rmb->hlines[y].z2 = z;
                    rmb->hlines[y].start.x = rmb->hlines[y].end.x = vecx;
                    rmb->hlines[y].start.y = rmb->hlines[y].end.y = vecy;
                    rmb->hlines[y].start.z = rmb->hlines[y].end.z = vecz;
                } else {
                    if ( x < rmb->hlines[y].x1 ) {
                         rmb->hlines[y].x1 = x;
                         rmb->hlines[y].z1 = z;
                         rmb->hlines[y].start.x = vecx;
                         rmb->hlines[y].start.y = vecy;
                         rmb->hlines[y].start.z = vecz;
                    }

                    if ( x > rmb->hlines[y].x2 ) {
                         rmb->hlines[y].x2 = x;
                         rmb->hlines[y].z2 = z;
                         rmb->hlines[y].end.x = vecx;
                         rmb->hlines[y].end.y = vecy;
                         rmb->hlines[y].end.z = vecz;
                    }

                    rmb->hlines[y].inited = 0x02;
        	}
            }

            cumul += ddy;
            x     += px;

            vecx  += pvecx;
            vecy  += pvecy;
            vecz  += pvecz;
            z     += pz;
        }
    } else {
        for ( i = 0x00; i < ddy; i++ ) {
            if ( cumul >= ddy ) {
                cumul -= ddy;
                x     += px;
            }

            if ( ( y >= 0 ) && ( y < rmb->height ) ) {
                if ( rmb->hlines[y].inited == 0x00 ) {
                    rmb->hlines[y].inited = 0x01;

                    rmb->hlines[y].x1 = rmb->hlines[y].x2 = x;
                    rmb->hlines[y].z1 = rmb->hlines[y].z2 = z;
                    rmb->hlines[y].start.x = rmb->hlines[y].end.x = vecx;
                    rmb->hlines[y].start.y = rmb->hlines[y].end.y = vecy;
                    rmb->hlines[y].start.z = rmb->hlines[y].end.z = vecz;
                } else {
                    if ( x < rmb->hlines[y].x1 ) {
                         rmb->hlines[y].x1 = x;
                         rmb->hlines[y].z1 = z;
                         rmb->hlines[y].start.x = vecx;
                         rmb->hlines[y].start.y = vecy;
                         rmb->hlines[y].start.z = vecz;
                    }

                    if ( x > rmb->hlines[y].x2 ) {
                         rmb->hlines[y].x2 = x;
                         rmb->hlines[y].z2 = z;
                         rmb->hlines[y].end.x = vecx;
                         rmb->hlines[y].end.y = vecy;
                         rmb->hlines[y].end.z = vecz;
                    }

                    rmb->hlines[y].inited = 0x02;
        	}
            }

            cumul += ddx;
            y     += py;

            vecx  += pvecx;
            vecy  += pvecy;
            vecz  += pvecz;
            z     += pz;
        }
    }
}

/****************************************************************************/
void r3dmotionblur_interpolateHLine ( R3DMOTIONBLUR *rmb, int32_t y ) {
    R3DMOTIONHLINE *hline = &rmb->hlines[y];
    int32_t x1 = hline->x1, x2 = hline->x2;
    int32_t dx = x2 - x1, ddx = ( dx == 0x00 ) ? 0x01 : abs ( dx );
    double dvecx = hline->end.x - hline->start.x, pvecx = ( dvecx / ddx ),
           dvecy = hline->end.y - hline->start.y, pvecy = ( dvecy / ddx ),
           dvecz = hline->end.z - hline->start.z, pvecz = ( dvecz / ddx ),
           dz    = hline->z2    - hline->z1     , pz    = ( dz    / ddx );
    long  px = ( dx > 0x00 ) ? 1 : -1;
    double vecx = hline->start.x,
           vecy = hline->start.y,
           vecz = hline->start.z,
           z    = hline->z1;
    int i;

    for ( i = 0x00; i <= ddx; i++ ) {
        uint32_t offset = ((y)*rmb->width)+x1;

        if ( x1 >= 0 && x1 < rmb->width ) {
            if ( z < rmb->z[offset] ) {
                rmb->vectors[offset].x = vecx;
                rmb->vectors[offset].y = vecy;
                rmb->vectors[offset].z = vecz;

                rmb->z[offset] = z;
            }
        }

        x1 += px;

        vecx += pvecx;
        vecy += pvecy;
        vecz += pvecz;
        z    += pz;
    }
}

/******************************************************************************/
void r3dmotionblur_fillVectorBuffer ( R3DMOTIONBLUR *rmb,
                                      R3DMOTIONTRACKPOINT *startPoint[3],
                                      R3DMOTIONTRACKPOINT *endPoint[3] ) {
    int i;

    memset ( rmb->hlines, 0x00, sizeof ( R3DMOTIONHLINE ) * rmb->height );

    for ( i = 0; i < 3; i++ ) {
        int n = ( i + 1 ) % 3;
                        /*** Starting point motion vector ***/
        R3DMOTIONVECTOR motion0 = { .x = ( endPoint[i]->x - startPoint[i]->x ),
                                    .y = ( endPoint[i]->y - startPoint[i]->y ),
                                    .z = ( endPoint[i]->z - startPoint[i]->z )},
                        /*** End point motion vector ***/
                        motion1 = { .x = ( endPoint[n]->x - startPoint[n]->x ),
                                    .y = ( endPoint[n]->y - startPoint[n]->y ),
                                    .z = ( endPoint[n]->z - startPoint[n]->z )};

         /*** For each edge, fill the Horizontal Lines Buffer ***/
         /*** They allow us to interpolate horizontal lines ***/
         r3dmotionblur_fillHLineBuffer ( rmb, startPoint[i], &motion0, 
                                              startPoint[n], &motion1 );
    }

    /*** Then interplate the vectors along the horizontal lines ***/
    for ( i = 0; i < rmb->height; i++ ) {
        if ( rmb->hlines[i].inited == 0x02 ){
            if ( ( ( rmb->hlines[i].x1 < rmb->width ) && 
                   ( rmb->hlines[i].x2 > 0          ) ) ||
                 ( ( rmb->hlines[i].x2 < rmb->width ) && 
                   ( rmb->hlines[i].x1 > 0          ) ) ) {
                r3dmotionblur_interpolateHLine ( rmb, i );
            }
        }
    }
}


/******************************************************************************/
void r3dmotionblur_getAverageColor ( R3DMOTIONBLUR *rmb,
                                     R3DMOTIONTRACKPOINT *src, 
                                     R3DMOTIONTRACKPOINT *dst,
                                     unsigned char (*img)[0x03],
                                     unsigned char *red,
                                     unsigned char *green,
                                     unsigned char *blue ) {
    /* we go from the destination to the source */
    int32_t dx = (src->x - dst->x), ddx = ( dx == 0x00 ) ? 0x01 : abs ( dx ),
            dy = (src->y - dst->y), ddy = ( dy == 0x00 ) ? 0x01 : abs ( dy ),
            dd = ( ddx > ddy ) ? ddx : ddy;
    int px = ( dx > 0x00 ) ? 1 : -1, py = ( dy > 0x00 ) ? 0x01 : -0x01;
    int32_t x = dst->x, y = dst->y;
    int i, cumul = 0x00;
    uint32_t R = 0, G = 0, B = 0;
    uint32_t div = 0x00;

    if ( ddx > ddy ) {
        for ( i = 0x00; i < ddx; i++ ) {
            if ( cumul >= ddx ) {
                cumul -= ddx;
                y     += py;
            }

            if ( ( y >= 0 && y < rmb->height ) && 
                 ( x >= 0 && x < rmb->width  ) ) {
                uint32_t offset = ((y)*rmb->width)+x;

        	    R += img[offset][0];
        	    G += img[offset][1];
        	    B += img[offset][2];

                div++;
            }

            cumul += ddy;
            x     += px;
        }
    } else {
        for ( i = 0x00; i < ddy; i++ ) {
            if ( cumul >= ddy ) {
                cumul -= ddy;
                x     += px;
            }

            if ( ( y >= 0 && y < rmb->height ) && 
                 ( x >= 0 && x < rmb->width  ) ) {
                uint32_t offset = ((y)*rmb->width)+x;

                R += img[offset][0];
                G += img[offset][1];
                B += img[offset][2];

                div++;
            }

            cumul += ddx;
            y     += py;
        }
    }

    if ( div ) {
        *red   = ( R / div );
        *green = ( G / div );
        *blue  = ( B / div );
    } else {
        *red   = 0;
        *green = 0;
        *blue  = 0;
    }
}

/******************************************************************************/
void r3dmotionblur_blurImage ( R3DMOTIONBLUR *rmb,
                               R3DMOTIONTRACKPOINT *src, 
                               R3DMOTIONVECTOR *vec,
                               unsigned char (*img)[0x03],
                               float strength ) {
    /* we go from the destination to the source */
    R3DMOTIONTRACKPOINT dst = { src->x + vec->x,
                                src->y + vec->y,
                                src->z + vec->z };
    int32_t dx  = (src->x - dst.x) * strength, 
            ddx = ( dx == 0x00 ) ? 0x01 : abs ( dx ),
            dy  = (src->y - dst.y) * strength, 
            ddy = ( dy == 0x00 ) ? 0x01 : abs ( dy ),
            dd  = ( ddx > ddy ) ? ddx : ddy;
    int32_t px  = ( dx > 0x00 ) ? 1 : -1, py = ( dy > 0x00 ) ? 1 : -1;
    double  dz  = (src->z - dst.z), pz = ( dz / dd );
    int32_t x = dst.x, 
            y = dst.y;
    double  z = dst.z;
    int i, cumul = 0x00;
    unsigned char avgR, avgG, avgB; /* average color values  */
                                    /* from along the vector */

    r3dmotionblur_getAverageColor (  rmb, src, &dst, img, &avgR,
                                                          &avgG,
                                                          &avgB );

    if ( ddx > ddy ) {
        for ( i = 0x00; i < ddx; i++ ) {
            float incFac = ( 1.0f / ddx ) * i, /* incremental factor */
                  decFac = ( 1.0f - incFac );  /* decremental factor */

            if ( cumul >= ddx ) {
                cumul -= ddx;
                y     += py;
            }

            /*if ( z < rmb->z[offset] ) {*/
            if ( ( y >= 0 && y < rmb->height ) && 
                 ( x >= 0 && x < rmb->width  ) ) {
                uint32_t offset = ((y)*rmb->width)+x;
                unsigned char curR = rmb->blur[offset][0],
                              curG = rmb->blur[offset][1],
                              curB = rmb->blur[offset][2];

                rmb->blur[offset][0] = ( avgR * decFac ) + ( curR * incFac );
                rmb->blur[offset][1] = ( avgG * decFac ) + ( curG * incFac );
                rmb->blur[offset][2] = ( avgB * decFac ) + ( curB * incFac );
            }
            /*}*/

            cumul += ddy;
            x     += px;
            z     += pz;
        }
    } else {
        for ( i = 0x00; i < ddy; i++ ) {
            float incFac = (1.0f / ddy) * i, /* incremental factor */
                  decFac = (1.0f - incFac);  /* decremental factor */

            if ( cumul >= ddy ) {
                cumul -= ddy;
                x     += px;
            }

            /*if ( z < rmb->z[offset] ) {*/
            if ( ( y >= 0 && y < rmb->height ) && 
                 ( x >= 0 && x < rmb->width  ) ) {
                uint32_t offset = ((y)*rmb->width)+x;
                unsigned char curR = rmb->blur[offset][0],
                              curG = rmb->blur[offset][1],
                              curB = rmb->blur[offset][2];

                rmb->blur[offset][0] = ( avgR * decFac ) + ( curR * incFac );
                rmb->blur[offset][1] = ( avgG * decFac ) + ( curG * incFac );
                rmb->blur[offset][2] = ( avgB * decFac ) + ( curB * incFac );
            }
            /*}*/

            cumul += ddx;
            y     += py;
            z     += pz;
        }
    }
}

/******************************************************************************/
void r3dmotionblur_blurify ( R3DMOTIONBLUR *rmb, unsigned char (*img)[0x03], 
                             R3DSCENE *rsce ) {
    uint32_t y, x;
    uint32_t i;

    for ( y = 0x00; y < rmb->height; y++ ) {
        for ( x = 0x00; x < rmb->width; x++ ) {
            rmb->blur[(y*rmb->width)+x][0] = img[(y*rmb->width)+x][0];
            rmb->blur[(y*rmb->width)+x][1] = img[(y*rmb->width)+x][1];
            rmb->blur[(y*rmb->width)+x][2] = img[(y*rmb->width)+x][2];
        }
    }

    for ( y = 0x00; y < rmb->height; y++ ) {
        for ( x = 0x00; x < rmb->width; x++ ) {
            uint32_t offset = ( y * rmb->width ) + x;
            /*** If any vector has been set in the vector buffer ***/
            /*** then its Z-value is different from 0. ***/
            if ( rmb->z[offset] != 1.0f ) {
                R3DMOTIONTRACKPOINT src = { .x = x,
                                            .y = y,
                                            .z = rmb->z[offset] };
                R3DMOTIONVECTOR *mvec = &rmb->vectors[offset];

                r3dmotionblur_blurImage ( rmb, &src, mvec, img, rmb->strength );
            }
        }
    }

    for ( y = 0x00; y < rmb->height; y++ ) {
        for ( x = 0x00; x < rmb->width; x++ ) {
            uint32_t offset = ((y)*rmb->width)+x;

            img[offset][0] = rmb->blur[offset][0];
            img[offset][1] = rmb->blur[offset][1];
            img[offset][2] = rmb->blur[offset][2];
        }
    }

    /*** Prepare/reset the vector array for the next frame ***/
    memset ( rmb->vectors, 0x00, rmb->width * 
                                 rmb->height * sizeof ( R3DMOTIONVECTOR ) );

    for ( i = 0x00; i < rmb->width * rmb->height; i++ ) {
        /*** Set vectors Z-value to the maximum Z depth, in order to ***/
        /*** be able to sort them later. ***/
        rmb->z[i] = 1.0f;
    }
}

/******************************************************************************/
R3DMOTIONMESH *r3dmotionmesh_new ( R3DMESH *rms ) {
    uint32_t structSize = sizeof ( R3DMOTIONMESH );
    R3DMOTIONMESH *mms = ( R3DMOTIONMESH * ) calloc ( 0x01, structSize );

    if ( mms == NULL ) {
        fprintf ( stderr, "r3dmotionblur_newMesh: calloc failed!\n" );

        return NULL;
    }

    mms->rms = rms;

    return mms;
}

/******************************************************************************/
/**
 * Function must be called after each rendered frame, on each mesh object.
 * 
 *
 */
uint32_t r3dmotionmesh_track ( R3DMOTIONMESH *mms, double *CMX, 
                                                   double *PJX, int *VPX ) {
#ifdef unused
    uint32_t structSize = sizeof ( R3DMOTIONTRACKPOINT );
    uint32_t trackingArraySize = mms->rms->nbrver * structSize;
    double IDX[] = { 1.0f, 0.0f, 0.0f, 0.0f,
                     0.0f, 1.0f, 0.0f, 0.0f,
                     0.0f, 0.0f, 1.0f, 0.0f,
                     0.0f, 0.0f, 0.0f, 1.0f };
    R3DMOTIONTRACKPOINT *trackingPoint = NULL;
    /* "trace" tells us if we have the starting point and the ending point */
    uint32_t trace = 0;
    uint32_t i;
    double fullmatrix[16];

    if ( mms->startPoint == NULL ) {
        mms->startPoint = ( R3DMOTIONTRACKPOINT * ) 
                                    calloc ( 0x01, trackingArraySize );
        if ( mms->startPoint == NULL ) {
            fprintf ( stderr, "r3dmotionblur_newMesh: startPoint"
                              " calloc failed!\n" );
            return 0;
        }

        trackingPoint = mms->startPoint;
    } else {
        if ( mms->endPoint == NULL ) {
            mms->endPoint = ( R3DMOTIONTRACKPOINT * )
                                      calloc ( 0x01, trackingArraySize );
            if ( mms->endPoint == NULL ) {
                fprintf ( stderr, "r3dmotionblur_newMesh: endPoint"
                                  " calloc failed!\n" );

                free ( mms->startPoint );
                return 0;
            }
        }

        trackingPoint = mms->endPoint;

        trace = 1;
    }

    g3dcore_multmatrix (  ((R3DOBJECT*)mms->rms)->wmatrix, CMX, fullmatrix);

    for ( i = 0x00; i < mms->rms->nbrver; i++ ) {
#ifdef unused
        gluProject ( mms->rms->rver[i].ori.x, 
                     mms->rms->rver[i].ori.y, 
                     mms->rms->rver[i].ori.z, 
                     /* R3DVERTICES are already transformed with the object's */
                     /* modelview matrix. So we use an identitiy matrix.      */
                     fullmatrix,
                     PJX,
                     VPX,
                     &trackingPoint[i].x,
                     &trackingPoint[i].y,
                     &trackingPoint[i].z );
        /* OpenGL screen cooords are inverted on the Y-Axis */
#endif
        trackingPoint[i].y = VPX[0x03] - trackingPoint[i].y;
    }

    return trace;
#endif
}

/******************************************************************************/
void r3dmotionblur_traceMesh ( R3DMOTIONBLUR *rmb, R3DMOTIONMESH *mms ) {
    uint32_t i;

    /*** For each triangle, fill the vector buffer ***/
    for ( i = 0x00; i < mms->rms->nbrfac; i++ ) {
#ifdef unused
        uint32_t ids[3] = { mms->rms->rfac[i].ver[0x00].id,
                            mms->rms->rfac[i].ver[0x01].id,
                            mms->rms->rfac[i].ver[0x02].id };
        R3DMOTIONTRACKPOINT *startPoint[3] = { &mms->startPoint[ids[0]],
                                               &mms->startPoint[ids[1]],
                                               &mms->startPoint[ids[2]] },
                            *endPoint[3]   = { &mms->endPoint[ids[0]],
                                               &mms->endPoint[ids[1]],
                                               &mms->endPoint[ids[2]] };

        r3dmotionblur_fillVectorBuffer ( rmb, startPoint, endPoint );
#endif
    }
}

/******************************************************************************/
R3DMOTIONMESH *r3dmotionblur_getMotionMesh ( R3DMOTIONBLUR *rmb, uint32_t id ) {
    LIST *ltmpMotionMesh = rmb->lMotionMesh;

    while ( ltmpMotionMesh ) {
        R3DMOTIONMESH *mms = ( R3DMOTIONMESH * ) ltmpMotionMesh->data;

        if ( mms->id == id ) {
            return mms;
        }

        ltmpMotionMesh = ltmpMotionMesh->next;
    }

    return NULL;
}

/******************************************************************************/
void r3dmotionblur_trace ( R3DMOTIONBLUR *rmb, R3DSCENE *rsce ) {
    LIST *ltmprob = rsce->lrob;
#ifdef unused
    while ( ltmprob ) {
        R3DOBJECT *rob = ( R3DOBJECT * ) ltmprob->data;
        if ( rob->obj->type == G3DMESHTYPE ) {
            R3DMESH *rms = ( R3DMESH * ) rob;
            R3DMOTIONMESH *mms = r3dmotionblur_getMotionMesh ( rmb, rob->id );

            if ( mms == NULL ) {
                mms = r3dmotionmesh_new ( rms );

                list_insert ( &rmb->lMotionMesh, mms );
            } else {
                mms->rms = rms; /* update rms */
            }
            mms->id = rob->id; /* this should be put in the init function **/

            if ( r3dmotionmesh_track ( mms, rsce->area.rcam->MVX, 
                                            rsce->area.rcam->PJX, 
                                            rsce->area.rcam->VPX ) ) {
                r3dmotionblur_traceMesh ( rmb, mms );

                if ( mms->endPoint ) {
                    uint32_t structSize = sizeof ( R3DMOTIONTRACKPOINT );
                    uint32_t arraySize = mms->rms->nbrver * structSize;
                    /*** the former end point is now the starting point ***/
                    memcpy ( mms->startPoint,
                             mms->endPoint, arraySize );
                }
            }
        }

        ltmprob = ltmprob->next;
    }
#endif
}


