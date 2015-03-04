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
#include <g3dui.h>

/******************************************************************************/
void common_g3dui_loadConfiguration ( G3DUI *gui, char *filename ) {
    FILE *fconf = NULL;
    char *str = NULL;
    char *home = getenv ( "HOME" );

    #ifdef __linux__
    if ( access( filename, F_OK ) == 0x00 ) {
    #endif
    #ifdef __MINGW32__
    if ( FileExists ( filename ) ) {
    #endif
        char buf[0x100];

        snprintf ( buf, 0x100, "%s/.gloss3d/", home );

        /*** Create default directory if needed ***/
        #ifdef __linux__
        if ( access ( buf, F_OK ) == 0x00 ) {
        #endif
        #ifdef __MINGW32__
        if ( FileExists ( buf ) ) {
        #endif
            printf ( "Creating default user directory ~/.gloss3d\n" );

            #ifdef __linux__
            mkdir ( buf, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH );
            #endif
            #ifdef __MINGW32__
            CreateDirectory ( buf, NULL );
            #endif
        }

        snprintf ( buf, 0x100, "%s/.gloss3d/gloss3d.conf", home );

        fconf = fopen ( buf, "wt" );
    } else {
        fconf = fopen ( filename, "rt" );
    }

    if ( fconf == NULL ) {
        fprintf ( stderr, "Could not open configuration file %s\n", filename );

        return;
    }

    while ( str = readEntry ( fconf ) ) {
        char param_name[0x100]  = { 0 };
        char param_value[0x100] = { 0 };

        if ( str ) {
            sscanf ( str, "%255[^= ] = %255s", param_name, param_value );
/*printf("%s, %s\n", param_name, param_value );*/
            if ( strcmp ( param_name, "undolevel" ) == 0x00 ) {
                gui->conf.undolevel = atoi ( param_value );

                continue;
            }
        }
    }

    fclose ( fconf );
}
