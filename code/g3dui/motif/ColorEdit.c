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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
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
static XtProc ClassInitialize ( void );
static XtInitProc Initialize  ( Widget, Widget, ArgList, Cardinal * );
static XtRealizeProc Realize  ( Widget, XtValueMask *, XSetWindowAttributes * );
static XtWidgetProc Destroy   ( Widget );
static XtWidgetProc Resize    ( Widget );
static XtExposeProc Redisplay ( Widget, XEvent *, Region );

static void drawCursor ( XImage *, XImage *, int, int );
static void triangle_init ( TRIANGLE *, unsigned char *,
                                        unsigned char *,
                                        unsigned char *,
                                        int *,
                                        int * );
static void updateText ( Widget );
static void sendColorEvent ( Widget, int, XEvent * );
static void GColorEditMouseEvent ( Widget, XEvent *, String *, Cardinal * );
void drawhline ( HLINE *hline, int y, XImage *img, XImage *msk );
void drawline ( int, int, int, int, int, int, int, int, int, int, HLINE * );
static uint32_t triangle_rgbIn ( TRIANGLE *, unsigned char,
                                             unsigned char,
                                             unsigned char,
                                             float *,
                                             float *,
                                             float * );
static uint32_t triangle_pointIn ( TRIANGLE *, int,
                                               int,
                                               float *,
                                               float *,
                                               float * );
static void hexagon_gray ( HEXAGON *, unsigned char );
static void hexagon_init ( HEXAGON *, unsigned char );
static uint32_t hexagon_rgb2xy ( HEXAGON *, unsigned char,
                                            unsigned char,
                                            unsigned char,
                                            int *,
                                            int *, unsigned char * );
static uint32_t hexagon_xy2rgb ( HEXAGON *, int,
                                            int,
                                            unsigned char *,
                                            unsigned char *,
                                            unsigned char * );
static void triangle_init ( TRIANGLE *, unsigned char *,
                                        unsigned char *,
                                        unsigned char *,
                                        int *,
                                        int * );
static void triangle_draw ( TRIANGLE *, XImage *, XImage * );
static void hexagon_draw  ( HEXAGON *, XImage *, XImage *, int, int );
static void fillBackground ( XImage *, Pixel );
static void redcbk ( Widget, XtPointer, XtPointer );
static void greencbk ( Widget, XtPointer, XtPointer );
static void bluecbk ( Widget, XtPointer, XtPointer );
static XImage *allocRGBImage ( Display *, Pixel );
static XImage *allocMaskImage ( Display * );
static void dragcbk ( Widget, XtPointer, XtPointer );
static void drawCursor ( XImage *, XImage *, int, int );
static void intvector_cross ( INTVECTOR *, INTVECTOR *, INTVECTOR * );
static void triangle_brightness ( TRIANGLE *, unsigned char,
                                              unsigned char,
                                              unsigned char );

/******************************************************************************/
static Widget getScale ( Widget w ) {
    WidgetList children;
    Cardinal nc;
    int i;

    XtVaGetValues ( w, XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];

        if ( XtClass ( child ) == xmScaleWidgetClass ) {
            return child;
        }
    }

    return NULL;
}

/******************************************************************************/
static void updateText ( Widget w ) {
    XmColorEditWidget xcw = ( XmColorEditWidget ) w;
    WidgetList children;
    Cardinal nc;
    /*G3DUI *gui;*/
    int i;

    XtVaGetValues ( w, /*XmNuserData, &gui,*/
                       XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    xcw->coloredit.lock = 0x01;

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];
        char *name = XtName ( child );
        WidgetClass class = XtClass ( child );
        unsigned long pixel = xcw->coloredit.pixel;
        char buf[0x10];

        if ( class == xmTextWidgetClass &&
             strcmp ( name, COLOREDITREDNAME ) == 0x00 ) {
            snprintf ( buf, 0x10, "%d", ( pixel & 0x00FF0000 ) >> 0x10 );

            XmTextSetString ( child, buf );
        }

        if ( class == xmTextWidgetClass &&
             strcmp ( name, COLOREDITGREENNAME ) == 0x00 ) {
            snprintf ( buf, 0x10, "%d", ( pixel & 0x0000FF00 ) >> 0x08 );

            XmTextSetString ( child, buf );
        }

        if ( class == xmTextWidgetClass &&
             strcmp ( name, COLOREDITBLUENAME ) == 0x00 ) {
            snprintf ( buf, 0x10, "%d", ( pixel & 0x000000FF ) );

            XmTextSetString ( child, buf );
        }
    }

    xcw->coloredit.lock = 0x00;
}

/******************************************************************************/
static void sendColorEvent ( Widget w, int reason, XEvent *event ) {
    XmColorEditWidget xcw = ( XmColorEditWidget ) w;
    int i = 0x00;

    switch ( reason ) {
        case XmCOLORCHANGING : {
            XmColorEditCallbackStruct cs = { XmCOLORCHANGING,
                                             event,
                                             xcw->coloredit.pixel };

            XtCallCallbackList ( w, xcw->coloredit.colorChangingCallback, &cs );
        } break;

        case XmCOLORCHANGED : {
            XmColorEditCallbackStruct cs = { XmCOLORCHANGED,
                                             event,
                                             xcw->coloredit.pixel };

            XtCallCallbackList ( w, xcw->coloredit.colorChangedCallback, &cs );
        } break;
    }
}

