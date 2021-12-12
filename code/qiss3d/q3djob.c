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
void q3djob_goToFrame ( Q3DJOB *qjob, float frame ) {
    qjob->curframe = frame;

    qjob->filters.toframe->draw ( qjob->filters.toframe, 
                                  NULL,
                                  0x00,
                                  frame,
                                  NULL, 
                                  0x00, 
                                  0x00, 
                                  0x00, 
                                  0x00 );
}

/******************************************************************************/
void q3djob_end ( Q3DJOB *qjob ) {
    qjob->running   = 0x00;
    qjob->cancelled = 0x01;
}

/******************************************************************************/
void q3djob_freeFilters ( Q3DJOB *qjob ) {
    uint32_t nbfil = sizeof ( Q3DFILTERSET ) / sizeof ( Q3DFILTER * );
    Q3DFILTER **qfil = ( Q3DFILTER * ) &qjob->filters;
    uint32_t i;

    for ( i = 0x00; i < nbfil; i++ ) {
        if ( qfil[i] == NULL                   ) continue;
        if ( qfil[i] == qjob->filters.tostatus ) continue; /*  freed by the caller */
        /*if ( qfil[i] == qjob->filters.towindow ) continue;*/
        if ( qfil[i] == qjob->filters.toframe  ) continue; /*  freed by the caller */

        q3dfilter_free ( qfil[i] );
    }
}

/******************************************************************************/
void q3djob_free ( Q3DJOB *qjob ) {
    q3dobject_free_r ( ( Q3DOBJECT * ) qjob->qsce );

    if ( qjob->img ) free ( qjob->img );

    q3djob_freeFilters ( qjob );

    q3darea_reset ( &qjob->qarea );

    q3dobject_free   ( ( Q3DOBJECT * ) qjob->qcam );

    printf ("Q3DJOB Freed\n" );
}

/******************************************************************************/
static uint32_t q3djob_getNextLine ( Q3DJOB *qjob, Q3DSEGMENT *qseg ) {
    Q3DAREA *qarea = &qjob->qarea;
    uint32_t scanline;

    pthread_mutex_lock ( &qarea->lock );

    scanline = qarea->scanline++;

    /*** prepare the next call ***/
    q3dsegment_interpolate ( &qarea->qseg[0x00], &qarea->vpol, 1.0f );

    memcpy ( qseg, &qarea->qseg[0x00], sizeof ( Q3DSEGMENT ) );

    pthread_mutex_unlock ( &qarea->lock  );

    return scanline;
}

/******************************************************************************/
void q3djob_filterline ( Q3DJOB  *qjob, 
                         uint32_t cpuID,
                         uint32_t from, 
                         uint32_t to,
                         uint32_t depth, 
                         uint32_t width ) {
    #define FILTERCOUNT 0x02
    Q3DFILTER *fil[FILTERCOUNT] = { qjob->filters.edgeaa,
                                    qjob->filters.towindow };
    char *img = qjob->img;
    uint32_t i;

    if ( qjob->running == 0x00 ) return;

    for ( i = 0x00; i < FILTERCOUNT; i++ ) {
        if ( fil[i] ) {
            if ( fil[i]->flags & ENABLEFILTER ) {
                if ( fil[i]->draw ( fil[i], 
                                   qjob, 
                                   cpuID,
                                   qjob->curframe, 
                                   img, 
                                   from, 
                                   to, 
                                   depth, 
                                   width ) ) {

                    /*** stop processing filters if 1 is returned ***/
                    return;
                }
            }
        }
    }
    #undef FILTERCOUNT
}

