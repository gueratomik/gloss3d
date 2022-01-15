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
static void q3dinstance_import ( Q3DINSTANCE *qins, 
                                 Q3DSCENE    *qsce ) {
    G3DINSTANCE *ins = ( G3DINSTANCE * ) qins->qobj.obj;

    qins->qref = q3dscene_getByObject ( qsce, ins->ref );
}

/******************************************************************************/
static void q3dinstance_free ( Q3DINSTANCE *qins ) {

}

/******************************************************************************/
static uint32_t q3dinstance_intersect ( Q3DINSTANCE *qins,
                                        Q3DRAY      *qray,
                                        Q3DSURFACE  *discard,
                                        uint32_t   (*cond)(Q3DOBJECT*,void *),
                                        void        *condData,
                                        float        frame,
                                        uint64_t     query_flags,
                                        uint64_t     render_flags ) {
    Q3DOBJECT *qobjins = ( Q3DOBJECT * ) qins;
    G3DINSTANCE *ins = ( G3DINSTANCE * ) q3dobject_getObject ( qobjins );
    uint32_t hit = 0x00;

    if ( qins->qref ) {
        if ( qins->qref->intersect ) {
            Q3DRAY insqray;

            memcpy ( &insqray, qray, sizeof ( Q3DRAY ) );

            if ( ((G3DOBJECT*)ins)->flags & INSTANCEMIRRORED ) {
                q3dvector3f_matrix ( &qray->src, qins->ISMVX, &insqray.src );
                q3dvector3f_matrix ( &qray->dir, qins->TSMVX, &insqray.dir );
            }

            hit = qins->qref->intersect ( qins->qref, 
                                         &insqray, 
                                          discard, 
                                          cond, 
                                          condData, 
                                          frame, 
                                          query_flags, 
                                          render_flags );

            qray->flags   |= insqray.flags;
            qray->color    = insqray.color;
            qray->distance = insqray.distance;

            qray->ratio[0x00] = insqray.ratio[0x00];
            qray->ratio[0x01] = insqray.ratio[0x01];
            qray->ratio[0x02] = insqray.ratio[0x02];

            if ( hit ) {
                qray->isx.qobj   = insqray.isx.qobj;
                qray->isx.qsur   = insqray.isx.qsur;

                memcpy ( &qray->isx.src, &insqray.isx.src, sizeof ( Q3DVECTOR3F ) );
                memcpy ( &qray->isx.dir, &insqray.isx.dir, sizeof ( Q3DVECTOR3F ) );

                if ( ((G3DOBJECT*)ins)->flags & INSTANCEMIRRORED ) {
                    q3dvector3f_matrix ( &insqray.isx.src, ins->smatrix, &qray->isx.src );
                    q3dvector3f_matrix ( &insqray.isx.dir, qins->TISMVX, &qray->isx.dir );
                }
            }
        }
    }

    return ( hit ) ? 0x01 : 0x00;
}

/******************************************************************************/
static void q3dinstance_init ( Q3DINSTANCE *qins, 
                               G3DINSTANCE *ins,
                               uint32_t     id,
                               uint64_t     object_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) ins;
    double TMPX[0x10], ITMPX[0x10];

    q3dobject_init ( ( Q3DOBJECT * ) qins,
                     ( G3DOBJECT * ) ins,
                     id,
                     object_flags,
    Q3DFREE_CALLBACK(q3dinstance_free),
Q3DINTERSECT_CALLBACK(q3dinstance_intersect) );

    ((Q3DOBJECT*)qins)->import = Q3DIMPORT_CALLBACK(q3dinstance_import);

    /*g3dcore_multmatrix ( obj->lmatrix, ins->smatrix, TMPX );*/
    g3dcore_invertMatrix    ( ins->smatrix, qins->ISMVX  );
    g3dcore_transposeMatrix ( qins->ISMVX , qins->TISMVX );

    g3dcore_transposeMatrix ( ins->smatrix, qins->TSMVX );
}

/******************************************************************************/
Q3DINSTANCE *q3dinstance_new ( G3DINSTANCE *ins,
                               uint32_t     id,
                               uint64_t     object_flags ) {
    Q3DINSTANCE *qins = ( Q3DINSTANCE * ) calloc ( 0x01, sizeof ( Q3DINSTANCE ) );

    if ( qins == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    q3dinstance_init ( qins, ins, id, object_flags );


    return qins;
}
