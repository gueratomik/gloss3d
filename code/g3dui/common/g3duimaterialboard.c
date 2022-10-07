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
void g3duimaterialboard_resize ( G3DUIMATERIALBOARD *matboard, 
                                 uint32_t            width,
                                 uint32_t            height ) {
    /*** Menu ***/
    matboard->menurec.x      = 0x00;
    matboard->menurec.y      = 0x00;
    matboard->menurec.width  = width;
    matboard->menurec.height = 0x20;

    matboard->listrec.x      = 0x00;
    matboard->listrec.y      = matboard->menurec.height;
    matboard->listrec.width  = width;
    matboard->listrec.height = ( height - matboard->menurec.height ) * 0.65f;

    matboard->editrec.x      = 0x00;
    matboard->editrec.y      = matboard->listrec.height +
                               matboard->menurec.height;
    matboard->editrec.width  = width;
    matboard->editrec.height = ( height - matboard->menurec.height ) * 0.35f;
}
