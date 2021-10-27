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
#include <g3dui_gtk3.h>

/******************************************************************************/
#include <xpm/horz.xpm>
#include <xpm/vert.xpm>
#include <xpm/shor.xpm>
#include <xpm/bonico.xpm>
#include <xpm/ligico.xpm>
#include <xpm/mesico.xpm>
#include <xpm/defico.xpm>
#include <xpm/symico.xpm>
#include <xpm/cubico.xpm>
#include <xpm/sphico.xpm>
#include <xpm/torico.xpm>
#include <xpm/cylico.xpm>
#include <xpm/camico.xpm>
#include <xpm/scnico.xpm>
#include <xpm/ffdico.xpm>
#include <xpm/mapico.xpm>
#include <xpm/plnico.xpm>
#include <xpm/eyeopn.xpm>
#include <xpm/eyesht.xpm>
#include <xpm/enaico.xpm>
#include <xpm/disico.xpm>
#include <xpm/subico.xpm>
#include <xpm/wirico.xpm>
#include <xpm/splico.xpm>
#include <xpm/srvico.xpm>
#include <xpm/txtico.xpm>
#include <xpm/tubico.xpm>
#include <xpm/morpher.xpm>
#include <xpm/vibrate.xpm>
#include <xpm/tracker.xpm>
#include <xpm/sknico.xpm>
#include <xpm/insico.xpm>

/*******************************************************************************/
static uint32_t getObjectNameWidth ( GtkStyleContext *context, G3DOBJECT *obj ) {
    PangoFontDescription *fdesc;
    uint32_t charwidth;

    gtk_style_context_get ( context, GTK_STATE_FLAG_NORMAL, "font", &fdesc, NULL );

    charwidth = pango_font_description_get_size ( fdesc ) / PANGO_SCALE;

    return ( charwidth * strlen ( obj->name ) );
}

/*******************************************************************************/
static uint32_t getSeparator_r ( GtkStyleContext *context, G3DOBJECT *obj,
                                                           uint32_t x,
                                                           uint32_t y,
                                                           uint32_t xindent,
                                                           uint32_t yindent ) {
    uint32_t strwidth = getObjectNameWidth ( context, obj );
    LIST *ltmpobj = obj->lchildren;
    static LISTEDOBJECT lob;
    static uint32_t sepx;
    static uint32_t curheight;
    uint32_t seppos;

    if ( obj->parent == NULL ) {
        /*** Inital separator position is 64 ***/
        sepx       = 0x40;
        curheight  = yindent;
    } else {
        /*** We use the static nature of curheight in the recursion ***/
        curheight += yindent;
    }

    /*** Find this objet's separator position ***/
    seppos = listedObject_sizeInit ( &lob, x, y, strwidth );

    /*** If it is further to the right than the current separator position ***/
    /*** we'll move the separator to a new position returned by this func. ***/
    if ( seppos > sepx  ) sepx = seppos;

    /*** Check against children ***/
    if ( ( obj->flags & OBJECTCOLLAPSED ) == 0x00 ) {
        while ( ltmpobj ) {
            G3DOBJECT *child = ( G3DOBJECT * ) ltmpobj->data;

            /*** If the separator must be placed further to the right, sepx ***/
            /*** will be modified by the recursion due to its static nature ***/
            getSeparator_r ( context, child, x + xindent, curheight, xindent,
                                                                     yindent );

            ltmpobj = ltmpobj->next;
        }
    }

    return sepx;
}

/******************************************************************************/
PICKEDOBJECT *selectObject_r ( GtkStyleContext *context, G3DOBJECT *obj,
                                    /* Object X pos  */  uint32_t x,
                                    /* Object Y pos  */  uint32_t y,
                                    /* Separator pos */  uint32_t xsep,
                                                         uint32_t xindent,
                                                         uint32_t yindent,
                                    /* Mouse X coord */  uint32_t xm,
                                    /* Mouse Y coord */  uint32_t ym,
                                                         G3DSCENE *sce,
                                                         G3DURMANAGER *urm,
                                                         uint32_t keep,
                                                         uint64_t engine_flags){
    LIST *ltmpobj = obj->lchildren;
    PICKEDOBJECT *pob;
    uint32_t strwidth;
    static curheight;

    strwidth = getObjectNameWidth ( context, obj );

    /*** We use static variable property to position objects in height ***/
    if ( obj->parent == NULL ) curheight  = yindent;
    else                       curheight += yindent;

    /*if ( obj->type != G3DSCENETYPE ) {*/
        pob = pickobject ( x, y, xsep, xm, ym, strwidth, obj, sce, urm, keep, engine_flags );

        if ( pob ) {
            return pob;
        }
    /*}*/

    if ( ( obj->flags & OBJECTCOLLAPSED ) == 0x00 ) {
        /*** Pick the children ***/
        while ( ltmpobj ) {
            G3DOBJECT *child = ( G3DOBJECT * ) ltmpobj->data;
            PICKEDOBJECT *sel;

            if ( ( sel = selectObject_r ( context, child,
                                                   x + xindent,
                                                   curheight,
                                                   xsep,
                                                   xindent,
                                                   yindent,
                                                   xm,
                                                   ym,
                                                   sce,
                                                   urm,
                                                   keep,
                                                   engine_flags ) ) ) {
                return sel;
            }


            ltmpobj = ltmpobj->next;
        }
    }


    return NULL;
}

