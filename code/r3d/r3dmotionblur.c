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
    LIST *lfilters = /*list_copy ( rsce->lfilters )*/NULL;
    R3DFILTER *tofrm = r3dfilter_getByName ( rsce->rsg->input.lfilters, 
                                                 GOTOFRAMEFILTERNAME );
    float middleFrame = frameID - ( rmb->strength * 0.5f );
    float fromFrame = middleFrame - ( rmb->strength * 0.5f );
    /*** toFrame is normally equal to frameID ***/
    float   toFrame = middleFrame + ( rmb->strength * 0.5f );
    uint32_t nbSamples = rmb->nbSamples;
    /*** 1 step for start frame, 1 for middle frame, 1 for end frame ***/
    /*** and then, eventually 1 for each inter-frame (backward+forward) ***/
    uint32_t nbSteps = ( nbSamples * 0x02 );
    float deltaFrame = ( nbSteps ) ? ( toFrame - fromFrame ) / nbSteps : 0.0f;
    float currentFrame = fromFrame;
    R3DSCENE *middlersce;
    double *MVX, *PJX;
    uint32_t i;
    R3DFILTER *toWin =  r3dfilter_getByName ( rsce->rsg->input.lfilters, 
                                                 TOWINDOWFILTERNAME );

    list_insert ( &lfilters, toWin );

    /*** dont motion-blurize the first frame ***/
    if ( rsce->curframe == rsce->rsg->output.startframe ) return 0x00;

    /*** remove motion blur from the filter list ***/
    /*list_remove ( &lfilters, r3dfilter_getByName ( lfilters, fil->name  ) );*/

    /*** remove go_to_frame as well. We keep other filters, like the ones ***/
    /*** displaying the image to the window widget. ***/
    /*list_remove ( &lfilters, tofrm );*/

    /*** Because gotonextframe_draw will jump to frame + 1.0f, we have ***/
    /*** to substract 1.0f ***/
    if ( tofrm ) {
        tofrm->draw ( tofrm, NULL, middleFrame - 1.0f, NULL, 0x00, 0x00, 
                                                             0x00, 0x00 );
    }

    /*** Prepare the scene ***/
    middlersce = r3dscene_new ( rsce->rsg, 0x00 );

    /** create motion blur meshes from the meshes calculated in middle frame **/
    /** by the above call to r3dscene_new() **/
    r3dmotionblur_initMotionMeshes ( rmb, middlersce, nbSteps + 1 );

    /** clear arrays that are reused between frames ***/
    r3dmotionblur_clear ( rmb );

    /*** register this child renderscene in case we need to cancel it ***/
    r3dscene_addSubRender ( rsce, middlersce );

    /*** Render and free the middle frame ***/
    r3dscene_render ( middlersce );

    list_free ( &lfilters, NULL );

    /** retrieve the rendered image ***/
    rmb->curimg = middlersce->area.img;

    /*** unregister this child renderscene. No need to cancel it ***/
    /*** anymore, all threads are over after r3dscene_render().  ***/
    r3dscene_removeSubRender ( rsce, middlersce );

  /** I don't know why but the below call to tofrm->draw() hangs when ***/
  /*** the rendering is cancelled. It should not hang as we only request ***/
  /*** a go_to_frame action. No rendering involved. So, to by pass this, ***/
  /** I just skip the code below if the rendering is cancelled. Anyways, we ***/
  /** dont need those geometry data when the rendering is cancelled. ***/
  if ( rsce->cancelled == 0x00 ) {
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

            /*
             * Prepare the scene. We only need geometry coords
             * no texture or octrees
             */
            steprsce = r3dscene_new ( rsce->rsg, GEOMETRYONLY );

            /*** create another set of geometry data ***/
            r3dmotionblur_fillMotionMeshes ( rmb, steprsce, i );

            /*** Free the current frame ***/
            r3dobject_free  ( ( R3DOBJECT * ) steprsce );
        }

        currentFrame += deltaFrame;
    }

    for ( i = 0x00; i < nbSteps; i++ ) {
        int n = i + 0x01;

        r3dmotionblur_drawInterpolatedFrame ( rmb, i, n );
    }

    r3dmotionblur_blurify ( rmb, img );
  }

    /*** Free the current frame ***/
    r3dobject_free  ( ( R3DOBJECT * ) middlersce );

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
                                            float    strength,
                                            uint32_t nbSamples,
                                            float    subSamplingRate ) {
    R3DFILTER *fil;

    fil = r3dfilter_new ( FILTERBEFORE, VECTORMOTIONBLURFILTERNAME,
                                        filtervectormotionblur_draw,
                                        filtervectormotionblur_free,
                                        r3dmotionblur_new ( width, 
                                                            height,
                                                            strength,
                                                            nbSamples,
                                                            subSamplingRate ) );

    return fil;
}

