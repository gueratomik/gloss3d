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

#define NBITER 0x10

void r3dmotionblur_clearZBuffer ( R3DMOTIONBLUR * );
void r3dmotionblur_drawInterpolatedFrame ( R3DMOTIONBLUR *rmb, 
                                           uint32_t fromGeometryID,
                                           uint32_t toGeometryID,
                                           float,
                                           float );
void r3dmotionblur_traceMesh ( R3DMOTIONBLUR *rmb, R3DMOTIONMESH *mms,
                                                   uint32_t fromGeometryID,
                                                   uint32_t toGeometryID,
                                                   float,
                                                   float );
void r3dmotionblur_interpolateTriangles ( R3DMOTIONBLUR *rmb, 
                                          R3DMOTIONPOINT *srcTri[0x03], 
                                          R3DMOTIONPOINT *dstTri[0x03],
                                          R3DMOTIONPOINT *midTri[0x03],
                                          float,
                                          float );
void r3dmotionblur_drawInterpolatedTriangle ( R3DMOTIONBLUR *rmb, 
                                              R3DMOTIONPOINT *itrTri[0x03],
                                              R3DMOTIONPOINT *midTri[0x03],
                                              float fac, uint32_t skipZ );
void r3dmotionblur_initMotionMeshes ( R3DMOTIONBLUR *rmb, R3DSCENE *rsce,
                                                          uint32_t  nbSteps );

void r3dmotionblur_fillMotionMeshes ( R3DMOTIONBLUR *rmb, R3DSCENE *rsce,
                                                          uint32_t  stepID );
R3DMOTIONMESH *r3dmotionblur_getMotionMesh ( R3DMOTIONBLUR *rmb, 
                                             R3DMESH       *rms );
R3DMOTIONMESH *r3dmotionmesh_new ( R3DMESH *rms, uint32_t nbSteps );
void r3dmotionmesh_init ( R3DMOTIONMESH *mms, R3DMESH *rms, 
                                              uint32_t nbGeometry );
void r3dmotionblur_free ( R3DMOTIONBLUR *rmb );
void r3dmotionmesh_free ( R3DMOTIONMESH *mms );
void r3dmotionmesh_fillGeometry ( R3DMOTIONMESH *mms, R3DMESH *geomes,
                                                      uint32_t geometryID );
void r3dmotionblur_blurify ( R3DMOTIONBLUR *rmb, unsigned char (*img)[0x03] );
void r3dmotionblur_clearZBuffer ( R3DMOTIONBLUR *rmb );
void r3dmotionblur_interpolateHLine ( R3DMOTIONBLUR  *rmb,
                                      int32_t y,
                                      R3DMOTIONPOINT *midTri[0x03],
                                      float incFac,
                                      uint32_t skipZ );
void r3dmotionblur_fillHLineBuffer ( R3DMOTIONBLUR   *rmb,
                                     R3DMOTIONPOINT  *srcPoint,
                                     R3DMOTIONPOINT  *dstPoint );
R3DMOTIONBLUR *r3dmotionblur_new ( uint32_t width,
                                   uint32_t height, 
                                   float    strength );
R3DFILTER *r3dfilter_VectorMotionBlur_new ( uint32_t width,
                                            uint32_t height,
                                            float    strength );
void filtervectormotionblur_free ( R3DFILTER *fil );
uint32_t filtervectormotionblur_draw ( R3DFILTER *fil, R3DSCENE *rsce,
                                                       float frameID,
                                                       unsigned char (*img)[0x03], 
                                                       uint32_t from, 
                                                       uint32_t to, 
                                                       uint32_t depth, 
                                                       uint32_t width );
void r3dmotionblur_copyImage ( R3DMOTIONBLUR *, unsigned char (*)[0x03] );

