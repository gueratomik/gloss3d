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
#include <g3dui_gtk3.h>

/****************** Event handler for interprocess communication **************/
void g3duicom_gotoframe ( GtkWidget *widget, gdouble frame, 
                                             gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    /*printf("%d %d %f\n", widget, gui, frame );*/

/*printf("gui:%lld %lld %f\n", widget, ( long long ) gui, frame );*/
    /*** Force disabling real time subdivision ***/
    /*** otherwise g3dobject_anim_r() may call some buffered subdivision ***/
    /*** updates and that;s gonna be really slow. We prefer to rely ***/
    /*** On real time stuff ***/
    gui->flags |= ONGOINGANIMATION;

    g3dobject_anim_r ( ( G3DOBJECT * ) gui->sce, frame, gui->flags );

    g3dui_redrawGLViews ( gui );

    gui->curframe = frame;

    /*** Re-enable real time subdivision ***/
    gui->flags &= (~ONGOINGANIMATION);
}
