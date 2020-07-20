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
#include <lips3d/lips3d.h>

/******************************************************************************/
void l3dsysinfo_reset ( L3DSYSINFO *sysinfo ) {
    sysinfo->bgcolor = 0xFFFFFFFF;
    sysinfo->fgcolor = 0x00000000;
}

/******************************************************************************/
static L3DSYSINFO *l3dsysinfo_new ( ) {
    uint32_t structSize = sizeof ( L3DSYSINFO );
    L3DSYSINFO *sysinfo = ( L3DSYSINFO * ) calloc ( 0x01, structSize );

    if ( sysinfo == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    l3dsysinfo_reset ( sysinfo );

    return sysinfo;
}

/******************************************************************************/
L3DSYSINFO *l3dsysinfo_get ( ) {
    /*** This way we don't need a sysinfo global variable or pass it as an ***/
    /*** argument. The first call to g3dsysinfo_get create the sysinfo     ***/
    /*** structure and later calls can retrieve it.                        ***/
    static L3DSYSINFO *sysinfo = NULL;

    if ( sysinfo == NULL ) sysinfo = l3dsysinfo_new ( );

    return sysinfo;
}

/******************************************************************************/
void l3dsysinfo_setPattern ( L3DSYSINFO *sysinfo, L3DPATTERN *pattern ) {
    sysinfo->pattern = pattern;
}

/******************************************************************************/
void l3dsysinfo_setForegroundColor ( L3DSYSINFO *sysinfo, uint32_t fgcolor ) {
    sysinfo->fgcolor = fgcolor;
}

/******************************************************************************/
void l3dsysinfo_setBackgroundColor ( L3DSYSINFO *sysinfo, uint32_t bgcolor ) {
    sysinfo->bgcolor = bgcolor;
}
