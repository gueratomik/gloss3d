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
uint32_t g3dexportscene ( G3DSCENE *sce, uint32_t flags, FILE *fdst ) {
    LIST *ltmpobj = ((G3DOBJECT*)sce)->lchildren;
    uint32_t objID = 0x00;
    uint32_t size = 0x00;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

        obj->id = objID++;

        /*** write objet identity ***/
        size += g3dexport_writeChunk ( SIG_OBJECT,
                                       g3dexportobject,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );

        ltmpobj = ltmpobj->next;
    }

    return size;
}

/******************************************************************************/
void g3dscene_exportv2 ( G3DSCENE *sce, 
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

    if ( ( fdst = fopen ( filename, "wb" ) ) == NULL ) {
        fprintf ( stderr, "g3dscene_export: cannot write destination file\n" );

        return;
    }

    size = g3dexport_writeChunk ( SIG_SCENE,
                                  g3dexportscene,
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
        G3DEXPORTEXTENSION *ext = ( G3DEXPORTEXTENSION * ) ltmpext->data;


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
