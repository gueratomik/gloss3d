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
#include <g3dui.h>

/******************************************************************************/
void g3dui_processAnimatedImages ( G3DUI *gui ) {
    Q3DSETTINGS *rsg = gui->currsg;
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );

    if ( rsg ) {
        g3dscene_processAnimatedImages ( gui->sce, 
                                         rsg->output.startframe,
                                         gui->curframe,
                                         rsg->output.endframe,
                                         rsg->output.fps,
                                         gui->engine_flags );
    }
}

/******************************************************************************/
G3DUIRENDERPROCESS *g3dui_render_q3d ( G3DUI       *gui, 
                                       Q3DSETTINGS *rsg,
                                       Q3DFILTER   *towindow,
                                       Q3DFILTER   *toframe,
                                       Q3DFILTER   *tostatus,
                                       Q3DFILTER   *makepreview,
                                       G3DCAMERA   *cam,
                                       float        resetFrame,
                                       uint64_t     id,
                                       uint32_t     sequence,
                                       uint64_t     job_flags ) {
    G3DSCENE *sce = gui->sce;

    /*** Don't start a new render before the current one has finished ***/
    /*if ( rpc == NULL ) {*/
        G3DUIRENDERPROCESS *rps;
        Q3DJOB *qjob  = NULL;
        G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
        pthread_attr_t attr;
        pthread_t tid;

        /*** Remember the thread id for cancelling on mouse input e.g ***/
        /*** We use the widget as an ID. This is done before QJOB creation ***/
        /*** becaue preparing the scene might take some time during which ***/
        /*** a user could start another rendering thread. So the association***/
        /** is made in g3duirenderprocess_init ( ) **/
        rps = g3duirenderprocess_new ( id );

        /*** register the renderprocess so that we can cancel it ***/
        list_insert ( &gui->lrps, rps );

        /*r3dfilter_setType ( towin, FILTERLINE );*/

        pthread_attr_init ( &attr );

        /*** start thread son all CPUs ***/
        pthread_attr_setscope ( &attr, PTHREAD_SCOPE_SYSTEM );

        qjob = q3djob_new ( rsg, 
                            sce,
                            cam,
                            towindow, 
                            toframe, 
                            tostatus, 
                            makepreview, 
                            job_flags );

        g3duirenderprocess_init( rps, gui, qjob, towindow, toframe );

        /*** launch rays in a thread ***/
        if ( sequence ) {
            pthread_create ( &rps->tid, &attr, (void*(*)(void*))g3duirenderprocess_render_sequence_t, rps );
        } else {
            pthread_create ( &rps->tid, &attr, (void*(*)(void*))g3duirenderprocess_render_frame_t, rps );
        }

        /*** prepare to release resources after thread termination ***/
        /*pthread_detach ( tid );*/
    /*}*/

    return rps;
}

/******************************************************************************/
uint32_t g3dui_selectAllCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;

    /*** select all objects ***/
    if ( gui->engine_flags & VIEWOBJECT ) {
        g3dscene_unselectAllObjects ( sce, gui->engine_flags );
        g3dscene_selectAllObjects   ( sce, gui->engine_flags );
    }

    /*** select all vertices ***/
    if ( gui->engine_flags & VIEWVERTEX ) {
        G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

        if ( obj && obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;

            g3dmesh_unselectAllVertices ( mes );
            g3dmesh_selectAllVertices   ( mes );
        }
    }

    /*** select all edges ***/
    if ( gui->engine_flags & VIEWEDGE ) {
        G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

        if ( obj && obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;

            g3dmesh_unselectAllEdges ( mes );
            g3dmesh_selectAllEdges   ( mes );
        }
    }

    /*** select all vertices ***/
    if ( gui->engine_flags & VIEWFACE ) {
        G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

        if ( obj && obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;

            g3dmesh_unselectAllFaces ( mes );
            g3dmesh_selectAllFaces   ( mes );
        }
    }


    return REDRAWVIEW | REDRAWLIST;

    /*#g3dui_redrawGLViews ( gui );
    g3dui_redrawObjectList     ( gui );#*/
}

