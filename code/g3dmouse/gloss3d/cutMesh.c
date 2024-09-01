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

static void cutMesh_draw ( G3DMOUSETOOL *mou,
                           G3DSCENE     *sce,
                           G3DCAMERA    *curcam,
                           G3DENGINE    *engine,
                           uint64_t engine_flags );
static int cutMesh_event ( G3DMOUSETOOL *mou, 
                           G3DSCENE     *sce, 
                           G3DCAMERA    *cam,
                           G3DURMANAGER *urm, 
                           uint64_t engine_flags, 
                           G3DEvent     *event );

/******************************************************************************/
G3DMOUSETOOLCUTMESH *g3dmousetoolcutmesh_new ( ) {
    uint32_t structsize = sizeof ( G3DMOUSETOOLCUTMESH );
    void *memarea = calloc ( 0x01, structsize );
    G3DMOUSETOOLCUTMESH *cm =  ( G3DMOUSETOOLCUTMESH * ) memarea;

    if ( cm == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    g3dmousetool_init ( ( G3DMOUSETOOL * ) cm,
                                           CUTMESHTOOL,
                                           's',
                                           NULL,
                                           NULL,
                                           cutMesh_draw,
                                           cutMesh_event,
                                           0x00 );

    cm->restrict_to_selection = 0x00;


    return cm;
}

/******************************************************************************/
static void cutMesh_draw ( G3DMOUSETOOL *mou,
                           G3DSCENE     *sce,
                           G3DCAMERA    *curcam,
                           G3DENGINE    *engine,
                           uint64_t engine_flags ) {
    if ( ( engine_flags & VIEWVERTEX ) ||
         ( engine_flags & VIEWEDGE   ) ||
         ( engine_flags & VIEWFACE   ) ) {
        G3DMOUSETOOLCUTMESH *cm = ( G3DMOUSETOOLCUTMESH * ) mou;
        /*** two dimensions array [4][3](x,y,z), relative to object ***/
        G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

        glPushAttrib( GL_ALL_ATTRIB_BITS );
        glDisable   ( GL_DEPTH_TEST );

        if ( cm->start && cm->obj ) {
            int mvpMatrixLocation = glGetUniformLocation( engine->coloredShaderProgram,
                                                          "mvpMatrix" );
            float mvp[0x10];
            float mvw[0x10];
            SHADERVERTEX vertices[2] = { 0 };

            g3dcore_multMatrixf( curcam->obj.inverseWorldMatrix,
                                 cm->obj->worldMatrix,
                                 mvw );

            /*** the matrix by which vertices coords are transformed ***/
            g3dcore_multMatrixf( curcam->pmatrix, mvw, mvp );

            glUseProgram( engine->coloredShaderProgram );

            glUniformMatrix4fv( mvpMatrixLocation, 1, GL_FALSE, mvp );

            vertices[0x00].pos   = cm->coord[0x00];
            vertices[0x00].col.r = 1.0f;
            vertices[0x01].pos   = cm->coord[0x02];
            vertices[0x01].col.r = 1.0f;

            g3dengine_drawLine( engine, vertices, 0, engine_flags );

            glUseProgram( 0 );
        }

        glPopAttrib ( );
    }
}

/******************************************************************************/
static int cutMesh_event ( G3DMOUSETOOL *mou, 
                           G3DSCENE     *sce, 
                           G3DCAMERA    *cam,
                           G3DURMANAGER *urm, 
                           uint64_t engine_flags, 
                           G3DEvent     *event ) {
    /*** selection rectangle coords ***/
    static float MVX[0x10];
    static GLint VPX[0x04];
    G3DMOUSETOOLCUTMESH *cm = ( G3DMOUSETOOLCUTMESH * ) mou;

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;
            G3DOBJECT *obj = ( G3DOBJECT * ) g3dscene_getLastSelected ( sce );

            cm->obj   = obj;
            cm->start = 0x01;

            if ( ( engine_flags & VIEWVERTEX ) ||
                 ( engine_flags & VIEWEDGE   ) ||
                 ( engine_flags & VIEWFACE   ) ) {
                if ( obj && ( ( obj->type == G3DMESHTYPE   ) ||
                              ( obj->type == G3DSPLINETYPE ) ) ) {
                    G3DMESH *mes = ( G3DMESH * ) obj;
                    double worldx, worldy, worldz;

                    /*** We need the selected object matrix in order to create ***/
                    /*** the cutting plan and find its coords, but do not ***/
                    /*** forget the current matrix is the camera transformations **/
                    g3dcore_multMatrixf ( mes->obj.worldMatrix,
                                          cam->obj.inverseWorldMatrix,
                                          MVX );

                    glGetIntegerv ( GL_VIEWPORT, VPX );

                    g3dcore_unprojectf ( ( double ) bev->x,
                                         ( double ) VPX[0x03] - bev->y,
                                                    0.000001f,
                                                    MVX,
                                                    cam->pmatrix,
                                                    VPX,
                                                   &worldx,
                                                   &worldy,
                                                   &worldz );
                    /* double to float */
                    cm->coord[0x00].x = cm->coord[0x02].x = worldx;
                    cm->coord[0x00].y = cm->coord[0x02].y = worldy;
                    cm->coord[0x00].z = cm->coord[0x02].z = worldz;

                    g3dcore_unprojectf ( ( double ) bev->x,
                                         ( double ) VPX[0x03] - bev->y,
                                                    0.999999f,
                                                    MVX,
                                                    cam->pmatrix,
                                                    VPX,
                                                   &worldx,
                                                   &worldy,
                                                   &worldz );
                    /* double to float */
                    cm->coord[0x01].x = cm->coord[0x03].x = worldx;
                    cm->coord[0x01].y = cm->coord[0x03].y = worldy;
                    cm->coord[0x01].z = cm->coord[0x03].z = worldz;
                }
            }
        } return REDRAWVIEW;

        case G3DMotionNotify : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            if ( ( engine_flags & VIEWVERTEX ) ||
                 ( engine_flags & VIEWEDGE   ) ||
                 ( engine_flags & VIEWFACE   ) ) {
                if ( cm->obj ) {
                    double worldx, worldy, worldz;

                    g3dcore_unprojectf ( ( double ) bev->x,
                                         ( double ) VPX[0x03] - bev->y, 
                                                    0.000001f,
                                                    MVX,
                                                    cam->pmatrix,
                                                    VPX,
                                                   &worldx,
                                                   &worldy,
                                                   &worldz );
                    /* double to float */
                    cm->coord[0x02].x = worldx;
                    cm->coord[0x02].y = worldy;
                    cm->coord[0x02].z = worldz;

                    g3dcore_unprojectf ( ( double ) bev->x,
                                         ( double ) VPX[0x03] - bev->y,
                                                    0.999999f,
                                                    MVX,
                                                    cam->pmatrix,
                                                    VPX,
                                                   &worldx,
                                                   &worldy,
                                                   &worldz );
                    /* double to float */
                    cm->coord[0x03].x = worldx;
                    cm->coord[0x03].y = worldy;
                    cm->coord[0x03].z = worldz;
                }
            }
        } return REDRAWVIEW;

        case G3DButtonRelease : {
            /*XButtonEvent *bev = ( G3DButtonEvent * ) event;*/
            G3DVERTEX *ver[0x04];
            G3DFACE *knife;
            int i;

            if ( ( engine_flags & VIEWVERTEX ) ||
                 ( engine_flags & VIEWEDGE   ) ||
                 ( engine_flags & VIEWFACE   ) ) {
                if ( cm->obj ) {
                    for ( i = 0x00; i < 0x04; i++ ) {
                        ver[i] = g3dvertex_new ( cm->coord[i].x,
                                                 cm->coord[i].y,
                                                 cm->coord[i].z );
                    }

                    knife = g3dquad_new ( ver[0x00], ver[0x01],
                                          ver[0x03], ver[0x02] );

                    if ( cm->obj->type == G3DMESHTYPE ) {
                        G3DMESH *mes = ( G3DMESH * ) cm->obj;

                        g3durm_mesh_cut ( urm, 
                                          mes, 
                                          knife,   
                                          cm->restrict_to_selection, 
                                          engine_flags, 
                                          REDRAWVIEW );
                    }

                    if ( cm->obj->type == G3DSPLINETYPE ) {
                        G3DSPLINE *spline = ( G3DSPLINE * ) cm->obj;

                        g3durm_spline_cut ( urm,
                                            spline, 
                                            knife,
                                            engine_flags,
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
            cm->obj = NULL;

        } return REDRAWVIEW;

        default :
        break;
    }

    return 0x00;
}
