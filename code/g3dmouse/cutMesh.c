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
/******************************************************************************/
G3DCUTMESH *cutMesh_new ( ) {
    uint32_t structsize = sizeof ( G3DCUTMESH );
    G3DCUTMESH *cm =  ( G3DCUTMESH * ) calloc ( 0x01, structsize );

    if ( cm == NULL ) {
        fprintf ( stderr, "cutMesh_new: Memory allocation failed\n" );
    }

    cm->restrict_to_selection = 0x01;


    return cm;
}

/******************************************************************************/
uint32_t cutMesh_init ( G3DMOUSETOOL *mou, G3DSCENE *sce, 
                                           G3DCAMERA *cam,
                                           G3DURMANAGER *urm, 
                                           uint32_t engine_flags ) {
    if ( mou->data ) {
        G3DCUTMESH *cm = mou->data;
    } else {
        mou->data = cutMesh_new ( );
    }

    return 0x00;
}

/******************************************************************************/
void cutMesh_draw ( G3DMOUSETOOL *mou, G3DSCENE *sce, uint32_t flags ) {
    if ( ( flags & VIEWVERTEX ) ||
         ( flags & VIEWEDGE   ) ||
         ( flags & VIEWFACE   ) ) {
        G3DCUTMESH *cm = mou->data;
        /*** two dimensions array [4][3](x,y,z), relative to object ***/
        GLdouble (*coord)[0x03] = cm->coord;
        G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

        glPushAttrib( GL_ALL_ATTRIB_BITS );
        glDisable   ( GL_DEPTH_TEST );
        glDisable   ( GL_LIGHTING );
        glColor3ub ( 0xFF, 0x00, 0x00 );

        if ( cm->start && obj ) {
            glPushMatrix ( );
            glMultMatrixd ( obj->wmatrix );
            glBegin ( GL_LINES );
            glVertex3dv ( ( GLdouble * ) &coord[0x00] );
            glVertex3dv ( ( GLdouble * ) &coord[0x02] );
            glEnd ( );
            glPopMatrix ( );
        }

        glPopAttrib ( );
    }
}

/*****************************************************************************/
int cutMesh_tool ( G3DMOUSETOOL *mou, G3DSCENE *sce, G3DCAMERA *cam,
                   G3DURMANAGER *urm, uint32_t flags, G3DEvent *event ) {
    /*** selection rectangle coords ***/
    static GLdouble MVX[0x10], PJX[0x10];
    static GLint VPX[0x04];
    static G3DOBJECT *obj;
    static G3DMESH *mes;
    G3DCUTMESH *cm = mou->data;

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            obj = ( G3DOBJECT * ) g3dscene_getLastSelected ( sce );

            cm->start = 0x01;

            if ( ( flags & VIEWVERTEX ) ||
                 ( flags & VIEWEDGE   ) ||
                 ( flags & VIEWFACE   ) ) {
                if ( obj && ( ( obj->type == G3DMESHTYPE   ) ||
                              ( obj->type == G3DSPLINETYPE ) ) ) {
                    mes = ( G3DMESH * ) obj;
                    /*** We need the selected object matrix in order to create ***/
                    /*** the cutting plan and find its coords, but do not ***/
                    /*** forget the current matrix is the camera transformations **/
                    glPushMatrix ( );
                    glMultMatrixd ( ((G3DOBJECT*)mes)->wmatrix );

                    glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
                    glGetDoublev  ( GL_MODELVIEW_MATRIX,  MVX );
                    glGetIntegerv ( GL_VIEWPORT, VPX );

                    glPopMatrix ( );

                    gluUnProject ( ( double ) bev->x,
                                   ( double ) VPX[0x03] - bev->y, 0.000001f,
                                    MVX, PJX, VPX, &cm->coord[0x00][0x00],
                                                   &cm->coord[0x00][0x01],
                                                   &cm->coord[0x00][0x02] );
                    gluUnProject ( ( double ) bev->x,
                                   ( double ) VPX[0x03] - bev->y, 0.999999f,
                                    MVX, PJX, VPX, &cm->coord[0x01][0x00],
                                                   &cm->coord[0x01][0x01],
                                                   &cm->coord[0x01][0x02] );

                    memcpy ( cm->coord[0x02], cm->coord[0x00], sizeof ( cm->coord[0x00] ) );
                    memcpy ( cm->coord[0x03], cm->coord[0x01], sizeof ( cm->coord[0x01] ) );
                }
            }
        } return REDRAWVIEW;

        case G3DMotionNotify : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            if ( ( flags & VIEWVERTEX ) ||
                 ( flags & VIEWEDGE   ) ||
                 ( flags & VIEWFACE   ) ) {
                if ( mes ) {
                    gluUnProject ( ( double ) bev->x,
                                   ( double ) VPX[0x03] - bev->y, 0.000001f,
                                    MVX, PJX, VPX, &cm->coord[0x02][0x00],
                                                   &cm->coord[0x02][0x01],
                                                   &cm->coord[0x02][0x02] );
                    gluUnProject ( ( double ) bev->x,
                                   ( double ) VPX[0x03] - bev->y, 0.999999f,
                                    MVX, PJX, VPX, &cm->coord[0x03][0x00],
                                                   &cm->coord[0x03][0x01],
                                                   &cm->coord[0x03][0x02] );
                } 
            }
        } return REDRAWVIEW;

        case G3DButtonRelease : {
            /*XButtonEvent *bev = ( G3DButtonEvent * ) event;*/
            G3DVERTEX *ver[0x04];
            G3DFACE *knife;
            int i;

            if ( ( flags & VIEWVERTEX ) ||
                 ( flags & VIEWEDGE   ) ||
                 ( flags & VIEWFACE   ) ) {
                if ( mes ) {
                    /*gluUnProject ( ( double ) bev->x, ( double ) bev->y, 0.001f,
                                    MVX, PJX, VPX, &coord[0x02][0x00],
                                                   &coord[0x02][0x01],
                                                   &coord[0x02][0x02] );
                    gluUnProject ( ( double ) bev->x, ( double ) bev->y, 0.999f,
                                    MVX, PJX, VPX, &coord[0x03][0x00],
                                                   &coord[0x03][0x01],
                                                   &coord[0x03][0x02] );*/

                    for ( i = 0x00; i < 0x04; i++ ) {
                        ver[i] = g3dvertex_new ( cm->coord[i][0x00],
                                                 cm->coord[i][0x01],
                                                 cm->coord[i][0x02] );
                    }

                    knife = g3dquad_new ( ver[0x00], ver[0x01],
                                          ver[0x03], ver[0x02] );

                    if ( obj->type == G3DMESHTYPE ) {
                        g3durm_mesh_cut ( urm, 
                                          mes, 
                                          knife,   
                                          cm->restrict_to_selection, 
                                          flags, 
                                          REDRAWVIEW );
                    }

                    if ( obj->type == G3DSPLINETYPE ) {
                        G3DSPLINE *spline = ( G3DSPLINE * ) obj;

                        g3durm_spline_cut ( urm,
                                            spline, 
                                            knife,
                                            flags,
                                            REDRAWVIEW );
                    }

                    free ( ver[0x00] );
                    free ( ver[0x01] );
                    free ( ver[0x02] );
                    free ( ver[0x03] );
                    free ( knife );
                }
            }

            cm->start = 0x00;

            obj = NULL;
            mes = NULL;
        } return REDRAWVIEW;

        default :
        break;
    }

    return 0x00;
}
