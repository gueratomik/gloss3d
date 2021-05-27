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
#include <g3dui_gtk3.h>

/******************************************************************************/
static void gotoframe ( G3DUI *gui, GOTOFRAME *gtf ) {
    /*** Force disabling real time subdivision ***/
    /*** otherwise g3dobject_anim_r() may call some buffered subdivision ***/
    /*** updates and that;s gonna be really slow. We prefer to rely ***/
    /*** On real time stuff ***/
    gui->engine_flags |= ONGOINGANIMATION;

    g3dobject_anim_r ( ( G3DOBJECT * ) gui->sce, gtf->frame, gui->engine_flags );

    g3dui_redrawGLViews  ( gui );
    g3dui_redrawTimeline ( gui );

    gui->curframe = gtf->frame;

    common_g3dui_processAnimatedImages ( gui );

    /*** Re-enable real time subdivision ***/
    gui->engine_flags &= (~ONGOINGANIMATION);
}

/******************************************************************************/
static void dumpscreen ( G3DUI *gui, DUMPSCREEN *dsn ) {
    glReadPixels ( dsn->x, 
                   dsn->y, 
                   dsn->width,
                   dsn->height, 
                   GL_RGBA, GL_UNSIGNED_BYTE,
                   dsn->buffer );
}

/****************** Event handler for interprocess communication **************/
void g3duicom_handleAction ( GtkWidget *widget, gpointer ptr, 
                                                gpointer user_data ) {
    G3DUIACTION *action = ( G3DUIACTION * ) ptr;
    G3DUI *gui = ( G3DUI * ) user_data;

    switch ( action->type ) {
        case ACTION_GOTOFRAME :
            gotoframe ( gui, action );
        break;

        case ACTION_DUMPSCREEN :
            dumpscreen ( gui, action );
        break;

        default:
        break;
    }

    pthread_mutex_unlock ( &action->done );
}

/******************************************************************************/
static gboolean emitAction ( G3DUIACTION *action ) {
    G3DUI *gui = action->gui;
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;

    g_signal_emit_by_name ( ggt->main, "action", action );

    return G_SOURCE_REMOVE;
}

/******************************************************************************/
void g3duicom_requestActionFromMainThread ( G3DUI *gui, G3DUIACTION *action ) {
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;

    pthread_mutex_lock(&action->done);

    /*** Because OpenGL only works within the main context, we ***/
    /*** cannot call g_signal_emit_by_name() directly, we have to ***/
    /*** call g_main_context_invoke_full() that calls a wrapper to ***/
    /*** g_signal_emit_by_name() ***/
    g_main_context_invoke_full ( NULL,
                                 G_PRIORITY_HIGH, 
                    (GSourceFunc)emitAction,
                                 action,
                                 NULL );

    /** wait until completion **/
    pthread_mutex_lock    ( &action->done );
    pthread_mutex_unlock  ( &action->done );
}

/******************************************************************************/
uint32_t filtergotoframe_draw ( Q3DFILTER *fil, Q3DJOB *qjob,
                                                float frameID,
                                                unsigned char *img, 
                                                uint32_t from, 
                                                uint32_t to, 
                                                uint32_t depth, 
                                                uint32_t width ) {
    G3DUI *gui = ( G3DUI * ) fil->data;
    static GOTOFRAME gtf = { .action = { .done = PTHREAD_MUTEX_INITIALIZER } };

    /*** jump to the next frame (this is a image filter, ran on image rendering completion) ***/
    /*gtf.action.wait = 0x00;*/
    gtf.action.type = ACTION_GOTOFRAME;
    gtf.action.gui  = gui;
    gtf.frame       = frameID + 1;

    g3duicom_requestActionFromMainThread ( gui, &gtf );

    return 0x00;
}

/******************************************************************************/
Q3DFILTER *q3dfilter_gotoframe_new ( G3DUI* gui ) {
    Q3DFILTER *fil;

    fil = q3dfilter_new ( FILTERIMAGE, GOTOFRAMEFILTERNAME,
                                       filtergotoframe_draw,
                                       NULL,
                                       gui );

    return fil;
}
