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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
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
/***************** Alloc memory for createFace tool private data **************/
/******** We could have used structure declared as static variable too ********/
G3DCREATEFACE *createFace_new ( ) {
    uint32_t structsize = sizeof ( G3DCREATEFACE );

    G3DCREATEFACE *cf =  ( G3DCREATEFACE * ) calloc ( 0x04, structsize );

    if ( cf == NULL ) {
        fprintf ( stderr, "createFace_new: Memory allocation failed\n" );
    }

    return cf;
}

/******************************************************************************/
uint32_t createFace_init ( G3DMOUSETOOL *mou, G3DSCENE *sce, 
                                              G3DCAMERA *cam,
                                              G3DURMANAGER *urm, 
                                              uint32_t engine_flags ) {
    /*** reset the vertex array ***/
    if ( mou->data ) {
        G3DCREATEFACE *cf = mou->data;
        G3DVERTEX **ver = cf->ver;

        /*g3dmesh_unselectAllVertices ( cf->mes );*/

        ver[0x00] = ver[0x01] = ver[0x02] = ver[0x03] = NULL;
        cf->draw = 0x00;
        cf->mes = NULL;
    } else {
        mou->data = createFace_new ( );
    }

    return 0x00;
}

/******************************************************************************/
void createFace_draw ( G3DMOUSETOOL *mou, G3DSCENE *sce, uint32_t flags ) {
    G3DCREATEFACE *cf = mou->data;

    if ( cf && cf->draw ) {
        G3DVERTEX **ver = cf->ver;
        G3DMESH *mes = cf->mes;

        if ( cf->mes ) {
            glPushAttrib( GL_ALL_ATTRIB_BITS );
            glDisable   ( GL_DEPTH_TEST );
            glDisable   ( GL_LIGHTING );
            glColor3ub  ( 0xFF, 0x00, 0x00 );

            /*** We need the selected object matrix in order to create ***/
            /*** the cutting plan and find its coords, but do not ***/
            /*** forget the current matrix is the camera transformations **/
            glPushMatrix ( );
            glMultMatrixd ( ((G3DOBJECT*)mes)->wmatrix );

            glBegin ( GL_LINES );
            if ( ver[0x02] && ver[0x03] ) {
                glVertex3fv ( ( GLfloat * ) &ver[0x02]->pos );
                glVertex3fv ( ( GLfloat * ) &ver[0x03]->pos );
            }

            if ( ver[0x00] && ver[0x01] ) {
                glVertex3fv ( ( GLfloat * ) &ver[0x00]->pos );
                glVertex3fv ( ( GLfloat * ) &ver[0x01]->pos );
            }
            glEnd ( );

            glPopMatrix ( );

            glPopAttrib ( );
        }
    }
}

/******************************************************************************/
/*void createFace_undo ( void *data ) {
    URMCREATEFACE *ucf = ( URMCREATEFACE * ) data;
    G3DFACE *fac = ucf->fac;
    G3DMESH *mes = ucf->mes;
    int i;

    list_remove ( &mes->lfac, fac );

    for ( i = 0x00; i < fac->nbver; i++ ) {
        list_remove ( &fac->ver[i]->lfac, fac );

        fac->ver[i]->nfac--;
    }
}*/

/******************************************************************************/
/*void createFace_redo ( void *data ) {
    URMCREATEFACE *ucf = ( URMCREATEFACE * ) data;
    G3DFACE *fac = ucf->fac;
    G3DMESH *mes = ucf->mes;
    int i;

    list_append ( &mes->lfac, fac );

    for ( i = 0x00; i < fac->nbver; i++ ) {
        list_append ( &fac->ver[i]->lfac, fac );

        fac->ver[i]->nfac++;
    }
}*/

/******************************************************************************/
/*void createFace_free ( void *data ) {
    URMCREATEFACE *ucf = ( URMCREATEFACE * ) data;
    G3DFACE *fac = ( G3DFACE * ) ucf->fac;

    g3dface_free ( fac );

    free ( ucf );
}*/

/******************************************************************************/
/*void createFace_push ( G3DURMANAGER *urm, G3DMESH *mes, G3DFACE *fac ) {
    URMCREATEFACE *ucf;

    ucf = ( URMCREATEFACE * ) calloc ( 0x01, sizeof ( URMCREATEFACE ) );

    if ( ucf == NULL ) {
        fprintf ( stderr, "createFace_push: calloc failed\n" );

        return;
    }

    ucf->mes = mes;
    ucf->fac = fac;

    g3durmanager_push ( urm, createFace_undo,
                             createFace_redo,
                             createFace_free, ucf );
}*/

