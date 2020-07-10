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
#include <lips3d/lips3d.h>

/******************************************************************************/
static int Init ( L3DTOOL  *tool,
                  int32_t   x,
                  int32_t   y,
                  char     *buffer, 
                  uint32_t  width, 
                  uint32_t  height,
                  uint32_t  bpp,
                  char     *mask,
                  char     *zbuffer,
                  uint32_t  engineFlags ) {
    L3DSPRAYTOOL *sprtool = ( L3DSPRAYTOOL * ) tool;

    sprtool->oldx = x;
    sprtool->oldy = y;

    memset ( zbuffer, 0x00, width * height );

    return 0x00;
}

/******************************************************************************/
static int Paint ( L3DTOOL       *tool,
                   int32_t        x,
                   int32_t        y,
                   unsigned char *buffer, 
                   uint32_t       width, 
                   uint32_t       height,
                   uint32_t       bpp,
                   unsigned char *mask,
                   unsigned char *zbuffer,
                   int32_t       *updx,
                   int32_t       *updy,
                   int32_t       *updw,
                   int32_t       *updh,
                   uint32_t       engineFlags ) {
    L3DSPRAYTOOL *sprtool = ( L3DSPRAYTOOL * ) tool;
    uint32_t size = 0x10, half = size / 0x02;
    /*** we use some margin because fro some unknown reason, glTexSubImage2D ***/
    /*** does not seem to copy the subimage completely ***/
    uint32_t margin = 10; 
    int32_t x1 = ( x < sprtool->oldx ) ? x - half : sprtool->oldx - half - margin,
            y1 = ( y < sprtool->oldy ) ? y - half : sprtool->oldy - half - margin,
            x2 = ( x > sprtool->oldx ) ? x + half : sprtool->oldx + half + margin,
            y2 = ( y > sprtool->oldy ) ? y + half : sprtool->oldy + half + margin;

    if ( updx ) (*updx) = x1;
    if ( updy ) (*updy) = y1;
    if ( updw ) (*updw) = ( x2 - x1 ) + 1;
    if ( updh ) (*updh) = ( y2 - y1 ) + 1;

    if ( (*updx) < 0x00 ) (*updx) = 0x00;
    if ( (*updy) < 0x00 ) (*updy) = 0x00;

    if ( ( (*updx) + (*updw) ) > width  ) (*updw) = width  - (*updx);
    if ( ( (*updy) + (*updh) ) > height ) (*updh) = height - (*updy);

    l3core_paintLine ( tool->pattern,
                       0xFF808080,
                       0.1f,
                       sprtool->oldx,
                       sprtool->oldy,
                       x,
                       y,
                       buffer,
                       width,
                       height, 
                       bpp, 
                       mask,
                       zbuffer,
                       engineFlags );

    /*l3dpattern_paint ( tool->pattern,
                       0xFF,
                       1.0f,
                       x,
                       y,
                       buffer,
                       width,
                       height,
                       bpp,
                       mask,
                       zbuffer,
                       engineFlags );*/

    sprtool->oldx = x;
    sprtool->oldy = y;

    return 0x00;
}

/******************************************************************************/
static int Done ( L3DTOOL       *tool,
                  int32_t        x,
                  int32_t        y,
                  unsigned char *buffer, 
                  uint32_t       width, 
                  uint32_t       height,
                  uint32_t       bpp,
                  unsigned char *mask,
                  unsigned char *zbuffer,
                  uint32_t       engineFlags ) {
    L3DSPRAYTOOL *sprtool = ( L3DSPRAYTOOL * ) tool;

    return 0x00;
}

/******************************************************************************/
L3DSPRAYTOOL *l3dspraytool_new ( ) {
    uint32_t structSize = sizeof ( L3DSPRAYTOOL );
    L3DSPRAYTOOL *sprtool = ( L3DSPRAYTOOL * ) calloc ( 0x01, structSize );

    if ( sprtool == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    sprtool->tool.pattern = l3dplainrectanglepattern_new ( 24 );

    sprtool->tool.init  = Init;
    sprtool->tool.paint = Paint;
    sprtool->tool.done  = Done;

    return sprtool;
}






