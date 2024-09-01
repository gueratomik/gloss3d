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
static uint32_t g3dexportv2object_textureRestrict  ( G3DEXPORTV2DATA  *ged, 
                                                   G3DTEXTURE     *tex, 
                                                   uint32_t        flags, 
                                                   FILE           *fdst ) {
    LIST *ltmpfacgrp = tex->lfacgrp;
    uint32_t size = 0x00;

    size += g3dexportv2_fwritel ( &tex->nbfacgrp, fdst );

    while ( ltmpfacgrp ) {
        G3DFACEGROUP *facgrp = ( G3DFACEGROUP * ) ltmpfacgrp->data;

        size += g3dexportv2_fwritel ( &facgrp->id, fdst );

        ltmpfacgrp = ltmpfacgrp->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2object_textureMaterial  ( G3DEXPORTV2DATA  *ged, 
                                                   G3DTEXTURE     *tex, 
                                                   uint32_t        flags, 
                                                   FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_fwritel ( &tex->mat->id, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2object_textureUVMap  ( G3DEXPORTV2DATA  *ged, 
                                                G3DTEXTURE     *tex, 
                                                uint32_t        flags, 
                                                FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_fwritel ( &((G3DOBJECT*)tex->map)->id, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2object_textureEntry  ( G3DEXPORTV2DATA  *ged, 
                                                G3DTEXTURE     *tex, 
                                                uint32_t        flags, 
                                                FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_writeChunk ( SIG_OBJECT_TEXTURE_MATERIAL,
                   EXPORTV2_CALLBACK(g3dexportv2object_textureMaterial),
                                     ged,
                                     tex,
                                     0xFFFFFFFF,
                                     fdst );

    if ( tex->map ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_TEXTURE_UVMAP,
                       EXPORTV2_CALLBACK(g3dexportv2object_textureUVMap),
                                         ged,
                                         tex,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( tex->flags & TEXTURERESTRICTED ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_TEXTURE_RESTRICT,
                       EXPORTV2_CALLBACK(g3dexportv2object_textureRestrict),
                                         ged,
                                         tex,
                                         0xFFFFFFFF,
                                         fdst );
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2object_textures  ( G3DEXPORTV2DATA  *ged, 
                                            G3DMESH        *mes, 
                                            uint32_t        flags, 
                                            FILE           *fdst ) {
    LIST *ltmptex = mes->ltex;
    uint32_t size = 0x00;

    while ( ltmptex ) {
        G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;

        size += g3dexportv2_writeChunk ( SIG_OBJECT_TEXTURE_ENTRY,
                       EXPORTV2_CALLBACK(g3dexportv2object_textureEntry),
                                         ged,
                                         tex,
                                         0xFFFFFFFF,
                                         fdst );

        ltmptex = ltmptex->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2object_uvmaps  ( G3DEXPORTV2DATA  *ged, 
                                          G3DMESH        *mes, 
                                          uint32_t        flags, 
                                          FILE           *fdst ) {
    LIST *ltmpuvmap = mes->luvmap;
    uint32_t size = 0x00;
    uint32_t uvmapID = 0x00;

    while ( ltmpuvmap ) {
        G3DUVMAP *uvmap = ( G3DUVMAP * ) ltmpuvmap->data;

        ((G3DOBJECT*)uvmap)->id = uvmapID++;

        size += g3dexportv2_writeChunk ( SIG_OBJECT_UVMAP_ENTRY,
                       EXPORTV2_CALLBACK(g3dexportv2uvmap),
                                         ged,
                                         uvmap,
                                         0xFFFFFFFF,
                                         fdst );

        ltmpuvmap = ltmpuvmap->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2object_keys ( G3DEXPORTV2DATA *ged, 
                                       G3DOBJECT     *obj, 
                                       uint32_t       flags, 
                                       FILE          *fdst ) {
    LIST *ltmpkey = obj->lkey;
    uint32_t keyID = 0x00;
    uint32_t size = 0x00;

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        key->id = keyID++;

        size += g3dexportv2_writeChunk ( SIG_OBJECT_KEY_ENTRY,
                       EXPORTV2_CALLBACK(g3dexportv2key),
                                         ged,
                                         key,
                                         0xFFFFFFFF,
                                         fdst );

        ltmpkey = ltmpkey->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2object_transformation ( G3DEXPORTV2DATA *ged, 
                                                 G3DOBJECT     *obj, 
                                                 uint32_t       flags, 
                                                 FILE          *fdst ) {
    uint32_t size = 0x00;
    float w = 0.0f;

    size += g3dexportv2_fwritef ( &obj->pos.x, fdst );
    size += g3dexportv2_fwritef ( &obj->pos.y, fdst );
    size += g3dexportv2_fwritef ( &obj->pos.z, fdst );
    size += g3dexportv2_fwritef ( &w         , fdst );

    size += g3dexportv2_fwritef ( &obj->rot.x, fdst );
    size += g3dexportv2_fwritef ( &obj->rot.y, fdst );
    size += g3dexportv2_fwritef ( &obj->rot.z, fdst );
    size += g3dexportv2_fwritef ( &w         , fdst );

    size += g3dexportv2_fwritef ( &obj->sca.x, fdst );
    size += g3dexportv2_fwritef ( &obj->sca.y, fdst );
    size += g3dexportv2_fwritef ( &obj->sca.z, fdst );
    size += g3dexportv2_fwritef ( &w         , fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2object_identityType ( G3DEXPORTV2DATA *ged, 
                                               G3DOBJECT     *obj, 
                                               uint32_t       flags, 
                                               FILE          *fdst ) {
    uint32_t size = 0x00;

    if ( obj->type == G3DSCENETYPE ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_SCENE,
                       EXPORTV2_CALLBACK(g3dexportv2scene),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type & PRIMITIVE ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_PRIMITIVE,
                       EXPORTV2_CALLBACK(g3dexportv2primitive),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DMESHTYPE ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_MESH,
                       EXPORTV2_CALLBACK(g3dexportv2mesh),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DCAMERATYPE ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_CAMERA,
                       EXPORTV2_CALLBACK(g3dexportv2camera),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DLIGHTTYPE ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_LIGHT,
                       EXPORTV2_CALLBACK(g3dexportv2light),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DBONETYPE ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_BONE,
                       EXPORTV2_CALLBACK(g3dexportv2bone),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DSYMMETRYTYPE ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_SYMMETRY,
                       EXPORTV2_CALLBACK(g3dexportv2symmetry),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DTEXTTYPE ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_TEXT,
                       EXPORTV2_CALLBACK(g3dexportv2text),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DSUBDIVIDERTYPE ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_SUBDIVIDER,
                       EXPORTV2_CALLBACK(g3dexportv2subdivider),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DWIREFRAMETYPE ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_WIREFRAME,
                       EXPORTV2_CALLBACK(g3dexportv2wireframe),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DSPLINEREVOLVERTYPE ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_SPLINEREVOLVER,
                       EXPORTV2_CALLBACK(g3dexportv2splinerevolver),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DSPLINETYPE ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_SPLINE,
                       EXPORTV2_CALLBACK(g3dexportv2spline),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DFFDTYPE ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_FFD,
                       EXPORTV2_CALLBACK(g3dexportv2ffd),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DMORPHERTYPE ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_MORPHER,
                       EXPORTV2_CALLBACK(g3dexportv2morpher),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DSKINTYPE ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_SKIN,
                       EXPORTV2_CALLBACK(g3dexportv2skin),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    /*if ( obj->type  & MODIFIER ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_MODIFIER,
                     EXPORTV2_CALLBACK(g3dexportv2modifier),
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }*/

    if ( size == 0x00 ) {
        printf("%s: unsupported Object Type for export\n", obj->name );
        size += g3dexportv2_writeChunk ( SIG_OBJECT_NULL,
                                       NULL,
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2object_identityParent ( G3DEXPORTV2DATA *ged, 
                                                 G3DOBJECT     *obj, 
                                                 uint32_t       flags, 
                                                 FILE          *fdst ) {
    return g3dexportv2_fwrite ( &obj->parent->id, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t g3dexportv2object_identityName ( G3DEXPORTV2DATA *ged, 
                                               G3DOBJECT     *obj, 
                                               uint32_t       flags, 
                                               FILE          *fdst ) {
    return g3dexportv2_fwrite ( obj->name, strlen ( obj->name ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t g3dexportv2object_identity ( G3DEXPORTV2DATA *ged, 
                                           G3DOBJECT     *obj, 
                                           uint32_t       flags, 
                                           FILE          *fdst ) {
    uint32_t size = 0x00;

    /*** write object name ***/
    size += g3dexportv2_writeChunk ( SIG_OBJECT_IDENTITY_NAME,
                   EXPORTV2_CALLBACK(g3dexportv2object_identityName),
                                     ged,
                                     obj,
                                     0xFFFFFFFF,
                                     fdst );

    if ( obj->parent ) {
        /*** write object parent ID ***/
        size += g3dexportv2_writeChunk ( SIG_OBJECT_IDENTITY_PARENT,
                       EXPORTV2_CALLBACK(g3dexportv2object_identityParent),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    /*** write object type ***/
    size += g3dexportv2_writeChunk ( SIG_OBJECT_IDENTITY_TYPE,
                   EXPORTV2_CALLBACK(g3dexportv2object_identityType),
                                     ged,
                                     obj,
                                     0xFFFFFFFF,
                                     fdst );
  
    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2object_active ( G3DEXPORTV2DATA *ged, 
                                         G3DOBJECT     *obj, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t active = ( obj->flags & OBJECTINACTIVE ) ? 0x00 : 0x01;

    return g3dexportv2_fwritel ( &active, fdst );
}

/******************************************************************************/
uint32_t g3dexportv2object ( G3DEXPORTV2DATA *ged, 
                           G3DOBJECT     *obj, 
                           uint32_t       flags,
                           FILE          *fdst ) {
    uint32_t size = 0x00;

    /*** ensure unique ID starting from 0, only when we write ***/
    if ( fdst ) {
        obj->id = ged->objectID++;
    }

    /*** write objet identity ***/
    size += g3dexportv2_writeChunk ( SIG_OBJECT_IDENTITY,
                   EXPORTV2_CALLBACK(g3dexportv2object_identity),
                                     ged,
                                     obj,
                                     0xFFFFFFFF,
                                     fdst );

    /*** write objet matrix transformations ***/
    size += g3dexportv2_writeChunk ( SIG_OBJECT_TRANSFORMATION,
                   EXPORTV2_CALLBACK(g3dexportv2object_transformation),
                                     ged,
                                     obj,
                                     0xFFFFFFFF,
                                     fdst );

    if ( obj->lkey ) {
        /*** write object's animation keys ***/
        size += g3dexportv2_writeChunk ( SIG_OBJECT_KEYS,
                       EXPORTV2_CALLBACK(g3dexportv2object_keys),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    /*** This concerns primitive as well and maybe other objects in ***/
    /*** future developments so this goes here and not in g3dexportv2mesh() ***/
    if ( obj->type & MESH ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        if ( mes->luvmap ) {
            size += g3dexportv2_writeChunk ( SIG_OBJECT_UVMAPS,
                           EXPORTV2_CALLBACK(g3dexportv2object_uvmaps),
                                             ged,
                                             mes,
                                             0xFFFFFFFF,
                                             fdst );
        }

        if ( mes->ltex ) {
            size += g3dexportv2_writeChunk ( SIG_OBJECT_TEXTURES,
                           EXPORTV2_CALLBACK(g3dexportv2object_textures),
                                             ged,
                                             mes,
                                             0xFFFFFFFF,
                                             fdst );
        }
    }

    /*** write object active or not ***/
    size += g3dexportv2_writeChunk ( SIG_OBJECT_ACTIVE,
                   EXPORTV2_CALLBACK(g3dexportv2object_active),
                                     ged,
                                     obj,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}