/******************************************************************************/
uint64_t getTotalMemory ( ) {
    #ifdef __linux__
    long long unsigned int memsize = 0x00;
    char str[0x100];
    FILE *fp;

    if ( ( fp =  fopen ( "/proc/meminfo", "r" ) ) == NULL ) {
        fprintf ( stderr, "Could not open /proc/meminfo\n" );

        return 0x00;
    }

    while ( feof ( fp ) == 0x00 ) {
        fgets ( str, sizeof ( str ), fp );

        if ( strstr ( str, "MemTotal" ) ) {
            sscanf ( str, "MemTotal: %llu kB", &memsize  );

            fclose ( fp );

            return ( memsize * 1024 );
        }

        memset ( str, 0x00, sizeof ( str ) );
    }

    fclose ( fp );

    return 0x00;
    #endif

    #ifdef __MINGW32__
        MEMORYSTATUS mse;

        GlobalMemoryStatus ( &mse );

        return mse.dwAvailPhys;
    #endif
}

/******************************************************************************/
void g3dui_useRenderSettings ( G3DUI       *gui,
                               Q3DSETTINGS *rsg ) {
    gui->currsg = rsg;
}

/******************************************************************************/
void g3dui_addRenderSettings ( G3DUI       *gui,
                               Q3DSETTINGS *rsg ) {
    list_insert ( &gui->lrsg, rsg );
}

/******************************************************************************/
G3DUIRENDERPROCESS *g3dui_getRenderProcessByID ( G3DUI   *gui,
                                                 uint64_t id ) {
    LIST *ltmprps = gui->lrps;

    while ( ltmprps ) {
        G3DUIRENDERPROCESS *rps = ( G3DUIRENDERPROCESS * ) ltmprps->data;

        if ( rps->id == id ) {

            return rps;
        }

        ltmprps = ltmprps->next;
    }

    return NULL;
}

/******************************************************************************/
G3DUIRENDERPROCESS *g3dui_getRenderProcessByJob ( G3DUI  *gui,
                                                  Q3DJOB *qjob ) {
    LIST *ltmprps = gui->lrps;

    while ( ltmprps ) {
        G3DUIRENDERPROCESS *rps = ( G3DUIRENDERPROCESS * ) ltmprps->data;

        if ( rps->qjob == qjob ) {

            return rps;
        }

        ltmprps = ltmprps->next;
    }

    return NULL;
}

/******************************************************************************/
uint32_t g3dui_cancelRenderByScene ( G3DUI  *gui, 
                                     Q3DJOB *qjob ) {
    G3DUIRENDERPROCESS *rps = g3dui_getRenderProcessByJob ( gui, qjob );

    if ( rps ) {
        q3djob_end ( rps->qjob );

        /*** Commented out: creates a deadlock with the mutex from ***/
        /*** gotoframe filter. Need to investigate more ***/
        /*pthread_join ( rps->tid, NULL );*/

        return 0x01;
    }

    return 0x00;
}

/******************************************************************************/
uint32_t g3dui_cancelRenderByID ( G3DUI   *gui, 
                                  uint64_t id ) {
    G3DUIRENDERPROCESS *rps = g3dui_getRenderProcessByID ( gui, id );

    if ( rps ) {
        if ( rps->qjob ) q3djob_end ( rps->qjob );

        /*** Commented out: creates a deadlock with the mutex from ***/
        /*** gotoframe filter. Need to investigate more ***/
        /*pthread_join ( rps->tid, NULL );*/


        return 0x01;
    }

    return 0x00;
}

/******************************************************************************/
void g3dui_exitokcbk ( G3DUI *gui ) {
    g3durmanager_clear ( gui->urm );

    if ( gui->sce ) {
        g3dobject_free ( ( G3DOBJECT * ) gui->sce );
    }

    /*** More cleaning would be needed ***/

    exit ( EXIT_SUCCESS );
}

