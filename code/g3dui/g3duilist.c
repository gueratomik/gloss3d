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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2014         */
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
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

/******************************************************************************/
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

/******************************************************************************/
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

/******************************************************************************/
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/PushB.h>
#include <Xm/Manager.h>
#include <Xm/ManagerP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/ScrolledW.h>

/******************************************************************************/
#include <X11/xpm.h>

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/GLwMDrawA.h>

/******************************************************************************/
#include <list.h>
#include <g3d.h>
#include <r3d.h>
#include <r3dnet.h>
#include <g3durmanager.h>
#include <g3dmouse.h>
#include <g3dui.h>

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

/************* THIS WIDGET IS TO COMPLICATED. SHOULD BE REWRITTEN *************/

/******************************************************************************/
static XtProc ClassInitialize ( void );
static XtInitProc Initialize  ( Widget, Widget, ArgList, Cardinal * );
static XtRealizeProc Realize  ( Widget, XtValueMask *, XSetWindowAttributes * );
static XtWidgetProc Destroy   ( Widget );
static XtWidgetProc Resize    ( Widget );
static XtExposeProc Redisplay ( Widget, XEvent *, Region );

/******************************************************************************/
static void drawActive ( Display *dis, Window win, GC gc, int x, int y ) {
    XPoint csr[] = {                                    {6,0},{7,0},
                                                  {5,1},{6,1}, 
                                                  {5,2},{6,2}, 
                                            {4,3},{5,3},       
                                            {4,4},{5,4},       
                                {2,5},      {4,5},{5,5},       
                                {2,6},{3,6},{4,6},             
                                      {3,7},{4,7},             
                                      {3,8},{4,8},                        };
    uint32_t i;

    for ( i = 0x00; i < 20; i++ ) {
        csr[i].x += x;
        csr[i].y += y;
    }

    /*if ( ( x >= 0x00 ) && ( x <= width ) ) {*/
        XDrawPoints ( dis, win, gc, csr, 20, CoordModeOrigin );
    /*}*/
}

/******************************************************************************/
static void drawInactive ( Display *dis, Window win, GC gc, int x, int y ) {
    XPoint csr[] = {      {1,0},{2,0},                  {6,0},{7,0},
                          {1,1},{2,1},                  {6,1},{7,1},
                                {2,2},{3,2},      {5,2},{6,2},
                                {2,3},{3,3},      {5,3},{6,3},
                                      {3,4},{4,4},{5,4},
                                {2,5},{3,5},      {5,5},{6,5},
                                {2,6},{3,6},      {5,6},{6,6},
                          {1,7},{2,7},                  {6,7},{7,7},
                          {1,8},{2,8},                  {6,8},{7,8}      };
    uint32_t i;

    for ( i = 0x00; i < 35; i++ ) {
        csr[i].x += x;
        csr[i].y += y;
    }

    /*if ( ( x >= 0x00 ) && ( x <= width ) ) {*/
        XDrawPoints ( dis, win, gc, csr, 35, CoordModeOrigin );
    /*}*/
}

/******************************************************************************/
static void drawClosedEye ( Display *dis, Window win, GC gc, int x, int y ) {
    XPoint csr[] = {                  {3,0},{4,0},{5,0},
                                {2,1},                  {6,1},      
                          {1,2},                              {7,2},
                    {0,3},{1,3},{2,3},{3,3},{4,3},{5,3},{6,3},{7,3},{8,3},
                          {1,4},                              {7,4},
                                {2,5},                  {6,5},
                                      {3,6},{4,6},{5,6} };
    uint32_t i;

    for ( i = 0x00; i < 23; i++ ) {
        csr[i].x += x;
        csr[i].y += y;
    }

    /*if ( ( x >= 0x00 ) && ( x <= width ) ) {*/
        XDrawPoints ( dis, win, gc, csr, 23, CoordModeOrigin );
    /*}*/
}

/******************************************************************************/
static void drawOpenedEye ( Display *dis, Window win, GC gc, int x, int y ) {
    XPoint csr[] = {                  {3,0},{4,0},{5,0},
                                {2,1},      {4,1},      {6,1},      
                          {1,2},      {3,2},{4,2},{5,2},      {7,2},
                    {0,3},            {3,3},      {5,3},            {8,3},
                          {1,4},      {3,4},{4,4},{5,4},      {7,4},
                                {2,5},      {4,5},      {6,5},
                                      {3,6},{4,6},{5,6} };
    uint32_t i;

    for ( i = 0x00; i < 26; i++ ) {
        csr[i].x += x;
        csr[i].y += y;
    }

    /*if ( ( x >= 0x00 ) && ( x <= width ) ) {*/
        XDrawPoints ( dis, win, gc, csr, 26, CoordModeOrigin );
    /*}*/
}

