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
uint32_t q3dobject_getID ( Q3DOBJECT *qobj ) {
    return qobj->id;
}

/******************************************************************************/
void q3dobject_exec_r ( Q3DOBJECT *qobj, 
                        void     (*func)( Q3DOBJECT *, void * ),
                        void      *data ) {
    LIST *ltmpchildren = qobj->lchildren;

    if ( func ) func ( qobj, data );

    while ( ltmpchildren ) {
        Q3DOBJECT *qchild  = ( Q3DOBJECT * ) ltmpchildren->data;

        q3dobject_exec_r ( qchild, func, data );

        ltmpchildren = ltmpchildren->next;
    }
}

/******************************************************************************/
uint32_t q3dobject_count_r ( Q3DOBJECT *qobj ) {
    LIST *ltmpchildren = qobj->lchildren;
    uint32_t total = 0x01;

    while ( ltmpchildren ) {
        Q3DOBJECT *qchild  = ( Q3DOBJECT * ) ltmpchildren->data;

        total += q3dobject_count_r ( qchild );

        ltmpchildren = ltmpchildren->next;
    }

    return total;
}

/******************************************************************************/
void q3dobject_free ( Q3DOBJECT *qobj ) {
    if ( qobj->free ) qobj->free ( qobj );
}

/******************************************************************************/
void q3dobject_free_r ( Q3DOBJECT *qobj ) {
    LIST *ltmpchildren = qobj->lchildren;

    if ( qobj->free ) qobj->free ( qobj );

    while ( ltmpchildren ) {
        Q3DOBJECT *qchild  = ( Q3DOBJECT * ) ltmpchildren->data;

        q3dobject_free_r ( qchild );

        ltmpchildren = ltmpchildren->next;
    }
}

/******************************************************************************/
void q3dobject_addChild ( Q3DOBJECT *qobj,
                          Q3DOBJECT *child ) {
    list_insert ( &qobj->lchildren, child );
}

/******************************************************************************/
static void q3dobject_default_free ( Q3DOBJECT *qobj ) {

}

/******************************************************************************/
static uint32_t q3dobject_default_intersect ( Q3DOBJECT *qobj,
                                              Q3DRAY    *qray,
                                              float      frame,
                                              uint64_t   query_flags,
                                              uint64_t   render_flags ) {
    return 0x00;
}

/******************************************************************************/
uint32_t q3dobject_intersectWithCondition_r ( Q3DOBJECT  *qobj,
                                              Q3DRAY     *qray,
                                              Q3DSURFACE *discard,
                                              uint32_t  (*cond)(Q3DOBJECT *, 
                                                                void      *),
                                              void       *condData,
                                              float       frame,
                                              uint64_t    query_flags,
                                              uint64_t    render_flags ) {
    LIST *ltmpchildren = qobj->lchildren;
    uint32_t hit = 0x00;
    Q3DRAY locqray;

    memcpy ( &locqray, qray, sizeof ( Q3DRAY ) );

    q3dvector3f_matrix ( &qray->src, qobj->IMVX , &locqray.src );
    q3dvector3f_matrix ( &qray->dir, qobj->TIMVX, &locqray.dir );

    if ( ( cond == NULL ) || cond ( qobj, condData ) ) {
        if ( qobj->intersect ) {
            hit += qobj->intersect ( qobj, 
                                    &locqray, 
                                     discard,
                                     frame, 
                                     query_flags,
                                     render_flags );
        }
    }

    while ( ltmpchildren ) {
        Q3DOBJECT *qchild = ( Q3DOBJECT * ) ltmpchildren->data;

        hit += q3dobject_intersectWithCondition_r ( qchild, 
                                                   &locqray,
                                                    discard,
                                                    cond,
                                                    condData,
                                                    frame,
                                                    query_flags,
                                                    render_flags );

        ltmpchildren = ltmpchildren->next;
    }

    qray->flags   |= locqray.flags;
    qray->color    = locqray.color;
    qray->distance = locqray.distance;
    qray->qobjID   = locqray.qobjID;
    qray->qtriID   = locqray.qtriID;

    qray->ratio[0x00] = locqray.ratio[0x00];
    qray->ratio[0x01] = locqray.ratio[0x01];
    qray->ratio[0x02] = locqray.ratio[0x02];

    return ( hit ) ? 0x01 : 0x00;
}