/******************************************************************************/
static void q3djob_filterimage ( Q3DJOB  *qjob,
                                 uint32_t from, 
                                 uint32_t to,
                                 uint32_t depth, 
                                 uint32_t width ) {
    #define FILTERCOUNT 0x06
    Q3DFILTER *fil[FILTERCOUNT] = { qjob->filters.softshadows,
                                    qjob->filters.simpleAA,
                                    qjob->filters.towindow,
                                    qjob->filters.tosequence,
                                    qjob->filters.toimage,
                                    qjob->filters.tostatus };
    char *img = qjob->img;
    uint32_t i;

    for ( i = 0x00; i < FILTERCOUNT; i++ ) {
        if ( qjob->running && fil[i] ) {
            if ( fil[i]->flags & ENABLEFILTER ) {
                if ( fil[i]->draw ( fil[i], 
                                    qjob,
                                    0x00,
                                    qjob->curframe, 
                                    img, 
                                    from, 
                                    to, 
                                    depth, 
                                    width ) ) {

                    /*** stop processing filters if 1 is returned ***/
                    return;
                }
            }
        }
    }
    #undef FILTERCOUNT
}

/******************************************************************************/
static uint32_t q3djob_filterbefore ( Q3DJOB  *qjob, 
                                      uint32_t from, 
                                      uint32_t to,
                                      uint32_t depth, 
                                      uint32_t width ) {
    #define FILTERCOUNT 0x03
    Q3DFILTER *fil[FILTERCOUNT] = { qjob->filters.tostatus,
                                    qjob->filters.makepreview,
                                    qjob->filters.motionblur };
    char *img = qjob->img;
    uint32_t i;

    for ( i = 0x00; i < FILTERCOUNT; i++ ) {
        if ( qjob->running && fil[i] ) {
            if ( fil[i]->flags & ENABLEFILTER ) {
                uint32_t ret = fil[i]->draw ( fil[i], 
                                              qjob, 
                                              0x00,
                                              qjob->curframe, 
                                              img, 
                                              from, 
                                              to, 
                                              depth, 
                                              width );
                if ( ret ) {
                    return ret;
                }
            }
        }
    }
    #undef FILTERCOUNT

    return 0x00;
}

