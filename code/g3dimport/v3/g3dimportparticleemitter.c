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
void g3dimportv3particleemitter ( G3DIMPORTV3DATA *gid, 
                                  uint32_t         chunkEnd, 
                                  FILE            *fsrc ) {
    uint32_t chunkSignature, chunkSize;
    G3DSKIN *skn = NULL;

    g3dimportv3data_incrementIndentLevel ( gid );

    g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_PARTICLEEMITTER_STARTAT : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) gid->currentObject;

                g3dimportv3_freadl ( &pem->startAtFrame, fsrc );
            } break;

            case SIG_OBJECT_PARTICLEEMITTER_ENDAT : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) gid->currentObject;

                g3dimportv3_freadl ( &pem->endAtFrame, fsrc );
            } break;

            case SIG_OBJECT_PARTICLEEMITTER_TYPE : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) gid->currentObject;

                g3dimportv3_freadl ( &pem->type, fsrc );
            } break;

            case SIG_OBJECT_PARTICLEEMITTER_PERFRAME : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) gid->currentObject;

                g3dimportv3_freadf ( &pem->particlesPerFrame, fsrc );

                g3dparticleemitter_reset ( pem );
            } break;

            case SIG_OBJECT_PARTICLEEMITTER_LIFETIME : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) gid->currentObject;

                g3dimportv3_freadl ( &pem->particleLifetime, fsrc );

                g3dparticleemitter_reset ( pem );
            } break;

            case SIG_OBJECT_PARTICLEEMITTER_RADIUS : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) gid->currentObject;

                g3dimportv3_freadf ( &pem->radius, fsrc );
            } break;

            case SIG_OBJECT_PARTICLEEMITTER_DISPLAYPARTICLES : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) gid->currentObject;
                uint32_t displayPart;

                g3dimportv3_freadl ( &displayPart, fsrc );

                if ( displayPart ) pem->obj.flags |= DISPLAYPARTICLES;
            } break;

            /*******************************************************/

            case SIG_OBJECT_PARTICLEEMITTER_INITIAL_ACCELERATION : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) gid->currentObject;

                g3dimportv3_freadf ( &pem->initialAccel.x, fsrc );
                g3dimportv3_freadf ( &pem->initialAccel.y, fsrc );
                g3dimportv3_freadf ( &pem->initialAccel.z, fsrc );
            } break;

            case SIG_OBJECT_PARTICLEEMITTER_INITIAL_SPEED : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) gid->currentObject;

                g3dimportv3_freadf ( &pem->initialSpeed.x, fsrc );
                g3dimportv3_freadf ( &pem->initialSpeed.y, fsrc );
                g3dimportv3_freadf ( &pem->initialSpeed.z, fsrc );
            } break;

            case SIG_OBJECT_PARTICLEEMITTER_INITIAL_SCALING : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) gid->currentObject;

                g3dimportv3_freadf ( &pem->initialScaling.x, fsrc );
                g3dimportv3_freadf ( &pem->initialScaling.y, fsrc );
                g3dimportv3_freadf ( &pem->initialScaling.z, fsrc );
            } break;

            case SIG_OBJECT_PARTICLEEMITTER_INITIAL_ROTATION : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) gid->currentObject;

                g3dimportv3_freadf ( &pem->initialRotation.x, fsrc );
                g3dimportv3_freadf ( &pem->initialRotation.y, fsrc );
                g3dimportv3_freadf ( &pem->initialRotation.z, fsrc );
            } break;

            case SIG_OBJECT_PARTICLEEMITTER_INITIAL_TRANSPARENCY : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) gid->currentObject;

                g3dimportv3_freadf ( &pem->initialTransparency, fsrc );
            } break;

            /*******************************************************/

            case SIG_OBJECT_PARTICLEEMITTER_FINAL_ACCELERATION : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) gid->currentObject;

                g3dimportv3_freadf ( &pem->finalAccel.x, fsrc );
                g3dimportv3_freadf ( &pem->finalAccel.y, fsrc );
                g3dimportv3_freadf ( &pem->finalAccel.z, fsrc );
            } break;

            case SIG_OBJECT_PARTICLEEMITTER_FINAL_SPEED : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) gid->currentObject;

                g3dimportv3_freadf ( &pem->finalSpeed.x, fsrc );
                g3dimportv3_freadf ( &pem->finalSpeed.y, fsrc );
                g3dimportv3_freadf ( &pem->finalSpeed.z, fsrc );
            } break;

            case SIG_OBJECT_PARTICLEEMITTER_FINAL_SCALING : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) gid->currentObject;

                g3dimportv3_freadf ( &pem->finalScaling.x, fsrc );
                g3dimportv3_freadf ( &pem->finalScaling.y, fsrc );
                g3dimportv3_freadf ( &pem->finalScaling.z, fsrc );
            } break;

            case SIG_OBJECT_PARTICLEEMITTER_FINAL_ROTATION : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) gid->currentObject;

                g3dimportv3_freadf ( &pem->finalRotation.x, fsrc );
                g3dimportv3_freadf ( &pem->finalRotation.y, fsrc );
                g3dimportv3_freadf ( &pem->finalRotation.z, fsrc );
            } break;

            case SIG_OBJECT_PARTICLEEMITTER_FINAL_TRANSPARENCY : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) gid->currentObject;

                g3dimportv3_freadf ( &pem->finalTransparency, fsrc );
            } break;

            /*******************************************************/

            case SIG_OBJECT_PARTICLEEMITTER_INITIALVAR_ACCELERATION : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) gid->currentObject;

                g3dimportv3_freadf ( &pem->initialVarAccel.x, fsrc );
                g3dimportv3_freadf ( &pem->initialVarAccel.y, fsrc );
                g3dimportv3_freadf ( &pem->initialVarAccel.z, fsrc );
            } break;

            case SIG_OBJECT_PARTICLEEMITTER_INITIALVAR_SPEED : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) gid->currentObject;

                g3dimportv3_freadf ( &pem->initialVarSpeed.x, fsrc );
                g3dimportv3_freadf ( &pem->initialVarSpeed.y, fsrc );
                g3dimportv3_freadf ( &pem->initialVarSpeed.z, fsrc );
            } break;

            case SIG_OBJECT_PARTICLEEMITTER_INITIALVAR_SCALING : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) gid->currentObject;

                g3dimportv3_freadf ( &pem->initialVarScaling.x, fsrc );
                g3dimportv3_freadf ( &pem->initialVarScaling.y, fsrc );
                g3dimportv3_freadf ( &pem->initialVarScaling.z, fsrc );
            } break;

            case SIG_OBJECT_PARTICLEEMITTER_INITIALVAR_ROTATION : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) gid->currentObject;

                g3dimportv3_freadf ( &pem->initialVarRotation.x, fsrc );
                g3dimportv3_freadf ( &pem->initialVarRotation.y, fsrc );
                g3dimportv3_freadf ( &pem->initialVarRotation.z, fsrc );
            } break;

            case SIG_OBJECT_PARTICLEEMITTER_INITIALVAR_TRANSPARENCY : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) gid->currentObject;

                g3dimportv3_freadf ( &pem->initialVarTransparency, fsrc );
            } break;

            /*******************************************************/

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
