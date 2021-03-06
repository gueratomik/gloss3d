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
#include <r3d.h>

/******************************************************************************/
R3DFILTER *r3dscene_getFilter ( R3DSCENE *rsce, const char *filtername ) {
    LIST *ltmpfilters = rsce->rsg->input.lfilters;

    while ( ltmpfilters ) {
        R3DFILTER *fil = ( R3DFILTER * ) ltmpfilters->data;

        if ( strcmp ( fil->name, filtername ) == 0x00 ) {

           return fil;
        }

        ltmpfilters = ltmpfilters->next;
    }

    return NULL;
}

/******************************************************************************/
void r3dscene_addSubRender ( R3DSCENE *rsce, R3DSCENE *subrsce ) {
    list_insert ( &rsce->lsubrsce, subrsce );
}

/******************************************************************************/
void r3dscene_removeSubRender ( R3DSCENE *rsce, R3DSCENE *subrsce ) {
    list_remove ( &rsce->lsubrsce, subrsce );
}

/******************************************************************************/
void r3dscene_cancelRender ( R3DSCENE *rsce ) {
    LIST *ltmpsubrsce = rsce->lsubrsce;

    printf ( "Stopping rendering threads\n" );

    while ( ltmpsubrsce ) {
        R3DSCENE *subrsce = ( R3DSCENE * ) ltmpsubrsce->data;

        /*** wait for raytracing scanlines to finish ***/
        r3dscene_cancel ( subrsce );

        ltmpsubrsce = ltmpsubrsce->next;
    }

    r3dscene_cancel ( rsce );

    /*Rendering might not be started form a thread, so we have to handle that*/
    if ( rsce->threaded ) {
        #ifdef __linux__
        pthread_join ( rsce->tid, NULL );
        #endif
        #ifdef __MINGW32__
        WaitForSingleObject ( rsce->tid, INFINITE );
        #endif
    }

    /*** COMMENTED: this is done by the parent thread, for example ***/
    /*** r3dscene_render_frame_t ***/
    /*list_free ( &rsce->lfilters, r3dfilter_free );
    r3dobject_free ( rsce );*/
}

/******************************************************************************/
void r3dscene_import ( R3DSCENE *rsce, 
                       uint32_t  dump_flags,
                       uint64_t  engine_flags ) {
    LIST *ltmp = ((R3DOBJECT*)rsce)->obj->lchildren;
    static double IDX[0x10] = { 1.0f, 0.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f, 0.0f,
                                0.0f, 0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, 0.0f, 1.0f };
    uint32_t robjID = ((R3DOBJECT*)rsce)->id++;

    r3dobject_import ( ((R3DOBJECT*)rsce)->obj, &robjID,
                                                 IDX,
                                                 rsce->area.rcam->MVX, 
                                                 rsce->area.rcam->PJX,
                                                 rsce->area.rcam->VPX,
                                                &rsce->lrob, 
                                                &rsce->lrlt,
                                                 dump_flags, 
                                                 engine_flags );
}