/******************************************************************************/
uint32_t g3dui_exportfileokcbk ( G3DUI      *gui,
                                 const char *filedesc, 
                                 const char *filename ) {
    /*#g3dui_setHourGlass ( gui );#*/

    printf ( "Exporting to %s of type %s ...\n", filename, filedesc );

    if ( strcmp ( filedesc, FILEDESC_POV ) == 0x00 ) {
        g3dscene_exportPov ( gui->sce, filename, "/*Made with Gloss3D-" PACKAGE_VERSION "*/\n\n", 0 );
    }

    if ( strcmp ( filedesc, FILEDESC_OBJ ) == 0x00 ) {
        g3dscene_exportObj ( gui->sce, filename, "#Made with Gloss3D-" PACKAGE_VERSION "\n\n", 0 );
    }

    if ( strcmp ( filedesc, FILEDESC_STA ) == 0x00 ) {
        g3dscene_exportStlA ( gui->sce, filename, "#Made with Gloss3D-" PACKAGE_VERSION "\n\n", 0 );
    }

    /*if ( strcmp ( filedesc, FILEDESC_V1 ) == 0x00 ) {
        g3dscene_write ( gui->sce,
                         gui->filename, 
                         "Made with Gloss3D-" PACKAGE_VERSION,
                         NULL,
                         0x00 );
    }*/

    /*#g3dui_unsetHourGlass ( gui );

    g3dui_redrawGLViews        ( gui );
    g3dui_updateCoords         ( gui );
    g3dui_redrawObjectList     ( gui );
    g3dui_updateAllCurrentEdit ( gui );#*/

    return REDRAWVIEW   |
           REDRAWCOORDS |
           REDRAWLIST   |
           REDRAWCURRENTOBJECT;
}

