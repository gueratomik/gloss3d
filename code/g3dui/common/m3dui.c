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


/******************************************************************************/
void m3dui_init ( M3DUI *mui, G3DUI *gui )  {
    mui->gui          = gui;
    mui->engine_flags = VIEWVERTEXUV;

    SHOWCHANNEL(mui->engine_flags,DIFFUSECHANNELID);
}

/******************************************************************************/
uint64_t m3dui_setMouseTool ( M3DUI          *mui, 
                              G3DUIMOUSETOOL *mou ) {
    G3DUI        *gui = mui->gui;
    uint64_t ret = 0x00;

    /*** Call the mouse tool initialization function once. This ***/
    /*** can be used by this function to initialize some values ***/
    if ( mou ) {
        if ( mou->tool->init ) {
            ret = mou->tool->init ( mou->tool,
                                    gui->sce,
                                    gui->urm,
                                    mui->engine_flags );
        }

        if ( ( mou->tool->flags & MOUSETOOLNOCURRENT ) == 0x00 ) {
            mui->curmou = ( G3DUIMOUSETOOL * ) mou;
        }
    } else {
        mui->curmou = NULL;
    }

    return ret;
}

/******************************************************************************/
G3DCHANNEL *m3dui_getWorkingChannel ( M3DUI *mui ) {
    G3DUI        *gui = mui->gui;
    G3DOBJECT *obj = g3dscene_getLastSelected ( mui->gui->sce );

    if ( obj ) {
        if ( obj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            /*** try the first texture in case no texture is selected ***/
            if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

            if ( tex ) {
                G3DMATERIAL *mat = tex->mat;

                if ( mat ) {
                    uint32_t channelID = GETCHANNEL(mui->engine_flags);
                    G3DCHANNEL *chn = g3dmaterial_getChannelByID(mat,channelID);

                    if ( chn ) {
                        return chn;
                    }
                }
            }
        }
    }

    return NULL;
}

/******************************************************************************/
G3DIMAGE *m3dui_getWorkingImage ( M3DUI *mui ) {
    G3DCHANNEL *chn = m3dui_getWorkingChannel ( mui );

    if ( chn ) {
        if ( chn->flags & USEIMAGECOLOR ) {
            if ( chn->image ) {
                return chn->image;
            }
        }
    }

    return NULL;
}

/******************************************************************************/
uint64_t m3dui_setUVMouseTool ( M3DUI          *mui, 
                                G3DCAMERA      *cam, 
                                G3DUIMOUSETOOL *mou ) {
    G3DUI *gui = mui->gui;
    uint32_t msk = 0x00;

    /*** Call the mouse tool initialization function once. This ***/
    /*** can be used by this function to initialize some values ***/
    if ( mou ) {
        if ( mou->tool->init ) {
            msk = mou->tool->init ( mou->tool, 
                                    gui->sce,
                                    gui->urm, 
                                    mui->engine_flags );
        }

        if ( ( mou->flags & MOUSETOOLNOCURRENT ) == 0x00 ) {
            mui->curmou = mou;
        }
    } else {
        mui->curmou = NULL;
    }


    return msk;
}

/******************************************************************************/
uint64_t m3dui_fillWithColor ( M3DUI   *mui, 
                               uint32_t color ) {
    G3DIMAGE *img = m3dui_getWorkingImage ( mui );

    if ( img ) {
        unsigned char R = ( color & 0x00FF0000 ) >> 0x10,
                      G = ( color & 0x0000FF00 ) >> 0x08,
                      B = ( color & 0x000000FF ) >> 0x00;
        uint32_t i, j;

        for ( i = 0x00; i < img->height; i++ ) {
            for ( j = 0x00; j < img->width; j++ ) {
                uint32_t offset = ( i * img->width ) + j;

                if ( mui->mask[offset] ) {
                    switch ( img->bytesPerPixel ) {
                        case 0x03 : {
                            unsigned char (*buffer)[0x03] = ( unsigned char (*)[0x03] ) img->data;

                            buffer[offset][0x00] = R;
                            buffer[offset][0x01] = G;
                            buffer[offset][0x02] = B;
                        } break;

                        default :
                        break;
                    }
                }
            }
        }

        if ( img->flags & GLIMAGE ) g3dimage_bind ( img );

        img->flags |= ALTEREDIMAGE;
    }

    return REDRAWVIEW;
}

/******************************************************************************/
void m3dui_resizeBuffers ( M3DUI *mui ) {
    G3DIMAGE *img = m3dui_getWorkingImage ( mui );
    G3DUIMOUSETOOL *mou = g3dui_getMouseTool ( mui->gui,
                                               SELECTTOOL );

    if ( mou ) {
        M3DMOUSETOOL *seltool = ( M3DMOUSETOOL * ) mou->tool;

        seltool->obj->reset ( seltool->obj, mui->engine_flags );

        if ( img ) {
            if ( img->width && img->height ) {
                uint32_t size = img->width *
                                img->height;
                mui->mask    = realloc ( mui->mask   , size );
                mui->zbuffer = realloc ( mui->zbuffer, size );

                memset ( mui->mask, 0xFF, size );
            }
        }
    }
}

