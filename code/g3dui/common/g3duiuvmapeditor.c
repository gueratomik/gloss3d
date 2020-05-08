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
    uvme->cam.obj.pos.x -= ( ( float ) ( x - xold ) * 0.005f );
    uvme->cam.obj.pos.y += ( ( float ) ( y - yold ) * 0.005f );

    g3dobject_updateMatrix ( ( G3DOBJECT * ) &uvme->cam );

    common_g3duiuvmapeditor_setCanevas ( uvme );
}


/******************************************************************************/
void common_g3duiuvmapeditor_moveForward ( G3DUIUVMAPEDITOR *uvme, 
                                           int32_t           x, 
                                           int32_t           xold ) {
    uvme->cam.obj.sca.z -= ( ( float ) ( x - xold ) * 0.01f );

    uvme->cam.obj.sca.x = uvme->cam.obj.sca.z;
    uvme->cam.obj.sca.y = uvme->cam.obj.sca.z;

    g3dobject_updateMatrix ( ( G3DOBJECT * ) &uvme->cam );

    common_g3duiuvmapeditor_setCanevas ( uvme );
}

/******************************************************************************/
void common_g3duiuvmapeditor_setCanevas ( G3DUIUVMAPEDITOR *uvme ) {

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

}

/******************************************************************************/
void common_g3duiuvmapeditor_init ( G3DUIUVMAPEDITOR *uvme, 
                                    uint32_t          width,
                                    uint32_t          height ) {
    uvme->buttonID = -1;

    /*** as we use an identity projection matrix, the coorinates system will ***/
    /*** be from -1.0f to 1.0f. So we have to move our UVMAP to the center ***/
    /*** of this coordinates system by shifting it by 0.5f ***/
    uvme->cam.obj.pos.x =  0.5f;
    uvme->cam.obj.pos.y =  0.5f;
    uvme->cam.obj.pos.z =  0.0f;

    uvme->cam.obj.rot.x = 0.0f;
    uvme->cam.obj.rot.y = 0.0f;
    uvme->cam.obj.rot.z = 0.0f;

    uvme->cam.obj.sca.x = 1.0f;
    uvme->cam.obj.sca.y = 1.0f;
    uvme->cam.obj.sca.z = 1.0f;

    uvme->cam.obj.flags = CAMERAORTHO;

    uvme->flags = VIEWVERTEXUV;

    g3dobject_updateMatrix ( ( G3DOBJECT * ) &uvme->cam );

    /*** projection matrix will never change ***/
    g3dcore_identityMatrix ( uvme->cam.pmatrix );
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
    g3dcamera_project ( &uvme->cam, 0x00  );

    glMatrixMode ( GL_MODELVIEW );
    glLoadIdentity ( );
    g3dcamera_view ( &uvme->cam, 0x00  );

    glEnable ( GL_COLOR_MATERIAL );
    glColor3ub ( 0xFF, 0xFF, 0xFF );

    /*glBegin ( GL_LINE_LOOP );
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
    glEnd ( );*/

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );
        G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );


        g3dmesh_drawUVs ( mes, engine_flags );

#ifdef UNUSED
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
#endif
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
}
