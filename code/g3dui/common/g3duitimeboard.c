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
void g3duitimeboard_resize ( G3DUITIMEBOARD *tbrd, 
                             uint32_t        width, 
                             uint32_t        height ) {
    tbrd->btnrec.x      = 0;
    tbrd->btnrec.y      = 0;
    tbrd->btnrec.width  = 168;
    tbrd->btnrec.height = 24;

    tbrd->linerec.x      = 174;
    tbrd->linerec.y      = 0;
    tbrd->linerec.width  = width - 150 - 48;
    tbrd->linerec.height = 24;

    tbrd->funcrec.x      = width - 48;
    tbrd->funcrec.y      = 0;
    tbrd->funcrec.width  = 48;
    tbrd->funcrec.height = 24;
}

/******************************************************************************/
uint64_t g3duitimeboard_stop ( G3DUITIMEBOARD *tbrd ) {
    G3DUI *gui = tbrd->gui;

    /*** if scene is currently played ***/
    if ( /*gui->playthreadid*/gui->playLock ) {
        /* #ifdef __linux__
        pthread_join   ( gui->playthreadid, NULL );
        #endif
        #ifdef __MINGW32__
        WaitForSingleObject ( gui->playthreadid, INFINITE );
        CloseHandle ( gui->playthreadid );
        #endif */

        gui->playLock     = 0x00;
        /*gui->playthreadid = 0x00;*/

        gui->engine_flags &= (~ONGOINGANIMATION);

        g3dscene_updateMeshes ( gui->sce, gui->engine_flags );
    }


   return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duitimeboard_gotoFrame ( G3DUITIMEBOARD *tbrd ) {
    G3DUI *gui = tbrd->gui;
    G3DSCENE *sce = gui->sce;

    /*** Force disabling real time subdivision ***/
    gui->engine_flags |= ONGOINGANIMATION;

    g3dobject_anim_r ( ( G3DOBJECT * ) sce, gui->curframe, gui->engine_flags );

    /*** Re-enable real time subdivision ***/
    gui->engine_flags &= (~ONGOINGANIMATION);

    /*** Update buffered subdivided meshes ***/
    g3dscene_updateMeshes ( gui->sce, gui->engine_flags );

    g3dui_processAnimatedImages ( gui );


   return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duitimeboard_recordFrame ( G3DUITIMEBOARD *tbrd,
                                         uint32_t        key_flags ) {
    G3DUI *gui = tbrd->gui;
    G3DSCENE *sce = gui->sce;

    g3durm_object_pose ( gui->urm,
                         sce->lsel,
                         gui->curframe,
                         /*&obj->pos,
                         &obj->rot,
                         &obj->sca,*/
                         key_flags,
                         gui->engine_flags,
                         REDRAWTIMELINE | REDRAWVIEW );

    if ( gui->curframe > gui->endframe   ) gui->endframe   = gui->curframe;
    if ( gui->curframe < gui->startframe ) gui->startframe = gui->curframe;


    return REDRAWVIEW | REDRAWTIMELINE;

    /*g3dui_updateKeyEdit  ( gui );
    g3dui_redrawTimeline ( gui );
    g3dui_redrawGLViews ( gui );*/
}