/******************************************************************************/
static void drawActive ( GtkStyleContext *context, cairo_t *cr,
                                                   G3DOBJECT *obj, 
                                                   uint32_t x, 
                                                   uint32_t y ) {
    static GdkPixbuf *enaico, *disico;

    /*** Inited once and for all ***/
    if ( enaico == NULL ) enaico = gdk_pixbuf_new_from_xpm_data ( enaico_xpm );
    if ( disico == NULL ) disico = gdk_pixbuf_new_from_xpm_data ( disico_xpm );

    /*if ( ( obj->type & MODIFIER ) || 
         ( obj->type & MESH     ) || 
         ( obj->type & BONE     ) ) {*/
        if ( obj->flags & OBJECTINACTIVE ) {
            gdk_cairo_set_source_pixbuf ( cr, disico, x, y );
        } else {
            gdk_cairo_set_source_pixbuf ( cr, enaico, x, y );
        }
        cairo_paint ( cr );
    /*}*/
}

/******************************************************************************/
static void drawVisible ( GtkStyleContext *context, cairo_t *cr,
                                                    G3DOBJECT *obj, 
                                                    uint32_t x, 
                                                    uint32_t y ) {
    static GdkPixbuf *eyeopn, *eyesht;

    /*** Inited once and for all ***/
    if ( eyeopn == NULL ) eyeopn = gdk_pixbuf_new_from_xpm_data ( eyeopn_xpm );
    if ( eyesht == NULL ) eyesht = gdk_pixbuf_new_from_xpm_data ( eyesht_xpm );

    if ( obj->flags & OBJECTINVISIBLE ) {
        gdk_cairo_set_source_pixbuf ( cr, eyesht, x, y );
    } else {
        gdk_cairo_set_source_pixbuf ( cr, eyeopn, x, y );
    }
    cairo_paint ( cr );
}

/******************************************************************************/
static void drawTags ( GtkStyleContext *context, 
                       cairo_t         *cr,
                       G3DOBJECT       *obj,
                       G3DUIRECTANGLE *rec ) {
    LIST *ltmptag = obj->ltag;

    while ( ltmptag ) {
        G3DTAG *tag = ( G3DTAG * ) ltmptag->data;
        GdkPixbuf  *pixbuf; 

        if ( ( tag->flags & TAGHIDDEN ) == 0x00 ) {
            /*** This is very inefficient: creating a pixbuf at each     ***/
            /*** exposure. I don't know how to fill directly a GdkPixbuf.***/
            switch ( tag->type ) {
                case G3DTAGVIBRATORTYPE :
                    pixbuf = gdk_pixbuf_new_from_xpm_data ( vibrate_xpm );
                break;

                case G3DTAGTRACKERTYPE :
                    pixbuf = gdk_pixbuf_new_from_xpm_data ( tracker_xpm );
                break;

                default :
                break;
            }

            gdk_cairo_set_source_pixbuf ( cr, pixbuf, rec->x, rec->y );

            cairo_paint ( cr );

            g_object_unref ( G_OBJECT(pixbuf) );

            if ( ( obj->flags & OBJECTSELECTED  ) &&
                 ( tag == obj->seltag     ) ) {
                cairo_set_source_rgb ( cr, 1.0f, 0.0f, 0.0f );
                cairo_set_line_width ( cr, 0.5f );
                cairo_rectangle      ( cr, rec->x, rec->y, rec->width,
                                                           rec->height );
                cairo_stroke         ( cr );
            }

            rec++;
        }

        ltmptag = ltmptag->next;
    }
}

