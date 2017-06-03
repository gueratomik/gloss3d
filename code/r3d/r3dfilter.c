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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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
#include <r3d.h>

/******************************************************************************/
void r3dfilter_enable ( R3DFILTER *fil ) {
    fil->flags |= ENABLEFILTER;
}

/******************************************************************************/
void r3dfilter_disable ( R3DFILTER *fil ) {
    fil->flags &= (~ENABLEFILTER);
}

/******************************************************************************/
void r3dfilter_setType ( R3DFILTER *fil, uint32_t type ) {
    fil->type = type;
}

/******************************************************************************/
R3DFILTER *r3dfilter_getByName ( LIST *lfil, char *name ) {
    LIST *ltmpfil = lfil;

    while ( ltmpfil ) {
        R3DFILTER *fil = ( R3DFILTER * ) ltmpfil->data;

        if ( strcmp ( fil->name, name ) == 0x00 ) {
            return fil;
        }

        ltmpfil = ltmpfil->next;
    }

    return NULL;
}

/******************************************************************************/
R3DFILTER *r3dfilter_new ( uint32_t type, char *name,
                                          uint32_t (*draw)( R3DFILTER *,
                                                            R3DSCENE *,
                                        /* Frame ID */      float,
                                        /*   Image data  */ unsigned char *,
                                        /* From scanline */ uint32_t,
                                        /*  To scanline  */ uint32_t,
                                        /*     Depth     */ uint32_t,
                                        /*     Width     */ uint32_t ),
                                          void (*free)( R3DFILTER * ),
                                          void  *data ) { /* Priv data */
    R3DFILTER *fil = ( R3DFILTER * ) calloc ( 0x01, sizeof ( R3DFILTER ) );

    if ( fil == NULL ) {
        fprintf ( stderr, "r3dfilter_new: memory allocation failed\n" );

        return NULL;
    }

    r3dfilter_setType ( fil, type );

    fil->draw  = draw;
    fil->data  = data;
    fil->free  = free;
    fil->name  = name;

    r3dfilter_enable ( fil );


    return fil;
}

/******************************************************************************/
void r3dfilter_free ( R3DFILTER *fil ) {
    printf ( "freeing filter \"%s\" ...", fil->name );

    if ( fil->free ) {
        fil->free ( fil );

        printf ( "done\n" );
    } else {
        printf ( "not required\n" );
    }

    free ( fil );
}

/******************************************************************************/
#ifdef __MINGW32__
/*** thanks to http://windows.developpez.com/faq/win32/?page=processus ***/
DWORD filtertoffmpeg_listen_t ( FILTERTOFFMPEG *ftf ) {
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    char args[0x100];

    printf("%s\n", ftf->exportpath );

    snprintf ( args, 0x100, "ffmpeg"
                            " -f rawvideo"
                            " -pix_fmt rgb24"
                            " -vcodec rawvideo"
                            " -s %dx%d"
                            " -r %d"
                            " -y"
                            " -an"
                            " -i pipe:0"
                            " %s", ftf->width, ftf->height, ftf->fps, ftf->exportpath );

    printf ( "Command line: ffmpeg %s\n", args );

    memset ( &si, 0x00, sizeof ( STARTUPINFO ) );

    if ( CreateProcess( ftf->ffmpegpath,
                        args,
                        NULL,
                        NULL,
                        FALSE,
                        0x00,
                        NULL,
                        NULL,
                        &si,
                        &pi ) ) {

        WaitForSingleObject ( pi.hProcess, INFINITE );
 
        CloseHandle ( pi.hProcess );
        CloseHandle ( pi.hThread  );
    }

    return 0x00;
}
#endif

