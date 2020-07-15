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
#include <g3dmouse.h>

/******************************************************************************/
/* Each function( Widget, XtPointer, XtPointer ) must return FALSE for        */
/* redrawing the OGL Widget it belongs to or TRUE to redraw all OGL           */
/* Widgets                                                                    */
/******************************************************************************/

static int createCube ( G3DMOUSETOOL *mou, 
                        G3DSCENE     *sce, 
                        G3DCAMERA    *cam,
                        G3DURMANAGER *urm, 
                        uint32_t      flags, 
                        G3DEvent     *event );

/******************************************************************************/
G3DMOUSETOOLCREATECUBE *g3dmousetoolcreatecube_new ( ) {
    uint32_t structsize = sizeof ( G3DMOUSETOOLCREATECUBE );
    void *memarea = calloc ( 0x01, structsize );
    G3DMOUSETOOLCREATECUBE *ct = ( G3DMOUSETOOLCREATECUBE * ) memarea;

    if ( ct == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    g3dmousetool_init ( ct,
                        CREATECUBETOOL,
                        's',
                        NULL, /* no icon */
                        NULL,
                        NULL,
                        createCube,
                        0x00 );

    return ct;
}

/******************************************************************************/
static int createCube ( G3DMOUSETOOL *mou, 
                        G3DSCENE     *sce, 
                        G3DCAMERA    *cam,
                        G3DURMANAGER *urm, 
                        uint32_t      flags, 
                        G3DEvent     *event ) {
    static GLdouble MVX[0x10], PJX[0x10];
    static GLint VPX[0x04];
    static double objx, objy, objz,
                  winx, winy, winz;
    static G3DPRIMITIVE *pri;

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;
            G3DOBJECT *obj;
            uint32_t pid = g3dscene_getNextObjectID ( sce );

            /** we dont need to get the mvx as we use the world matrix from the scene object ***/
            glGetDoublev  ( GL_MODELVIEW_MATRIX, MVX  );
            glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
            glGetIntegerv ( GL_VIEWPORT, VPX );

            gluProject ( 0.0f, 0.0f, 0.0f, MVX, PJX, VPX, &winx, &winy, &winz );
            gluUnProject ( ( GLdouble ) bev->x,
                           ( GLdouble ) VPX[0x03] - bev->y,
                           ( GLdouble ) winz,
                           MVX, PJX, VPX,
                           &objx, &objy, &objz );

            pri = g3dcube_new ( pid, "Cube", 0x01, 0x01, 0x01, 0.0f );

            obj = ( G3DOBJECT * ) pri;
            obj->pos.x = objx;
            obj->pos.y = objy;
            obj->pos.z = objz;

            g3dobject_updateMatrix_r ( obj, 0x00 );

            g3durm_object_addChild ( urm, sce, flags, REDRAWVIEW | REDRAWLIST,
                                     NULL, ( G3DOBJECT * ) sce, obj );
        } return REDRAWALL;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;

            if ( pri ) {
                G3DOBJECT *obj = ( G3DOBJECT * ) pri;
                CUBEDATASTRUCT *data = ( CUBEDATASTRUCT * ) pri->data;
                float radius;

                gluProject ( 0.0f, 0.0f, 0.0f, MVX, PJX, VPX, &winx, &winy, &winz );
                gluUnProject ( ( GLdouble ) mev->x,
                               ( GLdouble ) VPX[0x03] - mev->y,
                               ( GLdouble ) winz,
                               MVX, PJX, VPX,
                               &objx, &objy, &objz );

                radius = sqrt ( ( objx - obj->pos.x ) * ( objx - obj->pos.x ) +
                                ( objy - obj->pos.y ) * ( objy - obj->pos.y ) +
                                ( objz - obj->pos.z ) * ( objz - obj->pos.z ) );

                g3dcube_build ( pri, data->nbx, data->nby, data->nbz, radius );

                return REDRAWVIEW | REDRAWCURRENTOBJECT;
            }
        } return 0x00;

        case G3DButtonRelease : {
            pri = NULL;
        } break;

        default : {

        } break;
    }

    return FALSE;
}
