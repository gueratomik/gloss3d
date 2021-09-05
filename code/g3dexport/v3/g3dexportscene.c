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
static uint32_t g3dexportv3objects_declareObject ( G3DEXPORTV3DATA *ged, 
                                                   G3DOBJECT        *obj, 
                                                   uint32_t         flags, 
                                                   FILE            *fdst ) {
    g3dexportv3_fwrite ( obj->name, strlen ( obj->name ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t g3dexportv3objects_declareEntry ( G3DEXPORTV3DATA *ged, 
                                                  G3DOBJECT        *obj, 
                                                  uint32_t         flags, 
                                                  FILE            *fdst ) {
    uint32_t size = 0x00;

    switch ( obj->type ) {
        case G3DSCENETYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_SCENE,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DMESHTYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_MESH,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DSPHERETYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_SPHERE,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DTORUSTYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_TORUS,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DCUBETYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_CUBE,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        /*case G3DCONETYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_CONE,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;*/

        case G3DPLANETYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_PLANE,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DCYLINDERTYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_CYLINDER,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DTUBETYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_TUBE,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DCAMERATYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_CAMERA,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DSUBDIVIDERTYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_SUBDIVIDER,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DSPLINETYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_SPLINE,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DSPLINEREVOLVERTYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_SPLINEREVOLVER,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DSKINTYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_SKIN,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DFFDTYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_FFD,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DWIREFRAMETYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_WIREFRAME,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DSYMMETRYTYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_SYMMETRY,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DTEXTTYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_TEXT,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DBONETYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_BONE,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DLIGHTTYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_LIGHT,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DMORPHERTYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_MORPHER,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DINSTANCETYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_INSTANCE,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        default :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_NULL,
                                             g3dexportv3objects_declareObject,
                                             ged,
                                             obj,
                                             0xFFFFFFFF,
                                             fdst );
        break;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3objects_declare ( G3DEXPORTV3DATA *ged, 
                                             G3DSCENE        *sce, 
                                             uint32_t         flags, 
                                             FILE            *fdst ) {
    LIST *lobj = NULL, *ltmpobj; 
    uint32_t size = 0x00;

    ged->objectID = 0x00;

    /*** flatten the object tree ***/
    g3dobject_treeToList_r ( sce, &lobj );

    ltmpobj = lobj;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

        obj->id = ged->objectID++;

        ged->currentObject = obj;

        size += g3dexportv3_writeChunk ( SIG_OBJECT_DECLARE_ENTRY,
                                         g3dexportv3objects_declareEntry,
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );

        ged->currentObject = NULL;

        ltmpobj = ltmpobj->next;
    }

    list_free ( &lobj, NULL );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3objects_define ( G3DEXPORTV3DATA *ged, 
                                            G3DSCENE        *sce, 
                                            uint32_t         flags, 
                                            FILE            *fdst ) {
    LIST *lobj = NULL, *ltmpobj; 
    uint32_t size = 0x00;

    /*** flatten the object tree ***/
    g3dobject_treeToList_r ( sce, &lobj );

    ltmpobj = lobj;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

        ged->currentObject = obj;

        size += g3dexportv3_writeChunk ( SIG_OBJECT_ENTRY,
                                         g3dexportv3object,
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );

        ged->currentObject = NULL;

        ltmpobj = ltmpobj->next;
    }

    list_free ( &lobj, NULL );

    return size;
}


/******************************************************************************/
static uint32_t g3dexportv3root_objects ( G3DEXPORTV3DATA *ged, 
                                          G3DSCENE        *sce, 
                                          uint32_t         flags, 
                                          FILE            *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECTS_DECLARE,
                                     g3dexportv3objects_declare,
                                     ged,
                                     sce,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECTS_DEFINE,
                                     g3dexportv3objects_define,
                                     ged,
                                     sce,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3root_extensions ( G3DEXPORTV3DATA *ged, 
                                           G3DSCENE      *sce, 
                                           uint32_t       flags, 
                                           FILE          *fdst ) {
    LIST *ltmpext = ged->lext;
    uint32_t size = 0x00;

    while ( ltmpext ) {
        G3DEXPORTV3EXTENSION *ext = ( G3DEXPORTV3EXTENSION * ) ltmpext->data;

        size += g3dexportv3_writeChunk ( SIG_EXTENSION_ENTRY,
                                         g3dexportv3extension,
                                         ged,
                                         ext,
                                         0xFFFFFFFF,
                                         fdst );

        ltmpext = ltmpext->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3scene_materials ( G3DEXPORTV3DATA *ged, 
                                           G3DSCENE      *sce, 
                                           uint32_t       flags, 
                                           FILE          *fdst ) {
    LIST *ltmpmat = sce->lmat;
    uint32_t matID = 0x00;
    uint32_t size = 0x00;

    while ( ltmpmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpmat->data;

        mat->id = matID++;

        size += g3dexportv3_writeChunk ( SIG_MATERIAL_ENTRY,
                                         g3dexportv3material,
                                         ged,
                                         mat,
                                         0xFFFFFFFF,
                                         fdst );

        ltmpmat = ltmpmat->next;
    }

    return size;
}

/******************************************************************************/
uint32_t g3dexportv3scene ( G3DEXPORTV3DATA *ged, 
                          G3DSCENE      *sce, 
                          uint32_t       flags, 
                          FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_MATERIALS,
                                     g3dexportv3scene_materials,
                                     ged,
                                     sce,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
uint32_t g3dexportv3root ( G3DEXPORTV3DATA *ged, 
                         G3DSCENE      *sce, 
                         uint32_t       flags, 
                         FILE          *fdst ) {

    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECTS,
                                     g3dexportv3root_objects,
                                     ged,
                                     sce,
                                     0xFFFFFFFF,
                                     fdst );

    if ( ged->lext ) {
        size += g3dexportv3_writeChunk ( SIG_EXTENSIONS,
                                         g3dexportv3root_extensions,
                                         ged,
                                         sce,
                                         0xFFFFFFFF,
                                         fdst );
    }

    return size;
}

/******************************************************************************/
void g3dscene_exportv3 ( G3DSCENE *sce, 
                         char     *filename,
                         char     *comment,
                         LIST     *lextension, 
                         uint32_t  flags ) {
    char *version = VERSION;
    uint32_t objid = 0x00;
    FILE *fdst;
    G3DOBJECT *obj = ( G3DOBJECT * ) sce;
    LIST *ltmpext = lextension;
    uint32_t size = 0x00;
    G3DEXPORTV3DATA ged;

    if ( ( fdst = fopen ( filename, "wb" ) ) == NULL ) {
        fprintf ( stderr, "g3dscene_export: cannot write destination file\n" );

        return;
    }

    memset ( &ged, 0x00, sizeof ( ged ) );

    ged.currentScene = sce;
    ged.lext         = lextension;

    size = g3dexportv3_writeChunk ( SIG_ROOT,
                                    g3dexportv3root,
                                   &ged,
                                    sce,
                                    0xFFFFFFFF,
                                    fdst );

    fprintf ( stderr, "%d bytes written\n", size );


#ifdef UNUSED
    /* get unique IDs for all objects */
    /*g3dobject_renumber_r ( sce, &objid );*/

    writef ( MAGSIG, strlen ( MAGSIG ), 0x01, fdst );

    chunk_write ( APPSIG, app_blocksize ( version,
                                          comment, APPSAVEALL ), fdst );
    app_writeblock ( version, comment, APPSAVEALL, fdst );

    chunk_write ( OBJSIG, object_blocksize ( obj, OBJECTSAVEALL ), fdst );

    object_writeblock ( obj, &objid, OBJECTSAVEALL, fdst );

    while ( ltmpext ) {
        G3DEXPORTV3EXTENSION *ext = ( G3DEXPORTV3EXTENSION * ) ltmpext->data;


        chunk_write ( EXTENSIONSIG, ext->blockSize(ext->data) +
        /* we add +0x06 + name length for the EXTENSIONNAME chunk */
                                    0x06 + strlen ( ext->name ) +
        /* we add +0x06 for the EXTENSIONEND chunk */
                                    0x06, fdst );

        extensionname_writeBlock ( ext, fdst );

        ext->writeBlock ( ext->data, fdst );

        chunk_write ( EXTENSIONENDSIG, 0x00, fdst );

        ltmpext = ltmpext->next;
    }
#endif

    fclose ( fdst );
}
