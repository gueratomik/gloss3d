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
void common_timelinedata_deleteKey ( G3DUI *gui, TIMELINEDATA *tdata ) {
    g3durm_objectList_removeSelectedKeys ( gui->urm, 
                                           gui->sce->lsel, 
                                           gui->engine_flags,
                                           REDRAWTIMELINE | REDRAWVIEW );


    g3dui_redrawTimeline ( gui );
    g3dui_redrawGLViews ( gui );
}

/******************************************************************************/
uint32_t common_timelinedata_selectKey ( G3DUI        *gui, 
                                         TIMELINEDATA *tdata,
                                         int           frame,
                                         int           keep,
                                         int           width ) {
    uint32_t selected = 0x00; /*** No key selected yet ***/
    LIST *ltmpobj = gui->sce->lsel;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;
        G3DKEY *key;

        if ( keep == 0x00 ) {
            g3dobject_unselectAllKeys ( obj );
        }

        key = g3dobject_getKeyByFrame ( obj, frame );

        if ( key ) {
            g3dobject_selectKey ( obj, key );

            selected = 0x01;
        }

        ltmpobj = ltmpobj->next;
    }

    g3dui_redrawTimeline ( gui );

    return selected;
}

/******************************************************************************/
int32_t common_timelinedata_getFramePos ( TIMELINEDATA *tdata, float frame, 
                                                               int width ) {
    int32_t midx     = ( width >> 0x01 ),
            difframe = ( tdata->midframe - frame ),
            framepos = midx - ( difframe * tdata->nbpix );

    return framepos;
}

/******************************************************************************/
int32_t common_timelinedata_getFrame ( TIMELINEDATA *tdata, int x, 
                                                            int y, 
                                                            int width ) {
    int32_t midx  = ( width >> 0x01 ),
            difx  = ( x - midx ),
            /*** let's go to the nearest frame ***/
            nbfrm = ( int32_t ) roundf ( ( float ) difx / tdata->nbpix );

    return ( tdata->midframe + nbfrm );
}

/******************************************************************************/
/*** Function = Did we click on the timeline cursor  ?                      ***/
/*** x, y = mouse coord                                                     ***/
/*** width = width of the widget                                            ***/
/******************************************************************************/
uint32_t common_timelinedata_onFrame ( TIMELINEDATA *tdata, float curframe,
                                                            int x, 
                                                            int y, 
                                                            int width ) {
    int32_t onframe = common_timelinedata_getFrame ( tdata, x, y, width );

    if ( curframe == onframe ) {

        return 0x01;
    }

    return 0x00;
}

/******************************************************************************/
TIMELINEDATA *common_timelinedata_new ( ) {
    uint32_t structsize = sizeof ( TIMELINEDATA );
    TIMELINEDATA *tdata = ( TIMELINEDATA * ) calloc ( 0x01, structsize );

    if ( tdata == NULL ) {
        fprintf ( stderr, "g3duitimelinedata_new: memory allocation failed\n" );

        return NULL;
    }

    tdata->nbpix = DEFAULTFRAMEGAP;


    return tdata;
}
