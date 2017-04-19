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
#include <g3dui.h>

/******************************************************************************/
#include <xpm/sphere.xpm>
#include <xpm/cube.xpm>
#include <xpm/cylinder.xpm>
#include <xpm/knife.xpm>
#include <xpm/addvertex.xpm>
#include <xpm/bridge.xpm>
#include <xpm/extrude.xpm>

/******************************************************************************/
G3DUIRENDERPROCESS *common_g3dui_render ( G3DUI *gui, uint64_t id,
                                                      uint32_t x1, uint32_t y1,
                                                      uint32_t x2, uint32_t y2,
                                                      uint32_t width, uint32_t height,
                                                      LIST *lfilters,
                                                      uint32_t sequence ) {

    G3DUIRENDERSETTINGS *rsg = ( G3DUIRENDERSETTINGS * ) gui->currsg;
    G3DCAMERA *cam = gui->curcam;
    G3DSCENE *sce = gui->sce;

    /*** Don't start a new render before the current one has finished ***/
    /*if ( rpc == NULL ) {*/
        G3DUIRENDERPROCESS *rps;
        R3DSCENE *rsce  = NULL;
        #ifdef __linux__
        pthread_attr_t attr;
        pthread_t tid;

        /*r3dfilter_setType ( towin, FILTERLINE );*/

        pthread_attr_init ( &attr );

        /*** start thread son all CPUs ***/
        pthread_attr_setscope ( &attr, PTHREAD_SCOPE_SYSTEM );
        #endif

        rsce = r3dscene_new ( sce, cam, x1, y1,
                                        x2, y2,
                                        width, height,
                                        rsg->background,
                                        rsg->startframe,
                                        rsg->endframe,
                                        rsg->flags & RENDERWIREFRAME,
                                        rsg->flags & WIREFRAMELIGHTING,
                                        rsg->wireframeColor,
                                        rsg->wireframeThickness,
                                        lfilters );

        /*** launch rays in a thread ***/
        if ( sequence ) {
            #ifdef __linux__
            pthread_create ( &rsce->tid, &attr, (void*(*)(void*))r3dscene_render_sequence_t, rsce );
            #endif

            #ifdef __MINGW32__
            rsce->tid = CreateThread ( NULL, 
                                       0,
                                       (LPTHREAD_START_ROUTINE) r3dscene_render_sequence_t, 
                                       rsce,
                                       0,
                                       NULL );
            #endif
        } else {
            #ifdef __linux__
            pthread_create ( &rsce->tid, &attr, (void*(*)(void*))r3dscene_render_frame_t, rsce );
            #endif

            #ifdef __MINGW32__
            rsce->tid = CreateThread ( NULL, 
                                       0,
                                       (LPTHREAD_START_ROUTINE) r3dscene_render_frame_t, 
                                       rsce,
                                       0,
                                       NULL );
            #endif
        }

        /*** Remember the thread id for cancelling on mouse input e.g ***/
        /*** We use the widget as an ID ***/
        rps = g3duirenderprocess_new ( id, rsce, NULL/*towin*/, NULL/*tobuf*/ );

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
    if ( gui->flags & VIEWOBJECT ) {
        g3dscene_unselectAllObjects ( sce, gui->flags );
        g3dscene_selectAllObjects   ( sce, gui->flags );
    }

    /*** select all vertices ***/
    if ( gui->flags & VIEWVERTEX ) {
        G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

        if ( obj && obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;

            g3dmesh_unselectAllVertices ( mes );
            g3dmesh_selectAllVertices   ( mes );
        }
    }

    /*** select all edges ***/
    if ( gui->flags & VIEWEDGE ) {
        G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

        if ( obj && obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;

            g3dmesh_unselectAllEdges ( mes );
            g3dmesh_selectAllEdges   ( mes );
        }
    }

    /*** select all vertices ***/
    if ( gui->flags & VIEWFACE ) {
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
G3DUIRENDERPROCESS *common_g3dui_getRenderProcessByScene ( G3DUI *gui, R3DSCENE *rsce ) {
    LIST *ltmprps = gui->lrps;

    while ( ltmprps ) {
        G3DUIRENDERPROCESS *rps = ( G3DUIRENDERPROCESS * ) ltmprps->data;

        if ( rps->rsce == rsce ) {

            return rps;
        }

        ltmprps = ltmprps->next;
    }

    return NULL;
}

/******************************************************************************/
uint32_t common_g3dui_cancelRenderByScene ( G3DUI *gui, R3DSCENE *rsce ) {
    G3DUIRENDERPROCESS *rps = common_g3dui_getRenderProcessByScene ( gui, rsce );

    if ( rps ) {
        r3dscene_cancelRender ( rps->rsce );

        return 0x01;
    }

    return 0x00;
}

/******************************************************************************/
uint32_t common_g3dui_cancelRenderByID ( G3DUI *gui, uint64_t id ) {
    G3DUIRENDERPROCESS *rps = common_g3dui_getRenderProcessByID ( gui, id );

    if ( rps ) {
        r3dscene_cancelRender ( rps->rsce );

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
        g3dscene_exportPov ( gui->sce, filename, "/*Made with Gloss3D*/\n\n", 0 );
    }

    if ( strcmp ( filedesc, FILEDESC_OBJ ) == 0x00 ) {
        g3dscene_exportObj ( gui->sce, filename, "#Made with Gloss3D\n\n", 0 );
    }

    if ( strcmp ( filedesc, FILEDESC_STA ) == 0x00 ) {
        g3dscene_exportStlA ( gui->sce, filename, "#Made with Gloss3D\n\n", 0 );
    }

    g3dui_unsetHourGlass ( gui );

    g3dui_redrawGLViews        ( gui );
    g3dui_updateCoords         ( gui );
    g3dui_redrawObjectList     ( gui );
    g3dui_updateAllCurrentEdit ( gui );
}

/******************************************************************************/
G3DSCENE *common_g3dui_importfileokcbk ( G3DUI *gui, const char *filedesc, 
                                                     const char *filename ) {
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
            gui->sce = g3dscene_import3ds ( filename, gui->flags );
        }

        if ( strcmp ( filedesc, FILEDESC_OBJ ) == 0x00 ) {
            gui->sce = g3dscene_importObj ( filename, gui->flags );
        }
#ifdef HAVE_EXPAT_H
        if ( strcmp ( filedesc, FILEDESC_DAE ) == 0x00 ) {
            gui->sce = g3dscene_importCollada ( filename, gui->flags );
        }
#endif
#ifdef HAVE_C4D_H
        if ( strcmp ( filedesc, FILEDESC_C4D ) == 0x00 ) {
            gui->sce = g3dscene_importC4D ( filename, gui->flags );
        }
#endif

        if ( gui->sce ) {
            common_g3dui_setFileName ( gui, filename );

            printf ( "...Done!\n" );

            g3dui_unsetHourGlass ( gui );

            return gui->sce;
        } else {
            g3dui_unsetHourGlass ( gui );

            return g3dscene_new ( 0x00, "Gloss3D scene" );
        }
    }

    g3dui_redrawGLViews        ( gui );
    g3dui_updateCoords         ( gui );
    g3dui_redrawObjectList     ( gui );
    g3dui_updateAllCurrentEdit ( gui );
}