/******************************************************************************/
R3DFILTER *r3dfilter_toFfmpeg_new ( uint32_t flags, 
                                    uint32_t width, 
                                    uint32_t height,
                                    uint32_t depth,
                                    uint32_t fps,
                                    char *exportpath,
                                    char *ffmpegpath,
                                    char *ffplaypath ) {
    FILTERTOFFMPEG *ftf = filtertoffmpeg_new ( flags, width,
                                                      height,
                                                      depth,
                                                      fps,
                                                      exportpath,
                                                      ffmpegpath,
                                                      ffplaypath );
    R3DFILTER *fil;

    /*** if ffmpeg not found, ftf is NULL ***/
    if ( ftf == NULL ) {
        fprintf ( stderr, "r3dfilter_toFfmpeg_new: ffmpeg not found !\n" );

        return NULL;
    } 

    fil = r3dfilter_new ( FILTERIMAGE, TOFFMPEGFILTERNAME,
                                       filtertoffmpeg_draw,
                                       filtertoffmpeg_free,
                                       ftf );

    return fil;
}

/******************************************************************************/
FILTERTOFFMPEG *filtertoffmpeg_new ( uint32_t flags, 
                                     uint32_t width, 
                                     uint32_t height,
                                     uint32_t depth,
                                     uint32_t fps,
                                     char *exportpath,
                                     char *ffmpegpath,
                                     char *ffplaypath ) {
    uint32_t structsize = sizeof ( FILTERTOFFMPEG );
    FILTERTOFFMPEG *ftf = ( FILTERTOFFMPEG * ) calloc ( 0x01, structsize );
    #ifdef __linux__
    /*pthread_attr_t attr;
    pthread_t tid;*/
    pid_t cpid;
    #endif
    #ifdef __MINGW32__
    HANDLE hdl;
    #endif

    if ( ffmpegpath && ( strlen ( ffmpegpath ) == 0x00 ) ) {
        fprintf ( stderr, "filtertoffmpeg_new: ffmpeg is needed !\n" );

        return NULL;
    }

    if ( ftf == NULL ) {
        fprintf ( stderr, "filtertoffmpeg_new: memory allocation failed\n" );

        return NULL;
    }

    ftf->flags  = flags;
    ftf->width  = width;
    ftf->height = height;
    ftf->depth  = depth;
    ftf->fps    = fps;

    ftf->exportpath = strdup ( exportpath );
    ftf->ffmpegpath = strdup ( ffmpegpath );
    ftf->ffplaypath = strdup ( ffplaypath );

#ifdef __linux__
    /*** open ffmpeg pipe ***/
    pipe ( ftf->pipefd );

    /*pthread_attr_init ( &attr );*/

    /*** Start listening incoming raw data ***/
    /*pthread_create ( &ftf->tid, &attr, filtertoffmpeg_listen_t, ftf );*/

    /*** Start listening incoming raw data - Fork version ***/
    if ( ( cpid = fork ( ) ) == -1 ) {
        perror ( "fork error" );

        exit ( EXIT_FAILURE );
    }

    if ( cpid == 0x00 ) {            /*** Child reads from pipe  ***/
        char res[0x10];
        char fps[0x10];

        snprintf ( res, 0x10, "%dx%d", ftf->width, ftf->height );
        snprintf ( fps, 0x10, "%d"   , ftf->fps );

        close ( ftf->pipefd[0x01] ); /*** Close unused write end ***/

        /**********************************************************************/
        /*** http://synfig.sourcearchive.com/documentation/0.62.00/         ***/
        /*** trgt__ffmpeg_8cpp-source.html                                  ***/
        /**********************************************************************/

        /*** Dup pipeout to stdin ***/
        if( dup2 ( ftf->pipefd[0x00], STDIN_FILENO ) == -1 ) return NULL;

        /*** Close the unneeded pipeout ***/
        close ( ftf->pipefd[0x00] );

        /*** call ffmpeg to read STDIN and render our video ***/
        execl ( ftf->ffmpegpath,
                ftf->ffmpegpath,
                 "-f", "rawvideo",
                 "-pix_fmt", "rgb24",
                 "-vcodec", "rawvideo",
                 "-s", res,
                 "-r", fps,
                 "-y",
                 "-an",
                 "-i", "-",
                 "-q:v", "1",
                 ftf->exportpath,
                 NULL );

        printf ( "Exiting FFMpeg\n" );

        _exit ( EXIT_SUCCESS ); /*** Child is not needed anymore ***/
    } else {                   /*** Parent writes to pipe ***/
        close ( ftf->pipefd[0x00] ); /*** Close unused read end  ***/
    }
#endif
#ifdef __MINGW32__
    HANDLE _stdin_rd = NULL;
    HANDLE _stdin_wr = NULL;
    HANDLE _stdout_rd = NULL;
    HANDLE _stdout_wr = NULL;
    SECURITY_ATTRIBUTES sa;

    sa.nLength = sizeof ( SECURITY_ATTRIBUTES );
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if ( CreatePipe ( &ftf->pipefd[0x00], &ftf->pipefd[0x01], &sa, NULL ) == 0x00 ) {
        fprintf ( stderr, "filtertoffmpeg_new: could not create pipe\n" );
    }

    SetHandleInformation ( ftf->pipefd[0x00], HANDLE_FLAG_INHERIT, 0 );
    SetHandleInformation ( ftf->pipefd[0x01], HANDLE_FLAG_INHERIT, 0 );

    SetStdHandle ( STD_INPUT_HANDLE, ftf->pipefd[0x00] );

    hdl = CreateThread ( NULL, 
                         0x00,
                         (LPTHREAD_START_ROUTINE) filtertoffmpeg_listen_t, 
                         ftf,
                         0x00,
                         NULL );
#endif

    return ftf;

}