/******************************************************************************/
static void drawMinus ( Display *dis, Window win, GC gc, int x, int y ) {
    XPoint csr[] = {{0,0},{1,0},{2,0},{3,0},{4,0},{5,0},{6,0},{7,0},{8,0},
                    {0,1},                                          {8,1},
                    {0,2},                                          {8,2},
                    {0,3},                                          {8,3},
                    {0,4},      {2,4},{3,4},{4,4},{5,4},{6,4},      {8,4},
                    {0,5},                                          {8,5},
                    {0,6},                                          {8,6},
                    {0,7},                                          {8,7},
                    {0,8},{1,8},{2,8},{3,8},{4,8},{5,8},{6,8},{7,8},{8,8}};
    uint32_t i;

    for ( i = 0x00; i < 37; i++ ) {
        csr[i].x += x;
        csr[i].y += y;
    }

    /*if ( ( x >= 0x00 ) && ( x <= width ) ) {*/
        XDrawPoints ( dis, win, gc, csr, 37, CoordModeOrigin );
    /*}*/
}

/******************************************************************************/
static void drawPlus ( Display *dis, Window win, GC gc, int x, int y ) {
    XPoint csr[] = {{0,0},{1,0},{2,0},{3,0},{4,0},{5,0},{6,0},{7,0},{8,0},
                    {0,1},                                          {8,1},
                    {0,2},                  {4,2},                  {8,2},
                    {0,3},                  {4,3},                  {8,3},
                    {0,4},      {2,4},{3,4},{4,4},{5,4},{6,4},      {8,4},
                    {0,5},                  {4,5},                  {8,5},
                    {0,6},                  {4,6},                  {8,6},
                    {0,7},                                          {8,7},
                    {0,8},{1,8},{2,8},{3,8},{4,8},{5,8},{6,8},{7,8},{8,8}};
    uint32_t i;

    for ( i = 0x00; i < 41; i++ ) {
        csr[i].x += x;
        csr[i].y += y;
    }

    /*if ( ( x >= 0x00 ) && ( x <= width ) ) {*/
        XDrawPoints ( dis, win, gc, csr, 41, CoordModeOrigin );
    /*}*/
}

/*******************************************************************************
* This function sizes the area where our object is going to be displayed.
* This function is called when displaying and selecting.
* Texture tags are displayed after the vertical line drawn by the separator.
*******************************************************************************/
static LISTEDOBJECT *sizelistedobject ( G3DOBJECT *obj, Dimension x, 
                                                    Dimension y,
                                                    Dimension *xsep0,
                                                    Dimension *xsep1,
                                                    Dimension *width,
                                                    XmFontList fontlist,
                                                    uint32_t query_flags ) {
    XmString str       = XmStringCreate ( obj->name, XmFONTLIST_DEFAULT_TAG );
    uint32_t strwidth  = XmStringWidth  ( fontlist, str );
    static LISTEDOBJECT lob; /*** Defined as static variable so we can    ***/
                           /*** return a valid pointer to this variable ***/
    uint32_t nextx;

    lob.hit = 0x00;

    /*** The expand icon (+) or (-) ***/
    lob.expand.x       = x;
    lob.expand.y       = y;
    lob.expand.width   = LISTINDENT;
    lob.expand.height  = LISTINDENT;

    /*** The object type nice icon ***/
    lob.icon.x         = lob.expand.x + lob.expand.width;
    lob.icon.y         = y;
    lob.icon.width     = LISTINDENT;
    lob.icon.height    = LISTINDENT;

    /*** The object name area ***/
    lob.name.x         = lob.icon.x   + lob.icon.width + 0x04;
    lob.name.y         = y;
    lob.name.width     = strwidth;
    lob.name.height    = LISTINDENT;

    if ( query_flags & GETLISTSEPARATORS && xsep0 ) {
        (*xsep0) = ( lob.name.x + lob.name.width ) + 0x04;
    }

    lob.active.x       = (*xsep0) + 0x04;
    lob.active.y       = y;
    lob.active.width   = LISTINDENT;
    lob.active.height  = LISTINDENT;

    lob.visible.x      = lob.active.x + lob.active.width;
    lob.visible.y      = y;
    lob.visible.width  = LISTINDENT;
    lob.visible.height = LISTINDENT;

    if ( query_flags & GETLISTSEPARATORS && xsep1 ) {
        (*xsep1) = ( lob.visible.x + lob.visible.width ) + 0x04;
    }

    nextx = (*xsep1) + 0x04;

    /*** if the object has no texture tags, then its width stops there ***/
    if ( query_flags & GETLISTMAXWIDTH && width ) {
        (*width) = nextx;
    }

    /*** Otherwise, the widget width is the last texture tag boundary **/
    if ( obj->type == G3DMESHTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        LIST *ltmptex = mes->ltex;
        uint32_t nbtex = 0x00;

        while ( ltmptex && ( nbtex < MAXTEXTURES ) ) {
            G3DTEXTURE *tex  = ( G3DTEXTURE * ) ltmptex->data;
            G3DMATERIAL *mat = tex->mat;

            lob.texture[nbtex].x      = nextx;
            lob.texture[nbtex].y      = y;
            lob.texture[nbtex].width  = LISTINDENT;
            lob.texture[nbtex].height = LISTINDENT;

            nextx += LISTINDENT;

            if ( query_flags & GETLISTMAXWIDTH && width ) {
                (*width) = nextx;
            }

            nbtex++;

            ltmptex = ltmptex->next;
        }
    }

    XmStringFree ( str );


    return &lob;
}

