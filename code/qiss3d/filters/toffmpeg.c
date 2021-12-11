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
typedef struct _FILTERTOFFMPEG {
    uint32_t flags;   /*** Play on completion ? ***/
    uint32_t height;
    uint32_t width;
    uint32_t depth;
    uint32_t fps;
    char *exportpath;
    char *ffmpegpath;
    char *ffplaypath;
    #ifdef __linux__
    int pipefd[0x02]; /*** pipe to ffmpeg ***/
    pthread_t tid;   /*** the reader thread ***/
    #endif
    #ifdef __MINGW32__
    HANDLE pipefd[0x02];
    HANDLE tid;
    COMPVARS  *cvars;
    PAVIFILE pavi;
    PAVISTREAM pstm,ptmp;
    #endif
} FILTERTOFFMPEG;

/******************************************************************************/
static FILTERTOFFMPEG *filtertoffmpeg_new ( uint32_t  flags, 
                                            uint32_t  width, 
                                            uint32_t  height,
                                            uint32_t  depth,
                                            uint32_t  fps,
                                            uint32_t  nbFrames,
                                            #ifdef __MINGW32__
                                            COMPVARS *cvars,
                                            #endif
                                            char     *exportpath,
                                            char     *ffmpegpath,
                                            char     *ffplaypath ) {
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
    #ifdef unused
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
    #endif
    BITMAPINFO         binfo;
    AVICOMPRESSOPTIONS asopt;
    AVISTREAMINFO      asinfo;

    ftf->cvars = cvars;

    asinfo.fccType = streamtypeVIDEO;
    asinfo.fccHandler = comptypeDIB;
    asinfo.dwFlags = 0;
    asinfo.dwCaps = 0;
    asinfo.wPriority = 0;
    asinfo.wLanguage = 0;
    asinfo.dwScale = 1;
    asinfo.dwRate = fps;
    asinfo.dwStart = 0;
    asinfo.dwLength = nbFrames;
    asinfo.dwInitialFrames = 0 ;
    asinfo.dwSuggestedBufferSize = 0;
    asinfo.dwQuality = -1;
    asinfo.dwSampleSize = 0;
    asinfo.rcFrame.left = 0x00;
    asinfo.rcFrame.top = 0x00;
    asinfo.rcFrame.right = width;
    asinfo.rcFrame.bottom = height;
    asinfo.dwEditCount = 0;
    asinfo.dwFormatChangeCount = 0;

    snprintf( asinfo.szName, 64, "Video #1");

    binfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    binfo.bmiHeader.biWidth = width;
    binfo.bmiHeader.biHeight = height;
    binfo.bmiHeader.biPlanes = 1;
    binfo.bmiHeader.biBitCount = 24;
    binfo.bmiHeader.biCompression = BI_RGB;
    binfo.bmiHeader.biSizeImage = width * height * 0x03;
    binfo.bmiHeader.biXPelsPerMeter = 0;
    binfo.bmiHeader.biYPelsPerMeter = 0;
    binfo.bmiHeader.biClrUsed = 0;
    binfo.bmiHeader.biClrImportant = 0;

    asopt.fccType          = streamtypeVIDEO;
    asopt.fccHandler       = ftf->cvars->fccHandler;
    asopt.dwKeyFrameEvery  = ftf->cvars->lKey;
    asopt.dwQuality        = ftf->cvars->lQ;
    asopt.dwBytesPerSecond = ftf->cvars->lDataRate;
    asopt.dwFlags          = ( ( ftf->cvars->lDataRate > 0 ) ? AVICOMPRESSF_DATARATE  : 0 ) |
                             ( ( ftf->cvars->lKey      > 0 ) ? AVICOMPRESSF_KEYFRAMES : 0 );
    asopt.lpFormat          = NULL;
    asopt.cbFormat          = 0;
    asopt.lpParms           = ftf->cvars->lpState;
    asopt.cbParms           = ftf->cvars->cbState;
    asopt.dwInterleaveEvery = 0;

    AVIFileInit();

    AVIFileOpen ( &ftf->pavi, 
                   ftf->exportpath,
                   OF_CREATE | 
                   OF_WRITE,
                   NULL );

    AVIFileCreateStream ( ftf->pavi, 
                         &ftf->pstm,
                         &asinfo );

    AVIMakeCompressedStream ( &ftf->ptmp,
                               ftf->pstm,
                              &asopt,
                               NULL );

    AVIStreamSetFormat (  ftf->ptmp,
                          0,
                         &binfo.bmiHeader, 
                          sizeof ( BITMAPINFOHEADER ) );
#endif

    return ftf;

}

