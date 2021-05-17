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
void q3dmesh_free ( Q3DMESH *qmes ) {

}

/******************************************************************************/
void q3dmesh_intersect ( Q3DMESH *qmes,
                         Q3DRAY  *qray, 
                         float    frame,
                         uint64_t query_flags,
                         uint64_t render_flags ) {
    Q3DVERTEXSET *qverset = q3dmesh_getVertexSet ( qmes, frame );

    q3doctree_intersect_r ( qverset->qoct, 
                            qray,
                            qmes->qtri,
                            qverset->qver,
                            query_flags,
                            render_flags );
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
Q3DVERTEX *q3dmesh_getTriangles ( Q3DMESH *qmes ) {
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
                                  uint32_t nbqtri ) {
    G3DMESH *mes      = ( G3DMESH * ) qobject_getObject ( qmes );
    uint32_t nbuvmap  = g3dmesh_getUVMapCount ( mes );
    uint32_t nbquvs   = ( nbqtri * nbuvmap );
    uint32_t memsize  = ( nbqver * sizeof ( Q3DVERTEX   ) ) +
                        ( nbqtri * sizeof ( Q3DTRIANGLE ) ) + 
                        ( nbquvs * sizeof ( Q3DUVSET    ) );
    uint32_t i;

    qmes->nbqver = nbqver;

    q3dmesh_addVertexSet ( qmes, frame );

    qmes->qtri = ( Q3DTRIANGLE * ) calloc ( nbqtri, sizeof ( Q3DTRIANGLE ) );

    if ( qmes->qtri == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return;
    }

    if ( nbquvs ) {
        qmes->quvs = ( Q3DUVSET * ) calloc ( nbquvs, sizeof ( Q3DUVSET ) );

        for ( i = 0x00; i < nbqtri; i++ ) {
            qmes->qtri[i].quvs = &qmes->quvs[(i * nbuvmap)];
        }
    }

    qmes->curtri = qmes->qtri;

/*#ifdef VERBOSE*/
    printf ( "q3dvertex count: %lu\n", nbqver );
    printf ( "q3dface   count: %lu\n", nbqtri );
    printf ( "q3duvset  count: %lu\n", nbquvs );

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

    qmes->nbqtri = nbqtri;
    qmes->nbqver = nbqver;
    qmes->nbquvs = nbquvs;
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

    q3dmesh_allocArrays ( qmes, frame, nbver, nbtri + ( nbqua * 0x02 ) );
}

/******************************************************************************/
static void Dump ( G3DFACE *fac, 
                   void    *data ) {
    uint32_t polyCount = ( fac->nbver == 0x03 ) 0x01 : 0x02;
    Q3DDUMP *qdump = ( Q3DDUMP * ) data;
    Q3DMESH *qmes  = qdump->qmes;
    uint32_t i, j;
    Q3DVERTEXSET *qverset = q3dmesh_getVertexSet ( qmes, qdump->frame );

    /*** each QUAD gives birth to 2 triangles ***/
    for ( i = 0x00; i < polyCount; i++ ) {
        static uint32_t idx[0x02][0x03] = { { 0x00, 0x01, 0x02 },
                                            { 0x02, 0x03, 0x00 } };
        LIST *ltmpuvs = fac->luvs;
        uint32_t qverID0 = fac->ver[idx[i][0x00]]->id,
                 qverID1 = fac->ver[idx[i][0x01]]->id,
                 qverID2 = fac->ver[idx[i][0x02]]->id;
        Q3DVERTEX *qver[0x03] = { &qverset->qver[qverID[0x00]],
                                  &qverset->qver[qverID[0x01]],
                                  &qverset->qver[qverID[0x02]] };
        float length;

        qmes->curtri->textureSlots = fac->textureSlots;

        for ( j = 0x00; j < 0x03; j++ ) {
            G3DVERTEX *ver = fac->ver[idx[i][j]];
            float scalar = fabs ( g3dvector_scalar ( &ver->nor,
                                                     &fac->nor ) );
            float gouraudScalarLimit = mes->gouraudScalarLimit;
            G3DVECTOR *pos = ( ver->flags & VERTEXSKINNED ) ? &ver->skn :
                                                              &ver->pos,
                      *nor = ( scalar < gouraudScalarLimit ) ? &fac->nor :
                                                               &ver->nor;
            Q3DVERTEX *curqver = &qmes->qver[qmes->curfac->qverID[j]];

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
                           qdump->qver,
                           qverID0,
                           qverID1,
                           qverID2 );

        qmes->curtri->textureSlots = fac->textureSlots;

        qmes->curtri->flags |= ( polyCount == 0x01 ) ? QTRIANGLEFROMTRIANGLE : 
                                                       QTRIANGLEFROMQUAD;

        /*** this flag helps us to rebuild a quad from a RFACE and ***/
        /*** the face that follows in the array. Used for outlining ***/
        if ( i == 0x00 ) qmes->curtri->flags |= QTRIANGLEFROMQUADONE;
        if ( i == 0x01 ) qmes->curtri->flags |= QTRIANGLEFROMQUADTWO;

        /* 
         * no need uv coords when using, let's say,
         * the vector motion blur
         */
        if ( ( qdump->dump_flags & GEOMETRYONLY ) == 0x00 ) {
            while ( ltmpuvs ) {
                G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;
                uint32_t uvmapID = uvs->map->mapID;

                qmes->curtri->quvs[uvmapID].uv[0x00].u = uvs->veruv[verID[0x00]].u;
                qmes->curtri->quvs[uvmapID].uv[0x00].v = uvs->veruv[verID[0x00]].v;

                qmes->curtri->quvs[uvmapID].uv[0x01].u = uvs->veruv[verID[0x01]].u;
                qmes->curtri->quvs[uvmapID].uv[0x01].v = uvs->veruv[verID[0x01]].v;

                qmes->curtri->quvs[uvmapID].uv[0x02].u = uvs->veruv[verID[0x02]].u;
                qmes->curtri->quvs[uvmapID].uv[0x02].v = uvs->veruv[verID[0x02]].v;

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
                    float    frame,
                    uint32_t octreeCapacity ) {
    Q3DDUMP qdump = { .qmes  = qmes,
                      .frame = frame };

    q3dobject_init ( qmes,
                     mes,
                     id,
                     object_flags,
                     q3dmesh_free,
                     q3dmesh_intersect );

    g3dmesh_dump ( mes,
                   Alloc,
                   Dump,
                  &rdump,
                   /*dump_flags*/0x00 );

    if ( qmes->nbqtri ) {
        Q3DVERTEXSET *qverset = q3dmesh_getVertexSet ( qmes, frame );

        /*q3dvertexset_buildBoundingBox ( qverset );*/

        q3dvertexset_buildOctree      ( qverset,
                                        qmes->qtri,
                                        qmes->nbqtri,
                                        qmes->qver,
                                        octreeCapacity );
    }
}

/******************************************************************************/
Q3DMESH *q3dmesh_new ( G3DMESH *mes,
                       uint32_t id,
                       uint64_t object_flags,
                       float    frame,
                       uint32_t octreeCapacity ) {
    Q3DMESH *qmes = ( Q3DMESH * ) calloc ( 0x01, sizeof ( Q3DMESH ) );

    if ( qmes == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    q3dmesh_init ( qmes, mes, id, object_flags, frame, octreeCapacity );


    return qmes;
}
