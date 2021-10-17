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
void g3dimportv3path ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;
    G3DCURVE *curve;

    g3dimportv3data_incrementIndentLevel ( gid );

    g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_PATH_POSITION : {
                curve = gid->currentObject->curve[0x00];
            } break;

            case SIG_OBJECT_PATH_ROTATION : {
                curve = gid->currentObject->curve[0x01];
            } break;

            case SIG_OBJECT_PATH_SCALING : {
                curve = gid->currentObject->curve[0x02];
            } break;

            case SIG_OBJECT_PATH_SEGMENT : {
                float posHandle[0x02][0x03];
                G3DCURVESEGMENT *seg;
                uint32_t pid[0x02];

                g3dimportv3_freadl ( &pid[0x00], fsrc );
                g3dimportv3_freadl ( &pid[0x01], fsrc );

                g3dimportv3_freadf ( &posHandle[0x00][0x00], fsrc );
                g3dimportv3_freadf ( &posHandle[0x00][0x01], fsrc );
                g3dimportv3_freadf ( &posHandle[0x00][0x02], fsrc );

                g3dimportv3_freadf ( &posHandle[0x01][0x00], fsrc );
                g3dimportv3_freadf ( &posHandle[0x01][0x01], fsrc );
                g3dimportv3_freadf ( &posHandle[0x01][0x02], fsrc );

                seg = g3dcurve_seekSegmentByID ( curve, pid[0x00], pid[0x01] );

                seg->pt[P0HANDLEIDX]->pos.x  = posHandle[0x00][0x00];
                seg->pt[P0HANDLEIDX]->pos.y  = posHandle[0x00][0x01];
                seg->pt[P0HANDLEIDX]->pos.z  = posHandle[0x00][0x02];

                seg->pt[P1HANDLEIDX]->pos.x  = posHandle[0x01][0x00];
                seg->pt[P1HANDLEIDX]->pos.y  = posHandle[0x01][0x01];
                seg->pt[P1HANDLEIDX]->pos.z  = posHandle[0x01][0x02];
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