/******************************************************************************/
int createFace_tool  ( G3DMOUSETOOL *mou, G3DSCENE *sce, G3DCAMERA *cam,
                       G3DURMANAGER *urm, uint32_t flags, G3DEvent *event ) {
    /*G3DURMANAGER *urm = gdt->urm;*/
    G3DCREATEFACE *cf = mou->data;
    G3DVERTEX **ver = cf->ver;
    static GLint VPX[0x04];
    G3DPICKTOOL pt;

    pt.only_visible = 0x00;

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            /*** Start drawing ***/
            cf->draw = 0x01;

            glGetIntegerv ( GL_VIEWPORT, VPX );

            if ( obj && ( bev->button == 0x01 ) && ( obj->type & EDITABLE ) ) {
                G3DMESH *mes = ( G3DMESH * ) obj;
                /*** Selection rectangle ***/
                int32_t coord[0x04] = { bev->x, VPX[0x03] - bev->y,
                                        bev->x, VPX[0x03] - bev->y };

                if ( !ver[0x00] && !ver[0x01] && !ver[0x02] && !ver[0x03] ) {
                    g3dmesh_unselectAllVertices ( mes );
                }

                pick_Item ( &pt, ( G3DOBJECT * ) mes, cam, coord, 0x00, flags );

                /*** remember our Mesh for the drawing part ***/
                /*** because we need its world matrix.      ***/
                cf->mes = mes;

                /*** if any selected vertex ***/
                if ( mes->lselver ) {
                    if ( ver[0x00] ) {
                        ver[0x02] = g3dmesh_getLastSelectedVertex ( mes );
                    } else {
                        ver[0x00] = g3dmesh_getLastSelectedVertex ( mes );
                    }
                } else {
                /*** reset if click fails to pick a vertex ***/
                    ver[0x00] = ver[0x01] = ver[0x02] = ver[0x03] = NULL;
                }
            }
        } return REDRAWVIEW;

        case G3DMotionNotify : {
            G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
            G3DMotionEvent *bev = ( G3DMotionEvent * ) event;

            if ( obj && ( bev->state & G3DButton1Mask ) && ( obj->type & EDITABLE ) ) {
                G3DMESH *mes = ( G3DMESH * ) obj;
                /*** Selection rectangle ***/
                int32_t coord[0x04] = { bev->x, VPX[0x03] - bev->y,
                                        bev->x, VPX[0x03] - bev->y };

                pick_Item ( &pt, ( G3DOBJECT * ) mes, cam, coord, 0x00, VIEWVERTEX );

                if ( mes->lselver ) {
                    if ( ver[0x02] ) {
                        ver[0x03] = g3dmesh_getLastSelectedVertex ( mes );

                        g3dmesh_unselectAllVertices ( mes );
                        g3dmesh_selectVertex ( mes, ver[0x02] );
                        g3dmesh_selectVertex ( mes, ver[0x03] );
                    } else {
                        if ( ver[0x00] ) {
                            ver[0x01] = g3dmesh_getLastSelectedVertex ( mes );

                            g3dmesh_unselectAllVertices ( mes );
                            g3dmesh_selectVertex ( mes, ver[0x00] );
                            g3dmesh_selectVertex ( mes, ver[0x01] );
                        }
                    }
                }
            }
        } return REDRAWVIEW;

        case G3DButtonRelease : {
            G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
            /*XButtonEvent *bev = ( G3DButtonEvent * ) event;*/

            if (  ( obj ) && ( obj->type & EDITABLE ) && ( ver[0x00] &&
                                                           ver[0x01] && 
                                                           ver[0x02] &&
                                                           ver[0x03] ) ) {
                /*** Must not be the same vertices ***/
                if ( ( ( ver[0x00] != ver[0x02] ) && 
                       ( ver[0x01] != ver[0x03] ) ) ||
                     ( ( ver[0x00] != ver[0x03] ) && 
                       ( ver[0x01] != ver[0x02] ) ) ) {
                    G3DMESH *mes = ( G3DMESH * ) obj;
                    G3DFACE *fac = NULL;
                    G3DVERTEX *tmpver2 = ver[0x02],
                              *tmpver3 = ver[0x03];

                    /*** Triangle - ver[3] differs from the first 2 vertices ***/
                    if ( ( ver[0x02] == ver[0x00] ) ||
                         ( ver[0x02] == ver[0x01] ) ) {
                        ver[0x02] = ver[0x03];
                        ver[0x03] = NULL;
                    }

                    /*** Triangle - ver[2] differs from the first 2 vertices ***/
                    if ( ( ver[0x03] == ver[0x00] ) ||
                         ( ver[0x03] == ver[0x01] ) ) {
                        ver[0x03] = NULL;
                    }

                    /*** if ver[0x03] != NULL, then we did not create any ***/ 
                    /*** triangle. Then, create a QUAD ***/
                    if ( ver[0x03] ) {
                        fac = g3dquad_new ( ver[0x00], ver[0x01],
                                            ver[0x03], ver[0x02] );
                    } else {
                        fac = g3dtriangle_new ( ver[0x00], ver[0x01],
                                                           ver[0x02] );
                    }

                    g3dmesh_addFace ( mes, fac );

                    if ( g3dface_checkOrientation ( fac ) ) {
                        g3dface_invertNormal ( fac );
                    }

                    g3durm_mesh_createFace ( urm, mes, fac, REDRAWVIEW );

                    /*** regenerate subdivision buffer ***/
                    g3dmesh_update ( mes, NULL,
                                          NULL,
                                          NULL,
                                          UPDATEFACEPOSITION |
                                          UPDATEFACENORMAL   |
                                          UPDATEVERTEXNORMAL |
                                          RESETMODIFIERS, flags );

                    /*** be ready for another bridging ***/
                    ver[0x00] = tmpver2;
                    ver[0x01] = tmpver3;

                    ver[0x02] = NULL;
                    ver[0x03] = NULL;

                /*** add this action to undo/redo stack ***/
                /*createFace_push ( urm, mes, fac );*/
                }
            }

            /*** end drawing ***/
            cf->draw = 0x00;
            cf->mes = NULL;
        } return REDRAWVIEW;

        default :
        break;
    }

    /*xold = event->xmotion.x;
    yold = event->xmotion.y;*/

    return FALSE;
}