/******************************************************************************/
static void GColorEditMouseEvent ( Widget w, XEvent *event,
                                   String *params, Cardinal *num_params ) {
    XmColorEditWidget xcw = ( XmColorEditWidget ) w;
    unsigned char R = 0x00, G = 0x00, B = 0x00;
    XImage *img = xcw->coloredit.heximg;
    XImage *msk = xcw->coloredit.mskimg;

    switch ( event->type ) {
        case ButtonPress :
        case ButtonRelease : {
            XButtonEvent *bev = ( XButtonEvent * ) event;
            int mousex = xcw->coloredit.csrx, 
                mousey = xcw->coloredit.csry;

            if ( ( bev->x >= 0x00 ) &&
                 ( bev->x <  img->width  ) ) mousex = bev->x;

            if ( ( bev->y >= 0x00 ) &&
                 ( bev->y <  img->height ) ) mousey = bev->y;

            if ( msk->f.get_pixel ( msk, mousex, mousey ) ) {
                xcw->coloredit.csrx = mousex;
                xcw->coloredit.csry = mousey;
            }

            hexagon_draw ( &xcw->coloredit.hex, xcw->coloredit.heximg,
                                                xcw->coloredit.mskimg,
                                                xcw->coloredit.csrx,
                                                xcw->coloredit.csry );

            if ( hexagon_xy2rgb ( &xcw->coloredit.hex, xcw->coloredit.csrx,
                                                       xcw->coloredit.csry,
                                                       &R, &G, &B ) ) {
                xcw->coloredit.pixel = ( R << 0x10 ) | ( G << 0x08 ) | B;
            }

            updateText ( w );

            /*** notify attached callbacks ***/
            sendColorEvent ( w, XmCOLORCHANGING, event );

            XClearArea ( XtDisplay ( xcw ), XtWindow ( xcw ), 0x00, 0x00,
            /*** Some trick to generate exposure event without redrawing    ***/
            /*** the background, preventing flickering. Needs a better way. ***/
                                                          0x01, 0x01, True );
        } break;

        case MotionNotify : {
            XMotionEvent *mev = ( XMotionEvent * ) event;
            int mousex = xcw->coloredit.csrx, 
                mousey = xcw->coloredit.csry;

            if ( mev->state & Button1Mask ) {
                if ( ( mev->x >= 0x00 ) &&
                     ( mev->x <  img->width  ) ) mousex = mev->x;

                if ( ( mev->y >= 0x00 ) &&
                     ( mev->y <  img->height ) ) mousey = mev->y;

                if ( msk->f.get_pixel ( msk, mousex, mousey ) ) {
                    xcw->coloredit.csrx = mousex;
                    xcw->coloredit.csry = mousey;
                }

                hexagon_draw ( &xcw->coloredit.hex, xcw->coloredit.heximg,
                                                    xcw->coloredit.mskimg,
                                                    xcw->coloredit.csrx,
                                                    xcw->coloredit.csry );

                if ( hexagon_xy2rgb ( &xcw->coloredit.hex, xcw->coloredit.csrx,
                                                           xcw->coloredit.csry,
                                                           &R, &G, &B ) ) {
                    xcw->coloredit.pixel = ( R << 0x10 ) | ( G << 0x08 ) | B;
                }

                updateText ( w );

                /*** notify attached callbacks ***/
                sendColorEvent ( w, XmCOLORCHANGING, event );

                XClearArea ( XtDisplay ( xcw ), XtWindow ( xcw ), 0x00, 0x00,
                /* Some trick to generate exposure event without redrawing    */
                /* the background, preventing flickering. Needs a better way. */
                                                          0x01, 0x01, True );
            }
        } break;

        default :
        break;
    }
}

/******************************************************************************/
static char defaultTranslations[] = 
"<PtrMoved>: GColorEditMouseEvent()\n\
<BtnDown>:   GColorEditMouseEvent()\n\
<BtnUp>:     GColorEditMouseEvent()\n";

/******************************************************************************/
static XtActionsRec Actions[] = { { "GColorEditMouseEvent",
                                     GColorEditMouseEvent } };

/******************************************************************************/
static XtResource widgetRes[] = {
    { XtNcolor,
      XtNcolor, XtRInt, sizeof ( unsigned long ),
      XtOffsetOf ( XmColorEditRec, coloredit.pixel ),
      XtRImmediate, ( XtPointer ) 0x00 },
    { XtNcolorChangingCallback,
      XtNcolorChangingCallback, XtRCallback, sizeof ( XtPointer ),
      XtOffsetOf ( XmColorEditRec, coloredit.colorChangingCallback ),
      XtRImmediate, ( XtPointer ) NULL },
    { XtNcolorChangedCallback,
      XtNcolorChangedCallback, XtRCallback, sizeof ( XtPointer ),
      XtOffsetOf ( XmColorEditRec, coloredit.colorChangedCallback ),
      XtRImmediate, ( XtPointer ) NULL } };

/******************************************************************************/
void drawhline ( HLINE *hline, int y, XImage *img, XImage *msk ) {
    int dx = ( hline->x2 - hline->x1 ), ddx = ( dx ) ? abs ( dx ) : 0x01;
    int dr = ( hline->r2 - hline->r1 );
    int dg = ( hline->g2 - hline->g1 );
    int db = ( hline->b2 - hline->b1 );
    int px = ( dx > 0x00 ) ? 1 : -1;
    int pr = ( ddx ) ? ( dr / ddx ) : 0x00;
    int pg = ( ddx ) ? ( dg / ddx ) : 0x00;
    int pb = ( ddx ) ? ( db / ddx ) : 0x00;
    int x = hline->x1;
    int r = hline->r1;
    int g = hline->g1;
    int b = hline->b1;
    int i;

    if ( ddx == 0x00 ) return; /*** dont draw small hlines **/

    switch ( img->depth ) {
        case 0x10 : /*** 16 Bpp mode ***/
            for ( i = 0x00; i <= ddx; i++ ) {
                int R = ( r >> 0x08 ) >> 0x03;
                int G = ( g >> 0x08 ) >> 0x02;
                int B = ( b >> 0x08 ) >> 0x03;
                unsigned long pixel = ( ( R << 0x0B ) | ( G << 0x05 ) | B );

                img->f.put_pixel ( img, x, y, pixel );
                msk->f.put_pixel ( msk, x, y, 0x01  );

                x += px;

                r += pr;
                g += pg;
                b += pb;
            }
        break;

        case 0x18 : /*** 24 Bpp or ... ***/
        case 0x20 : /*** 32 Bpp mode.  ***/
            for ( i = 0x00; i <= ddx; i++ ) {
                int R = ( r >> 0x08 );
                int G = ( g >> 0x08 );
                int B = ( b >> 0x08 );
                unsigned long pixel = ( ( R << 0x10 ) | ( G << 0x08 ) | B );

                img->f.put_pixel ( img, x, y, pixel );
                msk->f.put_pixel ( msk, x, y, 0x01  );

                x += px;

                r += pr;
                g += pg;
                b += pb;
            }
        break;

        default :
        break;
    }
}

