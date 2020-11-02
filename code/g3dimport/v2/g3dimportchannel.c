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
void g3dimportchannel ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;

    g3dimportdata_incrementIndentLevel ( gid );

    g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_CHANNEL_SOLID : {
                g3dchannel_enableSolidColor ( gid->currentChannel );

                g3dimport_freadf ( &gid->currentChannel->solid.r, fsrc );
                g3dimport_freadf ( &gid->currentChannel->solid.g, fsrc );
                g3dimport_freadf ( &gid->currentChannel->solid.b, fsrc );
                g3dimport_freadf ( &gid->currentChannel->solid.a, fsrc );
            } break;

            case SIG_CHANNEL_IMAGE : {
                g3dchannel_enableImageColor ( gid->currentChannel );

                if ( chunkSize ) {
                    char *name = ( char * ) calloc ( 0x01, chunkSize + 0x01 );
                    G3DIMAGE *image;

                    g3dimport_fread ( name, chunkSize, 0x01, fsrc );

                    /*** generate a power of two image for the diffuse ***/
                    /*** channel because of OpenGL limitations ***/
                    /*if ( gid->currentChannel == &gid->currentMaterial->diffuse ) {
                        gid->currentChannel->image = g3dimage_load ( name, 1 );
                    } else {
                        gid->currentChannel->image = g3dimage_load ( name, 0 );
                    }*/

                    image = g3dscene_getImage ( gid->currentScene, name );

                    if ( image == NULL ) {
                        /*** Note: we generate only power of 2 images ***/
                        /*** regardless of their use. because images ***/
                        /*** are shared between materials ***/
                        image = g3dimage_load ( name, 1 );

                        g3dscene_registerImage ( gid->currentScene, image );
                    }

                    gid->currentChannel->image = image;

                    free ( name );
                }
            } break;

            case SIG_CHANNEL_PROCEDURAL : {
                g3dchannel_enableProcedural ( gid->currentChannel );
            } break;

            case SIG_CHANNEL_PROCEDURAL_CHESS : {
                G3DPROCEDURAL *proc = g3dproceduralchess_new ( );

                g3dchannel_setProcedural ( gid->currentChannel, proc );
            } break;

            case SIG_CHANNEL_PROCEDURAL_CHESS_GEOMETRY : {
                G3DPROCEDURAL *proc = gid->currentChannel->proc;
                G3DPROCEDURALCHESS *chess = ( G3DPROCEDURALCHESS * ) proc;

                g3dimport_freadl ( &chess->udiv, fsrc );
                g3dimport_freadl ( &chess->vdiv, fsrc );
            } break;

            case SIG_CHANNEL_PROCEDURAL_CHESS_COLORS : {
                G3DPROCEDURAL *proc = gid->currentChannel->proc;
                G3DPROCEDURALCHESS *chess = ( G3DPROCEDURALCHESS * ) proc;
                G3DCOLOR color1, color2;

                g3dimport_freadf ( &color1.r, fsrc );
                g3dimport_freadf ( &color1.g, fsrc );
                g3dimport_freadf ( &color1.b, fsrc );
                g3dimport_freadf ( &color1.a, fsrc );

                g3dimport_freadf ( &color2.r, fsrc );
                g3dimport_freadf ( &color2.g, fsrc );
                g3dimport_freadf ( &color2.b, fsrc );
                g3dimport_freadf ( &color2.a, fsrc );

                g3dcolor_toRGBA ( &color1, &chess->color1 );
                g3dcolor_toRGBA ( &color2, &chess->color2 );
            } break;

            case SIG_CHANNEL_PROCEDURAL_BRICK : {
                G3DPROCEDURAL *proc = g3dproceduralbrick_new ( );

                g3dchannel_setProcedural ( gid->currentChannel, proc );
            } break;

            case SIG_CHANNEL_PROCEDURAL_BRICK_GEOMETRY : {
                G3DPROCEDURAL *proc = gid->currentChannel->proc;
                G3DPROCEDURALBRICK *brick = ( G3DPROCEDURALBRICK * ) proc;

                g3dimport_freadl ( &brick->nbBricksPerLine, fsrc );
                g3dimport_freadl ( &brick->nbLines        , fsrc );
                g3dimport_freadf ( &brick->uspacing       , fsrc );
                g3dimport_freadf ( &brick->vspacing       , fsrc );
            } break;

            case SIG_CHANNEL_PROCEDURAL_BRICK_COLORS : {
                G3DPROCEDURAL *proc = gid->currentChannel->proc;
                G3DPROCEDURALBRICK *brick = ( G3DPROCEDURALBRICK * ) proc;
                G3DCOLOR color1, color2;

                g3dimport_freadf ( &color1.r, fsrc );
                g3dimport_freadf ( &color1.g, fsrc );
                g3dimport_freadf ( &color1.b, fsrc );
                g3dimport_freadf ( &color1.a, fsrc );

                g3dimport_freadf ( &color2.r, fsrc );
                g3dimport_freadf ( &color2.g, fsrc );
                g3dimport_freadf ( &color2.b, fsrc );
                g3dimport_freadf ( &color2.a, fsrc );

                g3dcolor_toRGBA ( &color1, &brick->brickColor   );
                g3dcolor_toRGBA ( &color2, &brick->spacingColor );
            } break;

            case SIG_CHANNEL_PROCEDURAL_RESOLUTION : {
                G3DPROCEDURAL *proc = gid->currentChannel->proc;
                uint32_t width, height, depth;
 
                g3dimport_freadl ( &width, fsrc );
                g3dimport_freadl ( &height, fsrc );
                g3dimport_freadl ( &depth, fsrc );

                /*** check because the procedural might not be created ***/
                /*** if type not supported ***/
                if ( proc ) {
                    /*** we have to do some special treatment to bin image to ***/
                    /*** OpenGL memory. Maybe we could just set some flags in ***/
                    /*** the channel to tell it needs GL binding ***/
                    if ( gid->currentChannel == &gid->currentMaterial->diffuse ) {
                        g3dprocedural_fill ( proc, width, 
                                                   height, depth * 0x08, 0x01 );
                    } else {
                        g3dprocedural_fill ( proc, width, 
                                                   height, depth * 0x08, 0x00 );
                    }
                }
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