/******************************************************************************/
uint32_t filtertoffmpeg_draw ( R3DFILTER *fil, R3DSCENE *rsce,
                                               float frameID,
                                               unsigned char *img, 
                                               uint32_t from, 
                                               uint32_t to, 
                                               uint32_t depth, 
                                               uint32_t width ) {
    FILTERTOFFMPEG *ftf = ( FILTERTOFFMPEG * ) fil->data;
    uint32_t height  = ( to - from ) + 0x01;
    uint32_t bufsize = ( height * width  * ( depth >> 0x03 ) );
    uint32_t wbytes, wret;

    #ifdef __linux__
    write ( ftf->pipefd[0x01], img, bufsize );
    #endif
    #ifdef __MINGW32__
    wret = WriteFile ( ftf->pipefd[0x01], /* handle to pipe           */
                       img,               /* buffer to write from     */
                       bufsize,           /* number of bytes to write */
                      &wbytes,            /* number of bytes written  */
                       NULL );            /* not overlapped I/O       */

    if ( ( wret == 0x00 ) || ( wbytes != bufsize ) ) {
        fprintf ( stderr, "Failure writing to pipe: %s", GetLastError( ) ); 
    }

    #endif

    return 0x00;

}

/******************************************************************************/
void filtertoffmpeg_free ( R3DFILTER *fil ) {
    FILTERTOFFMPEG *ftf = ( FILTERTOFFMPEG * ) fil->data;

    printf ( "Closing FFMpeg pipeline ...\n" );

    #ifdef __linux__
    close ( ftf->pipefd[0x01] ); /*** Reader will see EOF ***/

    /*** Wait for child to terminate ***/
    wait ( NULL );
    #endif
    #ifdef __MINGW32__
    CloseHandle ( ftf->pipefd[0x00] );
    CloseHandle ( ftf->pipefd[0x01] );
    #endif

    printf ( "Closed\n" );

    free ( ftf );

}