/******************************************************************************/
void drawline ( int x1, int y1, int r1, int g1, int b1,
                int x2, int y2, int r2, int g2, int b2, HLINE *hline ) {
    long dx = x2 - x1, ddx = abs ( dx ), px = ( dx > 0x00 ) ? 1 : -1,
         dy = y2 - y1, ddy = abs ( dy ), py = ( dy > 0x00 ) ? 1 : -1,
         i, cumul = 0x00;

    r1 = ( r1 << 0x08 ); r2 = ( r2 << 0x08 );
    g1 = ( g1 << 0x08 ); g2 = ( g2 << 0x08 );
    b1 = ( b1 << 0x08 ); b2 = ( b2 << 0x08 );

    if ( ddx > ddy ) {
        long dr = r2 - r1, pr = ( ddx ) ? dr / ddx : 0x00,
             dg = g2 - g1, pg = ( ddx ) ? dg / ddx : 0x00,
             db = b2 - b1, pb = ( ddx ) ? db / ddx : 0x00;

        for ( i = 0x00; i < ddx; i++ ) {
            if ( cumul >= ddx ) {
                cumul -= ddx;
                y1    += py;
            }

            if ( hline[y1].set == 0x00 ) {
                hline[y1].x1  = hline[y1].x2 = x1;
                hline[y1].r1  = hline[y1].r2 = r1;
                hline[y1].g1  = hline[y1].g2 = g1;
                hline[y1].b1  = hline[y1].b2 = b1;

                hline[y1].set = 0x01;
            } else {
                if ( x1 < hline[y1].x1 ) {
                    hline[y1].x1  = x1;
                    hline[y1].r1  = r1;
                    hline[y1].g1  = g1;
                    hline[y1].b1  = b1;
                }
                if ( x1 > hline[y1].x2 ) {
                    hline[y1].x2  = x1;
                    hline[y1].r2  = r1;
                    hline[y1].g2  = g1;
                    hline[y1].b2  = b1;
                }

                hline[y1].set = 0x02;
            }

            cumul += ddy;
            x1    += px;

            r1 += pr;
            g1 += pg;
            b1 += pb;
        }
    } else {
        long dr = r2 - r1, pr = ( ddy ) ? dr / ddy : 0x00,
             dg = g2 - g1, pg = ( ddy ) ? dg / ddy : 0x00,
             db = b2 - b1, pb = ( ddy ) ? db / ddy : 0x00;

        for ( i = 0x00; i < ddy; i++ ) {
            if ( cumul >= ddy ) {
                cumul -= ddy;
                x1    += px;
            }

            if ( hline[y1].set == 0x00 ) {
                hline[y1].x1  = hline[y1].x2 = x1;
                hline[y1].r1  = hline[y1].r2 = r1;
                hline[y1].g1  = hline[y1].g2 = g1;
                hline[y1].b1  = hline[y1].b2 = b1;

                hline[y1].set = 0x01;
            } else {
                if ( x1 < hline[y1].x1 ) {
                    hline[y1].x1  = x1;
                    hline[y1].r1  = r1;
                    hline[y1].g1  = g1;
                    hline[y1].b1  = b1;
                }
                if ( x1 > hline[y1].x2 ) {
                    hline[y1].x2  = x1;
                    hline[y1].r2  = r1;
                    hline[y1].g2  = g1;
                    hline[y1].b2  = b1;
                }

                hline[y1].set = 0x02;
            }

            cumul += ddx;
            y1    += py;

            r1 += pr;
            g1 += pg;
            b1 += pb;
        }
    }
}

/******************************************************************************/
static void triangle_intersect ( G3DVECTOR *vnor ) {

}

/******************************************************************************/
static void intvector_cross ( INTVECTOR *vone,
                              INTVECTOR *vtwo,
                              INTVECTOR *vout ) {
    vout->x = ( vone->y * vtwo->z ) - ( vone->z * vtwo->y );
    vout->y = ( vone->z * vtwo->x ) - ( vone->x * vtwo->z );
    vout->z = ( vone->x * vtwo->y ) - ( vone->y * vtwo->x );
}

/******************************************************************************/
static void triangle_brightness ( TRIANGLE *tri, unsigned char r,
                                                 unsigned char g,
                                                 unsigned char b ) {
    INTVECTOR v1vp = { .x = ( int32_t ) r - tri->r[0x01],
                       .y = ( int32_t ) g - tri->g[0x01],
                       .z = ( int32_t ) b - tri->b[0x01] },
              v2vp = { .x = ( int32_t ) r - tri->r[0x02],
                       .y = ( int32_t ) g - tri->g[0x02],
                       .z = ( int32_t ) b - tri->b[0x02] };
    INTVECTOR vnor;
    int32_t den, nom;
    float u;

    intvector_cross ( &v1vp, &v2vp, &vnor );

    nom = - ( ( int32_t ) ( r * vnor.x ) + ( g * vnor.y ) + ( b * vnor.z ) );
    den = - ( ( int32_t ) (     vnor.x ) + (     vnor.y ) + (     vnor.z ) );

    if ( den ) {
        tri->gray = ( float ) ( nom / den );
    } else {
        tri->gray = 0.0f;
    }
}

