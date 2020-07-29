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

static int pen_tool ( G3DMOUSETOOL *mou, 
                      G3DSCENE     *sce, 
                      G3DCAMERA    *cam,
                      G3DURMANAGER *urm,
                      uint64_t engine_flags, 
                      G3DEvent     *event );
static int eraser_tool ( G3DMOUSETOOL *mou, 
                         G3DSCENE     *sce, 
                         G3DCAMERA    *cam,
                         G3DURMANAGER *urm,
                         uint64_t engine_flags, 
                         G3DEvent     *event );
static void selector_draw ( G3DMOUSETOOL *gtool, 
                            G3DSCENE     *sce, 
                            uint64_t engine_flags );

/******************************************************************************/
L3DMOUSETOOLSELECTRANDOM *l3dmousetoolselectrandom_new ( ) {
    uint32_t structsize = sizeof ( L3DMOUSETOOLSELECTRANDOM );
    void *memarea = calloc ( 0x01, structsize );
    L3DMOUSETOOLSELECTRANDOM *sr =  ( L3DMOUSETOOLSELECTRANDOM * ) memarea;
    L3DMOUSETOOL *ltool = ( L3DMOUSETOOL * ) sr;

    if ( sr == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    l3dmousetool_init ( ltool,
                        SELECTRANDOMTOOL,
                        's',
                        NULL,
                        NULL,
                        selector_draw,
                        pen_tool,
                        MOUSETOOLREADONLY );

    ltool->obj = l3dselector_new ( );

    return sr;
}

/******************************************************************************/
L3DMOUSETOOLPEN *l3dmousetoolpen_new ( ) {
    uint32_t structsize = sizeof ( L3DMOUSETOOLPEN );
    void *memarea = calloc ( 0x01, structsize );
    L3DMOUSETOOLPEN *pn =  ( L3DMOUSETOOLPEN * ) memarea;
    L3DMOUSETOOL *ltool = ( L3DMOUSETOOL * ) pn;

    if ( pn == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    l3dmousetool_init ( ltool,
                        PENTOOL,
                        's',
                        NULL,
                        NULL,
                        NULL,
                        pen_tool,
                        MOUSETOOLREADWRITE );

    ltool->obj = l3dbasepen_new ( );

    return pn;
}

/******************************************************************************/
L3DMOUSETOOLERASER *l3dmousetooleraser_new ( ) {
    uint32_t structsize = sizeof ( L3DMOUSETOOLERASER );
    void *memarea = calloc ( 0x01, structsize );
    L3DMOUSETOOLERASER *er =  ( L3DMOUSETOOLERASER * ) memarea;
    L3DMOUSETOOL *ltool = ( L3DMOUSETOOL * ) er;

    if ( er == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    l3dmousetool_init ( ltool,
                        ERASERTOOL,
                        's',
                        NULL,
                        NULL,
                        NULL,
                        eraser_tool,
                        MOUSETOOLREADWRITE );

    ltool->obj = l3dbasepen_new ( );


    return er;
}

/******************************************************************************/
static void selector_draw ( G3DMOUSETOOL *gtool, 
                            G3DSCENE     *sce, 
                            uint64_t engine_flags ) {
    L3DMOUSETOOLSELECTRANDOM *sr =  ( L3DMOUSETOOLSELECTRANDOM * ) gtool;
    L3DMOUSETOOL *ltool = ( L3DMOUSETOOL * ) sr;
    L3DSELECTOR *sel = ( L3DSELECTOR * ) ltool->obj;
    LIST *ltmplin = sel->llines;
    double sx[0x04], sy[0x04], sz[0x04];

    glPushAttrib ( GL_ENABLE_BIT ); 

    glColor3ub ( 0xFF, 0xFF, 0xFF );
    glLineStipple ( 1, 0x5555 );
    glEnable ( GL_LINE_STIPPLE );
    glBegin ( GL_LINES );
    while ( ltmplin ) {
        L3DSELECTORLINE *lin = ( L3DSELECTORLINE * ) ltmplin->data;

        glVertex3f ( lin->srcpt->u, lin->srcpt->v, 0.0f );
        glVertex3f ( lin->dstpt->u, lin->dstpt->v, 0.0f );

        ltmplin = ltmplin->next;
    }
    glEnd ( );

    ltmplin = sel->llines;

    glColor3ub ( 0x00, 0x00, 0x00 );
    glLineStipple ( 1, 0x6666 );
    glEnable ( GL_LINE_STIPPLE );
    glBegin ( GL_LINES );
    while ( ltmplin ) {
        L3DSELECTORLINE *lin = ( L3DSELECTORLINE * ) ltmplin->data;

        glVertex3f ( lin->srcpt->u, lin->srcpt->v, 0.0f );
        glVertex3f ( lin->dstpt->u, lin->dstpt->v, 0.0f );

        ltmplin = ltmplin->next;
    }
    glEnd ( );

    glPopAttrib ( );
}

/******************************************************************************/
int basepen_tool ( G3DMOUSETOOL *mou, 
                   G3DSCENE     *sce, 
                   G3DCAMERA    *cam,
                   G3DURMANAGER *urm,
                   L3DPATTERN   *pattern,
                   uint32_t      fgcolor,
                   uint32_t      bgcolor,
                   uint64_t engine_flags, 
                   G3DEvent     *event ) {
    /*** selection rectangle coords ***/
    static GLdouble MVX[0x10], PJX[0x10];
    static GLint VPX[0x04];
    static G3DOBJECT *obj;
    static G3DMESH *mes;
    L3DMOUSETOOL *ltool = ( L3DMOUSETOOL * ) mou;
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
                        uint32_t channelID = GETCHANNEL(engine_flags);
                        G3DCHANNEL *chn = g3dmaterial_getChannelByID(mat,channelID);

                        if ( chn ) {
                            if ( chn->flags & USEIMAGECOLOR ) {
                                if ( chn->image ) {
                                    chn->image->flags |= ALTEREDIMAGE;

                                    gluUnProject ( bev->x, 
                                                   VPX[0x03] - bev->y, 
                                                   0.0f,
                                                   MVX,
                                                   PJX,
                                                   VPX,
                                                  &mx,
                                                  &my,
                                                  &mz );

                                    memset ( mou->zbuffer, 
                                             0x00, 
                                             chn->image->width * 
                                             chn->image->height);

                                    ltool->obj->press ( ltool->obj,
                                                        pattern,
                                                        fgcolor,
                                                        bgcolor,
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
            }

            oldx = bev->x;
            oldy = bev->y;
        } return REDRAWVIEW;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;
            int32_t subx, suby, subw, subh;

            subRect[0x02] = mev->x;
            subRect[0x03] = mev->y;

            obj = ( G3DOBJECT * ) g3dscene_getLastSelected ( sce );

            if ( obj ) {
                if ( obj->type & MESH ) {
                    G3DMESH *mes = ( G3DMESH * ) obj;
                    G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );
                    G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

                    if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

                    if ( tex ) {
                        G3DMATERIAL *mat = tex->mat;
                        uint32_t channelID = GETCHANNEL(engine_flags);
                        G3DCHANNEL *chn = g3dmaterial_getChannelByID(mat,channelID);

                        if ( chn ) {
                            if ( chn->flags & USEIMAGECOLOR ) {
                                if ( chn->image ) {
                                    G3DIMAGE *image = chn->image;
                                    uint32_t retval = 0x00;
                                    uint32_t l3dFlags = 0x00;

                                    chn->image->flags |= ALTEREDIMAGE;

                                    l3dFlags |= ( mev->state & G3DButton1Mask ) ? L3DBUTTON1PRESSED : 0x00;

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

                                        oldVector[0x00] = newVector[0x00];
                                        oldVector[0x01] = newVector[0x01];
                                    }

                                    retval = ltool->obj->move ( ltool->obj,
                                                                pattern,
                                                                fgcolor,
                                                                bgcolor,
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
                                                                l3dFlags );

                                    if ( retval & L3DUPDATESUBIMAGE ) {
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
                        uint32_t channelID = GETCHANNEL(engine_flags);
                        G3DCHANNEL *chn = g3dmaterial_getChannelByID(mat,channelID);

                        if ( chn ) {
                            if ( chn->flags & USEIMAGECOLOR ) {
                                if ( chn->image ) {
                                    chn->image->flags |= ALTEREDIMAGE;

                                    gluUnProject ( bev->x, 
                                                   VPX[0x03] - bev->y, 
                                                   0.0f,
                                                   MVX,
                                                   PJX,
                                                   VPX,
                                                  &mx,
                                                  &my,
                                                  &mz );

                                    ltool->obj->release ( ltool->obj,
                                                          pattern,
                                                          fgcolor,
                                                          bgcolor,
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

                        if ( channelID == DISPLACEMENTCHANNELID ) {
                        /** TODO: do this only for subdivided meshes ***/
                            g3dmaterial_updateMeshes ( mat, 
                                                       sce, 
                                                       engine_flags );
                        }
                    }
                }
            }

            /*obj = NULL; */
        } return REDRAWVIEW;

        default :
        break;
    }

    return 0x00;
}

/******************************************************************************/
static int pen_tool ( G3DMOUSETOOL *mou, 
                      G3DSCENE     *sce, 
                      G3DCAMERA    *cam,
                      G3DURMANAGER *urm,
                      uint64_t engine_flags, 
                      G3DEvent     *event ) {
    L3DSYSINFO *sysinfo = l3dsysinfo_get ( );

    basepen_tool ( mou, 
                   sce,
                   cam,
                   urm,
                   sysinfo->pattern,
                   sysinfo->fgcolor,
                   sysinfo->bgcolor, 
                   engine_flags, 
                   event );
}

/******************************************************************************/
static int eraser_tool ( G3DMOUSETOOL *mou, 
                         G3DSCENE     *sce, 
                         G3DCAMERA    *cam,
                         G3DURMANAGER *urm,
                         uint64_t engine_flags, 
                         G3DEvent     *event ) {
    L3DSYSINFO *sysinfo = l3dsysinfo_get ( );

    basepen_tool ( mou, 
                   sce,
                   cam,
                   urm,
                   sysinfo->pattern,
                   0xFFFFFFFF,
                   0x00000000, 
                   engine_flags, 
                   event );
}
