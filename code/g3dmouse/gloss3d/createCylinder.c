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

static int createCylinder_tool ( G3DMOUSETOOL *mou, 
                                 G3DSCENE     *sce, 
                                 G3DCAMERA    *cam,
                                 G3DURMANAGER *urm, 
                                 uint64_t engine_flags, 
                                 G3DEvent     *event );

/******************************************************************************/
G3DMOUSETOOLCREATECYLINDER *g3dmousetoolcreatecylinder_new ( ) {
    uint32_t structsize = sizeof ( G3DMOUSETOOLCREATECYLINDER );
    void *memarea = calloc ( 0x01, structsize );
    G3DMOUSETOOLCREATECYLINDER *cc = ( G3DMOUSETOOLCREATECYLINDER * ) memarea;

    if ( cc == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    g3dmousetool_init ( cc,
                        CREATECYLINDERTOOL,
                        's',
                        NULL, /* no icon */
                        NULL,
                        NULL,
                        createCylinder_tool,
                        0x00 );

    return cc;
}

/******************************************************************************/
static int createCylinder_tool ( G3DMOUSETOOL *mou, 
                                 G3DSCENE     *sce, 
                                 G3DCAMERA    *curcam,
                                 G3DURMANAGER *urm, 
                                 uint64_t engine_flags, 
                                 G3DEvent     *event ) {
    static GLint VPX[0x04];
    static double objx, objy, objz,
                  winx, winy, winz;
    static G3DPRIMITIVE *pri, *cyl;
    static float MVX[0x10];

    switch ( event->type ) {
        case G3DButtonPress : {
            /*** if step2 and step1 are over ***/
            if ( ( cyl == NULL ) && ( pri == NULL ) ) {
                G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

                /* can be replaced with cam->vmatrix */
                glGetIntegerv ( GL_VIEWPORT, VPX );

                g3dcore_projectf ( 0.0f,
                                   0.0f,
                                   0.0f,
                                   curcam->obj.inverseWorldMatrix,
                                   curcam->pmatrix,
                                   VPX,
                                  &winx,
                                  &winy,
                                  &winz );

                g3dcore_unprojectf ( ( GLdouble ) bev->x,
                                     ( GLdouble ) VPX[0x03] - bev->y,
                                     ( GLdouble ) winz,
                                                  curcam->obj.inverseWorldMatrix,
                                                  curcam->pmatrix,
                                                  VPX,
                                                 &objx,
                                                 &objy,
                                                 &objz );

                pri = g3dcylinder_new ( g3dscene_getNextObjectID ( sce ), "Cylinder" );

                g3dcylinder_build ( pri, 0x18, 0x01, 0x01, 0.0f, 0.0f );

                G3DOBJECTCAST(pri)->pos.x = objx;
                G3DOBJECTCAST(pri)->pos.y = objy;
                G3DOBJECTCAST(pri)->pos.z = objz;

                g3durm_object_addChild ( urm, 
                                         sce,
                                         engine_flags,
                                         REDRAWVIEW | REDRAWOBJECTLIST,
                                         NULL,
                                         G3DOBJECTCAST(sce),
                                         G3DOBJECTCAST(pri) );

                g3dobject_updateMatrix_r ( G3DOBJECTCAST(pri), 0x00 );

                g3dcore_multMatrixf( curcam->obj.inverseWorldMatrix,
                                     G3DOBJECTCAST(pri)->worldMatrix,
                                     MVX );
            }
        } return UPDATEANDREDRAWALL;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;

            /*** step1, the radius ***/
            if ( pri ) {
                CYLINDERDATASTRUCT *data = ( CYLINDERDATASTRUCT * ) pri->data;
                float length, radius;

                g3dcore_projectf ( 0.0f,
                                   0.0f,
                                   0.0f,
                                   MVX,
                                   curcam->pmatrix,
                                   VPX,
                                  &winx,
                                  &winy,
                                  &winz );

                g3dcore_unprojectf ( ( GLdouble ) mev->x,
                                     ( GLdouble ) VPX[0x03] - mev->y,
                                     ( GLdouble ) winz,
                                                  MVX,
                                                  curcam->pmatrix,
                                                  VPX,
                                                 &objx,
                                                 &objy,
                                                 &objz );

                radius = sqrt ( ( objx ) * ( objx ) +
                                ( objy ) * ( objy ) +
                                ( objz ) * ( objz ) );

                length = radius;

                g3dcylinder_build ( pri,
                                    data->slice,
                                    data->capx,
                                    data->capy,
                                    radius,
                                    length );

                return REDRAWVIEW | UPDATECURRENTOBJECT;
            }

            /*** step2, the length ***/
            if ( cyl ) {
                G3DOBJECT *obj = ( G3DOBJECT * ) cyl;
                CYLINDERDATASTRUCT *data = ( CYLINDERDATASTRUCT * ) cyl->data;
                float length;

                g3dcore_projectf ( 0.0f,
                                   0.0f,
                                   0.0f,
                                   MVX,
                                   curcam->pmatrix,
                                   VPX,
                                  &winx,
                                  &winy,
                                  &winz );

                g3dcore_unprojectf ( ( GLdouble ) mev->x,
                                     ( GLdouble ) VPX[0x03] - mev->y,
                                     ( GLdouble ) winz,
                                                  MVX,
                                                  curcam->pmatrix,
                                                  VPX,
                                                 &objx,
                                                 &objy,
                                                 &objz );
/*
                length = sqrt ( ( objx ) * ( objx ) +
                                ( objy ) * ( objy ) +
                                ( objz ) * ( objz ) );
*/
                length = objz;

                g3dcylinder_build ( cyl,
                                    data->slice,
                                    data->capx,
                                    data->capy,
                                    data->radius,
                                    length );

                return REDRAWVIEW | UPDATECURRENTOBJECT;
            }
        } return 0x00;

        case G3DButtonRelease : {
            /*** step2, the length ***/
            if ( cyl ) {
                cyl = NULL;
            }

            /*** step1, the radius ***/
            if ( pri ) {
                cyl = pri;
                pri = NULL;
            }
        } break;

        default : {

        } break;
    }

    return FALSE;
}