/******************************************************************************/
static uint32_t triangle_rgbIn ( TRIANGLE *tri, unsigned char r,
                                                unsigned char g,
                                                unsigned char b,
                                                float *RAT0,
                                                float *RAT1,
                                                float *RAT2 ) {
    G3DVECTOR v0v1 = { ( float ) tri->r[0x01] - tri->gray,
                       ( float ) tri->g[0x01] - tri->gray,
                       ( float ) tri->b[0x01] - tri->gray, 1.0f },
              v1v2 = { ( float ) tri->r[0x02] - tri->r[0x01],
                       ( float ) tri->g[0x02] - tri->g[0x01],
                       ( float ) tri->b[0x02] - tri->b[0x01], 1.0f },
              v2v0 = { ( float ) tri->gray    - tri->r[0x02],
                       ( float ) tri->gray    - tri->g[0x02],
                       ( float ) tri->gray    - tri->b[0x02], 1.0f };
    G3DVECTOR v0vp = { ( float ) r - tri->gray,
                       ( float ) g - tri->gray,
                       ( float ) b - tri->gray, 1.0f },
              v1vp = { ( float ) r - tri->r[0x01],
                       ( float ) g - tri->g[0x01],
                       ( float ) b - tri->b[0x01], 1.0f },
              v2vp = { ( float ) r - tri->r[0x02],
                       ( float ) g - tri->g[0x02],
                       ( float ) b - tri->b[0x02], 1.0f };
    float LEN0, LEN1, LEN2, LENF;
    G3DVECTOR c0, c1, c2, cf;

    g3dvector_cross ( &v0v1, &v1v2, &cf );

    g3dvector_cross ( &v0vp, &v0v1, &c0 );
    g3dvector_cross ( &v1vp, &v1v2, &c1 );
    g3dvector_cross ( &v2vp, &v2v0, &c2 );

    LENF = g3dvector_length ( &cf );
    LEN0 = g3dvector_length ( &c0 );
    LEN1 = g3dvector_length ( &c1 );
    LEN2 = g3dvector_length ( &c2 );

/*printf("AREA COLOR: %f %f\n", ( LEN0 + LEN1 + LEN2 ), ( LENF + 64.0f ) );*/

    if ( ( LEN0 + LEN1 + LEN2 ) <= ( LENF + 64.0f ) ) {
        if ( RAT2 ) (*RAT2) = ( LEN0 / LENF );
        if ( RAT0 ) (*RAT0) = ( LEN1 / LENF );
        if ( RAT1 ) (*RAT1) = ( LEN2 / LENF );

        return 0x01;
    }

    return 0x00;
}

/******************************************************************************/
static uint32_t triangle_pointIn ( TRIANGLE *tri, int csrx, 
                                                  int csry,
                                                  float *RAT0,
                                                  float *RAT1,
                                                  float *RAT2 ) {
    G2DVECTOR v0v1 = { ( tri->x[0x01] - tri->x[0x00] ),
                       ( tri->y[0x01] - tri->y[0x00] ) },
              v1v2 = { ( tri->x[0x02] - tri->x[0x01] ),
                       ( tri->y[0x02] - tri->y[0x01] ) },
              v2v0 = { ( tri->x[0x00] - tri->x[0x02] ),
                       ( tri->y[0x00] - tri->y[0x02] ) };
    G2DVECTOR v0vp = { ( csrx - tri->x[0x00] ),
                       ( csry - tri->y[0x00] ) },
              v1vp = { ( csrx - tri->x[0x01] ),
                       ( csry - tri->y[0x01] ) },
              v2vp = { ( csrx - tri->x[0x02] ),
                       ( csry - tri->y[0x02] ) };
    float LEN0, LEN1, LEN2, LENF;
    G3DVECTOR c0, c1, c2, cf;

    g2dvector_cross ( &v0v1, &v1v2, &cf );

    g2dvector_cross ( &v0vp, &v0v1, &c0 );
    g2dvector_cross ( &v1vp, &v1v2, &c1 );
    g2dvector_cross ( &v2vp, &v2v0, &c2 );

    LENF = g3dvector_length ( &cf );
    LEN0 = g3dvector_length ( &c0 );
    LEN1 = g3dvector_length ( &c1 );
    LEN2 = g3dvector_length ( &c2 );

    if ( ( LEN0 + LEN1 + LEN2 ) <= ( LENF + EPSILON ) ) {
        if ( RAT2 ) (*RAT2) = ( LEN0 / LENF );
        if ( RAT0 ) (*RAT0) = ( LEN1 / LENF );
        if ( RAT1 ) (*RAT1) = ( LEN2 / LENF );


        return 0x01;
    }

    return 0x00;
}

/******************************************************************************/
static void hexagon_gray ( HEXAGON *hex, unsigned char gray ) {
    int i;

    for ( i = 0x00; i < 0x06; i++ ) {
        hex->tri[i].r[0x00] = gray;
        hex->tri[i].g[0x00] = gray;
        hex->tri[i].b[0x00] = gray;
    }
}

/******************************************************************************/
static void hexagon_init ( HEXAGON *hex, unsigned char gray ) {
    /*** the hexagon is made of 6 triangles pointing to its middle ***/
    int xval[0x06] = { 0x7F, 0xE4, 0xE4, 0x7F, 0x12, 0x12 },
        yval[0x06] = { 0x00, 0x40, 0xC0, 0xFF, 0xC0, 0x40 };
    /*** red, magenta, blue, cyan, green, yellow ***/
    unsigned char rval[0x06] = { 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF },
                  gval[0x06] = { 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF },
                  bval[0x06] = { 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00 };
    int i;

    for ( i = 0x00; i < 0x06; i++ ) {
        int n = ( i + 0x01 ) % 0x06;
        unsigned char R[0x03] = { gray, rval[i], rval[n] },
                      G[0x03] = { gray, gval[i], gval[n] },
                      B[0x03] = { gray, bval[i], bval[n] };
        int X[0x03] = { HEXIMGORIG, xval[i], xval[n] },
            Y[0x03] = { HEXIMGORIG, yval[i], yval[n] };

        triangle_init ( &hex->tri[i], R, G, B, X, Y );
    }
}

