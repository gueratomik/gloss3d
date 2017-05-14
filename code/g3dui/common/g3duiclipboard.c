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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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
#include <g3dui.h>

/******************************************************************************/
G3DUICOPIEDITEM *g3duicopieditem_new ( G3DOBJECT *obj,
                                       G3DKEY *key, uint32_t operation ) {
    uint32_t structsize = sizeof ( G3DUICOPIEDITEM );
    G3DUICOPIEDITEM *item = ( G3DUICOPIEDITEM * ) calloc ( 0x01, structsize );

    if ( item == NULL ) {
        fprintf ( stderr, "g3duicopieditem_new: memory allocation failed\n" );

        return NULL;
    }

    switch ( operation ) {
        case CLIPBOARDCOPYOBJECT :
            item->obj = obj;

            /*** Save world matrix for converting coordinates into   ***/
            /*** the future parent local coordinates. This copy is   ***/
            /*** needed because there is no guaranty the parent will ***/
            /*** will still exist when pasting.                      ***/
            memcpy ( item->wmatrix, obj->wmatrix, sizeof ( item->wmatrix ) );
        break;

        case CLIPBOARDCOPYKEY :
            item->key = key;
        break;

        default :
        break;
    }

    item->obj = obj;
    item->key = key;


    return item;
}

/******************************************************************************/
void g3duicopieditem_clear ( G3DUICOPIEDITEM *item, uint32_t operation ) {
    switch ( operation ) {
        case CLIPBOARDCOPYOBJECT :
            g3dobject_free ( item->obj );
        break;

        case CLIPBOARDCOPYKEY :
            g3dkey_free ( item->key );
        break;

        default :
        break;
    }
}

/******************************************************************************/
void g3duicopieditem_free ( G3DUICOPIEDITEM *item ) {
    free ( item );
}

/******************************************************************************/
G3DUICLIPBOARD *g3duiclipboard_new ( ) {
    uint32_t structsize = sizeof ( G3DUICLIPBOARD );
    G3DUICLIPBOARD *cli = ( G3DUICLIPBOARD * ) calloc ( 0x01, structsize );

    if ( cli == NULL ) {
        fprintf ( stderr, "g3duiclipboard_new: memory allocation failed\n" );

        return NULL;
    }

    return cli;
}

/******************************************************************************/
void g3duiclipboard_free ( G3DUICLIPBOARD *cli ) {
    if ( cli->operation != CLIPBOARDEMPTY ) {
        g3duiclipboard_clear ( cli );
    }

    free ( cli );
}

/******************************************************************************/
void g3duiclipboard_clear ( G3DUICLIPBOARD *cli ) {
    LIST *ltmpcpy = cli->lcpy;

    while ( ltmpcpy ) {
        G3DUICOPIEDITEM *item = ( G3DUICOPIEDITEM * ) ltmpcpy->data;

        g3duicopieditem_clear ( item, cli->operation );

        ltmpcpy = ltmpcpy->next;
    }

    list_free ( &cli->lcpy, (void (*)(void *)) g3duicopieditem_free );


    cli->operation = CLIPBOARDEMPTY;
}

/******************************************************************************/
void g3duiclipboard_copyObject ( G3DUICLIPBOARD *cli, G3DSCENE *sce,
                                                      LIST *lobj,
                                                      int recurse, 
                                                      uint32_t engine_flags ) {
    LIST *ltmpobj = lobj;

    /*** First clear the clipboard ***/
    g3duiclipboard_clear ( cli );

    cli->operation = CLIPBOARDCOPYOBJECT;
    cli->sce = sce;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;
        G3DOBJECT *cpy = ( G3DOBJECT * ) g3dobject_copy ( obj, 0x00, obj->name, engine_flags );
        G3DUICOPIEDITEM *item = g3duicopieditem_new ( cpy,  /*** object  ***/
                                                      NULL, /*** not key ***/
                                                      CLIPBOARDCOPYOBJECT );

        list_insert ( &cli->lcpy, item );

        ltmpobj = ltmpobj->next;
    }
}

/******************************************************************************/
void g3duiclipboard_paste ( G3DUICLIPBOARD *cli, G3DURMANAGER *urm,
                                                 G3DSCENE *sce,
                                                 G3DOBJECT *dst,
                                                 uint32_t flags ) {
    switch ( cli->operation ) {
        case CLIPBOARDCOPYOBJECT : {
            LIST *ltmpcpy = cli->lcpy;
            LIST *lcpyobj = NULL;

            while ( ltmpcpy ) {
                G3DUICOPIEDITEM *item = ( G3DUICOPIEDITEM * ) ltmpcpy->data; 
                G3DOBJECT *cpyobj;
                char str[0x100];

                /*** In order be sure the world matrix hasnt been affected ***/
                /*** by some matrix transformations in between, we copy it.***/
                memcpy ( item->obj->wmatrix,
                         item->wmatrix, sizeof ( item->wmatrix ) );

                /*** We copy again. That way, the copied objects still are  ***/
                /*** in memory and can be pasted again, until a copy occur. ***/
                cpyobj = g3dobject_copy ( item->obj, 0x00, item->obj->name, flags );

                /*** The object renaming is done here and not in the ***/
                /*** g3dobject_copy function because ths function is ***/
                /*** also used when converting primitives to meshes. ***/
                snprintf ( str, 0x100, "Copy of %s", item->obj->name );
                g3dobject_name ( cpyobj, str );

                cpyobj->id = g3dscene_getNextObjectID ( sce );


                list_insert ( &lcpyobj, cpyobj );

                /*g3dobject_importChild ( dst, cpyobj );*/

                ltmpcpy = ltmpcpy->next;
            }

            g3durm_object_addChildren ( urm, sce, flags, REDRAWVIEW | REDRAWLIST,
                                        NULL, dst, lcpyobj );

            list_free ( &lcpyobj, NULL );
        } break;

        default :
        break;
    }
}

/******************************************************************************/
void g3duiclipboard_copyKey ( G3DUICLIPBOARD *cli, G3DSCENE *sce,
                                                   G3DOBJECT *obj,
                                                   LIST *lkey ) {
    /*** First clear the clipboard ***/
    g3duiclipboard_clear ( cli );
}