/******************************************************************************/
static void drawTextures ( GtkStyleContext *context, cairo_t *cr,
                                                     G3DOBJECT *obj, 
                                                     G3DUIRECTANGLE *rec ) {
    static G3DUIMATERIALMAP *matmap;
    static unsigned char pixel[0x100][0x03];

    /*** Init once and for all ***/
    if ( matmap == NULL ) {
        matmap = common_g3duimaterialmap_new ( LISTINDENT, LISTINDENT );
    }

    if ( obj->type & MESH && ( ( obj->type & MODIFIER ) == 0x00 ) ) {
        G3DMESH *mes =  ( G3DMESH * ) obj;
        LIST *ltmptex = mes->ltex;
        uint32_t nbtex = 0x00;

        while ( ltmptex ) {
            G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;
            GdkPixbuf  *pixbuf; 

            common_g3duimaterialmap_buildSphere ( matmap, tex->mat, 0.8f );

            common_g3duimaterialmap_fillData ( matmap, tex->mat, pixel );

            /*** This is very inefficient: creating a pixbuf at each     ***/
            /*** exposure. I don't know how to fill directly a GdkPixbuf.***/
            pixbuf = gdk_pixbuf_new_from_data ( (const guchar *) pixel, 
                                                GDK_COLORSPACE_RGB, 
                                                FALSE,
                                                0x08, 
                                                0x10,
                                                0x10,
                                                0x03 * 0x10,
                                                NULL, NULL );

            gdk_cairo_set_source_pixbuf ( cr, pixbuf, rec->x, rec->y );
            cairo_paint ( cr );

            g_object_unref ( G_OBJECT(pixbuf) );

            /*XPutImage ( dis, win, gc, mpv->img, 0x00, 0x00,
                                                lob->texture[nbtex].x + 0x01,
                                                lob->texture[nbtex].y + 0x01, 
                                                mpv->img->width,
                                                mpv->img->height );*/

            if ( ( obj->flags & OBJECTSELECTED  ) &&
                 ( tex->flags & TEXTURESELECTED ) ) {
                cairo_set_source_rgb ( cr, 1.0f, 0.0f, 0.0f );
                cairo_set_line_width ( cr, 0.5f );
                cairo_rectangle      ( cr, rec->x, rec->y, rec->width,
                                                           rec->height );
                cairo_stroke         ( cr );
            }

            nbtex++; rec++;

            ltmptex = ltmptex->next;
        }
    }
}

/******************************************************************************/
static void drawUVMaps ( GtkStyleContext *context, cairo_t *cr,
                                                   G3DOBJECT *obj, 
                                                   G3DUIRECTANGLE *rec ) {
    static G3DUIMATERIALMAP *matmap;
    static unsigned char pixel[0x10][0x10][0x03];
    static int init;

    if ( init == 0x00 ) {
        unsigned char color = 0x00;
        uint32_t i, j;
        init = 0x01;

        for ( i = 0x00; i < 0x10; i++ ) {
            for ( j = 0x00; j < 0x10; j++ ) {
                pixel[i][j][0x00] = 
                pixel[i][j][0x01] = 
                pixel[i][j][0x02] = color;

                if ( ( j % 0x04 ) == 0x00 ) {
                    color = (~color);
                }
            }

            if ( ( i % 0x04 ) == 0x00 ) {
                color = (~color);
            }
        }
    }


    /*** Init once and for all ***/
    if ( matmap == NULL ) matmap = common_g3duimaterialmap_new ( LISTINDENT,
                                                                 LISTINDENT );

    if ( obj->type & MESH && ( ( obj->type & MODIFIER ) == 0x00 ) ) {
        G3DMESH *mes =  ( G3DMESH * ) obj;
        LIST *ltmpuvmap = mes->luvmap;
        uint32_t nbuvmap = 0x00;

        while ( ltmpuvmap ) {
            G3DUVMAP  *uvmap = ( G3DUVMAP * ) ltmpuvmap->data;
            GdkPixbuf *pixbuf; 

            /*** This is very inefficient: creating a pixbuf at each     ***/
            /*** exposure. I don't know how to fill directly a GdkPixbuf.***/
            pixbuf = gdk_pixbuf_new_from_data ( (const guchar *) pixel, 
                                                GDK_COLORSPACE_RGB, 
                                                FALSE,
                                                0x08, 
                                                0x10,
                                                0x10,
                                                0x03 * 0x10,
                                                NULL, NULL );

            gdk_cairo_set_source_pixbuf ( cr, pixbuf, rec->x, rec->y );
            cairo_paint ( cr );

            g_object_unref ( G_OBJECT(pixbuf) );

            /*XPutImage ( dis, win, gc, mpv->img, 0x00, 0x00,
                                                lob->texture[nbtex].x + 0x01,
                                                lob->texture[nbtex].y + 0x01, 
                                                mpv->img->width,
                                                mpv->img->height );*/

            if ( ( obj->flags & OBJECTSELECTED ) &&
                 ( ((G3DOBJECT*)uvmap)->flags & OBJECTSELECTED ) ) {
                cairo_set_source_rgb ( cr, 1.0f, 0.0f, 1.0f );
                cairo_set_line_width ( cr, 0.5f );
                cairo_rectangle      ( cr, rec->x, rec->y, rec->width,
                                                           rec->height );
                cairo_stroke         ( cr );
            }

            nbuvmap++; rec++;

            ltmpuvmap = ltmpuvmap->next;
        }
    }
}

