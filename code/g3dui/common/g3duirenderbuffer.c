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

#ifdef __MINGW32__
/******************************************************************************/
static int put_pixel ( WImage  *wimg,
                       uint32_t x,
                       uint32_t y,
                       uint32_t color ) {
    uint32_t offset = ( wimg->bi->bmiHeader.biWidth * ( wimg->bi->bmiHeader.biHeight - 1 - y ) ) + x;

    if ( ( ( x >= 0 ) && ( x < wimg->bi->bmiHeader.biWidth  ) ) &&
         ( ( y >= 0 ) && ( y < wimg->bi->bmiHeader.biHeight ) ) ) {
        switch ( wimg->bi->bmiHeader.biBitCount ) {
            case 0x08 : 
                ((uint8_t *)wimg->data)[offset] = 0xFF;
            break;

            case 0x10 :
                ((uint16_t*)wimg->data)[offset] = color;
            break;

            case 0x20 :
                ((uint32_t*)wimg->data)[offset] = color;
            break;
   
            default :
            break;
        }
    }
}

/******************************************************************************/
static WImage *WCreateImage ( HDC      dc,
                              uint32_t width, 
                              uint32_t height,
                              uint32_t depth ) {
    WImage *wimg = ( WImage * ) calloc ( 0x01, sizeof ( WImage ) );

    if ( wimg == NULL ) {
        fprintf ( stderr, "WCreateImage: calloc failed\n" );

        return NULL;
    }

    wimg->dc = CreateCompatibleDC ( dc );

    wimg->f.put_pixel = put_pixel;

    switch ( depth ) {
        case 0x08 :
            fprintf ( stderr, "WCreateImage: 8 bpp WImage unsupported\n" );
        break;

        case 0x10 :
            wimg->bi = calloc ( 0x01, sizeof ( BITMAPINFO ) + ( 0x03 * sizeof ( DWORD ) ) );

            wimg->bi->bmiHeader.biSize          = sizeof ( BITMAPINFOHEADER );
            wimg->bi->bmiHeader.biPlanes        = 0x01;

            wimg->bi->bmiHeader.biBitCount      = 0x10;
            wimg->bi->bmiHeader.biCompression   = BI_BITFIELDS;
            ((DWORD*)wimg->bi->bmiColors)[0x00] = 0x0000F800;
            ((DWORD*)wimg->bi->bmiColors)[0x01] = 0x000007E0;
            ((DWORD*)wimg->bi->bmiColors)[0x02] = 0x0000001F;
            wimg->bi->bmiHeader.biWidth         = width;
            wimg->bi->bmiHeader.biHeight        = height;

            wimg->hBmp = CreateDIBSection ( wimg->dc, wimg->bi, DIB_RGB_COLORS, &wimg->data, NULL, 0x00 );
        break;

        case 0x20 :
            wimg->bi = calloc ( 0x01, sizeof ( BITMAPINFO ) );

            wimg->bi->bmiHeader.biSize          = sizeof ( BITMAPINFOHEADER );
            wimg->bi->bmiHeader.biPlanes        = 0x01;

            wimg->bi->bmiHeader.biBitCount      = 0x20;
            wimg->bi->bmiHeader.biCompression   = BI_RGB;
            wimg->bi->bmiHeader.biWidth         = width;
            wimg->bi->bmiHeader.biHeight        = height;

            wimg->hBmp = CreateDIBSection ( wimg->dc, wimg->bi, DIB_RGB_COLORS, &wimg->data, NULL, 0x00 );
        break;

        default :
        break;
    }

    SelectObject ( wimg->dc, ( HGDIOBJ ) wimg->hBmp );


    return wimg;
}

/******************************************************************************/
void g3duirenderbuffer_init ( G3DUIRENDERBUFFER *rbuf,
                              HWND               hWnd,
                              uint32_t           width,
                              uint32_t           height ) {
    rbuf->hWnd = hWnd;

    rbuf->wimg = WCreateImage ( GetDC ( hWnd ), width,
                                    height, 24 );
}

/******************************************************************************/
void g3duirenderbuffer_clear ( G3DUIRENDERBUFFER *rbuf ) {
    if ( rbuf->wimg ) {
        DeleteObject ( ( HGDIOBJ ) rbuf->wimg->hBmp );
        free ( rbuf->wimg );

        rbuf->wimg = NULL;
    }
}

#endif /* __MINGW32__ */

/******************************************************************************/
#ifdef __linux__
static XImage *allocXImage ( Display         *dis,
                             Window           win,
                             /*GC               gc,*/
                             uint32_t         width,
                             uint32_t         height,
                             XShmSegmentInfo *ssi ) {
    /*XWindowAttributes wattr;*/
    uint32_t imgsize;
    XImage *ximg;
    void *data;
    int shmid;

    /*XGetWindowAttributes ( dis, win, &wattr );*/

    if ( XShmQueryExtension ( dis ) == 0x00 ) {
        fprintf ( stderr, "filtertowindow_new: XSHM not availabale\n" );

        return NULL;
    }

    /*** http://www.x.org/releases/current/doc/xextproto/shm.html ***/
    /*** HOW_TO_USE_THE_SHARED_MEMORY_EXTENSION ***/
    ximg = XShmCreateImage ( dis, 
                             DefaultVisual ( dis, 0x00 ),
                             /*wattr.depth*/24,
                             ZPixmap,
                             NULL,
                             ssi,
                             /*wattr.*/width,
                             /*wattr.*/height );

    if ( ximg == NULL ) {
        fprintf ( stderr, "XShmCreateImage failed\n" );

        return NULL;
    }

    imgsize = ( ximg->bytes_per_line * ximg->height );

    ssi->shmid    = shmget ( IPC_PRIVATE, imgsize, IPC_CREAT | 0777 );
    ssi->shmaddr  = ximg->data = shmat ( ssi->shmid, 0x00, 0x00 );
    ssi->readOnly = False;

    if ( XShmAttach ( dis, ssi ) == 0x00 ) {
        fprintf ( stderr, "XSHM Failed\n" );

        return NULL;
    }

    shmctl ( ssi->shmid, IPC_RMID, 0x00 );

    XSync ( dis, False );

    return ximg;
}

/******************************************************************************/
void g3duirenderbuffer_init ( G3DUIRENDERBUFFER *rbuf,
                              Display            *dis,
                              Window              win,
                              uint32_t            width,
                              uint32_t            height ) {
/*
    GdkDisplay *gdkdpy = gtk_widget_get_display ( drawingArea );
    GdkWindow  *gdkwin = gtk_widget_get_window  ( drawingArea );

    rbuf->dis = gdk_x11_display_get_xdisplay ( gdkdpy );
    rbuf->win = gdk_x11_window_get_xid       ( gdkwin );
*/
    rbuf->dis = dis;
    rbuf->win = win;

    rbuf->gc  = XCreateGC ( rbuf->dis, 
                            rbuf->win, 0x00, NULL );

    rbuf->ximg = allocXImage ( rbuf->dis, 
                               rbuf->win, 
                               /*rbuf->gc, */
                               width,
                               height,
                              &rbuf->ssi );
}

/******************************************************************************/
void g3duirenderbuffer_clear ( G3DUIRENDERBUFFER *rbuf ) {
    if ( rbuf->ximg ) {
        XFreeGC ( rbuf->dis, rbuf->gc );
        XShmDetach ( rbuf->dis, &rbuf->ssi );
        XDestroyImage ( rbuf->ximg );
        shmdt ( rbuf->ssi.shmaddr );

        rbuf->ximg = NULL;
        rbuf->gc = NULL;
    }
}

#endif
