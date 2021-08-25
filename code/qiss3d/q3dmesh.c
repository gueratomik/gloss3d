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

/******************************* Dump flags ***********************************/
#define GEOMETRYONLY ( 1      )

/******************************************************************************/
typedef struct _Q3DDUMP {
    Q3DMESH *qmes;
    float    frame;
    uint32_t dump_flags;
} Q3DDUMP;

/******************************************************************************/
void q3dmesh_freeVertexSets ( Q3DMESH *qmes ) {
    if ( qmes->nbVertexSet ) {
        uint32_t i;

        for ( i = 0x00; i < qmes->nbVertexSet; i++ ) {
            Q3DVERTEXSET *qverset = &qmes->vertexSet[i];

            q3doctree_free_r ( qverset->qoct );

            if ( qverset->qver ) free ( qverset->qver );
        }

        free ( qmes->vertexSet );
    }
}

/******************************************************************************/
void q3dmesh_free ( Q3DMESH *qmes ) {
    if ( qmes->quvs  ) free ( qmes->quvs );
    if ( qmes->qtri  ) free ( qmes->qtri );

    q3dmesh_freeVertexSets ( qmes );
}

/******************************************************************************/
uint32_t q3dmesh_intersect ( Q3DMESH    *qmes,
                             Q3DRAY     *qray,
                             Q3DSURFACE *discard,
                             uint32_t  (*cond)(Q3DOBJECT *, 
                                               void      *),
                             void       *condData,
                             float       frame,
                             uint64_t    query_flags,
                             uint64_t    render_flags ) {
    Q3DVERTEXSET *qverset;

    /*** Optimize for speed to avoid function call in most cases ***/
    if ( frame == qmes->vertexSet[0x00].frame ) {
        qverset = &qmes->vertexSet[0x00];
    } else {
        qverset = q3dmesh_getVertexSet ( qmes, frame );
    }

    if ( q3doctree_intersect_r ( qverset->qoct, 
                                 qray,
                                 qmes->qtri,
                                &discard->tri,
                                 qverset->qver,
                                 query_flags,
                                 render_flags ) ) {
        qray->isx.qobj = ( Q3DOBJECT * )qmes;

        return 0x01;
    }

    return 0x00;
}

/******************************************************************************/
Q3DVERTEXSET *q3dmesh_getVertexSet ( Q3DMESH *qmes, 
                                     float    frame ) {
    uint32_t i;

    for ( i = 0x00; i < qmes->nbVertexSet; i++ ) {
        if ( qmes->vertexSet[i].frame == frame ) {
            return &qmes->vertexSet[i];
        }
    }

    return NULL;
}

/******************************************************************************/
void q3dmesh_addVertexSet ( Q3DMESH *qmes, 
                            float    frame ) {
    uint32_t structSize  = sizeof ( Q3DVERTEXSET );
    uint32_t vertexSetID = qmes->nbVertexSet;
    uint32_t arraySize   = (++qmes->nbVertexSet) * structSize;

    qmes->vertexSet = ( Q3DVERTEXSET * ) realloc ( qmes->vertexSet, arraySize );

    if ( qmes->nbqver ) {
        Q3DVERTEX *qver = ( Q3DVERTEX * ) calloc ( qmes->nbqver,
                                                   sizeof ( Q3DVERTEX ) );

        qmes->vertexSet[vertexSetID].qver  = qver;
        qmes->vertexSet[vertexSetID].frame = frame;

        qmes->vertexSet[vertexSetID].min.x = FLT_MAX;
        qmes->vertexSet[vertexSetID].min.y = FLT_MAX;
        qmes->vertexSet[vertexSetID].min.z = FLT_MAX;

        qmes->vertexSet[vertexSetID].max.x = FLT_MIN;
        qmes->vertexSet[vertexSetID].max.y = FLT_MIN;
        qmes->vertexSet[vertexSetID].max.z = FLT_MIN;
    }
}

/******************************************************************************/
Q3DVERTEX *q3dmesh_getVertices ( Q3DMESH *qmes, 
                                 float    frame ) {
    Q3DVERTEXSET *qverset = q3dmesh_getVertexSet ( qmes, frame );

    return qverset->qver;
}

/******************************************************************************/
Q3DTRIANGLE *q3dmesh_getTriangles ( Q3DMESH *qmes ) {
    return qmes->qtri;
}

/******************************************************************************/
uint32_t q3dmesh_getTriangleCount ( Q3DMESH *qmes ) {
    return qmes->nbqtri;
}

