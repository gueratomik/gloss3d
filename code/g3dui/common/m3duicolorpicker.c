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

#define FGBGBORDER 0x02

/******************************************************************************/
void m3duicolorpicker_resize ( M3DUICOLORPICKER *colpick, 
                               uint32_t          width,
                               uint32_t          height ) {
    colpick->fgcolrec.x      = ( 0x00 ) + FGBGBORDER;
    colpick->fgcolrec.y      = ( 0x00 ) + FGBGBORDER;
    colpick->fgcolrec.width  = ( width  * 0x05 / 0x08 ) - ( FGBGBORDER * 2 );
    colpick->fgcolrec.height = ( height * 0x05 / 0x08 ) - ( FGBGBORDER * 2 );

    colpick->bgcolrec.x      = ( width  * 0x03 / 0x08 ) + FGBGBORDER;
    colpick->bgcolrec.y      = ( height * 0x03 / 0x08 ) + FGBGBORDER;
    colpick->bgcolrec.width  = ( width  * 0x05 / 0x08 ) - ( FGBGBORDER * 2 );
    colpick->bgcolrec.height = ( height * 0x05 / 0x08 ) - ( FGBGBORDER * 2 );

    colpick->bwcolrec.x      = ( 0x00 ) + FGBGBORDER;
    colpick->bwcolrec.y      = ( height * 0x05 / 0x08 ) + FGBGBORDER;
    colpick->bwcolrec.width  = ( width  * 0x03 / 0x08 ) - ( FGBGBORDER * 2 );
    colpick->bwcolrec.height = ( height * 0x03 / 0x08 ) - ( FGBGBORDER * 2 );

    colpick->swcolrec.x      = ( height * 0x05 / 0x08 ) + FGBGBORDER;
    colpick->swcolrec.y      = ( 0x00 ) + FGBGBORDER;
    colpick->swcolrec.width  = ( width  * 0x03 / 0x08 ) - ( FGBGBORDER * 2 );
    colpick->swcolrec.height = ( height * 0x03 / 0x08 ) - ( FGBGBORDER * 2 );
}
