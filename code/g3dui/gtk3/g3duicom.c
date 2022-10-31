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
static void gotoframe ( GTK3G3DUI *gtk3gui,
                        GOTOFRAME *gtf ) {
    G3DUI *gui = &gtk3gui->core;

    /*** Force disabling real time subdivision ***/
    /*** otherwise g3dobject_anim_r() may call some buffered subdivision ***/
    /*** updates and that;s gonna be really slow. We prefer to rely ***/
    /*** On real time stuff ***/
    gui->engine_flags |= ONGOINGANIMATION;
/*printf("g3dgotoframe %s %f\n", __func__, gtf->frame );*/

    g3dobject_anim_r ( ( G3DOBJECT * ) gui->sce, 
                                       gtf->frame, 
                                       gui->engine_flags |
                                       gtf->extra_engine_flags );

    g3dobject_update_r ( ( G3DOBJECT * ) gui->sce, gui->engine_flags |
                                                   gtf->extra_engine_flags );


    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW | REDRAWTIMELINE );

    gui->curframe = gtf->frame;

    g3dui_processAnimatedImages ( gui );

    /*** Re-enable real time subdivision ***/
    gui->engine_flags &= (~ONGOINGANIMATION);
}

/******************************************************************************/
static void dumpscreen ( GTK3G3DUI  *gtk3gui, 
                         DUMPSCREEN *dsn ) {
    G3DUI *gui = &gtk3gui->core;

    glReadPixels ( dsn->x, 
                   dsn->y, 
                   dsn->width,
                   dsn->height, 
                   GL_RGBA, GL_UNSIGNED_BYTE,
                   dsn->buffer );
}

/****************** Event handler for interprocess communication **************/
void gtk3_g3duicom_handleAction ( GtkWidget *widget, 
                                  gpointer   ptr, 
                                  gpointer   user_data ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) user_data;
    G3DUIACTION *action = ( G3DUIACTION * ) ptr;
    G3DUI *gui = &gtk3gui->core;

    switch ( action->type ) {
        case ACTION_GOTOFRAME :
            gotoframe ( gtk3gui, ( GOTOFRAME * ) action );
        break;

        case ACTION_DUMPSCREEN :
            dumpscreen ( gtk3gui, ( DUMPSCREEN * ) action );
        break;

        default:
        break;
    }

    pthread_mutex_unlock ( &action->done );
}

/******************************************************************************/
static gboolean emitAction ( G3DUIACTION *action ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) action->gui;

    g_signal_emit_by_name ( gtk3gui->topWin, "action", action );


    return G_SOURCE_REMOVE;
}

/******************************************************************************/
void gtk3_g3duicom_requestActionFromMainThread ( GTK3G3DUI   *gtk3gui,
                                                 G3DUIACTION *action ) {
    G3DUI *gui = &gtk3gui->core;

    pthread_mutex_lock(&action->done);

    /*** Because OpenGL only works within the main context, we ***/
    /*** cannot call g_signal_emit_by_name() directly, we have to ***/
    /*** call g_main_context_invoke_full() that calls a wrapper to ***/
    /*** g_signal_emit_by_name() ***/
    g_main_context_invoke_full ( NULL,
#ifdef __linux__
                                 G_PRIORITY_HIGH, 
#endif
#ifdef __MINGW32__
                                 G_PRIORITY_DEFAULT,
#endif
                    (GSourceFunc)emitAction,
                                 action,
                                 NULL );

    /** wait until completion **/
    pthread_mutex_lock    ( &action->done );
    pthread_mutex_unlock  ( &action->done );
}

/******************************************************************************/
uint32_t filtergotoframe_draw ( Q3DFILTER     *fil, 
                                Q3DJOB        *qjob,
                                uint32_t       cpuID, 
                                float          frameID,
                                unsigned char *img, 
                                uint32_t       from, 
                                uint32_t       to, 
                                uint32_t       depth, 
                                uint32_t       width ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) fil->data;
    static GOTOFRAME gtf = { .action = { .done = PTHREAD_MUTEX_INITIALIZER } };

    gtf.action.type        =  ACTION_GOTOFRAME;
    gtf.action.gui         = &gtk3gui->core;
    gtf.frame              =  frameID;
    gtf.extra_engine_flags =  ONGOINGRENDERING;

    gtk3_g3duicom_requestActionFromMainThread ( gtk3gui, ( G3DUIACTION * ) &gtf );

    return 0x00;
}

/******************************************************************************/
Q3DFILTER *q3dfilter_gotoframe_new ( GTK3G3DUI *gtk3gui ) {
    Q3DFILTER *fil;

    fil = q3dfilter_new ( FILTERIMAGE, GOTOFRAMEFILTERNAME,
                                       filtergotoframe_draw,
                                       NULL,
                                       gtk3gui );

    return fil;
}
