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
#include <qiss3d/q3d.h>


#ifdef __MINGW32__

/******************************************************************************/
void filtertowindow_free (  Q3DFILTER *fil ) {
    FILTERTOWINDOW *ftw = ( FILTERTOWINDOW * ) fil->data;

    free ( ftw );
}

/******************************************************************************/
FILTERTOWINDOW *filtertowindow_new ( HWND     hWnd,
                                     WImage  *wimg,
                                     uint32_t active_fill ) {
    uint32_t structsize = sizeof ( FILTERTOWINDOW );
    FILTERTOWINDOW *ftw = ( FILTERTOWINDOW * ) calloc ( 0x01, structsize );
    HDC dc;
    RECT rec;

    if ( ftw == NULL ) {
        fprintf ( stderr, "filtertowindow_new: memory allocation failed\n" );

        return NULL;
    }

    ftw->active_fill = active_fill;

    ftw->hWnd = hWnd;
    ftw->wimg = wimg;

    /*filtertowindow_allocWImage ( ftw, hWnd ); */

    return ftw;
}

/******************************************************************************/
uint32_t filtertowindow_draw ( Q3DFILTER     *fil, 
                               Q3DJOB        *qjob,
                               float          frameID,
                               unsigned char *img, 
                               uint32_t       from, 
                               uint32_t       to, 
                               uint32_t       depth, 
                               uint32_t       width ) {
    uint32_t bytesperline = ( depth >> 0x03 ) * width;
    FILTERTOWINDOW *ftw = ( FILTERTOWINDOW * ) fil->data;
    uint32_t win_depth, win_width, win_height;
    int i, j;
    HDC dc = GetDC ( ftw->hWnd );
    RECT rec;

    GetWindowRect ( ftw->hWnd, &rec );

    ftw->depth = win_depth  = GetDeviceCaps ( dc, BITSPIXEL );
    win_width  = ( rec.right  - rec.left );
    win_height = ( rec.bottom  - rec.top );

    for ( i = from; i < to; i++ ) {
        uint32_t offset = ( i * bytesperline );
        unsigned char *imgptr = &img[offset];

        switch ( ftw->depth ) {
            case 0x18 :
            case 0x20 : {
                for ( j = 0x00; j < width; j++ ) {
                    unsigned char R = imgptr[0x00];
                    unsigned char G = imgptr[0x01];
                    unsigned char B = imgptr[0x02];
                    uint32_t color  = ( R << 0x10 | G << 0x08 | B );

                    imgptr += 0x03;

                    /*** Here we assume the XImage has the ***/
                    /*** same size and depth as the img array. ***/
                    ftw->wimg->f.put_pixel ( ftw->wimg, j, i, color );
                }
            } break;

            case 0x10 : {
                for ( j = 0x00; j < width; j++ ) {
                    unsigned char R = ( imgptr[0x00] >> 0x03 );
                    unsigned char G = ( imgptr[0x01] >> 0x02 );
                    unsigned char B = ( imgptr[0x02] >> 0x03 );
                    uint32_t color  = ( R << 0x0B | G << 0x05 | B );

                    imgptr += 0x03;

                    /*** Here we assume the XImage has the ***/
                    /*** same size and depth as the img array. ***/
                    ftw->wimg->f.put_pixel ( ftw->wimg, j, i, color );
                }
            } break;

            default :
            break;
        }

        /** Note: crashes when window closed abruptly. Set active_fill to 1 ***/
        /** only on non-destroyable windows ***/
        if ( ftw->active_fill ) {
            SetDIBitsToDevice ( dc, 0x00,            /* int XDest               */
                                    0x00,            /* int YDest               */
                                    win_width,       /* DWORD dwWidth           */
                                    win_height,      /* DWORD dwHeight          */
                                    0x00,            /* int XSrc                */
                                    0x00,               /* int YSrc                */
                                    0x00,            /* UINT uStartScan         */
                                    win_height,      /* UINT cScanLines         */
                                    ftw->wimg->data, /* const VOID *lpvBits     */
                                    ftw->wimg->bi,   /* const BITMAPINFO *lpbmi */
                                    ftw->wimg->bi->bmiHeader.biClrUsed );
        }
    }

    ReleaseDC ( ftw->hWnd, dc );

    return 0x00;
}

/******************************************************************************/
Q3DFILTER *q3dfilter_toWindow_new ( HWND     hWnd,
                                    WImage  *wimg,
                                    uint32_t active_fill ) {
    Q3DFILTER *fil;

    fil = q3dfilter_new ( FILTERLINE, 
                          TOWINDOWFILTERNAME,
                          filtertowindow_draw,
                          filtertowindow_free,
                          filtertowindow_new ( hWnd, wimg, active_fill ) );

    return fil;
}
#endif
