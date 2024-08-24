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
void g3dimportv3bone ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;
    G3DSKIN *skn = NULL;

    g3dimportv3data_incrementIndentLevel ( gid );

    g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_BONE_LENGTH : {
                G3DBONE *bon = ( G3DBONE * ) gid->currentObject;

                g3dimportv3_freadf ( &bon->len, fsrc );
            } break;

            case SIG_OBJECT_BONE_RIGS : {
            } break;

            case SIG_OBJECT_BONE_RIG_ENTRY : {
            } break;

            case SIG_OBJECT_BONE_RIG_SKIN : {
                G3DBONE *bon = ( G3DBONE * ) gid->currentObject;
                uint32_t sknID;

                g3dimportv3_freadl ( &sknID, fsrc );

                skn = ( G3DSKIN * ) g3dobject_getChildByID ( ( G3DOBJECT * ) gid->currentScene, 
                                               sknID );
            } break;

            case SIG_OBJECT_BONE_RIG_WEIGHTGROUPS : {
            } break;

            case SIG_OBJECT_BONE_RIG_WEIGHTGROUP_ENTRY : {
                G3DBONE *bon = ( G3DBONE * ) gid->currentObject;
                G3DOBJECT *obj = gid->currentObject;
                G3DOBJECT *child;
                G3DWEIGHTGROUP *grp = NULL;
                G3DMESH *mes = NULL;
                uint32_t grpID;
                uint32_t objID;

                g3dimportv3_freadl ( &grpID, fsrc );
                g3dimportv3_freadl ( &objID, fsrc );

                child = g3dobject_getChildByID ( ( G3DOBJECT * ) gid->currentScene, objID );

                if ( child ) {
                    if ( child->type == G3DMESHTYPE ) {
                        mes = ( G3DMESH * ) child;

                        grp = g3dmesh_getWeightGroupByID ( mes, grpID );

                        if ( grp ) {
                            gid->currentRig = g3dbone_addWeightGroup ( bon, 
                                                                       skn, 
                                                                       grp );
                        }
                    }
                }
            } break;

            case SIG_OBJECT_BONE_RIG_SKINMATRIX : {
                G3DBONE *bon = ( G3DBONE * ) gid->currentObject;
                G3DOBJECT *obj = gid->currentObject;
                G3DRIG *rig = gid->currentRig;
                uint32_t i;

                for ( i = 0x00; i < 0x10; i++ ) {
                    double cell;

                    g3dimportv3_freadd ( &cell, fsrc );

                    rig->isknmatrix[i] = cell;
                }

                obj->flags |= BONEFROMFILE;
            } break;

            case SIG_OBJECT_BONE_FIXING : {
                G3DBONE *bon = ( G3DBONE * ) gid->currentObject;
                G3DOBJECT *obj = gid->currentObject;

                obj->flags |= BONEFIXED;
            } break;

            case SIG_OBJECT_BONE_FIXING_TRANSFORMATION : {
                G3DBONE *bon = ( G3DBONE * ) gid->currentObject;

                g3dimportv3_freadf ( &bon->fixpos.x, fsrc );
                g3dimportv3_freadf ( &bon->fixpos.y, fsrc );
                g3dimportv3_freadf ( &bon->fixpos.z, fsrc );
                g3dimportv3_freadf ( &bon->fixpos.w, fsrc );

                g3dimportv3_freadf ( &bon->fixrot.x, fsrc );
                g3dimportv3_freadf ( &bon->fixrot.y, fsrc );
                g3dimportv3_freadf ( &bon->fixrot.z, fsrc );
                g3dimportv3_freadf ( &bon->fixrot.w, fsrc );

                g3dimportv3_freadf ( &bon->fixsca.x, fsrc );
                g3dimportv3_freadf ( &bon->fixsca.y, fsrc );
                g3dimportv3_freadf ( &bon->fixsca.z, fsrc );
                g3dimportv3_freadf ( &bon->fixsca.w, fsrc );
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
