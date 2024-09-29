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
void g3dimportv2text ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;
    char *fontFile = NULL;
    char *fontFace = NULL;

    g3dimportv2data_incrementIndentLevel ( gid );

    g3dimportv2_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv2_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_TEXT_FONT_FILE : {
                G3DTEXT *txt = ( G3DTEXT * ) gid->currentObject;

                fontFile = calloc ( 0x01, chunkSize + 0x01 );

                g3dimportv2_fread ( fontFile, chunkSize, 0x01, fsrc );
            } break;

            case SIG_OBJECT_TEXT_FONT_FACE : {
                G3DTEXT *txt = ( G3DTEXT * ) gid->currentObject;

                fontFace = calloc ( 0x01, chunkSize + 0x01 );

                g3dimportv2_fread ( fontFace, chunkSize, 0x01, fsrc );
            } break;

            case SIG_OBJECT_TEXT_FONT_SIZE : {
                G3DTEXT *txt = ( G3DTEXT * ) gid->currentObject;
                uint32_t fontSize;

                g3dimportv2_freadl ( &fontSize, fsrc );

                g3dtext_setFont ( txt,
                                  fontFace,
                                  fontFile,
                                  fontSize );

                if ( fontFile ) free ( fontFile );
                if ( fontFace ) free ( fontFace );
            } break;

            case SIG_OBJECT_TEXT_FONT : {
                G3DTEXT *txt = ( G3DTEXT * ) gid->currentObject;


            } break;

            case SIG_OBJECT_TEXT_THICKNESS : {
                G3DTEXT *txt = ( G3DTEXT * ) gid->currentObject;

                g3dimportv2_freadf ( &txt->thickness, fsrc );
            } break;

            case SIG_OBJECT_TEXT_ROUNDNESS : {
                G3DTEXT *txt = ( G3DTEXT * ) gid->currentObject;

                g3dimportv2_freadl ( &txt->roundness, fsrc );
            } break;

            case SIG_OBJECT_TEXT_STRING : {
                char *str = calloc ( 0x01, chunkSize + 0x01 );
                G3DTEXT *txt = ( G3DTEXT * ) gid->currentObject;

                g3dimportv2_fread ( str, chunkSize, 0x01, fsrc );

                g3dtext_setText ( txt, str );

                free ( str );
            } break;

            default : {
                fseek ( fsrc, chunkSize, SEEK_CUR );
            } break;
        }

        /** hand the file back to the parent function ***/
        if ( ftell ( fsrc ) == chunkEnd ) break;

        g3dimportv2_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
        g3dimportv2_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );

    g3dimportv2data_decrementIndentLevel ( gid );
}