/******************************************************************************/
uint32_t filtermotionblur_draw ( R3DFILTER *fil, R3DSCENE *rsce,
                                                 float frameID,
                                                 unsigned char *img, 
                                                 uint32_t from, 
                                                 uint32_t to, 
                                                 uint32_t depth, 
                                                 uint32_t width ) {
    G3DSCENE *sce  = ( G3DSCENE  * ) ((R3DOBJECT*)rsce)->obj;
    G3DCAMERA *cam = ( G3DCAMERA * ) ((R3DOBJECT*)rsce->area.rcam)->obj;
    uint32_t x1 = rsce->area.x1, 
             y1 = rsce->area.y1,
             x2 = rsce->area.x2,
             y2 = rsce->area.y2;
    uint32_t height = rsce->area.height;
    int32_t startframe = rsce->rsg->output.startframe,
            endframe = rsce->rsg->output.endframe;
    uint32_t bytesperline = ( depth >> 0x03 ) * width;
    FILTERMOTIONBLUR *fmb = ( FILTERMOTIONBLUR * ) fil->data;
    R3DFILTER *towin = r3dfilter_getByName ( rsce->rsg->input.lfilters, TOWINDOWFILTERNAME  );
    R3DFILTER *tofrm = r3dfilter_getByName ( rsce->rsg->input.lfilters, GOTOFRAMEFILTERNAME );
    LIST *lkeepfilters = NULL;
    float difstep = ( float ) 1.0f / ( fmb->steps + 1 );
    float nextframe = difstep;
    R3DSCENE *blurrsce = rsce;
    int i, j, k;

    /*** dont motion-blurize the first frame ***/
    if ( rsce->curframe == rsce->rsg->output.startframe ) return 0x00;

    /*** init our buffer with the first image ***/
    for ( j = from; j < to; j++ ) {
        uint32_t offset = ( j * bytesperline );
        uint8_t  (*oriimg)[0x03] = ( uint8_t  (*)[0x03] ) &img[offset];
        uint16_t (*avgptr)[0x03] = ( uint16_t (*)[0x03] ) &fmb->buffer[offset];

        for ( k = 0x00; k < width; k++ ) {
            avgptr[k][0x00] = oriimg[k][0x00];
            avgptr[k][0x01] = oriimg[k][0x01];
            avgptr[k][0x02] = oriimg[k][0x02];
        }
    }

    if ( towin ) list_insert ( &lkeepfilters, towin );

    /*** Build interpolated renders ***/
    for ( i = 0x01; i < fmb->steps; i++ ) {
        float frame = ( float ) rsce->curframe - nextframe;

      if ( rsce->cancelled == 0x00 ) {
        /*** Because gotonextframe_draw will jump to frame + 1.0f, we have ***/
        /*** to substract 1.0f ***/
        if ( tofrm ) {
            tofrm->draw ( tofrm, NULL, frame - 1.0f, NULL, 0x00, 0x00, 
                                                           0x00, 0x00 );
        } else {
            fprintf ( stderr, "no callback to jump to nextframe\n" );
        }

        nextframe += difstep;

        /*** Prepare the scene ***/
        blurrsce = r3dscene_new ( rsce->rsg, 0x00 );

        /*** register this child renderscene in case we need to cancel it ***/
        r3dscene_addSubRender ( rsce, blurrsce );

        /*** Render and free the current frame ***/
        blurrsce->rsg->input.lfilters = lkeepfilters;
        r3dscene_render ( blurrsce );
        blurrsce->rsg->input.lfilters = lbackupOriginalfilters;

        /*** unregister this child renderscene. No need to cancel it ***/
        /*** anymore, all threads are over after r3dscene_render().  ***/
        r3dscene_removeSubRender ( rsce, blurrsce );

        for ( j = from; j < to; j++ ) {
            uint32_t offset = ( j * bytesperline );
            uint8_t  (*oriimg)[0x03] = ( uint8_t  (*)[0x03] ) &img[offset];
            uint8_t  (*stpimg)[0x03] = ( uint8_t  (*)[0x03] ) &blurrsce->area.img[offset];
            uint16_t (*avgptr)[0x03] = ( uint16_t (*)[0x03] ) &fmb->buffer[offset];


            for ( k = 0x00; k < width; k++ ) {
                avgptr[k][0x00] += ( stpimg[k][0x00] );
                avgptr[k][0x01] += ( stpimg[k][0x01] );
                avgptr[k][0x02] += ( stpimg[k][0x02] );

                /*** Average pixel values ***/
                if ( i == ( fmb->steps - 1 ) ) {
                    oriimg[k][0x00] = avgptr[k][0x00] / fmb->steps;
                    oriimg[k][0x01] = avgptr[k][0x01] / fmb->steps;
                    oriimg[k][0x02] = avgptr[k][0x02] / fmb->steps;

                    avgptr[k][0x00] = avgptr[k][0x01] = avgptr[k][0x02] = 0x00;

                    /*** Store in the "To Window buffer" as well ***/
                    if ( towin ) {
                        FILTERTOWINDOW *ftw = ( FILTERTOWINDOW * ) towin->data;
                        uint32_t color = ( ( uint32_t ) oriimg[k][0x00] << 0x10 ) |
                                         ( ( uint32_t ) oriimg[k][0x01] << 0x08 ) |
                                         ( ( uint32_t ) oriimg[k][0x02] );
                        #ifdef __linux__
                        ftw->ximg->f.put_pixel ( ftw->ximg, k, j, color );
                        #endif
                        #ifdef __MINGW32__
                        ftw->wimg->f.put_pixel ( ftw->wimg, k, j, color );
                        #endif
                    }
                }
            }
        }

        r3dobject_free  ( ( R3DOBJECT * ) blurrsce );
      } else { return 0x01; } /* cancel all remaining filters */
    }

    /*** free filters after rendering ***/
    /*list_free ( &lkeepfilters, (void(*)(void*)) r3dfilter_free );*/


    return 0x00;
}