/******************************************************************************/
void *q3djob_raytrace_t ( void *ptr ) {
    Q3DRAYTRACETHREAD *rtt = ( Q3DRAYTRACETHREAD * ) ptr;
    Q3DJOB *qjob = rtt->qjob;
    Q3DAREA *qarea = &qjob->qarea;
    Q3DSEGMENT qseg;
    unsigned char *img = qjob->img;
    uint32_t scanline;
    uint32_t steps  = ( qarea->x2 - qarea->x1 );
    uint32_t width  = ( qarea->x2 - qarea->x1 ) + 0x01;
    uint32_t height = ( qarea->y2 - qarea->y1 ) + 0x01;
    uint32_t bytesperline = ( width * 0x03 );
    uint32_t outlineFlag = ( qjob->qrsg->flags & RENDERWIREFRAME ) ? RAYQUERYOUTLINE : 0x00;
    int i;

    qjob->qarea.scanline = 0x00;

    /*** Compute the interpolation factors for rays ***/
    q3darea_viewport ( qarea, qjob->qcam );

    /*** return immediately when canceled ***/
    /*pthread_setcanceltype ( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );*/

    while ( ( ( scanline = q3djob_getNextLine ( qjob, 
                                               &qseg ) ) <= qarea->y2 ) && qjob->running ) {
        unsigned char *imgptr = &img[(scanline*bytesperline)];

        if ( qjob->filters.edgeaa ) {
            q3dfilter_edgeaa_initScanline ( qjob->filters.edgeaa,
                                            rtt->cpuID,
                                           &qseg,
                                            steps );
        }

        for ( i = qarea->x1; ( i <= qarea->x2 ) && qjob->running; i++ ) {
            uint32_t color;
            Q3DVECTOR3F intersectionPointToSource;
            Q3DRAY qray;
            float viewingDistance;
            uint32_t offset = ( scanline * qarea->width ) + i;

            /*** Ray building ***/
            /*** First, reset the ray ***/
            memset ( &qray, 0x00, sizeof ( Q3DRAY ) );

            qray.x = i;
            qray.y = scanline;

            /*** then set its source position ***/
            /*memcpy ( &ray.ori, &pone.src, sizeof ( R3DVECTOR ) );*/

            qray.src.x = qseg.src.x;
            qray.src.y = qseg.src.y;
            qray.src.z = qseg.src.z;

            /*** and its destination vector ***/
            qray.dir.x = ( qseg.dst.x - qseg.src.x );
            qray.dir.y = ( qseg.dst.y - qseg.src.y );
            qray.dir.z = ( qseg.dst.z - qseg.src.z );

            /*** This value will be compared to the depth value of the hit  ***/
            /*** point. This allows us to pick the closest hit to the eye.  ***/
            qray.distance = INFINITY;
            qray.flags    = Q3DRAY_PRIMARY_BIT;

            /*** but don't forget to normalize the latter **/
            q3dvector3f_normalize ( &qray.dir, &viewingDistance );

            /*** shoot the ray ***/
            if ( qjob->flags & JOBRENDERSOFTSHADOWS ) {
                color = q3dray_shoot_r ( &qray,
                                          qjob,
                                          NULL,
                                          NULL,
                                          NULL,
                                          qjob->curframe,
                                          0x04,
                                          RAYQUERYHIT |
                                          RAYQUERYSOFTSHADOWSPASS1 |
                                          RAYQUERYLIGHTING       |
                                          RAYQUERYREFLECTION     |
                                          RAYQUERYREFRACTION     |
                                          RAYQUERYIGNOREBACKFACE/* |
                                          outlineFlag*/ );
            } else {
                color = q3dray_shoot_r ( &qray,
                                          qjob,
                                          NULL,
                                          NULL,
                                          NULL,
                                          qjob->curframe,
                                          0x04,
                                          RAYQUERYHIT            |
                                          RAYQUERYSOFTSHADOWSPASS2 |
                                          RAYQUERYHARDSHADOWS    |
                                          RAYQUERYSHADING        |
                                          RAYQUERYSURFACECOLOR   | 
                                          RAYQUERYLIGHTING       |
                                          RAYQUERYREFLECTION     |
                                          RAYQUERYREFRACTION     |
                                          RAYQUERYIGNOREBACKFACE/* |
                                          outlineFlag*/ );

                imgptr[0x00] = ( color & 0x00FF0000 ) >> 0x10;
                imgptr[0x01] = ( color & 0x0000FF00 ) >> 0x08;
                imgptr[0x02] = ( color & 0x000000FF );

                if ( qjob->qrsg->flags & RENDERFOG ) {
                    uint32_t fogR = ( ( qjob->qrsg->fog.color & 0x00FF0000 ) >> 0x10 ),
                             fogG = ( ( qjob->qrsg->fog.color & 0x0000FF00 ) >> 0x08 ),
                             fogB =   ( qjob->qrsg->fog.color & 0x000000FF );

                    if ( ( ( qray.distance == INFINITY ) && 
                           ( qjob->qrsg->fog.flags & FOGAFFECTSBACKGROUND ) ) ||
                           ( qray.distance != INFINITY ) ) {
                        float fogNear = qjob->qrsg->fog.fnear;
                        float fogFar = qjob->qrsg->fog.ffar;
                        float deltaFog = ( fogFar - fogNear );
                        float incFogRatio = ( deltaFog ) ? ( qray.distance - 
                                                             fogNear ) / deltaFog : 0.0f;
                        /*** note: fog strength is between 0.0f and 1.0f ***/
                        float decFogRatio = qjob->qrsg->fog.strength - incFogRatio;

                        if ( qray.distance < fogNear ) {
                            incFogRatio = 0.0f;
                            decFogRatio = 1.0f;
                        }

                        if ( qray.distance > fogNear ) {
                            if ( incFogRatio > 1.0f ) incFogRatio = 1.0f;

                            incFogRatio = incFogRatio * qjob->qrsg->fog.strength;
                            decFogRatio = 1.0f - incFogRatio;
                        }

                        uint32_t R = ( imgptr[0x00] * decFogRatio ) + ( fogR * incFogRatio );
                        uint32_t G = ( imgptr[0x01] * decFogRatio ) + ( fogG * incFogRatio );
                        uint32_t B = ( imgptr[0x02] * decFogRatio ) + ( fogB * incFogRatio );

                        if ( R > 0xFF ) R = 0xFF;
                        if ( G > 0xFF ) G = 0xFF;
                        if ( B > 0xFF ) B = 0xFF;

                        imgptr[0x00] = R;
                        imgptr[0x01] = G;
                        imgptr[0x02] = B;
                    }
                }

                imgptr += 0x03;
            }

            /*** be ready for the next ray ***/
            q3dsegment_interpolate ( &qseg, &qjob->qarea.hpol, 1.0f );
        }

        q3djob_filterline ( qjob, 
                            rtt->cpuID, 
                            scanline, 
                            scanline + 0x01, 
                            0x18, 
                            width );
    }

    /*** this is needed for memory release ***/
    pthread_exit ( NULL );


    return NULL;
}

