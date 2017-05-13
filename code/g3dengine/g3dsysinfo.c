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
#include <g3dengine/g3dengine.h>

/****************************** Private Macros ********************************/
#define FFMPEGPATHLEN 0x200

/******************************************************************************/
static char *getFfmpegPath ( ) {
    char *path = calloc ( FFMPEGPATHLEN, sizeof ( char ) );

    #ifdef __linux__
    FILE *fp = popen ("which ffmpeg", "r" );

    if ( fp ) {
        fgets ( path, FFMPEGPATHLEN, fp );

        if ( strlen ( path ) == 0x00 ) return NULL;

        pclose ( fp );
    }
    #endif
    #ifdef __MINGW32__
    /* get current folder name */
    GetModuleFileName ( GetModuleHandle ( NULL ), path, FFMPEGPATHLEN );

    /* extract the dirname */
    *(strrchr ( path, '\\' )) = 0;

    strncat ( path, "\\ffmpeg.exe", FFMPEGPATHLEN);
    #endif

    /* trim final newline */
    path[strcspn(path, "\n")] = 0;

    return path;
}

/******************************************************************************/
static char *getFfplayPath ( ) {
    char *path = calloc ( FFMPEGPATHLEN, sizeof ( char ) );

    #ifdef __linux__
    FILE *fp = popen ("which ffplay", "r" );

    if ( fp ) {
        fgets ( path, FFMPEGPATHLEN, fp );

        if ( strlen ( path ) == 0x00 ) return NULL;

        pclose ( fp );
    }
    #endif
    #ifdef __MINGW32__
    /* get current folder name */
    GetModuleFileName ( GetModuleHandle ( NULL ), path, FFMPEGPATHLEN );

    /* extract the dirname */
    *(strrchr ( path, '\\' )) = 0;

    strncat ( path, "\\ffplay.exe", FFMPEGPATHLEN);
    #endif

    /* trim final newline */
    path[strcspn(path, "\n")] = 0;

    return path;
}

/******************************************************************************/
static char *getFfprobePath ( ) {
    char *path = calloc ( FFMPEGPATHLEN, sizeof ( char ) );

    #ifdef __linux__
    FILE *fp = popen ("which ffprobe", "r" );

    if ( fp ) {
        fgets ( path, FFMPEGPATHLEN, fp );

        if ( strlen ( path ) == 0x00 ) return NULL;

        pclose ( fp );
    }
    #endif
    #ifdef __MINGW32__
    /* get current folder name */
    GetModuleFileName ( GetModuleHandle ( NULL ), path, FFMPEGPATHLEN );

    /* extract the dirname */
    *(strrchr ( path, '\\' )) = 0;

    strncat ( path, "\\ffprobe.exe", FFMPEGPATHLEN);
    #endif

    /* trim final newline */
    path[strcspn(path, "\n")] = 0;

    return path;
}

/******************************************************************************/
static G3DSYSINFO *g3dsysinfo_new ( ) {
    uint32_t structSize = sizeof ( G3DSYSINFO );
    uint32_t ptrSize = sizeof ( void * );
    G3DSYSINFO *sif = ( G3DSYSINFO * ) calloc ( 0x01, structSize );
    uint32_t i;

    if ( sif == NULL ) {
        fprintf ( stderr, "g3dsysinfo_new: calloc failed\n" );

        return NULL;
    }


    sif->ffmpegPath  = getFfmpegPath  ( );
    sif->ffplayPath  = getFfplayPath  ( );
    sif->ffprobePath = getFfprobePath ( );

    sif->nbcpu = g3dcore_getNumberOfCPUs ( );

    sif->subdivisions = ( G3DSUBDIVISION ** ) calloc ( sif->nbcpu, ptrSize );

    for ( i = 0x00; i < sif->nbcpu; i++ ) {
        sif->subdivisions[i] = g3dsubdivisionV3_new ( );
    }

    return sif;
}
/******************************************************************************/
G3DSUBDIVISION *g3dsysinfo_getSubdivision ( G3DSYSINFO *sif, uint32_t cpuID ) {
    return sif->subdivisions[cpuID];
}

/******************************************************************************/
G3DSYSINFO *g3dsysinfo_get ( ) {
    /*** This way we don't need a sysinfo global variable or pass it as an ***/
    /*** argument. The first call to g3dsysinfo_get create the sysinfo     ***/
    /*** structure and later calls can retrieve it.                        ***/
    static G3DSYSINFO *sif = NULL;

    if ( sif == NULL ) sif = g3dsysinfo_new ( );

    return sif;
}
