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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/******************************************************************************/
#include <config.h>
#include <g3dui.h>

/******************************************************************************/
/*** Apply picking policy ***/
static void pickedobject_parse ( PICKEDOBJECT *pob, G3DSCENE *sce, 
                                                    G3DURMANAGER *urm,
                                                    uint32_t pick_flags,
                                                    uint64_t engine_flags ) {
    G3DOBJECT *obj = pob->obj;
    G3DOBJECT *oldobj = g3dscene_getLastSelected ( sce );
    LIST *lselold, *lselnew;

    /*** When using this function during mouse motion, we pass NULL as urm ***/
    /*** so we need to check the value ***/
    if ( urm ) {
        /*** only pick once ***/
        if (  obj != oldobj ) {
            if ( pob->picked == NAMERECTHIT ) {
                lselold = list_copy ( sce->lsel );

                /*** Press CTRL to select multiple objects ***/
                if ( ( pick_flags & PICKEOBJECTKEEPALL ) == 0x00 ) {
                    g3dscene_unselectAllObjects ( sce, engine_flags );
                }

                g3dscene_selectObject ( sce, obj, engine_flags );

                lselnew = list_copy ( sce->lsel );

                /*** remember selection ***/
                /*
                 * Note: force the VIEWOBJECT flags because object can be picked from
                 * the object list even if we are in sub-object edition mode.
                 */
                g3durm_scene_pickObject  ( urm, sce, lselold, 
                                                     lselnew, 
                                                     VIEWOBJECT, 
                                                     REDRAWVIEW | 
                                                     REDRAWLIST );
            }
        }
    }

    /*** If we clicked [+] or [-], develop or collapse ***/
    if ( pob->picked == EXPANDRECTHIT ) {
        if ( obj->flags & OBJECTCOLLAPSED ) {
            obj->flags &= (~OBJECTCOLLAPSED);
        } else {
            obj->flags |=  (OBJECTCOLLAPSED);
        }
    }

    if ( pob->picked == ACTIVERECTHIT ) {
        if ( g3dobject_isActive ( obj ) == 0x00 ) {
            g3dobject_activate ( obj, engine_flags );
        } else {
            g3dobject_deactivate ( obj, engine_flags );
        }
    }

    if ( pob->picked == VISIBLERECTHIT ) {
        if ( obj->flags & OBJECTINVISIBLE ) {
            obj->flags &= (~OBJECTINVISIBLE);
        } else {
            obj->flags |=  (OBJECTINVISIBLE);
        }
    }

    if ( pob->picked == TEXTURERECTHIT ) {
        G3DMESH *mes = ( G3DMESH * ) pob->obj;
        G3DTEXTURE *tex = pob->tex;

        /*** we check the texture does not already belong to the list of ***/
        /*** selected texture. We could also check the "selected" flag,  ***/
        /*** but I think it is less reliable ***/
        if ( list_seek ( mes->lseltex, tex ) == NULL ) {
            g3dmesh_selectTexture ( mes, tex );
        }
    }

    if ( pob->picked == UVMAPRECTHIT ) {
        G3DMESH  *mes = ( G3DMESH * ) pob->obj;
        G3DUVMAP *uvmap = pob->uvmap;

        /*** we check the texture does not already belong to the list of ***/
        /*** selected texture. We could also check the "selected" flag,  ***/
        /*** but I think it is less reliable ***/
        /*if ( list_seek ( mes->lseluvmap, uvmap ) == NULL ) {*/
            g3dmesh_unselectAllUVMaps ( mes );
            g3dmesh_selectUVMap ( mes, uvmap );
        /*}*/
    }

    if ( pob->picked == TAGRECTHIT ) {
        G3DTAG *tag = pob->tag;

        obj->seltag = pob->tag;
    }
}

/******************************************************************************/
static uint32_t inRectangle ( G3DUIRECTANGLE *rec, uint32_t xm, uint32_t ym ) {
    uint32_t x1 = rec->x,
             x2 = rec->x + rec->width,
             y1 = rec->y,
             y2 = rec->y + rec->height;

    if ( ( xm > x1 ) && ( xm < x2 ) &&
         ( ym > y1 ) && ( ym < y2 ) ) {

        return 0x01;
    }

    return 0x00;
}

