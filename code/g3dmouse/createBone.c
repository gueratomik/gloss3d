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
#include <g3dmouse.h>

/******************************************************************************/
int createBone ( G3DMOUSETOOL *mou, G3DSCENE *sce, G3DCAMERA *cam,
                 G3DURMANAGER *urm, uint32_t flags, G3DEvent *event ) {
    static GLdouble MVX[0x10], PJX[0x10];
    static GLint VPX[0x04];
    static double objx, objy, objz,
                  winx, winy, winz;
    static G3DBONE *bon;

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;
            G3DOBJECT *par = NULL;
            G3DOBJECT *obj;
            uint32_t pid = g3dscene_getNextObjectID ( sce );

            if ( bev->state & G3DControlMask ) {
                par = g3dscene_getLastSelected ( sce );
            }

            if ( par == NULL || ( par->type != G3DBONETYPE ) ) {
                par = ( G3DOBJECT * ) sce;
            }

            glPushMatrix ( );
            glMultMatrixd ( par->wmatrix );
            glGetDoublev  ( GL_MODELVIEW_MATRIX, MVX  );
            glPopMatrix ( );

            glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
            glGetIntegerv ( GL_VIEWPORT, VPX );

            gluProject ( 0.0f, 0.0f, 0.0f, MVX, PJX, VPX, &winx, &winy, &winz );
            gluUnProject ( ( GLdouble ) bev->x,
                           ( GLdouble ) VPX[0x03] - bev->y,
                           ( GLdouble ) winz,
                           MVX, PJX, VPX,
                           &objx, &objy, &objz );

            bon = g3dbone_new ( pid, "Bone", 0.0f );

            obj = ( G3DOBJECT * ) bon;

            obj->pos.x = objx;
            obj->pos.y = objy;
            obj->pos.z = objz;

            g3dobject_updateMatrix_r ( obj, 0x00 );

            g3durm_object_addChild ( urm, sce, flags, REDRAWVIEW | REDRAWLIST,
                                     NULL, par, obj );
        } return REDRAWALL;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;

            if ( bon ) {
                G3DOBJECT *obj = ( G3DOBJECT * ) bon;
                G3DVECTOR yref = { .x = 0.0f, .y = 1.0f, .z = 0.0f, .w = 1.0f },
                          nref = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f },
                          axis;
                float c, s, t, tx, ty, angle, a20;
                float len;

                glPushMatrix ( );
                glMultMatrixd ( obj->parent->wmatrix );
                glGetDoublev  ( GL_MODELVIEW_MATRIX, MVX  );
                glPopMatrix ( );

                gluProject ( 0.0f, 0.0f, 0.0f, MVX, PJX, VPX, &winx, &winy, &winz );
                gluUnProject ( ( GLdouble ) mev->x,
                               ( GLdouble ) VPX[0x03] - mev->y,
                               ( GLdouble ) winz,
                               MVX, PJX, VPX,
                               &objx, &objy, &objz );

                nref.x = ( objx - obj->pos.x );
                nref.y = ( objy - obj->pos.y );
                nref.z = ( objz - obj->pos.z );

                bon->len = g3dvector_length ( &nref );

                g3dvector_cross ( &yref, &nref, &axis );

                g3dvector_normalize ( &axis, NULL );

                angle = g3dvector_angle ( &yref, &nref );

                c  = ( float ) cos ( angle );
                s  = ( float ) sin ( angle );
                t  = ( float ) 1.0f - c;
                tx = t * axis.x;
                ty = t * axis.y;

                a20 = tx * axis.z - s * axis.y; /* according to the tutorial i read, a20 was negative, but i didnt worked, so i let a20 positive and it works */

                obj->rot.y = ( float ) asin ( a20 ) / M_PI * 180;
/*printf("%f %f %f\n", obj->rot.x, obj->rot.y, obj->rot.z );*/
                if ( a20 == 1.0f || a20 == -1.0f ){
                    obj->rot.x = ( float ) atan2 ( tx * axis.y - s * axis.z, ty * axis.y + c ) / M_PI * 180;
                    obj->rot.z = ( float ) 0.0f;
                } else {
                    obj->rot.x = ( float ) atan2 ( ty * axis.z + s * axis.x, t * axis.z * axis.z + c ) / M_PI * 180;
                    obj->rot.z = ( float ) atan2 ( tx * axis.y + s * axis.z, tx * axis.x + c ) / M_PI * 180;
                }

                g3dobject_updateMatrix_r ( obj, 0x00 );

                return REDRAWVIEW | REDRAWCURRENTOBJECT;
            }
        } return 0x00;

        case G3DButtonRelease : {
            bon = NULL;
        } break;

        default : {

        } break;
    }

    return FALSE;
}
