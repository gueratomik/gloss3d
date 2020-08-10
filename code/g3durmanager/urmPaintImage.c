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
#include <g3durmanager.h>

/******************************************************************************/
typedef struct _URMPAINTIMAGE {
    G3DIMAGE *img;
    G3DIMAGE *subimg;
    G3DIMAGE *bufsubimg;
    uint32_t  x;
    uint32_t  y;
} URMPAINTIMAGE;

/******************************************************************************/
static URMPAINTIMAGE *urmpaintimage_new ( G3DIMAGE *img,
                                          G3DIMAGE *bufimg,
                                          int32_t   x1,
                                          int32_t   y1,
                                          int32_t   x2,
                                          int32_t   y2 ) {
    uint32_t structsize = sizeof ( URMPAINTIMAGE );

    URMPAINTIMAGE *pim = ( URMPAINTIMAGE * ) calloc ( 0x01, structsize );

    if ( pim == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    pim->img       = img;
    pim->subimg    = g3dimage_getSubImage ( img, x1, y1, x2, y2 );
    pim->bufsubimg = g3dimage_getSubImage ( bufimg, x1, y1, x2, y2 );
    pim->x         = x1;
    pim->y         = y1;

    return pim;
}

/******************************************************************************/
static void urmpaintimage_free ( URMPAINTIMAGE *pim ) {
    free ( pim );
}

/******************************************************************************/
static void paintImage_free ( void *data, uint32_t commit ) {
    URMPAINTIMAGE *pim = ( URMPAINTIMAGE * ) data;

    g3dimage_free ( pim->subimg );
    g3dimage_free ( pim->bufsubimg );

    urmpaintimage_free ( pim );
}

/******************************************************************************/
static void paintImage_undo ( G3DURMANAGER *urm, 
                              void         *data, 
                              uint64_t      engine_flags ) {
    URMPAINTIMAGE *pim = ( URMPAINTIMAGE * ) data;

    g3dimage_merge ( pim->img,
                     pim->bufsubimg,
                     pim->x,
                     pim->y );

    g3dimage_bind ( pim->img );
}

/******************************************************************************/
static void paintImage_redo ( G3DURMANAGER *urm, 
                              void         *data,
                              uint64_t      engine_flags ) {
    URMPAINTIMAGE *pim = ( URMPAINTIMAGE * ) data;

    g3dimage_merge ( pim->img,
                     pim->subimg,
                     pim->x,
                     pim->y );

    g3dimage_bind ( pim->img );
}

/******************************************************************************/
void g3durm_image_paint ( G3DURMANAGER *urm, 
                          G3DIMAGE     *img,
                          G3DIMAGE     *bufimg,
                          int32_t       x1,
                          int32_t       y1,
                          int32_t       x2,
                          int32_t       y2,
                          uint32_t      return_flags ) {
    URMPAINTIMAGE *pim = urmpaintimage_new ( img, bufimg, x1, y1, x2, y2 );

    g3durmanager_push ( urm, paintImage_undo,
                             paintImage_redo,
                             paintImage_free, pim, return_flags );
}