/******************************************************************************/
FILTERMOTIONBLUR *filtermotionblur_new ( uint32_t width, uint32_t height, 
                                         uint32_t depth, uint32_t steps ) {
    uint32_t structsize = sizeof ( FILTERMOTIONBLUR );
    FILTERMOTIONBLUR *fmb = ( FILTERMOTIONBLUR * ) calloc ( 0x01, structsize );
    uint32_t bytesperline = ( depth >> 0x03 ) * width;
    uint32_t bufsize = bytesperline * height;

    if ( fmb == NULL ) {
        fprintf ( stderr, "r3dfilter_MotionBlur_new: calloc failed\n" );

        return NULL;
    }

    fmb->width  = width;
    fmb->height = height;
    fmb->depth  = depth;
    fmb->steps  = steps;

    /*** Multiply by 2 to store the pixel on a WORD[], not on a BYTE[]. ***/
    /*** This allows us to get number large enough for later averaging. ***/
    fmb->buffer = ( uint16_t * ) calloc ( 0x02, bufsize );

    return fmb;
}

/******************************************************************************/
void filtermotionblur_free ( R3DFILTER *fil ) {
    FILTERMOTIONBLUR *fmb = ( FILTERMOTIONBLUR * ) fil->data;

    free ( fmb->buffer );

    free ( fmb );
}

/******************************************************************************/
void filtertowindow_free (  R3DFILTER *fil ) {
    FILTERTOWINDOW *ftw = ( FILTERTOWINDOW * ) fil->data;
#ifdef __linux__
    XFreeGC( ftw->dis, ftw->gc );

    XShmDetach ( ftw->dis, &ftw->ssi );
    XDestroyImage ( ftw->ximg );
    shmdt ( ftw->ssi.shmaddr );

    free ( ftw );
#endif
}

