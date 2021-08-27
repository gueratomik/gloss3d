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
void g3dimportv2spline ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;

    g3dimportv2data_incrementIndentLevel ( gid );

    g3dimportv2_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv2_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_SPLINE_GEOMETRY : {
            } break;

            case SIG_OBJECT_SPLINE_GEOMETRY_POINTS : {
                G3DSPLINE *spl = ( G3DSPLINE * ) gid->currentObject;
                uint32_t nbpt;
                uint32_t i;

                g3dimportv2_freadl ( &nbpt, fsrc );

                gid->currentPointArray = ( G3DCURVEPOINT ** ) realloc ( gid->currentPointArray, nbpt * sizeof ( G3DCURVEPOINT * ) );

                for ( i = 0x00; i < nbpt; i++ ) {
                    uint32_t ptFlags;
                    float x, y, z;

                    g3dimportv2_freadf ( &x, fsrc );
                    g3dimportv2_freadf ( &y, fsrc );
                    g3dimportv2_freadf ( &z, fsrc );
                    g3dimportv2_freadl ( &ptFlags, fsrc );

                    gid->currentPointArray[i] = g3dcurvepoint_new ( x, y, z );

                    g3dcurve_addPoint ( spl->curve, gid->currentPointArray[i] );
                }
            } break;

            case SIG_OBJECT_SPLINE_GEOMETRY_SEGMENTS : {
                G3DSPLINE *spl = ( G3DSPLINE * ) gid->currentObject;
                uint32_t nbseg;
                uint32_t i;

                g3dimportv2_freadl ( &nbseg, fsrc );

                for ( i = 0x00; i < nbseg; i++ ) {
                    float posHandle[0x02][0x03];
                    G3DCUBICSEGMENT *seg;
                    uint32_t pid[0x02];

                    g3dimportv2_freadl ( &pid[0x00], fsrc );
                    g3dimportv2_freadl ( &pid[0x01], fsrc );

                    g3dimportv2_freadf ( &posHandle[0x00][0x00], fsrc );
                    g3dimportv2_freadf ( &posHandle[0x00][0x01], fsrc );
                    g3dimportv2_freadf ( &posHandle[0x00][0x02], fsrc );

                    g3dimportv2_freadf ( &posHandle[0x01][0x00], fsrc );
                    g3dimportv2_freadf ( &posHandle[0x01][0x01], fsrc );
                    g3dimportv2_freadf ( &posHandle[0x01][0x02], fsrc );

                    seg = g3dcubicsegment_new ( gid->currentPointArray[pid[0x00]],
                                                gid->currentPointArray[pid[0x01]],
                                                posHandle[0x00][0x00],
                                                posHandle[0x00][0x01],
                                                posHandle[0x00][0x02],
                                                posHandle[0x01][0x00],
                                                posHandle[0x01][0x01],
                                                posHandle[0x01][0x02] );

                    g3dcurve_addSegment ( spl->curve, seg );
                }
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