/******************************************************************************/
PICKEDOBJECT *pickobject ( uint32_t x, 
                           uint32_t y,
                           uint32_t xsep,
                           uint32_t xm,
                           uint32_t ym,
                           uint32_t strwidth,
                           G3DOBJECT *obj,
                           G3DSCENE *sce,
                           G3DURMANAGER *urm,
                           uint32_t pick_flags,
                           uint64_t engine_flags ) {
    LISTEDOBJECT *lob = common_g3duilist_sizeListedObject ( obj, x, y, xsep, strwidth, 0x00 );
    static PICKEDOBJECT pob;
    uint32_t nbtex = 0x00, nbtag = 0x00;
    uint32_t i;
    LIST *ltmptag = obj->ltag;

    pob.picked = 0x00;

    /*** where did the mouse hit ? ***/
    if ( inRectangle ( &lob->expand , xm, ym ) ) pob.picked = EXPANDRECTHIT;
    if ( inRectangle ( &lob->icon   , xm, ym ) ) pob.picked = ICONRECTHIT;
    if ( inRectangle ( &lob->name   , xm, ym ) ) pob.picked = NAMERECTHIT;
    if ( inRectangle ( &lob->active , xm, ym ) ) pob.picked = ACTIVERECTHIT;
    if ( inRectangle ( &lob->visible, xm, ym ) ) pob.picked = VISIBLERECTHIT;

    if ( obj->type & MESH ) {
        G3DMESH *mes  = ( G3DMESH * ) obj;
        LIST *ltmpuvmap = mes->luvmap;
        uint32_t nbuvmap = 0x00;

        while ( ltmpuvmap ) {
            G3DUVMAP *uvmap = ( G3DUVMAP * ) ltmpuvmap->data;

            if ( inRectangle ( &lob->uvmap[nbuvmap], xm, ym ) ) {
                pob.uvmap  = uvmap;
                pob.picked = UVMAPRECTHIT;
            }

            nbuvmap++;

            ltmpuvmap = ltmpuvmap->next;
        }
    }

    if ( obj->type & MESH ) {
        G3DMESH *mes  = ( G3DMESH * ) obj;
        LIST *ltmptex = mes->ltex;

        while ( ltmptex ) {
            G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;

            if ( inRectangle ( &lob->texture[nbtex], xm, ym ) ) {
                pob.tex    = tex;
                pob.picked = TEXTURERECTHIT;
            }

            nbtex++;

            ltmptex = ltmptex->next;
        }
    }

    while ( ltmptag ) {
        G3DTAG *tag = ( G3DTAG * ) ltmptag->data;

        if ( ( tag->flags & TAGHIDDEN ) == 0x00 ) {
            if ( inRectangle ( &lob->tag[nbtag], xm, ym ) ) {
                pob.tag    = tag;
                pob.picked = TAGRECTHIT;
            }

            nbtag++;
        }

        ltmptag = ltmptag->next;
    }

    if ( pob.picked ) {
        pob.obj = obj;

        /*** When no parsing happens, pob rectangles are not processed.    ***/
        /*** Nothing is passed to G3D API or G3DURMANAGER API. No          ***/
        /*** selection from G3DSCENE, no texture selection for the object. ***/
        if ( ( pick_flags & PICKEDOBJECTNOPARSE ) == 0x00 ) {
            pickedobject_parse ( &pob, sce, urm, pick_flags, engine_flags );
        }

        return &pob;
    }


    return NULL;
}

/*******************************************************************************/
uint32_t listedObject_sizeInit ( LISTEDOBJECT *lob, uint32_t x, 
                                                    uint32_t y,
                                                    uint32_t strwidth ) {
    lob->hit = 0x00;

    /*** The expand icon (+) or (-) ***/
    lob->expand.x       = x;
    lob->expand.y       = y;
    lob->expand.width   = LISTINDENT;
    lob->expand.height  = LISTINDENT;

    /*** The object type nice icon ***/
    lob->icon.x         = lob->expand.x + lob->expand.width;
    lob->icon.y         = y;
    lob->icon.width     = LISTINDENT;
    lob->icon.height    = LISTINDENT;

    /*** The object name area ***/
    lob->name.x         = lob->icon.x + lob->icon.width + 0x04;
    lob->name.y         = y;
    lob->name.width     = strwidth;
    lob->name.height    = LISTINDENT;

    return ( lob->name.x + lob->name.width );
}