/******************************************************************************/
static uint32_t filtertoffmpeg_draw ( Q3DFILTER     *fil, 
                                      Q3DJOB        *qjob,
                                      uint32_t       cpuID, 
                                      float          frameID,
                                      unsigned char *img, 
                                      uint32_t       from, 
                                      uint32_t       to, 
                                      uint32_t       depth, 
                                      uint32_t       width ) {
    FILTERTOFFMPEG *ftf = ( FILTERTOFFMPEG * ) fil->data;
    uint32_t height  = ( to - from ) + 0x01;
    uint32_t bufsize = ( height * width  * ( depth >> 0x03 ) );
    uint32_t wbytes, wret;

    
    #ifdef __linux__
    write ( ftf->pipefd[0x01], img, bufsize );
    #endif
    #ifdef __MINGW32__
    BOOL isKeyFrame;
    LONG maxFrameSize = 0x0FFFFFFF;

    #ifdef unused
    wret = WriteFile ( ftf->pipefd[0x01], /* handle to pipe           */
                       img,               /* buffer to write from     */
                       bufsize,           /* number of bytes to write */
                      &wbytes,            /* number of bytes written  */
                       NULL );            /* not overlapped I/O       */

    if ( ( wret == 0x00 ) || ( wbytes != bufsize ) ) {
        fprintf ( stderr, "Failure writing to pipe: %s", GetLastError( ) ); 
    }
    #endif

    /* we must invert the image for using the Windows Video Compression API */
    unsigned char *invimg = malloc ( bufsize );
    uint32_t si, di, j;
    
    for ( si = 0x00, di = ( height - 1 ); si < height; si++, di-- ) {
        for ( j = 0x00; j < width; j++ ) {
            switch ( depth ) {
                case 0x18: {
                    unsigned long srcoffset = ( si * width ) + j,
                                  dstoffset = ( di * width ) + j;
                    unsigned char (*srcimg)[0x03] = img,
                                  (*dstimg)[0x03] = invimg;
                                  
                    dstimg[dstoffset][0x02] = srcimg[srcoffset][0x00];
                    dstimg[dstoffset][0x01] = srcimg[srcoffset][0x01];
                    dstimg[dstoffset][0x00] = srcimg[srcoffset][0x02];
                } break;

                default :
                break;
            }
        }
    }
    
    AVIStreamWrite ( ftf->ptmp, (LONG) frameID, 1, invimg, bufsize, AVIIF_KEYFRAME, NULL, NULL);

    free ( invimg );

    #endif

    return 0x00;

}

/******************************************************************************/
static void filtertoffmpeg_free ( Q3DFILTER *fil ) {
    FILTERTOFFMPEG *ftf = ( FILTERTOFFMPEG * ) fil->data;

    printf ( "Closing FFMpeg pipeline ...\n" );

    #ifdef __linux__
    close ( ftf->pipefd[0x01] ); /*** Reader will see EOF ***/

    /*** Wait for child to terminate ***/
    wait ( NULL );
    #endif
    #ifdef __MINGW32__
    
    #ifdef unused
    CloseHandle ( ftf->pipefd[0x00] );
    CloseHandle ( ftf->pipefd[0x01] );
    #endif

    AVIStreamRelease ( ftf->ptmp );
    AVIStreamRelease ( ftf->pstm );
    AVIFileRelease   ( ftf->pavi );
    AVIFileExit();
    /*ICCompressorFree(&cv);*/
    
    #endif

    printf ( "Closed\n" );

    free ( ftf );
}

/******************************************************************************/
Q3DFILTER *q3dfilter_toFfmpeg_new ( uint32_t flags, 
                                    uint32_t width, 
                                    uint32_t height,
                                    uint32_t depth,
                                    uint32_t fps,
                                    uint32_t nbFrames,
                                    #ifdef __MINGW32__
                                    COMPVARS *cvars,
                                    #endif
                                    char *exportpath,
                                    char *ffmpegpath,
                                    char *ffplaypath ) {
    FILTERTOFFMPEG *ftf;
    Q3DFILTER *fil;

    if ( ffmpegpath == NULL ) return NULL;
    if ( ffplaypath == NULL ) return NULL;

    ftf = filtertoffmpeg_new ( flags, 
                               width,
                               height,
                               depth,
                               fps,
                               nbFrames,
                               #ifdef __MINGW32__
                               cvars,
                               #endif
                               exportpath,
                               ffmpegpath,
                               ffplaypath );


    /*** if ffmpeg not found, ftf is NULL ***/
    if ( ftf == NULL ) {
        fprintf ( stderr, "r3dfilter_toFfmpeg_new: ffmpeg not found !\n" );

        return NULL;
    } 

    fil = q3dfilter_new ( FILTERIMAGE, 
                          TOFFMPEGFILTERNAME,
                          filtertoffmpeg_draw,
                          filtertoffmpeg_free,
                          ftf );

    return fil;
}
