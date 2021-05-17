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
static void q3dsymmetry_free ( Q3DSYMMETRY *qsym ) {

}

/******************************************************************************/
static void q3dsymmetry_intersect ( Q3DSYMMETRY *qsym,
                                    Q3DRAY      *qray,
                                    float        frame,
                                    uint64_t     query_flags,
                                    uint64_t     render_flags ) {
    Q3DOBJECT *qobj = ( Q3DOBJECT * ) qsym;
    Q3DRAY symqray;

    memcpy ( &symqray, qray, sizeof ( Q3DRAY ) );

    q3dvector_matrix ( qray->ori, qsym->ISMVX , &symqray.ori );
    q3dvector_matrix ( qray->dir, qsym->TISMVX, &symqray.dir );

    while ( ltmpchildren ) {
        Q3DOBJECT *qchild = ( Q3DOBJECT * ) ltmpchildren->data;

        q3dobject_intersect_r ( qchild, &symqray, frame, render_flags );

        ltmpchildren = ltmpchildren->next;
    }

    qray->color    = symqray.color;
    qray->distance = symqray.distance;
    qray->qobj     = symqray.qobj;
    qray->surface  = symqray.surface;
}

/******************************************************************************/
void q3dsymmetry_init ( Q3DSYMMETRY *qsym, 
                        G3DSYMMETRY *sym,
                        uint32_t     id,
                        uint64_t     object_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sym;
    double TMPX[0x10], ITMPX[0x10];

    q3dobject_init ( qsym,
                     sym,
                     id,
                     flags,
                     q3dsymmetry_free,
                     q3dsymmetry_intersect );

    g3dcore_multMatrix ( obj->lmatrix, sym->smatrix, TMPX );
    g3dcore_invertMatrix ( TMPX, qsym->ISMVX );
    g3dcore_transposeMatrix ( qsym->ISMVX, qsym->TISMVX );
}

/******************************************************************************/
Q3DSYMMETRY *q3dsymmetry_new ( G3DSYMMETRY *sym,
                               uint32_t     id,
                               uint64_t     object_flags ) {
    Q3DSYMMETRY *qsym = ( Q3DSYMMETRY * ) calloc ( 0x01, sizeof ( Q3DSYMMETRY ) );

    if ( qsym == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    q3dsymmetry_init ( qsym, sym, id, flags );


    return qsym;
}