/******************************************************************************/
static void q3dmesh_allocArrays ( Q3DMESH *qmes,
                                  float    frame,
                                  uint32_t nbqver,
                                  uint32_t nbqtri,
                                  uint32_t dump_flags ) {
    G3DMESH *mes     = ( G3DMESH * ) q3dobject_getObject ( ( Q3DOBJECT * ) qmes );
    uint32_t nbuvmap = g3dmesh_getUVMapCount ( mes );
    uint32_t nbquvs  = ( nbqtri * nbuvmap );
    uint32_t memsize = 0x00;
    uint32_t i;

    qmes->nbqver = nbqver;

    q3dmesh_addVertexSet ( qmes, frame );

    qmes->qtri = ( Q3DTRIANGLE * ) calloc ( nbqtri, sizeof ( Q3DTRIANGLE ) );

    if ( qmes->qtri == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return;
    }

    qmes->nbqtri = nbqtri;

    qmes->curtri = qmes->qtri;

    if ( ( dump_flags & GEOMETRYONLY ) == 0x00 ) {
        if ( nbquvs ) {
            qmes->quvs = ( Q3DUVSET * ) calloc ( nbquvs, sizeof ( Q3DUVSET ) );

            for ( i = 0x00; i < nbqtri; i++ ) {
                qmes->qtri[i].quvs = &qmes->quvs[(i * nbuvmap)];
            }

            qmes->nbquvs = nbquvs;
        }
    }

    memsize = ( qmes->nbqver * sizeof ( Q3DVERTEX   ) ) +
              ( qmes->nbqtri * sizeof ( Q3DTRIANGLE ) ) + 
              ( qmes->nbquvs * sizeof ( Q3DUVSET    ) );

/*#ifdef VERBOSE*/
    printf ( "q3dvertex count: %d\n", qmes->nbqver );
    printf ( "q3dface   count: %d\n", qmes->nbqtri );
    printf ( "q3duvset  count: %d\n", qmes->nbquvs );

    if ( memsize < 1024 ) {
        printf ( "Q3DMESH mem: %d Bytes\n"   ,           memsize              );
    } else if ( memsize < 1048576 ) {
        printf ( "Q3DMESH mem: %.2f KBytes\n", ( float ) memsize / 1024       );
    } else if ( memsize < 1073741824 ) {
        printf ( "Q3DMESH mem: %.2f MBytes\n", ( float ) memsize / 1048576    );
    } else {
        printf ( "Q3DMESH mem: %.2f GBytes\n", ( float ) memsize / 1073741824 );
    }
/*#endif*/
}

/******************************************************************************/
static void Alloc ( uint32_t nbver,
                    uint32_t nbtri, 
                    uint32_t nbqua,
                    uint32_t nbuv,
                    void    *data ) {
    Q3DDUMP *qdump = ( Q3DDUMP * ) data;
    Q3DMESH *qmes  = qdump->qmes;
    float    frame = qdump->frame;

    q3dmesh_allocArrays ( qmes, 
                          frame, 
                          nbver, 
                          nbtri + ( nbqua * 0x02 ), 
                          qdump->dump_flags );
}