/******************************************************************************/
#ifdef __linux__
void filtertowindow_allocXImage ( FILTERTOWINDOW *ftw, 
                                  Display *dis, Window win ) {

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
#endif


#ifdef __MINGW32__
/******************************************************************************/
int put_pixel ( WImage *wimg, uint32_t x, uint32_t y, uint32_t color ) {
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
WImage *WCreateImage ( HDC dc, uint32_t width, 
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
void *WDestroyImage ( WImage *wimg ) {
    DeleteObject ( ( HGDIOBJ ) wimg->hBmp );

    free ( wimg );
}

/******************************************************************************/
void filtertowindow_allocWImage ( FILTERTOWINDOW *ftw, HWND hWnd ) {
    HDC dc = GetDC ( hWnd );
    uint32_t depth = GetDeviceCaps ( dc, BITSPIXEL );
    RECT rec;

    GetWindowRect ( hWnd, &rec );

    ftw->wimg = WCreateImage ( dc, ( rec.right  - rec.left ),
                                   ( rec.bottom - rec.top  ), depth );

    /*BitBlt ( ftw->wimg->dc, 0x00, 0x00, ( rec.right  - rec.left ),
                                        ( rec.bottom - rec.top  ), dc,
                            0x00, 0x00, SRCCOPY );*/

    /*glReadPixels ( 0, 0, ( rec.right  - rec.left ),
                         ( rec.bottom - rec.top  ),  GL_BGRA, GL_UNSIGNED_BYTE, ftw->wimg->data );*/

    ReleaseDC ( hWnd, dc );
}
#endif

/******************************************************************************/
#ifdef __linux__
FILTERTOWINDOW *filtertowindow_new ( Display *dis, Window win, 
                                                   uint32_t active_fill ) {
#endif

#ifdef __MINGW32__
FILTERTOWINDOW *filtertowindow_new ( HWND hWnd, uint32_t active_fill ) {
#endif

    uint32_t structsize = sizeof ( FILTERTOWINDOW );
    FILTERTOWINDOW *ftw = ( FILTERTOWINDOW * ) calloc ( 0x01, structsize );

    if ( ftw == NULL ) {
        fprintf ( stderr, "filtertowindow_new: memory allocation failed\n" );

        return NULL;
    }

    ftw->active_fill = active_fill;

#ifdef __linux__
    ftw->dis = dis;
    ftw->win = win;
    ftw->gc  = XCreateGC ( dis, win, 0x00, NULL );

    filtertowindow_allocXImage ( ftw, dis, win );
#endif

#ifdef __MINGW32__
    ftw->hWnd = hWnd;

    filtertowindow_allocWImage ( ftw, hWnd );
#endif

    return ftw;
}

/******************************************************************************/
uint32_t filtertowindow_draw ( R3DFILTER *fil, R3DSCENE *rsce,
                                               float frameID,
                                               unsigned char *img, 
                                               uint32_t from, 
                                               uint32_t to, 
                                               uint32_t depth, 
                                               uint32_t width ) {
    uint32_t bytesperline = ( depth >> 0x03 ) * width;
    FILTERTOWINDOW *ftw = ( FILTERTOWINDOW * ) fil->data;
    uint32_t win_depth, win_width, win_height;
    int i, j;

    #ifdef __linux__
    int CompletionType = XShmGetEventBase ( ftw->dis ) + ShmCompletion;
    XWindowAttributes wattr;

    XGetWindowAttributes ( ftw->dis, ftw->win, &wattr );

    win_depth  = wattr.depth;
    win_width  = wattr.width;
    win_height = wattr.height;
    #endif

    #ifdef __MINGW32__
    HDC dc = GetDC ( ftw->hWnd );
    RECT rec;

    GetWindowRect ( ftw->hWnd, &rec );

    win_depth  = GetDeviceCaps ( dc, BITSPIXEL );
    win_width  = ( rec.right  - rec.left );
    win_height = ( rec.bottom  - rec.top );
    #endif

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
                    #ifdef __linux__
                    ftw->ximg->f.put_pixel ( ftw->ximg, j, i, color );
                    #endif
                    #ifdef __MINGW32__
                    ftw->wimg->f.put_pixel ( ftw->wimg, j, i, color );
                    #endif
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
                    #ifdef __linux__
                    ftw->ximg->f.put_pixel ( ftw->ximg, j, i, color );
                    #endif
                    #ifdef __MINGW32__
                    ftw->wimg->f.put_pixel ( ftw->wimg, j, i, color );
                    #endif
                }
            } break;

            default :
            break;
        }

        if ( ftw->active_fill ) {
            #ifdef __linux__
            XShmPutImage ( ftw->dis, ftw->win, ftw->gc, ftw->ximg,
                           0x00, i,
                           0x00, i,
                           win_width, 0x01, False );
            #endif
            #ifdef __MINGW32__
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
            #endif
        } else {
            #ifdef __linux__
            /*XClearWindow ( ftw->dis, ftw->win );*/
#ifdef WITH_MOTIF
XExposeEvent ev = { Expose, 0, 1,  ftw->dis, ftw->win, 0, 0, win_width, win_height, 0 };
/** For some unknon reason, GTK3 does not need exposure signal to be sent. ***/
/** If I send an exposure event, it flickers. ***/
XSendEvent ( ftw->dis, ftw->win, False, ExposureMask, &ev );
#endif
            /*XFlush ( ftw->dis );*/
            #endif
            #ifdef __MINGW32__
            /*InvalidateRect ( ftw->hWnd, NULL, TRUE );*/
            #endif
        }

        #ifdef __linux__
        XSync ( ftw->dis, 0 );

        /*** because the rendering engine is threaded, ***/
        /*** this causes hangs, so I comment it. ***/
	/*while ( 0x01 ) {
	    XEvent xev;
	    XNextEvent ( ftw->dis, &xev );
	    if ( xev.type == CompletionType ) {
                break;
            }
	}*/

        XFlush ( ftw->dis );
        #endif
    }

    #ifdef __MINGW32__
    ReleaseDC ( ftw->hWnd, dc );
    #endif

    return 0x00;
}

