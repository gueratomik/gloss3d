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
static uint32_t g3dexportobject_textureRestrict  ( G3DEXPORTDATA  *ged, 
                                                   G3DTEXTURE     *tex, 
                                                   uint32_t        flags, 
                                                   FILE           *fdst ) {
    LIST *ltmpfacgrp = tex->lfacgrp;
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &tex->nbfacgrp, fdst );

    while ( ltmpfacgrp ) {
        G3DFACEGROUP *facgrp = ( G3DFACEGROUP * ) ltmpfacgrp->data;

        size += g3dexport_fwritel ( &facgrp->id, fdst );

        ltmpfacgrp = ltmpfacgrp->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportobject_textureMaterial  ( G3DEXPORTDATA  *ged, 
                                                   G3DTEXTURE     *tex, 
                                                   uint32_t        flags, 
                                                   FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &tex->mat->id, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportobject_textureUVMap  ( G3DEXPORTDATA  *ged, 
                                                G3DTEXTURE     *tex, 
                                                uint32_t        flags, 
                                                FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &((G3DOBJECT*)tex->map)->id, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportobject_textureEntry  ( G3DEXPORTDATA  *ged, 
                                                G3DTEXTURE     *tex, 
                                                uint32_t        flags, 
                                                FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_OBJECT_TEXTURE_MATERIAL,
                                   g3dexportobject_textureMaterial,
                                   ged,
                                   tex,
                                   0xFFFFFFFF,
                                   fdst );

    if ( tex->map ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_TEXTURE_UVMAP,
                                       g3dexportobject_textureUVMap,
                                       ged,
                                       tex,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( tex->flags & TEXTURERESTRICTED ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_TEXTURE_RESTRICT,
                                       g3dexportobject_textureRestrict,
                                       ged,
                                       tex,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportobject_textures  ( G3DEXPORTDATA  *ged, 
                                            G3DMESH        *mes, 
                                            uint32_t        flags, 
                                            FILE           *fdst ) {
    LIST *ltmptex = mes->ltex;
    uint32_t size = 0x00;

    while ( ltmptex ) {
        G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;

        size += g3dexport_writeChunk ( SIG_OBJECT_TEXTURE_ENTRY,
                                       g3dexportobject_textureEntry,
                                       ged,
                                       tex,
                                       0xFFFFFFFF,
                                       fdst );

        ltmptex = ltmptex->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportobject_uvmaps  ( G3DEXPORTDATA  *ged, 
                                          G3DMESH        *mes, 
                                          uint32_t        flags, 
                                          FILE           *fdst ) {
    LIST *ltmpuvmap = mes->luvmap;
    uint32_t size = 0x00;

    while ( ltmpuvmap ) {
        G3DUVMAP *uvmap = ( G3DUVMAP * ) ltmpuvmap->data;

        size += g3dexport_writeChunk ( SIG_OBJECT_UVMAP_ENTRY,
                                       g3dexportuvmap,
                                       ged,
                                       uvmap,
                                       0xFFFFFFFF,
                                       fdst );

        ltmpuvmap = ltmpuvmap->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportobject_keysKeyLoop ( G3DEXPORTDATA *ged, 
                                              G3DKEY        *key,
                                              uint32_t       flags, 
                                              FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &key->loopFrame, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportobject_keysKeyTransformation ( G3DEXPORTDATA *ged, 
                                                        G3DKEY        *key, 
                                                        uint32_t       flags, 
                                                        FILE          *fdst ) {
    uint32_t usepos = ( key->flags & KEYPOSITION ) ? 0x01 : 0x00;
    uint32_t userot = ( key->flags & KEYROTATION ) ? 0x01 : 0x00;
    uint32_t usesca = ( key->flags & KEYSCALING  ) ? 0x01 : 0x00;
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &key->frame, fdst );

    size += g3dexport_fwritef ( &key->pos.x, fdst );
    size += g3dexport_fwritef ( &key->pos.y, fdst );
    size += g3dexport_fwritef ( &key->pos.z, fdst );
    size += g3dexport_fwritel ( &usepos    , fdst );

    size += g3dexport_fwritef ( &key->rot.x, fdst );
    size += g3dexport_fwritef ( &key->rot.y, fdst );
    size += g3dexport_fwritef ( &key->rot.z, fdst );
    size += g3dexport_fwritel ( &userot    , fdst );

    size += g3dexport_fwritef ( &key->sca.x, fdst );
    size += g3dexport_fwritef ( &key->sca.y, fdst );
    size += g3dexport_fwritef ( &key->sca.z, fdst );
    size += g3dexport_fwritel ( &usesca    , fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportobject_keysKey ( G3DEXPORTDATA *ged, 
                                          G3DKEY        *key, 
                                          uint32_t       flags, 
                                          FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_OBJECT_KEY_TRANSFORMATION,
                                   g3dexportobject_keysKeyTransformation,
                                   ged,
                                   key,
                                   0xFFFFFFFF,
                                   fdst );

    if ( key->flags & KEYLOOP ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_KEY_LOOP,
                                       g3dexportobject_keysKeyLoop,
                                       ged,
                                       key,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportobject_keys ( G3DEXPORTDATA *ged, 
                                       G3DOBJECT     *obj, 
                                       uint32_t       flags, 
                                       FILE          *fdst ) {
    LIST *ltmpkey = obj->lkey;
    uint32_t keyID = 0x00;
    uint32_t size = 0x00;

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        key->id = keyID++;

        size += g3dexport_writeChunk ( SIG_OBJECT_KEY_ENTRY,
                                       g3dexportobject_keysKey,
                                       ged,
                                       key,
                                       0xFFFFFFFF,
                                       fdst );

        ltmpkey = ltmpkey->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportobject_transformation ( G3DEXPORTDATA *ged, 
                                                 G3DOBJECT     *obj, 
                                                 uint32_t       flags, 
                                                 FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &obj->pos.x, fdst );
    size += g3dexport_fwritef ( &obj->pos.y, fdst );
    size += g3dexport_fwritef ( &obj->pos.z, fdst );
    size += g3dexport_fwritef ( &obj->pos.w, fdst );

    size += g3dexport_fwritef ( &obj->rot.x, fdst );
    size += g3dexport_fwritef ( &obj->rot.y, fdst );
    size += g3dexport_fwritef ( &obj->rot.z, fdst );
    size += g3dexport_fwritef ( &obj->rot.w, fdst );

    size += g3dexport_fwritef ( &obj->sca.x, fdst );
    size += g3dexport_fwritef ( &obj->sca.y, fdst );
    size += g3dexport_fwritef ( &obj->sca.z, fdst );
    size += g3dexport_fwritef ( &obj->sca.w, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportobject_identityType ( G3DEXPORTDATA *ged, 
                                               G3DOBJECT     *obj, 
                                               uint32_t       flags, 
                                               FILE          *fdst ) {
    uint32_t size = 0x00;

    if ( obj->type == G3DSCENETYPE ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_SCENE,
                                       g3dexportscene,
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( obj->type & PRIMITIVE ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_PRIMITIVE,
                                       g3dexportprimitive,
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( obj->type == G3DMESHTYPE ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_MESH,
                                       g3dexportmesh,
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( obj->type == G3DCAMERATYPE ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_CAMERA,
                                       g3dexportcamera,
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( obj->type == G3DLIGHTTYPE ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_LIGHT,
                                       g3dexportlight,
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( obj->type == G3DBONETYPE ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_BONE,
                                       g3dexportbone,
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( obj->type == G3DSYMMETRYTYPE ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_SYMMETRY,
                                       g3dexportsymmetry,
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( obj->type == G3DTEXTTYPE ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_TEXT,
                                       g3dexporttext,
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( obj->type == G3DSUBDIVIDERTYPE ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_SUBDIVIDER,
                                       g3dexportsubdivider,
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( obj->type == G3DWIREFRAMETYPE ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_WIREFRAME,
                                       g3dexportwireframe,
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( obj->type == G3DSPLINEREVOLVERTYPE ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_SPLINEREVOLVER,
                                       g3dexportsplinerevolver,
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( obj->type == G3DSPLINETYPE ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_SPLINE,
                                       g3dexportspline,
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }



    /*if ( obj->type  & MODIFIER ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_MODIFIER,
                                       g3dexportmodifier,
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }*/

    if ( size == 0x00 ) {
        printf("%s: unsupported Object Type for export\n", obj->name );
        size += g3dexport_writeChunk ( SIG_OBJECT_NULL,
                                       NULL,
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportobject_identityParent ( G3DEXPORTDATA *ged, 
                                                 G3DOBJECT     *obj, 
                                                 uint32_t       flags, 
                                                 FILE          *fdst ) {
    return g3dexport_fwrite ( &obj->parent->id, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t g3dexportobject_identityName ( G3DEXPORTDATA *ged, 
                                               G3DOBJECT     *obj, 
                                               uint32_t       flags, 
                                               FILE          *fdst ) {
    return g3dexport_fwrite ( obj->name, strlen ( obj->name ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t g3dexportobject_identity ( G3DEXPORTDATA *ged, 
                                           G3DOBJECT     *obj, 
                                           uint32_t       flags, 
                                           FILE          *fdst ) {
    uint32_t size = 0x00;

    /*** write object name ***/
    size += g3dexport_writeChunk ( SIG_OBJECT_IDENTITY_NAME,
                                   g3dexportobject_identityName,
                                   ged,
                                   obj,
                                   0xFFFFFFFF,
                                   fdst );

    /*** write object type ***/
    size += g3dexport_writeChunk ( SIG_OBJECT_IDENTITY_TYPE,
                                   g3dexportobject_identityType,
                                   ged,
                                   obj,
                                   0xFFFFFFFF,
                                   fdst );

    if ( obj->parent ) {
        /*** write object parent ID ***/
        size += g3dexport_writeChunk ( SIG_OBJECT_IDENTITY_PARENT,
                                       g3dexportobject_identityParent,
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportobject_active ( G3DEXPORTDATA *ged, 
                                         G3DOBJECT     *obj, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t active = ( obj->flags & OBJECTINACTIVE ) ? 0x00 : 0x01;

    return g3dexport_fwritel ( &active, fdst );
}

/******************************************************************************/
uint32_t g3dexportobject ( G3DEXPORTDATA *ged, 
                           G3DOBJECT     *obj, 
                           uint32_t       flags,
                           FILE          *fdst ) {
    uint32_t size = 0x00;

    /*** ensure unique ID starting from 0, only when we write ***/
    if ( fdst ) {
        obj->id = ged->objectID++;
    }

    /*** write objet identity ***/
    size += g3dexport_writeChunk ( SIG_OBJECT_IDENTITY,
                                   g3dexportobject_identity,
                                   ged,
                                   obj,
                                   0xFFFFFFFF,
                                   fdst );

    /*** write objet matrix transformations ***/
    size += g3dexport_writeChunk ( SIG_OBJECT_TRANSFORMATION,
                                   g3dexportobject_transformation,
                                   ged,
                                   obj,
                                   0xFFFFFFFF,
                                   fdst );

    if ( obj->lkey ) {
        /*** write object's animation keys ***/
        size += g3dexport_writeChunk ( SIG_OBJECT_KEYS,
                                       g3dexportobject_keys,
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }

    /*** This concerns primitive as well and maybe other objects in ***/
    /*** future developments so this goes here and not in g3dexportmesh() ***/
    if ( obj->type & MESH ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        if ( mes->luvmap ) {
            size += g3dexport_writeChunk ( SIG_OBJECT_UVMAPS,
                                           g3dexportobject_uvmaps,
                                           ged,
                                           mes,
                                           0xFFFFFFFF,
                                           fdst );
        }

        if ( mes->ltex ) {
            size += g3dexport_writeChunk ( SIG_OBJECT_TEXTURES,
                                           g3dexportobject_textures,
                                           ged,
                                           mes,
                                           0xFFFFFFFF,
                                           fdst );
        }
    }

    /*** write object active or not ***/
    size += g3dexport_writeChunk ( SIG_OBJECT_ACTIVE,
                                   g3dexportobject_active,
                                   ged,
                                   obj,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}
