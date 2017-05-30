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
#include <g3durmanager.h>

/******************************************************************************/
void g3durmanager_clearFrom ( G3DURMANAGER *urm, LIST **lprocs ) {
    LIST *ltmpprocs = (*lprocs);
    LIST *lprev     = (*lprocs) ? (*lprocs)->prev : NULL;
    /*** Initalize undoredo items by pushing one element in the list ***/
    /*** then we know we have hit the end of the list ***/
    LIST *lnew      = list_new ( NULL );

    while ( ltmpprocs ) {
        G3DURITEM *uri = ( G3DURITEM * ) ltmpprocs->data;

        /*** The first stack item is NULL, dont forget to check the value ***/
        if ( uri ) {
            g3duritem_free ( uri );
        }

        ltmpprocs = ltmpprocs->next;
    }

    list_free ( lprocs, NULL );

    if ( lprev ) {
        lprev->next = lnew;

        lnew->prev  = lprev;
    } else {
        (*lprocs)   = lnew;
    }
}

/******************************************************************************/
void g3durmanager_clear ( G3DURMANAGER *urm ) {
    g3durmanager_clearFrom ( urm, &urm->lprocs );

    /** urm->lcur should never be NULL, and the rest of code will assume so **/
    urm->lcurprocs = urm->lprocs;
}

/******************************************************************************/
void g3durmanager_init ( G3DURMANAGER *urm, uint32_t level ) {
    urm->level = level;

    g3durmanager_clear ( urm );
}

/******************************************************************************/
G3DURMANAGER *g3durmanager_new ( uint32_t level ) {
    uint32_t structsize = sizeof ( G3DURMANAGER );
    G3DURMANAGER *urm = ( G3DURMANAGER * ) calloc ( 0x01, structsize );

    if ( urm == NULL ) {
        fprintf ( stderr, "g3durmanager_new: memory allocation failed\n" );

        return NULL;
    }

    g3durmanager_init ( urm, level );


    return urm;
}

/******************************************************************************/
uint32_t g3durmanager_undo ( G3DURMANAGER *urm, uint32_t flags ) {
    G3DURITEM *uri = urm->lcurprocs->data;


    if ( uri == NULL ) {
        fprintf ( stderr, "g3durmanager_undo: hit the end of the list\n" );
    } else {
        if ( uri->undo ) uri->undo ( urm, uri->data, flags );

        /*** changes were undone, so not commited ***/
        uri->commit = 0x00;

        urm->lcurprocs = urm->lcurprocs->next;

        return uri->return_flags;
    }

    return 0x00;
}

/******************************************************************************/
uint32_t g3durmanager_redo ( G3DURMANAGER *urm, uint32_t flags ) {
    if ( urm->lcurprocs->prev ) {
        G3DURITEM *uri ;

        urm->lcurprocs = urm->lcurprocs->prev;

        uri = urm->lcurprocs->data;

        if ( uri->redo ) uri->redo ( urm, uri->data, flags );

        /*** changes were done, so commited ***/
        uri->commit = 0x01;

        return uri->return_flags;
    } else {
        fprintf ( stderr, "g3durmanager_undo: hit the end of the list\n" );
    }

    return 0x00;
}

/******************************************************************************/
void g3durmanager_push ( G3DURMANAGER *urm, 
                         void (*undofunc) ( G3DURMANAGER *, void *, uint32_t ),
                         void (*redofunc) ( G3DURMANAGER *, void *, uint32_t ),
                         void (*freefunc) ( void *, uint32_t ),
                         void *data, uint32_t return_flags ) {

    G3DURITEM *uri = g3duritem_new ( undofunc,
                                     redofunc,
                                     freefunc,
                                     data,
                                     return_flags );

    list_freeprev ( urm->lcurprocs, (void(*)(void*))g3duritem_free );

    printf("B: %d %d %d\n", list_count ( urm->lcurprocs ), urm->lprocs, urm->lcurprocs );


    /*** set this item as the current item ***/
    urm->lprocs = list_insert ( &urm->lcurprocs, uri );

    printf("A: %d %d %d\n", list_count ( urm->lcurprocs ), urm->lprocs, urm->lcurprocs );

    if ( urm->level ) {
        LIST *lclear = list_forward ( urm->lprocs, urm->level );

        if ( lclear ) {
            printf ( "Maximum undo level reached, clearing ...\n" );

            g3durmanager_clearFrom ( urm, &lclear );
        }
    }
}

/******************************************************************************/
G3DURITEM *g3duritem_new ( void (*undofunc) ( G3DURMANAGER *, void *, uint32_t ),
                           void (*redofunc) ( G3DURMANAGER *, void *, uint32_t ),
                           void (*freefunc) ( void *, uint32_t ), 
                           void *data,
                           uint32_t return_flags ) {
    G3DURITEM *uri = ( G3DURITEM * ) calloc ( 0x01, sizeof ( G3DURITEM ));

    if ( uri == NULL ) {
        fprintf ( stderr, "g3duritem_new: memory allocation failed\n" );

        return NULL;
    }

    if ( freefunc == NULL ) {
        fprintf ( stderr, "WARNING: g3duritem_new no \"free\" function set\n" );
    }

    uri->undo = undofunc;
    uri->redo = redofunc;
    uri->free = freefunc;
    uri->data = data;
    uri->return_flags = return_flags;
    uri->commit = 0x01;


    return uri;
}

/******************************************************************************/
void g3duritem_free ( G3DURITEM *uri ) {
    if ( uri->free ) uri->free ( uri->data, uri->commit );

    free ( uri );
}
