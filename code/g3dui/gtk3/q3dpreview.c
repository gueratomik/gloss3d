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
#include <g3dui_gtk3.h>

#ifdef unusedQISS3D
/******************************************************************************/
uint32_t filterpreview_draw ( R3DFILTER *fil, R3DSCENE *rsce,
                                              float frameID,
                                              unsigned char (*img)[0x03], 
                                              uint32_t from, 
                                              uint32_t to, 
                                              uint32_t depth, 
                                              uint32_t width ) {
    G3DCAMERA *cam = (G3DCAMERA*)((R3DOBJECT*)rsce->area.rcam)->obj;
    uint32_t scrX = cam->vmatrix[0x00],
             scrY = cam->vmatrix[0x01],
             scrW = cam->vmatrix[0x02],
             scrH = cam->vmatrix[0x03];
    uint32_t height = rsce->area.height;
    G3DUI *gui = ( G3DUI * ) fil->data;
    static DUMPSCREEN dsn = { .action = { .done = PTHREAD_MUTEX_INITIALIZER } };
    uint32_t i, j;

    dsn.action.type = ACTION_DUMPSCREEN;
    dsn.action.gui = gui;
    dsn.x = scrX;
    dsn.y = scrY;
    dsn.width = scrW;
    dsn.height = scrH;

    float wRatio = ( float ) dsn.width / width,
          hRatio = ( float ) dsn.height / height;

    dsn.buffer = calloc ( dsn.width *
                          dsn.height, sizeof ( unsigned char ) * 0x04 );

    g3duicom_requestActionFromMainThread ( gui, &dsn );

    /*** Scale to fit the destination buffer ***/
    for ( j = 0x00; j < height; j++ ) {
        for ( i = 0x00; i < width; i++ ) {
            uint32_t dstOffset =   ( j * width  ) + i;
            uint32_t srcj = dsn.height - ( j * hRatio );
            uint32_t srci = ( i * wRatio );
            uint32_t srcOffset = ( srcj * dsn.width ) + srci;

            if ( ( srcj > 0 && srcj < dsn.height ) &&
                 ( srci > 0 && srci < dsn.width  ) ) {
                img[dstOffset][0x00] = dsn.buffer[srcOffset][0x00];
                img[dstOffset][0x01] = dsn.buffer[srcOffset][0x01];
                img[dstOffset][0x02] = dsn.buffer[srcOffset][0x02];
            }
        }
    }

    free ( dsn.buffer );

    return 0x02; /** don't raytrace ***/
}

/******************************************************************************/
R3DFILTER *r3dfilter_preview_new ( G3DUI *gui ) {
    R3DFILTER *fil;

    fil = r3dfilter_new ( FILTERBEFORE, PREVIEWFILTERNAME,
                                        filterpreview_draw,
                                        NULL,   /** no free()   **/
                                        gui );

    return fil;
}
#endif