/******************************************************************************/
static uint32_t hexagon_rgb2xy ( HEXAGON *hex, unsigned char r,
                                               unsigned char g,
                                               unsigned char b,
                                               int *X,
                                               int *Y,
                                               unsigned char *gray ) {
    int i;

    for ( i = 0x00; i < 0x06; i++ ) {
        float RAT0, RAT1, RAT2;

        triangle_brightness ( &hex->tri[i], r, g, b );

        hexagon_gray ( hex, ( unsigned char ) hex->tri[i].gray );

        (*gray) = ( unsigned char ) hex->tri[i].gray;

        if ( triangle_rgbIn ( &hex->tri[i], r, g, b, &RAT0, &RAT1, &RAT2 ) ) {
            (*X) = ( RAT0 * hex->tri[i].x[0x00] ) +
                   ( RAT1 * hex->tri[i].x[0x01] ) +
                   ( RAT2 * hex->tri[i].x[0x02] );

            (*Y) = ( RAT0 * hex->tri[i].y[0x00] ) +
                   ( RAT1 * hex->tri[i].y[0x01] ) +
                   ( RAT2 * hex->tri[i].y[0x02] );

            return 0x01;
        }
    }

    return 0x00;
}

/******************************************************************************/
static uint32_t hexagon_xy2rgb ( HEXAGON *hex, int csrx,
                                               int csry,
                                               unsigned char *R,
                                               unsigned char *G,
                                               unsigned char *B  ) {
    int i;

    for ( i = 0x00; i < 0x06; i++ ) {
        float RAT0, RAT1, RAT2;

        if ( triangle_pointIn ( &hex->tri[i], csrx, 
                                              csry, &RAT0, &RAT1, &RAT2 ) ) {
            (*R) = ( RAT0 * hex->tri[i].r[0x00] ) +
                   ( RAT1 * hex->tri[i].r[0x01] ) +
                   ( RAT2 * hex->tri[i].r[0x02] );

            (*G) = ( RAT0 * hex->tri[i].g[0x00] ) +
                   ( RAT1 * hex->tri[i].g[0x01] ) +
                   ( RAT2 * hex->tri[i].g[0x02] );

            (*B) = ( RAT0 * hex->tri[i].b[0x00] ) +
                   ( RAT1 * hex->tri[i].b[0x01] ) +
                   ( RAT2 * hex->tri[i].b[0x02] );

            return 0x01;
        }
    }

    return 0x00;
}

/******************************************************************************/
static void triangle_init ( TRIANGLE *tri, unsigned char *r,
                                           unsigned char *g,
                                           unsigned char *b,
                                           int *x,
                                           int *y ) {
    int i;

    for ( i = 0x00; i < 0x03; i++ ) {
        tri->r[i] = r[i];
        tri->g[i] = g[i];
        tri->b[i] = b[i];

        tri->x[i] = x[i];
        tri->y[i] = y[i];
    }
}

/******************************************************************************/
static void triangle_draw ( TRIANGLE *tri, XImage *img, XImage *msk ) {
    HLINE hline[0x100];
    int i;

    memset ( hline, 0x00, sizeof ( hline ) );

    for ( i = 0x00; i < 0x03; i++ ) {
        int n = ( i + 0x01 ) % 0x03;

        drawline ( tri->x[i], tri->y[i],
                   tri->r[i], tri->g[i], tri->b[i],
                   tri->x[n], tri->y[n],
                   tri->r[n], tri->g[n], tri->b[n], hline );
    }

    for ( i = 0x00; i < HEXIMGSIZE; i++ ) {
        if ( hline[i].set == 0x02 ) {

            drawhline ( &hline[i], i, img, msk );
        }
    }
}

/******************************************************************************/
/*** img  : RGB Color image                                                 ***/
/*** msk  : Mask image                                                      ***/
/*** gray : luminance value                                                 ***/
/*** csrx : cursor X position                                               ***/
/*** csry : cursor y position                                               ***/
static void hexagon_draw ( HEXAGON *hex, XImage *img,
                                         XImage *msk,
                                         int csrx,
                                         int csry ) {
    int i;

    for ( i = 0x00; i < 0x06; i++ ) {
        triangle_draw ( &hex->tri[i], img, msk );
    }

    drawCursor ( img, msk, csrx, csry );
}

/******************************************************************************/
static XtProc ClassInitialize ( ) {

    return ( XtProc ) NULL;
}

/******************************************************************************/
static XtInitProc Initialize ( Widget request, Widget init,
                               ArgList args, Cardinal *num ) {

    return ( XtInitProc ) NULL;
}

/******************************************************************************/
static void fillBackground ( XImage *img, Pixel background ) {
    int i, j;

    for ( i = 0x00; i < img->height; i++ ) {
        for ( j = 0x00; j < img->width; j++ ) {
            img->f.put_pixel ( img, j, i, background );
        }
    }
}

/******************************************************************************/
static void getColorCbk ( Widget w, unsigned char *R, 
                                    unsigned char *G, 
                                    unsigned char *B ) {
    XmColorEditWidget xcw = ( XmColorEditWidget ) w;

    (*R) = ( xcw->coloredit.pixel & 0x00FF0000 ) >> 0x10;
    (*G) = ( xcw->coloredit.pixel & 0x0000FF00 ) >> 0x08;
    (*B) = ( xcw->coloredit.pixel & 0x000000FF );
}