/******************************************************************************/
static void drawObjectName ( GtkStyleContext *context, cairo_t *cr,
                                                       G3DOBJECT *obj, 
                                                       float x, 
                                                       float y ) {
    cairo_text_extents_t te;
    GdkRGBA fg;

    cairo_text_extents ( cr, obj->name, &te );

    gtk_style_context_get_color ( context, GTK_STATE_FLAG_NORMAL, &fg );

    if ( obj->flags & OBJECTSELECTED ) {
        cairo_set_source_rgba ( cr, 1.0f  , 0.0f    , 0.0f   , fg.alpha );
    } else {
        cairo_set_source_rgba ( cr, fg.red, fg.green, fg.blue, fg.alpha );
    }

    y = y - ( te.height / 2 + te.y_bearing ) + LISTINDENT * 0.5f;

    cairo_move_to   ( cr, x, y );
    cairo_show_text ( cr, obj->name );
}

/******************************************************************************/
static void drawObjectIcon ( GtkStyleContext *context, cairo_t *cr,
                                                       G3DOBJECT *obj, 
                                                       uint32_t x, 
                                                       uint32_t y ) {
    static GdkPixbuf *bonico, *ligico, *mesico,
                     *defico, *symico, *cubico,
                     *sphico, *torico, *conico,
                     *plnico, *ffdico, *cylico,
                     *camico, *scnico, *mapico, 
                     *objico, *subico, *wirico, 
                     *splico, *srvico, *txtico,
                     *tubico, *mrpico, *sknico,
                     *insico;

    /*********************** Inited once and for all **************************/
    if ( bonico == NULL ) bonico = gdk_pixbuf_new_from_xpm_data ( bonico_xpm );
    if ( ligico == NULL ) ligico = gdk_pixbuf_new_from_xpm_data ( ligico_xpm );
    if ( mesico == NULL ) mesico = gdk_pixbuf_new_from_xpm_data ( mesico_xpm );
    if ( defico == NULL ) defico = gdk_pixbuf_new_from_xpm_data ( defico_xpm );
    if ( symico == NULL ) symico = gdk_pixbuf_new_from_xpm_data ( symico_xpm );
    if ( cubico == NULL ) cubico = gdk_pixbuf_new_from_xpm_data ( cubico_xpm );
    if ( sphico == NULL ) sphico = gdk_pixbuf_new_from_xpm_data ( sphico_xpm );
    if ( torico == NULL ) torico = gdk_pixbuf_new_from_xpm_data ( torico_xpm );
    /*if ( conico == NULL ) conico = gdk_pixbuf_new_from_xpm_data ( conico_xpm );*/
    if ( plnico == NULL ) plnico = gdk_pixbuf_new_from_xpm_data ( plnico_xpm );
    if ( ffdico == NULL ) ffdico = gdk_pixbuf_new_from_xpm_data ( ffdico_xpm );
    if ( cylico == NULL ) cylico = gdk_pixbuf_new_from_xpm_data ( cylico_xpm );
    if ( camico == NULL ) camico = gdk_pixbuf_new_from_xpm_data ( camico_xpm );
    if ( scnico == NULL ) scnico = gdk_pixbuf_new_from_xpm_data ( scnico_xpm );
    if ( mapico == NULL ) mapico = gdk_pixbuf_new_from_xpm_data ( mapico_xpm );
    if ( subico == NULL ) subico = gdk_pixbuf_new_from_xpm_data ( subico_xpm );
    if ( wirico == NULL ) wirico = gdk_pixbuf_new_from_xpm_data ( wirico_xpm );
    if ( splico == NULL ) splico = gdk_pixbuf_new_from_xpm_data ( splico_xpm );
    if ( srvico == NULL ) srvico = gdk_pixbuf_new_from_xpm_data ( srvico_xpm );
    if ( txtico == NULL ) txtico = gdk_pixbuf_new_from_xpm_data ( txtico_xpm );
    if ( tubico == NULL ) tubico = gdk_pixbuf_new_from_xpm_data ( tubico_xpm );
    if ( mrpico == NULL ) mrpico = gdk_pixbuf_new_from_xpm_data ( morpher_xpm );
    if ( sknico == NULL ) sknico = gdk_pixbuf_new_from_xpm_data ( sknico_xpm );
    if ( insico == NULL ) insico = gdk_pixbuf_new_from_xpm_data ( insico_xpm );
   /***************************************************************************/

    switch ( obj->type ) {
        case G3DLIGHTTYPE          : objico = ligico; break;
        case G3DBONETYPE           : objico = bonico; break;
        case G3DMESHTYPE           : objico = mesico; break;
        case G3DSYMMETRYTYPE       : objico = symico; break;
        case G3DSUBDIVIDERTYPE     : objico = subico; break;
        case G3DCUBETYPE           : objico = cubico; break;
        case G3DSPHERETYPE         : objico = sphico; break;
        case G3DTORUSTYPE          : objico = torico; break;
        case G3DCONETYPE           : objico = conico; break;
        case G3DPLANETYPE          : objico = plnico; break;
        case G3DFFDTYPE            : objico = ffdico; break;
        case G3DCYLINDERTYPE       : objico = cylico; break;
        case G3DCAMERATYPE         : objico = camico; break;
        case G3DSCENETYPE          : objico = scnico; break;
        case G3DUVMAPTYPE          : objico = mapico; break;
        case G3DWIREFRAMETYPE      : objico = wirico; break;
        case G3DSPLINETYPE         : objico = splico; break;
        case G3DSPLINEREVOLVERTYPE : objico = srvico; break;
        case G3DTEXTTYPE           : objico = txtico; break;
        case G3DTUBETYPE           : objico = tubico; break;
        case G3DMORPHERTYPE        : objico = mrpico; break;
        case G3DSKINTYPE           : objico = sknico; break;
        case G3DINSTANCETYPE       : objico = insico; break;
        default                    : objico = defico; break;
    }

    gdk_cairo_set_source_pixbuf ( cr, objico, x, y );
    cairo_paint ( cr );
}

