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

typedef struct _FILTERTOSTATUSBAR {
    GtkWidget *widget;
    float      lastFrame;
    uint32_t   done;
} FILTERTOSTATUSBAR;

/******************************************************************************/
uint32_t q3dfilter_toStatusBar_getStatus ( Q3DFILTER *fil ) {
    FILTERTOSTATUSBAR *tsb = ( FILTERTOSTATUSBAR * ) fil->data;

    return tsb->done;
}

/******************************************************************************/
static uint32_t filtertostatusbar_draw ( Q3DFILTER     *fil, 
                                  Q3DJOB        *qjob,
                                  uint32_t       cpuID, 
                                  float          frameID,
                                  unsigned char *img, 
                                  uint32_t       from, 
                                  uint32_t       to, 
                                  uint32_t       depth, 
                                  uint32_t       width ) {
    FILTERTOSTATUSBAR *tsb = ( FILTERTOSTATUSBAR * ) fil->data;
    guint cont = gtk_statusbar_get_context_id ( tsb->widget, "context" );
    static char str[100];

    /*** When called from "filter render before" event ***/
    if ( ( from == 0x00 ) &&
         ( to   == 0x00 ) ) {
        snprintf ( str, 100, "Rendering frame %.2f (%.2f%%)", frameID,
                                                              ( frameID - qjob->qrsg->output.startframe ) / ( qjob->qrsg->output.endframe - qjob->qrsg->output.startframe ) * 100.0f );
    } else {
        /*** when called from "filter render image" event ***/
        if ( ( int ) tsb->lastFrame == ( int ) frameID ) {
            snprintf ( str, 100, "Done (100%%)" );

            tsb->done = 0xFFFFFFFF;
        }
    }

    gtk_statusbar_push ( tsb->widget, cont, str );

    return 0x00;
}

/******************************************************************************/
static void filtertostatusbar_free ( Q3DFILTER *fil ) {
    FILTERTOSTATUSBAR *tsb = ( FILTERTOSTATUSBAR * ) fil->data;

    free ( tsb );
}

/******************************************************************************/
/*** This filter is declared in the g3dui layer because of GtkWidget struct***/
Q3DFILTER *q3dfilter_toStatusBar_new ( GtkWidget *widget, 
                                       float      lastFrame ) {
    FILTERTOSTATUSBAR *tsb = calloc ( 0x01, sizeof ( FILTERTOSTATUSBAR ) );

    Q3DFILTER *fil = q3dfilter_new ( FILTERBEFORE | FILTERIMAGE,
                                     TOSTATUSBARFILTERNAME,
                                     filtertostatusbar_draw,
                                     filtertostatusbar_free, 
                                     tsb );

    tsb->widget = widget;
    tsb->lastFrame = lastFrame;

    return fil;
}
