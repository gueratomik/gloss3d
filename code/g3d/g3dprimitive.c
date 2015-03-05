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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
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
#include <g3d.h>

/******************************************************************************/
G3DOBJECT *g3dprimitive_copy ( G3DOBJECT *obj, uint32_t engine_flags ) {
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
    g3dmesh_clone ( ( G3DMESH * ) pri, ( G3DMESH * ) cpypri, engine_flags );


    return ( G3DOBJECT * ) cpypri;
}

/******************************************************************************/
G3DMESH *g3dprimitive_convert ( G3DPRIMITIVE *pri, uint32_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) pri;
    G3DMESH *mes;

    /*** some trick to force the creation of a Mesh, not a Primitive. ***/
    obj->copy = g3dmesh_copy;

    mes = ( G3DMESH * ) g3dobject_copy ( obj, 0x00, engine_flags );

    /*** prepare the precomputed values for Catmull-Clark Subdivision ***/
    g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          NULL,
                          COMPUTEFACEPOSITION |
                          COMPUTEEDGEPOSITION |
                          COMPUTEFACENORMAL   |
                          COMPUTEVERTEXNORMAL, engine_flags );

    if ( obj->parent ) {
        G3DOBJECT *parent = obj->parent;

        g3dobject_removeChild ( parent, obj );
        g3dobject_addChild    ( parent, ( G3DOBJECT * ) mes );
    }

    /*** Restore the default copy function ***/
    obj->copy = g3dprimitive_copy;


    return mes;
}

/******************************************************************************/
void g3dprimitive_draw ( G3DOBJECT *obj, G3DCAMERA *curcam, uint32_t engine_flags ) {
    /*** Primitive is not editable ***/
    engine_flags &= (~MODEMASK) | VIEWOBJECT;

    /*static GLfloat whiteDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f },
                   whiteSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f },
                   whiteAmbient[]  = { 0.0f, 0.0f, 0.0f, 1.0f },
                   whiteShininess  = 0.0f;
    G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
    uint32_t i;

    glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE,    whiteDiffuse   );
    glMaterialfv ( GL_FRONT_AND_BACK, GL_AMBIENT,    whiteAmbient   );
    glMaterialfv ( GL_FRONT_AND_BACK, GL_SPECULAR,   whiteSpecular  );
    glMaterialfv ( GL_FRONT_AND_BACK, GL_SHININESS, &whiteShininess );*/

    g3dmesh_draw ( obj, curcam, engine_flags );
}

/******************************************************************************/
void g3dprimitive_free ( G3DOBJECT *obj ) {
    G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;

    if ( pri->data ) {
        free ( pri->data );
    }
}

/******************************************************************************/
void g3dprimitive_init ( G3DPRIMITIVE *pri, uint32_t id, char *name,
                                                         void *data,
                                                         uint32_t datalen ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) pri;

    pri->data    = data;
    pri->datalen = datalen;

    g3dobject_init ( obj, G3DPRIMITIVETYPE, id, name, g3dprimitive_draw,
                                                      g3dprimitive_free );

    obj->copy = g3dprimitive_copy;
}

/******************************************************************************/
G3DPRIMITIVE *g3dprimitive_new ( uint32_t id, char *name, void *data,
                                                          uint32_t datalen ) {
    G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) calloc ( 0x01, sizeof ( G3DPRIMITIVE ) );

    if ( pri == NULL ) {
        fprintf ( stderr, "g3dprimitive_new: calloc failed\n" );

        return NULL;
    }

    g3dprimitive_init ( pri, id, name, data, datalen );


    return pri;
}