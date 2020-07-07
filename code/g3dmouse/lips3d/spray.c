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
SPRAY *spray_new ( ) {
    uint32_t structsize = sizeof ( SPRAY );
    SPRAY *spr =  ( SPRAY * ) calloc ( 0x01, structsize );

    if ( spr == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    spr->tool = l3dspraytool_new ( );

    return spr;
}

/******************************************************************************/
uint32_t spray_init ( G3DMOUSETOOL *mou, 
                      G3DSCENE     *sce, 
                      G3DCAMERA    *cam,
                      G3DURMANAGER *urm,
                      uint32_t      engine_flags ) {
    if ( mou->data ) {
        SPRAY *cm = mou->data;
    } else {
        mou->data = spray_new ( );
    }

    return 0x00;
}

/******************************************************************************/
void spray_draw ( G3DMOUSETOOL *mou, G3DSCENE *sce, uint32_t flags ) {
        SPRAY *spr = mou->data;

}

/******************************************************************************/
int spray_tool ( G3DMOUSETOOL *mou, 
                 G3DSCENE     *sce, 
                 G3DCAMERA    *cam,
                 G3DURMANAGER *urm, 
                 uint32_t      flags, 
                 G3DEvent     *event ) {
    /*** selection rectangle coords ***/
    static GLdouble MVX[0x10], PJX[0x10];
    static GLint VPX[0x04];
    static G3DOBJECT *obj;
    static G3DMESH *mes;
    SPRAY *spr = mou->data;
    double mx, my, mz;
    static int subRect[0x04];
    static double oldx, oldy;

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            subRect[0x00] = subRect[0x02] = bev->x;
            subRect[0x01] = subRect[0x03] = bev->y;

            glGetDoublev  ( GL_MODELVIEW_MATRIX,  MVX );
            glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
            glGetIntegerv ( GL_VIEWPORT, VPX );

            obj = ( G3DOBJECT * ) g3dscene_getLastSelected ( sce );

            if ( obj ) {
                if ( obj->type & MESH ) {
                    G3DMESH *mes = ( G3DMESH * ) obj;
                    G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );
                    G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

                    if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

                    if ( tex ) {
                        G3DMATERIAL *mat = tex->mat;
                        G3DCHANNEL *chn = &mat->diffuse;

                        if ( chn->flags & USEIMAGECOLOR ) {
                            if ( chn->image ) {
                                gluUnProject ( bev->x, 
                                               VPX[0x03] - bev->y, 
                                               0.0f,
                                               MVX,
                                               PJX,
                                               VPX,
                                              &mx,
                                              &my,
                                              &mz );

                                spr->tool->init ( spr->tool,
                                                  mx * chn->image->width,
                                                  my * chn->image->height,
                                                  chn->image->data,
                                                  chn->image->width,
                                                  chn->image->height,
                                                  chn->image->bytesPerPixel * 0x08,
                                                  mou->mask,
                                                  mou->zbuffer,
                                                  0x00 );

                                g3dimage_bind ( chn->image );
                            }
                        }
                    }
                }
            }

            oldx = bev->x;
            oldy = bev->y;
        } return REDRAWVIEW;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;
            int32_t subx, suby, subw, subh;

            subRect[0x02] = mev->x;
            subRect[0x03] = mev->y;

            if ( obj ) {
                if ( obj->type & MESH ) {
                    G3DMESH *mes = ( G3DMESH * ) obj;
                    G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );
                    G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

                    if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

                    if ( tex ) {
                        G3DMATERIAL *mat = tex->mat;
                        G3DCHANNEL *chn = &mat->diffuse;

                        if ( chn->flags & USEIMAGECOLOR ) {
                            if ( chn->image ) {
                                G3DIMAGE *image = chn->image;

                                gluUnProject ( mev->x, 
                                               VPX[0x03] - mev->y, 
                                               0.0f,
                                               MVX,
                                               PJX,
                                               VPX,
                                              &mx,
                                              &my,
                                              &mz );

                                if ( mev->state & G3DButton1Mask ) {
                                    static double oldVector[0x02];
                                           double newVector[0x02];

                                    newVector[0x00] = mev->x - oldx;
                                    newVector[0x01] = mev->y - oldy;

                                    /*** If we are going in the opposite direction, then reset the zbuffer. ***/
                                    /*** This allows for a second pass of the pencil ***/
                                    if ( ( ( newVector[0x00] * 
                                             oldVector[0x00] ) +
                                           ( newVector[0x01] * 
                                             oldVector[0x01] ) ) < 0x00 ) {
                                        memset ( mou->zbuffer, 
                                                 0x00, 
                                                 image->width * image->height );
                                    }

                                    spr->tool->paint ( spr->tool,
                                                       mx * image->width,
                                                       my * image->height,
                                                       image->data,
                                                       image->width,
                                                       image->height,
                                                       image->bytesPerPixel * 0x08,
                                                       mou->mask,
                                                       mou->zbuffer,
                                                      &subx,
                                                      &suby,
                                                      &subw,
                                                      &subh,
                                                       0x00 );

                                    oldVector[0x00] = newVector[0x00];
                                    oldVector[0x01] = newVector[0x01];

                                    if ( ( subx < image->width  ) &&
                                         ( suby < image->height ) ) {
                                        glEnable ( GL_TEXTURE_2D );
                                        glBindTexture ( GL_TEXTURE_2D, image->id );
                                        glActiveTexture( image->id ); 
                                        glPixelStorei ( GL_UNPACK_ROW_LENGTH, image->width );
                                        glTexSubImage2D ( GL_TEXTURE_2D, 
                                                          0x00,
                                                          subx,
                                                          suby,
                                                          subw,
                                                          subh,
                                                          GL_RGB,
                                                          GL_UNSIGNED_BYTE,
                                                          image->data + ( suby * image->bytesPerLine ) + ( subx * image->bytesPerPixel ) );
                                        glBindTexture ( GL_TEXTURE_2D, 0x00 );
                                        glDisable ( GL_TEXTURE_2D );
                                    }
                                }
                            }
                        }
                    }
                }
            }

            subRect[0x00] = subRect[0x02];
            subRect[0x01] = subRect[0x03];

            oldx = mev->x;
            oldy = mev->y;
        } return REDRAWUVMAPEDITOR;

        case G3DButtonRelease : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            if ( obj ) {
                if ( obj->type & MESH ) {
                    G3DMESH *mes = ( G3DMESH * ) obj;
                    G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );
                    G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

                    if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

                    if ( tex ) {
                        G3DMATERIAL *mat = tex->mat;
                        G3DCHANNEL *chn = &mat->diffuse;

                        if ( chn->flags & USEIMAGECOLOR ) {
                            if ( chn->image ) {
                                gluUnProject ( bev->x, 
                                               VPX[0x03] - bev->y, 
                                               0.0f,
                                               MVX,
                                               PJX,
                                               VPX,
                                              &mx,
                                              &my,
                                              &mz );

                                spr->tool->done ( spr->tool,
                                                  mx * chn->image->width,
                                                  my * chn->image->height,
                                                  chn->image->data,
                                                  chn->image->width,
                                                  chn->image->height,
                                                  chn->image->bytesPerPixel * 0x08,
                                                  mou->mask,
                                                  mou->zbuffer,
                                                  0x00 );

                                g3dimage_bind ( chn->image );
                            }
                        }
                    }
                }
            }

            obj = NULL;
        } return REDRAWVIEW;

        default :
        break;
    }

    return 0x00;
}
