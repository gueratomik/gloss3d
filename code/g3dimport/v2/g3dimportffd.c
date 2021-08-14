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
void g3dimportffd ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;

    g3dimportdata_incrementIndentLevel ( gid );

    g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_FFD_GEOMETRY : {
                G3DFFD *ffd = ( G3DFFD * ) gid->currentObject;
                uint32_t nbx, nby, nbz;
                float radx, rady, radz;

                g3dimport_freadl ( &nbx, fsrc );
                g3dimport_freadl ( &nby, fsrc );
                g3dimport_freadl ( &nbz, fsrc );

                g3dimport_freadf ( &radx, fsrc );
                g3dimport_freadf ( &rady, fsrc );
                g3dimport_freadf ( &radz, fsrc );

                g3dffd_shape ( ffd, nbx, nby, nbz, radx, rady, radz );
            } break;

            case SIG_OBJECT_FFD_UVW : {
                G3DFFD *ffd = ( G3DFFD * ) gid->currentObject;
                uint32_t nbpnt = ((G3DMESH*)ffd)->nbver;
                uint32_t i;

                for ( i = 0x00; i < nbpnt; i++ ) {
                    G3DVERTEX *pnt = &ffd->pnt[i];

                    g3dimport_freadf ( &pnt->pos.x, fsrc );
                    g3dimport_freadf ( &pnt->pos.y, fsrc );
                    g3dimport_freadf ( &pnt->pos.z, fsrc );
                    /*** normal vector stores the local UVW coordinates ***/
                    g3dimport_freadf ( &pnt->nor.x, fsrc );
                    g3dimport_freadf ( &pnt->nor.y, fsrc );
                    g3dimport_freadf ( &pnt->nor.z, fsrc );
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
