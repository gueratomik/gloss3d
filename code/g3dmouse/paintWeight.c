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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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
#include <g3dmouse.h>

/******************************************************************************/
uint32_t paintWeight_init ( G3DMOUSETOOL *mou, G3DSCENE *sce, 
                                               G3DCAMERA *cam,
                                               G3DURMANAGER *urm, 
                                               uint32_t engine_flags ) {
    return 0x00;
}

/******************************************************************************/
int paintWeight_tool ( G3DMOUSETOOL *mou, G3DSCENE *sce, G3DCAMERA *cam,
                       G3DURMANAGER *urm, uint32_t flags, G3DEvent *event ) {
    static double objx, objy, objz,
                  winx, winy, winz;
    static G3DOBJECT *obj;
    static GLint  VPX[0x04];
    static double MVX[0x10];
    static double PJX[0x10];
    G3DPICKTOOL *pt = mou->data;

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            obj = g3dscene_getLastSelected ( sce );

            if ( obj && ( obj->type == G3DMESHTYPE ) ) {
                G3DMESH *mes = ( G3DMESH * ) obj;

                /*** If there is no weightGroup, add one ***/
                if ( mes->lgrp == NULL ) {
                    G3DWEIGHTGROUP *grp;
                    char buf[0x20];

                    snprintf ( buf, 0x20, "VertexWeightGroup%02i", mes->nbgrp );

                    grp = g3dweightgroup_new ( mes, buf );

                    g3dmesh_addWeightGroup ( mes, grp );

                    mes->curgrp = grp;
                }
            }

            glGetIntegerv ( GL_VIEWPORT, VPX );
        } return REDRAWVIEW | REDRAWCURRENTOBJECT;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;

            if ( obj && ( obj->type == G3DMESHTYPE ) ) {
                G3DMESH *mes = ( G3DMESH * ) obj;

                /*** Paint only if a weightgroup is selected ***/
                if ( mes->curgrp ) {
                    uint32_t x = mev->x,
                             y = VPX[0x03] - mev->y;

                    glMatrixMode ( GL_PROJECTION );
                    glPushMatrix ( );
                    glLoadIdentity ( );
                    gluPickMatrix ( x, y, pt->paint_radius,
                                          pt->paint_radius, VPX );
                    g3dcamera_project ( cam, flags );
                    /*glGetDoublev ( GL_PROJECTION_MATRIX, PJX );*/

                    glMatrixMode ( GL_MODELVIEW );
                    glPushMatrix ( );
                    glLoadIdentity ( );
                    g3dcamera_view ( cam, 0x00 );
                    glMultMatrixd ( obj->wmatrix );
                    /*glGetDoublev ( GL_MODELVIEW_MATRIX, MVX );*/

/*** Note : If pt->weight == 0.0f, the vertex is removed from the weightgroup ***/
                    g3dmesh_paintVertices ( mes, cam, pt->weight,
                                                      pt->only_visible, flags );

                    glPopMatrix   ( );
                    glMatrixMode  ( GL_PROJECTION );
                    glPopMatrix   ( );

                    glMatrixMode ( GL_MODELVIEW );
                }

                return REDRAWVIEW;
            }
        } return 0x00;

        case G3DButtonRelease : {
            obj = NULL;
        } break;

        default : {

        } break;
    }

    return FALSE;
}







