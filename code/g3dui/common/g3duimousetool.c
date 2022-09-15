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
G3DUIMOUSETOOL *g3duimousetool_new ( G3DMOUSETOOL *tool,
                                     G3DUIMENU    *menu,
                                     uint32_t      flags ) {
    uint32_t memSize = sizeof ( G3DUIMOUSETOOL );
    G3DUIMOUSETOOL *mou = ( G3DUIMOUSETOOL * ) calloc ( 0x01, memSize );

    if ( mou == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    mou->tool  = tool;
    mou->menu  = menu;
    mou->flags = flags;

    return mou;
}

/******************************************************************************/
void g3duimousetool_free ( G3DUIMOUSETOOL *mou ) {
    free ( mou );
}
