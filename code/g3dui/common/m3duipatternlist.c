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
#include <g3dui.h>

#include <xpm/brush_0.c>
#include <xpm/brush_1.c>
#include <xpm/brush_2.c>
#include <xpm/brush_3.c>
#include <xpm/brush_4.c>
#include <xpm/brush_5.c>

/******************************************************************************/
M3DUIPATTERNLIST *m3duipatternlist_new ( uint32_t size ) {
    uint32_t structsize = sizeof ( M3DUIPATTERNLIST );
    M3DUIPATTERNLIST *pdata = ( M3DUIPATTERNLIST * ) calloc ( 0x01,
                                                              structsize );

    if ( pdata == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    pdata->preview_border = 0x04;
    pdata->image_width    = size;
    pdata->image_height   = size;
    pdata->preview_width  = size  + ( pdata->preview_border * 0x02 );
    pdata->preview_height = size  + ( pdata->preview_border * 0x02 );

    pdata->patterns[0x0B] = ( M3DPATTERN *) m3dplainrectanglepattern_new ( size );
    pdata->patterns[0x0A] = ( M3DPATTERN *) m3dplaincirclepattern_new    ( size );
    pdata->patterns[0x09] = ( M3DPATTERN *) m3dfadedcirclepattern_new    ( size, 1.0f, 0.00f );
    pdata->patterns[0x08] = ( M3DPATTERN *) m3dfadedcirclepattern_new    ( size, 1.0f, 0.10f );
    pdata->patterns[0x07] = ( M3DPATTERN *) m3dfadedcirclepattern_new    ( size, 1.0f, 0.25f );
    pdata->patterns[0x06] = ( M3DPATTERN *) m3dfadedcirclepattern_new    ( size, 1.0f, 0.50f );
    pdata->patterns[0x05] = ( M3DPATTERN *) m3dbrushpattern_new          ( size, ( M3DGIMPBRUSH * ) &brush_0 );
    pdata->patterns[0x04] = ( M3DPATTERN *) m3dbrushpattern_new          ( size, ( M3DGIMPBRUSH * ) &brush_1 );
    pdata->patterns[0x03] = ( M3DPATTERN *) m3dbrushpattern_new          ( size, ( M3DGIMPBRUSH * ) &brush_2 );
    pdata->patterns[0x02] = ( M3DPATTERN *) m3dbrushpattern_new          ( size, ( M3DGIMPBRUSH * ) &brush_3 );
    pdata->patterns[0x01] = ( M3DPATTERN *) m3dbrushpattern_new          ( size, ( M3DGIMPBRUSH * ) &brush_4 );
    pdata->patterns[0x00] = ( M3DPATTERN *) m3dbrushpattern_new          ( size, ( M3DGIMPBRUSH * ) &brush_5 );

    return pdata;
}