/******************************************************************************/
void common_g3dui_saveG3DFile ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;

    g3dscene_write ( sce, gui->filename, "Made with GLOSS-3D", 0x00 );
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
G3DSCENE *common_g3dui_openG3DFile ( G3DUI *gui, const char *filename ) {
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

        g3dui_setHourGlass ( gui );

        gui->sce = g3dscene_open ( filename, gui->flags );

        g3dui_unsetHourGlass ( gui );

        if ( gui->sce ) {
            common_g3dui_setFileName ( gui, filename );

            g3dui_clearMaterials ( gui );
            g3dui_redrawGLViews (  gui );
            g3dui_importMaterials ( gui );
            g3dui_redrawObjectList ( gui );

            printf ( "...Done!\n" );

            return gui->sce;
        } else {
            return g3dscene_new ( 0x00, "Gloss3D scene" );
        }
    }

    return NULL;
}

/******************************************************************************/
void common_g3dui_dispatchGLMenuButton ( G3DUI *gui, G3DMOUSETOOL *mou, 
                                                     uint32_t tool_flags ) {
    LIST *lobjmenu = gui->lobjmenu,
         *lvermenu = gui->lvermenu,
         *ledgmenu = gui->ledgmenu,
         *lfacmenu = gui->lfacmenu,
         *lscpmenu = gui->lscpmenu;

    if ( tool_flags & OBJECTMODETOOL ) addMenuListButton ( gui, lobjmenu, mou );
    if ( tool_flags & VERTEXMODETOOL ) addMenuListButton ( gui, lvermenu, mou );
    if ( tool_flags & EDGEMODETOOL   ) addMenuListButton ( gui, ledgmenu, mou );
    if ( tool_flags & FACEMODETOOL   ) addMenuListButton ( gui, lfacmenu, mou );
    if ( tool_flags & SCULPTMODETOOL ) addMenuListButton ( gui, lscpmenu, mou );
}

