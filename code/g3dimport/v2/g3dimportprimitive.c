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
void g3dimportv2primitive ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;

    g3dimportv2data_incrementIndentLevel ( gid );

    g3dimportv2_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv2_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_PRIMITIVE_SPHERE : {
                uint32_t slice, cap;
                float radius;

                g3dimportv2_fread ( &radius, sizeof ( float    ), 0x01, fsrc );
                g3dimportv2_fread ( &slice , sizeof ( uint32_t ), 0x01, fsrc );
                g3dimportv2_fread ( &cap   , sizeof ( uint32_t ), 0x01, fsrc );

                gid->currentObject = g3dsphere_new ( gid->currentObjectID++, 
                                                     gid->currentObjectName, 
                                                     slice, 
                                                     cap, 
                                                     radius );

                g3dobject_addChild ( gid->parentObject, 
                                     gid->currentObject, 
                                     gid->engineFlags );
                
            } break;

            case SIG_OBJECT_PRIMITIVE_CUBE : {
                uint32_t nbx, nby, nbz;
                float radius;

                g3dimportv2_fread ( &radius, sizeof ( float    ), 0x01, fsrc );
                g3dimportv2_fread ( &nbx   , sizeof ( float    ), 0x01, fsrc );
                g3dimportv2_fread ( &nby   , sizeof ( uint32_t ), 0x01, fsrc );
                g3dimportv2_fread ( &nbz   , sizeof ( uint32_t ), 0x01, fsrc );

                gid->currentObject = g3dcube_new ( gid->currentObjectID++, 
                                                   gid->currentObjectName,
                                                   nbx,
                                                   nby,
                                                   nbz,
                                                   radius );

                g3dobject_addChild ( gid->parentObject, 
                                     gid->currentObject, 
                                     gid->engineFlags );
            } break;

            case SIG_OBJECT_PRIMITIVE_TORUS : {
                float extrad, intrad;
                uint32_t slice, cap;
                uint32_t orientation;

                g3dimportv2_fread ( &extrad     , sizeof ( float    ), 0x01, fsrc );
                g3dimportv2_fread ( &intrad     , sizeof ( float    ), 0x01, fsrc );
                g3dimportv2_fread ( &slice      , sizeof ( uint32_t ), 0x01, fsrc );
                g3dimportv2_fread ( &cap        , sizeof ( uint32_t ), 0x01, fsrc );
                g3dimportv2_fread ( &orientation, sizeof ( uint32_t ), 0x01, fsrc );

                gid->currentObject = g3dtorus_new ( gid->currentObjectID++, 
                                                    gid->currentObjectName,
                                                    orientation,
                                                    slice,
                                                    cap,
                                                    extrad,
                                                    intrad );

                g3dobject_addChild ( gid->parentObject, 
                                     gid->currentObject, 
                                     gid->engineFlags );
            } break;

            case SIG_OBJECT_PRIMITIVE_CYLINDER : {
                uint32_t closed, orientation;
                uint32_t slice, capx, capy;
                float length, radius;

                g3dimportv2_fread ( &length     , sizeof ( float    ), 0x01, fsrc );
                g3dimportv2_fread ( &radius     , sizeof ( float    ), 0x01, fsrc );
                g3dimportv2_fread ( &slice      , sizeof ( uint32_t ), 0x01, fsrc );
                g3dimportv2_fread ( &capx       , sizeof ( uint32_t ), 0x01, fsrc );
                g3dimportv2_fread ( &capy       , sizeof ( uint32_t ), 0x01, fsrc );
                g3dimportv2_fread ( &closed     , sizeof ( uint32_t ), 0x01, fsrc );
                g3dimportv2_fread ( &orientation, sizeof ( uint32_t ), 0x01, fsrc );

                gid->currentObject = g3dcylinder_new ( gid->currentObjectID++, 
                                                       gid->currentObjectName,
                                                       slice,
                                                       capx,
                                                       capy, 
                                                       radius,
                                                       length );

                g3dobject_addChild ( gid->parentObject, 
                                     gid->currentObject, 
                                     gid->engineFlags );
            } break;

            case SIG_OBJECT_PRIMITIVE_PLANE : {
                uint32_t orientation;
                uint32_t nbu, nbv;
                float radu, radv;

                g3dimportv2_fread ( &radu       , sizeof ( float    ), 0x01, fsrc );
                g3dimportv2_fread ( &radv       , sizeof ( float    ), 0x01, fsrc );
                g3dimportv2_fread ( &nbu        , sizeof ( uint32_t ), 0x01, fsrc );
                g3dimportv2_fread ( &nbv        , sizeof ( uint32_t ), 0x01, fsrc );
                g3dimportv2_fread ( &orientation, sizeof ( uint32_t ), 0x01, fsrc );

                gid->currentObject = g3dplane_new ( gid->currentObjectID++, 
                                                    gid->currentObjectName,
                                                    orientation,
                                                    nbu, 
                                                    nbv, 
                                                    radu, 
                                                    radv );

                g3dobject_addChild ( gid->parentObject, 
                                     gid->currentObject, 
                                     gid->engineFlags );
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
