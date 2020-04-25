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
#include <g3dimportv2.h>

/******************************************************************************/
void g3dimportlight ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;

    g3dimportdata_incrementIndentLevel ( gid );

    g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_LIGHT_INTENSITY : {
                G3DLIGHT *lig = ( G3DLIGHT * ) gid->currentObject;

                g3dimport_freadf ( &lig->intensity, fsrc );
            } break;

            case SIG_OBJECT_LIGHT_SHADOW : {
            } break;

            case SIG_OBJECT_LIGHT_SHADOW_CASTING : {
                G3DLIGHT *lig = ( G3DLIGHT * ) gid->currentObject;
                uint32_t shadowing;

                g3dimport_freadl ( &shadowing, fsrc );

                if ( shadowing ) gid->currentObject->flags |= LIGHTCASTSHADOWS;
            } break;

            case SIG_OBJECT_LIGHT_SHADOW_COLOR : {
                G3DLIGHT *lig = ( G3DLIGHT * ) gid->currentObject;
                G3DCOLOR color;

                g3dimport_freadf ( &color.r, fsrc );
                g3dimport_freadf ( &color.g, fsrc );
                g3dimport_freadf ( &color.b, fsrc );
                g3dimport_freadf ( &color.a, fsrc );

                g3dcolor_toRGBA ( &color, &lig->shadowColor );
            } break;

            case SIG_OBJECT_LIGHT_DIFFUSE : {
                G3DLIGHT *lig = ( G3DLIGHT * ) gid->currentObject;
                G3DCOLOR color;

                g3dimport_freadf ( &color.r, fsrc );
                g3dimport_freadf ( &color.g, fsrc );
                g3dimport_freadf ( &color.b, fsrc );
                g3dimport_freadf ( &color.a, fsrc );

                g3dcolor_toRGBA ( &color, &lig->diffuseColor );
            } break;

            case SIG_OBJECT_LIGHT_SPECULAR : {
                G3DLIGHT *lig = ( G3DLIGHT * ) gid->currentObject;
                G3DCOLOR color;

                g3dimport_freadf ( &color.r, fsrc );
                g3dimport_freadf ( &color.g, fsrc );
                g3dimport_freadf ( &color.b, fsrc );
                g3dimport_freadf ( &color.a, fsrc );

                g3dcolor_toRGBA ( &color, &lig->specularColor );
            } break;

            case SIG_OBJECT_LIGHT_AMBIENT : {
                G3DLIGHT *lig = ( G3DLIGHT * ) gid->currentObject;
                G3DCOLOR color;

                g3dimport_freadf ( &color.r, fsrc );
                g3dimport_freadf ( &color.g, fsrc );
                g3dimport_freadf ( &color.b, fsrc );
                g3dimport_freadf ( &color.a, fsrc );

                g3dcolor_toRGBA ( &color, &lig->ambientColor );
            } break;

            default : {
                fseek ( fsrc, chunkSize, SEEK_CUR );
            } break;
        }

        /** hand the file back to the parent function ***/
        if ( ftell ( fsrc ) == chunkEnd ) break;

        g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
        g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );

    g3dimportdata_decrementIndentLevel ( gid );
}