/******************************************************************************/
static void drawArrow ( GtkStyleContext *context, cairo_t *cr,
                                                  G3DOBJECT *obj, 
                                                  uint32_t x, 
                                                  uint32_t y ) {
    /*** Draw the arrow only for parent objects ***/
    if ( obj->lchildren ) {
        if ( obj->flags & OBJECTCOLLAPSED ) {
            gtk_style_context_set_state ( context, GTK_STATE_FLAG_NORMAL );
        } else {
            gtk_style_context_set_state ( context, GTK_STATE_FLAG_ACTIVE );
        }
        gtk_render_expander ( context, cr, x, y, LISTINDENT, LISTINDENT );
    }
}

/******************************************************************************/
uint32_t printObject_r ( GtkStyleContext *context, cairo_t *cr, 
                                                   G3DOBJECT *obj, 
                                                   uint32_t x, 
                                                   uint32_t y,
                              /* Separator pos */  uint32_t xsep,
                                                   uint32_t xindent,
                                                   uint32_t yindent ) {
    static uint32_t maxheight, maxwidth, curheight;
    LIST *ltmpobj = obj->lchildren;
    uint32_t total_height = yindent;
    LISTEDOBJECT *lob;
    uint32_t strwidth;

    /*** We use static variable property to position objects in height ***/
    if ( obj->parent == NULL ) { curheight  = yindent; maxwidth = 0x00; }
    else                         curheight += yindent;

    strwidth = getObjectNameWidth ( context, obj );

    lob = common_g3duilist_sizeListedObject ( obj, x, y, xsep, strwidth, 0x00 );

    if ( maxwidth < lob->endx ) maxwidth = lob->endx;

    drawArrow      ( context, cr, obj,  lob->expand.x , lob->expand.y  );
    drawObjectIcon ( context, cr, obj,  lob->icon.x   , lob->icon.y    );
    drawObjectName ( context, cr, obj,  lob->name.x   , lob->name.y    );
    drawActive     ( context, cr, obj,  lob->active.x , lob->active.y  );
    drawVisible    ( context, cr, obj,  lob->visible.x, lob->visible.y );
    drawTextures   ( context, cr, obj,  lob->texture                   );
    drawUVMaps     ( context, cr, obj,  lob->uvmap                     );
    drawTags       ( context, cr, obj,  lob->tag                       );

    /*** Recurse ***/
    if ( ( obj->flags & OBJECTCOLLAPSED ) == 0x00 ) {
        /*** Draw the children ***/
        while ( ltmpobj ) {
            G3DOBJECT *child = ( G3DOBJECT * ) ltmpobj->data;

            printObject_r ( context, cr, child, x + xindent, curheight, xsep, xindent,
                                                                              yindent );


            ltmpobj = ltmpobj->next;
        }
    }

    maxheight = ( curheight );


    return ( ( maxheight << 0x10 ) | maxwidth );
}