/******************************************************************************/
uint32_t q3dobject_intersect_r ( Q3DOBJECT  *qobj,
                                 Q3DRAY     *qray,
                                 Q3DSURFACE *discard,
                                 float       frame,
                                 uint64_t    query_flags,
                                 uint64_t    render_flags ) {
    return q3dobject_intersectWithCondition_r ( qobj, 
                                                qray,
                                                discard,
                                                NULL,
                                                NULL,
                                                frame,
                                                query_flags,
                                                render_flags );
}

/******************************************************************************/
G3DOBJECT *q3dobject_getObject ( Q3DOBJECT *qobj ) {
    return qobj->obj;
}

/******************************************************************************/
void q3dobject_init ( Q3DOBJECT *qobj,
                      G3DOBJECT *obj,
                      uint32_t   id,
                      uint64_t   flags,
                      void     (*Free)      ( Q3DOBJECT * ),
                      uint32_t (*Intersect) ( Q3DOBJECT *obj, 
                                              Q3DRAY    *ray,
                                              float      frame,
                                              uint64_t   query_flags,
                                              uint64_t   render_flags ) ) {
    qobj->obj   = obj;
    qobj->id    = id;
    qobj->flags = flags;

    qobj->free      = Free;
    qobj->intersect = Intersect;

    g3dcore_invertMatrix    ( obj->lmatrix, qobj->IMVX  );
    g3dcore_transposeMatrix ( qobj->IMVX  , qobj->TIMVX );

    /*** Note: IWMVX could also be retrieved via obj->iwmatrix ***/
    g3dcore_invertMatrix    ( obj->wmatrix, qobj->IWMVX  );
    g3dcore_transposeMatrix ( qobj->IWMVX , qobj->TIWMVX );
}

/******************************************************************************/
Q3DOBJECT *q3dobject_new ( G3DOBJECT *obj, 
                           uint32_t   id,
                           uint64_t   flags ) {
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
    static Q3DSCENE *qsce;
    Q3DOBJECT *qobj;

    switch ( obj->type ) {
        case G3DSPHERETYPE   :
        case G3DCUBETYPE     :
        case G3DPLANETYPE    :
        case G3DTORUSTYPE    :
        case G3DCYLINDERTYPE :
        case G3DTUBETYPE     :
        case G3DMESHTYPE     : {
            G3DMESH *mes = ( G3DMESH * ) obj;
            Q3DMESH *qmes = q3dmesh_new ( mes, 
                                          qsce->qobjID++,
                                          0x00,
                                          frame,
                                          0x40  );

            qobj = ( Q3DOBJECT * ) qmes;
        } break;

        case G3DSYMMETRYTYPE : {
            G3DSYMMETRY *sym = ( G3DSYMMETRY * ) obj;
            Q3DSYMMETRY *qsym = q3dsymmetry_new ( sym, 
                                                  qsce->qobjID++,
                                                  0x00 );

            qobj = ( Q3DOBJECT * ) qsym;
        } break;

        case G3DSCENETYPE : {
            G3DSCENE *sce = ( G3DSCENE * ) obj;
            qsce = q3dscene_new ( sce, 0x00, 0x00 );

            qsce->qobjID++;

            qobj = ( Q3DOBJECT * ) qsce;
        } break;

        case G3DLIGHTTYPE : {
            G3DLIGHT *lig = ( G3DLIGHT * ) obj;
            Q3DLIGHT *qlig = q3dlight_new ( lig, 
                                            qsce->qobjID++, 
                                            0x00 );

            /*** qsce is a static variable, so this recursive function ***/
            /*** expects to have gone through a SCENE first ***/
            q3dscene_addLight ( qsce, qlig );

            qobj = ( Q3DOBJECT * ) qlig;
        } break;

        default : {
            qobj = q3dobject_new ( obj, 
                                   qsce->qobjID++, 
                                   0x00 );
        } break;
    }

    while ( ltmpchildren ) {
        G3DOBJECT *child  = ( G3DOBJECT * ) ltmpchildren->data;
        Q3DOBJECT *qchild = q3dobject_import_r ( child, frame );

        q3dobject_addChild ( qobj, qchild );

        ltmpchildren = ltmpchildren->next;
    }

    return qobj;
}
