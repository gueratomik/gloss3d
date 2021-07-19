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

/******************************************************************************/
#ifdef __linux__
static void filtertowindow_allocXImage ( FILTERTOWINDOW *ftw, 
                                         Display        *dis, 
                                         Window          win ) {

    XWindowAttributes wattr;
    uint32_t imgsize;
    XImage *ximg;
    void *data;
    int shmid;

    XGetWindowAttributes ( ftw->dis, ftw->win, &wattr );

    if ( XShmQueryExtension ( dis ) == 0x00 ) {
        fprintf ( stderr, "filtertowindow_new: XSHM not availabale\n" );

        return;
    }

    /*** http://www.x.org/releases/current/doc/xextproto/shm.html ***/
    /*** HOW_TO_USE_THE_SHARED_MEMORY_EXTENSION ***/
    ximg = XShmCreateImage ( dis, DefaultVisual ( dis, 0x00 ),
                             wattr.depth, ZPixmap, NULL, &ftw->ssi,
                             wattr.width,
                             wattr.height );

    if ( ximg == NULL ) {
        fprintf ( stderr, "XShmCreateImage failed\n" );

        return;
    }

    imgsize = ( ximg->bytes_per_line * ximg->height );

    ftw->ssi.shmid    = shmget ( IPC_PRIVATE, imgsize, IPC_CREAT | 0777 );
    ftw->ssi.shmaddr  = ximg->data = shmat ( ftw->ssi.shmid, 0x00, 0x00 );
    ftw->ssi.readOnly = False;

    if ( XShmAttach ( dis, &ftw->ssi ) == 0x00 ) {
        fprintf ( stderr, "XSHM Failed\n" );

        return;
    }

    shmctl ( ftw->ssi.shmid, IPC_RMID, 0x00 );

    XSync ( dis, False );

    ftw->ximg = ximg;
}

/******************************************************************************/
void filtertowindow_free (  Q3DFILTER *fil ) {
    FILTERTOWINDOW *ftw = ( FILTERTOWINDOW * ) fil->data;

    XSync  ( ftw->dis, 0 );
    XFlush ( ftw->dis );

    XFreeGC( ftw->dis, ftw->gc );
    XShmDetach ( ftw->dis, &ftw->ssi );
    XDestroyImage ( ftw->ximg );
    shmdt ( ftw->ssi.shmaddr );

    free ( ftw );
}

/******************************************************************************/
FILTERTOWINDOW *filtertowindow_new ( Display *dis, 
                                     Window   win, 
                                     uint32_t active_fill ) {

    uint32_t structsize = sizeof ( FILTERTOWINDOW );
    FILTERTOWINDOW *ftw = ( FILTERTOWINDOW * ) calloc ( 0x01, structsize );

    if ( ftw == NULL ) {
        fprintf ( stderr, "filtertowindow_new: memory allocation failed\n" );

        return NULL;
    }

    ftw->active_fill = active_fill;

    ftw->dis = dis;
    ftw->win = win;
    ftw->gc  = XCreateGC ( dis, win, 0x00, NULL );

    filtertowindow_allocXImage ( ftw, dis, win );

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
    int CompletionType = XShmGetEventBase ( ftw->dis ) + ShmCompletion;
    XWindowAttributes wattr;

    XGetWindowAttributes ( ftw->dis, ftw->win, &wattr );

    win_depth  = wattr.depth;
    win_width  = wattr.width;
    win_height = wattr.height;

    for ( i = from; i < to; i++ ) {
        uint32_t offset = ( i * bytesperline );
        unsigned char *imgptr = &img[offset];

        switch ( win_depth ) {
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
                    ftw->ximg->f.put_pixel ( ftw->ximg, j, i, color );
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
                    ftw->ximg->f.put_pixel ( ftw->ximg, j, i, color );
                }
            } break;

            default :
            break;
        }

        if ( ftw->active_fill ) {
            XShmPutImage ( ftw->dis, ftw->win, ftw->gc, ftw->ximg,
                           0x00, i,
                           0x00, i,
                           win_width, 0x01, False );
        }

        XSync  ( ftw->dis, 0 );
        XFlush ( ftw->dis );
    }

    return 0x00;
}

/******************************************************************************/
Q3DFILTER *q3dfilter_toWindow_new ( Display *dis, 
                                    Window   win, 
                                    uint32_t active_fill ) {
    Q3DFILTER *fil;

    fil = q3dfilter_new ( FILTERLINE, 
                          TOWINDOWFILTERNAME,
                          filtertowindow_draw,
                          filtertowindow_free,
                          filtertowindow_new ( dis, win, active_fill ) );

    return fil;
}
#endif
