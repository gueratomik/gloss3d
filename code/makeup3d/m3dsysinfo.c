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
#include <makeup3d/makeup3d.h>

/******************************************************************************/
void m3dsysinfo_reset ( M3DSYSINFO *sysinfo ) {
    sysinfo->bgcolor = 0xFFFFFFFF;
    sysinfo->fgcolor = 0x00000000;
}

/******************************************************************************/
static M3DSYSINFO *m3dsysinfo_new ( ) {
    uint32_t structSize = sizeof ( M3DSYSINFO );
    M3DSYSINFO *sysinfo = ( M3DSYSINFO * ) calloc ( 0x01, structSize );

    if ( sysinfo == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    m3dsysinfo_reset ( sysinfo );

    return sysinfo;
}

/******************************************************************************/
M3DSYSINFO *m3dsysinfo_get ( ) {
    /*** This way we don't need a sysinfo global variable or pass it as an ***/
    /*** argument. The first call to g3dsysinfo_get create the sysinfo     ***/
    /*** structure and later calls can retrieve it.                        ***/
    static M3DSYSINFO *sysinfo = NULL;

    if ( sysinfo == NULL ) sysinfo = m3dsysinfo_new ( );

    return sysinfo;
}

/******************************************************************************/
void m3dsysinfo_setPattern ( M3DSYSINFO *sysinfo, M3DPATTERN *pattern ) {
    sysinfo->pattern = pattern;
}

/******************************************************************************/
void m3dsysinfo_setForegroundColor ( M3DSYSINFO *sysinfo, uint32_t fgcolor ) {
    sysinfo->fgcolor = fgcolor;
}

/******************************************************************************/
void m3dsysinfo_setBackgroundColor ( M3DSYSINFO *sysinfo, uint32_t bgcolor ) {
    sysinfo->bgcolor = bgcolor;
}
