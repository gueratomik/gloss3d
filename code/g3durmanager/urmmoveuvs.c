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
#include <g3durmanager.h>

/******************************************************************************/
typedef struct _URMMOVEUVS {
    G3DSCENE *sce;
    G3DUVMAP *uvmap;
    LIST     *luv;
    G3DUV    *olduv;
    G3DUV    *newuv;
} URMMOVEUVS;

/******************************************************************************/
static URMMOVEUVS *urmmoveuvs_new ( G3DSCENE *sce,
                                    G3DUVMAP  *uvmap, 
                                    LIST      *luv,
                                    G3DUV     *olduv,
                                    G3DUV     *newuv ) {
    uint32_t structsize = sizeof ( URMMOVEUVS );

    URMMOVEUVS *muvs = ( URMMOVEUVS * ) calloc ( 0x01, structsize );

    if ( muvs == NULL ) {
        fprintf ( stderr, "urmmoveuvs_new: memory allocation falied\n" );

        return NULL;
    }

    muvs->sce   = sce;
    muvs->uvmap = uvmap;
    muvs->luv   = list_copy ( luv );
    muvs->olduv = olduv;
    muvs->newuv = newuv;


    return muvs;
}

/******************************************************************************/
static void urmmoveuvs_free ( URMMOVEUVS *muvs ) {
    list_free ( &muvs->luv, NULL );

    free ( muvs->olduv );
    free ( muvs->newuv );

    free ( muvs );
}

/******************************************************************************/
static void moveUVs_free ( void    *data,
                           uint32_t commit ) {
    URMMOVEUVS *muvs = ( URMMOVEUVS * ) data;

    urmmoveuvs_free ( muvs );
}

/******************************************************************************/
void moveUVs_undo ( G3DURMANAGER *urm,
                    void         *data,
                    uint64_t      engine_flags ) {
    URMMOVEUVS *muvs = ( URMMOVEUVS * ) data;
    G3DUVMAP *uvmap  = muvs->uvmap;
    G3DMESH *mes = ( G3DMESH * ) ((G3DOBJECT*)uvmap)->parent;
    LIST *ltmpuv = muvs->luv;
    uint32_t i = 0x00;

    while ( ltmpuv ) {
        G3DUV *uv = ( G3DUV * ) ltmpuv->data;

        uv->u = muvs->olduv[i].u;
        uv->v = muvs->olduv[i].v;

        i++;

        ltmpuv = ltmpuv->next;
    }

    /*** update mesh because UVs could concern displacement mapping ***/
    g3dobject_update ( G3DOBJECTCAST(muvs->sce), 0x00, engine_flags );
}

/******************************************************************************/
void moveUVs_redo ( G3DURMANAGER *urm,
                    void         *data,
                    uint64_t      engine_flags ) {
    URMMOVEUVS *muvs = ( URMMOVEUVS * ) data;
    G3DUVMAP *uvmap  = muvs->uvmap;
    G3DMESH *mes = ( G3DMESH * ) ((G3DOBJECT*)uvmap)->parent;
    LIST *ltmpuv = muvs->luv;
    uint32_t i = 0x00;

    while ( ltmpuv ) {
        G3DUV *uv = ( G3DUV * ) ltmpuv->data;

        uv->u = muvs->newuv[i].u;
        uv->v = muvs->newuv[i].v;

        i++;

        ltmpuv = ltmpuv->next;
    }

    /*** update mesh because UVs could concern displacement mapping ***/
    g3dobject_update ( G3DOBJECTCAST(muvs->sce), 0x00, engine_flags );
}

/******************************************************************************/
void g3durm_uvmap_moveUVList ( G3DURMANAGER *urm,
                               G3DSCENE     *sce, 
                               G3DUVMAP     *uvmap, 
                               LIST         *luv,
                               G3DUV        *olduv,
                               G3DUV        *newuv, 
                               uint32_t      return_flags ) {
    URMMOVEUVS *muvs;

    muvs = urmmoveuvs_new ( sce, uvmap, luv, olduv, newuv );

    g3durmanager_push ( urm, moveUVs_undo,
                             moveUVs_redo,
                             moveUVs_free, muvs, return_flags );
}