/******************************************************************************/
void objectlistarea_input ( GtkWidget *widget, GdkEvent *gdkev, 
                                               gpointer user_data ) {
    GtkStyleContext *context = gtk_widget_get_style_context ( widget );
    G3DUI    *gui = ( G3DUI * ) user_data;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    static G3DOBJECT *obj, *dst;
    static PICKEDOBJECT *pob;
    G3DUICLIPBOARD *cli = gui->cli;

    switch ( gdkev->type ) {
        case GDK_KEY_PRESS : {
            GdkEventKey *kev = ( GdkEventKey * ) gdkev;

            switch ( kev->keyval ) {
                case GDK_KEY_Delete: {
                    uint32_t retflags = ( REDRAWVIEW | REDRAWLIST );

                    if ( pob ) {
                        switch ( pob->picked ) {
                            case TEXTURERECTHIT : {
                                if ( pob->obj->type & MESH ) {
                                    g3durm_mesh_removeTexture ( urm,
                                                                pob->obj,
                                                                pob->tex,
                                                                gui->engine_flags,
                                                                retflags );
                                }
                            } break;

                            case UVMAPRECTHIT : {
                                g3durm_mesh_removeUVMap ( urm,
                                                          pob->obj,
                                                          pob->uvmap, 
                                                          gui->engine_flags,
                                                          REDRAWVIEW |REDRAWLIST );
                            } break;

                            case TAGRECTHIT : {
                                g3durm_selection_removeTag ( urm,
                                                             pob->obj,
                                                             pob->tag,
                                                             gui->engine_flags,
                                                             REDRAWVIEW | REDRAWLIST );
                            } break;

                            default : {
                                g3durm_scene_deleteSelectedObjects ( urm, sce, gui->engine_flags, retflags );

                                pob = NULL;
                            } break;
                        }
                    }
                } break;

                case GDK_KEY_c: {
                    printf("copying %d object(s)\n", list_count ( sce->lsel ) );

                    g3dui_setHourGlass ( gui );
                    g3duiclipboard_copyObject ( cli, 
                                                sce,
                                                sce->lsel, 0x01, gui->engine_flags );
                    g3dui_unsetHourGlass ( gui );
                } break;

                case GDK_KEY_v: {
                    G3DOBJECT *src = g3dscene_getSelectedObject ( sce );
                    G3DOBJECT *dst = ( G3DOBJECT * ) sce;

                    printf("pasting\n");

                    /*** add the copied object to the   ***/
                    /*** selected object's parent objet ***/
                    if ( src ) dst = src->parent;

                    g3dui_setHourGlass   ( gui );
                    g3duiclipboard_paste ( cli, urm, sce, dst, gui->engine_flags );
                    g3dui_unsetHourGlass ( gui );
                } break;
            }

            g3dui_redrawObjectList     ( gui );
            g3dui_updateAllCurrentEdit ( gui );
            g3dui_redrawGLViews        ( gui );
        } break;

        case GDK_2BUTTON_PRESS : {
            GdkEventButton *bev = ( GdkEventButton * ) gdkev;
            uint32_t xsep = getSeparator_r ( context, ( G3DOBJECT * ) sce,
                                                      0x00,
                                                      0x00,
                                                      LISTINDENT,
                                                      LISTINDENT );

            pob = selectObject_r ( context, ( G3DOBJECT * ) sce,
                                            0x00,
                                            0x00,
                                            xsep,
                                            LISTINDENT, 
                                            LISTINDENT,
                                            bev->x,
                                            bev->y,
                                            sce, urm, 0x00, gui->engine_flags );

            if ( pob && ( pob->picked == TEXTURERECTHIT ) ) {
                GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );

                createTextureEdit ( dial, gui, "TEXTUREEDIT", 0, 0, 264, 48 );

                gtk_widget_show ( dial );
            }

            if ( pob && ( pob->picked == UVMAPRECTHIT ) ) {
                GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );

                createUVMapEdit ( dial, gui, "UVMAPEDIT", 0, 0, 264, 48 );

                gtk_widget_show ( dial );
            }

            if ( pob && ( pob->picked == TAGRECTHIT ) ) {
                GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );

                createTrackerTagEdit ( dial, gui, "TrackerTag Edit", 0, 0, 264, 48 );

                gtk_widget_show ( dial );
            }
        } break;

        case GDK_BUTTON_PRESS : {
            GdkEventButton *bev = ( GdkEventButton * ) gdkev;
            uint32_t pick_flags = ( bev->state & GDK_CONTROL_MASK ) ? PICKEOBJECTKEEPALL : 0x00;
            /*** Retrieve separator's position ***/
            uint32_t xsep = getSeparator_r ( context, ( G3DOBJECT * ) sce,
                                                      0x00,
                                                      0x00,
                                                      LISTINDENT,
                                                      LISTINDENT );
            GtkAdjustment *vadj;
            double vadj_value;
            LIST *lmes = NULL;
            GtkWidget *viewport = gtk_widget_get_parent ( widget );
            GtkWidget *scrolledWindow = gtk_widget_get_parent ( viewport );

            /*** For keyboard inputs, use gtk_widget_grab_focus          ***/
            /*** Note, for some unknown reason, gtk_widget_grab_focus    ***/
            /*** sets scrollbar adjustments to 0, so we have to save it  ***/
            /*** and restore it in order to avoid the window to get      ***/
            /*** scrolled up everytime it gains focus. Search Google for ***/
            /*** "gtk_widget_grab_focus sets scrollbar adjustments to 0" ***/

            vadj = gtk_scrolled_window_get_vadjustment ( scrolledWindow );
            vadj_value = gtk_adjustment_get_value ( vadj );
            gtk_widget_grab_focus ( widget );
            gtk_adjustment_set_value ( vadj, vadj_value );

            /*** .Unselect previously selected texture tags ***/
            /*** .Free the list and clear texture selection ***/
            g3dobject_getObjectsByType_r ( ( G3DOBJECT * ) sce, G3DMESHTYPE, &lmes );

            list_free ( &lmes, (void (*)(void *)) g3dmesh_unselectAllTextures );

            g3dui_setHourGlass   ( gui );

            pob = selectObject_r ( context, ( G3DOBJECT * ) sce,
                                            0x00,
                                            0x00,
                                            xsep,
                                            LISTINDENT, 
                                            LISTINDENT,
                                            bev->x,
                                            bev->y,
                                            sce, urm, pick_flags, gui->engine_flags );
            g3dui_unsetHourGlass ( gui );

            if ( pob ) {
                obj = pob->obj;

                if ( pob->picked == TEXTURERECTHIT ) {
                    g3dui_resizeUVMapEditors ( gui );
                }

                /*g3dui_updateCoords ( gui );
                g3dui_redrawTimeline ( gui );*/
            }

            g3dui_redrawObjectList ( gui );
            g3dui_redrawGLViews ( gui );
            g3dui_updateAllCurrentEdit ( gui );
            g3dui_redrawTimeline ( gui );
        } break;

        case GDK_MOTION_NOTIFY : {
            GdkEventMotion *mev = ( GdkEventMotion * ) gdkev;

            if ( obj ) {
                /*** Retrieve separator's position ***/
                uint32_t xsep = getSeparator_r ( context, ( G3DOBJECT * ) sce,
                                                          0x00,
                                                          0x00,
                                                          LISTINDENT,
                                                          LISTINDENT );

                /*** drag and drop ***/
                pob = selectObject_r ( context, ( G3DOBJECT * ) sce,
                                                0x00,
                                                0x00,
                                                xsep,
                                                LISTINDENT, 
                                                LISTINDENT,
                                                mev->x,
                                                mev->y,
                                                sce, NULL, PICKEDOBJECTNOPARSE, gui->engine_flags );

                dst = ( pob ) ? pob->obj : NULL;

                g3dui_redrawObjectList ( gui );
            }
        } break;

        case GDK_BUTTON_RELEASE : {
            GdkEventButton *bev = ( GdkEventButton * ) gdkev;

                if ( ( obj        ) && /*** If we pressed on an object   ***/
                     ( dst        ) && /*** And we released on an object ***/
                     ( dst != obj ) && /*** and they are different       ***/
                     /*** and the future child is not the current parent ***/
                     ( g3dobject_isChild ( obj, dst ) == 0x00 ) ) {
                    G3DOBJECT *par = obj->parent;

                    if ( g3dobject_hasRiggedBone_r ( obj ) ) {
                        GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
                        GtkMessageDialog *dialog;

                        dialog = gtk_message_dialog_new ( NULL,
                                                          flags,
                                                          GTK_MESSAGE_ERROR,
                                                          GTK_BUTTONS_CLOSE,
                                                          "Could alter hierarchy with rigged bones. Please unrig bones first." );
                        gtk_dialog_run ( GTK_DIALOG ( dialog ) );
                        gtk_widget_destroy ( dialog );
                    } else {
                        /** prevent scene from becoming a child object ***/
                        if ( obj->type == G3DSCENETYPE ) break;

                        /*** Perform action & record for the undo-redo manager ***/
                        g3durm_object_addChild ( urm, sce, gui->engine_flags,
                                                           ( REDRAWVIEW |
                                                             REDRAWLIST | 
                                                             REDRAWCURRENTOBJECT ),
                                               ( G3DOBJECT * ) par,
                                               ( G3DOBJECT * ) dst,
                                               ( G3DOBJECT * ) obj );

                        if ( ( dst->type == G3DSYMMETRYTYPE ) &&
                             ( obj->type == G3DMESHTYPE ) ) {
                            g3dsymmetry_meshChildChange ( ( G3DSYMMETRY * ) dst, ( G3DMESH * ) obj );

                            g3dmesh_update ( ( G3DMESH * ) obj, NULL,
                                                  NULL,
                                                  NULL,
                                                  UPDATEFACEPOSITION, gui->engine_flags );
                        }

                        g3dui_updateCoords ( gui );
                    }
                }

            g3dui_redrawObjectList ( gui );
            g3dui_updateCoords ( gui );
            g3dui_redrawGLViews ( gui );
            g3dui_redrawTimeline ( gui );
            g3dui_updateMenuBar ( gui );

            obj = dst = NULL;
        } break;

        default:
        break;
    }
}

