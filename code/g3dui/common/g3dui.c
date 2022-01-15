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
#include <xpm/cube.xpm>
#include <xpm/cylinder.xpm>
#include <xpm/knife.xpm>
#include <xpm/addvertex.xpm>
#include <xpm/bridge.xpm>
#include <xpm/extrude.xpm>

/******************************************************************************/
void common_g3dui_processAnimatedImages ( G3DUI *gui ) {
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
G3DUIRENDERPROCESS *common_g3dui_render_q3d ( G3DUI       *gui, 
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

        /*** Remember the thread id for cancelling on mouse input e.g ***/
        /*** We use the widget as an ID ***/
        rps = g3duirenderprocess_new ( id, gui, qjob, towindow, toframe );

        /*** launch rays in a thread ***/
        if ( sequence ) {
            pthread_create ( &rps->tid, &attr, (void*(*)(void*))g3duirenderprocess_render_sequence_t, rps );
        } else {
            pthread_create ( &rps->tid, &attr, (void*(*)(void*))g3duirenderprocess_render_frame_t, rps );
        }

        /*** register the renderprocess so that we can cancel it ***/
        list_insert ( &gui->lrps, rps );

        /*** prepare to release resources after thread termination ***/
        /*pthread_detach ( tid );*/
    /*}*/

    return rps;
}

/******************************************************************************/
G3DUIRENDERPROCESS *common_g3dui_render ( G3DUI       *gui,
                                          Q3DSETTINGS *rsg,
                                          Q3DFILTER   *towindow,
                                          Q3DFILTER   *toframe,
                                          Q3DFILTER   *tostatus,
                                          Q3DFILTER   *makepreview,
                                          float        resetFrame,
                                          uint64_t     id,
                                          uint32_t     sequence ) {
    G3DSCENE *sce = rsg->input.sce;
    G3DCAMERA *cam = rsg->input.cam;

    /*** Don't start a new render before the current one has finished ***/
    /*if ( rpc == NULL ) {*/
        G3DUIRENDERPROCESS *rps;
        Q3DJOB *qjob  = NULL;
        G3DSYSINFO *sysinfo = g3dsysinfo_get ( );

        pthread_attr_t attr;
        pthread_t tid;

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
                            0x00 );

        /*** Remember the thread id for cancelling on mouse input e.g ***/
        /*** We use the widget as an ID ***/
        rps = g3duirenderprocess_new ( id, gui, qjob, NULL/*towin*/, NULL/*tobuf*/ );

        /*** launch rays in a thread ***/
        if ( sequence ) {
            pthread_create ( &rps->tid, 
                             &attr, (void*(*)(void*))g3duirenderprocess_render_sequence_t, rps );
        } else {
            pthread_create ( &rps->tid, 
                             &attr, (void*(*)(void*))g3duirenderprocess_render_frame_t, rps );
        }

        /*** register the renderprocess so that we can cancel it ***/
        list_insert ( &gui->lrps, rps );

        /*** prepare to release resources after thread termination ***/
        /*pthread_detach ( tid );*/
    /*}*/

    return rps;
}