/******************************************************************************/
static void setColorCbk ( Widget w, unsigned char R, 
                                    unsigned char G, 
                                    unsigned char B ) {
    XmColorEditWidget xcw = ( XmColorEditWidget ) w;
    Widget scale = getScale ( w );
    unsigned char gray;

    if ( hexagon_rgb2xy ( &xcw->coloredit.hex, R, G, B, &xcw->coloredit.csrx,
                                                        &xcw->coloredit.csry,
                                                        &gray ) ) {
        xcw->coloredit.pixel = ( R << 0x10 ) | ( G << 0x08 ) | B;
    }

    hexagon_gray ( &xcw->coloredit.hex, gray );

    hexagon_draw ( &xcw->coloredit.hex, xcw->coloredit.heximg,
                                        xcw->coloredit.mskimg,
                                        xcw->coloredit.csrx,
                                        xcw->coloredit.csry );

    if ( scale ) XmScaleSetValue ( scale, gray );

    XClearArea ( XtDisplay ( xcw ), XtWindow ( xcw ), 0x00, 0x00,
        /*** Some trick to generate exposure event without redrawing    ***/
        /*** the background, preventing flickering. Needs a better way. ***/
                                                      0x01, 0x01, True );
}

/******************************************************************************/
static void redcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmColorEditWidget xcw = ( XmColorEditWidget ) XtParent ( w );
    char *value = XmTextGetString ( w );
    unsigned char R, G, B;

    if ( xcw->coloredit.lock ) return;

    getColorCbk ( xcw, &R, &G, &B );

    setColorCbk ( xcw, strtol ( value, NULL, 10 ),
                       G, 
                       B );

    XtFree ( value );
}

/******************************************************************************/
static void greencbk ( Widget w, XtPointer client, XtPointer call ) {
    XmColorEditWidget xcw = ( XmColorEditWidget ) XtParent ( w );
    char *value = XmTextGetString ( w );
    unsigned char R, G, B;

    if ( xcw->coloredit.lock ) return;

    getColorCbk ( xcw, &R, &G, &B );

    setColorCbk ( xcw, R,
                       strtol ( value, NULL, 10 ),
                       B );

    XtFree ( value );
}

/******************************************************************************/
static void bluecbk ( Widget w, XtPointer client, XtPointer call ) {
    XmColorEditWidget xcw = ( XmColorEditWidget ) XtParent ( w );
    char *value = XmTextGetString ( w );
    unsigned char R, G, B;

    if ( xcw->coloredit.lock ) return;

    getColorCbk ( xcw, &R, &G, &B );

    setColorCbk ( xcw, R,
                       G,
                       strtol ( value, NULL, 10 ) );

    XtFree ( value );
}

/******************************************************************************/
static XImage *allocRGBImage ( Display *dis, Pixel background ) {
    XImage *img;

    img = XCreateImage ( dis, DefaultVisual ( dis, 0x00 ),
                              DefaultDepth  ( dis, 0x00 ), ZPixmap,
                              0x00, NULL,
                              HEXIMGSIZE, /*** Square Image ...   ***/
                              HEXIMGSIZE, /*** ... Width = Height ***/
                              0x20,  /** True Color ***/
                              0x00 );

    if ( img == NULL ) {
        fprintf ( stderr, "ColorEdit: XCreateImage failed\n" );

        return NULL;
    }

    img->data = ( char * ) calloc ( img->height, img->bytes_per_line );

    if ( img->data == NULL ) {
        fprintf ( stderr, "ColorEdit: memory allocation failed\n" );

        return NULL;
    }

    fillBackground ( img, background );


    return img;
}

/******************************************************************************/
static XImage *allocMaskImage ( Display *dis ) {
    Window win = XDefaultRootWindow ( dis );
    XWindowAttributes attr;
    XImage *img;

    XGetWindowAttributes ( dis, win, &attr );

    switch ( attr.depth ) {
        case 0x10 :
            img = XCreateImage ( dis, DefaultVisual ( dis, 0x00 ),
                                 0x01, XYBitmap,
                                 0x00, NULL,
                                 HEXIMGSIZE, /*** Square Image ...   ***/
                                 HEXIMGSIZE, /*** ... Width = Height ***/
                                 0x10,
                                 0x00 );
        break;

        case 0x18 :
        case 0x20 :
            img = XCreateImage ( dis, DefaultVisual ( dis, 0x00 ),
                                 0x01, XYBitmap,
                                 0x00, NULL,
                                 HEXIMGSIZE, /*** Square Image ...   ***/
                                 HEXIMGSIZE, /*** ... Width = Height ***/
                                 0x20,
                                 0x00 );
        break;

        default :
            fprintf ( stderr, "Unsupported depth : %d bpp\n", attr.depth );
        break;
    }

    if ( img == NULL ) {
        fprintf ( stderr, "ColorEdit: XCreateImage failed\n" );

        return NULL;
    }

    /*** Alloc and fills mask image with zeros ***/
    img->data = ( char * ) calloc ( img->height * img->bytes_per_line, 0x01 );

    if ( img->data == NULL ) {
        fprintf ( stderr, "ColorEdit: memory allocation failed\n" );

        return NULL;
    }


    return img;
}

/******************************************************************************/
/*static void okcbk ( Widget w, XtPointer client, XtPointer call ) {
    Widget parent = XtParent ( w );

    XtDestroyWidget ( parent );
}*/

/******************************************************************************/
static void dragcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmScaleCallbackStruct *scs = ( XmScaleCallbackStruct * ) call;
    XmColorEditWidget xcw = ( XmColorEditWidget ) XtParent ( w );
    int value;

    if ( xcw->coloredit.lock ) return;

    XmScaleGetValue ( w, &value );

    if ( xcw ) {
        unsigned char R, G, B;

        hexagon_gray ( &xcw->coloredit.hex, ( unsigned char ) value );

        hexagon_draw ( &xcw->coloredit.hex, xcw->coloredit.heximg,
                                            xcw->coloredit.mskimg,
                                            xcw->coloredit.csrx,
                                            xcw->coloredit.csry );

        if ( hexagon_xy2rgb ( &xcw->coloredit.hex, xcw->coloredit.csrx,
                                                   xcw->coloredit.csry,
                                                   &R, &G, &B ) ) {
            xcw->coloredit.pixel = ( R << 0x10 ) | ( G << 0x08 ) | B;
        }

        updateText ( ( Widget ) xcw );

        /*** notify attached callbacks ***/
        sendColorEvent ( ( Widget ) xcw, XmCOLORCHANGING, scs->event );

        XClearArea ( XtDisplay ( xcw ), XtWindow ( xcw ), 0x00, 0x00,
        /*** Some trick to generate exposure event without redrawing    ***/
        /*** the background, preventing flickering. Needs a better way. ***/
                                                          0x01, 0x01, True );
    }
}