/******************************************************************************/
uint32_t filtervectormotionblur_draw ( R3DFILTER *fil, R3DSCENE *rsce,
                                                       float frameID,
                                                       unsigned char (*img)[0x03], 
                                                       uint32_t from, 
                                                       uint32_t to, 
                                                       uint32_t depth, 
                                                       uint32_t width ) {
    R3DMOTIONBLUR *rmb = ( R3DMOTIONBLUR * ) fil->data;
    G3DSCENE *sce = ( G3DSCENE  * ) ((R3DOBJECT*)rsce)->obj;
    G3DCAMERA *cam = ( G3DCAMERA * ) ((R3DOBJECT*)rsce->area.rcam)->obj;
    LIST *lfilters = list_copy ( rsce->lfilters );
    R3DFILTER *tofrm = r3dfilter_getByName ( rsce->lfilters, 
                                                 GOTOFRAMEFILTERNAME );
    float middleFrame = frameID - ( rmb->strength * 0.5f );
    float fromFrame = middleFrame - ( rmb->strength * 0.5f );
    /*** toFrame is normally equal to frameID ***/
    float   toFrame = middleFrame + ( rmb->strength * 0.5f );
    uint32_t nbSamples = 0x01;
    /*** 1 step for start frame, 1 for middle frame, 1 for end frame ***/
    /*** and then, eventually 1 for each inter-frame (backward+forward) ***/
    uint32_t nbSteps = ( nbSamples * 0x02 );
    float deltaFrame = ( nbSteps ) ? ( toFrame - fromFrame ) / nbSteps : 0.0f;
    float currentFrame = fromFrame;
    float visibilityDelta = 1.0f / nbSamples;
    float visibilityStart = 0.0f;
    float visibilityEnd   = visibilityStart;
    R3DSCENE *middlersce;
    double *MVX, *PJX;
    uint32_t i;

    /*** dont motion-blurize the first frame ***/
    if ( rsce->curframe == rsce->startframe ) return 0x00;

    /*** remove motion blur from the filter list ***/
    list_remove ( &lfilters, r3dfilter_getByName ( lfilters, fil->name  ) );


    /*** Because gotonextframe_draw will jump to frame + 1.0f, we have ***/
    /*** to substract 1.0f ***/
    if ( tofrm ) {
        tofrm->draw ( tofrm, NULL, middleFrame - 1.0f, NULL, 0x00, 0x00, 
                                                             0x00, 0x00 );
    }

    /*** Prepare the scene ***/
    middlersce = r3dscene_new ( sce, cam, rsce->area.x1,
                                          rsce->area.y1,
                                          rsce->area.x2,
                                          rsce->area.y2,
                                          rsce->area.width,
                                          rsce->area.height, 
                                          rsce->background,
                                          rsce->startframe, 
                                          rsce->endframe,
                                          rsce->wireframe,
                                          rsce->wireframeLighting,
                                          rsce->wireframeColor,
                                          rsce->wireframeThickness,
                                          lfilters );

    /** create motion blur meshes from the meshes calculated in middle frame **/
    /** by the above call to r3dscene_new() **/
    r3dmotionblur_initMotionMeshes ( rmb, middlersce, nbSteps + 1 );

    /*** register this child renderscene in case we need to cancel it ***/
    r3dscene_addSubRender ( rsce, middlersce );

    /*** Render and free the middle frame ***/
    r3dscene_render ( middlersce );

    rmb->curimg = middlersce->area.img;

    memcpy ( rmb->cpyimg, rmb->curimg, rsce->area.width * rsce->area.height * 0x03 );

    /*** unregister this child renderscene. No need to cancel it ***/
    /*** anymore, all threads are over after r3dscene_render().  ***/
    r3dscene_removeSubRender ( rsce, middlersce );

    for ( i = 0x00; i <= nbSteps ; i++ ) {
        if ( i == ( nbSteps / 2 ) ) {
            r3dmotionblur_fillMotionMeshes ( rmb, middlersce, i );
        } else if ( i == ( nbSteps ) ) {
            r3dmotionblur_fillMotionMeshes ( rmb, rsce, i );
        } else {
            R3DSCENE *steprsce;

            /*** Because gotonextframe_draw will jump to frame + 1.0f, we have ***/
            /*** to substract 1.0f ***/
            if ( tofrm ) {
                tofrm->draw ( tofrm, NULL, currentFrame - 1.0f, NULL, 0x00, 0x00, 
                                                                      0x00, 0x00 );
            }

            /*** Prepare the scene ***/
            steprsce = r3dscene_new ( sce, cam, 
                                           rsce->area.x1,
                                           rsce->area.y1,
                                           rsce->area.x2,
                                           rsce->area.y2,
                                           rsce->area.width,
                                           rsce->area.height, 
                                           rsce->background,
                                           rsce->startframe, 
                                           rsce->endframe,
                                           rsce->wireframe,
                                           rsce->wireframeLighting,
                                           rsce->wireframeColor,
                                           rsce->wireframeThickness,
                                           NULL );

            /*** create another set of geometry data ***/
            r3dmotionblur_fillMotionMeshes ( rmb, steprsce, i );

            /*** Free the current frame ***/
            r3dobject_free  ( ( R3DOBJECT * ) steprsce );
        }

        currentFrame += deltaFrame;
    }

    r3dmotionblur_copyImage ( rmb, middlersce->area.img );

    for ( i = 0x00; i < nbSteps; i++ ) {
        int n = i + 0x01;

        /* reverse visibility when we hit the middle frame */
        if ( i == ( nbSteps / 2 ) ) {
            visibilityDelta = - visibilityDelta;
        }

        visibilityEnd  += visibilityDelta;
/*printf("%f %f\n", visibilityStart, visibilityEnd );*/
        r3dmotionblur_clearZBuffer ( rmb );
        r3dmotionblur_drawInterpolatedFrame ( rmb, i, n, visibilityStart,
                                                         visibilityEnd );

        visibilityStart = visibilityEnd;

    }

    r3dmotionblur_blurify ( rmb, img );

    /*** Free the current frame ***/
    r3dobject_free  ( ( R3DOBJECT * ) middlersce );

    /*r3dscene_import ( );*/

    /* return 2 to interrupt previous rendering. */
    /* Filter type must be FILTERBEFORE */
    return 0x02;
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

    fil = r3dfilter_new ( FILTERBEFORE, VECTORMOTIONBLURFILTERNAME,
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

    /*** Store vectors Z position. We don't need to store x and y because ***/
    /*** they are given by the screen coordinates. ***/
    rmb->z = calloc ( width * height, sizeof ( double ) );

    if ( rmb->z == NULL ) {
        fprintf ( stderr, "r3dmotionblur_new: z calloc failed\n");

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
    rmb->blur = ( uint32_t * ) calloc ( width * height, 0x03 * sizeof( uint32_t ) );

    if ( rmb->blur == NULL ) {
        fprintf ( stderr, "r3dmotionblur_new: blur calloc failed\n");

        free ( rmb->z );
        free ( rmb->hlines );
        free ( rmb );

        return NULL;
    }

    rmb->cpyimg = ( unsigned char * ) calloc ( width * height, 0x03 );

    rmb->div = calloc ( width * height, sizeof ( uint32_t ) );

    rmb->width    = width;
    rmb->height   = height;
    rmb->strength = strength;

    return rmb;
}

/******************************************************************************/
void r3dmotionblur_fillHLineBuffer ( R3DMOTIONBLUR   *rmb,
                                     R3DMOTIONPOINT  *srcPoint,
                                     R3DMOTIONPOINT  *dstPoint ) {
    int32_t dx  = ( dstPoint->x - srcPoint->x ),
            ddx = ( dx == 0x00 ) ? 0x01 : abs ( dx ),
            dy  = ( dstPoint->y - srcPoint->y ),
            ddy = ( dy == 0x00 ) ? 0x01 : abs ( dy ),
            dd  = ( ddx > ddy ) ? ddx : ddy;
    double  dz  = dstPoint->z  - srcPoint->z , pz    = ( dz    / dd );
    int px = ( dx > 0x00 ) ? 1 : -1, 
        py = ( dy > 0x00 ) ? 1 : -1;
    int32_t x = srcPoint->x, 
            y = srcPoint->y;
    double  z = srcPoint->z;
    int i, cumul = 0x00;
    float ratio0 = srcPoint->ratio0,
          ratio1 = srcPoint->ratio1,
          ratio2 = srcPoint->ratio2;
    float dratio0 = dstPoint->ratio0 - srcPoint->ratio0, pratio0 = ( dratio0 / (dd+1) ),
          dratio1 = dstPoint->ratio1 - srcPoint->ratio1, pratio1 = ( dratio1 / (dd+1) ),
          dratio2 = dstPoint->ratio2 - srcPoint->ratio2, pratio2 = ( dratio2 / (dd+1) );


    if ( ddx > ddy ) {
        for ( i = 0x00; i <= ddx; i++ ) {
            if ( cumul >= ddx ) {
                cumul -= ddx;
                y     += py;
            }

            if ( ( y >= 0 ) && ( y < rmb->height ) ) {
                if ( rmb->hlines[y].inited == 0x00 ) {
                    rmb->hlines[y].inited = 0x01;

                    rmb->hlines[y].x1 = rmb->hlines[y].x2 = x;
                    rmb->hlines[y].z1 = rmb->hlines[y].z2 = z;

                    rmb->hlines[y].start.ratio0 = rmb->hlines[y].end.ratio0 = ratio0;
                    rmb->hlines[y].start.ratio1 = rmb->hlines[y].end.ratio1 = ratio1;
                    rmb->hlines[y].start.ratio2 = rmb->hlines[y].end.ratio2 = ratio2;
                } else {
                    if ( x < rmb->hlines[y].x1 ) {
                        rmb->hlines[y].x1 = x;
                        rmb->hlines[y].z1 = z;

                        rmb->hlines[y].start.ratio0 = ratio0;
                        rmb->hlines[y].start.ratio1 = ratio1;
                        rmb->hlines[y].start.ratio2 = ratio2;
                    }

                    if ( x > rmb->hlines[y].x2 ) {
                        rmb->hlines[y].x2 = x;
                        rmb->hlines[y].z2 = z;

                        rmb->hlines[y].end.ratio0 = ratio0;
                        rmb->hlines[y].end.ratio1 = ratio1;
                        rmb->hlines[y].end.ratio2 = ratio2;
                    }

                    rmb->hlines[y].inited = 0x02;
        	    }
            }

            cumul += ddy;
            x     += px;

            z      += pz;
            ratio0 += pratio0;
            ratio1 += pratio1;
            ratio2 += pratio2;
        }
    } else {
        for ( i = 0x00; i <= ddy; i++ ) {
            if ( cumul >= ddy ) {
                cumul -= ddy;
                x     += px;
            }

            if ( ( y >= 0 ) && ( y < rmb->height ) ) {
                if ( rmb->hlines[y].inited == 0x00 ) {
                    rmb->hlines[y].inited = 0x01;

                    rmb->hlines[y].x1 = rmb->hlines[y].x2 = x;
                    rmb->hlines[y].z1 = rmb->hlines[y].z2 = z;

                    rmb->hlines[y].start.ratio0 = rmb->hlines[y].end.ratio0 = ratio0;
                    rmb->hlines[y].start.ratio1 = rmb->hlines[y].end.ratio1 = ratio1;
                    rmb->hlines[y].start.ratio2 = rmb->hlines[y].end.ratio2 = ratio2;
                } else {
                    if ( x < rmb->hlines[y].x1 ) {
                        rmb->hlines[y].x1 = x;
                        rmb->hlines[y].z1 = z;

                        rmb->hlines[y].start.ratio0 = ratio0;
                        rmb->hlines[y].start.ratio1 = ratio1;
                        rmb->hlines[y].start.ratio2 = ratio2;
                    }

                    if ( x > rmb->hlines[y].x2 ) {
                        rmb->hlines[y].x2 = x;
                        rmb->hlines[y].z2 = z;

                        rmb->hlines[y].end.ratio0 = ratio0;
                        rmb->hlines[y].end.ratio1 = ratio1;
                        rmb->hlines[y].end.ratio2 = ratio2;
                    }

                    rmb->hlines[y].inited = 0x02;
                }
            }

            cumul += ddx;
            y     += py;

            z      += pz;
            ratio0 += pratio0;
            ratio1 += pratio1;
            ratio2 += pratio2;
        }
    }
}

/****************************************************************************/
void r3dmotionblur_interpolateHLine ( R3DMOTIONBLUR  *rmb,
                                      int32_t y,
                                      R3DMOTIONPOINT *midTri[0x03],
                                      float incFac,
                                      uint32_t skipZ ) {
    R3DMOTIONHLINE *hline = &rmb->hlines[y];
    int32_t x1 = hline->x1, x2 = hline->x2;
    int32_t dx = x2 - x1, ddx = ( dx == 0x00 ) ? 0x01 : abs ( dx );
    double dz    = hline->z2    - hline->z1     , pz    = ( dz    / ddx );
    long  px = ( dx > 0x00 ) ? 1 : -1;
    double z    = hline->z1;
    float ratio0 = hline->start.ratio0,
          ratio1 = hline->start.ratio1,
          ratio2 = hline->start.ratio2;
    float dratio0 = hline->end.ratio0 - hline->start.ratio0, pratio0 = ( dratio0 / (ddx) ),
          dratio1 = hline->end.ratio1 - hline->start.ratio1, pratio1 = ( dratio1 / (ddx) ),
          dratio2 = hline->end.ratio2 - hline->start.ratio2, pratio2 = ( dratio2 / (ddx) );
    int i;
    uint32_t R, G, B;
    incFac = 1.0f - cos ( incFac * 0.5f * M_PI );
    float decFac = 1.0f - incFac;

    for ( i = 0x00; i < ddx; i++ ) {
        uint32_t offset = ((y)*rmb->width)+x1;

        if ( x1 >= 0 && x1 < rmb->width ) {
            if ( skipZ || ( z < ( rmb->z[offset] ) ) ) {
                int32_t midTriX = ( midTri[0x00]->x * ratio0 ) +
                                  ( midTri[0x01]->x * ratio1 ) +
                                  ( midTri[0x02]->x * ratio2 );
                int32_t midTriY = ( midTri[0x00]->y * ratio0 ) +
                                  ( midTri[0x01]->y * ratio1 ) +
                                  ( midTri[0x02]->y * ratio2 );
                uint32_t midTriOffset = midTriY * rmb->width + midTriX;

                R = ( uint32_t ) rmb->curimg [midTriOffset][0x00];
                G = ( uint32_t ) rmb->curimg [midTriOffset][0x01];
                B = ( uint32_t ) rmb->curimg [midTriOffset][0x02];

                /* prevent drawing the same pixel (e.g on edges) */
                /*if ( rmb->div[offset] == 0x00 ) {*/
                    rmb->blur[offset][0x00] = ( ( uint32_t ) ( incFac * R ) + ( uint32_t ) ( decFac * rmb->blur[offset][0] ) );
                    rmb->blur[offset][0x01] = ( ( uint32_t ) ( incFac * G ) + ( uint32_t ) ( decFac * rmb->blur[offset][1] ) );
                    rmb->blur[offset][0x02] = ( ( uint32_t ) ( incFac * B ) + ( uint32_t ) ( decFac * rmb->blur[offset][2] ) );

                    rmb->div[offset] = 1;
                /*}*/

                rmb->z[offset] = z;
            }
        }

        x1 += px;

        z      += pz;
        ratio0 += pratio0;
        ratio1 += pratio1;
        ratio2 += pratio2;
    }
}

/******************************************************************************/
void r3dmotionblur_clearZBuffer ( R3DMOTIONBLUR *rmb ) {
    uint32_t i;

    for ( i = 0x00; i < rmb->width * rmb->height; i++ ) {
        /*** Set vectors Z-value to the maximum Z depth, in order to ***/
        /*** be able to sort them later. ***/
        rmb->z[i] = 1.0f;
    }
}

/******************************************************************************/
void r3dmotionblur_copyImage ( R3DMOTIONBLUR *rmb, unsigned char (*img)[0x03] ) {
    uint32_t y, x;

    for ( y = 0x00; y < rmb->height; y++ ) {
        for ( x = 0x00; x < rmb->width; x++ ) {
            uint32_t offset = ((y)*rmb->width)+x;

            rmb->blur[offset][0] = img[offset][0];
            rmb->blur[offset][1] = img[offset][1];
            rmb->blur[offset][2] = img[offset][2];
        }
    }
}

/******************************************************************************/
void r3dmotionblur_blurify ( R3DMOTIONBLUR *rmb, unsigned char (*img)[0x03] ) {
    uint32_t y, x;

    for ( y = 0x00; y < rmb->height; y++ ) {
        for ( x = 0x00; x < rmb->width; x++ ) {
            uint32_t offset = ((y)*rmb->width)+x;

            /* dark pixel are considered unset and replaced by the pixels from */
            /* the original image */
            if ( rmb->div[offset] ) {
                img[offset][0] = rmb->blur[offset][0] / rmb->div[offset];
                img[offset][1] = rmb->blur[offset][1] / rmb->div[offset];
                img[offset][2] = rmb->blur[offset][2] / rmb->div[offset];
            } else {
                img[offset][0] = rmb->curimg[offset][0];
                img[offset][1] = rmb->curimg[offset][1];
                img[offset][2] = rmb->curimg[offset][2];
            }
        }
    }
}


/******************************************************************************/
void r3dmotionmesh_fillGeometry ( R3DMOTIONMESH *mms, R3DMESH *geomes,
                                                      uint32_t geometryID ) {
    uint32_t i;

    for ( i = 0x00; i < geomes->nbrver; i++ ) {
        mms->geometry[geometryID][i].x = geomes->rver[i].scr.x;
        mms->geometry[geometryID][i].y = geomes->rver[i].scr.y;
        mms->geometry[geometryID][i].z = geomes->rver[i].scr.z;
    }
}

/******************************************************************************/
void r3dmotionmesh_free ( R3DMOTIONMESH *mms ) {
    uint32_t i;

    for ( i = 0x00; i < mms->nbGeometry; i++ ) {
        if ( mms->geometry[i] ) free ( mms->geometry[i] );
    }

    free ( mms->geometry );

    free ( mms );
}

/******************************************************************************/
void r3dmotionblur_free ( R3DMOTIONBLUR *rmb ) {
    if ( rmb->z       ) free ( rmb->z       );
    if ( rmb->hlines  ) free ( rmb->hlines  );
    if ( rmb->blur    ) free ( rmb->blur    );

    list_free ( &rmb->lMotionMesh, r3dmotionmesh_free );

    free ( rmb );
}

/******************************************************************************/
void r3dmotionmesh_init ( R3DMOTIONMESH *mms, R3DMESH *rms, 
                                              uint32_t nbGeometry ) {
    uint32_t ptrSize = sizeof ( R3DMOTIONPOINT * );
    uint32_t i;

    mms->nbGeometry = nbGeometry;
    mms->rms        = rms;

    mms->geometry = ( R3DMOTIONPOINT ** ) calloc ( nbGeometry, ptrSize );

    if ( rms->nbrver ) { /* beware the empty meshes */
        for ( i = 0x00; i < nbGeometry; i++ ) {
            uint32_t structSize =  sizeof ( R3DMOTIONPOINT );
            uint32_t nbrver = rms->nbrver;

            mms->geometry[i] = ( R3DMOTIONPOINT * ) calloc ( nbrver, 
                                                             structSize );
        }
    }
}

/******************************************************************************/
R3DMOTIONMESH *r3dmotionmesh_new ( R3DMESH *rms, uint32_t nbSteps ) {
    uint32_t structSize = sizeof ( R3DMOTIONMESH );
    R3DMOTIONMESH *mms = ( R3DMOTIONMESH * ) calloc ( 0x01, structSize );

    if ( mms == NULL ) {
        fprintf ( stderr, "r3dmotionblur_newMesh: calloc failed!\n" );

        return NULL;
    }

    r3dmotionmesh_init ( mms, rms, nbSteps );

    return mms;
}

/******************************************************************************/
R3DMOTIONMESH *r3dmotionblur_getMotionMesh ( R3DMOTIONBLUR *rmb, 
                                             R3DMESH       *rms ) {
    LIST *ltmpMotionMesh = rmb->lMotionMesh;

    while ( ltmpMotionMesh ) {
        R3DMOTIONMESH *mms = ( R3DMOTIONMESH * ) ltmpMotionMesh->data;

        if ( ((R3DOBJECT*)mms->rms)->id == ((R3DOBJECT*)rms)->id ) {
            return mms;
        }

        ltmpMotionMesh = ltmpMotionMesh->next;
    }

    return NULL;
}

/******************************************************************************/
void r3dmotionblur_fillMotionMeshes ( R3DMOTIONBLUR *rmb, R3DSCENE *rsce,
                                                          uint32_t  stepID ) {
    LIST *ltmprob = rsce->lrob;

    while ( ltmprob ) {
        R3DOBJECT *rob = ( R3DOBJECT * ) ltmprob->data;

        if ( rob->obj->type & MESH ) {
            R3DMESH       *rms = ( R3DMESH * ) rob;
            R3DMOTIONMESH *mms = r3dmotionblur_getMotionMesh ( rmb, rms );

            if ( mms ) {
                r3dmotionmesh_fillGeometry ( mms, rms, stepID );
            }
        }

        ltmprob = ltmprob->next;
    }
}

/******************************************************************************/
/*** create motion meshes ***/
void r3dmotionblur_initMotionMeshes ( R3DMOTIONBLUR *rmb, R3DSCENE *rsce,
                                                          uint32_t  nbSteps ) {
    LIST *ltmprob = rsce->lrob;

    while ( ltmprob ) {
        R3DOBJECT *rob = ( R3DOBJECT * ) ltmprob->data;
        if ( rob->obj->type == G3DMESHTYPE ) {
            R3DMESH *rms = ( R3DMESH * ) rob;
            R3DMOTIONMESH *mms = r3dmotionmesh_new ( rms, nbSteps );

            list_insert ( &rmb->lMotionMesh, mms );
        }

        ltmprob = ltmprob->next;
    }
}

/******************************************************************************/
void r3dmotionblur_drawInterpolatedTriangle ( R3DMOTIONBLUR *rmb, 
                                              R3DMOTIONPOINT *itrTri[0x03],
                                              R3DMOTIONPOINT *midTri[0x03],
                                              float fac,
                                              uint32_t skipZ ) {
    uint32_t i;

    memset ( rmb->hlines, 0x00, sizeof ( R3DMOTIONHLINE ) * rmb->height );

    for ( i = 0x00; i < 0x03; i++ ) {
        int n = ( i + 0x01 ) % 0x03;

        if ( itrTri[i]->x < itrTri[n]->x ) {
            r3dmotionblur_fillHLineBuffer ( rmb, itrTri[i], itrTri[n] );
        } else {
            r3dmotionblur_fillHLineBuffer ( rmb, itrTri[n], itrTri[i] );
        }
    }

    /*** Then interplate the vectors along the horizontal lines ***/
    for ( i = 0; i < rmb->height; i++ ) {
        if ( rmb->hlines[i].inited == 0x02 ){
            if ( ( ( rmb->hlines[i].x1 < rmb->width ) && 
                   ( rmb->hlines[i].x2 > 0          ) ) ||
                 ( ( rmb->hlines[i].x2 < rmb->width ) && 
                   ( rmb->hlines[i].x1 > 0          ) ) ) {
                r3dmotionblur_interpolateHLine ( rmb, i, midTri, fac, skipZ );
            }
        }
    }
}

/******************************************************************************/
void r3dmotionblur_interpolateTriangles ( R3DMOTIONBLUR *rmb, 
                                          R3DMOTIONPOINT *srcTri[0x03], 
                                          R3DMOTIONPOINT *dstTri[0x03],
                                          R3DMOTIONPOINT *midTri[0x03],
                                          float visibilityStart,
                                          float visibilityEnd ) {
    R3DMOTIONVECTOR mvec[0x03] = { { .x = dstTri[0x00]->x - srcTri[0x00]->x,
                                     .y = dstTri[0x00]->y - srcTri[0x00]->y,
                                     .z = dstTri[0x00]->z - srcTri[0x00]->z,
                                     .ratio0 = 0.950f,
                                     .ratio1 = 0.025f,
                                     .ratio2 = 0.025f },
                                   { .x = dstTri[0x01]->x - srcTri[0x01]->x,
                                     .y = dstTri[0x01]->y - srcTri[0x01]->y,
                                     .z = dstTri[0x01]->z - srcTri[0x01]->z,
                                     .ratio0 = 0.025f,
                                     .ratio1 = 0.950f,
                                     .ratio2 = 0.025f },
                                   { .x = dstTri[0x02]->x - srcTri[0x02]->x,
                                     .y = dstTri[0x02]->y - srcTri[0x02]->y,
                                     .z = dstTri[0x02]->z - srcTri[0x02]->z,
                                     .ratio0 = 0.025f,
                                     .ratio1 = 0.025f,
                                     .ratio2 = 0.950f } };
    G2DVECTOR vlen[0x03] = { { .x = mvec[0x00].x,
                               .y = mvec[0x00].y },
                             { .x = mvec[0x01].x,
                               .y = mvec[0x01].y },
                             { .x = mvec[0x02].x,
                               .y = mvec[0x02].y } };
    uint32_t len[0x03] = { g2dvector_length ( &vlen[0x00] ), 
                           g2dvector_length ( &vlen[0x01] ),
                           g2dvector_length ( &vlen[0x02] ) };
    uint32_t maxLen = ( len[0] > len[1] ) ? ( ( len[0] > len[2] ) ? len[0] : len[2] ) : ( ( len[1] > len[2] ) ? len[1] : len[2] );
    uint32_t iter = /*maxLen*/16;
    uint32_t skipZ = 0x00;

    if ( maxLen ) {
        uint32_t i;

        for ( i = 0x00; i < iter; i++ ) {
            float pos = ( float ) i / iter;
            float fac = ( float ) ( visibilityEnd - 
                                    visibilityStart ) * pos;
            R3DMOTIONPOINT p[0x03] = { { .x = srcTri[0x00]->x + 
                                              ( mvec[0x00].x * pos ),
                                         .y = srcTri[0x00]->y + 
                                              ( mvec[0x00].y * pos ),
                                         .z = srcTri[0x00]->z + 
                                              ( mvec[0x00].z * pos ),
                                         .ratio0 = mvec[0x00].ratio0,
                                         .ratio1 = mvec[0x00].ratio1,
                                         .ratio2 = mvec[0x00].ratio2 },
                                       { .x = srcTri[0x01]->x + 
                                              ( mvec[0x01].x * pos ),
                                         .y = srcTri[0x01]->y + 
                                              ( mvec[0x01].y * pos ),
                                         .z = srcTri[0x01]->z + 
                                              ( mvec[0x01].z * pos ),
                                         .ratio0 = mvec[0x01].ratio0,
                                         .ratio1 = mvec[0x01].ratio1,
                                         .ratio2 = mvec[0x01].ratio2 },
                                       { .x = srcTri[0x02]->x + 
                                              ( mvec[0x02].x * pos ),
                                         .y = srcTri[0x02]->y +
                                              ( mvec[0x02].y * pos ),
                                         .z = srcTri[0x02]->z + 
                                              ( mvec[0x02].z * pos ),
                                         .ratio0 = mvec[0x02].ratio0,
                                         .ratio1 = mvec[0x02].ratio1,
                                         .ratio2 = mvec[0x02].ratio2 } };
            R3DMOTIONPOINT *itrTri[0x03] = { &p[0x00],
                                             &p[0x01], 
                                             &p[0x02] };

            if ( fac < 0.0f ) fac = 1.0f + fac;

            r3dmotionblur_drawInterpolatedTriangle ( rmb, itrTri,
                                                          midTri,
                                                          fac,
                                                          skipZ );

            skipZ = 0x01;
        }
    }
}

/******************************************************************************/
/* static void rearrangeIds ( uint32_t ids[0x03] ) {
    uint32_t i;

    for ( i = 0x00; i < 0x03; i++ ) {
        int n = ( i + 0x01 ) % 0x03;
        int m = ( i + 0x02 ) % 0x03;

        if ( ids[i] > ids[n] ) {
            uint32_t tmpid = ids[i];

            ids[i] = ids[n];
            ids[n] = tmpid;
        }

        if ( ids[n] > ids[m] ) {
            uint32_t tmpid = ids[n];

            ids[n] = ids[m];
            ids[m] = tmpid;
        }
    }
} */

/******************************************************************************/
void r3dmotionblur_traceMesh ( R3DMOTIONBLUR *rmb, R3DMOTIONMESH *mms,
                                                   uint32_t fromGeometryID,
                                                   uint32_t toGeometryID,
                                                   float visibilityStart,
                                                   float visibilityEnd ) {
    uint32_t i;

    /*** For each triangle, fill the vector buffer ***/
    for ( i = 0x00; i < mms->rms->nbrfac; i++ ) {
        uint32_t ids[3] = { mms->rms->rfac[i].rverID[0x00],
                            mms->rms->rfac[i].rverID[0x01],
                            mms->rms->rfac[i].rverID[0x02] };

        R3DMOTIONPOINT *midTri[0x03] = { &mms->geometry[(mms->nbGeometry-1)/2][ids[0x00]],
                                         &mms->geometry[(mms->nbGeometry-1)/2][ids[0x01]],
                                         &mms->geometry[(mms->nbGeometry-1)/2][ids[0x02]] },
                       *srcTri[0x03] = { &mms->geometry[fromGeometryID][ids[0x00]],
                                         &mms->geometry[fromGeometryID][ids[0x01]],
                                         &mms->geometry[fromGeometryID][ids[0x02]] },
                       *dstTri[0x03] = { &mms->geometry[toGeometryID][ids[0x00]],
                                         &mms->geometry[toGeometryID][ids[0x01]],
                                         &mms->geometry[toGeometryID][ids[0x02]] };

        uint32_t j;

        if ( mms->rms->rfac[i].flags & RFACEHITATSTART ) {
            r3dmotionblur_interpolateTriangles ( rmb, srcTri, 
                                                      dstTri, 
                                                      midTri,
                                                      visibilityStart,
                                                      visibilityEnd );
        }
    }
}

/******************************************************************************/
void r3dmotionblur_drawInterpolatedFrame ( R3DMOTIONBLUR *rmb, 
                                           uint32_t fromGeometryID,
                                           uint32_t toGeometryID,
                                           float visibilityStart,
                                           float visibilityEnd ) {
    LIST *ltmpMotionMesh = rmb->lMotionMesh;

    while ( ltmpMotionMesh ) {
        R3DMOTIONMESH *mms = ( R3DMOTIONMESH * ) ltmpMotionMesh->data;

        /*memset ( rmb->div, 0x00, rmb->width * 
                                 rmb->height * sizeof ( uint32_t ) );*/

        r3dmotionblur_traceMesh ( rmb, mms, 
                                       fromGeometryID, 
                                       toGeometryID,
                                       visibilityStart,
                                       visibilityEnd );

        ltmpMotionMesh = ltmpMotionMesh->next;
    }
}