/******************************************************************************/
/******** Wait the end of the render process, ie the end of each thread *******/
void r3dscene_wait ( R3DSCENE *rsce ) {
    LIST *ltmp = rsce->lthread;

    while ( ltmp ) {
        #ifdef __linux__
        pthread_t tid = ( pthread_t ) ltmp->data;

        pthread_join ( tid, NULL );

        #endif

        #ifdef __MINGW32__
        HANDLE tid = ( HANDLE ) ltmp->data;

        WaitForSingleObject ( tid, INFINITE );
        #endif

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
/******** Wait the end of the render process, ie the end of each thread *******/
void r3dscene_cancel ( R3DSCENE *rsce ) {
    rsce->running = 0x00;

    rsce->cancelled = 0x01;
}

/******************************************************************************/
void r3dscene_free ( R3DOBJECT *rob ) {
    R3DSCENE *rsce = ( R3DSCENE * ) rob;

    /*** free all rendering threads - one per CPU ***/
    list_free ( &rsce->lthread, NULL );

    list_free ( &rsce->lrob, (void(*)(void*))r3dobject_free );

    free ( rsce->area.img );
    free ( rsce->area.zBuffer );
    free ( rsce->area.rfc );

    r3dcamera_free ( rsce->area.rcam );
#ifdef VERBOSE
    printf ("R3DSCENE Freed\n" );
#endif
}

/******************************************************************************/
static uint32_t r3dscene_getNextLine ( R3DSCENE *rsce,
                                       R3DINTERPOLATION *pone,
                                       R3DINTERPOLATION *ptwo ) {
    R3DAREA *area = &rsce->area;
    uint32_t scanline;

    #ifdef __linux__
    pthread_mutex_lock ( &area->lock );
    #endif
    #ifdef __MINGW32__
    WaitForSingleObject ( area->lock, INFINITE );
    #endif

    scanline = area->scanline++;

    memcpy ( &pone->src, &area->pol[0x00].src, sizeof ( R3DVECTOR ) );
    memcpy ( &pone->dst, &area->pol[0x00].dst, sizeof ( R3DVECTOR ) );

    memcpy ( &ptwo->src, &area->pol[0x01].src, sizeof ( R3DVECTOR ) );
    memcpy ( &ptwo->dst, &area->pol[0x01].dst, sizeof ( R3DVECTOR ) );

    r3dinterpolation_step ( &area->pol[0x00] );
    r3dinterpolation_step ( &area->pol[0x01] );

    #ifdef __linux__
    pthread_mutex_unlock ( &area->lock  );
    #endif
    #ifdef __MINGW32__
    ReleaseMutex ( area->lock );
    #endif

    return scanline;
}

/******************************************************************************/
void rd3scene_filterline ( R3DSCENE *rsce, uint32_t from, 
                                               uint32_t to,
                                               uint32_t depth, 
                                               uint32_t width ) {
    LIST *ltmp = rsce->rsg->input.lfilters;
    char *img = rsce->area.img;

    if ( rsce->running == 0x00 ) return;

    while ( ltmp ) {
        R3DFILTER *fil = ( R3DFILTER * ) ltmp->data;

        if ( ( fil->flags & ENABLEFILTER ) && ( fil->type & FILTERLINE ) ) {
            if ( fil->draw ( fil, rsce, rsce->curframe, img, from, to, depth, width ) ) {
                /*** stop processing filters if 1 is returned ***/
                return;
            }
        }

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void rd3scene_filterimage ( R3DSCENE *rsce, uint32_t from, 
                                            uint32_t to,
                                            uint32_t depth, 
                                            uint32_t width ) {
    LIST *ltmp = rsce->rsg->input.lfilters;
    char *img = rsce->area.img;

    /*** Filter "CLEAN" must be run no matter what ***/
    if ( rsce->running == 0x00 ) {
        while ( ltmp ) {
            R3DFILTER *fil = ( R3DFILTER * ) ltmp->data;

            if ( ( fil->flags & ENABLEFILTER ) && 
                 ( fil->type  & FILTERIMAGE  ) &&
                 ( strcmp ( fil->name, "CLEAN" ) == 0x00 ) ) {
                if ( fil->draw ( fil, rsce, rsce->curframe, img, from, to, depth, width ) ) {
                    /*** stop processing filters if 1 is returned ***/
                    return;
                }
            }

            ltmp = ltmp->next;
        }
    } else {
        while ( ltmp ) {
            R3DFILTER *fil = ( R3DFILTER * ) ltmp->data;

            if ( ( fil->flags & ENABLEFILTER ) && ( fil->type & FILTERIMAGE ) ) {
                if ( fil->draw ( fil, rsce, rsce->curframe, img, from, to, depth, width ) ) {
                    /*** stop processing filters if 1 is returned ***/
                    return;
                }
            }

            ltmp = ltmp->next;
        }
    }
}

/******************************************************************************/
uint32_t rd3scene_filterbefore ( R3DSCENE *rsce, uint32_t from, 
                                                 uint32_t to,
                                                 uint32_t depth, 
                                                 uint32_t width ) {
    LIST *ltmp = rsce->rsg->input.lfilters;
    char *img = rsce->area.img;
    uint32_t ret = 0x00;

    if ( rsce->running == 0x00 ) return 0x00;

    while ( ltmp ) {
        R3DFILTER *fil = ( R3DFILTER * ) ltmp->data;

        if ( ( fil->flags & ENABLEFILTER ) && ( fil->type & FILTERBEFORE ) ) {

            if ( ret = fil->draw ( fil, rsce, rsce->curframe, img, from, to, depth, width ) ) {
                /*** stop processing filters if 1 is returned ***/
                return ret;
            }
        }

        ltmp = ltmp->next;
    }

    return 0x00;
}

/******************************************************************************/
void *r3dscene_raytrace ( void *ptr ) {
    R3DSCENE *rsce = ( R3DSCENE * ) ptr;
    R3DAREA *area = &rsce->area;
    R3DINTERPOLATION pone, ptwo;
    unsigned char *img = area->img;
    uint32_t scanline;
    uint32_t steps  = ( area->x2 - area->x1 );
    uint32_t width  = ( area->x2 - area->x1 ) + 0x01;
    uint32_t height = ( area->y2 - area->y1 ) + 0x01;
    uint32_t bytesperline = ( width * 0x03 );
    uint32_t outlineFlag = ( rsce->rsg->flags & RENDERWIREFRAME ) ? RAYQUERYOUTLINE : 0x00;
    int i;

    /*** return immediately when canceled ***/
    /*pthread_setcanceltype ( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );*/

    while ( ( ( scanline = r3dscene_getNextLine ( rsce,
                                                 &pone,
                                                 &ptwo ) ) <= area->y2 ) && rsce->running ) {
        unsigned char *imgptr = &img[(scanline*bytesperline)];

        _R3DINTERPOLATION_BUILD ( &pone, &ptwo, steps );

        for ( i = area->x1; ( i <= area->x2 ) && rsce->running; i++ ) {
            uint32_t color;
            G3DVECTOR intersectionPointToSource;
            R3DRAY ray;
            float viewingDistance;
            uint32_t offset = ( scanline * area->width ) + i;

            /*** Ray building ***/
            /*** First, reset the ray ***/
            memset ( &ray, 0x00, sizeof ( R3DRAY ) );

            ray.x = i;
            ray.y = scanline;

            /*** then set its source position ***/
            /*memcpy ( &ray.ori, &pone.src, sizeof ( R3DVECTOR ) );*/

            ray.ori.x = pone.src.x;
            ray.ori.y = pone.src.y;
            ray.ori.z = pone.src.z;

            /*** and its destination vector ***/
            ray.dir.x = ( pone.dst.x - pone.src.x );
            ray.dir.y = ( pone.dst.y - pone.src.y );
            ray.dir.z = ( pone.dst.z - pone.src.z );

            /*** This value will be compared to the depth value of the hit  ***/
            /*** point. This allows us to pick the closest hit to the eye.  ***/
            ray.distance = INFINITY;

            /*** but don't forget to normalize the latter **/
            r3dtinyvector_normalize ( &ray.dir, &viewingDistance );

            /*** shoot the ray ***/
            color = r3dray_shoot ( &ray, rsce, 
                                         NULL, 
                                         0x00, 
                                         RAYSTART               |
                                         RAYQUERYHIT            | 
                                         RAYQUERYLIGHTING       |
                                         RAYQUERYREFLECTION     |
                                         RAYQUERYREFRACTION     |
                                         RAYQUERYIGNOREBACKFACE |
                                         outlineFlag );

            imgptr[0x00] = ( color & 0x00FF0000 ) >> 0x10;
            imgptr[0x01] = ( color & 0x0000FF00 ) >> 0x08;
            imgptr[0x02] = ( color & 0x000000FF );

            if ( ray.flags & INTERSECT ) {
                intersectionPointToSource.x = ray.pnt.x - pone.src.x;
                intersectionPointToSource.y = ray.pnt.y - pone.src.y;
                intersectionPointToSource.z = ray.pnt.z - pone.src.z;

                area->zBuffer[offset] = g3dvector_length ( &intersectionPointToSource );
            } else {
                area->zBuffer[offset] = INFINITY;
            }

            if ( rsce->rsg->flags & RENDERFOG ) {
                uint32_t fogR = ( ( rsce->rsg->fog.color & 0x00FF0000 ) >> 0x10 ),
                         fogG = ( ( rsce->rsg->fog.color & 0x0000FF00 ) >> 0x08 ),
                         fogB =   ( rsce->rsg->fog.color & 0x000000FF );

                float fogDistance = area->zBuffer[offset];
                float fogNear = rsce->rsg->fog.fnear;
                float fogFar = rsce->rsg->fog.ffar;
                float deltaFog = ( fogFar - fogNear );
                float incFogRatio = ( deltaFog ) ? ( fogDistance - 
                                                     fogNear ) / deltaFog : 0.0f;
                /*** note: fog strength is between 0.0f and 1.0f ***/
                float decFogRatio = rsce->rsg->fog.strength - incFogRatio;

                if ( fogDistance < fogNear ) {
                    incFogRatio = 0.0f;
                    decFogRatio = 1.0f;
                }

                if ( fogDistance > fogNear ) {
                    if ( incFogRatio > 1.0f ) incFogRatio = 1.0f;

                    incFogRatio = incFogRatio * rsce->rsg->fog.strength;
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

            imgptr += 0x03;

            /*** be ready for the next ray ***/
            _R3DINTERPOLATION_STEP ( &pone );
        }

        rd3scene_filterline ( rsce, scanline, scanline + 0x01, 0x18, width );
    }

    /*** this is needed for memory release ***/
    /*pthread_exit ( NULL );*/


    return NULL;
}

/******************************************************************************/
#ifdef __linux__
void r3dscene_createRenderThread ( R3DSCENE *rsce ) {
    pthread_attr_t attr;
    pthread_t tid;

    pthread_attr_init ( &attr );

    /*** start thread son all CPUs ***/
    pthread_attr_setscope ( &attr, PTHREAD_SCOPE_SYSTEM );

    /*** launch rays ***/
    pthread_create ( &tid, &attr, r3dscene_raytrace, rsce );

    /*** register our thread for r3dscene_wait() can use pthread_join() ***/
    list_insert ( &rsce->lthread, ( void * ) tid );
}
#endif

#ifdef __MINGW32__
void r3dscene_createRenderThread ( R3DSCENE *rsce ) {
    DWORD  thread;
    HANDLE tid = CreateThread ( NULL, 
                                0,
                                (LPTHREAD_START_ROUTINE) r3dscene_raytrace, 
                                rsce,
                                0,
                                &thread );

    /*** register our thread for r3dscene_wait() can use pthread_join() ***/
    list_insert ( &rsce->lthread, ( void * ) tid );
}
#endif

/******************************************************************************/
R3DSCENE *r3dscene_new ( R3DRENDERSETTINGS *rsg, uint32_t dump_flags,
                                                 uint64_t robj_flags ) {
    uint32_t structsize = sizeof ( R3DSCENE );
    uint32_t bytesperline = ( rsg->output.width * 0x03 );
    G3DOBJECT *obj = ( G3DOBJECT * ) rsg->input.sce;
    R3DSCENE *rsce;
    int i;

    if ( ( rsce = ( R3DSCENE * ) calloc ( 0x01, structsize ) ) == NULL ) {
        fprintf ( stderr, "r3dscene_new: memory allocation failed\n" );

        return NULL;
    }

    rsce->rsg = rsg;

    ((R3DOBJECT*)rsce)->obj = obj;

    /*** This 24bpp buffer will receive the raytraced pixel values ***/
    if ( ( rsce->area.img = calloc ( rsg->output.height, bytesperline ) ) == NULL ) {
        fprintf ( stderr, "r3dscene_new: image memory allocation failed\n" );

        free ( rsce );

        return NULL;
    }

    /*** This 24bpp buffer will receive the raytraced pixel values ***/
    if ( ( rsce->area.zBuffer = calloc ( rsg->output.width * 
                                         rsg->output.height, sizeof ( float ) ) ) == NULL ) {
        fprintf ( stderr, "r3dscene_new: ZBuffer memory allocation failed\n" );

        free ( rsce->area.img );
        free ( rsce );

        return NULL;
    }

    for ( i = 0; i < rsg->output.width * rsg->output.height; i++ ) {
        rsce->area.zBuffer[i] = INFINITY;
    }

    /*** This fce buffer for storing face-pixel information ***/
    if ( ( rsce->area.rfc = ( R3DFACE ** ) calloc ( rsg->output.width * 
                                                    rsg->output.height, sizeof ( R3DFACE * ) ) ) == NULL ) {
        fprintf ( stderr, "r3dscene_new: fac memory allocation failed\n" );

        free ( rsce->area.img );
        free ( rsce->area.zBuffer );
        free ( rsce );

        return NULL;
    }

    /*** default background color ***/
    /*rsce->backgroundMode            = backgroundMode;
    rsce->backgroundColor           = backgroundColor;
    rsce->backgroundImage           = backgroundImage;
    rsce->backgroundImageWidthRatio = backgroundImageWidthRatio;*/

    /*** viewing camera ***/
    rsce->area.rcam = r3dcamera_new ( rsg->input.cam, 
                                      rsg->output.width,
                                      rsg->output.height );

    /*rsce->startframe = startframe;
    rsce->endframe   = endframe;*/
    rsce->curframe   = rsg->output.startframe;

    /*** render square area ***/
    rsce->area.x1     = rsg->output.x1;
    rsce->area.x2     = rsg->output.x2;
    rsce->area.y1     = rsg->output.y1;
    rsce->area.y2     = rsg->output.y2;
    rsce->area.width  = rsg->output.width;
    rsce->area.height = rsg->output.height;
    rsce->area.depth  = 0x18;

    /*** first scan line ***/
    rsce->area.scanline = rsg->output.y1;

    /*rsce->wireframe          = outline;
    rsce->wireframeLighting  = wireframeLighting;
    rsce->wireframeColor     = wireframeColor;
    rsce->wireframeThickness = wireframeThickness;*/

    rsce->running = 0x01;

    /*** Compute the interpolation factors for rays ***/
    r3darea_viewport ( &rsce->area, rsg->output.x1, 
                                    rsg->output.y1, 
                                    rsg->output.x2, 
                                    rsg->output.y2, 
                                    rsg->output.width,
                                    rsg->output.height );

    r3dobject_init ( ( R3DOBJECT * ) rsce, 0, obj->type, robj_flags, r3dscene_free );

    /*** Convert objects into world oriented objects ***/
    r3dscene_import ( rsce, dump_flags, 0x00 );

    return rsce;
}

/******************************************************************************/
void r3dscene_render_t_free ( R3DSCENE *rsce ) {
    /*** free filters after rendering ***/
    if ( ( ((R3DOBJECT*)rsce)->flags & NOFREEFILTERS ) == 0x00 ) {
        list_free ( &rsce->rsg->input.lfilters, r3dfilter_free );
    }

    /*** free the first frame ***/
    r3dobject_free  ( ( R3DOBJECT * ) rsce );
}

/******************************************************************************/
void *r3dscene_render_sequence_t ( R3DSCENE *rsce ) {
    G3DSCENE *sce  = ( G3DSCENE *  ) ((R3DOBJECT*)rsce)->obj;
    G3DCAMERA *cam = ( G3DCAMERA * ) ((R3DOBJECT*)rsce->area.rcam)->obj;
    uint32_t x1 = rsce->area.x1, 
             y1 = rsce->area.y1,
             x2 = rsce->area.x2,
             y2 = rsce->area.y2;
    uint32_t width  = rsce->area.width,
             height = rsce->area.height;
    LIST *lfilters = rsce->rsg->input.lfilters;
    int32_t startframe = rsce->rsg->output.startframe,
            endframe = rsce->rsg->output.endframe;
    int32_t i, j;

    rsce->threaded = 0x01;

    /*** Render the first frame ***/
    r3dscene_render ( rsce );

    for ( i = startframe + 1; i <= endframe; i++ ) {
        if ( rsce->running ) {
            R3DSCENE *nextrsce;

            nextrsce = r3dscene_new ( rsce->rsg, 0x00, NOFREEFILTERS );

            nextrsce->curframe = i;

            /*** register this child renderscene in case we need to cancel it ***/
            r3dscene_addSubRender ( rsce, nextrsce );

            /*** Render the current frame ***/
            r3dscene_render ( nextrsce );

            /*** unregister this child renderscene. No need to cancel it ***/
            /*** anymore after this step, all threads are over after ***/
            /*** r3dscene_render().  ***/
            r3dscene_removeSubRender ( rsce, nextrsce );

            /*** Free the current frame ***/
            r3dobject_free  ( ( R3DOBJECT * ) nextrsce );
        }
    }

    rsce->running = 0x00;

    /*** this is needed for memory release ***/
    #ifdef __linux__
    pthread_exit ( NULL );
    #endif

    return NULL;
}

/******************************************************************************/
void *r3dscene_render_frame_t ( R3DSCENE *rsce ) {
    /*** return immediately when canceled ***/
    /*pthread_setcanceltype ( PTHREAD_CANCEL_ASYNCHRONOUS, NULL );*/

    /*** clean up rsce on exit ***/
    /*pthread_cleanup_push  ( (void(*)(void*))r3dscene_render_t_cancel, rsce );*/

    rsce->threaded = 0x01;

    /*** RENDER ! ***/
    r3dscene_render ( rsce );

    rsce->running = 0x00;

    /*pthread_cleanup_pop ( 0x00 );*/

    /*** calls r3dscene_render_t_free ***/
    /*r3dscene_render_t_free ( rsce );*/

    /*** this is needed for memory release ***/
    /*pthread_exit ( NULL );*/




    return NULL;
}

/******************************************************************************/
void r3dscene_render ( R3DSCENE *rsce ) {
    uint32_t nbcpu = g3dcore_getNumberOfCPUs ( );
    clock_t t = clock ( );  
    int i;
    uint32_t doRender;

    /*** http://www.thegeekstuff.com/2012/05/c-mutex-examples/ ***/
    /*** This mutex is used when each rendering thread ***/
    /*** is querying for the next scanline to render   ***/
    #ifdef __linux__
    if ( pthread_mutex_init ( &rsce->area.lock, NULL ) != 0x00 ) {
        fprintf ( stderr, "mutex init failed\n" );

        return;
    }
    #endif

    #ifdef __MINGW32__
    rsce->area.lock = CreateMutex ( NULL, FALSE, NULL );
    #endif

    doRender = rd3scene_filterbefore ( rsce, 0, 
                                             0,
                                             rsce->area.depth, 
                                             rsce->area.width );

    if ( doRender != 0x02 ) {
        /*** Start as many threads as CPUs. Each thread queries the parent ***/
        /*** process in order to retrieve the scanline number it must render. ***/
        for ( i = 0x00; i < nbcpu; i++ ) {
            r3dscene_createRenderThread ( rsce );
        }

        /*** wait all render threads to finish ***/ 
        r3dscene_wait ( rsce );
    }

    rd3scene_filterimage ( rsce, rsce->area.y1, 
                                 rsce->area.y2,
                                 rsce->area.depth, 
                                 rsce->area.width );

    t = clock() - t;
#ifdef VERBOSE
    printf ("Render took %f seconds.\n", ( float ) t / ( CLOCKS_PER_SEC * nbcpu ) );
#endif
}