/*******************************************************************************/
LISTEDOBJECT *common_g3duilist_sizeListedObject ( G3DOBJECT *obj, 
                                                  uint32_t x, 
                                                  uint32_t y,
                                                  uint32_t xsep,
                                                  uint32_t strwidth,
                                                  uint32_t query_flags ) {
    static LISTEDOBJECT lob; /*** Defined as static variable so we can    ***/
                           /*** return a valid pointer to this variable ***/
    uint32_t nextx;
    uint32_t nbtag = 0x00;
    LIST *ltmptag = obj->ltag;

    /*** size expander + icon + name ***/
    listedObject_sizeInit ( &lob, x, y, strwidth );

    /*** the name rectangle expands to the separator ***/
    lob.name.width     = ( xsep - lob.name.x );

    lob.active.x       = xsep + 0x04;
    lob.active.y       = y;
    lob.active.width   = LISTINDENT;
    lob.active.height  = LISTINDENT;

    lob.visible.x      = lob.active.x + lob.active.width;
    lob.visible.y      = y;
    lob.visible.width  = LISTINDENT;
    lob.visible.height = LISTINDENT;

    lob.endx           = lob.visible.x + lob.visible.height;

    /*** Otherwise, the widget width is the last uvmap tag boundary **/
    if ( obj->type & MESH ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        LIST *ltmpuvmap = mes->luvmap;
        uint32_t nbuvmap = 0x00;

        while ( ltmpuvmap && ( nbuvmap < MAXUVMAPS ) ) {
            G3DUVMAP *uvmap  = ( G3DUVMAP * ) ltmpuvmap->data;

            lob.uvmap[nbuvmap].x      = lob.endx;
            lob.uvmap[nbuvmap].y      = y;
            lob.uvmap[nbuvmap].width  = LISTINDENT;
            lob.uvmap[nbuvmap].height = LISTINDENT;

            lob.endx += ( LISTINDENT + 0x02 );

            nbuvmap++;

            ltmpuvmap = ltmpuvmap->next;
        }
    }

    /*** Otherwise, the widget width is the last texture tag boundary **/
    if ( obj->type & MESH ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        LIST *ltmptex = mes->ltex;
        uint32_t nbtex = 0x00;

        while ( ltmptex && ( nbtex < MAXTEXTURES ) ) {
            G3DTEXTURE *tex  = ( G3DTEXTURE * ) ltmptex->data;

            lob.texture[nbtex].x      = lob.endx;
            lob.texture[nbtex].y      = y;
            lob.texture[nbtex].width  = LISTINDENT;
            lob.texture[nbtex].height = LISTINDENT;

            lob.endx += ( LISTINDENT + 0x02 );

            nbtex++;

            ltmptex = ltmptex->next;
        }
    }

    while ( ltmptag && ( nbtag < MAXTAGS ) ) {
        G3DTAG *tag  = ( G3DTAG * ) ltmptag->data;

        if ( ( tag->flags & TAGHIDDEN ) == 0x00 ) {
            lob.tag[nbtag].x      = lob.endx;
            lob.tag[nbtag].y      = y;
            lob.tag[nbtag].width  = LISTINDENT;
            lob.tag[nbtag].height = LISTINDENT;

            lob.endx += ( LISTINDENT + 0x02 );

            nbtag++;
        }

        ltmptag = ltmptag->next;
    }

    return &lob;
}

/*******************************************************************************/
void objectlistprivatedata_free ( OBJECTLISTPRIVATEDATA *opd ) {
    free ( opd );
}

/*******************************************************************************/
OBJECTLISTPRIVATEDATA *objectlistprivatedata_new ( G3DUI *gui ) {
    OBJECTLISTPRIVATEDATA *opd = ( OBJECTLISTPRIVATEDATA * ) calloc ( 0x01, sizeof ( OBJECTLISTPRIVATEDATA ) );

    opd->gui = gui;

    return opd;
}
