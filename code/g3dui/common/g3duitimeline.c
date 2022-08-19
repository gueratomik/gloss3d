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
uint64_t g3duitimeline_scaleSelectedKeysCbk ( G3DUITIMELINE *tim, 
                                              float  factor, 
                                              float  reference ) {
    G3DUI *gui = tim->gui;

/*    g3durm_objectList_removeSelectedKeys ( gui->urm, 
                                           gui->sce->lsel, 
                                           gui->engine_flags,
                                           REDRAWTIMELINE | REDRAWVIEW );

*/

    g3durm_objectList_scaleSelectedKeys ( gui->urm,
                                          gui->sce->lsel,
                                          factor,
                                          reference,
                                          gui->engine_flags,
                                          REDRAWTIMELINE | 
                                          REDRAWVIEW );


    return REDRAWVIEW | REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3duitimeline_deleteSelectedKeysCbk ( G3DUITIMELINE *tim ) {
    G3DUI *gui = tim->gui;

    g3durm_objectList_removeSelectedKeys ( gui->urm, 
                                           gui->sce->lsel, 
                                           gui->engine_flags,
                                           REDRAWTIMELINE | REDRAWVIEW );


    return REDRAWVIEW | REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3duitimeline_selectAllKeysCbk ( G3DUITIMELINE *tim ) {
    G3DUI *gui = tim->gui;
    uint32_t selected = 0x00; /*** No key selected yet ***/
    LIST *ltmpobj = gui->sce->lsel;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

        g3dobject_selectAllKeys ( obj );

        ltmpobj = ltmpobj->next;
    }

    return REDRAWTIMELINE;
}

/******************************************************************************/
uint32_t g3duitimeline_selectKey ( G3DUITIMELINE *tim,
                                   int           frame,
                                   int           keep,
                                   int           range,
                                   int           width ) {
    G3DUI *gui = tim->gui;
    uint32_t selected = 0x00; /*** No key selected yet ***/
    LIST *ltmpobj = gui->sce->lsel;
    static float firstFrame = FLT_MIN;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;
        G3DKEY *key;

        if ( keep == 0x00 ) {
            g3dobject_unselectAllKeys ( obj );
        }

        if ( range ) {
            float lastFrame = frame;

            if ( firstFrame != FLT_MIN ) {
                if ( firstFrame < lastFrame ) {
                    g3dobject_selectKeyRange ( obj, firstFrame, lastFrame  );
                } else {
                    g3dobject_selectKeyRange ( obj, lastFrame , firstFrame );
                }
            }
        } else {
            key = g3dobject_getKeyByFrame ( obj, frame );

            if ( key ) {
                g3dobject_selectKey ( obj, key );

                selected = 0x01;
            }

            firstFrame = frame;
        }

        ltmpobj = ltmpobj->next;
    }

    /*g3dui_redrawTimeline ( gui );*/

    return selected;
}

/******************************************************************************/
uint32_t g3duitimeline_isOnKey ( G3DUITIMELINE *tim, 
                                 int            frame ) {
    G3DUI *gui = tim->gui;
    LIST *ltmpobj = gui->sce->lsel;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;
        G3DKEY *key;

        key = g3dobject_getKeyByFrame ( obj, frame );

        if ( key ) {
            return 0x01;
        }

        ltmpobj = ltmpobj->next;
    }

    return 0x00;
}

/******************************************************************************/
int32_t g3duitimeline_getFramePos ( G3DUITIMELINE *tim, 
                                    float          frame, 
                                    int            width ) {
    int32_t midx     = ( width >> 0x01 ),
            difframe = ( tim->midframe - frame ),
            framepos = midx - ( difframe * tim->nbpix );

    return framepos;
}

/******************************************************************************/
int32_t g3duitimeline_getFrame ( G3DUITIMELINE *tim,
                                 int            x, 
                                 int            y, 
                                 int            width ) {
    int32_t midx  = ( width >> 0x01 ),
            difx  = ( x - midx ),
            /*** let's go to the nearest frame ***/
            nbfrm = ( int32_t ) roundf ( ( float ) difx / tim->nbpix );

    return ( tim->midframe + nbfrm );
}

/******************************************************************************/
/*** Function = Did we click on the timeline cursor  ?                      ***/
/*** x, y = mouse coord                                                     ***/
/*** width = width of the widget                                            ***/
/******************************************************************************/
uint32_t g3duitimeline_onFrame ( G3DUITIMELINE *tim,
                                 float          curframe,
                                 int            x, 
                                 int            y, 
                                 int            width ) {
    int32_t onframe = g3duitimeline_getFrame ( tim, x, y, width );

    if ( curframe == onframe ) {

        return 0x01;
    }

    return 0x00;
}

/******************************************************************************/
void g3duitimeline_init ( G3DUITIMELINE *tim ) {
    tim->nbpix = DEFAULTFRAMEGAP;
}
