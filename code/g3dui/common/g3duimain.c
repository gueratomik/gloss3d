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
void g3duimain_resize ( G3DUIMAIN *gmn, 
                        uint32_t   width, 
                        uint32_t   height ) {
    /*** Menu ***/
    gmn->menurec.x      = 0x00;
    gmn->menurec.y      = 0x00;
    gmn->menurec.width  = width;
    gmn->menurec.height = 0x20;

    /*** Tool Bar ***/
    gmn->tbarrec.x      = 0x00;
    gmn->tbarrec.y      = 0x20;
    gmn->tbarrec.width  = width;
    gmn->tbarrec.height = TOOLBARBUTTONSIZE;

    /*** Mode Bar ***/
    gmn->mbarrec.x      = 0x00;
    gmn->mbarrec.y      = gmn->tbarrec.y + gmn->tbarrec.height;
    gmn->mbarrec.width  = 0x30;
    gmn->mbarrec.height = height - gmn->menurec.height - gmn->tbarrec.height;

    /*** Upper Right Panel ***/
    gmn->mbrdrec.x      = width  - 0x140;
    gmn->mbrdrec.y      = gmn->tbarrec.y + gmn->tbarrec.height;
    gmn->mbrdrec.width  = 0x140;
    gmn->mbrdrec.height = height - gmn->menurec.height - gmn->tbarrec.height;

    /*** Quad View Panel ***/
    gmn->quadrec.x      = 0x38;
    gmn->quadrec.y      = gmn->tbarrec.y + gmn->tbarrec.height;
    gmn->quadrec.width  = width  - gmn->mbrdrec.width - gmn->quadrec.x;

    gmn->quadrec.height = gmn->mbarrec.height - 0x2A;

    /*** Timeboard Panel ***/
    gmn->timerec.x      = 0x38;
    gmn->timerec.y      = gmn->quadrec.y + gmn->quadrec.height;
    gmn->timerec.width  = gmn->quadrec.width;
    gmn->timerec.height = 0x18;

    /*** Information Panel ***/
    gmn->inforec.x      = 0x38;
    gmn->inforec.y      = gmn->timerec.y + gmn->timerec.height;
    gmn->inforec.width  = gmn->quadrec.width;
    gmn->inforec.height = 0x12;
}
