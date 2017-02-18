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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
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
#include <g3dengine/g3dengine.h>

/******************************************************************************/
static G3DSYSINFO *g3dsysinfo_new ( ) {
    G3DSYSINFO *sif = ( G3DSYSINFO * ) calloc ( 0x01, sizeof ( G3DSYSINFO ) );
    uint32_t i;

    if ( sif == NULL ) {
        fprintf ( stderr, "g3dsysinfo_new: calloc failed\n" );

        return NULL;
    }


    sif->nbcpu = g3dcore_getNumberOfCPUs ( );

    sif->subdivisions = ( G3DSUBDIVISION ** ) calloc ( sif->nbcpu, sizeof ( G3DSUBDIVISION * ) );

    for ( i = 0x00; i < sif->nbcpu; i++ ) {
        sif->subdivisions[i] = g3dsubdivisionV3_new ( );
    }

    return sif;
}
/******************************************************************************/
G3DSUBDIVISION *g3dsysinfo_getSubdivision ( G3DSYSINFO *sif, uint32_t cpuID ) {
    return sif->subdivisions[cpuID];
}

/******************************************************************************/
G3DSYSINFO *g3dsysinfo_get ( ) {
    /*** This way we don't need a sysinfo global variable or pass it as an ***/
    /*** argument. The first call to g3dsysinfo_get create the sysinfo     ***/
    /*** structure and later calls can retrieve it.                        ***/
    static G3DSYSINFO *sif = NULL;

    if ( sif == NULL ) sif = g3dsysinfo_new ( );

    return sif;
}