/******************************************************************************/
static void drawCursor ( XImage *img, XImage *msk, int x, int y ) {
    static int point[0x21][0x02] = { {-1,-3},{ 0,-3},{ 1,-3},
                                     {-1,-2},{ 0,-2},{ 1,-2},
                     {-3,-1},{-2,-1},{-1,-1},{ 0,-1},{ 1,-1},{ 2,-1},{ 3,-1},
                     {-3, 0},{-2, 0},{-1, 0},{ 0, 0},{ 1, 0},{ 2, 0},{ 3, 0},
                     {-3, 1},{-2, 1},{-1, 1},{ 0, 1},{ 1, 1},{ 2, 1},{ 3, 1},
                                     {-1, 2},{ 0, 2},{ 1, 2},
                                     {-1, 3},{ 0, 3},{ 1, 3} };
    int i;

    /*** cursor must lies in the hexagon ***/
    if ( msk->f.get_pixel ( msk, x, y ) ) {
        for ( i = 0x00; i < 0x21; i++ ) {
            int px = point[i][0x00],
                py = point[i][0x01];

            /*** Watch out the hexagon boundaries ***/
            if ( msk->f.get_pixel ( msk, x + px, y + py ) ) {
                if (  px && py ) {
                    img->f.put_pixel ( img, x + px, y + py, 0x00000000 );
                } else {
                    img->f.put_pixel ( img, x + px, y + py, 0xFFFFFFFF );
                }
            }
        }
    }
}

/******************************************************************************/
static void createColorEditLabel ( Widget parent, char *name,
                                                  Dimension x, 
                                                  Dimension y,
                                                  Dimension labwidth, 
                                                  Dimension labheight,
                                                  Dimension txtwidth, 
                                                  Dimension txtheight,
                                                  void (*cbk)( Widget, 
                                                               XtPointer,
                                                               XtPointer ) ) {
    Pixel background, foreground;
    Widget txt, lab;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground,
                            NULL );

    lab = XmVaCreateManagedLabel ( parent, name,
                                   XmNx, x,
                                   XmNy, y,
                                   XmNwidth , labwidth,
                                   XmNheight, labheight,
                                   /*XmNfontList, gui->fontlist,*/
                                   XmNforeground, foreground,
                                   XmNbackground, background,
                                   NULL );

    txt = XmVaCreateManagedText  ( parent, name,
                                   XmNx, x + labwidth,
                                   XmNy, y,
                                   XmNwidth , txtwidth,
                                   XmNheight, txtheight,
                                   /*XmNfontList, gui->fontlist,*/
                                   XmNmarginWidth, 0x00,
                                   XmNmarginHeight, 0x00,
                                   XmNshadowThickness, 0x01,
                                   XmNhighlightOnEnter, True,
                                   XmNhighlightThickness, 0x01,
                                   XmNforeground, foreground,
                                   XmNbackground, background,
                                   NULL );

    /*XtAddCallback ( txt, XmNmodifyVerifyCallback, verifyintcbk, NULL );*/

    if ( cbk ) {
        XtAddCallback ( txt, XmNvalueChangedCallback, cbk, NULL );
    }
}

/******************************************************************************/
static XtRealizeProc Realize ( Widget w, XtValueMask *value_mask,
                               XSetWindowAttributes *attributes ) {
    XmColorEditWidget xcw = ( XmColorEditWidget ) w;
    Display *dis = XtDisplay ( w );
    Pixel background, foreground;
    unsigned char R, G, B;
    unsigned char gray;
    Widget sca, okb;

    /*** this is required if we want to inherit the event mask. ***/
    /*** Redisplay procedure would not be called otherwise      ***/
    xmManagerClassRec.core_class.realize ( w, value_mask, attributes );

    XtVaGetValues ( w, XmNbackground, &background,
                       XmNforeground, &foreground,
                       NULL );

    /*** Alloc RGB and Mask images. Maks is used for checking boundaries ***/
    /** of our hexagon when the user clicks in it.                       ***/
    xcw->coloredit.heximg = allocRGBImage  ( dis, background );
    xcw->coloredit.mskimg = allocMaskImage ( dis );

    /*xcw->coloredit.csrx = HEXIMGORIG;
    xcw->coloredit.csry = HEXIMGORIG;*/

    /*** Build the colorful hexagon. We also have to   ***/
    /*** store pixel value under the cursor. We cannot ***/
    /*** get it after the cursor is drawn because we'd ***/
    /*** get the cursor color value !!                 ***/
    hexagon_init ( &xcw->coloredit.hex, ( unsigned char ) 0x00 );

    R = ( xcw->coloredit.pixel & 0x00FF0000 ) >> 0x10;
    G = ( xcw->coloredit.pixel & 0x0000FF00 ) >> 0x08;
    B = ( xcw->coloredit.pixel & 0x000000FF );

    hexagon_rgb2xy ( &xcw->coloredit.hex, R, G, B, &xcw->coloredit.csrx,
                                                   &xcw->coloredit.csry,
                                                   &gray );

    hexagon_draw ( &xcw->coloredit.hex, xcw->coloredit.heximg,
                                        xcw->coloredit.mskimg,
                                        xcw->coloredit.csrx,
                                        xcw->coloredit.csry );

    createColorEditLabel ( w, COLOREDITREDNAME  , 0x100, 0x20,
                                                   0x20, 0x12,
                                                   0x20, 0x12, redcbk );
    createColorEditLabel ( w, COLOREDITGREENNAME, 0x100, 0x40,
                                                   0x20, 0x12,
                                                   0x20, 0x12, greencbk );
    createColorEditLabel ( w, COLOREDITBLUENAME , 0x100, 0x60,
                                                   0x20, 0x12,
                                                   0x20, 0x12, bluecbk );

    updateText ( w );

    sca = XmVaCreateManagedScale ( w, "SCALE",
                                   XmNx, 0x00,
                                   XmNy, 0x100,
                                   XmNwidth, 0x100,
                                   XmNheight, 0x10,
                                   XmNmaximum, 0xFF,
                                   XmNminimum, 0x00,
                                   XmNhighlightThickness, 0x00,
                                   XmNshadowThickness, 0x01,
                                   XmNorientation, XmHORIZONTAL,
                                   XmNforeground, foreground,
                                   XmNbackground, background,
                                   XmNvalue, gray,
                                   NULL );

    XtAddCallback ( sca, XmNdragCallback, dragcbk, NULL );

    /*okb = XmVaCreateManagedPushButton ( w, COLOREDITOKNAME,
                                        XmNx, 0x130,
                                        XmNy, 0x100,
                                        XmNwidth, 0x20,
                                        XmNheight, 0x20,
                                        XmNhighlightThickness, 0x00,
                                        XmNshadowThickness, 0x01,
                                        XmNforeground, foreground,
                                        XmNbackground, background,
                                        NULL );*/

    return ( XtRealizeProc ) NULL;
}

