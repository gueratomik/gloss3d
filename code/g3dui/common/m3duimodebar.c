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
uint64_t m3duimodebar_setMode ( M3DUIMODEBAR *mmb, 
                                const char   *modename ) {
    M3DUI *mui = mmb->mui;
    uint64_t curflags = mui->engine_flags & (~UVMODEMASK);
    uint64_t newmode = 0x00;

    if ( strcmp ( modename, MODE_VIEWVERTEX ) == 0x00 ) newmode = VIEWVERTEXUV;
    if ( strcmp ( modename, MODE_VIEWFACE   ) == 0x00 ) newmode = VIEWFACEUV;

    mui->engine_flags = ( curflags | newmode );

    return REDRAWUVMAPEDITOR;
}