/******************************************************************************/
R3DMOTIONBLUR *r3dmotionblur_new ( uint32_t width,
                                   uint32_t height,
                                   float    strength,
                                   uint32_t nbSamples,
                                   float    subSamplingRate ) {
    R3DMOTIONBLUR *rmb = calloc ( 0x01, sizeof ( R3DMOTIONBLUR ) );
    int i;

    if ( rmb == NULL ) {
        fprintf ( stderr, "r3dmotionblur_new: calloc failed\n");

        return NULL;
    }

    /* number of geometry samples. 1 is enough for translation. > 1 is */
    /* necessary for rotations */
    rmb->nbSamples       = ( nbSamples == 0x00 ) ? 0x01 : nbSamples;
    rmb->subSamplingRate = ( subSamplingRate > 1.0f ) ? 1.0f : subSamplingRate;

    rmb->hlines = calloc ( height, sizeof ( R3DMOTIONHLINE ) );

    if ( rmb->hlines == NULL ) {
        fprintf ( stderr, "r3dmotionblur_new: hlines calloc failed\n");

        free ( rmb );

        return NULL;
    }

    /*** Store vectors Z position. We don't need to store x and y because ***/
    /*** they are given by the screen coordinates. ***/
    rmb->zBuffer = calloc ( width * height, sizeof ( double ) );

    if ( rmb->zBuffer == NULL ) {
        fprintf ( stderr, "r3dmotionblur_new: z calloc failed\n");

        free ( rmb->hlines );
        free ( rmb );

        return NULL;
    }

    /*** receives the final image. Used to be needed because we worked with ***/
    /*** uint32_t but now we work with char and we dont compute any average ***/
    /*** value, so we should be able to directly usr the img buffer.        ***/
    rmb->blur = ( uint32_t * ) calloc ( width * height, 0x03 * sizeof( uint32_t ) );

    if ( rmb->blur == NULL ) {
        fprintf ( stderr, "r3dmotionblur_new: blur calloc failed\n");

        /*free ( rmb->zBuffer );*/
        free ( rmb->hlines );
        free ( rmb );

        return NULL;
    }

    rmb->div = calloc ( width * height, sizeof ( uint32_t ) );

    if ( rmb->div == NULL ) {
        fprintf ( stderr, "r3dmotionblur_new: div calloc failed\n");

        free ( rmb->blur );
        /*free ( rmb->zBuffer );*/
        free ( rmb->hlines );
        free ( rmb );

        return NULL;
    }

    rmb->faceBuffer = calloc ( width * height, sizeof ( R3DMOTIONFACE * ) );

    if ( rmb->faceBuffer == NULL ) {
        fprintf ( stderr, "r3dmotionblur_new: faceBuffer calloc failed\n");

        free ( rmb->div );
        free ( rmb->blur );
        /*free ( rmb->zBuffer );*/
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
void r3dmotionblur_fillHLineBuffer ( R3DMOTIONBLUR   *rmb,
                                     R3DMOTIONPOINT  *srcPoint,
                                     float            srcRatio[0x03],
                                     R3DMOTIONPOINT  *dstPoint,
                                     float            dstRatio[0x03] ) {
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
    float ratio0 = srcRatio[0],
          ratio1 = srcRatio[1],
          ratio2 = srcRatio[2];
    float dratio0 = dstRatio[0] - srcRatio[0], pratio0 = ( dratio0 / (dd+1) ),
          dratio1 = dstRatio[1] - srcRatio[1], pratio1 = ( dratio1 / (dd+1) ),
          dratio2 = dstRatio[2] - srcRatio[2], pratio2 = ( dratio2 / (dd+1) );
    int32_t prevy = 0x00, prevx = 0x00;

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
uint32_t r3dmotionblur_interpolateHLine ( R3DMOTIONBLUR  *rmb,
                                          int32_t         y,
                                          R3DMOTIONFACE  *mfac,
                                          R3DMOTIONPOINT *midTri[0x03] ) {
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
    uint32_t drawn = 0x00;
    uint32_t R, G, B;
    int i;

    for ( i = 0x00; i < ddx; i++ ) {
        uint32_t offset = ((y)*rmb->width)+x1;

        if ( x1 >= 0 && x1 < rmb->width ) {
            /** We filter the Z axis but we want a face to overwrite itself, */
            /* hence the epsilon value (otherwise there is some Z fighting */
            /* with itself. */
            /*if ( z <= ( rmb->zBuffer[offset] + 0.0001f ) ) {*/
                /*** Query original pixel position ***/
                int32_t midTriX = ( ( float ) midTri[0x00]->x * ratio0 ) +
                                  ( ( float ) midTri[0x01]->x * ratio1 ) +
                                  ( ( float ) midTri[0x02]->x * ratio2 );
                int32_t midTriY = ( ( float ) midTri[0x00]->y * ratio0 ) +
                                  ( ( float ) midTri[0x01]->y * ratio1 ) +
                                  ( ( float ) midTri[0x02]->y * ratio2 );

                if ( ( midTriX >= 0 ) && ( midTriX < rmb->width  ) &&
                     ( midTriY >= 0 ) && ( midTriY < rmb->height ) ) {
                    uint32_t midTriOffset = midTriY * rmb->width + midTriX;

                    R = ( uint32_t ) rmb->curimg [midTriOffset][0x00];
                    G = ( uint32_t ) rmb->curimg [midTriOffset][0x01];
                    B = ( uint32_t ) rmb->curimg [midTriOffset][0x02];

                    drawn = 0x01;

                    rmb->faceBuffer[offset] = mfac;

                    rmb->div[offset]++;

                    rmb->blur[offset][0x00] += R;
                    rmb->blur[offset][0x01] += G;
                    rmb->blur[offset][0x02] += B;
                }

                rmb->zBuffer[offset] = z;
            /*}*/
        }

        x1 += px;

        z      += pz;
        ratio0 += pratio0;
        ratio1 += pratio1;
        ratio2 += pratio2;
    }

    return drawn;
}

/******************************************************************************/
void r3dmotionblur_clearZBuffer ( R3DMOTIONBLUR *rmb ) {
    uint32_t i;

    for ( i = 0x00; i < rmb->width * rmb->height; i++ ) {
        /*** Set vectors Z-value to the maximum Z depth, in order to ***/
        /*** be able to sort them later. ***/
        rmb->zBuffer[i] = 1.0f;
    }
}

/******************************************************************************/
void r3dmotionblur_blurify ( R3DMOTIONBLUR *rmb, unsigned char (*img)[0x03] ) {
    uint32_t y, x;

    for ( y = 0x00; y < rmb->height; y++ ) {
        for ( x = 0x00; x < rmb->width; x++ ) {
            uint32_t offset = ((y)*rmb->width)+x;

            R3DMOTIONFACE *mfac = rmb->faceBuffer[offset];
            uint32_t R = 0, G = 0, B = 0;

            if ( rmb->div[offset] && mfac->nbDraw ) {
                float incFactor = ( float ) rmb->div[offset] / mfac->nbDraw;
                float decFactor;

                /* it might happen that some pixel are drawn more than */
                /* the number of face drawing, because of face overlapping */
                /* so we need to hack this. */
                if ( incFactor > 1.0f ) incFactor = 1.0f;

                decFactor = 1.0f - incFactor;

                R = ( rmb->blur[offset][0] / ( float ) rmb->div[offset] * incFactor ) + ( decFactor * rmb->curimg[offset][0] );
                G = ( rmb->blur[offset][1] / ( float ) rmb->div[offset] * incFactor ) + ( decFactor * rmb->curimg[offset][1] );
                B = ( rmb->blur[offset][2] / ( float ) rmb->div[offset] * incFactor ) + ( decFactor * rmb->curimg[offset][2] ); 

            } else {
                R = ( rmb->curimg[offset][0] );
                G = ( rmb->curimg[offset][1] );
                B = ( rmb->curimg[offset][2] );
            }

            /* if any overflow, use the original image */
            if ( R > 0xFF ) R = rmb->curimg[offset][0];
            if ( G > 0xFF ) G = rmb->curimg[offset][1];
            if ( B > 0xFF ) B = rmb->curimg[offset][2];

            img[offset][0] = R;
            img[offset][1] = G;
            img[offset][2] = B;
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

    free ( mms->mfac );
    free ( mms->geometry );

    free ( mms );
}

/******************************************************************************/
void r3dmotionblur_free ( R3DMOTIONBLUR *rmb ) {
    if ( rmb->zBuffer       ) free ( rmb->zBuffer       );
    if ( rmb->div           ) free ( rmb->div           );
    if ( rmb->hlines        ) free ( rmb->hlines        );
    if ( rmb->blur          ) free ( rmb->blur          );
    if ( rmb->faceBuffer    ) free ( rmb->faceBuffer    );

    list_free ( &rmb->lMotionMesh, r3dmotionmesh_free );

    free ( rmb );
}

/******************************************************************************/
void r3dmotionblur_clear ( R3DMOTIONBLUR *rmb ) {
    LIST *ltmpmms = rmb->lMotionMesh;

    memset ( rmb->div       , 0x00, rmb->width * 
                                    rmb->height * sizeof ( uint32_t ) );

    memset ( rmb->blur      , 0x00, rmb->width * 
                                    rmb->height * sizeof ( uint32_t ) * 0x03 );

    memset ( rmb->faceBuffer, 0x00, rmb->width *
                                    rmb->height * sizeof ( R3DMOTIONFACE * ) );

    r3dmotionblur_clearZBuffer ( rmb );

    while ( ltmpmms ) {
        R3DMOTIONMESH *mms = ( R3DMOTIONMESH * ) ltmpmms->data;

        memset ( mms->mfac, 0x00, mms->rms->nbrfac * sizeof ( R3DMOTIONFACE ) );

        ltmpmms = ltmpmms->next;
    }

}

/******************************************************************************/
void r3dmotionmesh_init ( R3DMOTIONMESH *mms, R3DMESH *rms, 
                                              uint32_t nbGeometry ) {
    uint32_t ptrSize = sizeof ( R3DMOTIONPOINT * );
    uint32_t i;

    mms->nbGeometry = nbGeometry;
    mms->rms        = rms;
    mms->id         = ((R3DOBJECT*)rms)->id;

    mms->geometry = ( R3DMOTIONPOINT ** ) calloc ( nbGeometry, ptrSize );

    if ( rms->nbrver ) { /* beware the empty meshes */
        for ( i = 0x00; i < nbGeometry; i++ ) {
            uint32_t structSize =  sizeof ( R3DMOTIONPOINT );
            uint32_t nbrver = rms->nbrver;

            mms->geometry[i] = ( R3DMOTIONPOINT * ) calloc ( nbrver, 
                                                             structSize );
        }
    }

    if ( rms->nbrfac ) {
        uint32_t structSize =  sizeof ( R3DMOTIONFACE );

        mms->mfac = ( R3DMOTIONFACE * ) calloc ( rms->nbrfac, structSize );
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
R3DMOTIONMESH *r3dmotionblur_getMotionMeshById ( R3DMOTIONBLUR *rmb, 
                                                 uint32_t       id ) {
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
void r3dmotionblur_fillMotionMeshes ( R3DMOTIONBLUR *rmb, R3DSCENE *rsce,
                                                          uint32_t  stepID ) {
    LIST *ltmprob = rsce->lrob;

    while ( ltmprob ) {
        R3DOBJECT *rob = ( R3DOBJECT * ) ltmprob->data;

        if ( rob->obj->type & MESH ) {
            R3DMESH       *rms = ( R3DMESH * ) rob;
            uint32_t       qId = ((R3DOBJECT*)rms)->id;
            R3DMOTIONMESH *mms = r3dmotionblur_getMotionMeshById ( rmb, qId );

            if ( mms ) {
                r3dmotionmesh_fillGeometry ( mms, rms, stepID );
            }
        }

        ltmprob = ltmprob->next;
    }
}

/******************************************************************************/
void r3dmotionblur_initMotionMeshes ( R3DMOTIONBLUR *rmb, R3DSCENE *rsce,
                                                          uint32_t  nbSteps ) {
    LIST *ltmprob = rsce->lrob;

    while ( ltmprob ) {
        R3DOBJECT *rob = ( R3DOBJECT * ) ltmprob->data;
        if ( rob->obj->type == G3DMESHTYPE ) {
            R3DMESH *rms = ( R3DMESH * ) rob;
            uint32_t       qId = ((R3DOBJECT*)rms)->id;
            R3DMOTIONMESH *mms = r3dmotionblur_getMotionMeshById ( rmb, qId );

            /* For sequence rendering, mesh are created by the first blurred */
            /* frame and then reused for other frames */
            if ( mms == NULL ) {
                mms = r3dmotionmesh_new ( rms, nbSteps );

                list_insert ( &rmb->lMotionMesh, mms );
            } else {
                /* update with valid pointer or else it contains the one */
                /* from the previous frame */
                mms->rms = rms; 
            }
        }

        ltmprob = ltmprob->next;
    }
}

/******************************************************************************/
#define FULLRATIO 0.9f
#define VOIDRATIO ((1.0f-FULLRATIO)/2.0f)
void r3dmotionblur_drawInterpolatedTriangle ( R3DMOTIONBLUR  *rmb,
                                              R3DMOTIONFACE  *mfac,
                                              R3DMOTIONPOINT *itrTri[0x03],
                                              R3DMOTIONPOINT *midTri[0x03] ) {
    static float ratio[0x03][0x03] = { { 1.0f, 0.0f, 0.0f },
                                       { 0.0f, 1.0f, 0.0f },
                                       { 0.0f, 0.0f, 1.0f } };
    uint32_t drawn = 0x00;
    uint32_t i;

    memset ( rmb->hlines, 0x00, sizeof ( R3DMOTIONHLINE ) * rmb->height );

    for ( i = 0x00; i < 0x03; i++ ) {
        int n = ( i + 0x01 ) % 0x03;

        /* draw lines in the same X direction */
        if ( itrTri[i]->x < itrTri[n]->x ) {
            r3dmotionblur_fillHLineBuffer ( rmb, itrTri[i], ratio[i],
                                                 itrTri[n], ratio[n] );
        } else {
            r3dmotionblur_fillHLineBuffer ( rmb, itrTri[n], ratio[n],
                                                 itrTri[i], ratio[i] );
        }
    }

    /*** Then interplate the vectors along the horizontal lines ***/
    for ( i = 0; i < rmb->height; i++ ) {
        if ( rmb->hlines[i].inited == 0x02 ){
            if ( ( ( rmb->hlines[i].x1 < rmb->width ) && 
                   ( rmb->hlines[i].x2 > 0          ) ) ||
                 ( ( rmb->hlines[i].x2 < rmb->width ) && 
                   ( rmb->hlines[i].x1 > 0          ) ) ) {
                drawn += r3dmotionblur_interpolateHLine ( rmb, 
                                                          i,
                                                          mfac,
                                                          midTri );
            }
        }
    }

    if ( drawn ) mfac->nbDraw++;
}

/******************************************************************************/
void r3dmotionblur_interpolateTriangles ( R3DMOTIONBLUR  *rmb, 
                                          R3DMOTIONFACE  *mfac,
                                          R3DMOTIONPOINT *srcTri[0x03], 
                                          R3DMOTIONPOINT *dstTri[0x03],
                                          R3DMOTIONPOINT *midTri[0x03] ) {
    R3DMOTIONVECTOR mvec[0x03] = { { .x = dstTri[0x00]->x - srcTri[0x00]->x,
                                     .y = dstTri[0x00]->y - srcTri[0x00]->y,
                                     .z = dstTri[0x00]->z - srcTri[0x00]->z },
                                   { .x = dstTri[0x01]->x - srcTri[0x01]->x,
                                     .y = dstTri[0x01]->y - srcTri[0x01]->y,
                                     .z = dstTri[0x01]->z - srcTri[0x01]->z },
                                   { .x = dstTri[0x02]->x - srcTri[0x02]->x,
                                     .y = dstTri[0x02]->y - srcTri[0x02]->y,
                                     .z = dstTri[0x02]->z - srcTri[0x02]->z } };
    G2DVECTOR vlen[0x03] = { { .x = mvec[0x00].x,
                               .y = mvec[0x00].y },
                             { .x = mvec[0x01].x,
                               .y = mvec[0x01].y },
                             { .x = mvec[0x02].x,
                               .y = mvec[0x02].y } };
    uint32_t len[0x03] = { g2dvector_length ( &vlen[0x00] ), 
                           g2dvector_length ( &vlen[0x01] ),
                           g2dvector_length ( &vlen[0x02] ) };
    /*** We draw as many triangles as the maximum length between source ***/
    /*** and destination triangles (if subSamplingRate is 100%). So we ***/
    /*** first have to get the maximum length and then we compute the ***/
    /*** number of iterations from this length and subSamplingRate. ***/
    uint32_t maxLen = ( len[0] > len[1] ) ? 
                    ( ( len[0] > len[2] ) ? len[0] : len[2] ) :
                    ( ( len[1] > len[2] ) ? len[1] : len[2] );
    uint32_t iter = maxLen * rmb->subSamplingRate;

    if ( maxLen ) {
        uint32_t i;

        for ( i = 0x00; i < iter; i++ ) {
            float pos = ( float ) i / iter;
            R3DMOTIONPOINT p[3] = { { .x = srcTri[0]->x + (mvec[0].x * pos),
                                      .y = srcTri[0]->y + (mvec[0].y * pos),
                                      .z = srcTri[0]->z + (mvec[0].z * pos) },
                                    { .x = srcTri[1]->x + (mvec[1].x * pos),
                                      .y = srcTri[1]->y + (mvec[1].y * pos),
                                      .z = srcTri[1]->z + (mvec[1].z * pos) },
                                    { .x = srcTri[2]->x + (mvec[2].x * pos),
                                      .y = srcTri[2]->y + (mvec[2].y * pos),
                                      .z = srcTri[2]->z + (mvec[2].z * pos) } };
            R3DMOTIONPOINT *itrTri[0x03] = { &p[0x00],
                                             &p[0x01], 
                                             &p[0x02] };

            r3dmotionblur_drawInterpolatedTriangle ( rmb, mfac,
                                                          itrTri,
                                                          midTri );
        }
    }
}

/******************************************************************************/
void r3dmotionblur_traceMesh ( R3DMOTIONBLUR *rmb, R3DMOTIONMESH *mms,
                                                   uint32_t fromGeometryID,
                                                   uint32_t toGeometryID ) {
    uint32_t i;

    /*** For each triangle, fill the vector buffer ***/
    for ( i = 0x00; i < mms->rms->nbrfac; i++ ) {
        uint32_t ids[3] = { mms->rms->rfac[i].rverID[0x00],
                            mms->rms->rfac[i].rverID[0x01],
                            mms->rms->rfac[i].rverID[0x02] };
        uint32_t facID = i;

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
            r3dmotionblur_interpolateTriangles ( rmb, &mms->mfac[i],
                                                       srcTri, 
                                                       dstTri, 
                                                       midTri );
        }
    }
}

/******************************************************************************/
void r3dmotionblur_drawInterpolatedFrame ( R3DMOTIONBLUR *rmb, 
                                           uint32_t fromGeometryID,
                                           uint32_t toGeometryID ) {
    LIST *ltmpMotionMesh = rmb->lMotionMesh;

    while ( ltmpMotionMesh ) {
        R3DMOTIONMESH *mms = ( R3DMOTIONMESH * ) ltmpMotionMesh->data;

        r3dmotionblur_traceMesh ( rmb, mms, 
                                       fromGeometryID, 
                                       toGeometryID );

        ltmpMotionMesh = ltmpMotionMesh->next;
    }
}
