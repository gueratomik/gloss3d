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
void g3dimportv3subdivider ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;
    G3DFACESCULPTEXTENSION *fse;

    g3dimportv3data_incrementIndentLevel ( gid );

    g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_SUBDIVIDER_SCULPTMAP_GEOMETRY : {
                G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) gid->currentObject;

                g3dimportv3_fread ( fse->pos, 
                                    sizeof ( G3DVECTOR ), 
                                    fse->nbver,
                                    fsrc );
            } break;

            case SIG_OBJECT_SUBDIVIDER_SCULPTMAP_FACEID : {
                G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) gid->currentObject;
                uint32_t faceID;
                G3DFACE *fac;

                g3dimportv3_freadl ( &faceID, fsrc );

                fac = gid->currentFaceArray[faceID];

                fse = g3dfacesculptextension_new ( ( uint64_t ) sdr,
                                                                fac,
                                                                sdr->sculptResolution );
                g3dface_addExtension ( fac, fse );
            } break;

            case SIG_OBJECT_SUBDIVIDER_SCULPTMAP_ENTRY : {
            } break;

            case SIG_OBJECT_SUBDIVIDER_SCULPTMAPS_RESOLUTION : {
                G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) gid->currentObject;

                g3dimportv3_freadl ( &sdr->sculptResolution, fsrc );
            } break;

            case SIG_OBJECT_SUBDIVIDER_SCULPTMAPS : {

            } break;

            case SIG_OBJECT_SUBDIVIDER_SYNC : {
                G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) gid->currentObject;
                uint32_t sync;

                g3dimportv3_freadl ( &sync, fsrc );

                if ( sync ) ((G3DOBJECT*)sdr)->flags |=   SYNCLEVELS;
                else        ((G3DOBJECT*)sdr)->flags &= (~SYNCLEVELS);
            } break;

            case SIG_OBJECT_SUBDIVIDER_LEVEL : {
                G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) gid->currentObject;

                g3dimportv3_freadl ( &sdr->subdiv_preview, fsrc );
                g3dimportv3_freadl ( &sdr->subdiv_render , fsrc );
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