/******************************************************************************/
uint64_t m3dui_uvset2fac ( M3DUI *mui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( mui->gui->sce );

    if ( obj ) {

        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            if ( tex ) {
                G3DUVMAP *curmap = tex->map;

                if ( curmap ) {
                    LIST *ltmpfac = mes->lfac;
                    LIST *lselold, *lselnew;

                    lselold = list_copy ( mes->lselfac );

                    g3dmesh_unselectAllFaces ( mes );

                    while ( ltmpfac ) {
                        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                        G3DUVSET *uvset = g3dface_getUVSet ( fac, curmap );
                        int i;

                        if ( uvset->flags & UVSETSELECTED ) {
                            if ( ( fac->flags & FACESELECTED ) == 0x00 ) {
                                g3dmesh_selectFace ( mes, fac );
                            }
                        }

                        ltmpfac = ltmpfac->next;
                    }

                    lselnew = list_copy ( mes->lselfac );

                    /*** remember selection ***/
                    g3durm_mesh_pickFaces  ( mui->gui->urm,
                                             mui->gui->sce,
                                             mes,
                                             lselold,
                                             lselnew,
                                             VIEWFACE,
                                             REDRAWVIEW |
                                             REDRAWUVMAPEDITOR );
                }
            }
        }
    }

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t m3dui_fac2uvset ( M3DUI *mui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( mui->gui->sce );

    if ( obj ) {

        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            if ( tex ) {
                G3DUVMAP *curmap = tex->map;

                if ( curmap ) {
                    LIST *ltmpfac = mes->lfac;
                    LIST *lselold, *lselnew;

                    lselold = list_copy ( curmap->lseluvset );

                    g3duvmap_unselectAllUVSets ( curmap );

                    while ( ltmpfac ) {
                        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                        int i;

                        if ( fac->flags & FACESELECTED ) {
                            G3DUVSET *uvset = g3dface_getUVSet ( fac, curmap );

                            if ( ( uvset->flags & UVSETSELECTED ) == 0x00 ) {
                                g3duvmap_selectUVSet ( curmap, uvset );
                            }
                        }

                        ltmpfac = ltmpfac->next;
                    }

                    lselnew = list_copy ( curmap->lseluvset );

                    /*** remember selection ***/
                    g3durm_uvmap_pickUVSets  ( /*mui->uvurm*/
                                               mui->gui->urm,
                                               mui->gui->sce,
                                               curmap,
                                               lselold,
                                               lselnew,
                                               VIEWFACEUV,
                                               REDRAWVIEW |
                                               REDRAWUVMAPEDITOR );
                }
            }
        }
    }

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t m3dui_uv2ver ( M3DUI *mui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( mui->gui->sce );

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            if ( tex ) {
                G3DUVMAP *curmap = tex->map;

                if ( curmap ) {
                    LIST *ltmpfac = mes->lfac;
                    LIST *lselold, *lselnew;

                    lselold = list_copy ( mes->lselver );

                    g3dmesh_unselectAllVertices ( mes );

                    while ( ltmpfac ) {
                        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                        G3DUVSET *uvset = g3dface_getUVSet ( fac, curmap );
                        int i;

                        for ( i = 0x00; i < uvset->nbuv; i++ ) {
                            G3DUV *uv = &uvset->veruv[i];

                            if ( uv->flags & UVSELECTED ) {
                                G3DVERTEX *ver = fac->ver[i];

                                if ( ( ver->flags & VERTEXSELECTED ) == 0x00 ) {
                                    g3dmesh_selectVertex ( mes, ver );
                                }
                            }
                        }

                        ltmpfac = ltmpfac->next;
                    }

                    lselnew = list_copy ( mes->lselver );

                    /*** remember selection ***/
                    g3durm_mesh_pickVertices  ( mui->gui->urm,
                                                mui->gui->sce,
                                                mes,
                                                lselold,
                                                lselnew,
                                                VIEWVERTEX,
                                                REDRAWVIEW |
                                                REDRAWUVMAPEDITOR );
                }
            }
        }
    }

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t m3dui_ver2uv ( M3DUI *mui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( mui->gui->sce );

    if ( obj ) {

        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            if ( tex ) {
                G3DUVMAP *curmap = tex->map;

                if ( curmap ) {
                    LIST *ltmpfac = mes->lfac;
                    LIST *lselold, *lselnew;

                    lselold = list_copy ( curmap->lseluv );

                    g3duvmap_unselectAllUVs ( curmap );

                    while ( ltmpfac ) {
                        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                        G3DUVSET *uvset = g3dface_getUVSet ( fac, curmap );
                        int i;

                        for ( i = 0x00; i < fac->nbver; i++ ) {
                            G3DVERTEX *ver = fac->ver[i];

                            if ( ver->flags & VERTEXSELECTED ) {
                                G3DUV *uv = &uvset->veruv[i];

                                if ( ( uv->flags & UVSELECTED ) == 0x00 ) {
                                    g3duvmap_selectUV ( curmap, uv );
                                }
                            }
                        }

                        ltmpfac = ltmpfac->next;
                    }

                    lselnew = list_copy ( curmap->lseluv );

                    /*** remember selection ***/
                    g3durm_uvmap_pickUVs  ( /*mui->uvurm*/
                                            mui->gui->urm, 
                                            mui->gui->sce,
                                            curmap,
                                            lselold,
                                            lselnew,
                                            VIEWVERTEXUV,
                                            REDRAWVIEW |
                                            REDRAWUVMAPEDITOR );
                }
            }
        }
    }

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t m3dui_undo ( M3DUI *mui ) {
    return g3durmanager_undo ( mui->gui->urm, mui->engine_flags ) |
           REDRAWUVMAPEDITOR; /*** some action dont call it, we'll find out later ***/
}

/******************************************************************************/
uint64_t m3dui_redo ( M3DUI *mui ) {
    return g3durmanager_redo ( mui->gui->urm, mui->engine_flags )|
           REDRAWUVMAPEDITOR; /*** some action dont call it, we'll find out later ***/
}