/******************************************************************************/
static void q3djob_createRenderThread ( Q3DRAYTRACETHREAD *rtt ) {
    pthread_attr_t attr;

    pthread_attr_init ( &attr );

    /*** start thread son all CPUs ***/
    pthread_attr_setscope ( &attr, PTHREAD_SCOPE_SYSTEM );

    /*** launch rays ***/
    pthread_create ( &rtt->tid, &attr, q3djob_raytrace_t, rtt );
}

/******************************************************************************/
static void q3djob_initFilters ( Q3DJOB    *qjob,
                                 Q3DFILTER *towindow,
                                 Q3DFILTER *toframe,
                                 Q3DFILTER *tostatus,
                                 Q3DFILTER *makepreview ) {
    Q3DSETTINGS *qrsg   = qjob->qrsg;
   /* Q3DFILTER *simpleAA = q3dfilter_simpleaa_new ( );*/
    Q3DFILTER *softshadows = q3dfilter_softshadows_new ( );

    qjob->filters.towindow    = towindow;
    qjob->filters.toframe     = toframe;
    qjob->filters.tostatus    = tostatus;
    qjob->filters.makepreview = makepreview;

    if ( qrsg->flags & ENABLEMOTIONBLUR ) {
        if ( qrsg->flags & VECTORMOTIONBLUR ) {
            Q3DFILTER *motionblur = q3dfilter_vmb_new ( qrsg->output.width,
                                                        qrsg->output.height,
                                                        qrsg->motionBlur.strength,
                                                        qrsg->motionBlur.vMotionBlurSamples,
                                                        qrsg->motionBlur.vMotionBlurSubSamplingRate );

            qjob->filters.motionblur = motionblur;
        }

        if ( qrsg->flags & SCENEMOTIONBLUR ) {
            Q3DFILTER *motionblur = q3dfilter_smb_new ( qrsg->output.width, 
                                                        qrsg->output.height,
                                                        qrsg->motionBlur.strength,
                                                        qrsg->motionBlur.iterations );

            qjob->filters.motionblur = motionblur;
        }
    }

    if ( qrsg->flags & ENABLEAA ) {
        qjob->filters.edgeaa = q3dfilter_edgeaa_new ( qrsg->aa.nbsamples );
    }

    if ( qrsg->flags & RENDERSAVE ) {
        if ( qrsg->output.format == RENDERTOVIDEO ) {
            G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
            char buf[0x1000];

            snprintf ( buf, 0x1000, "%s.avi", qrsg->output.outfile );

            qjob->filters.tosequence = q3dfilter_toFfmpeg_new ( 0x00, 
                                                                qrsg->output.width,
                                                                qrsg->output.height,
                                                                qrsg->output.depth,
                                                                qrsg->output.fps,
                                                                qrsg->output.endframe - 
                                                                qrsg->output.startframe,
                    #ifdef __MINGW32__
                                                               &qrsg->cvars,
                    #endif
                                                                buf,
                                                                sysinfo->ffmpegPath,
                                                                sysinfo->ffplayPath );

            if ( qjob->filters.tosequence == NULL ) {
                fprintf ( stderr, "FFMpeg not found!\n");
            }
        }

        if ( qrsg->output.format == RENDERTOIMAGE ) {
            static char buf[0x1000];

            snprintf ( buf, 0x1000, "%s.jpg", qrsg->output.outfile );

            if ( qrsg->output.startframe != qrsg->output.endframe ) {
                qjob->filters.toimage = q3dfilter_writeImage_new ( buf, 0x01 );
            } else {
                qjob->filters.toimage = q3dfilter_writeImage_new ( buf, 0x00 );
            }
        }
    }
}

