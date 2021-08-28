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
void g3dimportv3primitive ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;

    g3dimportv3data_incrementIndentLevel ( gid );

    g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_PRIMITIVE_SPHERE : {
                uint32_t slice, cap;
                float radius;

                g3dimportv3_freadf ( &radius, fsrc );
                g3dimportv3_freadl ( &slice , fsrc );
                g3dimportv3_freadl ( &cap   , fsrc );

                g3dsphere_build ( gid->currentObject,  
                                  slice, 
                                  cap, 
                                  radius );
                
            } break;

            case SIG_OBJECT_PRIMITIVE_CUBE : {
                uint32_t nbx, nby, nbz;
                float radius;

                g3dimportv3_freadf ( &radius, fsrc );
                g3dimportv3_freadf ( &nbx   , fsrc );
                g3dimportv3_freadl ( &nby   , fsrc );
                g3dimportv3_freadl ( &nbz   , fsrc );

                g3dcube_build ( gid->currentObject,
                                nbx,
                                nby,
                                nbz,
                                radius );
            } break;

            case SIG_OBJECT_PRIMITIVE_TORUS : {
                float extrad, intrad;
                uint32_t slice, cap;
                uint32_t orientation;

                g3dimportv3_freadf ( &extrad     , fsrc );
                g3dimportv3_freadf ( &intrad     , fsrc );
                g3dimportv3_freadl ( &slice      , fsrc );
                g3dimportv3_freadl ( &cap        , fsrc );
                g3dimportv3_freadl ( &orientation, fsrc );

                g3dtorus_build ( gid->currentObject,
                                 orientation,
                                 slice,
                                 cap,
                                 extrad,
                                 intrad );
            } break;

            case SIG_OBJECT_PRIMITIVE_CYLINDER : {
                uint32_t closed, orientation;
                uint32_t slice, capx, capy;
                float length, radius;

                g3dimportv3_freadf ( &length     , fsrc );
                g3dimportv3_freadf ( &radius     , fsrc );
                g3dimportv3_freadl ( &slice      , fsrc );
                g3dimportv3_freadl ( &capx       , fsrc );
                g3dimportv3_freadl ( &capy       , fsrc );
                g3dimportv3_freadl ( &closed     , fsrc );
                g3dimportv3_freadl ( &orientation, fsrc );

                g3dcylinder_build ( gid->currentObject,
                                    slice,
                                    capx,
                                    capy, 
                                    radius,
                                    length );
            } break;

            case SIG_OBJECT_PRIMITIVE_TUBE : {
                uint32_t slice, capx, capy, orientation;
                float length, radius, thickness;

                g3dimportv3_freadf ( &length     , fsrc );
                g3dimportv3_freadf ( &radius     , fsrc );
                g3dimportv3_freadl ( &slice      , fsrc );
                g3dimportv3_freadl ( &capx       , fsrc );
                g3dimportv3_freadl ( &capy       , fsrc );
                g3dimportv3_freadf ( &thickness  , fsrc );
                g3dimportv3_freadl ( &orientation, fsrc );

                g3dtube_build ( gid->currentObject,
                                slice,
                                capx,
                                capy, 
                                radius,
                                thickness,
                                length );
            } break;

            case SIG_OBJECT_PRIMITIVE_PLANE : {
                uint32_t orientation;
                uint32_t nbu, nbv;
                float radu, radv;

                g3dimportv3_freadf ( &radu       , fsrc );
                g3dimportv3_freadf ( &radv       , fsrc );
                g3dimportv3_freadl ( &nbu        , fsrc );
                g3dimportv3_freadl ( &nbv        , fsrc );
                g3dimportv3_freadl ( &orientation, fsrc );

                g3dplane_build ( gid->currentObject,
                                 orientation,
                                 nbu, 
                                 nbv, 
                                 radu, 
                                 radv );
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