/******************************************************************************/
void objectlistarea_draw ( GtkWidget *widget,  
                           cairo_t   *cr,  
                           gpointer   user_data ) {
    OBJECTLISTPRIVATEDATA *opd = ( OBJECTLISTPRIVATEDATA * ) user_data;
    G3DUI    *gui = ( G3DUI * ) opd->gui;
    G3DSCENE *sce = gui->sce;
    GtkStyleContext *context = gtk_widget_get_style_context ( widget );
    GtkAllocation allocation;
    cairo_text_extents_t te;
    uint32_t width, height;
    uint32_t size;
    /*** Retrieve separator's position ***/
    uint32_t xsep = getSeparator_r ( context, ( G3DOBJECT * ) sce,
                                              0x00,
                                              0x00,
                                              LISTINDENT,
                                              LISTINDENT );

    gtk_widget_get_allocation ( widget, &allocation );

    gtk_render_background ( context, cr,
                                     0x00,
                                     0x00,
                                     allocation.width,
                                     allocation.height );

    /*** Draw the separator ***/
    gtk_render_line ( context, cr,
                              xsep,
                              0x00,
                              xsep,
                              allocation.height );
 
    /*** Draw objects ***/
    size = printObject_r ( context, cr, 
                                    (G3DOBJECT *)sce,
                                    0x00, 0x00,
                                    xsep,
                                    LISTINDENT, LISTINDENT );

    /*** Adjust drawing area size. This implies draw() is called ***/
    /*** twice. We might want to change that in the future.      ***/

    /** Note: we add 0x10 in widht and height otherwise the scroll ba might ***/
    /*** get in the way and that's annoying . ***/
    width  = ( size & 0x0000FFFF ) + 0x10;
    height = ( size >> 0x10 ) + 0x10;

    if ( ( opd->oldWidth  != width  ) || 
         ( opd->oldHeight != height ) ) {
        gtk_widget_set_size_request ( widget, width, height );

        opd->oldWidth  = width;
        opd->oldHeight = height;
    }
}

