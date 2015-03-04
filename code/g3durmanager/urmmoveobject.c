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
#include <g3durmanager.h>

/******************************************************************************/
URMMOVEOBJECT *urmmoveobject_new ( G3DOBJECT *obj, G3DVECTOR *oldpos,
                                                   G3DVECTOR *oldrot,
                                                   G3DVECTOR *oldsca,
                                                   G3DVECTOR *newpos,
                                                   G3DVECTOR *newrot,
                                                   G3DVECTOR *newsca ) {
    uint32_t structsize = sizeof ( URMMOVEOBJECT );

    URMMOVEOBJECT *mos = ( URMMOVEOBJECT * ) calloc ( 0x01, structsize );

    if ( mos == NULL ) {
        fprintf ( stderr, "urmmoveobject_new: memory allocation falied\n" );

        return NULL;
    }

    mos->obj = obj;
    memcpy ( &mos->oldpos, oldpos, sizeof ( G3DVECTOR ) );
    memcpy ( &mos->oldrot, oldrot, sizeof ( G3DVECTOR ) );
    memcpy ( &mos->oldsca, oldsca, sizeof ( G3DVECTOR ) );
    memcpy ( &mos->newpos, newpos, sizeof ( G3DVECTOR ) );
    memcpy ( &mos->newrot, newrot, sizeof ( G3DVECTOR ) );
    memcpy ( &mos->newsca, newsca, sizeof ( G3DVECTOR ) );

    return mos;
}

/******************************************************************************/
void urmmoveobject_free ( URMMOVEOBJECT *mos ) {
    free ( mos );
}

/******************************************************************************/
void moveObject_free ( void *data, uint32_t commit ) {
    URMMOVEOBJECT *mos = ( URMMOVEOBJECT * ) data;

    urmmoveobject_free ( mos );
}

/******************************************************************************/
void moveObject_undo ( G3DURMANAGER *urm, void *data, uint32_t flags ) {
    URMMOVEOBJECT *mos = ( URMMOVEOBJECT * ) data;

    memcpy ( &mos->obj->pos, &mos->oldpos, sizeof ( G3DVECTOR ) );
    memcpy ( &mos->obj->rot, &mos->oldrot, sizeof ( G3DVECTOR ) );
    memcpy ( &mos->obj->sca, &mos->oldsca, sizeof ( G3DVECTOR ) );

    g3dobject_updateMatrix_r ( mos->obj, 0x00 );
}

/******************************************************************************/
void moveObject_redo ( G3DURMANAGER *urm, void *data, uint32_t flags ) {
    URMMOVEOBJECT *mos = ( URMMOVEOBJECT * ) data;

    memcpy ( &mos->obj->pos, &mos->newpos, sizeof ( G3DVECTOR ) );
    memcpy ( &mos->obj->rot, &mos->newrot, sizeof ( G3DVECTOR ) );
    memcpy ( &mos->obj->sca, &mos->newsca, sizeof ( G3DVECTOR ) );

    g3dobject_updateMatrix_r ( mos->obj, 0x00 );
}

/******************************************************************************/
void g3durm_object_move ( G3DURMANAGER *urm, G3DOBJECT *obj,
                                             G3DVECTOR *oldpos,
                                             G3DVECTOR *oldrot,
                                             G3DVECTOR *oldsca,
                                             uint32_t return_flags ) {
    URMMOVEOBJECT *mos;

    mos = urmmoveobject_new ( obj, oldpos,
                                   oldrot,
                                   oldsca, 
                                   &obj->pos, 
                                   &obj->rot,  
                                   &obj->sca );

    g3durmanager_push ( urm, moveObject_undo,
                             moveObject_redo,
                             moveObject_free, mos, return_flags );
}