/******************************************************************************/
static XtWidgetProc Destroy ( Widget w ) {

    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
static XtWidgetProc Resize ( Widget w ) {

    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
static XtExposeProc Redisplay ( Widget w, XEvent *event, Region region ) {
    XmColorEditWidget xcw = ( XmColorEditWidget ) w;
    XImage *img = xcw->coloredit.heximg;
    HEXAGON *hex = &xcw->coloredit.hex;
    XGCValues values;
    Display *dis = XtDisplay ( w );
    Window win = XtWindow ( w );
    uint32_t pixel;
    GC gc;
    int i;

    gc = XtGetGC ( w, 0x00, &values );

    XPutImage ( dis, win, gc, img, 0x00, 0x00,
                                   0x00, 0x00,
                                   HEXIMGSIZE,
                                   HEXIMGSIZE );

    switch ( img->depth ) {
        case 0x10 : {
            uint32_t R = ( ( xcw->coloredit.pixel & 0x00FF0000 ) >> 0x10 ) >> 0x03;
            uint32_t G = ( ( xcw->coloredit.pixel & 0x0000FF00 ) >> 0x08 ) >> 0x02;
            uint32_t B = ( ( xcw->coloredit.pixel & 0x000000FF )         ) >> 0x03;

            pixel = ( R << 0x0B ) | ( G << 0x05 ) | B;
        } break;

        case 0x18 :
        case 0x20 :
            pixel = xcw->coloredit.pixel;
        break;

        default :
        break;
    }


    XSetForeground ( dis, gc, pixel );
    XFillRectangle ( dis, win, gc, 0x100, 0x80, 0x40, 0x30 );

    XSetForeground ( dis, gc, 0x00000000 );

    /*for ( i = 0x00; i < 0x06; i++ ) {
        XDrawLine ( dis, win, gc, hex->tri[i].x[0x01], hex->tri[i].y[0x01],
                                  hex->tri[i].x[0x02], hex->tri[i].y[0x02] );
    }*/

    XDrawRectangle ( dis, win, gc, 0x100, 0x80, 0x40, 0x30 );



    XtReleaseGC ( w, gc );


    return ( XtExposeProc ) NULL;
}

/******************************************************************************/
static XtSetValuesFunc set_values ( Widget current, Widget request, Widget set,
                                    ArgList args, Cardinal *numargs ) {


    return 0x00;
}

/******************************************************************************/
XmColorEditClassRec xmColorEditClassRec = 
                              { { (WidgetClass) &xmManagerClassRec,   /*parent*/
                                  "COLOREDIT",                  /* class_name */
                                  sizeof ( XmColorEditRec ),   /* widget size */
                       ( XtProc ) ClassInitialize,        /* class_initialize */
                                  NULL,              /* class_part_initialize */
                                  FALSE,                      /* class_inited */
                   ( XtInitProc ) Initialize,                   /* initialize */
                                  NULL,                    /* initialize_hook */
                ( XtRealizeProc ) Realize,                         /* realize */
                                  Actions/*NULL*/,                 /* actions */
                                  XtNumber ( Actions )/*0x00*/,/* num_actions */
                                  widgetRes,                      /* resouces */
                                  XtNumber ( widgetRes ),    /* num_resources */
                                  NULLQUARK,                      /*xrm_class */
                                  TRUE,                    /* compress_motion */
                                  XtExposeCompressMaximal,/*compress_exposure */
                                  TRUE,                /* compress_enterleave */
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
                                  /* Composite class part */
                                { XtInheritGeometryManager,
                                  XtInheritChangeManaged,
                                  XtInheritInsertChild,
                                  XtInheritDeleteChild,
                                  NULL }
                                  /* Constraint class part */
                                ,{ NULL,
                                  0x00,
                                  0x00,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL },
                                  /* XmManager class part */
                                {
/* translations                 */ XtInheritTranslations,
/* syn_resources                */ NULL,
/* num_syn_resources            */ 0x00,
/* syn_constraint_resources     */ NULL,
/* num_syn_constraint_resources */ 0x00,
/* parent_process               */ XmInheritParentProcess,
/* extension                    */ NULL } };

/******************************************************************************/
WidgetClass xmColorEditWidgetClass = ( WidgetClass ) &xmColorEditClassRec;
