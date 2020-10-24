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
#include <g3dengine/g3dengine.h>

/****************************** Private Macros ********************************/
#define FFMPEGPATHLEN 0x200

/******************************************************************************/
uint32_t g3dsysinfo_requestExtensionName ( ) {
    G3DSYSINFO *g3dsysinfo_get ( );

    return sysinfo->extensionName++;
}

/******************************************************************************/
void g3dsysinfo_processAnimatedImages ( G3DSYSINFO *info, 
                                        float       sceneStartFrame,
                                        float       sceneCurrentFrame,
                                        float       sceneEndFrame,
                                        float       sceneFramesPerSecond,
                                        uint64_t engine_flags ) {
    LIST *ltmpAnimatedImages = info->lanimatedImages;

    while ( ltmpAnimatedImages ) {
        G3DIMAGE *img = ( G3DIMAGE * ) ltmpAnimatedImages->data;

        g3dimage_animate ( img,
                           sceneStartFrame, 
                           sceneCurrentFrame,
                           sceneEndFrame,
                           sceneFramesPerSecond,
                           engine_flags );

        g3dimage_bind ( img );

        ltmpAnimatedImages = ltmpAnimatedImages->next;
    }
}

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
void g3dsysinfo_reset ( G3DSYSINFO *sysinfo ) {
    uint32_t nbCpus = g3dcore_getNumberOfCPUs ( );
    uint32_t ptrSize = sizeof ( void * );
    uint32_t i;

    if ( sysinfo->ffmpegPath  ) free ( sysinfo->ffmpegPath  );
    if ( sysinfo->ffplayPath  ) free ( sysinfo->ffplayPath  );
    if ( sysinfo->ffprobePath ) free ( sysinfo->ffprobePath );

    sysinfo->ffmpegPath  = getFfmpegPath  ( );
    sysinfo->ffplayPath  = getFfplayPath  ( );
    sysinfo->ffprobePath = getFfprobePath ( );

    sysinfo->nbcpu = nbCpus;

    if ( sysinfo->subdivisions ) {
        for ( i = 0x00; i < sysinfo->nbcpu; i++ ) {
            if ( sysinfo->subdivisions[i] ) {
                g3dsubdivisionV3_free ( sysinfo->subdivisions[i] );
            }
        }

        free ( sysinfo->subdivisions );
    }

    sysinfo->subdivisions = ( G3DSUBDIVISION ** ) calloc ( nbCpus, ptrSize );

    for ( i = 0x00; i < sysinfo->nbcpu; i++ ) {
        sysinfo->subdivisions[i] = g3dsubdivisionV3_new ( );
    }

    if ( sysinfo->backgroundImage ) {
        g3dimage_free ( sysinfo->backgroundImage );

        sysinfo->backgroundImage = NULL;
    }
}

/******************************************************************************/
static G3DSYSINFO *g3dsysinfo_new ( ) {
    uint32_t structSize = sizeof ( G3DSYSINFO );
    uint32_t ptrSize = sizeof ( void * );
    G3DSYSINFO *sysinfo = ( G3DSYSINFO * ) calloc ( 0x01, structSize );
    uint32_t i;

    if ( sysinfo == NULL ) {
        fprintf ( stderr, "g3dsysinfo_new: calloc failed\n" );

        return NULL;
    }

    g3dsysinfo_reset ( sysinfo );

    return sysinfo;
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
    static G3DSYSINFO *sysinfo = NULL;

    if ( sysinfo == NULL ) sysinfo = g3dsysinfo_new ( );

    return sysinfo;
}
