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
#include <g3dui.h>

/******************************************************************************/
void common_g3dui_stopCbk ( G3DUI *gui ) {
    /*** if scene is currently played ***/
    if ( gui->playthreadid ) {
        #ifdef __linux__
        pthread_join   ( gui->playthreadid, NULL );
        #endif
        #ifdef __MINGW32__
        WaitForSingleObject ( gui->playthreadid, INFINITE );
        CloseHandle ( gui->playthreadid );
        #endif

        gui->playLock     = 0x00;
        gui->playthreadid = 0x00;

        gui->flags &= (~ONGOINGANIMATION);

        g3dscene_updateMeshes ( gui->sce, gui->flags );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3dui_gotoFrameCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;

    /*** Force disabling real time subdivision ***/
    gui->flags |= ONGOINGANIMATION;

    g3dobject_anim_r ( ( G3DOBJECT * ) sce, gui->curframe, gui->flags );

    /*** Re-enable real time subdivision ***/
    gui->flags &= (~ONGOINGANIMATION);

    /*** Update buffered subdivided meshes ***/
    g3dscene_updateMeshes ( gui->sce, gui->flags );

    common_g3dui_processAnimatedImages ( gui );

    g3dui_redrawGLViews ( gui );
}

/******************************************************************************/
void common_g3dui_recordFrameCbk ( G3DUI *gui, uint32_t key_flags ) {
    G3DSCENE *sce = gui->sce;
    LIST *ltmpobj = sce->lsel;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

        g3dobject_pose ( obj, gui->curframe, &obj->pos, 
                                             &obj->rot, 
                                             &obj->sca, key_flags );

        printf("Keying at frame %d for %s\n", ( int32_t ) gui->curframe, obj->name );

        if ( gui->curframe > gui->endframe   ) gui->endframe   = gui->curframe;
        if ( gui->curframe < gui->startframe ) gui->startframe = gui->curframe;

        ltmpobj = ltmpobj->next;
    }

    g3dui_updateKeyEdit  ( gui );
    g3dui_redrawTimeline ( gui );
}