/******************************************************************************/
uint32_t filtertobuffer_draw ( R3DFILTER *fil, R3DSCENE *rsce,
                                               float frameID,
                                               unsigned char *img, 
                                               uint32_t from, 
                                               uint32_t to, 
                                               uint32_t depth, 
                                               uint32_t width ) {
    FILTERTOBUFFER *ftb = ( FILTERTOBUFFER * ) fil->data;
    uint32_t bytesperline = width * ( depth >> 0x03 );
    uint32_t memlen = ( to - from ) * bytesperline;
    uint32_t offset = from * bytesperline;

    memcpy ( &ftb->rawimg[offset], &img[offset], memlen );


    return 0x00;
}

/******************************************************************************/
void filtertobuffer_free ( R3DFILTER *fil ) {
    FILTERTOBUFFER *ftb = ( FILTERTOBUFFER * ) fil->data;

    free ( ftb->rawimg );

    free ( ftb );
}

/******************************************************************************/
FILTERTOBUFFER *filtertobuffer_new ( uint32_t width, uint32_t height,
                                     uint32_t depth, uint32_t background ) {
    uint32_t structsize = sizeof ( FILTERTOBUFFER );
    FILTERTOBUFFER *ftb = ( FILTERTOBUFFER * ) calloc ( 0x01, structsize );
    uint32_t bytesperline =  width * ( depth >> 0x03 );
    uint32_t imgsize = height * bytesperline;
    uint32_t i, j;

    if ( ftb == NULL ) {
        fprintf ( stderr, "filtertowindow_new: memory allocation failed\n" );

        return NULL;
    }

    ftb->width  = width;
    ftb->height = height;
    ftb->depth  = depth;

    ftb->rawimg = malloc ( imgsize );

    for ( i = 0x00; i < height; i++ ) {
        uint32_t offset = i * bytesperline;
        unsigned char *imgptr = &ftb->rawimg[offset];

        switch ( depth ) {
            case 0x10 : {
                for ( j = 0x00; j < width; j++ ) {
                    memcpy ( imgptr, &background, 0x02 );

                    imgptr += 0x02;
                }
            } break;

            case 0x18 : {
                for ( j = 0x00; j < width; j++ ) {
                    memcpy ( imgptr, &background, 0x03 );

                    imgptr += 0x03;
                }
            } break;

            case 0x20 : {
                for ( j = 0x00; j < width; j++ ) {
                    memcpy ( imgptr, &background, 0x04 );

                    imgptr += 0x04;
                }
            } break;

            default :
            break;
        }
    }

    if ( ftb->rawimg == NULL ) {
        fprintf ( stderr, "filtertobuffer_new: memory allocation failed\n" );

        free ( ftb );

        return NULL;
    }


    return ftb;
}

/******************************************************************************/
R3DFILTER *r3dfilter_toBuffer_new ( uint32_t width, uint32_t height,
                                    uint32_t depth, uint32_t background ) {
    R3DFILTER *fil;

    fil = r3dfilter_new ( FILTERIMAGE, TOBUFFERFILTERNAME,
                                      filtertobuffer_draw,
                                /*** freeing is done by the widget on close ***/
                                      filtertobuffer_free,
                                      filtertobuffer_new ( width, height, 
                                                           depth, background ) );

    return fil;
}