/******************************************************************************/
static gboolean objectlistarea_delete ( GtkWidget *widget, 
                                        GdkEvent  *event,
                                        gpointer   data ) {
    OBJECTLISTPRIVATEDATA *opd = ( OBJECTLISTPRIVATEDATA * ) data;

    objectlistprivatedata_free ( opd );

    return FALSE;
}

/******************************************************************************/
GtkWidget *createObjectList ( GtkWidget *parent, G3DUI *gui,
                                                 char *name,
                                                 gint x,
                                                 gint y,
                                                 gint width,
                                                 gint height ) {
    OBJECTLISTPRIVATEDATA *opd = objectlistprivatedata_new ( gui );
    GdkRectangle scrrec = { 0, 0, width, height };
    GdkRectangle drwrec = { 0, 0, 0x120, 0x120  };
    GtkWidget *scr, *drw;

    scr = gtk_scrolled_window_new ( NULL, NULL );

    gtk_widget_set_name ( scr, name );

    gtk_widget_set_size_request ( scr, scrrec.width, scrrec.height );

    gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW(scr),
                                     GTK_POLICY_AUTOMATIC,
                                     GTK_POLICY_AUTOMATIC );

    gtk_fixed_put ( GTK_FIXED(parent), scr, x, y );



    /*** Drawing area within the Scrolled Window ***/
    drw = gtk_drawing_area_new ( );

    /*** For keyboard inputs ***/
    gtk_widget_set_can_focus ( drw, TRUE );

    /*** Drawing area does not receive mous events by defaults ***/
    gtk_widget_set_events ( drw, gtk_widget_get_events ( drw )  |
                                 GDK_KEY_PRESS_MASK             |
                                 GDK_KEY_RELEASE_MASK           |
                                 GDK_BUTTON_PRESS_MASK          |
                                 GDK_BUTTON_RELEASE_MASK        |
                                 GDK_POINTER_MOTION_MASK        |
                                 GDK_POINTER_MOTION_HINT_MASK );

/*    g_signal_connect ( G_OBJECT (drw), "size-allocate"       , G_CALLBACK (gtk3_sizeGL ), view );
    g_signal_connect ( G_OBJECT (drw), "realize"             , G_CALLBACK (gtk3_initGL ), view );
*/    
    g_signal_connect ( G_OBJECT (drw), "delete-event",
		                               G_CALLBACK (objectlistarea_delete), gui);

    g_signal_connect ( G_OBJECT (drw), "draw"                ,
                                       G_CALLBACK (objectlistarea_draw ), opd );

    g_signal_connect ( G_OBJECT (drw), "motion_notify_event" ,
                                       G_CALLBACK (objectlistarea_input), gui );

    g_signal_connect ( G_OBJECT (drw), "button_press_event"  ,
                                       G_CALLBACK (objectlistarea_input), gui );

    g_signal_connect ( G_OBJECT (drw), "button_release_event",
                                       G_CALLBACK (objectlistarea_input), gui );

    g_signal_connect ( G_OBJECT (drw), "key_press_event"     ,
                                       G_CALLBACK (objectlistarea_input), gui );

    g_signal_connect ( G_OBJECT (drw), "key_release_event"   ,
                                       G_CALLBACK (objectlistarea_input), gui );

    gtk_widget_set_size_request ( drw, drwrec.width, drwrec.height );

#if GTK_CHECK_VERSION(3,8,0)
    gtk_container_add ( GTK_CONTAINER(scr), drw );
#else
    gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW(scr), drw );
#endif

    list_insert ( &gui->lobjlist, drw );

    gtk_widget_show ( drw );
    gtk_widget_show ( scr );


    return scr;
}
