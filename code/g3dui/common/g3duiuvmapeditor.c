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
#include <g3dui.h>

#define CLEARCOLOR 100

/******************************************************************************/
void common_g3duiuvmapeditor_moveSideward ( G3DUIUVMAPEDITOR *uvme, 
                                            int32_t           x, 
                                            int32_t           y, 
                                            int32_t           xold, 
                                            int32_t           yold ) {

}


/******************************************************************************/
void common_g3duiuvmapeditor_moveForward ( G3DUIUVMAPEDITOR *uvme, 
                                           int32_t           x, 
                                           int32_t           xold ) {

}


/******************************************************************************/
/* Find the current button ID according to x and y coordinates. This helsp us */
/* determine which button we are on. We don't use Motif button here because   */
/* we need to track the mouse after a click, something I think, will be easier*/
/* to achieve that way. I might be wrong though :-/.                          */
/******************************************************************************/
int common_g3duiuvmapeditor_getCurrentButton ( G3DUIUVMAPEDITOR *uvme,
                                               int x,
                                               int y ) {
    int i;

    for ( i = 0x00; i < NBUVMAPBUTTON; i++ ) {
        G3DUIRECTANGLE *rec = &uvme->rec[i];

        if ( ( ( x >= rec->x ) && ( x <= ( rec->x + rec->width  ) ) ) &&
             ( ( y >= rec->y ) && ( y <= ( rec->y + rec->height ) ) ) ) {
            return i;
        }
    }

    return -1;
}

/******************************************************************************/
void common_g3duiuvmapeditor_init ( G3DUIUVMAPEDITOR *uvme, 
                                    uint32_t          width,
                                    uint32_t          height ) {
    common_g3duiuvmapeditor_resize ( uvme, width, height );

    uvme->buttonID = -1;
}

/******************************************************************************/
void common_g3duiuvmapeditor_resize ( G3DUIUVMAPEDITOR *uvme, 
                                      uint32_t          width, 
                                      uint32_t          height ) {
    int i, xpos = ( width - BUTTONSIZE ), brd = 0x02;

    /*** set rectangle position for each button ***/
    for ( i = 0x00; i < NBUVMAPBUTTON; i++, xpos = ( xpos - BUTTONSIZE - brd ) ) {
        uvme->rec[i].x      = xpos;
        uvme->rec[i].y      = 0x00;
        uvme->rec[i].width  = BUTTONSIZE;
        uvme->rec[i].height = BUTTONSIZE;
    }

    uvme->arearec.x      = 0x00;
    uvme->arearec.y      = BUTTONSIZE;
    uvme->arearec.width  = width;
    uvme->arearec.height = height - BUTTONSIZE;
}

/******************************************************************************/
LIST *common_g3duiuvmapeditor_getUV ( G3DUIUVMAPEDITOR *uvme,
                                      G3DUI            *gui,
                                      int32_t           xm,
                                      int32_t           ym,
                                      uint32_t          width,
                                      uint32_t          height ) {
    G3DOBJECT *obj = g3dscene_getSelectedObject ( gui->sce );
    LIST *luv = NULL;
    int32_t x, y;

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );

        if ( uvmap ) {
             LIST *ltmpfac = ( uvmap->facgrp ) ? uvmap->facgrp->lfac :
                                                 mes->lfac;

            while ( ltmpfac ) {
                G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                G3DUVSET *uvs = g3dface_getUVSet ( fac, uvmap );

                if ( uvs ) {
                    uint32_t i;

                    for ( i = 0x00; i < fac->nbver; i++ ) {
                        int32_t xi = uvs->veruv[i].u * uvme->arearec.width,
                                yi = uvs->veruv[i].v * uvme->arearec.height;

                        if ( ( xi >= xm ) && ( xi <= ( xm + width  ) ) &&
                             ( yi >= ym ) && ( yi <= ( ym + height ) ) ) {
                            list_insert ( &luv, &uvs->veruv[i] );
                        }
                    }
                }

                ltmpfac = ltmpfac->next;
            }
        }
    }

    return luv;
}

/******************************************************************************/
void common_g3duiuvmapeditor_showGL ( G3DUIUVMAPEDITOR *uvme,
                                      G3DUI            *gui ) {
    G3DOBJECT *obj = g3dscene_getSelectedObject ( gui->sce );

    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho ( 0.0f, uvme->arearec.width, 
                    uvme->arearec.height, 0.0f, 0.0f, 1.0f );

    glMatrixMode ( GL_MODELVIEW );
    glLoadIdentity ( );

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );

        if ( uvmap ) {
             LIST *ltmpfac = ( uvmap->facgrp ) ? uvmap->facgrp->lfac :
                                                 mes->lfac;

            while ( ltmpfac ) {
                G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                G3DUVSET *uvs = g3dface_getUVSet ( fac, uvmap );

                if ( uvs ) {
                    uint32_t i;

                    glLineWidth ( 1.0f );
                    glColor3ub ( 0x00, 0x00, 0x00 );
                    glBegin ( GL_LINES );
                    for ( i = 0x00; i < fac->nbver; i++ ) {
                        uint32_t n = ( i + 0x01 ) % fac->nbver;
                        int32_t xi = uvs->veruv[i].u * uvme->arearec.width,
                                yi = uvs->veruv[i].v * uvme->arearec.height;
                        int32_t xn = uvs->veruv[n].u * uvme->arearec.width,
                                yn = uvs->veruv[n].v * uvme->arearec.height;

                        glVertex2f ( xi, yi );
                        glVertex2f ( xn, yn );

                    }
                    glEnd ( );

                    if ( gui->flags & VIEWVERTEX ) { 
                        glPointSize ( 3.0f );
                        glColor3ub ( 0x00, 0x00, 0xFF );
                        glBegin ( GL_POINTS );
                        for ( i = 0x00; i < fac->nbver; i++ ) {
                            int32_t xi = uvs->veruv[i].u * uvme->arearec.width,
                                    yi = uvs->veruv[i].v * uvme->arearec.height;

                            glVertex2f ( xi, yi );
                        }
                        glEnd ( );
                    }
                }

                ltmpfac = ltmpfac->next;
            }
        }
    }
}

/******************************************************************************/
void common_g3duiuvmapeditor_sizeGL ( G3DUIUVMAPEDITOR *uvme, 
                                      uint32_t          width, 
                                      uint32_t          height ) {
    glViewport ( 0, 0, width, height );
}

/******************************************************************************/
void common_g3duiuvmapeditor_initGL ( G3DUIUVMAPEDITOR *uvme ) {
    float clearColorf = ( float ) CLEARCOLOR / 255.0f;

    /*** Set clear color for the OpenGL Window ***/
    glClearColor ( clearColorf, clearColorf, clearColorf, 1.0f );

    glDisable ( GL_LIGHTING   );
    glEnable  ( GL_DEPTH_TEST );

    /*** I still have to understand this below ***/
    /*** I got it from the almighty Internet for avoiding Z-Fighting ***/
    glPolygonOffset ( 1.0f, 1.0f );
    glEnable ( GL_POLYGON_OFFSET_FILL );

    glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
    /*glLightModeli ( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );*/
}
