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
#include <config.h>
#include <g3dui.h>

#define CLEARCOLOR 100

/******************************************************************************/
void common_g3duiuvmapeditor_moveSideward ( G3DUIUVMAPEDITOR *uvme, 
                                            int32_t           x, 
                                            int32_t           y, 
                                            int32_t           xold, 
                                            int32_t           yold ) {
    ((G3DOBJECT*)uvme->cam)->pos.x += ( ( float ) ( x - xold ) * 0.5f );
    ((G3DOBJECT*)uvme->cam)->pos.y += ( ( float ) ( y - yold ) * 0.5f );

    common_g3duiuvmapeditor_setCanevas ( uvme );
}


/******************************************************************************/
void common_g3duiuvmapeditor_moveForward ( G3DUIUVMAPEDITOR *uvme, 
                                           int32_t           x, 
                                           int32_t           xold ) {
    ((G3DOBJECT*)uvme->cam)->pos.z -= ( ( float ) ( x - xold ) * 0.01f );

    if ( ((G3DOBJECT*)uvme->cam)->pos.z < 0.0f ) {
        ((G3DOBJECT*)uvme->cam)->pos.z = 0.0f;
    }

    common_g3duiuvmapeditor_setCanevas ( uvme );
}

/******************************************************************************/
void common_g3duiuvmapeditor_setCanevas ( G3DUIUVMAPEDITOR *uvme ) {
    int32_t midx = ( uvme->arearec.width  * 0.5f ),
            midy = ( uvme->arearec.height * 0.5f );

    int32_t x, y;

    if ( uvme->cam ) {
        int32_t x1 = 0,
                y1 = 0,
                x2 = uvme->arearec.width,
                y2 = uvme->arearec.height;
        G3DVECTOR vec[0x02] = { { .x = x1 - midx,
                                  .y = y1 - midy,
                                  .z = 0.0f },
                                { .x = x2 - midx,
                                  .y = y2 - midy,
                                  .z = 0.0f } };

        G3DVECTOR *pos = &((G3DOBJECT*)uvme->cam)->pos;

        x1 = ( midx + pos->x ) + ( vec[0x00].x * pos->z );
        y1 = ( midy + pos->y ) + ( vec[0x00].y * pos->z );
        x2 = ( midx + pos->x ) + ( vec[0x01].x * pos->z );
        y2 = ( midy + pos->y ) + ( vec[0x01].y * pos->z );

        uvme->cam->canevas.x = x1;
        uvme->cam->canevas.y = y1;

        uvme->cam->canevas.width  = ( x2 - x1 );
        uvme->cam->canevas.height = ( y2 - y1 );
    }
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
void common_g3duiuvmapeditor_destroyGL ( G3DUIUVMAPEDITOR *uvme ) {
    g3dobject_free ( uvme->cam );
}

/******************************************************************************/
void common_g3duiuvmapeditor_init ( G3DUIUVMAPEDITOR *uvme, 
                                    uint32_t          width,
                                    uint32_t          height ) {
    uvme->buttonID = -1;

    uvme->cam = g3dcamera_new ( 0x00, "CAM", 0, 0, 0, 0 );

    ((G3DOBJECT*)uvme->cam)->pos.z = 1.0f;
    ((G3DOBJECT*)uvme->cam)->pos.x = 0.0f;
    ((G3DOBJECT*)uvme->cam)->pos.y = 0.0f;

    common_g3duiuvmapeditor_resize ( uvme, width, height );

    g3dcamera_setGrid  ( uvme->cam, NULL );
    g3dcamera_setOrtho ( uvme->cam, width, height );

    g3dobject_updateMatrix ( uvme->cam );
}

/******************************************************************************/
void common_g3duiuvmapeditor_resize ( G3DUIUVMAPEDITOR *uvme, 
                                      uint32_t          width, 
                                      uint32_t          height ) {
    int i, xpos = ( width - BUTTONSIZE ), brd = 0x02;

    /*** set rectangle position for each button ***/
    for ( i = 0x00; i < NBUVMAPBUTTON; i++, xpos = ( xpos - BUTTONSIZE - brd ) ) {
        uvme->rec[i].x      = xpos;
        uvme->rec[i].y      = TOOLBARBUTTONSIZE;
        uvme->rec[i].width  = BUTTONSIZE;
        uvme->rec[i].height = BUTTONSIZE - TOOLBARBUTTONSIZE;
    }

    uvme->arearec.x      = MODEBARBUTTONSIZE;
    uvme->arearec.y      = BUTTONSIZE;
    uvme->arearec.width  = width - MODEBARBUTTONSIZE;
    uvme->arearec.height = height - BUTTONSIZE;

    common_g3duiuvmapeditor_setCanevas ( uvme );
}

/******************************************************************************/
void common_g3duiuvmapeditor_showGL ( G3DUIUVMAPEDITOR *uvme,
                                      G3DUI            *gui,
                                      G3DMOUSETOOL     *mou,
                                      uint32_t          engine_flags ) {
    G3DOBJECT *obj = g3dscene_getSelectedObject ( gui->sce );

    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    g3dcamera_project ( uvme->cam, engine_flags );
    glMatrixMode ( GL_MODELVIEW );
    glLoadIdentity ( );

    glEnable ( GL_COLOR_MATERIAL );
    glColor3ub ( 0xFF, 0xFF, 0xFF );

    glBegin ( GL_LINE_LOOP );
    glVertex3f   ( uvme->cam->canevas.x, 
                   uvme->cam->canevas.y, 0.0f );
    glVertex3f   ( uvme->cam->canevas.x + 
                   uvme->cam->canevas.width, 
                   uvme->cam->canevas.y, 0.0f );
    glVertex3f   ( uvme->cam->canevas.x + 
                   uvme->cam->canevas.width, 
                   uvme->cam->canevas.y + 
                   uvme->cam->canevas.height, 0.0f );
    glVertex3f   ( uvme->cam->canevas.x, 
                   uvme->cam->canevas.y + 
                   uvme->cam->canevas.height, 0.0f );
    glEnd ( );

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );
        G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

        if ( tex ) {
            G3DMATERIAL *mat = tex->mat;

            if ( mat ) {
                G3DCHANNEL *chn = &mat->diffuse;

                if ( chn->flags & USEIMAGECOLOR ) {
                    if ( chn->image ) {
                        float imgratio = chn->image->width / 
                                         chn->image->height;
                        float x1 =   uvme->cam->canevas.x,
                              y1 =   uvme->cam->canevas.y,
                              x2 = ( uvme->cam->canevas.x + 
                                     uvme->cam->canevas.width ),
                              y2 = ( uvme->cam->canevas.y + 
                                     uvme->cam->canevas.height );

                        glEnable      ( GL_TEXTURE_2D );
                        glBindTexture ( GL_TEXTURE_2D, chn->image->id );

                        /*common_g3duiuvmapeditor_setCanevas ( uvme );*/

                        glBegin ( GL_QUADS );
                        glTexCoord2f ( 0, 0 );
                        glVertex3f   ( x1, y1, 0.0f );
                        glTexCoord2f ( 1, 0 );
                        glVertex3f   ( x2, y1, 0.0f );
                        glTexCoord2f ( 1, 1 );
                        glVertex3f   ( x2, y2, 0.0f );
                        glTexCoord2f ( 0, 1 );
                        glVertex3f   ( x1, y2, 0.0f );
                        glEnd ( );

                        glDisable ( GL_TEXTURE_2D );
                    }
                }
            }
        }

        if ( uvmap ) {
             LIST *ltmpfac = mes->lfac;

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
                        int32_t xi = uvs->veruv[i].u * uvme->cam->canevas.width,
                                yi = uvs->veruv[i].v * uvme->cam->canevas.height;
                        int32_t xn = uvs->veruv[n].u * uvme->cam->canevas.width,
                                yn = uvs->veruv[n].v * uvme->cam->canevas.height;

                        glVertex2f ( uvme->cam->canevas.x + xi, 
                                     uvme->cam->canevas.y + yi );
                        glVertex2f ( uvme->cam->canevas.x + xn, 
                                     uvme->cam->canevas.y + yn );

                    }
                    glEnd ( );

                    if ( gui->flags & VIEWVERTEX ) { 
                        glPointSize ( 3.0f );

                        glBegin ( GL_POINTS );
                        for ( i = 0x00; i < fac->nbver; i++ ) {
                            int32_t xi = uvs->veruv[i].u * uvme->cam->canevas.width,
                                    yi = uvs->veruv[i].v * uvme->cam->canevas.height;

                            uvs->veruv[i].x = uvme->cam->canevas.x + xi; /* for picking */
                            uvs->veruv[i].y = uvme->cam->canevas.y + yi; /* for picking */

                            if ( uvs->veruv[i].flags & UVSELECTED ) {
                                glColor3ub ( 0xFF, 0x00, 0x00 );
                            } else {
                                glColor3ub ( 0x00, 0x00, 0xFF );
                            }

                            glVertex2f ( uvs->veruv[i].x, 
                                         uvs->veruv[i].y );
                        }
                        glEnd ( );
                    }
                }

                ltmpfac = ltmpfac->next;
            }
        }
    }

    if ( mou && mou->draw ) {
        mou->draw ( mou, gui->sce, engine_flags );
    }
}

/******************************************************************************/
void common_g3duiuvmapeditor_sizeGL ( G3DUIUVMAPEDITOR *uvme, 
                                      uint32_t          width, 
                                      uint32_t          height ) {
    glViewport ( 0, 0, width, height );

    uvme->arearec.width   = width;
    uvme->arearec.height  = height;
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

    uvme->cam->canevas.width  = uvme->arearec.width;
    uvme->cam->canevas.height = uvme->arearec.height;
}
