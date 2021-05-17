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
static void q3dobject_default_free ( Q3DOBJECT *qobj ) {

}

/******************************************************************************/
static void qobject_default_intersect ( Q3DOBJECT *qobj,
                                        Q3DRAY    *qray,
                                        float      frame,
                                        uint64_t   query_flags,
                                        uint64_t   render_flags ) {
}

/******************************************************************************/
void qobject_intersect_r ( Q3DOBJECT *qobj,
                           Q3DRAY    *qray,
                           float      frame,
                           uint64_t   query_flags,
                           uint64_t   render_flags ) {
    Q3DRAY locqray;

    memcpy ( &locqray, qray, sizeof ( Q3DRAY ) );

    q3dvector_matrix ( qray->ori, qobj->IMVX , &locqray.ori );
    q3dvector_matrix ( qray->dir, qobj->TIVMX, &locqray.dir );

    if ( qobj->intersect ) {
        qobj->intersect ( qobj, 
                         &locqray, 
                          frame, 
                          query_flags,
                          render_flags );
    }

    while ( ltmpchildren ) {
        Q3DOBJECT *qchild = ( Q3DOBJECT * ) ltmpchildren->data;

        qobject_intersect_r ( qchild, 
                             &locqray, 
                              frame,
                              query_flags,
                              render_flags );

        ltmpchildren = ltmpchildren->next;
    }

    qray->color    = locqray.color;
    qray->distance = locqray.distance;
    qray->qobj     = locqray.qobj;
    qray->surface  = locqray.surface;
}

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
                      void     (*Intersect) ( Q3DOBJECT *obj, 
                                              Q3DRAY    *ray,
                                              float      frame,
                                              uint64_t   query_flags,
                                              uint64_t   render_flags ) ) {
    qobj->obj   = obj;
    qobj->id    = id;
    qobj->flags = flags;

    qobj->free      = Free;
    qobj->intersect = Intersect;

    g3dcore_invertMatrix ( obj->lmatrix, qobj->IMVX );
    g3dcore_transposeMatrix ( qobj->IMVX, qobj->TIMVX );
}

/******************************************************************************/
Q3DOBJECT *q3dobject_new ( G3DOBJECT *obj ) {
    Q3DOBJECT *qobj = ( Q3DOBJECT * ) calloc ( 0x01, sizeof ( Q3DOBJECT ) );

    if ( qobj == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    q3dobject_init ( qobj,
                     obj,
                     id,
                     flags,
                     q3dobject_default_free,
                     q3dobject_default_intersect );

    return qobj;
}

/******************************************************************************/
Q3DOBJECT *q3dobject_import_r ( G3DOBJECT *obj,
                                float      frame ) {
    LIST *ltmpchildren = obj->lchildren;
    Q3DOBJECT *qobj;

    switch ( obj->type ) {
        case G3DMESHTYPE : {
            G3DMESH *mes = ( G3DMESH * ) obj;
            Q3DMESH *qmes = q3dmesh_new ( mes, 0x00, 0x00, frame, 0x40  );

            qobj = qmes;
        } break;

        case G3DSYMMETRYTYPE : {
            G3DSYMMETRY *sym = ( G3DSYMMETRY * ) obj;
            Q3DSYMMETRY *qsym = q3dsymmetry_new ( sym );

            qobj = qsym;
        } break;

        case G3DSCENETYPE : {
            G3DSCENE *sce = ( G3DSCENE * ) obj;
            Q3DSCENE *qsce = q3dscene_new ( sce,
                                            0x00,
                                            0x00 );

            qobj = qsce;
        } break;

        default : {
            qobj = q3dobject_new ( obj );
        } break;
    }

    while ( ltmpchildren ) {
        G3DOBJECT *child  = ( G3DOBJECT * ) ltmpchildren->data;
        Q3DOBJECT *qchild = q3dobject_import_r ( child, frame );

        q3dobject_addChild ( qobj, qchild );

        ltmpchildren = ltmpchildren->next;
    }
}