/******************************************************************************/
void common_g3dui_selectAllCbk ( G3DUI *gui ) {
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

    g3dui_redrawGLViews ( gui );
    g3dui_redrawObjectList     ( gui );
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
void common_g3dui_useRenderSettings ( G3DUI *gui, Q3DSETTINGS *rsg ) {
    gui->currsg = rsg;
}

/******************************************************************************/
void common_g3dui_addRenderSettings ( G3DUI *gui, Q3DSETTINGS *rsg ) {
    list_insert ( &gui->lrsg, rsg );
}

/******************************************************************************/
G3DUIRENDERPROCESS *common_g3dui_getRenderProcessByID ( G3DUI *gui, uint64_t id ) {
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
G3DUIRENDERPROCESS *common_g3dui_getRenderProcessByJob ( G3DUI  *gui, 
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
uint32_t common_g3dui_cancelRenderByScene ( G3DUI *gui, 
                                            Q3DJOB *qjob ) {
    G3DUIRENDERPROCESS *rps = common_g3dui_getRenderProcessByJob ( gui, qjob );

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
uint32_t common_g3dui_cancelRenderByID ( G3DUI *gui, uint64_t id ) {
    G3DUIRENDERPROCESS *rps = common_g3dui_getRenderProcessByID ( gui, id );

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
void common_g3dui_exitokcbk ( G3DUI *gui ) {
    g3durmanager_clear ( gui->urm );

    if ( gui->sce ) {
        g3dobject_free ( ( G3DOBJECT * ) gui->sce );
    }

    /*** More cleaning would be needed ***/

    exit ( EXIT_SUCCESS );
}

/******************************************************************************/
void common_g3dui_exportfileokcbk ( G3DUI *gui, const char *filedesc, 
                                                const char *filename ) {
    g3dui_setHourGlass ( gui );

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

    g3dui_unsetHourGlass ( gui );

    g3dui_redrawGLViews        ( gui );
    g3dui_updateCoords         ( gui );
    g3dui_redrawObjectList     ( gui );
    g3dui_updateAllCurrentEdit ( gui );
}

/******************************************************************************/
G3DSCENE *common_g3dui_importfileokcbk ( G3DUI *gui, const char *filedesc, 
                                                     const char *filename ) {
    G3DSCENE *sce = gui->sce;

    g3dui_setHourGlass ( gui );
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
            common_g3dui_setFileName ( gui, filename );

            printf ( "...Done!\n" );

	        g3dui_clearMaterials ( gui );
	        g3dui_importMaterials ( gui );

            sce = gui->sce;
        } else {
            gui->sce = g3dscene_new ( 0x00, "Gloss3D scene" );
        }
    }

    g3dui_updateGLViewsMenu    ( gui );
    g3dui_redrawGLViews        ( gui );
    g3dui_updateCoords         ( gui );
    g3dui_redrawObjectList     ( gui );
    g3dui_updateAllCurrentEdit ( gui );

    g3dui_unsetHourGlass ( gui );

    return sce;
}

/******************************************************************************/
void common_g3dui_saveG3DFile ( G3DUI *gui ) {
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
    G3DEXPORTV3EXTENSION *r3dext, *g3duiext;
    LIST *lext = NULL;

    g3dui_setHourGlass ( gui );

    /*
     * we put this here and not in gloss3d initialization because the pointer
     * gui->lrsg is going to heck when we add a new set of render settings.
     */
    r3dext = g3dexportv3extension_new ( SIG_RENDERSETTINGS_EXTENSION,
                                      q3dsettings_write,
                                      gui->lrsg );

    g3duiext = g3dexportv3extension_new ( SIG_G3DUI,
                                          g3dui_write,
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

    g3dui_unsetHourGlass ( gui );
}

/******************************************************************************/
void common_g3dui_setFileName ( G3DUI *gui, const char *filename ) {
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
void common_g3dui_openG3DFile ( G3DUI *gui, const char *filename ) {
    G3DIMPORTV3EXTENSION *r3dext, *g3duiext;
    LIST *lext = NULL;
    LIST *lrsg = NULL;

    g3dui_setHourGlass ( gui );

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

        list_free ( &gui->lrsg, q3dsettings_free );

        /* import render settings module */
        r3dext = g3dimportv3extension_new ( SIG_RENDERSETTINGS_EXTENSION,
                                          q3dsettings_read,
                                         &lrsg );
        /* import G3DUI settings module */
        g3duiext = g3dimportv3extension_new ( SIG_G3DUI,
                                              g3dui_read,
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
                common_g3dui_addRenderSettings ( gui, rsg );
                common_g3dui_useRenderSettings ( gui, rsg );

                /*** that's kind of a global variable, because ***/
                /*** I did not want to change the engine to display ***/
                /*** a background image. But this must be redesigned **/
                sysinfo->backgroundImage = rsg->background.image;
            /*}*/

            ltmprsg = ltmprsg->next;
        }
    } else {
        Q3DSETTINGS *defaultRsg = q3dsettings_new ( );

        common_g3dui_addRenderSettings ( gui, defaultRsg );
        common_g3dui_useRenderSettings ( gui, defaultRsg );
    }

    g3dimportv3extension_free ( g3duiext );
    g3dimportv3extension_free ( r3dext   );

    list_free ( &lext, NULL );
    list_free ( &lrsg, NULL );

    if ( gui->sce ) {
        common_g3dui_setFileName ( gui, filename );

        g3dui_updateGLViewsMenu    ( gui );
        g3dui_clearMaterials ( gui );
        g3dui_redrawGLViews (  gui );
        g3dui_importMaterials ( gui );
        g3dui_redrawObjectList ( gui );

        printf ( "...Done!\n" );
    } else {
        gui->sce = g3dscene_new ( 0x00, "Gloss3D scene" );

        g3dui_updateGLViewsMenu    ( gui );
        g3dui_redrawGLViews        ( gui );
        g3dui_updateCoords         ( gui );
        g3dui_redrawObjectList     ( gui );
        g3dui_updateAllCurrentEdit ( gui );
    }

    g3dui_unsetHourGlass ( gui );
}

/******************************************************************************/
G3DSCENE *common_g3dui_mergeG3DFile ( G3DUI *gui, const char *filename ) {
    LIST *limportExtensions = NULL;
    G3DIMPORTV3EXTENSION *r3dext;

    /* import render settings module */
    r3dext = g3dimportv3extension_new ( SIG_RENDERSETTINGS_EXTENSION,
                                      q3dsettings_read,
                                     &gui->lrsg );

    list_insert ( &limportExtensions, r3dext );

#ifdef __linux__
    if ( access( filename, F_OK ) == 0x00 ) {
#endif
#ifdef __MINGW32__
    if ( PathFileExists ( filename ) ) {
#endif
        printf ( "Opening %s ...\n", filename );

        g3durmanager_clear ( gui->urm );

        g3dui_setHourGlass ( gui );

        g3dscene_importv3 ( filename, gui->sce, limportExtensions, gui->engine_flags );

        g3dui_unsetHourGlass ( gui );

        g3dui_clearMaterials ( gui );
        g3dui_redrawGLViews (  gui );
        g3dui_importMaterials ( gui );
        g3dui_redrawObjectList ( gui );

        printf ( "...Done!\n" );
    }

    g3dimportv3extension_free ( r3dext );

    list_free ( &limportExtensions, NULL );

    return NULL;
}

/******************************************************************************/
void common_g3dui_dispatchGLMenuButton ( G3DUI *gui, G3DMOUSETOOL *mou, 
                                                     uint32_t tool_flags ) {
    uint32_t vertexModeSplineFlags  = ( VERTEXMODETOOL | SPLINETOOL  ),
             vertexModeMeshFlags    = ( VERTEXMODETOOL | MESHTOOL    ),
             edgeModeMeshFlags      = ( EDGEMODETOOL   | MESHTOOL    ),
             faceModeMeshFlags      = ( FACEMODETOOL   | MESHTOOL    ),
             sculptModeMeshFlags    = ( SCULPTMODETOOL | MESHTOOL    ),
             vertexModeMorpherFlags = ( VERTEXMODETOOL | MORPHERTOOL );


    if ( tool_flags & ( OBJECTMODETOOL ) ) {
        addMenuListButton ( gui, gui->lObjectModeMenu, mou );
    }

    if ( ( tool_flags & vertexModeMeshFlags ) == vertexModeMeshFlags ) {
        addMenuListButton ( gui, gui->lVertexModeMeshMenu, mou );
    }

    if ( ( tool_flags & edgeModeMeshFlags ) == edgeModeMeshFlags ) {
        addMenuListButton ( gui, gui->lEdgeModeMeshMenu  , mou );
    }

    if ( ( tool_flags & faceModeMeshFlags ) == faceModeMeshFlags ) {
        addMenuListButton ( gui, gui->lFaceModeMeshMenu  , mou );
    }

    if ( ( tool_flags & sculptModeMeshFlags ) == sculptModeMeshFlags ) {
        addMenuListButton ( gui, gui->lSculptModeMeshMenu, mou );
    }

    if ( ( tool_flags & vertexModeSplineFlags ) == vertexModeSplineFlags ) {
        addMenuListButton ( gui, gui->lVertexModeSplineMenu, mou );
    }

    if ( ( tool_flags & vertexModeMorpherFlags ) == vertexModeMorpherFlags ) {
        addMenuListButton ( gui, gui->lVertexModeMorpherMenu, mou );
    }
}

/******************************************************************************/
void common_g3dui_setMouseTool ( G3DUI        *gui, 
                                 G3DCAMERA    *cam, 
                                 G3DMOUSETOOL *mou ) {
    /*** Call the mouse tool initialization function once. This ***/
    /*** can be used by this function to initialize some values ***/
    if ( mou ) {
        if ( mou->init ) {
            uint32_t msk = mou->init ( mou, gui->sce, 
                                            cam, 
                                            gui->urm, gui->engine_flags );

            common_g3dui_interpretMouseToolReturnFlags ( gui, msk );
        }

        if ( ( mou->flags & MOUSETOOLNOCURRENT ) == 0x00 ) {
            gui->mou = mou;
        }
    } else {
        gui->mou = NULL;
    }
}

/******************************************************************************/
void common_g3dui_interpretMouseToolReturnFlags ( G3DUI *gui, uint32_t msk ) {
    if ( msk & REDRAWVIEW ) {
        g3dui_redrawGLViews ( gui );
    }

    if ( msk & REDRAWMATERIALLIST ) {
        g3dui_redrawMaterialList ( gui );
    }

    if ( msk & REBUILDMATERIALLIST ) {
        g3dui_clearMaterials ( gui );
        g3dui_importMaterials ( gui );
        g3dui_redrawMaterialList ( gui );
    }

    if ( msk & REDRAWUVMAPEDITOR ) {
        g3dui_redrawUVMapEditors ( gui );
    }

    if ( msk & REDRAWLIST ) {
        g3dui_redrawObjectList ( gui );

        g3dui_updateMenuBar ( gui );
    }

    if ( msk & REDRAWCURRENTOBJECT ) {
        g3dui_updateAllCurrentEdit ( gui );
    }

    if ( msk & REDRAWTIMELINE ) {
        g3dui_redrawTimeline ( gui );
    }

    if ( msk & REDRAWCOORDS ) {
        g3dui_updateCoords ( gui );
    }

    if ( msk & NOBUFFEREDSUBDIVISION ) {
        /*** this should be replace by some MEANINGFUL mask ***/
        gui->engine_flags |= ONGOINGANIMATION;
    }

    if ( msk & BUFFEREDSUBDIVISIONOK ) {
        /*** this should be replace by some MEANINGFUL mask ***/
        gui->engine_flags &= (~ONGOINGANIMATION);
    }
}

/******************************************************************************/
G3DMOUSETOOL *common_g3dui_getMouseTool ( G3DUI *gui, const char *name ) {
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
void common_g3dui_addMouseTool ( G3DUI *gui, G3DMOUSETOOL *mou,
                                             uint32_t tool_flags ) {
    if ( tool_flags & GLMENUTOOL ) {
        common_g3dui_dispatchGLMenuButton ( gui, mou, tool_flags );
    }

    list_insert ( &gui->lmou, mou );
}


/******************************************************************************/
void common_g3dui_initDefaultMouseTools ( G3DUI *gui, G3DCAMERA *cam ) {
    G3DMOUSETOOL *mou;

    /********************************/

    mou = ( G3DMOUSETOOL * ) g3dmousetoolpick_new ( );

    common_g3dui_addMouseTool ( gui, mou, 0x00 );

    /*** Pick is the default mouse tool ***/
    common_g3dui_setMouseTool ( gui, cam, mou );

    /********************************/

    common_g3dui_addMouseTool ( gui, 
                                g3dmousetoolcreatesphere_new ( ),
                                OBJECTMODETOOL | 
                                GLMENUTOOL );

    /********************************/

    common_g3dui_addMouseTool ( gui, 
                                g3dmousetoolcreatecube_new ( ),
                                OBJECTMODETOOL | 
                                GLMENUTOOL );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                g3dmousetoolcreateplane_new ( ),
                                OBJECTMODETOOL | 
                                GLMENUTOOL );

    /********************************/

    common_g3dui_addMouseTool ( gui, 
                                g3dmousetoolcreatecylinder_new ( ),
                                OBJECTMODETOOL | 
                                GLMENUTOOL );

    /********************************/

    common_g3dui_addMouseTool ( gui, 
                                g3dmousetoolcreatetube_new ( ),
                                OBJECTMODETOOL | 
                                GLMENUTOOL );

    /********************************/

    common_g3dui_addMouseTool ( gui, 
                                g3dmousetoolcreatetorus_new ( ),
                                OBJECTMODETOOL | 
                                GLMENUTOOL );

    /********************************/

    common_g3dui_addMouseTool ( gui, 
                                g3dmousetoolcreatebone_new ( ),
                                OBJECTMODETOOL | 
                                GLMENUTOOL );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                g3dmousetoolmakeeditable_new ( ),
                                OBJECTMODETOOL |
                                GLMENUTOOL );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                g3dmousetoolcutmesh_new ( ),
                                VERTEXMODETOOL | 
                                FACEMODETOOL   | 
                                EDGEMODETOOL   |
                                MESHTOOL       |
                                SPLINETOOL     |
                                GLMENUTOOL );

    /********************************/

    common_g3dui_addMouseTool ( gui, 
                                g3dmousetoolcreatevertex_new ( ),
                                VERTEXMODETOOL |
                                MESHTOOL       |
                                SPLINETOOL     |
                                GLMENUTOOL );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                g3dmousetoolrevertspline_new ( ),
                                VERTEXMODETOOL |
                                SPLINETOOL     |
                                GLMENUTOOL );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                g3dmousetoolremovevertexpose_new ( ),
                                VERTEXMODETOOL |
                                MORPHERTOOL     |
                                GLMENUTOOL );

    /********************************/

    common_g3dui_addMouseTool ( gui, 
                                g3dmousetoolbridge_new ( ), 
                                VERTEXMODETOOL |
                                MESHTOOL       |
                                SPLINETOOL     |
                                GLMENUTOOL );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                g3dmousetoolextrudeface_new ( ), 
                                FACEMODETOOL | 
                                MESHTOOL     |
                                GLMENUTOOL );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                g3dmousetoolextrudeinner_new ( ), 
                                FACEMODETOOL | 
                                MESHTOOL     |
                                GLMENUTOOL );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                g3dmousetooluntriangulate_new ( ),
                                FACEMODETOOL |
                                MESHTOOL     | 
                                GLMENUTOOL );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                g3dmousetooltriangulate_new ( ),
                                FACEMODETOOL |
                                MESHTOOL     | 
                                GLMENUTOOL );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                g3dmousetoolroundsplinepoint_new ( ),
                                VERTEXMODETOOL |
                                SPLINETOOL     | 
                                GLMENUTOOL );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                g3dmousetoolweldvertices_new ( ),
                                VERTEXMODETOOL |
                                MESHTOOL     | 
                                GLMENUTOOL );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                g3dmousetoolweldneighbours_new ( ),
                                VERTEXMODETOOL |
                                MESHTOOL     | 
                                GLMENUTOOL );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                g3dmousetoolinvertnormal_new ( ),
                                FACEMODETOOL |
                                MESHTOOL     |
                                GLMENUTOOL );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                g3dmousetoolcreatefacegroup_new ( ), 
                                FACEMODETOOL | 
                                MESHTOOL     |
                                GLMENUTOOL );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                g3dmousetoolmove_new ( ), 
                                0x00 );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                g3dmousetoolscale_new ( ), 
                                0x00 );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                g3dmousetoolrotate_new ( ), 
                                0x00 );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                g3dmousetoolpickUV_new ( ), 
                                0x00 );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                g3dmousetoolmoveUV_new ( ),
                                0x00 );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                g3dmousetoolscaleUV_new ( ), 
                                0x00 );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                g3dmousetoolrotateUV_new ( ), 
                                0x00 );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                m3dmousetoolpen_new ( ), 
                                0x00 );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                m3dmousetooleraser_new ( ), 
                                0x00 );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                m3dmousetoolselect_new ( ), 
                                0x00 );

    /********************************/

    common_g3dui_addMouseTool ( gui,
                                m3dmousetoolbucket_new ( ),
                                0x00 );
}

