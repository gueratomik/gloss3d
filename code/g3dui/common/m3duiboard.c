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
void m3duiboard_resize ( M3DUIBOARD *board, 
                         uint32_t    width,
                         uint32_t    height ) {
    /*** Patterns ***/
    board->patrec.x      = 0x00;
    board->patrec.y      = 0x00;
    board->patrec.width  = width;
    board->patrec.height = 200;

    /*** Color Picker ***/
    board->colrec.x      = ( width * 0.5f ) - 32;
    board->colrec.y      = board->patrec.y + board->patrec.height;
    board->colrec.width  = 64;
    board->colrec.height = 64;

    /*** Tools ***/
    board->toolrec.x       = 0x00;
    board->toolrec.y       = board->colrec.y + board->colrec.height;
    board->toolrec.width   = width;
    board->toolrec.height  = height - board->toolrec.y;
}
