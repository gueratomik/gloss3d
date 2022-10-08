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
#include <g3dui_motif.h>

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

/*******************************************************************************/
static uint32_t getObjectNameWidth ( XmFontList fontlist, G3DOBJECT *obj ) {
    XmString str       = XmStringCreate ( obj->name, XmFONTLIST_DEFAULT_TAG );
    uint32_t strwidth  = XmStringWidth  ( fontlist, str );

    XmStringFree ( str );


    return strwidth;
}

/*******************************************************************************/
static uint32_t getSeparator_r ( XmFontList fontlist, G3DOBJECT *obj,
                                                      uint32_t x,
                                                      uint32_t y,
                                                      uint32_t xindent,
                                                      uint32_t yindent ) {
    uint32_t strwidth = getObjectNameWidth ( fontlist, obj );
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
            getSeparator_r ( fontlist, child, x + xindent, curheight, xindent,
                                                                      yindent );

            ltmpobj = ltmpobj->next;
        }
    }

    return sepx;
}

/******************************************************************************/
PICKEDOBJECT *selectObject_r ( XmFontList fontlist, G3DOBJECT *obj,
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
                                                         uint64_t engine_flags ) {
    LIST *ltmpobj = obj->lchildren;
    PICKEDOBJECT *pob;
    uint32_t strwidth;
    static curheight;

    strwidth = getObjectNameWidth ( fontlist, obj );

    /*** We use static variable property to position objects in height ***/
    if ( obj->parent == NULL ) curheight  = yindent;
    else                       curheight += yindent;

    /*** SCENE object is not pickable ***/
    if ( obj->type != G3DSCENETYPE ) {
        pob = pickobject ( x, y, xsep, xm, ym, strwidth, obj, sce, urm, keep, engine_flags );

        if ( pob ) {
            return pob;
        }
    }

    if ( ( obj->flags & OBJECTCOLLAPSED ) == 0x00 ) {
        /*** Pick the children ***/
        while ( ltmpobj ) {
            G3DOBJECT *child = ( G3DOBJECT * ) ltmpobj->data;
            G3DOBJECT *sel;

            if ( ( sel = selectObject_r ( fontlist, child,
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
static void drawActive ( Display *dis, Window win, 
                                       GC gc,
                                       G3DOBJECT *obj,
                                       G3DUIRECTANGLE *rec ) {
}

/******************************************************************************/
static void drawVisible ( Display *dis, Window win, 
                                        GC gc,
                                        G3DOBJECT *obj,
                                        G3DUIRECTANGLE *rec ) {
}

/******************************************************************************/
static void drawTextures ( Display *dis, Window win, 
                                         GC gc,
                                         G3DOBJECT *obj, 
                                         G3DUIRECTANGLE *rec ) {
    static G3DUIMATERIALMAP *matmap;
    static unsigned char pixel[0x100][0x03];

    /*** Init once and for all ***/
    if ( matmap == NULL ) matmap = common_g3duimaterialmap_new ( LISTINDENT,
                                                                 LISTINDENT );

    if ( obj->type & MESH ) {
        G3DMESH *mes =  ( G3DMESH * ) obj;
        LIST *ltmptex = mes->ltex;
        uint32_t nbtex = 0x00, i, j;

            while ( ltmptex ) {
                G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;
                Visual *vis = DefaultVisual ( dis, 0x00 );
                int depth = XDefaultDepth ( dis, 0x00 );
                XImage *img;

                common_g3duimaterialmap_fillData ( matmap, tex->mat, pixel );

                img = XCreateImage ( dis, CopyFromParent, depth, ZPixmap, 0, NULL, LISTINDENT, LISTINDENT, 0x20, 0x00 );

                img->data = calloc ( img->height, img->bytes_per_line );

                XInitImage ( img );

                for ( i = 0x00; i < img->height; i++ ) {
                    for ( j = 0x00; j < img->width; j++ ) {
                        uint32_t r = pixel[(i*0x10)+j][0],
                                 g = pixel[(i*0x10)+j][1],
                                 b = pixel[(i*0x10)+j][2];

                        img->f.put_pixel ( img, j, i, ( r << 0x10 ) | ( g << 0x08 ) | b );
                    }
                }

                XPutImage ( dis, win, gc, img, 0x00, 0x00,
                                               rec[nbtex].x + 0x01,
                                               rec[nbtex].y + 0x01, 
                                               img->width,
                                               img->height );

                XDestroyImage ( img );

                if ( tex->flags & TEXTURESELECTED ) {
                    XSetForeground ( dis, gc, 0xFF0000 );
                    XDrawRectangle ( dis, win, gc, rec[nbtex].x,
                                                   rec[nbtex].y, 
                                                   rec[nbtex].width,
                                                   rec[nbtex].height );
                }

                nbtex++;

                ltmptex = ltmptex->next;
            }
    }
}

/******************************************************************************/
static void drawObjectName ( Display *dis, Window win, 
                                           GC gc,
                                           XmFontList ftl,
                                           G3DOBJECT *obj,
                                           G3DUIRECTANGLE *rec  ) {
    unsigned long whitepixel = XWhitePixel ( dis, 0x00 ),
                  blackpixel = XBlackPixel ( dis, 0x00 );

    if ( obj->name ) {
        XmString str = XmStringCreate ( obj->name, XmFONTLIST_DEFAULT_TAG );

        if ( obj->flags & OBJECTSELECTED ) {
            XSetForeground ( dis, gc, 0xFF0000 );
        } else {
            XSetForeground ( dis, gc, blackpixel );
        }

        /*** Draw the object name ***/
        XmStringDraw ( dis, win, ftl, str, gc, rec->x, 
                                               rec->y,
                                               rec->width,
                                               XmALIGNMENT_BEGINNING, 
                                               0x00,
                                               NULL );

        XmStringFree ( str );
    }
}

/******************************************************************************/
static void drawObjectIcon ( Display *dis, Window win, 
                                           GC gc,
                                           Pixel bg,
                                           G3DOBJECT *obj,
                                           G3DUIRECTANGLE *rec ) {
    static Pixmap bonico, ligico, mesico,
                  defico, symico, cubico,
                  sphico, torico, conico,
                  plnico, ffdico, cylico,
                  camico, scnico, mapico, objico;

    /*********************** Inited once and for all **************************/
    if ( bonico == 0x00 ) bonico = createSimplePixmap ( dis, bonico_xpm, bg );
    if ( ligico == 0x00 ) ligico = createSimplePixmap ( dis, ligico_xpm, bg );
    if ( mesico == 0x00 ) mesico = createSimplePixmap ( dis, mesico_xpm, bg );
    if ( defico == 0x00 ) defico = createSimplePixmap ( dis, defico_xpm, bg );
    if ( symico == 0x00 ) symico = createSimplePixmap ( dis, symico_xpm, bg );
    if ( cubico == 0x00 ) cubico = createSimplePixmap ( dis, cubico_xpm, bg );
    if ( sphico == 0x00 ) sphico = createSimplePixmap ( dis, sphico_xpm, bg );
    if ( torico == 0x00 ) torico = createSimplePixmap ( dis, torico_xpm, bg );
    /*if ( conico == 0x00 ) conico = gdk_pixbuf_new_from_xpm_data ( conico_xpm );*/
    if ( plnico == 0x00 ) plnico = createSimplePixmap ( dis, plnico_xpm, bg );
    if ( ffdico == 0x00 ) ffdico = createSimplePixmap ( dis, ffdico_xpm, bg );
    if ( cylico == 0x00 ) cylico = createSimplePixmap ( dis, cylico_xpm, bg );
    if ( camico == 0x00 ) camico = createSimplePixmap ( dis, camico_xpm, bg );
    if ( scnico == 0x00 ) scnico = createSimplePixmap ( dis, scnico_xpm, bg );
    if ( mapico == 0x00 ) mapico = createSimplePixmap ( dis, mapico_xpm, bg );
   /***************************************************************************/

    switch ( obj->type ) {
        case G3DLIGHTTYPE    : objico = ligico; break;
        case G3DBONETYPE     : objico = bonico; break;
        case G3DMESHTYPE     : objico = mesico; break;
        case G3DSYMMETRYTYPE : objico = symico; break;
        case G3DCUBETYPE     : objico = cubico; break;
        case G3DSPHERETYPE   : objico = sphico; break;
        case G3DTORUSTYPE    : objico = torico; break;
        case G3DCONETYPE     : objico = conico; break;
        case G3DPLANETYPE    : objico = plnico; break;
        case G3DFFDTYPE      : objico = ffdico; break;
        case G3DCYLINDERTYPE : objico = cylico; break;
        case G3DCAMERATYPE   : objico = camico; break;
        case G3DSCENETYPE    : objico = scnico; break;
        case G3DUVMAPTYPE    : objico = mapico; break;
        default              : objico = defico; break;
    }

    XCopyArea ( dis, objico, win, gc, 0x00, 0x00, rec->width,
                                                  rec->height,
                                                  rec->x,
                                                  rec->y );
}

/******************************************************************************/
static void drawArrow ( Display *dis, Window win, 
                                      GC gc, 
                                      G3DOBJECT *obj,
                                      G3DUIRECTANGLE *rec ) {
}

/******************************************************************************/
static uint32_t printObject_r ( Display *dis, Window win, 
                                              GC gc,
                                              Pixel bg,
                                              XmFontList ftl,
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

    strwidth = getObjectNameWidth ( ftl, obj );

    lob = common_g3duilist_sizeListedObject ( obj, x, y, xsep, strwidth, 0x00 );

    if ( maxwidth < lob->endx ) maxwidth = lob->endx;

    drawArrow      ( dis, win, gc,      obj, &lob->expand  );
    drawObjectIcon ( dis, win, gc, bg , obj, &lob->icon    );
    drawObjectName ( dis, win, gc, ftl, obj, &lob->name    );
    drawActive     ( dis, win, gc,      obj, &lob->active  );
    drawVisible    ( dis, win, gc,      obj, &lob->visible );
    drawTextures   ( dis, win, gc,      obj,  lob->texture );

    /*** Recurse ***/
    if ( ( obj->flags & OBJECTCOLLAPSED ) == 0x00 ) {
        /*** Draw the children ***/
        while ( ltmpobj ) {
            G3DOBJECT *child = ( G3DOBJECT * ) ltmpobj->data;

            printObject_r ( dis, win,
                                 gc,
                                 bg,
                                 ftl, 
                                 child, 
                             x + xindent, 
                                 curheight, 
                                 xsep, 
                                 xindent,
                                 yindent );


            ltmpobj = ltmpobj->next;
        }
    }

    maxheight = ( curheight );


    return ( ( maxheight << 0x10 ) | maxwidth );
}

/******************************************************************************/
static void Input ( Widget w, XtPointer client, 
                              XEvent *event,
                              Boolean *continue_to_dispatch ) {
    G3DUI    *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    static G3DOBJECT *obj, *dst;
    static PICKEDOBJECT *pob;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    switch ( event->type ) {
        case KeyPress : {
            XKeyEvent *kev = ( XKeyEvent * ) event;
            KeySym keysym;
            char c;

            if ( XLookupString ( kev, &c, 0x01, &keysym, NULL ) > 0x00 ) {
                switch ( keysym ) {
                    case XK_Delete: {
                        uint32_t retflags = ( REDRAWVIEW | REDRAWOBJECTLIST );

                        if ( pob && ( pob->picked == TEXTURERECTHIT ) ) {
                            g3dmesh_removeMaterial ( ( G3DMESH * ) pob->obj, pob->tex->mat );
                        } else {
                            g3durm_scene_deleteSelectedObjects ( urm, sce, gui->engine_flags, retflags );
                        }

                        pob = NULL;
                    } break;
 
                    /*** This piece of code is also in g3duiview ****/
                    /*** Ctrl+c is copy object ***/
                    case XK_c :
                        if ( kev->state & ControlMask ) {
printf("copying %d object(s)\n", list_count ( sce->lsel ) );
                           g3dui_setHourGlass ( gui );
                           /*g3duiclipboard_copyObject ( cli, sce, sce->lsel, 0x01, gui->engine_flags );*/
                           g3dui_unsetHourGlass ( gui );
                        }
                    break;

                    /*** Ctrl+v is paste object ***/
                    case XK_v :
                        if ( kev->state & ControlMask ) {
                           G3DOBJECT *src = g3dscene_getSelectedObject ( sce );
                           G3DOBJECT *dst = ( G3DOBJECT * ) sce;

                           /*** add the copied object to the   ***/
                           /*** selected object's parent objet ***/
                           if ( src ) dst = src->parent;
printf("pasting\n");
                           g3dui_setHourGlass ( gui );
                           /*g3duiclipboard_paste ( cli, urm, sce, dst, gui->engine_flags );*/
                           g3dui_unsetHourGlass ( gui );
                        }
                    break;
                    /*************************************************/

                    default :
                    break;
                }
            }

            g3dui_redrawObjectList     ( gui );
            g3dui_updateAllCurrentEdit ( gui );
            g3dui_redrawGLViews        ( gui );
        } break;

        case ButtonPress : {
            XButtonEvent *bev = ( XButtonEvent * ) event;
            uint32_t pick_flags = ( bev->state & ControlMask ) ? PICKEOBJECTKEEPALL : 0x00;
            /*** Retrieve separator's position ***/
            uint32_t xsep = getSeparator_r ( gmt->fontlist, ( G3DOBJECT * ) sce,
                                                      0x00,
                                                      0x00,
                                                      LISTINDENT,
                                                      LISTINDENT );

            LIST *lmes = NULL;

            /*** .Unselect previously selected texture tags ***/
            /*** .Free the list and clear texture selection ***/
            g3dobject_getObjectsByType_r ( ( G3DOBJECT * ) sce, G3DMESHTYPE, &lmes );

            list_free ( &lmes, (void (*)(void *)) g3dmesh_unselectAllTextures );

            pob = selectObject_r ( gmt->fontlist, ( G3DOBJECT * ) sce,
                                            0x00,
                                            0x00,
                                            xsep,
                                            LISTINDENT, 
                                            LISTINDENT,
                                            bev->x,
                                            bev->y,
                                            sce, urm, pick_flags, gui->engine_flags );


            if ( pob ) {
                obj = pob->obj;

                /*g3dui_updateCoords ( gui );
                g3dui_redrawTimeline ( gui );*/
            }

            g3dui_redrawObjectList ( gui );
            g3dui_redrawGLViews ( gui );
            g3dui_updateAllCurrentEdit ( gui );
            g3dui_redrawTimeline ( gui );
        } break;

        case MotionNotify : {
            XMotionEvent *mev = ( XMotionEvent * ) event;

            if ( obj ) {
                /*** Retrieve separator's position ***/
                uint32_t xsep = getSeparator_r ( gmt->fontlist, ( G3DOBJECT * ) sce,
                                                          0x00,
                                                          0x00,
                                                          LISTINDENT,
                                                          LISTINDENT );

                /*** drag and drop ***/
                pob = selectObject_r ( gmt->fontlist, ( G3DOBJECT * ) sce,
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

        case ButtonRelease : {
            XButtonEvent *bev = ( XButtonEvent * ) event;

                if ( ( obj        ) && /*** If we pressed on an object   ***/
                     ( dst        ) && /*** And we released on an object ***/
                     ( dst != obj ) && /*** and they are different       ***/
                     /*** and the future child is not the current parent ***/
                     ( g3dobject_isChild ( obj, dst ) == 0x00 ) ) {
                    G3DOBJECT *par = obj->parent;

                    /*** Perform action & record for the undo-redo manager ***/
                    g3durm_object_addChild ( urm, sce, gui->engine_flags,
                                                       ( REDRAWVIEW |
                                                         REDRAWOBJECTLIST | 
                                                         UPDATECURRENTOBJECT ),
                                           ( G3DOBJECT * ) par,
                                           ( G3DOBJECT * ) dst,
                                           ( G3DOBJECT * ) obj );

                    if ( ( dst->type & G3DSYMMETRYTYPE ) &&
                         ( obj->type & G3DMESHTYPE ) ) {
                        g3dsymmetry_meshChildChange ( ( G3DSYMMETRY * ) dst, ( G3DMESH * ) obj );

                        g3dmesh_update ( ( G3DMESH * ) obj, NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              UPDATEFACEPOSITION |
                                              COMPUTEEDGEPOSITION |
                                              COMPUTESUBDIVISION, gui->engine_flags );
                    }

                    g3dui_updateCoords ( gui );
                }

            g3dui_redrawObjectList ( gui );
            g3dui_redrawGLViews ( gui );
            g3dui_redrawTimeline ( gui );

            obj = dst = NULL;
        } break;

        default:
        break;
    }
}

#ifdef OLDCODE

/******************************************************************************/
static void Input ( Widget w, XtPointer client, 
                              XEvent *event,
                              Boolean *continue_to_dispatch ) {
    Pixel background, foreground;
    static G3DOBJECT *obj, *dst;
    static PICKEDOBJECT *pob;
    G3DUICLIPBOARD *cli;
    G3DURMANAGER *urm;
    static Time click;
    G3DSCENE *sce;
    G3DUI *gui;
    G3DUIMOTIF *gmt;

    /*** retrieve the global GUI structure ***/
    XtVaGetValues ( w, XmNuserData, &gui,
                       XmNbackground, &background,
                       XmNforeground, &foreground, NULL );
    urm = gui->urm;
    sce = gui->sce;
    cli = gui->cli;

    gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    switch ( event->type ) {
        case KeyPress : {
            XKeyEvent *kev = ( XKeyEvent * ) event;
            KeySym keysym;
            char c;

            if ( XLookupString ( kev, &c, 0x01, &keysym, NULL ) > 0x00 ) {
                switch ( keysym ) {
                    case XK_Delete: {
                        uint32_t retflags = ( REDRAWVIEW | REDRAWOBJECTLIST );

                        if ( pob && ( pob->picked == TEXTURERECTHIT ) ) {
                            g3dmesh_removeMaterial ( ( G3DMESH * ) pob->obj, pob->tex->mat );
                        } else {
                            g3durm_scene_deleteSelectedObjects ( urm, sce, gui->engine_flags, retflags );
                        }

                        pob = NULL;
                    } break;
 
                    /*** This piece of code is also in g3duiview ****/
                    /*** Ctrl+c is copy object ***/
                    case XK_c :
                        if ( kev->state & ControlMask ) {
printf("copying %d object(s)\n", list_count ( sce->lsel ) );
                           g3dui_setHourGlass ( gui );
                           g3duiclipboard_copyObject ( cli, sce, sce->lsel, 0x01, gui->engine_flags );
                           g3dui_unsetHourGlass ( gui );
                        }
                    break;

                    /*** Ctrl+v is paste object ***/
                    case XK_v :
                        if ( kev->state & ControlMask ) {
                           G3DOBJECT *src = g3dscene_getSelectedObject ( sce );
                           G3DOBJECT *dst = ( G3DOBJECT * ) sce;

                           /*** add the copied object to the   ***/
                           /*** selected object's parent objet ***/
                           if ( src ) dst = src->parent;
printf("pasting\n");
                           g3dui_setHourGlass ( gui );
                           g3duiclipboard_paste ( cli, urm, sce, dst, gui->engine_flags );
                           g3dui_unsetHourGlass ( gui );
                        }
                    break;
                    /*************************************************/

                    default :
                    break;
                }
            }

            g3dui_redrawObjectList     ( gui );
            g3dui_updateAllCurrentEdit ( gui );
            g3dui_redrawGLViews        ( gui );
        } break;

        case ButtonPress : {
            XButtonEvent *bev = ( XButtonEvent * ) event;
            Dimension xpos  = 0x00, ypos = 0x00;
            Dimension xsep0 = 0x00, xsep1 = 0x00, width = 0x00;
            int doubleclick = 0x00;

            if ( gui && gui->sce ) {
                /*** if we got a first click, check the difference time ***/
                if ( click ) {
                    Time dift = ( bev->time - click );

                    /*** double-click on the same tile as before ***/
                    if ( dift < 500 ) {
                        doubleclick = 0x01;
                    }

                    /*** no double-click ***/          
                    click = 0x00;
                /*** or else, set the initial values ***/
                } else {
                    click = bev->time;
                }

                getSeparators_r ( gmt->fontlist, 0x00, &xsep0, 
                                                       &xsep1, ( G3DOBJECT * ) gui->sce );

                pob = selectObject_r ( XtDisplay ( w ), XtWindow ( w ),
                                       gmt->fontlist,
                                       &xpos, &ypos,
                                       bev->x, bev->y,
                                       xsep0, xsep1,
                                       ( G3DOBJECT * ) gui->sce );

                

                /*** if an object was picked ***/
                if ( pob ) {
                    LIST *lselold = NULL, 
                         *lselnew = NULL;
                    obj = pob->obj;

                    /*** SCENE object is not pickable ***/
                    if ( obj->type != G3DSCENETYPE ) {
                        LIST *lmes = NULL;


                        /*** .Unselect previously selected texture tags ***/
                        /*** .Free the list and clear texture selection ***/
                        g3dobject_getObjectsByType_r ( ( G3DOBJECT * ) gui->sce, G3DMESHTYPE, &lmes );

                        list_free ( &lmes, (void (*)(void *)) g3dmesh_unselectAllTextures );
                        /**************************************************/

                        /*** Proceed with picking ***/
                        pickedobject_parse ( pob );

                        if ( ( pob->picked == TEXTURERECTHIT ) && doubleclick ) {
                            Display *dis = XtDisplay ( w );
                            Window root = XDefaultRootWindow ( dis );
                            XWindowAttributes wat;
                            Widget shell, edit;

                            XGetWindowAttributes ( dis, root, &wat );

                            shell = XtVaAppCreateShell ( NULL, "Class",
                                                         topLevelShellWidgetClass,
                                                         dis, 
                                                         XtNtitle,"Texture Editor",
                                                         XtNx, ( wat.width/2) -160,
                                                         XtNy, ( wat.height/2) -30,
                                                         XtNforeground, foreground,
                                                         XtNbackground, background,
                                                         XtNwidth, 320,
                                                         XtNheight, 60,
                                                         NULL );
   
                            createTextureEdit ( shell, gui, "Texture Edit",
                                                0x00, 0x00,
                                                320 , 60 );

                            /*XtManageChild ( edit );*/

                            XtRealizeWidget ( shell );
                        }

                        lselold = list_copy ( sce->lsel );

                        /*** Press CTRL to select multiple objects ***/
                        if ( ( bev->state & ControlMask ) == 0x00 ) {
                            g3dscene_unselectAllObjects ( gui->sce, gui->engine_flags );
                        }

                        g3dscene_selectObject ( gui->sce, obj, gui->engine_flags );

                        lselnew = list_copy ( sce->lsel );

                        /*** remember selection ***/
                        g3durm_scene_pickObject  ( urm, sce,
                                                        lselold,
                                                        lselnew,
                                                        gui->engine_flags,
                                                        REDRAWVIEW );
                    }
                }
            }

            if ( obj ) {
                g3dui_updateCoords ( gui );
                redrawTimeline ( gui );
            }

            g3dui_redrawObjectList ( gui );
            g3dui_redrawGLViews ( gui );
            g3dui_updateAllCurrentEdit ( gui );
        } break;

        /*** Drag ***/
        case MotionNotify : {
            XMotionEvent *mev = ( XMotionEvent * ) event;
            Dimension xpos = 0x00, ypos = 0x00;
            Dimension xsep0 = 0x00, xsep1 = 0x00, width;

            if ( gui && gui->sce && obj ) {
                getSeparators_r ( gmt->fontlist, 0x00, &xsep0,
                                                       &xsep1, ( G3DOBJECT * ) gui->sce );

                /*** drag and drop ***/
                pob = selectObject_r ( XtDisplay ( w ), XtWindow ( w ),
                                       gmt->fontlist,
                                       &xpos, &ypos,
                                       mev->x,
                                       mev->y,
                                       xsep0, xsep1,
                                       ( G3DOBJECT * ) gui->sce );

                dst = ( pob ) ? pob->obj : NULL;
            }

            click = 0x00;
        } break;

        /*** 'n Drop ***/
        case ButtonRelease : {
            /*XButtonEvent *bev = ( XButtonEvent * ) event;*/

/*** Need for keyboard input !!! ***/
XmProcessTraversal(w, XmTRAVERSE_CURRENT);

            if ( gui && gui->sce ) {
                if ( ( obj        ) && /*** If we pressed on an object   ***/
                     ( dst        ) && /*** And we released on an object ***/
                     ( dst != obj ) && /*** and they are different       ***/
                     /*** and the future child is not the current parent ***/
                     ( g3dobject_isChild ( obj, dst ) == 0x00 ) ) {
                    G3DOBJECT *par = obj->parent;

                    /*** Perform action & record for the undo-redo manager ***/
                    g3durm_object_addChild ( urm, sce, gui->engine_flags,
                                                       ( REDRAWVIEW |
                                                         REDRAWOBJECTLIST | 
                                                         UPDATECURRENTOBJECT ),
                                           ( G3DOBJECT * ) par,
                                           ( G3DOBJECT * ) dst,
                                           ( G3DOBJECT * ) obj );

                    if ( ( dst->type & G3DSYMMETRYTYPE ) &&
                         ( obj->type & G3DMESHTYPE ) ) {
                        g3dsymmetry_meshChildChange ( ( G3DSYMMETRY * ) dst, ( G3DMESH * ) obj );

                        g3dmesh_update ( ( G3DMESH * ) obj, NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              UPDATEFACEPOSITION |
                                              COMPUTEEDGEPOSITION |
                                              COMPUTESUBDIVISION, gui->engine_flags );
                    }

                    g3dui_updateCoords ( gui );
                }
            }

            g3dui_redrawObjectList ( gui );
            g3dui_redrawGLViews ( gui );

            obj = dst = NULL;
        } break;

        default :
        break;
    }
}
#endif

/******************************************************************************/
static void Redisplay ( Widget w, XtPointer client, XtPointer call ) {
    Display *dis = XtDisplay ( w );
    Window win = XtWindow ( w );
    XGCValues values;
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Dimension winwidth = 0x0, winheight = 0x00, gotwidth, gotheight;
    uint32_t size;
    Pixel background;
    /*** Retrieve separator's position ***/
    uint32_t xsep = getSeparator_r ( gmt->fontlist, ( G3DOBJECT * ) sce,
                                                    0x00,
                                                    0x00,
                                                    LISTINDENT,
                                                    LISTINDENT );
    GC gc = XtGetGC ( w, 0x00, &values );
    Pixel toppix, botpix;

    XtVaGetValues ( w, XmNwidth            , &winwidth,
                       XmNheight           , &winheight,
                       XmNbackground       , &background,
                       XmNtopShadowColor   , &toppix, /* For the vertical bar */
                       XmNbottomShadowColor, &botpix, /* For the vertical bar */
                       NULL );

    XSetForeground ( dis, gc, botpix );
    XDrawLine      ( dis, win, gc, xsep    , 0x00, xsep    , winheight );
    XSetForeground ( dis, gc, toppix );
    XDrawLine      ( dis, win, gc, xsep + 1, 0x00, xsep + 1, winheight );

    /*** Draw objects ***/
    size = printObject_r ( dis, win,
                                gc,
                                background,
                                gmt->fontlist,
                                sce, 
                                0x00, 
                                0x00, 
                                xsep, 
                                LISTINDENT, 
                                LISTINDENT );

    if ( size ) {
        XtMakeResizeRequest ( w, size & 0x0000FFFF,
                                 size >> 0x10, 
                                &gotwidth, 
                                &gotheight );

        if ( gotwidth && gotheight ) {
            XtResizeWidget ( w, gotwidth, gotheight, 0x00 );
        }
    }

    XtReleaseGC ( w, gc );
}

/******************************************************************************/
Widget createObjectList ( Widget parent, G3DUI *gui, char *name,
                          Dimension x    , Dimension y,
                          Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget drw;
    Widget scr;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    scr = XmVaCreateScrolledWindow ( parent, name,
                                             XmNx, x,
                                             XmNy, y,
                                             XmNwidth , width,
                                             XmNheight, height,
                                             XmNscrollingPolicy, XmAUTOMATIC,
                                             XmNspacing, 0x00,
                                             XmNforeground, foreground,
                                             XmNbackground, background,
                                             XmNtopShadowColor, background,
                                             XmNbottomShadowColor, background,
                                             XmNshadowThickness, 0x01,
                                             NULL );

    drw = XmVaCreateDrawingArea ( scr, name,
                                          XtNx, 0,
                                          XtNy, 0,
                                          XtNwidth , width,
                                          XtNheight, height,
                                          /*** needed for keyboard input ***/
                                          XmNtraversalOn,   TRUE,
                                          /*XmNancestorSensitive, TRUE,
                                          XmNsensitive, TRUE,
                                          XmNkeyboardFocusPolicy, XmEXPLICIT,*/
                                          NULL );

    XtAddCallback ( drw, XmNexposeCallback, Redisplay, gui );

    XtAddEventHandler ( drw, ButtonPressMask   | 
                             ButtonReleaseMask | 
                             KeyPressMask      | 
                             KeyReleaseMask    | 
                             PointerMotionMask, False, Input, gui );

    XtManageChild ( drw );

    /*** add this widget to the list of ObjectList widget for easy update ***/
    list_insert ( &gui->lobjlist, drw );

    XtManageChild ( scr );


    return drw;
}
