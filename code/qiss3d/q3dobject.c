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

    free ( qobj );
}

/******************************************************************************/
void q3dobject_free_r ( Q3DOBJECT *qobj ) {
    LIST *ltmpchildren = qobj->lchildren;

    if ( qobj->obj->name ) {
        printf ( "Freeing render memory for:%s\n", qobj->obj->name );
    }

    if ( qobj->free ) qobj->free ( qobj );

    while ( ltmpchildren ) {
        Q3DOBJECT *qchild  = ( Q3DOBJECT * ) ltmpchildren->data;

        q3dobject_free_r ( qchild );

        ltmpchildren = ltmpchildren->next;
    }

    free ( qobj );
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
                                              uint32_t  (*cond)(Q3DOBJECT *, 
                                                                void      *),
                                              void       *condData,
                                              float      frame,
                                              uint64_t   query_flags,
                                              uint64_t   render_flags ) {
    return 0x00;
}

/******************************************************************************/
void q3dcore_buildLocalQRay ( Q3DRAY *qray,
                              double *IMVX,
                              Q3DRAY *locqray ) {
    Q3DVECTOR3F dest, locdest;

    memcpy ( locqray, qray, sizeof ( Q3DRAY ) );

    q3dvector3f_matrix ( &qray->src, IMVX, &locqray->src );

    /*** NOTE: We could use the transpose inverse matrix to transform the   ***/
    /*** direction vector, BUT it would not work for non-uniform scaled     ***/
    /*** matrices, so we compute an arbitrary destination point, we find    ***/
    /*** its local position, we deduce the direction vector from the source ***/
    /*** and the destination. It's as simple as that.                       ***/
    dest.x = qray->src.x + ( qray->dir.x );
    dest.y = qray->src.y + ( qray->dir.y ); 
    dest.z = qray->src.z + ( qray->dir.z );

    q3dvector3f_matrix ( &dest, IMVX, &locdest );

    locqray->dir.x = locdest.x - locqray->src.x;
    locqray->dir.y = locdest.y - locqray->src.y;
    locqray->dir.z = locdest.z - locqray->src.z;
}

/******************************************************************************/
uint32_t q3dobject_intersect_r ( Q3DOBJECT  *qobj,
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
    Q3DVECTOR3F src, dir;

    q3dcore_buildLocalQRay ( qray, qobj->IMVX, &locqray );

    if ( ( cond == NULL ) || cond ( qobj, condData ) ) {
        if ( qobj->intersect ) {
            hit += qobj->intersect ( qobj, 
                                    &locqray, 
                                     discard,
                                     cond,
                                     condData,
                                     frame,
                                     query_flags,
                                     render_flags );
        }
    }

    while ( ltmpchildren ) {
        Q3DOBJECT *qchild = ( Q3DOBJECT * ) ltmpchildren->data;

        hit += q3dobject_intersect_r ( qchild, 
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

    if ( hit ) {
        qray->color       = locqray.color;

        qray->ratio[0x00] = locqray.ratio[0x00];
        qray->ratio[0x01] = locqray.ratio[0x01];
        qray->ratio[0x02] = locqray.ratio[0x02];

        qray->isx.qobj    = locqray.isx.qobj;
        qray->isx.qsur    = locqray.isx.qsur;

        q3dvector3f_matrix ( &locqray.isx.src, qobj->obj->lmatrix, &qray->isx.src );
        q3dvector3f_matrix ( &locqray.isx.dir, qobj->TIMVX       , &qray->isx.dir );

        q3dvector3f_normalize ( &qray->isx.dir, NULL );

        qray->distance = locqray.distance;
    }
    

    return ( hit ) ? 0x01 : 0x00;
}

/******************************************************************************/
G3DOBJECT *q3dobject_getObject ( Q3DOBJECT *qobj ) {
    return qobj->obj;
}

/******************************************************************************/
Q3DOBJECT *q3dobject_getByObject_r ( Q3DOBJECT *qobj, 
                                     G3DOBJECT *obj ) {


    LIST *ltmpqobj = qobj->lchildren;

    if ( qobj->obj == obj ) return qobj;

    while ( ltmpqobj ) {
        Q3DOBJECT *qkid = ( Q3DOBJECT * ) ltmpqobj->data;
        Q3DOBJECT *ret = q3dobject_getByObject_r ( qkid, obj );

        if ( ret ) return ret;

        ltmpqobj = ltmpqobj->next;
    }

    return NULL;
}

/******************************************************************************/
void q3dobject_init ( Q3DOBJECT *qobj,
                      G3DOBJECT *obj,
                      uint32_t   id,
                      uint64_t   flags,
                      void     (*Free)      ( Q3DOBJECT * ),
                      uint32_t (*Intersect) ( Q3DOBJECT *obj, 
                                              Q3DRAY    *ray,
                                              uint32_t (*cond)(Q3DOBJECT*,void*),
                                              void      *condData,
                                              float      frame,
                                              uint64_t   query_flags,
                                              uint64_t   render_flags ) ) {
    qobj->obj   = obj;
    qobj->id    = id;
    qobj->flags = flags;

    qobj->free      = Free;
    qobj->intersect = Intersect;

    g3dcore_invertMatrix    ( obj->lmatrix, qobj->IMVX );
    g3dcore_transposeMatrix ( qobj->IMVX, qobj->TIMVX );
    /*** When a point is multiplied by a matrix, we multiply its normal ***/
    /*** by the transpose inverse. Here, the matrix is already the inverse ***/
    /*** hence the inverse of the inverse is the matrix itself. We just ***/
    /*** transpose it. ***/
    g3dcore_transposeMatrix ( obj->lmatrix, qobj->TMVX );
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
                                float      frame,
                                uint32_t   importFlags ) {
    LIST *ltmpchildren = obj->lchildren;
    static Q3DSCENE *qsce;
    Q3DOBJECT *qobj;

    if ( ( obj->flags & OBJECTINACTIVE ) == 0x00 ) {
        switch ( obj->type ) {
            case G3DSPHERETYPE   :
            case G3DCUBETYPE     :
            case G3DPLANETYPE    :
            case G3DTORUSTYPE    :
            case G3DCYLINDERTYPE :
            case G3DTUBETYPE     :
            case G3DSPLINEREVOLVERTYPE :
            case G3DMESHTYPE     : {
                G3DMESH *mes = ( G3DMESH * ) obj;
                Q3DMESH *qmes = q3dmesh_new ( mes, 
                                              qsce->qobjID++,
                                              0x00,
                                              importFlags,
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

            case G3DINSTANCETYPE : {
                G3DINSTANCE *ins = ( G3DINSTANCE * ) obj;
                Q3DINSTANCE *qins = q3dinstance_new ( ins, 
                                                      qsce->qobjID++,
                                                      0x00 );

                qobj = ( Q3DOBJECT * ) qins;
            } break;

            case G3DPARTICLEEMITTERTYPE : {
                G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) obj;
                Q3DPARTICLEEMITTER *qpem = q3dparticleemitter_new ( pem, 
                                                                    qsce->qobjID++,
                                                                    0x00 );

                qobj = ( Q3DOBJECT * ) qpem;
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
    } else {
        qobj = q3dobject_new ( obj, 
                               qsce->qobjID++, 
                               0x00 );
    }

    while ( ltmpchildren ) {
        G3DOBJECT *child  = ( G3DOBJECT * ) ltmpchildren->data;
        Q3DOBJECT *qchild = q3dobject_import_r ( child, frame, importFlags );

        q3dobject_addChild ( qobj, qchild );

        ltmpchildren = ltmpchildren->next;
    }

    return qobj;
}
