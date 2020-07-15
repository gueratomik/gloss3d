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
/* each function must return FALSE for redrawing the OGL Widget it belongs to */
/* only or TRUE to redraw all OGL Widgets                                     */
/******************************************************************************/

static int createTube ( G3DMOUSETOOL *mou, 
                        G3DSCENE     *sce, 
                        G3DCAMERA    *cam,
                        G3DURMANAGER *urm, 
                        uint32_t      flags, 
                        G3DEvent     *event );

/******************************************************************************/
G3DMOUSETOOLCREATETUBE *g3dmousetoolcreatetube_new ( ) {
    uint32_t structsize = sizeof ( G3DMOUSETOOLCREATETUBE );
    void *memarea = calloc ( 0x01, structsize );
    G3DMOUSETOOLCREATETUBE *ct = ( G3DMOUSETOOLCREATETUBE * ) memarea;

    if ( ct == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    g3dmousetool_init ( ct,
                        CREATETUBETOOL,
                        's',
                        NULL, /* no icon */
                        NULL,
                        NULL,
                        createTube,
                        0x00 );

    return ct;
}

/******************************************************************************/
static int createTube ( G3DMOUSETOOL *mou, 
                        G3DSCENE     *sce, 
                        G3DCAMERA    *cam,
                        G3DURMANAGER *urm, 
                        uint32_t      flags, 
                        G3DEvent     *event ) {
    static GLdouble MVX[0x10], PJX[0x10];
    static GLint VPX[0x04];
    static double objx, objy, objz,
                  winx, winy, winz;
    static G3DPRIMITIVE *pri, *tub;

    switch ( event->type ) {
        case G3DButtonPress : {
            /*** if step2 and step1 are over ***/
            if ( ( tub == NULL ) && ( pri == NULL ) ) {
                G3DButtonEvent *bev = ( G3DButtonEvent * ) event;
                G3DOBJECT *obj;
                /*** we dont need to get the mvx as we use the world matrix ***/
                /*** from the scene object ***/
                glGetDoublev  ( GL_MODELVIEW_MATRIX, MVX  );
                glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
                glGetIntegerv ( GL_VIEWPORT, VPX );

                gluProject ( 0.0f, 0.0f, 0.0f, MVX, PJX, VPX, &winx, &winy, &winz );
                gluUnProject ( ( GLdouble ) bev->x,
                               ( GLdouble ) VPX[0x03] - bev->y,
                               ( GLdouble ) winz,
                               MVX, PJX, VPX,
                               &objx, &objy, &objz );

                pri = g3dtube_new ( g3dscene_getNextObjectID ( sce ), "Tube", 0x18, 0x01, 0x01, 0.0f, 0.1f, 0.0f );
                obj = ( G3DOBJECT * ) pri;
                obj->pos.x = objx;
                obj->pos.y = objy;
                obj->pos.z = objz;

                g3dobject_updateMatrix_r ( obj, 0x00 );

                g3durm_object_addChild ( urm, sce, flags, REDRAWVIEW | REDRAWLIST,
                                         NULL, ( G3DOBJECT * ) sce, obj );
            }
        } return REDRAWALL;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;

            /*** step1, the radius ***/
            if ( pri ) {
                G3DOBJECT *obj = ( G3DOBJECT * ) pri;
                TUBEDATASTRUCT *data = ( TUBEDATASTRUCT * ) pri->data;
                float length, radius;

                gluProject ( 0.0f, 0.0f, 0.0f, MVX, PJX, VPX, &winx, &winy, &winz );
                gluUnProject ( ( GLdouble ) mev->x,
                               ( GLdouble ) VPX[0x03] - mev->y,
                               ( GLdouble ) winz,
                               MVX, PJX, VPX,
                               &objx, &objy, &objz );

                radius = sqrt ( ( objx - obj->pos.x ) * ( objx - obj->pos.x ) +
                                ( objy - obj->pos.y ) * ( objy - obj->pos.y ) +
                                ( objz - obj->pos.z ) * ( objz - obj->pos.z ) );

                length = radius;

                g3dtube_build ( pri, data->slice,
                                         data->capx, data->capy,
                                         radius, data->thickness, length );

                return REDRAWVIEW | REDRAWCURRENTOBJECT;
            }

            /*** step2, the length ***/
            if ( tub ) {
                G3DOBJECT *obj = ( G3DOBJECT * ) tub;
                TUBEDATASTRUCT *data = ( TUBEDATASTRUCT * ) tub->data;
                float length;

                gluProject ( 0.0f, 0.0f, 0.0f, MVX, PJX, VPX, &winx, &winy, &winz );
                gluUnProject ( ( GLdouble ) mev->x,
                               ( GLdouble ) VPX[0x03] - mev->y,
                               ( GLdouble ) winz,
                               MVX, PJX, VPX,
                               &objx, &objy, &objz );

                length = sqrt ( ( objx - obj->pos.x ) * ( objx - obj->pos.x ) +
                                ( objy - obj->pos.y ) * ( objy - obj->pos.y ) +
                                ( objz - obj->pos.z ) * ( objz - obj->pos.z ) );

                g3dtube_build ( tub, 
                                data->slice,
                                data->capx, data->capy,
                                data->radius, data->thickness, length );

                return REDRAWVIEW | REDRAWCURRENTOBJECT;
            }
        } return 0x00;

        case G3DButtonRelease : {
            /*** step2, the length ***/
            if ( tub ) {
                tub = NULL;
            }

            /*** step1, the radius ***/
            if ( pri ) {
                tub = pri;
                pri = NULL;
            }
        } break;

        default : {

        } break;
    }

    return FALSE;
}
