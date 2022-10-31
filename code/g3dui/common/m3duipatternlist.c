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
void m3duipatternlist_init ( M3DUIPATTERNLIST *patlist, uint32_t size ) {
    patlist->preview_border = 0x01;
    patlist->image_width    = size;
    patlist->image_height   = size;
    patlist->preview_width  = size  + ( patlist->preview_border * 0x02 );
    patlist->preview_height = size  + ( patlist->preview_border * 0x02 );

    list_insert ( &patlist->lpattern, m3dplainrectanglepattern_new ( size ) );
    list_insert ( &patlist->lpattern, m3dplaincirclepattern_new    ( size ) );
    list_insert ( &patlist->lpattern, m3dfadedcirclepattern_new    ( size, 1.0f, 0.00f ) );
    list_insert ( &patlist->lpattern, m3dfadedcirclepattern_new    ( size, 1.0f, 0.10f ) );
    list_insert ( &patlist->lpattern, m3dfadedcirclepattern_new    ( size, 1.0f, 0.25f ) );
    list_insert ( &patlist->lpattern, m3dfadedcirclepattern_new    ( size, 1.0f, 0.50f ) );
    list_insert ( &patlist->lpattern, m3dbrushpattern_new          ( size, ( M3DGIMPBRUSH * ) &brush_0 ) );
    list_insert ( &patlist->lpattern, m3dbrushpattern_new          ( size, ( M3DGIMPBRUSH * ) &brush_1 ) );
    list_insert ( &patlist->lpattern, m3dbrushpattern_new          ( size, ( M3DGIMPBRUSH * ) &brush_2 ) );
    list_insert ( &patlist->lpattern, m3dbrushpattern_new          ( size, ( M3DGIMPBRUSH * ) &brush_3 ) );
    list_insert ( &patlist->lpattern, m3dbrushpattern_new          ( size, ( M3DGIMPBRUSH * ) &brush_4 ) );
    list_insert ( &patlist->lpattern, m3dbrushpattern_new          ( size, ( M3DGIMPBRUSH * ) &brush_5 ) );
}

/******************************************************************************/
void m3duipatternlist_clearPatterns ( M3DUIPATTERNLIST *patlist ) {
    fprintf( stderr, "TODO: %s - clear patterns \n" );
}

/******************************************************************************/
void m3duipatternlist_arrangePreviews ( M3DUIPATTERNLIST *patlist,
                                        uint32_t          width,
                                        uint32_t          height ) {
    LIST *ltmppreview = patlist->lpreview;
    uint32_t x = 0x00;
    uint32_t y = 0x00;
    uint32_t xcumul;

    while ( ltmppreview ) {
        M3DUIPATTERNPREVIEW *patpreview = ( M3DUIPATTERNPREVIEW * ) ltmppreview->data;

        patpreview->rec.x = x;
        patpreview->rec.y = y;
        patpreview->rec.width = patlist->preview_width;
        patpreview->rec.height = patlist->preview_height;

        xcumul += patlist->preview_width;

        if ( xcumul > width ) {
            y += patlist->preview_height;
        }

        ltmppreview = ltmppreview->next;
    }
}

/******************************************************************************/
M3DUIPATTERNPREVIEW *m3duipatternlist_pickPreview ( M3DUIPATTERNLIST *patlist,
                                                    uint32_t          x,
                                                    uint32_t          y ) {
    LIST *ltmppreview = patlist->lpreview;

    while ( ltmppreview ) {
        M3DUIPATTERNPREVIEW *patpreview = ( M3DUIPATTERNPREVIEW * ) ltmppreview->data;

        if ( ( x > patpreview->rec.x ) &&
             ( x < ( patpreview->rec.x + patpreview->rec.width ) ) &&
             ( y > patpreview->rec.y ) &&
             ( y < ( patpreview->rec.y + patpreview->rec.height ) ) ) {

            return patpreview;
        }

        ltmppreview = ltmppreview->next;
    }

    return NULL;
}
