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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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
#include <g3dmouse.h>

/******************************************************************************/
G3DMOUSETOOL *g3dmousetool_new ( char *name, char key, const char **icon,
                                 uint32_t (*init) ( G3DMOUSETOOL *, G3DSCENE *, 
                                                    G3DCAMERA *,
                                                    G3DURMANAGER *, 
                                                    uint32_t ),
                                 void (*draw) ( G3DMOUSETOOL *,
                                                G3DSCENE *, uint32_t ),
                                 int  (*tool) ( G3DMOUSETOOL *, G3DSCENE *,
                                                G3DCAMERA *, G3DURMANAGER *,
                                                uint32_t,
                                                G3DEvent * ),
                                 uint32_t flags ) {

    int len = ( name ) ? strlen ( name ) : 0x00;
    G3DMOUSETOOL *mou;

    if ( len == 0x00 ) {
        fprintf ( stderr, "mousetool_new: tool has no name !\n" );

        return NULL;
    }

    mou = ( G3DMOUSETOOL * ) calloc ( 0x01, sizeof ( G3DMOUSETOOL ) );

    if ( mou == NULL ) {
        fprintf ( stderr, "mousetool_new: memory allocation failed\n" );

        return NULL;
    }

    mou->name = ( char * ) calloc ( 0x01, ( len + 0x01 ) );
    strncpy ( mou->name, name, len );

    mou->icon  = icon;
    mou->init  = init;
    mou->draw  = draw;
    mou->tool  = tool;
    mou->key   = key;
    mou->flags = flags;


    return mou;
}
