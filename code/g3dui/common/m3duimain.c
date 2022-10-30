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
void m3duimain_resize ( M3DUIMAIN *mmn, 
                        uint32_t   width, 
                        uint32_t   height ) {
    /*** Menu ***/
    mmn->menurec.x      = 0x00;
    mmn->menurec.y      = 0x00;
    mmn->menurec.width  = width;
    mmn->menurec.height = 0x20;

    /*** Tool Bar ***/
    mmn->tbarrec.x      = 0x00;
    mmn->tbarrec.y      = 0x20;
    mmn->tbarrec.width  = width;
    mmn->tbarrec.height = TOOLBARBUTTONSIZE;

    /*** Mode Bar ***/
    mmn->mbarrec.x      = 0x00;
    mmn->mbarrec.y      = mmn->tbarrec.y + mmn->tbarrec.height;
    mmn->mbarrec.width  = 0x30;
    mmn->mbarrec.height = height - mmn->menurec.height - mmn->tbarrec.height;

    /*** Upper Right Panel ***/
    mmn->mbrdrec.x      = width  - 0x140;
    mmn->mbrdrec.y      = mmn->tbarrec.y + mmn->tbarrec.height;
    mmn->mbrdrec.width  = 0x140;
    mmn->mbrdrec.height = height - mmn->menurec.height - mmn->tbarrec.height;

    /*** View Panel ***/
    mmn->viewrec.x      = 0x30;
    mmn->viewrec.y      = mmn->tbarrec.y + mmn->tbarrec.height;
    mmn->viewrec.width  = width  - mmn->mbarrec.width  - mmn->mbrdrec.width;
    mmn->viewrec.height = mmn->mbarrec.height - 0x2A;
}
