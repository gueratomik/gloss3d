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
#include <g3dengine/g3dengine.h>

/******************************************************************************/
static G3DOBJECT *_default_copy ( G3DOBJECT *obj, 
                                  uint64_t   engineFlags ) {
    G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
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
    cpypri = g3dprimitive_new ( ((G3DOBJECT*)pri)->id, 
                                ((G3DOBJECT*)pri)->name, data, pri->datalen );

    ((G3DOBJECT*)cpypri)->type = ((G3DOBJECT*)pri)->type;

    /*** A primitive is a mesh ***/
    g3dmesh_clone ( ( G3DMESH * ) pri, NULL, ( G3DMESH * ) cpypri, engineFlags );


    return ( G3DOBJECT * ) cpypri;
}

/******************************************************************************/
G3DMESH *g3dprimitive_convert ( G3DPRIMITIVE *pri, 
                                uint64_t      engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) pri;
    G3DMESH *mes;

    /*** some trick to force the creation of a Mesh, not a Primitive. ***/
    obj->copy = COPY_CALLBACK(g3dmesh_default_copy);

    /* deep copy */
    mes = ( G3DMESH * ) g3dobject_copy ( obj,
                                         obj->id, 
                                         obj->name, 
                                         engineFlags );

    g3dobject_invalidate( ( G3DOBJECT * ) mes, INVALIDATE_ALL );

    if ( obj->parent ) {
        G3DOBJECT *parent = obj->parent;

        g3dobject_removeChild ( parent, obj, engineFlags );
        g3dobject_addChild    ( parent, ( G3DOBJECT * ) mes, engineFlags );
    }

    /*** Restore the default copy function ***/
    obj->copy = COPY_CALLBACK(_default_copy);


    return mes;
}

/******************************************************************************/
static uint32_t _default_pick ( G3DPRIMITIVE *pri, 
                                G3DCAMERA    *curcam, 
                                uint64_t      engineFlags ) {
    g3dmesh_default_pick( ( G3DMESH * ) pri, curcam, engineFlags );

    return 0x00;
}

/******************************************************************************/
static uint32_t _default_draw ( G3DPRIMITIVE *pri, 
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
static void _default_free ( G3DPRIMITIVE *pri ) {
    if ( pri->data ) {
        free ( pri->data );
    }
}

/******************************************************************************/
void g3dprimitive_init ( G3DPRIMITIVE *pri, 
                         uint32_t     id,
                         char        *name,
                         void        *data,
                         uint32_t     datalen ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) pri;

    pri->data    = data;
    pri->datalen = datalen;

    g3dmesh_init   ( G3DMESHCAST(pri),
                     G3DPRIMITIVETYPE,
                     id,
                     name,
                     0x00,
       DRAW_CALLBACK(_default_draw),
       FREE_CALLBACK(_default_free),
       PICK_CALLBACK(_default_pick),
       ANIM_CALLBACK(NULL),
     UPDATE_CALLBACK(NULL),
       POSE_CALLBACK(NULL),
       COPY_CALLBACK(_default_copy),
  TRANSFORM_CALLBACK(NULL),
   ACTIVATE_CALLBACK(NULL),
 DEACTIVATE_CALLBACK(NULL),
     COMMIT_CALLBACK(NULL),
   ADDCHILD_CALLBACK(NULL),
REMOVECHILD_CALLBACK(NULL),
  SETPARENT_CALLBACK(NULL),
       DUMP_CALLBACK(NULL) );
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

    g3dprimitive_init ( pri, id, name, data, datalen );


    return pri;
}
