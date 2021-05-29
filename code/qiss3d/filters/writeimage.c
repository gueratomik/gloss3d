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
typedef struct _FILTERWRITEIMAGE {
    char    *imagename;
    uint32_t sequence; /*** 0 or 1 ***/
} FILTERWRITEIMAGE;

/******************************************************************************/
static FILTERWRITEIMAGE *filterwriteimage_new ( const char *filename, 
                                                uint32_t    seq ) {
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
static void filterwriteimage_free ( Q3DFILTER *fil ) {
    FILTERWRITEIMAGE *fwi = ( FILTERWRITEIMAGE * ) fil->data;

    free ( fwi->imagename );

    free ( fwi );
}

/******************************************************************************/
static uint32_t filterwriteimage_draw ( Q3DFILTER     *fil, 
                                        Q3DJOB        *qjob,
                                        float          frameID,
                                        unsigned char *img, 
                                        uint32_t       from, 
                                        uint32_t       to, 
                                        uint32_t       depth, 
                                        uint32_t       width ) {
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
Q3DFILTER *q3dfilter_writeImage_new ( const char *filename,
                                      uint32_t    seq ) {
    Q3DFILTER *fil;

    fil = q3dfilter_new ( FILTERIMAGE, WRITEIMAGEFILTERNAME,
                                       filterwriteimage_draw,
                                       filterwriteimage_free,
                                       filterwriteimage_new ( filename, seq ) );

    return fil;
}