/******************************************************************************/
/*** Coordinates within rectangle  ? ***/
static uint32_t inRectangle ( XRectangle *rec, Dimension xm, Dimension ym ) {
    Dimension x1 = rec->x,
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
/*** Apply picking policy ***/
static void pickedobject_parse ( PICKEDOBJECT *pob ) {
    G3DOBJECT *obj = pob->obj;

    /*** If we clicked [+] or [-], develop or collapse ***/
    if ( pob->picked == EXPANDRECTHIT ) {
        if ( obj->flags & OBJECTCOLLAPSED ) {
            obj->flags &= (~OBJECTCOLLAPSED);
        } else {
            obj->flags |=  (OBJECTCOLLAPSED);
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
}

/******************************************************************************/
static PICKEDOBJECT *pickobject ( Dimension x , Dimension y,
                                  Dimension xm, Dimension ym,
                                  Dimension xsep0,
                                  Dimension xsep1,
                                  XmFontList fontlist,
                                  G3DOBJECT *obj ) {
    LISTEDOBJECT *lob = sizelistedobject ( obj, x, y, &xsep0,
                                                      &xsep1,
                                                      NULL, 
                                                      fontlist, 0x00 );
    static PICKEDOBJECT pob;
    uint32_t nbtex = 0x00;
    uint32_t i;

    pob.picked = 0x00;

    /*** where did the mouse hit ? ***/
    if ( inRectangle ( &lob->expand , xm, ym ) ) pob.picked = EXPANDRECTHIT;
    if ( inRectangle ( &lob->icon   , xm, ym ) ) pob.picked = ICONRECTHIT;
    if ( inRectangle ( &lob->name   , xm, ym ) ) pob.picked = NAMERECTHIT;
    if ( inRectangle ( &lob->active , xm, ym ) ) pob.picked = ACTIVERECTHIT;
    if ( inRectangle ( &lob->visible, xm, ym ) ) pob.picked = VISIBLERECTHIT;

    if ( obj->type == G3DMESHTYPE ) {
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

    if ( pob.picked ) {
        pob.obj = obj;

        return &pob;
    }

    return NULL;
}

/******************************************************************************/
PICKEDOBJECT *selectObject_r ( Display *dis, Window win,
                                             XmFontList fontlist,
                                             Dimension *x,      /* Object X pos  */
                                             Dimension *y,      /* Object Y pos  */ 
                                             Dimension  xm,     /* Mouse X coord */
                                             Dimension  ym,     /* Mouse Y coord */
                                             Dimension  xsep0,  /* Separator 1   */
                                             Dimension  xsep1,  /* Separator 2   */
                                             G3DOBJECT *obj ) {
    LIST *ltmp = obj->lchildren;

    if ( obj->name ) {
        Dimension x1 = (*x),
                  y1 = (*y);
        PICKEDOBJECT *pob = pickobject ( x1, y1,
                                         xm, ym, xsep0, xsep1, fontlist, obj );

        if ( pob ) {
            return pob;
        }
    }

    if ( ( obj->flags & OBJECTCOLLAPSED ) == 0x00 ) {
        Dimension x1, y1, x2, y2;

        while ( ltmp ) {
            G3DOBJECT *child = ( G3DOBJECT * ) ltmp->data;
            Dimension saveposx = (*x);
            PICKEDOBJECT *sel;

            x1 = (*x) + 0x04;
            y1 = (*y) + 0x0B;

            (*y) = ( (*y) + LISTINDENT );
            (*x) = ( (*x) + LISTINDENT );

            if ( ( sel = selectObject_r ( dis, win, fontlist,
                                          x, y, xm, ym, xsep0, xsep1,
                                          child ) ) ) {
                return sel;
            }

            (*x) = saveposx;


            ltmp = ltmp->next;
        }
    }

    return NULL;
}

/******************************************************************************/
/** Retrieve the width of the List widget, i.e the last texture tag position **/
void getListMaxWidth_r ( XmFontList fontlist, Dimension  x,
                                              Dimension  xsep0,
                                              Dimension  xsep1,
                                              Dimension *width,
                                              G3DOBJECT *obj ) {
    Dimension WIDTH = 0x00;
    LISTEDOBJECT *lob = sizelistedobject ( obj, x, 0x00, &xsep0,
                                                     &xsep1,
                                                     &WIDTH,
                                                     fontlist, GETLISTMAXWIDTH );

    if ( (*width) < WIDTH ) (*width) = WIDTH;

    if ( ( obj->flags & OBJECTCOLLAPSED ) == 0x00 ) {
        LIST *ltmpchildren = obj->lchildren;

        while ( ltmpchildren ) {
            G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;
            Dimension kidx = ( x + LISTINDENT );

            /*** we dont really care about the Y Axis indentation ***/
            getListMaxWidth_r ( fontlist, kidx, xsep0, xsep1, width, child );


            ltmpchildren = ltmpchildren->next;
        }
    }
}

/******************************************************************************/
void getSeparators_r ( XmFontList fontlist, Dimension  x,
                                            Dimension *xsep0,
                                            Dimension *xsep1,
                                            G3DOBJECT *obj ) {
    Dimension XSEP0 = 0x00,
              XSEP1 = 0x00;
    LISTEDOBJECT *lob = sizelistedobject ( obj, x, 0x00, &XSEP0, 
                                                     &XSEP1,
                                                     NULL,  /* Widget width not needed */
                                                     fontlist, GETLISTSEPARATORS );

    if ( (*xsep0) < XSEP0 ) (*xsep0) = XSEP0;
    if ( (*xsep1) < XSEP1 ) (*xsep1) = XSEP1;

    if ( ( obj->flags & OBJECTCOLLAPSED ) == 0x00 ) {
        LIST *ltmpchildren = obj->lchildren;

        while ( ltmpchildren ) {
            G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;
            Dimension kidx = ( x + LISTINDENT );

            /*** we dont really care about the Y Axis indentation ***/
            getSeparators_r ( fontlist, kidx, xsep0, xsep1, child );


            ltmpchildren = ltmpchildren->next;
        }
    }
}

/******************************************************************************/
uint32_t printObject_r ( Display *dis, Window win, XmFontList fontlist, GC gc,
                         Dimension *x, Dimension *y,
                         Dimension *xsep0,
                         Dimension *xsep1,
                         G3DOBJECT *obj,
                         Pixmap bonico, Pixmap ligico, Pixmap mesico,
                         Pixmap defico, Pixmap symico, Pixmap cubico,
                         Pixmap sphico, Pixmap torico, Pixmap conico,
                         Pixmap plnico, Pixmap ffdico, Pixmap cylico,
                         Pixmap camico, Pixmap scnico, Pixmap mapico,
                         XMMATPREVIEW *mpv ) {
    LIST *ltmp = obj->lchildren;
    uint32_t height = LISTINDENT;
    unsigned long whitepixel = XWhitePixel ( dis, 0x00 ),
                  blackpixel = XBlackPixel ( dis, 0x00 );
    Pixel objico = defico;

    if ( obj->name ) {
        XmString str = XmStringCreate ( obj->name, XmFONTLIST_DEFAULT_TAG );
        Dimension x1 = (*x),
                  y1 = (*y);
        LISTEDOBJECT *lob = sizelistedobject ( obj, x1, y1, xsep0, xsep1, NULL, fontlist, 0x00 );

        if ( obj->parent ) {
            XSetForeground ( dis, gc, blackpixel );
            XDrawLine ( dis, win, gc, (*x) - 0x0B, (*y) + 0x06,
                                      (*x)       , (*y) + 0x06 );
        }

        if ( obj->flags & OBJECTINACTIVE ) {
            XSetForeground ( dis, gc, 0xFF0000 );
            drawInactive ( dis, win, gc, lob->active.x, lob->active.y + 2 );
        } else {
            XSetForeground ( dis, gc, 0x008000 );
            drawActive ( dis, win, gc, lob->active.x, lob->active.y + 2 );
        }

        if ( obj->flags & OBJECTINVISIBLE ) {
            XSetForeground ( dis, gc, blackpixel );
            drawClosedEye ( dis, win, gc, lob->visible.x, lob->visible.y + 2 );
        } else {
            XSetForeground ( dis, gc, blackpixel );
            drawOpenedEye ( dis, win, gc, lob->visible.x, lob->visible.y + 2 );
        }

        /*** Draw the expansion square [+] or [-] ***/
        if ( obj->lchildren ) {
            if ( obj->flags & OBJECTCOLLAPSED ) {
                drawPlus  ( dis, win, gc, lob->expand.x, lob->expand.y + 2 );
            } else {
                drawMinus ( dis, win, gc, lob->expand.x, lob->expand.y + 2 );
            }
        }

        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes =  ( G3DMESH * ) obj;
            LIST *ltmptex = mes->ltex;
            uint32_t nbtex = 0x00;

            while ( ltmptex ) {
                G3DTEXTURE *tex      = ( G3DTEXTURE * ) ltmptex->data;
                XMMATPIXEL   *vecmap = glistClassRec.glist.vecmap;

                fillPreview ( mpv->img, tex->mat, vecmap );

                XPutImage ( dis, win, gc, mpv->img, 0x00, 0x00,
                                                    lob->texture[nbtex].x + 0x01,
                                                    lob->texture[nbtex].y + 0x01, 
                                                    mpv->img->width,
                                                    mpv->img->height );

                if ( tex->flags & TEXTURESELECTED ) {
                    XSetForeground ( dis, gc, 0xFF0000 );
                    XDrawRectangle ( dis, win, gc, lob->texture[nbtex].x,
                                                   lob->texture[nbtex].y, 
                                                   lob->texture[nbtex].width,
                                                   lob->texture[nbtex].height );
                }

                nbtex++;

                ltmptex = ltmptex->next;
            }
        }

        /*** Draw the object type icon ***/
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

        XCopyArea ( dis, objico, win, gc, 0x00, 0x00, lob->icon.width,
                                                      lob->icon.height,
                                                      lob->icon.x,
                                                      lob->icon.y );

        /*** Foreground color is white if object selected, black otherwise ***/
        if ( obj->flags & OBJECTSELECTED ) {
            XSetForeground ( dis, gc, whitepixel );
        } else {
            XSetForeground ( dis, gc, blackpixel );
        }

        /*** Draw the object name ***/
        XmStringDraw ( dis, win, fontlist, str, gc, lob->name.x,
                                                    lob->name.y,
                                                    lob->name.width,
                       XmALIGNMENT_BEGINNING, 0x00, NULL );

        XmStringFree ( str );
    }

    if ( ( obj->flags & OBJECTCOLLAPSED ) == 0x00 ) {
        Dimension x1, y1, x2, y2;

        while ( ltmp ) {
            G3DOBJECT *child = ( G3DOBJECT * ) ltmp->data;
            Dimension saveposx = (*x);
            uint32_t childheight = 0x00;

            x1 = (*x) + 0x04;
            y1 = (*y) + 0x0B;

            (*y) = ( (*y) + LISTINDENT );
            (*x) = ( (*x) + LISTINDENT );

            childheight = printObject_r ( dis, win, fontlist, gc, x, y, 
                                          xsep0, xsep1,
                                          child, bonico, ligico, mesico, 
                                                 defico, symico, cubico,
                                                 sphico, torico, conico,
                                                 plnico, ffdico, cylico,
                                                 camico, scnico, mapico, mpv );

            x2 = (x1);

            if ( ltmp->next == NULL ) {
                y2 = (y1 + 0x0B);

                /*** draw vertical line for children indentation ***/
                XSetForeground ( dis, gc, blackpixel );
                XDrawLine      ( dis, win, gc, x1, y1, x2, y2 );
            } else {
                y2 = (y1 + childheight);

                /*** draw vertical line for children indentation ***/
                XSetForeground ( dis, gc, blackpixel );
                XDrawLine      ( dis, win, gc, x1, y1, x2, y2 );
            }

            (*x) = saveposx;

            height += childheight;

            ltmp = ltmp->next;
        }
    }

    return height;
}

/******************************************************************************/
static void GListMouseEvent ( Widget w, XEvent *event,
                              String *params, Cardinal *num_params ) {
    Pixel background, foreground;
    static G3DOBJECT *obj, *dst;
    static PICKEDOBJECT *pob;
    G3DUICLIPBOARD *cli;
    G3DURMANAGER *urm;
    static Time click;
    G3DSCENE *sce;
    G3DUI *gui;

    /*** retrieve the global GUI structure ***/
    XtVaGetValues ( w, XmNuserData, &gui,
                       XmNbackground, &background,
                       XmNforeground, &foreground, NULL );
    urm = gui->urm;
    sce = gui->sce;
    cli = gui->cli;

    switch ( event->type ) {
        case KeyPress : {
            XKeyEvent *kev = ( XKeyEvent * ) event;
            KeySym keysym;
            char c;

            if ( XLookupString ( kev, &c, 0x01, &keysym, NULL ) > 0x00 ) {
                switch ( keysym ) {
                    case XK_Delete: {
                        uint32_t retflags = ( REDRAWVIEW | REDRAWLIST );

                        if ( pob && ( pob->picked == TEXTURERECTHIT ) ) {
                            g3dmesh_removeMaterial ( ( G3DMESH * ) pob->obj, pob->tex->mat );
                        } else {
                            g3durm_scene_deleteSelectedObjects ( urm, sce, gui->flags, retflags );
                        }

                        pob = NULL;
                    } break;
 
                    /*** This piece of code is also in g3duiview ****/
                    /*** Ctrl+c is copy object ***/
                    case XK_c :
                        if ( kev->state & ControlMask ) {
printf("copying %d object(s)\n", list_count ( sce->lsel ) );
                           g3dui_setHourGlass ( gui );
                           g3duiclipboard_copyObject ( cli, sce, sce->lsel, 0x01, gui->flags );
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
                           g3duiclipboard_paste ( cli, urm, sce, dst, gui->flags );
                           g3dui_unsetHourGlass ( gui );
                        }
                    break;
                    /*************************************************/

                    default :
                    break;
                }
            }

            redrawObjectList     ( gui );
            updateAllCurrentEdit ( gui );
            redrawGLViews        ( gui );
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

                getSeparators_r ( gui->fontlist, 0x00, &xsep0, 
                                                       &xsep1, ( G3DOBJECT * ) gui->sce );

                pob = selectObject_r ( XtDisplay ( w ), XtWindow ( w ),
                                       gui->fontlist,
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
                            g3dscene_unselectAllObjects ( gui->sce, gui->flags );
                        }

                        g3dscene_selectObject ( gui->sce, obj, gui->flags );

                        lselnew = list_copy ( sce->lsel );

                        /*** remember selection ***/
                        g3durm_scene_pickObject  ( urm, sce,
                                                        lselold,
                                                        lselnew,
                                                        gui->flags,
                                                        REDRAWVIEW );
                    }
                }
            }

            if ( obj ) {
                updateCoords ( gui );
                redrawTimeline ( gui );
            }

            redrawObjectList ( gui );
            redrawGLViews ( gui );
            updateAllCurrentEdit ( gui );
        } break;

        /*** Drag ***/
        case MotionNotify : {
            XMotionEvent *mev = ( XMotionEvent * ) event;
            Dimension xpos = 0x00, ypos = 0x00;
            Dimension xsep0 = 0x00, xsep1 = 0x00, width;

            if ( gui && gui->sce && obj ) {
                getSeparators_r ( gui->fontlist, 0x00, &xsep0,
                                                       &xsep1, ( G3DOBJECT * ) gui->sce );

                /*** drag and drop ***/
                pob = selectObject_r ( XtDisplay ( w ), XtWindow ( w ),
                                       gui->fontlist,
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
                    g3durm_object_addChild ( urm, sce, gui->flags,
                                                       ( REDRAWVIEW |
                                                         REDRAWLIST | 
                                                         REDRAWCURRENTOBJECT ),
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
                                              COMPUTEFACEPOSITION |
                                              COMPUTEEDGEPOSITION |
                                              COMPUTESUBDIVISION, gui->flags );
                    }

                    updateCoords ( gui );
                }
            }

            redrawObjectList ( gui );
            redrawGLViews ( gui );

            obj = dst = NULL;
        } break;

        default :
        break;
    }
}

