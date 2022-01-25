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
#include <g3dmouse.h>

/******************************************************************************/
void g3dmousetool_init ( G3DMOUSETOOL *gtool,
                         char         *name, 
                         char          key, 
                         const char **icon,
                         uint32_t (*init) ( G3DMOUSETOOL *, 
                                            G3DSCENE *, 
                                            G3DCAMERA *,
                                            G3DURMANAGER *, 
                                            uint64_t ),
                         void (*draw) ( G3DMOUSETOOL *,
                                        G3DSCENE *, 
                                        uint64_t ),
                         int  (*tool) ( G3DMOUSETOOL *, 
                                        G3DSCENE *,
                                        G3DCAMERA *, 
                                        G3DURMANAGER *,
                                        uint64_t,
                                        G3DEvent * ),
                         uint32_t flags ) {

    int len = ( name ) ? strlen ( name ) : 0x00;

    gtool->name = ( char * ) calloc ( 0x01, ( len + 0x01 ) );

    strncpy ( gtool->name, name, len );

    gtool->icon  = icon;
    gtool->init  = G3DMOUSE_INITFUNC(init);
    gtool->draw  = G3DMOUSE_DRAWFUNC(draw);
    gtool->tool  = G3DMOUSE_TOOLFUNC(tool);
    gtool->key   = key;
    gtool->flags = flags;
}

/******************************************************************************/
void m3dmousetool_init ( M3DMOUSETOOL *ltool,
                         char *name, char key, const char **icon,
                         uint32_t (*init) ( G3DMOUSETOOL *,
                                            G3DSCENE *, 
                                            G3DCAMERA *,
                                            G3DURMANAGER *, 
                                            uint64_t ),
                         void (*draw) ( G3DMOUSETOOL *,
                                        G3DSCENE *, 
                                        uint64_t ),
                         int  (*tool) ( G3DMOUSETOOL *, 
                                        G3DSCENE *,
                                        G3DCAMERA *, 
                                        G3DURMANAGER *,
                                        uint64_t,
                                        G3DEvent * ),
                         uint32_t flags ) {
    g3dmousetool_init ( &ltool->gtool,
                         name,
                         key,
                         icon,
                         init,
                         draw,
                         tool,
                         flags );
}