/******************************************************************************/
void common_g3dui_resetDefaultCameras ( G3DUI *gui ) {
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
void common_g3dui_createDefaultCameras ( G3DUI *gui ) {
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

    common_g3dui_resetDefaultCameras ( gui );

    sysinfo->defaultCamera = gui->defaultCameras[0x00];
}

/******************************************************************************/
void common_g3dui_resizeWidget ( G3DUI *gui, uint32_t width, 
                                             uint32_t height ) {
    /*** Menu ***/
    gui->menurec.x      = 0x00;
    gui->menurec.y      = 0x00;
    gui->menurec.width  = width;
    gui->menurec.height = 0x20;

    /*** Tool Bar ***/
    gui->tbarrec.x      = 0x00;
    gui->tbarrec.y      = 0x20;
    gui->tbarrec.width  = width;
    gui->tbarrec.height = TOOLBARBUTTONSIZE;

    /*** Mode Bar ***/
    gui->mbarrec.x      = 0x00;
    gui->mbarrec.y      = gui->tbarrec.y + gui->tbarrec.height;
    gui->mbarrec.width  = 0x30;
    gui->mbarrec.height = height - gui->menurec.height - gui->tbarrec.height;

    /*** Upper Right Panel ***/
    gui->mbrdrec.x      = width  - 0x140;
    gui->mbrdrec.y      = gui->tbarrec.y + gui->tbarrec.height;
    gui->mbrdrec.width  = 0x140;
    gui->mbrdrec.height = height - gui->menurec.height - gui->tbarrec.height;

    /*** Quad View Panel ***/
    gui->quadrec.x      = 0x30;
    gui->quadrec.y      = gui->tbarrec.y + gui->tbarrec.height;
    gui->quadrec.width  = width  - gui->mbarrec.width  - gui->mbrdrec.width;
    gui->quadrec.height = gui->mbarrec.height - 0x2A;

    /*** Timeboard Panel ***/
    gui->timerec.x      = 0x30;
    gui->timerec.y      = gui->quadrec.y + gui->quadrec.height;
    gui->timerec.width  = gui->quadrec.width;
    gui->timerec.height = 0x18;

    /*** Information Panel ***/
    gui->inforec.x      = 0x30;
    gui->inforec.y      = gui->timerec.y + gui->timerec.height;
    gui->inforec.width  = gui->quadrec.width;
    gui->inforec.height = 0x12;
}

/******************************************************************************/
void common_g3dui_closeScene ( G3DUI *gui ) {
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
    Q3DSETTINGS *rsg;

    /* reset background image and system values */
    g3dsysinfo_reset ( sysinfo );

    if ( gui->sce ) g3dobject_free ( gui->sce );

    gui->sce = NULL;

    if ( gui->lrsg ) list_free ( &gui->lrsg, q3dsettings_free );

    rsg = q3dsettings_new ( );

    common_g3dui_addRenderSettings ( gui, rsg );
    common_g3dui_useRenderSettings ( gui, rsg );

    common_g3dui_resetDefaultCameras ( gui );
}