/******************************************************************************/
static char defaultTranslations[] = 
"<PtrMoved>: GListMouseEvent()\n\
<BtnDown>:   GListMouseEvent()\n\
<KeyPress>:  GListMouseEvent()\n\
<BtnUp>:     GListMouseEvent()\n";

/******************************************************************************/
static XtActionsRec Actions[] = { { "GListMouseEvent", GListMouseEvent } };

/******************************************************************************/
static XtProc ClassInitialize ( ) {
    glistClassRec.glist.vecmap = buildMap ( LISTINDENT-1, LISTINDENT-1 );

    return ( XtProc ) NULL;
}

/******************************************************************************/
static XtInitProc Initialize ( Widget request, Widget init,
                               ArgList args, Cardinal *num ) {

    return ( XtInitProc ) NULL;
}

/******************************************************************************/
static XtRealizeProc Realize ( Widget w, XtValueMask *value_mask,
                               XSetWindowAttributes *attributes ) {
    Pixel foreground;
    Pixel background;
    GListWidget gqw = ( GListWidget ) w;
    Widget parent = XtParent ( w );
    Display *dis = XtDisplay ( w );
    /*** XCreateImage crashes if drawable is "win" variable, why ? ***/
    Visual *vis = DefaultVisual ( dis, 0x00 );
    G3DUI *gui;
    int depth;

    /*** this is required if we want to inherit the event mask. ***/
    /*** Redisplay procedure would not be called otherwise      ***/
    xmPrimitiveClassRec.core_class.realize ( w, value_mask, attributes );

    XtVaGetValues ( w, XmNdepth, &depth,
                       XmNuserData, &gui,
                       XmNbackground, &background,
                       XmNforeground, &foreground, NULL );

    gqw->glist.mpv      = xmmatpreview_new  ( "Temporary Preview", NULL );
    gqw->glist.mpv->img = allocPreviewImage ( depth, LISTINDENT-1, 
                                                     LISTINDENT-1, 0x20, dis,
                                                                         vis );

    createTransparentPixmap ( dis, &gqw->glist.bonico, NULL, bonico_xpm,
                              depth, background, foreground );

    createTransparentPixmap ( dis, &gqw->glist.ligico, NULL, ligico_xpm,
                              depth, background, foreground );

    createTransparentPixmap ( dis, &gqw->glist.mesico, NULL, mesico_xpm,
                              depth, background, foreground );

    createTransparentPixmap ( dis, &gqw->glist.defico, NULL, defico_xpm,
                              depth, background, foreground );

    createTransparentPixmap ( dis, &gqw->glist.symico, NULL, symico_xpm,
                              depth, background, foreground );

    createTransparentPixmap ( dis, &gqw->glist.cubico, NULL, cubico_xpm,
                              depth, background, foreground );

    createTransparentPixmap ( dis, &gqw->glist.sphico, NULL, sphico_xpm,
                              depth, background, foreground );

    createTransparentPixmap ( dis, &gqw->glist.torico, NULL, torico_xpm,
                              depth, background, foreground );

    /*createTransparentPixmap ( dis, &gqw->glist.conico, NULL, symico_xpm,
                              depth, background, foreground );*/

    createTransparentPixmap ( dis, &gqw->glist.plnico, NULL, plnico_xpm,
                              depth, background, foreground );

    createTransparentPixmap ( dis, &gqw->glist.ffdico, NULL, ffdico_xpm,
                              depth, background, foreground );

    createTransparentPixmap ( dis, &gqw->glist.cylico, NULL, cylico_xpm,
                              depth, background, foreground );

    createTransparentPixmap ( dis, &gqw->glist.camico, NULL, camico_xpm,
                              depth, background, foreground );

    createTransparentPixmap ( dis, &gqw->glist.scnico, NULL, scnico_xpm,
                              depth, background, foreground );

    createTransparentPixmap ( dis, &gqw->glist.mapico, NULL, mapico_xpm,
                              depth, background, foreground );

    return ( XtRealizeProc ) NULL;
}

