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
#include <g3dexportv3.h>

/******************************************************************************/
static uint32_t g3dexportv3object_pathSegment ( G3DEXPORTV3DATA *ged, 
                                                G3DCURVESEGMENT *seg, 
                                                uint32_t         flags, 
                                                FILE            *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &seg->pt[P0IDX]->id, fdst );
    size += g3dexportv3_fwritel ( &seg->pt[P1IDX]->id, fdst );

    size += g3dexportv3_fwritef ( &seg->pt[P0HANDLEIDX]->pos.x, fdst );
    size += g3dexportv3_fwritef ( &seg->pt[P0HANDLEIDX]->pos.y, fdst );
    size += g3dexportv3_fwritef ( &seg->pt[P0HANDLEIDX]->pos.z, fdst );

    size += g3dexportv3_fwritef ( &seg->pt[P1HANDLEIDX]->pos.x, fdst );
    size += g3dexportv3_fwritef ( &seg->pt[P1HANDLEIDX]->pos.y, fdst );
    size += g3dexportv3_fwritef ( &seg->pt[P1HANDLEIDX]->pos.z, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3object_path ( G3DEXPORTV3DATA *ged, 
                                         G3DCURVE        *curve, 
                                         uint32_t         flags, 
                                         FILE            *fdst ) {
    LIST *ltmpseg = curve->lseg;
    uint32_t size = 0x00;

    while ( ltmpseg ) {
        G3DCURVESEGMENT *seg = ( G3DCURVESEGMENT * ) ltmpseg->data;

        size += g3dexportv3_writeChunk ( SIG_OBJECT_PATH_SEGMENT,
                       EXPORTV3_CALLBACK(g3dexportv3object_pathSegment),
                                         ged,
                                         seg,
                                         0xFFFFFFFF,
                                         fdst );

        ltmpseg = ltmpseg->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3object_paths ( G3DEXPORTV3DATA *ged, 
                                               G3DOBJECT       *obj, 
                                               uint32_t         flags, 
                                               FILE            *fdst ) {
    uint32_t size = 0x00;

    if ( obj->curve[0x00]->lseg ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_PATH_POSITION,
                       EXPORTV3_CALLBACK(g3dexportv3object_path),
                                         ged,
                                         obj->curve[0x00],
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->curve[0x01]->lseg ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_PATH_ROTATION,
                       EXPORTV3_CALLBACK(g3dexportv3object_path),
                                         ged,
                                         obj->curve[0x01],
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->curve[0x02]->lseg ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_PATH_SCALING,
                       EXPORTV3_CALLBACK(g3dexportv3object_path),
                                         ged,
                                         obj->curve[0x02],
                                         0xFFFFFFFF,
                                         fdst );
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3object_textureRestrict  ( G3DEXPORTV3DATA  *ged, 
                                                   G3DTEXTURE     *tex, 
                                                   uint32_t        flags, 
                                                   FILE           *fdst ) {
    LIST *ltmpfacgrp = tex->lfacgrp;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &tex->nbfacgrp, fdst );

    while ( ltmpfacgrp ) {
        G3DFACEGROUP *facgrp = ( G3DFACEGROUP * ) ltmpfacgrp->data;

        size += g3dexportv3_fwritel ( &facgrp->id, fdst );

        ltmpfacgrp = ltmpfacgrp->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3object_textureMaterial  ( G3DEXPORTV3DATA  *ged, 
                                                   G3DTEXTURE     *tex, 
                                                   uint32_t        flags, 
                                                   FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &tex->mat->id, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3object_textureUVMap  ( G3DEXPORTV3DATA  *ged, 
                                                G3DTEXTURE     *tex, 
                                                uint32_t        flags, 
                                                FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &((G3DOBJECT*)tex->map)->id, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3object_textureEntry  ( G3DEXPORTV3DATA  *ged, 
                                                G3DTEXTURE     *tex, 
                                                uint32_t        flags, 
                                                FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_TEXTURE_MATERIAL,
                   EXPORTV3_CALLBACK(g3dexportv3object_textureMaterial),
                                     ged,
                                     tex,
                                     0xFFFFFFFF,
                                     fdst );

    if ( tex->map ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_TEXTURE_UVMAP,
                       EXPORTV3_CALLBACK(g3dexportv3object_textureUVMap),
                                         ged,
                                         tex,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( tex->flags & TEXTURERESTRICTED ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_TEXTURE_RESTRICT,
                       EXPORTV3_CALLBACK(g3dexportv3object_textureRestrict),
                                         ged,
                                         tex,
                                         0xFFFFFFFF,
                                         fdst );
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3object_textures  ( G3DEXPORTV3DATA  *ged, 
                                            G3DMESH        *mes, 
                                            uint32_t        flags, 
                                            FILE           *fdst ) {
    LIST *ltmptex = mes->ltex;
    uint32_t size = 0x00;

    while ( ltmptex ) {
        G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;

        size += g3dexportv3_writeChunk ( SIG_OBJECT_TEXTURE_ENTRY,
                       EXPORTV3_CALLBACK(g3dexportv3object_textureEntry),
                                         ged,
                                         tex,
                                         0xFFFFFFFF,
                                         fdst );

        ltmptex = ltmptex->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3object_uvmaps  ( G3DEXPORTV3DATA  *ged, 
                                          G3DMESH        *mes, 
                                          uint32_t        flags, 
                                          FILE           *fdst ) {
    LIST *ltmpuvmap = mes->luvmap;
    uint32_t size = 0x00;
    uint32_t uvmapID = 0x00;

    while ( ltmpuvmap ) {
        G3DUVMAP *uvmap = ( G3DUVMAP * ) ltmpuvmap->data;

        ((G3DOBJECT*)uvmap)->id = uvmapID++;

        size += g3dexportv3_writeChunk ( SIG_OBJECT_UVMAP_ENTRY,
                       EXPORTV3_CALLBACK(g3dexportv3uvmap),
                                         ged,
                                         uvmap,
                                         0xFFFFFFFF,
                                         fdst );

        ltmpuvmap = ltmpuvmap->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3object_keys ( G3DEXPORTV3DATA *ged, 
                                       G3DOBJECT     *obj, 
                                       uint32_t       flags, 
                                       FILE          *fdst ) {
    LIST *ltmpkey = obj->lkey;
    uint32_t keyID = 0x00;
    uint32_t size = 0x00;

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        key->id = keyID++;

        /*** used by path segments ***/
        key->curvePoint[0x00].id = key->id;
        key->curvePoint[0x01].id = key->id;
        key->curvePoint[0x02].id = key->id;

        size += g3dexportv3_writeChunk ( SIG_OBJECT_KEY_ENTRY,
                       EXPORTV3_CALLBACK(g3dexportv3key),
                                         ged,
                                         key,
                                         0xFFFFFFFF,
                                         fdst );

        ltmpkey = ltmpkey->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3object_tags ( G3DEXPORTV3DATA *ged, 
                                         G3DOBJECT     *obj, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    LIST *ltmptag = obj->ltag;
    uint32_t keyID = 0x00;
    uint32_t size = 0x00;

    while ( ltmptag ) {
        G3DTAG *tag = ( G3DTAG * ) ltmptag->data;

        size += g3dexportv3_writeChunk ( SIG_OBJECT_TAG_ENTRY,
                       EXPORTV3_CALLBACK(g3dexportv3tag),
                                         ged,
                                         tag,
                                         0xFFFFFFFF,
                                         fdst );

        ltmptag = ltmptag->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3object_transformation ( G3DEXPORTV3DATA *ged, 
                                                 G3DOBJECT     *obj, 
                                                 uint32_t       flags, 
                                                 FILE          *fdst ) {
    uint32_t size = 0x00;
    float w = 0.0f;

    size += g3dexportv3_fwritef ( &obj->pos.x, fdst );
    size += g3dexportv3_fwritef ( &obj->pos.y, fdst );
    size += g3dexportv3_fwritef ( &obj->pos.z, fdst );
    size += g3dexportv3_fwritef ( &w         , fdst );

    size += g3dexportv3_fwritef ( &obj->rot.x, fdst );
    size += g3dexportv3_fwritef ( &obj->rot.y, fdst );
    size += g3dexportv3_fwritef ( &obj->rot.z, fdst );
    size += g3dexportv3_fwritef ( &w         , fdst );

    size += g3dexportv3_fwritef ( &obj->sca.x, fdst );
    size += g3dexportv3_fwritef ( &obj->sca.y, fdst );
    size += g3dexportv3_fwritef ( &obj->sca.z, fdst );
    size += g3dexportv3_fwritef ( &w         , fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3object_identityType ( G3DEXPORTV3DATA *ged, 
                                               G3DOBJECT     *obj, 
                                               uint32_t       flags, 
                                               FILE          *fdst ) {
    uint32_t size = 0x00;

    if ( obj->type == G3DSCENETYPE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_SCENE,
                       EXPORTV3_CALLBACK(g3dexportv3scene),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type & PRIMITIVE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_PRIMITIVE,
                       EXPORTV3_CALLBACK(g3dexportv3primitive),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DMESHTYPE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH,
                       EXPORTV3_CALLBACK(g3dexportv3mesh),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DCAMERATYPE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_CAMERA,
                       EXPORTV3_CALLBACK(g3dexportv3camera),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DLIGHTTYPE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_LIGHT,
                       EXPORTV3_CALLBACK(g3dexportv3light),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DBONETYPE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_BONE,
                       EXPORTV3_CALLBACK(g3dexportv3bone),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DSYMMETRYTYPE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_SYMMETRY,
                       EXPORTV3_CALLBACK(g3dexportv3symmetry),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DTEXTTYPE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_TEXT,
                       EXPORTV3_CALLBACK(g3dexportv3text),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DSUBDIVIDERTYPE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_SUBDIVIDER,
                       EXPORTV3_CALLBACK(g3dexportv3subdivider),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DWIREFRAMETYPE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_WIREFRAME,
                       EXPORTV3_CALLBACK(g3dexportv3wireframe),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DSPLINEREVOLVERTYPE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_SPLINEREVOLVER,
                       EXPORTV3_CALLBACK(g3dexportv3splinerevolver),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DSPLINETYPE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_SPLINE,
                       EXPORTV3_CALLBACK(g3dexportv3spline),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DFFDTYPE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_FFD,
                       EXPORTV3_CALLBACK(g3dexportv3ffd),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DMORPHERTYPE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_MORPHER,
                       EXPORTV3_CALLBACK(g3dexportv3morpher),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DSKINTYPE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_SKIN,
                       EXPORTV3_CALLBACK(g3dexportv3skin),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DINSTANCETYPE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_INSTANCE,
                       EXPORTV3_CALLBACK(g3dexportv3instance),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DPARTICLEEMITTERTYPE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER,
                       EXPORTV3_CALLBACK(g3dexportv3particleemitter),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    /*if ( obj->type  & MODIFIER ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_MODIFIER,
                     EXPORTV3_CALLBACK(g3dexportv3modifier),
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }*/

    if ( size == 0x00 ) {
        printf("%s: unsupported Object Type for export\n", obj->name );
        size += g3dexportv3_writeChunk ( SIG_OBJECT_NULL,
                     EXPORTV3_CALLBACK(NULL),
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3object_identityParent ( G3DEXPORTV3DATA *ged, 
                                                 G3DOBJECT     *obj, 
                                                 uint32_t       flags, 
                                                 FILE          *fdst ) {
    return g3dexportv3_fwrite ( &obj->parent->id, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t g3dexportv3object_identityName ( G3DEXPORTV3DATA *ged, 
                                               G3DOBJECT     *obj, 
                                               uint32_t       flags, 
                                               FILE          *fdst ) {
    return g3dexportv3_fwrite ( obj->name, strlen ( obj->name ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t g3dexportv3object_identityID ( G3DEXPORTV3DATA *ged, 
                                               G3DOBJECT       *obj, 
                                               uint32_t         flags, 
                                               FILE            *fdst ) {
    return g3dexportv3_fwrite ( &obj->id, sizeof ( uint32_t ), 0x01, fdst );
}


/******************************************************************************/
static uint32_t g3dexportv3object_identity ( G3DEXPORTV3DATA *ged, 
                                           G3DOBJECT     *obj, 
                                           uint32_t       flags, 
                                           FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_IDENTITY_ID,
                   EXPORTV3_CALLBACK(g3dexportv3object_identityID),
                                     ged,
                                     obj,
                                     0xFFFFFFFF,
                                     fdst );

    /** commented out: now in the OBJECT DECLARE section ***/
    /*size += g3dexportv3_writeChunk ( SIG_OBJECT_IDENTITY_NAME,
                     EXPORTV3_CALLBACK(g3dexportv3object_identityName),
                                     ged,
                                     obj,
                                     0xFFFFFFFF,
                                     fdst );*/

    if ( obj->parent ) {
        /*** write object parent ID ***/
        size += g3dexportv3_writeChunk ( SIG_OBJECT_IDENTITY_PARENT,
                       EXPORTV3_CALLBACK(g3dexportv3object_identityParent),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    /*** write object type ***/
    size += g3dexportv3_writeChunk ( SIG_OBJECT_IDENTITY_TYPE,
                   EXPORTV3_CALLBACK(g3dexportv3object_identityType),
                                     ged,
                                     obj,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3object_active ( G3DEXPORTV3DATA *ged, 
                                         G3DOBJECT     *obj, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t active = ( obj->flags & OBJECTINACTIVE ) ? 0x00 : 0x01;

    return g3dexportv3_fwritel ( &active, fdst );
}

/******************************************************************************/
static uint32_t g3dexportv3object_noShading ( G3DEXPORTV3DATA *ged, 
                                              G3DOBJECT     *obj, 
                                              uint32_t       flags, 
                                              FILE          *fdst ) {
    uint32_t noshading = ( obj->flags & OBJECTNOSHADING ) ? 0x01 : 0x00;

    return g3dexportv3_fwritel ( &noshading, fdst );
}

/******************************************************************************/
uint32_t g3dexportv3object ( G3DEXPORTV3DATA *ged, 
                             G3DOBJECT     *obj, 
                             uint32_t       flags,
                             FILE          *fdst ) {
    uint32_t size = 0x00;

    /*** write objet identity ***/
    size += g3dexportv3_writeChunk ( SIG_OBJECT_IDENTITY,
                   EXPORTV3_CALLBACK(g3dexportv3object_identity),
                                     ged,
                                     obj,
                                     0xFFFFFFFF,
                                     fdst );

    /*** write objet matrix transformations ***/
    size += g3dexportv3_writeChunk ( SIG_OBJECT_TRANSFORMATION,
                   EXPORTV3_CALLBACK(g3dexportv3object_transformation),
                                     ged,
                                     obj,
                                     0xFFFFFFFF,
                                     fdst );

    if ( obj->lkey ) {
        /*** write object's animation keys ***/
        size += g3dexportv3_writeChunk ( SIG_OBJECT_KEYS,
                       EXPORTV3_CALLBACK(g3dexportv3object_keys),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );

        size += g3dexportv3_writeChunk ( SIG_OBJECT_PATHS,
                       EXPORTV3_CALLBACK(g3dexportv3object_paths),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->ltag ) {
        /*** write object's animation keys ***/
        size += g3dexportv3_writeChunk ( SIG_OBJECT_TAGS,
                       EXPORTV3_CALLBACK(g3dexportv3object_tags),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    /*** This concerns primitive as well and maybe other objects in ***/
    /*** future developments so this goes here and not in g3dexportv3mesh() ***/
    if ( obj->type & MESH ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        if ( mes->luvmap ) {
            size += g3dexportv3_writeChunk ( SIG_OBJECT_UVMAPS,
                           EXPORTV3_CALLBACK(g3dexportv3object_uvmaps),
                                             ged,
                                             mes,
                                             0xFFFFFFFF,
                                             fdst );
        }

        if ( mes->ltex ) {
            size += g3dexportv3_writeChunk ( SIG_OBJECT_TEXTURES,
                           EXPORTV3_CALLBACK(g3dexportv3object_textures),
                                             ged,
                                             mes,
                                             0xFFFFFFFF,
                                             fdst );
        }
    }

    /*** write object shades or not ***/
    size += g3dexportv3_writeChunk ( SIG_OBJECT_NOSHADING,
                   EXPORTV3_CALLBACK(g3dexportv3object_noShading),
                                     ged,
                                     obj,
                                     0xFFFFFFFF,
                                     fdst );

    /*** write object active or not ***/
    size += g3dexportv3_writeChunk ( SIG_OBJECT_ACTIVE,
                   EXPORTV3_CALLBACK(g3dexportv3object_active),
                                     ged,
                                     obj,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}
