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
#include <g3dengine/vtable/g3dprimitivevtable.h>

/******************************************************************************/
G3DPRIMITIVEVTABLE _vtable = { G3DPRIMITIVEVTABLE_DEFAULT };

/******************************************************************************/
G3DPRIMITIVE* g3dprimitive_default_copy ( G3DPRIMITIVE *pri,
                                          uint32_t      id,
                                          const char   *name,
                                          uint64_t      engineFlags ) {
    G3DPRIMITIVE *cpypri;
    void *data;
    uint32_t i;

    if ( ( data = calloc ( 0x01, pri->datalen ) ) == NULL ) {
        fprintf ( stderr, "g3dprimitive_copy: memory allocation failed\n" );

        return NULL;
    }

    /*** copy primitive private data ***/
    memcpy ( data, pri->data, pri->datalen );

    /*** create the primitive ***/
    cpypri = g3dprimitive_new ( G3DOBJECTCAST(pri)->id, 
                                G3DOBJECTCAST(pri)->name, data, pri->datalen );

    G3DOBJECTCAST(cpypri)->type = G3DOBJECTCAST(pri)->type;

    /*** A primitive is a mesh ***/
    g3dmesh_clone ( G3DMESHCAST(pri), NULL, G3DMESHCAST(cpypri), engineFlags );


    return cpypri;
}

/******************************************************************************/
G3DMESH *g3dprimitive_convert ( G3DPRIMITIVE *pri, 
                                uint64_t      engineFlags ) {
    LIST *ltmpchild = G3DOBJECTCAST(pri)->childList;
    G3DMESH *mes;

    mes = g3dmesh_default_copy ( G3DMESHCAST(pri),
                                 G3DOBJECTCAST(pri)->id,
                                 G3DOBJECTCAST(pri)->name,
                                 engineFlags );

    while ( ltmpchild ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpchild->data;

       g3dobject_removeChild( child->parent     , child, engineFlags  );
       g3dobject_addChild   ( G3DOBJECTCAST(mes), child, engineFlags  );

       ltmpchild = ltmpchild->next;
    }

    if ( G3DOBJECTCAST(pri)->parent ) {
        G3DOBJECT *parent = G3DOBJECTCAST(pri)->parent;

        g3dobject_removeChild ( parent, G3DOBJECTCAST(pri), engineFlags );
        g3dobject_addChild    ( parent, G3DOBJECTCAST(mes), engineFlags );
    }


    return mes;
}

/******************************************************************************/
uint32_t g3dprimitive_default_draw ( G3DPRIMITIVE *pri, 
                                     G3DCAMERA    *curcam, 
                                     G3DENGINE    *engine, 
                                     uint64_t      engineFlags ) {
    g3dmesh_default_draw ( G3DMESHCAST(pri), 
                           curcam, 
                           engine, 
                           engineFlags & (~MODEMASK) | VIEWOBJECT );

    return 0x00;
}

/******************************************************************************/
void g3dprimitive_default_free ( G3DPRIMITIVE *pri ) {
    if ( pri->data ) {
        free ( pri->data );
    }
}

/******************************************************************************/
void g3dprimitive_init ( G3DPRIMITIVE       *pri, 
                         uint32_t            id,
                         char               *name,
                         void               *data,
                         uint32_t            datalen,
                         G3DPRIMITIVEVTABLE *vtable ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) pri;

    pri->data    = data;
    pri->datalen = datalen;

    g3dmesh_init   ( G3DMESHCAST(pri),
                     G3DPRIMITIVETYPE,
                     id,
                     name,
                     0x00,
                     vtable ? G3DMESHVTABLECAST(vtable)
                            : G3DMESHVTABLECAST(&_vtable) );
}

/******************************************************************************/
G3DPRIMITIVE *g3dprimitive_new ( uint32_t id, 
                                 char    *name, 
                                 void    *data,
                                 uint32_t datalen ) {
    G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) calloc ( 0x01, sizeof ( G3DPRIMITIVE ) );

    if ( pri == NULL ) {
        fprintf ( stderr, "g3dprimitive_new: calloc failed\n" );

        return NULL;
    }

    g3dprimitive_init ( pri, id, name, data, datalen, NULL );


    return pri;
}
