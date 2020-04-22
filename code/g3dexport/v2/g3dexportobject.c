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
#include <g3dexportv2.h>

/******************************************************************************/
static uint32_t g3dexportobject_keysKeyLoop ( G3DKEY    *key,
                                              uint32_t   flags, 
                                              FILE      *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwrite ( &key->loopFrame, sizeof ( float ), 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportobject_keysKeyTransformation ( G3DKEY  *key, 
                                                        uint32_t flags, 
                                                        FILE     *fdst ) {
    uint32_t usepos = ( key->flags & KEYPOSITION ) ? 0x01 : 0x00;
    uint32_t userot = ( key->flags & KEYROTATION ) ? 0x01 : 0x00;
    uint32_t usesca = ( key->flags & KEYSCALING  ) ? 0x01 : 0x00;
    uint32_t size = 0x00;

    size += g3dexport_fwrite ( &key->frame, sizeof ( float    ), 0x01, fdst );

    size += g3dexport_fwrite ( &key->pos.x, sizeof ( float    ), 0x01, fdst );
    size += g3dexport_fwrite ( &key->pos.y, sizeof ( float    ), 0x01, fdst );
    size += g3dexport_fwrite ( &key->pos.z, sizeof ( float    ), 0x01, fdst );
    size += g3dexport_fwrite ( &usepos    , sizeof ( uint32_t ), 0x01, fdst );

    size += g3dexport_fwrite ( &key->rot.x, sizeof ( float    ), 0x01, fdst );
    size += g3dexport_fwrite ( &key->rot.y, sizeof ( float    ), 0x01, fdst );
    size += g3dexport_fwrite ( &key->rot.z, sizeof ( float    ), 0x01, fdst );
    size += g3dexport_fwrite ( &userot    , sizeof ( uint32_t ), 0x01, fdst );

    size += g3dexport_fwrite ( &key->sca.x, sizeof ( float    ), 0x01, fdst );
    size += g3dexport_fwrite ( &key->sca.y, sizeof ( float    ), 0x01, fdst );
    size += g3dexport_fwrite ( &key->sca.z, sizeof ( float    ), 0x01, fdst );
    size += g3dexport_fwrite ( &usesca    , sizeof ( uint32_t ), 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportobject_keysKey ( G3DKEY    *key, 
                                          uint32_t   flags, 
                                          FILE      *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_OBJECT_KEYS_KEY_TRANSFORMATION,
                                   g3dexportobject_keysKeyTransformation,
                                   key,
                                   0xFFFFFFFF,
                                   fdst );

    if ( key->flags & KEYLOOP ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_KEYS_KEY_LOOP,
                                       g3dexportobject_keysKeyLoop,
                                       key,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportobject_keys ( G3DOBJECT *obj, 
                                       uint32_t   flags, 
                                       FILE      *fdst ) {
    LIST *ltmpkey = obj->lkey;
    uint32_t keyID = 0x00;
    uint32_t size = 0x00;

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        key->id = keyID++;

        size += g3dexport_writeChunk ( SIG_OBJECT_KEYS_KEY,
                                       g3dexportobject_keysKey,
                                       key,
                                       0xFFFFFFFF,
                                       fdst );

        ltmpkey = ltmpkey->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportobject_transformation ( G3DOBJECT *obj, 
                                                 uint32_t   flags, 
                                                 FILE      *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwrite ( &obj->pos.x, sizeof ( float ), 0x01, fdst );
    size += g3dexport_fwrite ( &obj->pos.y, sizeof ( float ), 0x01, fdst );
    size += g3dexport_fwrite ( &obj->pos.z, sizeof ( float ), 0x01, fdst );
    size += g3dexport_fwrite ( &obj->pos.w, sizeof ( float ), 0x01, fdst );

    size += g3dexport_fwrite ( &obj->rot.x, sizeof ( float ), 0x01, fdst );
    size += g3dexport_fwrite ( &obj->rot.y, sizeof ( float ), 0x01, fdst );
    size += g3dexport_fwrite ( &obj->rot.z, sizeof ( float ), 0x01, fdst );
    size += g3dexport_fwrite ( &obj->rot.w, sizeof ( float ), 0x01, fdst );

    size += g3dexport_fwrite ( &obj->sca.x, sizeof ( float ), 0x01, fdst );
    size += g3dexport_fwrite ( &obj->sca.y, sizeof ( float ), 0x01, fdst );
    size += g3dexport_fwrite ( &obj->sca.z, sizeof ( float ), 0x01, fdst );
    size += g3dexport_fwrite ( &obj->sca.w, sizeof ( float ), 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportobject_identityActive ( G3DOBJECT *obj, 
                                                 uint32_t   flags, 
                                                 FILE      *fdst ) {
    uint32_t active = ( obj->flags & OBJECTINACTIVE ) ? 0x01 : 0x00;

    return g3dexport_fwrite ( &active, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t g3dexportobject_identityType ( G3DOBJECT *obj, 
                                               uint32_t   flags, 
                                               FILE      *fdst ) {
    return g3dexport_fwrite ( &obj->type, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t g3dexportobject_identityParent ( G3DOBJECT *obj, 
                                                 uint32_t   flags, 
                                                 FILE      *fdst ) {
    return g3dexport_fwrite ( &obj->id, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t g3dexportobject_identityName ( G3DOBJECT *obj, 
                                               uint32_t   flags, 
                                               FILE      *fdst ) {
    return g3dexport_fwrite ( obj->name, strlen ( obj->name ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t g3dexportobject_identity ( G3DOBJECT *obj, 
                                           uint32_t  flags, 
                                           FILE     *fdst ) {
    uint32_t size = 0x00;

    /*** write object name ***/
    size += g3dexport_writeChunk ( SIG_OBJECT_IDENTITY_NAME,
                                   g3dexportobject_identityName,
                                   obj,
                                   0xFFFFFFFF,
                                   fdst );

    /*** write object type ***/
    size += g3dexport_writeChunk ( SIG_OBJECT_IDENTITY_TYPE,
                                   g3dexportobject_identityParent,
                                   obj,
                                   0xFFFFFFFF,
                                   fdst );

    /*** write object parent ID ***/
    size += g3dexport_writeChunk ( SIG_OBJECT_IDENTITY_PARENT,
                                   g3dexportobject_identityParent,
                                   obj,
                                   0xFFFFFFFF,
                                   fdst );

    /*** write object active or not ***/
    size += g3dexport_writeChunk ( SIG_OBJECT_IDENTITY_ACTIVE,
                                   g3dexportobject_identityActive,
                                   obj,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}


/******************************************************************************/
uint32_t g3dexportobject ( G3DOBJECT *obj, uint32_t flags, FILE *fdst ) {
    uint32_t size = 0x00;

    /*** write objet identity ***/
    size += g3dexport_writeChunk ( SIG_OBJECT_IDENTITY,
                                   g3dexportobject_identity,
                                   obj,
                                   0xFFFFFFFF,
                                   fdst );

    /*** write objet matrix transformations ***/
    size += g3dexport_writeChunk ( SIG_OBJECT_TRANSFORMATION,
                                   g3dexportobject_transformation,
                                   obj,
                                   0xFFFFFFFF,
                                   fdst );

    /*** write object's animation keys ***/
    size += g3dexport_writeChunk ( SIG_OBJECT_KEYS,
                                   g3dexportobject_keys,
                                   obj,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}
