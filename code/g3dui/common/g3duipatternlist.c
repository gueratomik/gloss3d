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

/******************************************************************************/
PATTERNLISTDATA *common_patternlistdata_new ( uint32_t size ) {
    uint32_t structsize = sizeof ( PATTERNLISTDATA );
    PATTERNLISTDATA *pdata = ( PATTERNLISTDATA * ) calloc ( 0x01, structsize );

    if ( pdata == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    pdata->preview_border = 0x04;
    pdata->image_width    = size;
    pdata->image_height   = size;
    pdata->preview_width  = size  + ( pdata->preview_border * 0x02 );
    pdata->preview_height = size  + ( pdata->preview_border * 0x02 );

    pdata->patterns[0x00] = l3dplainrectanglepattern_new ( size );
    pdata->patterns[0x01] = l3dplaincirclepattern_new    ( size );
    pdata->patterns[0x02] = l3dfadedcirclepattern_new    ( size );
    pdata->patterns[0x03] = l3dplainrectanglepattern_new ( size );
    pdata->patterns[0x04] = l3dplainrectanglepattern_new ( size );
    pdata->patterns[0x05] = l3dplainrectanglepattern_new ( size );
    pdata->patterns[0x06] = l3dplainrectanglepattern_new ( size );
    pdata->patterns[0x07] = l3dplainrectanglepattern_new ( size );
    pdata->patterns[0x08] = l3dplainrectanglepattern_new ( size );
    pdata->patterns[0x09] = l3dplainrectanglepattern_new ( size );
    pdata->patterns[0x0A] = l3dplainrectanglepattern_new ( size );
    pdata->patterns[0x0B] = l3dplainrectanglepattern_new ( size );
    pdata->patterns[0x0C] = l3dplainrectanglepattern_new ( size );

    return pdata;
}
