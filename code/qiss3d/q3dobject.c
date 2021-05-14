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
#include <qiss3d/q3d.h>

/******************************************************************************/
G3DOBJECT *qobject_getObject ( Q3DOBJECT *qobj ) {
    return qobj->obj;
}

/******************************************************************************/
void q3dobject_init ( Q3DOBJECT *qobj,
                      G3DOBJECT *obj,
                      uint32_t   id,
                      uint64_t   flags,
                      void     (*Free)      ( Q3DOBJECT * ),
                      void     (*Bound)     ( Q3DOBJECT * ),
                      void     (*Intersect) ( Q3DOBJECT *obj, 
                                              Q3DRAY    *ray,
                                              float      frame,
                                              uint64_t   render_flags ) ) {
    qobj->obj   = obj;
    qobj->id    = id;
    qobj->flags = flags;

    qobj->free      = Free;
    qobj->bound     = Bound;
    qobj->intersect = Intersect;
}

/******************************************************************************/
Q3DOBJECT *q3dobject_import_r ( G3DOBJECT *obj,
                                float      frame ) {
    LIST *ltmpchildren = obj->lchildren;


    while ( ltmpchildren ) {
        G3DOBJECT *child  = ( G3DOBJECT * ) ltmpchildren->data;
        Q3DOBJECT *qchild = q3dobject_import_r ( child, frame );

        ltmpchildren = ltmpchildren->next;
    }

    switch ( obj->type ) {
        case G3DMESHTYPE : {
            G3DMESH *mes = ( G3DMESH * ) obj;
            Q3DMESH *qmes = q3dmesh_new ( mes, frame );

            q3dobject_addChild ( );
        } break;

        case G3DSYMMETRYTYPE :

        break;

        default : {


        } break;
    }

}