/******************************************************************************/
#ifdef __linux__
R3DFILTER *r3dfilter_toWindow_new ( Display *dis, Window win, 
                                                  uint32_t active_fill ) {
    R3DFILTER *fil;

    fil = r3dfilter_new ( FILTERLINE, TOWINDOWFILTERNAME,
                                      filtertowindow_draw,
                                      filtertowindow_free,
                                      filtertowindow_new ( dis, win, active_fill ) );

    return fil;
}
#endif
/******************************************************************************/
R3DFILTER *r3dfilter_MotionBlur_new ( uint32_t width, uint32_t height, 
                                      uint32_t depth, uint32_t steps ) {
    R3DFILTER *fil;

    fil = r3dfilter_new ( FILTERIMAGE, MOTIONBLURFILTERNAME,
                                       filtermotionblur_draw,
                                       filtermotionblur_free,
                                       filtermotionblur_new ( width, height, 
                                                              depth, steps ) );

    return fil;
}

/******************************************************************************/
FILTERWRITEIMAGE *filterwriteimage_new ( const char *filename, uint32_t seq ) {
    uint32_t structsize = sizeof ( FILTERWRITEIMAGE );
    FILTERWRITEIMAGE *fwi = ( FILTERWRITEIMAGE * ) calloc ( 0x01, structsize );

    if ( fwi == NULL ) {
        fprintf ( stderr, "filterwriteimage_new: memory allocation failed\n" );

        return NULL;
    }

    fwi->imagename = strdup ( filename );
    fwi->sequence  = seq;

    return fwi;
}

/******************************************************************************/
void filterwriteimage_free ( R3DFILTER *fil ) {
    FILTERWRITEIMAGE *fwi = ( FILTERWRITEIMAGE * ) fil->data;

    free ( fwi->imagename );

    free ( fwi );
}

/******************************************************************************/
uint32_t filterwriteimage_draw ( R3DFILTER *fil, R3DSCENE *rsce,
                                                 float frameID,
                                                 unsigned char *img, 
                                                 uint32_t from, 
                                                 uint32_t to, 
                                                 uint32_t depth, 
                                                 uint32_t width ) {
    FILTERWRITEIMAGE *fwi = ( FILTERWRITEIMAGE * ) fil->data;
    uint32_t height = ( to - from ) + 0x01;
    char *dirc, *basec, *bname, *dname;
    char imagename[0x100];

    if ( fwi->sequence ) {
        dirc = strdup ( fwi->imagename );
        basec = strdup ( fwi->imagename );

        #ifdef __linux__
        /*** http://linux.die.net/man/3/dirname ***/
        dname = dirname ( dirc );
        bname = basename ( basec );

        snprintf ( imagename, 0x100, "%s/%03d_%s", dname, ( uint32_t ) frameID, bname );
        #endif
        #ifdef __MINGW32__
        dname = dirname ( dirc );

        if ( strrchr ( basec, '\\' ) ) bname = basename ( basec );
        else                           bname = basec;

        snprintf ( imagename, 0x100, "%s\\%03d_%s", dname, ( uint32_t ) frameID, bname );
        #endif

        printf ( "saving %s\n", imagename );

        g3dcore_writeJpeg ( imagename, width, height, depth, img );

        free ( dirc  );
        free ( basec );
    } else {
        printf ( "saving %s\n", fwi->imagename );

        g3dcore_writeJpeg ( fwi->imagename, width, height, depth, img );
    }

    return 0x00;
}

/******************************************************************************/
R3DFILTER *r3dfilter_writeImage_new ( const char *filename, uint32_t seq ) {
    R3DFILTER *fil;

    fil = r3dfilter_new ( FILTERIMAGE, WRITEIMAGEFILTERNAME,
                                       filterwriteimage_draw,
                                       filterwriteimage_free,
                                       filterwriteimage_new ( filename, seq ) );

    return fil;
}