/******************************************************************************/
void common_g3dui_setMouseTool ( G3DUI *gui, G3DMOUSETOOL *mou ) {
    /*** Call the mouse tool initialization function once. This ***/
    /*** can be used by this function to initialize some values ***/
    if ( mou->init ) {
        uint32_t msk = mou->init ( mou, gui->sce, 
                                        gui->curcam, 
                                        gui->urm, gui->flags );
			
        common_g3dui_interpretMouseToolReturnFlags ( gui, msk );
    }

    if ( ( mou->flags & MOUSETOOLNOCURRENT ) == 0x00 ) {
        /*** We don't check this variable is non-NULL because it must never be ***/
        gui->mou    = mou;
    }
}

/******************************************************************************/
void common_g3dui_interpretMouseToolReturnFlags ( G3DUI *gui, uint32_t msk ) {
    if ( msk & REDRAWVIEW ) {
        g3dui_redrawGLViews ( gui );
    }

    if ( msk & REDRAWLIST ) {
        g3dui_redrawObjectList ( gui );
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

    if ( msk & REDRAWCOORDS ) {
        g3dui_updateCoords ( gui );
    }

    if ( msk & NOBUFFEREDSUBDIVISION ) {
        /*** this should be replace by some MEANINGFUL mask ***/
        gui->flags |= ONGOINGANIMATION;
    }

    if ( msk & BUFFEREDSUBDIVISIONOK ) {
        /*** this should be replace by some MEANINGFUL mask ***/
        gui->flags &= (~ONGOINGANIMATION);
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
void common_g3dui_initDefaultMouseTools ( G3DUI *gui ) {
    G3DMOUSETOOL *mou;

    /********************************/

    mou = g3dmousetool_new ( PICKTOOL, 's', NULL,
                             pickTool_init, pick_draw, pick_tool, 0x00 );

    common_g3dui_addMouseTool ( gui, mou, 0x00 );

    /*** Pick is the default mouse tool ***/
    common_g3dui_setMouseTool ( gui, mou );

    /********************************/

    mou = g3dmousetool_new ( CREATESPHERETOOL, 'a', sphere_xpm,
                             NULL, NULL, createSphere, 0x00 );

    common_g3dui_addMouseTool ( gui, mou, OBJECTMODETOOL | GLMENUTOOL );

    /********************************/

    mou = g3dmousetool_new ( CREATECUBETOOL, 'a', cube_xpm,
                             NULL, NULL, createCube, 0x00 );

    common_g3dui_addMouseTool ( gui, mou, OBJECTMODETOOL | GLMENUTOOL );

    /********************************/

    mou = g3dmousetool_new ( CREATEPLANETOOL, 'a', sphere_xpm,
                             NULL, NULL, createPlane, 0x00 );

    common_g3dui_addMouseTool ( gui, mou, OBJECTMODETOOL | GLMENUTOOL );

    /********************************/

    mou = g3dmousetool_new ( CREATECYLINDERTOOL, 'a', cylinder_xpm,
                             NULL, NULL, createCylinder, 0x00 );
    common_g3dui_addMouseTool ( gui, mou, OBJECTMODETOOL | GLMENUTOOL );

    /********************************/

    mou = g3dmousetool_new ( CREATETORUSTOOL, 'a', cylinder_xpm,
                             NULL, NULL, createTorus, 0x00 );

    common_g3dui_addMouseTool ( gui, mou, OBJECTMODETOOL | GLMENUTOOL );

    /********************************/

    mou = g3dmousetool_new ( CREATEBONETOOL, 'a', cylinder_xpm,
                             NULL, NULL, createBone, 0x00 );

    common_g3dui_addMouseTool ( gui, mou, OBJECTMODETOOL | GLMENUTOOL );

    /********************************/

    mou = g3dmousetool_new ( CUTMESHTOOL, 'a', knife_xpm,
                             cutMesh_init,
                             cutMesh_draw,
                             cutMesh_tool, 0x00 );

    common_g3dui_addMouseTool ( gui, mou, VERTEXMODETOOL | 
                                   FACEMODETOOL   | 
                                   EDGEMODETOOL   | 
                                   GLMENUTOOL );

    /********************************/

    mou = g3dmousetool_new ( ADDVERTEXTOOL, 'a', addvertex_xpm,
                             createVertex_init, NULL, createVertex, 0x00 );

    common_g3dui_addMouseTool ( gui, mou, VERTEXMODETOOL | GLMENUTOOL );

    /********************************/

    mou = g3dmousetool_new ( BRIDGETOOL, 'a', bridge_xpm,
                             createFace_init,
                             createFace_draw, createFace_tool, 0x00 );

    common_g3dui_addMouseTool ( gui, mou, VERTEXMODETOOL | GLMENUTOOL );

    /********************************/

    mou = g3dmousetool_new ( PAINTWEIGHTTOOL, 'x', extrude_xpm,
                             paintWeight_init,
                             NULL, paintWeight_tool, 0x00 );

    common_g3dui_addMouseTool ( gui, mou, VERTEXMODETOOL );

    /********************************/

    mou = g3dmousetool_new ( SCULPTTOOL, 'x', extrude_xpm,
                             sculptTool_init,
                             NULL, sculpt_tool, 0x00 );

    common_g3dui_addMouseTool ( gui, mou, SCULPTMODETOOL | GLMENUTOOL );

    /********************************/

    /*mou = g3dmousetool_new ( SMOOTHTOOL, 'x', extrude_xpm,
                             smoothTool_init,
                             NULL, smooth_tool, 0x00 );

    common_g3dui_addMouseTool ( gui, mou, SCULPTMODETOOL | GLMENUTOOL );*/

    /********************************/

    mou = g3dmousetool_new ( EXTRUDETOOL, 'x', extrude_xpm,
                             extrudeFace_init,
                             NULL, extrudeFace_tool, 0x00 );

    common_g3dui_addMouseTool ( gui, mou, FACEMODETOOL | GLMENUTOOL );

    /********************************/

    mou = g3dmousetool_new ( EXTRUDINNERTOOL, 'x', extrude_xpm,
                             extrudeInner_init,
                             NULL, extrudeFace_tool, 0x00 );

    common_g3dui_addMouseTool ( gui, mou, FACEMODETOOL | GLMENUTOOL );

    /********************************/

    mou = g3dmousetool_new ( UNTRIANGULATETOOL, 'a', NULL,
                             untriangulate_init,
                             NULL, NULL, MOUSETOOLNOCURRENT );

    common_g3dui_addMouseTool ( gui, mou, FACEMODETOOL | GLMENUTOOL );

    /********************************/

    mou = g3dmousetool_new ( TRIANGULATETOOL, 'a', NULL,
                             triangulate_init,
                             NULL, NULL, MOUSETOOLNOCURRENT );

    common_g3dui_addMouseTool ( gui, mou, FACEMODETOOL | GLMENUTOOL );

    /********************************/

    mou = g3dmousetool_new ( WELDVERTICESTOOL, 'a', NULL,
                             weldvertices_init,
                             NULL, NULL, MOUSETOOLNOCURRENT );

    common_g3dui_addMouseTool ( gui, mou, VERTEXMODETOOL | GLMENUTOOL );

    /********************************/

    mou = g3dmousetool_new ( INVERTNORMALTOOL, 'a', NULL,
                             invertNormal_init,
                             NULL, NULL, MOUSETOOLNOCURRENT );

    common_g3dui_addMouseTool ( gui, mou, FACEMODETOOL | GLMENUTOOL );


    /********************************/

    mou = g3dmousetool_new ( MOVETOOL, 's', NULL,
                             NULL, NULL, move_tool, 0x00 );

    common_g3dui_addMouseTool ( gui, mou, 0x00 );

    /********************************/

    mou = g3dmousetool_new ( SCALETOOL, 's', NULL,
                             NULL, NULL, scale_tool, 0x00 );

    common_g3dui_addMouseTool ( gui, mou, 0x00 );

    /********************************/

    mou = g3dmousetool_new ( ROTATETOOL, 'r', NULL,
                             NULL, NULL, rotate_tool, 0x00 );

    common_g3dui_addMouseTool ( gui, mou, 0x00 );

    /********************************/
}

/******************************************************************************/
/*** Create the 4 defgault cameras ***/
G3DCAMERA **common_g3dui_createDefaultCameras ( G3DUI *gui ) {
    static G3DCAMERA *cam[0x04]; /*** static, so the local variable doesnot die ***/
    void (*camgrid[0x04])(G3DCAMERA *,uint32_t) = { g3dcamera_grid3D,
                                                    g3dcamera_gridXY,
                                                    g3dcamera_gridZX,
                                                    g3dcamera_gridYZ };
    G3DVECTOR campos[0x04] = { {  -4.851479f,   1.710646f,   4.851479f, 1.0f },
                               {        0.0f,        0.0f,       15.0f, 1.0f },
                               {        0.0f,       15.0f,        0.0f, 1.0f },
                               {       15.0f,        0.0f,        0.0f, 1.0f } },
              camrot[0x04] = { { -19.422806f, -43.322395f, -13.599810f, 1.0f },
                               {        0.0f,        0.0f,        0.0f, 1.0f },
                               { -90.000000f,        0.0f,        0.0f, 1.0f },
                               {        0.0f, -90.000000f,        0.0f, 1.0f } },
              camsca[0x04] = { {        1.0f,        1.0f,        1.0f, 1.0f },
                               {        1.0f,        1.0f,        1.0f, 1.0f },
                               {        1.0f,        1.0f,        1.0f, 1.0f },
                               {        1.0f,        1.0f,        1.0f, 1.0f } };
    float camfoc[0x04] = { 45.0f, 2.0f, 2.0f, 2.0f };
    uint32_t i;

    cam[0x00] = g3dcamera_new ( 0x00, "Camera",  0.0f, 1.0f,     0.1f, 1000.0f );
    cam[0x01] = g3dcamera_new ( 0x01, "Camera",  0.0f, 1.0f, -1000.0f, 1000.0f );
    cam[0x02] = g3dcamera_new ( 0x02, "Camera",  0.0f, 1.0f, -1000.0f, 1000.0f );
    cam[0x03] = g3dcamera_new ( 0x03, "Camera",  0.0f, 1.0f, -1000.0f, 1000.0f );

    for ( i = 0x00; i < 0x04; i++ ) {
        G3DOBJECT *objcam = ( G3DOBJECT * ) cam[i];

        cam[i]->focal =  camfoc[i];
        cam[i]->grid  = camgrid[i];

        memcpy ( &objcam->pos, &campos[i], sizeof ( G3DVECTOR ) );
        memcpy ( &objcam->rot, &camrot[i], sizeof ( G3DVECTOR ) );
        memcpy ( &objcam->sca, &camsca[i], sizeof ( G3DVECTOR ) );

        /*** OpenGL is nto initalized at this step, matrix computation wont ***/
        /*** work, this is why the line below is commented. Camera matrix ***/
        /*** is updated in G3DUIVIEW widget (Motif and Gtk)
        /*g3dobject_updateMatrix ( objcam );*/
    }

    return cam;
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