/******************************************************************************/
static void Dump ( G3DFACE   *fac, 
                   G3DVECTOR *stkpos,
                   G3DVECTOR *stknor,
                   void      *data ) {
    uint32_t polyCount = ( fac->nbver == 0x03 ) ? 0x01 : 0x02;
    Q3DDUMP *qdump = ( Q3DDUMP * ) data;
    Q3DMESH *qmes  = qdump->qmes;
    G3DMESH *mes = (G3DMESH*) ((Q3DOBJECT*)qmes)->obj;
    uint32_t i, j;
    Q3DVERTEXSET *qverset = q3dmesh_getVertexSet ( qmes, qdump->frame );

    /*** each QUAD gives birth to 2 triangles ***/
    for ( i = 0x00; i < polyCount; i++ ) {
        static uint32_t idx[0x02][0x03] = { { 0x00, 0x01, 0x02 },
                                            { 0x02, 0x03, 0x00 } };
        LIST *ltmpuvs = fac->luvs;
        uint32_t qverID[0x03] = { fac->ver[idx[i][0x00]]->id,
                                  fac->ver[idx[i][0x01]]->id,
                                  fac->ver[idx[i][0x02]]->id };
        Q3DVERTEX *qver[0x03] = { &qverset->qver[qverID[0x00]],
                                  &qverset->qver[qverID[0x01]],
                                  &qverset->qver[qverID[0x02]] };
        float length;

        qmes->curtri->textureSlots = fac->textureSlots;

        for ( j = 0x00; j < 0x03; j++ ) {
            G3DVERTEX *ver = fac->ver[idx[i][j]];
            float scalar = fabs ( g3dvector_scalar ( g3dvertex_getModifiedNormal ( ver, stknor ),
                                                     &fac->nor ) );
            float gouraudScalarLimit = mes->gouraudScalarLimit;
            G3DVECTOR *pos = g3dvertex_getModifiedPosition ( ver, stkpos ),
                      *nor = ( scalar < gouraudScalarLimit ) ? &fac->nor :
                                                               g3dvertex_getModifiedNormal ( ver, stknor );
            Q3DVERTEX *curqver = &qverset->qver[qverID[j]];

            curqver->pos.x = pos->x;
            curqver->pos.y = pos->y;
            curqver->pos.z = pos->z;

            curqver->nor.x = nor->x;
            curqver->nor.y = nor->y;
            curqver->nor.z = nor->z;

            if ( curqver->pos.x < qverset->min.x ) qverset->min.x = curqver->pos.x;
            if ( curqver->pos.y < qverset->min.y ) qverset->min.y = curqver->pos.y;
            if ( curqver->pos.z < qverset->min.z ) qverset->min.z = curqver->pos.z;

            if ( curqver->pos.x > qverset->max.x ) qverset->max.x = curqver->pos.x;
            if ( curqver->pos.y > qverset->max.y ) qverset->max.y = curqver->pos.y;
            if ( curqver->pos.z > qverset->max.z ) qverset->max.z = curqver->pos.z;
        }

        q3dtriangle_init ( qmes->curtri,
                           qverset->qver,
                           qverID[0x00],
                           qverID[0x01],
                           qverID[0x02] );

        qmes->curtri->textureSlots = fac->textureSlots;

        qmes->curtri->flags |= ( polyCount == 0x01 ) ? TRIANGLEFROMTRIANGLE : 
                                                       TRIANGLEFROMQUAD;

        /*** this flag helps us to rebuild a quad from a RFACE and ***/
        /*** the face that follows in the array. Used for outlining ***/
        if ( i == 0x00 ) qmes->curtri->flags |= TRIANGLEFROMQUADONE;
        if ( i == 0x01 ) qmes->curtri->flags |= TRIANGLEFROMQUADTWO;

        /* 
         * no need uv coords when using, let's say,
         * the vector motion blur
         */
        if ( ( qdump->dump_flags & GEOMETRYONLY ) == 0x00 ) {
            while ( ltmpuvs ) {
                G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;
                uint32_t uvmapID = uvs->map->mapID;

                qmes->curtri->quvs[uvmapID].uv[0x00].u = uvs->veruv[idx[i][0x00]].u;
                qmes->curtri->quvs[uvmapID].uv[0x00].v = uvs->veruv[idx[i][0x00]].v;

                qmes->curtri->quvs[uvmapID].uv[0x01].u = uvs->veruv[idx[i][0x01]].u;
                qmes->curtri->quvs[uvmapID].uv[0x01].v = uvs->veruv[idx[i][0x01]].v;

                qmes->curtri->quvs[uvmapID].uv[0x02].u = uvs->veruv[idx[i][0x02]].u;
                qmes->curtri->quvs[uvmapID].uv[0x02].v = uvs->veruv[idx[i][0x02]].v;

                ltmpuvs = ltmpuvs->next;
            }
        }

        qmes->curtri++;
    }
}

/******************************************************************************/
void q3dmesh_init ( Q3DMESH *qmes, 
                    G3DMESH *mes,
                    uint32_t id,
                    uint64_t object_flags,
                    uint32_t dump_flags,
                    float    frame,
                    uint32_t octreeCapacity ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    Q3DDUMP qdump = { .qmes       = qmes,
                      .frame      = frame,
                      .dump_flags = dump_flags };

    if ( ( obj->type == G3DSPHERETYPE   ) &&
         ( obj->flags & SPHEREISPERFECT ) ) {
        q3dobject_init ( ( Q3DOBJECT * ) qmes,
                         ( G3DOBJECT * ) mes,
                         id,
                         object_flags,
        Q3DFREE_CALLBACK(q3dmesh_free),
   Q3DINTERSECT_CALLBACK(q3dsphere_intersect) );
    } else {
        q3dobject_init ( ( Q3DOBJECT * ) qmes,
                         ( G3DOBJECT * ) mes,
                         id,
                         object_flags,
        Q3DFREE_CALLBACK(q3dmesh_free),
   Q3DINTERSECT_CALLBACK(q3dmesh_intersect) );
    }

    g3dmesh_dump ( mes,
                   Alloc,
                   Dump,
                  &qdump,
                   SUBDIVISIONCOMPUTE );

    if ( qmes->nbqtri ) {
        Q3DVERTEXSET *qverset = q3dmesh_getVertexSet ( qmes, frame );

        /* commented out : not needed. The octree does the job ***/
        /*q3dvertexset_buildBoundingBox ( qverset );*/

        q3dvertexset_buildOctree      ( qverset,
                                        qmes->qtri,
                                        qmes->nbqtri,
                                        octreeCapacity );
    }
}

/******************************************************************************/
Q3DMESH *q3dmesh_new ( G3DMESH *mes,
                       uint32_t id,
                       uint64_t object_flags,
                       uint32_t dump_flags,
                       float    frame,
                       uint32_t octreeCapacity ) {
    Q3DMESH *qmes = ( Q3DMESH * ) calloc ( 0x01, sizeof ( Q3DMESH ) );

    if ( qmes == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    q3dmesh_init ( qmes, mes, id, object_flags, 
                                  dump_flags, frame, octreeCapacity );


    return qmes;
}