/******************************************************************************/
static XtWidgetProc Destroy ( Widget w ) {

    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
static XtWidgetProc Resize ( Widget w ) {
    /*GListWidget gqw = ( GListWidget ) w;*/

    /*** XmManager doesnot provide a resize function, so we have to do some ***/
    /*** routines by ourselves.                                             ***/
    if ( XtIsRealized ( w ) ) {
        XClearArea ( XtDisplay ( w ), XtWindow ( w ), 0x00, 0x00,
                                                      0x00, 0x00, True );
    }

    /*** And use the parent class of xmPrimitive for the resizing routines ***/
    if ( xmPrimitiveClassRec.core_class.resize ) {
        xmPrimitiveClassRec.core_class.resize ( w );
    }

    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
static XtExposeProc Redisplay ( Widget w, XEvent *event, Region region ) {
    GListWidget gqw = ( GListWidget ) w;
    Display *dis = XtDisplay ( w );
    Window win = XtWindow ( w );
    XGCValues values;
    Dimension winwidth = 0x0, winheight = 0x00, ret_width, ret_height;
    Pixel toppix, botpix;
    G3DUI *gui;
    GC gc;

    /*** retrieve the global GUI structure ***/
    XtVaGetValues ( w, XmNuserData         , &gui,
                       XmNwidth            , &winwidth,
                       XmNheight           , &winheight,
                       XmNtopShadowColor   , &toppix, /* For the vertical bar */
                       XmNbottomShadowColor, &botpix, /* For the vertical bar */
                       NULL );

    gc = XtGetGC ( w, 0x00, &values );

    if ( gui && gui->sce ) {
        G3DOBJECT * obj = ( G3DOBJECT * ) gui->sce;
        Dimension xpos = 0x00, ypos = 0x00;
        Dimension xsep0 = 0x00, xsep1 = 0x00, width;
        Dimension height;

        getSeparators_r   ( gui->fontlist, 0x00, &xsep0, &xsep1, obj );

        /*** In case there is no texture tag, the ***/
        /*** widget width is the second seperator ***/
        winwidth  = xsep1 + 0x04;

        getListMaxWidth_r ( gui->fontlist, 0x00,  xsep0,  xsep1, &winwidth, obj );

        /*** Otherwise, the widget width is the last texture tag boundary+4 ***/
        winwidth += 0x04;

        height = printObject_r ( dis, win, gui->fontlist, gc, 
                                    &xpos, &ypos, &xsep0, &xsep1, obj,
                                    gqw->glist.bonico, gqw->glist.ligico,
                                    gqw->glist.mesico, gqw->glist.defico,
                                    gqw->glist.symico, gqw->glist.cubico,
                                    gqw->glist.sphico, gqw->glist.torico,
                                    gqw->glist.conico, gqw->glist.plnico,
                                    gqw->glist.ffdico, gqw->glist.cylico,
                                    gqw->glist.camico, gqw->glist.scnico,
                                    gqw->glist.mapico, gqw->glist.mpv );

        if ( height > winheight ) winheight = height;

        XSetForeground ( dis, gc, botpix );
        XDrawLine      ( dis, win, gc, xsep0    , 0x00, xsep0    , winheight );
        XSetForeground ( dis, gc, toppix );
        XDrawLine      ( dis, win, gc, xsep0 + 1, 0x00, xsep0 + 1, winheight );

        XSetForeground ( dis, gc, botpix );
        XDrawLine      ( dis, win, gc, xsep1    , 0x00, xsep1    , winheight );
        XSetForeground ( dis, gc, toppix );
        XDrawLine      ( dis, win, gc, xsep1 + 1, 0x00, xsep1 + 1, winheight );
    }

    XtMakeResizeRequest ( w, winwidth, winheight, &ret_width, &ret_height );

    XtResizeWidget ( w, ret_width, ret_height, 0x00 );

    XtReleaseGC ( w, gc );

    return ( XtExposeProc ) NULL;
}

/******************************************************************************/
static XtSetValuesFunc set_values ( Widget current, Widget request, Widget set,
                                    ArgList args, Cardinal *numargs ) {

    return 0x00;
}

/******************************************************************************/
GListClassRec glistClassRec = { { (WidgetClass) &xmPrimitiveClassRec,/*parent*/
                                  "GQUAD",                      /* class_name */
                                  sizeof ( GListRec ),         /* widget size */
                       ( XtProc ) ClassInitialize,        /* class_initialize */
                                  NULL,              /* class_part_initialize */
                                  FALSE,                      /* class_inited */
                   ( XtInitProc ) Initialize,                   /* initialize */
                                  NULL,                    /* initialize_hook */
                ( XtRealizeProc ) Realize,                         /* realize */
                                  Actions/*NULL*/,                 /* actions */
                                  XtNumber ( Actions )/*0x00*/,/* num_actions */
                                  NULL/*widgetRes*/,              /* resouces */
                                  0x00/*XtNumber ( widgetRes )*/,/* num_resources */
                                  NULLQUARK,                      /*xrm_class */
                                  TRUE,                    /* compress_motion */
                                  XtExposeCompressMaximal,/*compress_exposure */
                                  TRUE,               /* compress_enterleave */
                                  FALSE,                  /* visible_interest */
                ( XtWidgetProc )  Destroy,                          /*destroy */
                ( XtWidgetProc )  Resize,                           /* resize */
                ( XtExposeProc )  Redisplay,                        /* expose */
             ( XtSetValuesFunc )  set_values,                   /* set_values */
                                  NULL,                    /* set_values_hook */
                                  XtInheritSetValuesAlmost,/*set_values_almost*/
                                  NULL,                    /* get_values_hook */
                                  NULL,                       /* accept_focus */
                                  XtVersion,                       /* version */
                                  NULL,                   /* callback_offsets */
                                  defaultTranslations,            /* tm_table */
                                  XtInheritQueryGeometry,   /* query_geometry */
                                  NULL,                /* display_accelerator */
                                  NULL },                        /* extension */
                                  /* Primitive class part */
                                { XmInheritBorderHighlight,
                                  XmInheritBorderUnhighlight,
                                  XtInheritTranslations,
                                  NULL,
                                  NULL,
                                  0x00,
                                  NULL },
                                /*** Timeline Class part ***/
                                { 0x00 } };

/******************************************************************************/
WidgetClass glistWidgetClass = ( WidgetClass ) &glistClassRec;

/******************************************************************************/
Widget createObjectList ( Widget parent, G3DUI *gui, char *name,
                          Dimension x    , Dimension y,
                          Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget listfrm;
    Widget scr;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    scr = XmVaCreateManagedScrolledWindow ( parent, name,
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
                                            XmNuserData, gui,
                                            NULL );

    listfrm = XtVaCreateWidget ( name, glistWidgetClass, scr,
                                 XtNx, x,
                                 XtNy, y,
                                 XtNwidth , width,
                                 XtNheight, height,
                                 XmNforeground, foreground,
                                 XmNbackground, background,
/*** needed for keyboard input ***/  XmNtraversalOn,   TRUE,
XmNancestorSensitive, TRUE,
XmNsensitive, TRUE,
XmNkeyboardFocusPolicy, XmEXPLICIT,
                                 XmNuserData, gui,
                                 NULL );

    XtManageChild ( listfrm );

    /*** add this widget to the list of ObjectList widget for easy update ***/
    list_insert ( &gui->lobjlist, listfrm );


    return listfrm;
}