/******************************************************************************/
void q3djob_clear ( Q3DJOB *qjob ) {
    if ( qjob->qsce ) q3dobject_free_r ( qjob->qsce );
    if ( qjob->qcam ) q3dobject_free   ( qjob->qcam );

    qjob->qsce = NULL;
    qjob->qcam = NULL;
}

/******************************************************************************/
void q3djob_prepare ( Q3DJOB      *qjob,
                      G3DSCENE    *sce,
                      G3DCAMERA   *cam ) {

    qjob->qsce = q3dscene_import ( sce, qjob->curframe, 0x00 );

    qjob->qcam = q3dcamera_new ( cam, 0x00, 0x00,
                                      qjob->qrsg->output.width, 
                                      qjob->qrsg->output.height );

    q3darea_init ( &qjob->qarea,
                    qjob->qsce,
                    qjob->qcam,
                    qjob->qrsg->output.x1,
                    qjob->qrsg->output.y1,
                    qjob->qrsg->output.x2,
                    qjob->qrsg->output.y2,
                    qjob->qrsg->output.width,
                    qjob->qrsg->output.height,
                    0x18,
                    qjob->curframe );
}

/******************************************************************************/
Q3DJOB *q3djob_new ( Q3DSETTINGS *qrsg, 
                     G3DSCENE    *sce,
                     G3DCAMERA   *cam,
                     Q3DFILTER   *towindow,
                     Q3DFILTER   *toframe,
                     Q3DFILTER   *tostatus,
                     Q3DFILTER   *makepreview,
                     uint64_t     flags ) {
    uint32_t structsize = sizeof ( Q3DJOB );
    uint32_t bytesperline = ( qrsg->output.width * 0x03 );
    Q3DJOB *qjob;
    int i;

    if ( ( qjob = ( Q3DJOB * ) calloc ( 0x01, structsize ) ) == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    /*** This 24bpp buffer will receive the raytraced pixel values ***/
    if ( ( qjob->img = calloc ( qrsg->output.height, bytesperline ) ) == NULL ) {
        fprintf ( stderr, "%s: image memory allocation failed\n", __func__ );

        free ( qjob );

        return NULL;
    }

    qjob->qrsg  = qrsg;
    qjob->nbcpu = g3dcore_getNumberOfCPUs ( );

    q3djob_initFilters ( qjob,
                         towindow,
                         toframe,
                         tostatus,
                         makepreview );




    return qjob;
}

/******************************************************************************/
void q3djob_render_sequence ( Q3DJOB *qjob ) {
    G3DSCENE *sce  = qjob->qrsg->input.sce;
    G3DCAMERA *cam = qjob->qrsg->input.cam;
    int32_t startframe = qjob->qrsg->output.startframe,
            endframe = qjob->qrsg->output.endframe;
    int32_t i, j;

    qjob->threaded = 0x01;
    qjob->running  = 0x01;

    q3djob_goToFrame ( qjob, qjob->qrsg->output.startframe );

    q3djob_prepare ( qjob, sce, cam );

    /*** Render the first frame ***/
    q3djob_render ( qjob );

    if ( qjob->filters.toframe ) {
        for ( i = startframe + 1; i <= endframe; i++ ) {
            if ( qjob->running ) {
                q3djob_goToFrame ( qjob, i );

                q3djob_clear   ( qjob );
                q3djob_prepare ( qjob, sce, cam );

                /*** Render the current frame ***/
                q3djob_render ( qjob );
            }
        }
    }

    qjob->running  = 0x00;
    qjob->threaded = 0x00;

    q3djob_free ( qjob );
}

/******************************************************************************/
void q3djob_render_frame ( Q3DJOB *qjob ) {
    G3DSCENE *sce  = qjob->qrsg->input.sce;
    G3DCAMERA *cam = qjob->qrsg->input.cam;
    qjob->threaded = 0x01;
    qjob->running  = 0x01;

    q3djob_goToFrame ( qjob, qjob->qrsg->output.startframe );

    q3djob_prepare ( qjob, sce, cam );

    /*** RENDER ! ***/
    q3djob_render ( qjob );

    qjob->running  = 0x00;
    qjob->threaded = 0x00;

    q3djob_free ( qjob );
}

/******************************************************************************/
void q3djob_render ( Q3DJOB *qjob ) {
    clock_t t = clock ( );  
    int i;
    uint32_t doRender;

    /*** http://www.thegeekstuff.com/2012/05/c-mutex-examples/ ***/
    /*** This mutex is used when each rendering thread ***/
    /*** is querying for the next scanline to render   ***/
    if ( pthread_mutex_init ( &qjob->qarea.lock, NULL ) != 0x00 ) {
        fprintf ( stderr, "mutex init failed\n" );

        return;
    }

    doRender = q3djob_filterbefore ( qjob, 
                                     0x00, 
                                     0x00,
                                     qjob->qarea.depth, 
                                     qjob->qarea.width );

    if ( ( doRender != 0x02 ) ) {
        Q3DRAYTRACETHREAD *rtt = calloc ( qjob->nbcpu, sizeof ( Q3DRAYTRACETHREAD ) );

        for ( i = 0x00; i < qjob->nbcpu; i++ ) {
            rtt[i].cpuID = i;
            rtt[i].qjob  = qjob;
        }

        /*** prerender soft shadows ***/
        if ( q3dscene_needsSoftShadows ( qjob->qsce ) ) {
            qjob->flags |= JOBRENDERSOFTSHADOWS;

            for ( i = 0x00; i < qjob->nbcpu; i++ ) {
                q3djob_createRenderThread ( &rtt[i] );
            }

            for ( i = 0x00; i < qjob->nbcpu; i++ ) {
                pthread_join ( rtt[i].tid, NULL );
            }

            q3darea_averageSoftShadows ( &qjob->qarea );

            qjob->flags &= (~JOBRENDERSOFTSHADOWS);
        }

        /*** Start as many threads as CPUs. Each thread queries the parent ***/
        /*** process in order to retrieve the scanline number it must render. ***/
        for ( i = 0x00; i < qjob->nbcpu; i++ ) {
            q3djob_createRenderThread ( &rtt[i] );
        }

        for ( i = 0x00; i < qjob->nbcpu; i++ ) {
            pthread_join ( rtt[i].tid, NULL );
        }

        free ( rtt );
    }

    q3djob_filterimage ( qjob, 
                         qjob->qarea.y1, 
                         qjob->qarea.y2,
                         qjob->qarea.depth, 
                         qjob->qarea.width );

    t = clock() - t;

    printf ("Render took %f seconds.\n", ( float ) t / ( CLOCKS_PER_SEC * qjob->nbcpu ) );
}