/******************************************************************************/
G3DSCENE *g3dui_importfileokcbk ( G3DUI      *gui, 
                                  const char *filedesc, 
                                  const char *filename ) {
    G3DSCENE *sce = gui->sce;

    /*#g3dui_setHourGlass ( gui );#*/

#ifdef __linux__
    if ( access( filename, F_OK ) == 0x00 ) {
#endif
#ifdef __MINGW32__
    if ( PathFileExists ( filename ) ) {
#endif
        printf ( "Opening %s of type %s ...\n", filename, filedesc );

        g3durmanager_clear ( gui->urm );

        /*** free memory the previous scene ***/
        if ( gui->sce ) {
            g3dobject_free ( ( G3DOBJECT * ) gui->sce );
        }

        if ( strcmp ( filedesc, FILEDESC_3DS ) == 0x00 ) {
            gui->sce = g3dscene_import3ds ( filename, gui->engine_flags );
        }

        if ( strcmp ( filedesc, FILEDESC_OBJ ) == 0x00 ) {
            gui->sce = g3dscene_importObj ( filename, gui->engine_flags );
        }
#ifdef HAVE_EXPAT_H
        if ( strcmp ( filedesc, FILEDESC_DAE ) == 0x00 ) {
            gui->sce = g3dscene_importCollada ( filename, gui->engine_flags );
        }
#endif
#ifdef HAVE_C4D_H
        if ( strcmp ( filedesc, FILEDESC_C4D ) == 0x00 ) {
            gui->sce = g3dscene_importC4D ( filename, gui->engine_flags );
        }
#endif
        if ( strcmp ( filedesc, FILEDESC_V2 ) == 0x00 ) {
            gui->sce = g3dscene_importv2 ( filename, 
                                           NULL, 
                                           NULL, 
                                           gui->engine_flags );
        }

        if ( gui->sce ) {
            g3dui_setFileName ( gui, filename );

            printf ( "...Done!\n" );

	        /*g3dui_clearMaterials ( gui );
	        g3dui_importMaterials ( gui );*/

            sce = gui->sce;
        } else {
            gui->sce = g3dscene_new ( 0x00, "Gloss3D scene" );
        }
    }

    /*#g3dui_updateGLViewsMenu    ( gui );
    g3dui_redrawGLViews        ( gui );
    g3dui_updateCoords         ( gui );
    g3dui_redrawObjectList     ( gui );
    g3dui_updateAllCurrentEdit ( gui );

    g3dui_unsetHourGlass ( gui );#*/

    return sce;
}

/******************************************************************************/
void g3dui_saveG3DFile ( G3DUI *gui ) {
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
    G3DEXPORTV3EXTENSION *r3dext, *g3duiext;
    LIST *lext = NULL;

    /*#g3dui_setHourGlass ( gui );#*/

    /*
     * we put this here and not in gloss3d initialization because the pointer
     * gui->lrsg is going to heck when we add a new set of render settings.
     */
    r3dext = g3dexportv3extension_new ( SIG_RENDERSETTINGS_EXTENSION,
                      EXPORTV3_CALLBACK(q3dsettings_write),
                                        gui->lrsg );

    g3duiext = g3dexportv3extension_new ( SIG_G3DUI,
                        EXPORTV3_CALLBACK(g3dui_write),
                                          gui );

    list_insert ( &lext, r3dext   );
    list_insert ( &lext, g3duiext );

    g3dscene_exportv3 ( gui->sce, 
                        gui->filename, 
                       "Made with GLOSS3D", 
                        lext, 
                        0x00 );

    g3dexportv3extension_free ( g3duiext );
    g3dexportv3extension_free ( r3dext   );

    list_free ( &lext, NULL );

    /*#g3dui_unsetHourGlass ( gui );#*/
}

/******************************************************************************/
void g3dui_setFileName ( G3DUI      *gui, 
                         const char *filename ) {
    int len;

    if ( gui->filename ) {
        free ( gui->filename );

        gui->filename = NULL;
    }

    len = strlen ( filename );

    /*** 0x04 is for the extension ***/
    if ( strstr ( filename, ".g3d" ) ) {
        gui->filename = ( char * ) calloc ( len + 0x01       , sizeof ( char ) );
    } else {
        gui->filename = ( char * ) calloc ( len + 0x01 + 0x04, sizeof ( char ) );

        strncpy ( gui->filename + len, ".g3d", 0x04 );
    }

    if ( gui->filename == NULL ) {
        fprintf ( stderr, "g3duiSaveG3DFile: calloc failed\n" );
    } else {
        memcpy ( gui->filename, filename, len );
    }
}

/******************************************************************************/
uint32_t g3dui_setMouseTool ( G3DUI        *gui, 
                              G3DCAMERA    *cam, 
                              G3DMOUSETOOL *mou ) {
    uint32_t msk = 0x00;

    /*** Call the mouse tool initialization function once. This ***/
    /*** can be used by this function to initialize some values ***/
    if ( mou ) {
        if ( mou->init ) {
            msk = mou->init ( mou, 
                              gui->sce, 
                              cam, 
                              gui->urm,
                              gui->engine_flags );
        }

        if ( ( mou->flags & MOUSETOOLNOCURRENT ) == 0x00 ) {
            gui->mou = mou;
        }
    } else {
        gui->mou = NULL;
    }

    return msk;
}

/******************************************************************************/
G3DMOUSETOOL *g3dui_getMouseTool ( G3DUI      *gui, 
                                   const char *name ) {
    LIST *ltmp = gui->lmou;

    while ( ltmp ) {
        G3DMOUSETOOL *mou = ( G3DMOUSETOOL * ) ltmp->data;

        if ( strcmp ( mou->name, name ) == 0x00 ) {

            return mou;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
void g3dui_addMouseTool ( G3DUI        *gui, 
                          G3DMOUSETOOL *mou ) {
    list_insert ( &gui->lmou, mou );
}

/******************************************************************************/
void g3dui_resetDefaultCameras ( G3DUI *gui ) {
    uint32_t camflags[0x04] = { CAMERAPERSPECTIVE,
                                CAMERAORTHOGRAPHIC | CAMERAXY | OBJECTNOROTATION,
                                CAMERAORTHOGRAPHIC | CAMERAYZ | OBJECTNOROTATION,
                                CAMERAORTHOGRAPHIC | CAMERAZX | OBJECTNOROTATION };
    G3DVECTOR campos[0x04] = { {       10.0f,        3.0f,       10.0f, 1.0f },
                               {        0.0f,        0.0f,        0.0f, 1.0f },
                               {        0.0f,        0.0f,        0.0f, 1.0f },
                               {        0.0f,        0.0f,        0.0f, 1.0f } },
              camrot[0x04] = { {    -12.516f,        45.0,        0.0f, 1.0f },
                               {        0.0f,        0.0f,        0.0f, 1.0f },
                               {        0.0f,      -90.0f,        0.0f, 1.0f },
                               {      -90.0f,        0.0f,        0.0f, 1.0f } },
              camsca[0x04] = { {        1.0f,        1.0f,        1.0f, 1.0f },
                               {        1.0f,        1.0f,        1.0f, 1.0f },
                               {        1.0f,        1.0f,        1.0f, 1.0f },
                               {        1.0f,        1.0f,        1.0f, 1.0f } };
    float camfoc[0x04] = { 45.0f, 2.0f, 2.0f, 2.0f };
    uint32_t i;

    for ( i = 0x00; i < 0x04; i++ ) {
        G3DCAMERA *cam = gui->defaultCameras[i];
        G3DOBJECT *objcam = ( G3DOBJECT * ) cam;

        cam->focal = camfoc[i];

        objcam->flags |= camflags[i];

        memcpy ( &objcam->pos, &campos[i], sizeof ( G3DVECTOR ) );
        memcpy ( &objcam->rot, &camrot[i], sizeof ( G3DVECTOR ) );
        memcpy ( &objcam->sca, &camsca[i], sizeof ( G3DVECTOR ) );

        g3dobject_updateMatrix ( objcam, gui->engine_flags );
    }
}

/******************************************************************************/
/*** Create the 4 default cameras ***/
void g3dui_createDefaultCameras ( G3DUI *gui ) {
    uint32_t ptrSize = sizeof ( G3DCAMERA * );
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );

    gui->defaultCameras = ( G3DCAMERA ** ) calloc ( 0x04, ptrSize );


    gui->defaultCameras[0x00] = g3dcamera_new ( 0x00, 
                                                "Camera",
                                                0.0f,
                                                1.0f,
                                                0.1f,
                                                1000.0f );

    gui->defaultCameras[0x01] = g3dcamera_new ( 0x01, 
                                                "Camera",
                                                0.0f,
                                                1.0f,
                                               -1000.0f,
                                                1000.0f );

    gui->defaultCameras[0x02] = g3dcamera_new ( 0x02,
                                                "Camera",
                                                0.0f,
                                                1.0f,
                                               -1000.0f,
                                                1000.0f );

    gui->defaultCameras[0x03] = g3dcamera_new ( 0x03,
                                                "Camera",
                                                0.0f,
                                                1.0f,
                                               -1000.0f,
                                                1000.0f );

    g3dui_resetDefaultCameras ( gui );

    sysinfo->defaultCamera = gui->defaultCameras[0x00];
}

/******************************************************************************/
uint64_t g3dui_openG3DFile ( G3DUI      *gui, 
                             const char *filename ) {
    G3DIMPORTV3EXTENSION *r3dext,
                         *g3duiext;
    LIST *lext = NULL;
    LIST *lrsg = NULL;

#ifdef __linux__
    if ( access( filename, F_OK ) == 0x00 ) {
#endif
#ifdef __MINGW32__
    if ( PathFileExists ( filename ) ) {
#endif
        printf ( "Opening %s ...\n", filename );

        g3durmanager_clear ( gui->urm );

        /*** free memory the previous scene ***/
        if ( gui->sce ) {
            g3dobject_free ( ( G3DOBJECT * ) gui->sce );
        }

        list_free ( &gui->lrsg, LIST_FUNCDATA(q3dsettings_free) );

        /* import render settings module */
        r3dext = g3dimportv3extension_new ( SIG_RENDERSETTINGS_EXTENSION,
                          IMPORTV3_CALLBACK(q3dsettings_read),
                                           &lrsg );
        /* import G3DUI settings module */
        g3duiext = g3dimportv3extension_new ( SIG_G3DUI,
                            IMPORTV3_CALLBACK(g3dui_read),
                                              gui );

        list_insert ( &lext, r3dext   );
        list_insert ( &lext, g3duiext );

        gui->sce = g3dscene_importv3 ( filename, NULL, lext, gui->engine_flags );
    }

    if ( lrsg ) {
        G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
        LIST *ltmprsg = lrsg;

        while ( ltmprsg ) {
            Q3DSETTINGS *rsg = ( Q3DSETTINGS * ) ltmprsg->data;

            /*if ( rsg->flags & RENDERDEFAULT ) {*/
                gui->currsg = rsg;
                g3dui_addRenderSettings ( gui, rsg );
                g3dui_useRenderSettings ( gui, rsg );

                /*** that's kind of a global variable, because ***/
                /*** I did not want to change the engine to display ***/
                /*** a background image. But this must be redesigned **/
                sysinfo->backgroundImage = rsg->background.image;
            /*}*/

            ltmprsg = ltmprsg->next;
        }
    } else {
        Q3DSETTINGS *defaultRsg = q3dsettings_new ( );

        g3dui_addRenderSettings ( gui, defaultRsg );
        g3dui_useRenderSettings ( gui, defaultRsg );
    }

    g3dimportv3extension_free ( g3duiext );
    g3dimportv3extension_free ( r3dext   );

    list_free ( &lext, NULL );
    list_free ( &lrsg, NULL );

    if ( gui->sce ) {
        g3dui_setFileName ( gui, filename );

        printf ( "...Done!\n" );
    } else {
        gui->sce = g3dscene_new ( 0x00, "Gloss3D scene" );
    }

    return REDRAWVIEW          |
           REBUILDMATERIALLIST |
           REDRAWVIEWMENU      |
           REDRAWLIST          |
           REDRAWCOORDS        |
           REDRAWCURRENTOBJECT;
}

/******************************************************************************/
uint64_t g3dui_redo ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;

    return g3durmanager_redo ( urm, gui->engine_flags );
}

/******************************************************************************/
uint64_t g3dui_undo ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;

    return g3durmanager_undo ( urm, gui->engine_flags );
}

/******************************************************************************/
void g3dui_init ( G3DUI *gui ) {
    Q3DSETTINGS *rsg;
    #ifdef __linux__
    char *home = getenv ( "HOME" );
    #endif
    #ifdef __MINGW32__
    static char *home = getenv ( "USERPROFILE" );
    #endif
    static char configFileName[0x1000];
    char *loadFile = NULL;

    #ifdef __linux__
    snprintf ( configFileName, 0x1000, "%s/.gloss3d/gloss3d.conf", home );
    #endif
    #ifdef __MINGW32__
    snprintf ( configFileName, 0x1000, "%s\\.gloss3d\\gloss3d.conf", home );
    #endif

    g3dui_loadConfiguration    ( gui, configFileName );
    g3dui_createDefaultCameras ( gui );

    /*** undo redo manager ***/
    gui->urm = g3durmanager_new ( gui->conf.undolevel );

    gui->lrps = NULL;

    /*** copy-paste manager ***/
    gui->cli = g3duiclipboard_new ( );

    gui->engine_flags = ( VIEWOBJECT | XAXIS | YAXIS | ZAXIS );

    rsg = q3dsettings_new ( ); /*** default render settings ***/

    gui->toframe = q3dfilter_gotoframe_new ( gui );

    g3dui_addRenderSettings ( gui, rsg );
    g3dui_useRenderSettings ( gui, rsg );
}
