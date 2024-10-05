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
                        uint64_t engine_flags, 
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
                        G3DCAMERA    *curcam,
                        G3DURMANAGER *urm, 
                        uint64_t engine_flags, 
                        G3DEvent     *event ) {
    static GLint VPX[0x04];
    static double objx, objy, objz,
                  winx, winy, winz;
    static G3DPRIMITIVE *pri;
    static float MVX[0x10];

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;
            G3DOBJECT *obj;
            uint32_t pid = g3dscene_getNextObjectID ( sce );

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

            pri = g3dcube_new ( pid, "Cube" );

            g3dcube_build ( pri, 0x01, 0x01, 0x01, 0.0f );

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
        } return UPDATEANDREDRAWALL;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;

            if ( pri ) {
                G3DOBJECT *obj = ( G3DOBJECT * ) pri;
                CUBEDATASTRUCT *data = ( CUBEDATASTRUCT * ) pri->data;
                float radius;

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

                g3dcube_build ( pri, data->nbx, data->nby, data->nbz, radius );

                return REDRAWVIEW | UPDATECURRENTOBJECT;
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
