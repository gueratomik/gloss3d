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
#include <g3dimportv3.h>

/******************************************************************************/
void g3dimportv3light ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;

    g3dimportv3data_incrementIndentLevel ( gid );

    g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_LIGHT_INTENSITY : {
                G3DLIGHT *lig = ( G3DLIGHT * ) gid->currentObject;

                g3dimportv3_freadf ( &lig->intensity, fsrc );
            } break;

            case SIG_OBJECT_LIGHT_SHADOW : {
            } break;

            case SIG_OBJECT_LIGHT_SPOT : {
                G3DLIGHT *lig = ( G3DLIGHT * ) gid->currentObject;
                float spotLength, spotAngle, spotFadeAngle;

                g3dimportv3_freadf ( &spotLength   , fsrc );
                g3dimportv3_freadf ( &spotAngle    , fsrc );
                g3dimportv3_freadf ( &spotFadeAngle, fsrc );

                g3dlight_setSpot ( lig, spotLength, spotAngle, spotFadeAngle );
            } break;

            case SIG_OBJECT_LIGHT_SHADOW_CASTING : {
                G3DLIGHT *lig = ( G3DLIGHT * ) gid->currentObject;
                uint32_t shadowing;

                g3dimportv3_freadl ( &shadowing, fsrc );

                if ( shadowing ) gid->currentObject->flags |= LIGHTCASTSHADOWS;
            } break;

            case SIG_OBJECT_LIGHT_SHADOW_COLOR : {
                G3DLIGHT *lig = ( G3DLIGHT * ) gid->currentObject;
                G3DCOLOR color;

                g3dimportv3_freadf ( &color.r, fsrc );
                g3dimportv3_freadf ( &color.g, fsrc );
                g3dimportv3_freadf ( &color.b, fsrc );
                g3dimportv3_freadf ( &color.a, fsrc );

                g3dcolor_toRGBA ( &color, &lig->shadowColor );
            } break;

            case SIG_OBJECT_LIGHT_SHADOW_SOFT : {
                G3DLIGHT *lig = ( G3DLIGHT * ) gid->currentObject;
                uint32_t soft;

                g3dimportv3_freadl ( &soft, fsrc );

                if ( soft ) ((G3DOBJECT*)lig)->flags |= LIGHTSOFTSHADOWS;
            } break;

            case SIG_OBJECT_LIGHT_SHADOW_SOFT_SAMPLING : {
                G3DLIGHT *lig = ( G3DLIGHT * ) gid->currentObject;
                uint32_t nbSamples;

                g3dimportv3_freadl ( &nbSamples, fsrc );

                lig->shadowSample = nbSamples;
            } break;

            case SIG_OBJECT_LIGHT_SHADOW_SOFT_RADIUS : {
                G3DLIGHT *lig = ( G3DLIGHT * ) gid->currentObject;
                float radius;

                g3dimportv3_freadf ( &radius, fsrc );

                lig->shadowRadius = radius;
            } break;

            case SIG_OBJECT_LIGHT_DIFFUSE : {
                G3DLIGHT *lig = ( G3DLIGHT * ) gid->currentObject;
                G3DCOLOR color;

                g3dimportv3_freadf ( &color.r, fsrc );
                g3dimportv3_freadf ( &color.g, fsrc );
                g3dimportv3_freadf ( &color.b, fsrc );
                g3dimportv3_freadf ( &color.a, fsrc );

                g3dcolor_toRGBA ( &color, &lig->diffuseColor );
            } break;

            case SIG_OBJECT_LIGHT_SPECULAR : {
                G3DLIGHT *lig = ( G3DLIGHT * ) gid->currentObject;
                G3DCOLOR color;

                g3dimportv3_freadf ( &color.r, fsrc );
                g3dimportv3_freadf ( &color.g, fsrc );
                g3dimportv3_freadf ( &color.b, fsrc );
                g3dimportv3_freadf ( &color.a, fsrc );

                g3dcolor_toRGBA ( &color, &lig->specularColor );
            } break;

            case SIG_OBJECT_LIGHT_AMBIENT : {
                G3DLIGHT *lig = ( G3DLIGHT * ) gid->currentObject;
                G3DCOLOR color;

                g3dimportv3_freadf ( &color.r, fsrc );
                g3dimportv3_freadf ( &color.g, fsrc );
                g3dimportv3_freadf ( &color.b, fsrc );
                g3dimportv3_freadf ( &color.a, fsrc );

                g3dcolor_toRGBA ( &color, &lig->ambientColor );
            } break;

            default : {
                fseek ( fsrc, chunkSize, SEEK_CUR );
            } break;
        }

        /** hand the file back to the parent function ***/
        if ( ftell ( fsrc ) == chunkEnd ) break;

        g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
        g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );

    g3dimportv3data_decrementIndentLevel ( gid );
}
