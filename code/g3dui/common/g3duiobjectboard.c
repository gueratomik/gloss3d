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
void g3duiobjectboard_resize ( G3DUIOBJECTBOARD *objboard, 
                               uint32_t          width,
                               uint32_t          height ) {
    /*** Menu ***/
    objboard->menurec.x      = 0x00;
    objboard->menurec.y      = 0x00;
    objboard->menurec.width  = width;
    objboard->menurec.height = 0x20;

    objboard->listrec.x      = 0x00;
    objboard->listrec.y      = objboard->menurec.height;
    objboard->listrec.width  = width;
    objboard->listrec.height = ( height - objboard->menurec.height ) * 0.65f;

    objboard->editrec.x      = 0x00;
    objboard->editrec.y      = objboard->listrec.height +
                               objboard->menurec.height;
    objboard->editrec.width  = width;
    objboard->editrec.height = ( height - objboard->menurec.height ) * 0.35f;
}
