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
#include <g3dexportv3.h>

/******************************************************************************/
void g3dmesh_getMin ( G3DMESH *mes, float *xmin, 
                                    float *ymin, 
                                    float *zmin ) {
    G3DOBJECT *objmes = ( G3DOBJECT * ) mes;
    G3DVECTOR locmin = { objmes->bbox.xmin, 
                         objmes->bbox.ymin,
                         objmes->bbox.zmin }, glomin,
              locmax = { objmes->bbox.xmax, 
                         objmes->bbox.ymax,
                         objmes->bbox.zmax }, glomax;

    g3dvector_matrix ( &locmin, objmes->wmatrix, &glomin );
    g3dvector_matrix ( &locmax, objmes->wmatrix, &glomax );

    if ( glomin.x < (*xmin) ) (*xmin) = glomin.x;
    if ( glomin.y < (*ymin) ) (*ymin) = glomin.y;
    if ( glomin.z < (*zmin) ) (*zmin) = glomin.z;

    if ( glomax.x < (*xmin) ) (*xmin) = glomax.x;
    if ( glomax.y < (*ymin) ) (*ymin) = glomax.y;
    if ( glomax.z < (*zmin) ) (*zmin) = glomax.z;
}

/******************************************************************************/
void g3dobject_getMin_r ( G3DOBJECT *obj, float *xmin, 
                                          float *ymin, 
                                          float *zmin ) {
    LIST *ltmpobj = obj->lchildren;


    if ( obj->type == G3DMESHTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3dmesh_getMin ( mes, xmin, ymin, zmin );
    }

    while ( ltmpobj ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpobj->data;

        g3dobject_getMin_r ( child, xmin, ymin, zmin );

        ltmpobj = ltmpobj->next;
    }
}

/******************************************************************************/
void g3dmesh_exportStlA ( G3DMESH *mes, float xmin, 
                                        float ymin,
                                        float zmin,
                                        FILE *fdst,
                                        uint32_t save_flags ) {
    G3DOBJECT *objmes = ( G3DOBJECT * ) mes;
    LIST *ltmpver = mes->lver;
    LIST *ltmpfac = mes->lfac;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        uint32_t i, j, index[0x06] = { 0x00, 0x01, 0x02, 0x02, 0x03, 0x00 };
        uint32_t *id = index;
        G3DVECTOR facnor;

        for ( j = 0x00; j < fac->nbver; j += 0x03 ) {
            G3DVECTOR verpos[0x03];
            G3DVECTOR v0v1, v0v2;

            for ( i = 0x00; i < 0x03; i++ ) {
                g3dvector_matrix ( &fac->ver[*(id++)]->pos, objmes->wmatrix, 
                                   &verpos[i] );
            }

            if ( xmin < 0.0f ) {
                verpos[0x00].x = verpos[0x00].x - xmin;
                verpos[0x01].x = verpos[0x01].x - xmin;
                verpos[0x02].x = verpos[0x02].x - xmin;
            }

            if ( ymin < 0.0f ) {
                verpos[0x00].y = verpos[0x00].y - ymin;
                verpos[0x01].y = verpos[0x01].y - ymin;
                verpos[0x02].y = verpos[0x02].y - ymin;
            }

            if ( zmin < 0.0f ) {
                verpos[0x00].z = verpos[0x00].z - zmin;
                verpos[0x01].z = verpos[0x01].z - zmin;
                verpos[0x02].z = verpos[0x02].z - zmin;
            }

            v0v1.x = verpos[0x01].x - verpos[0x00].x;
            v0v1.y = verpos[0x01].y - verpos[0x00].y;
            v0v1.z = verpos[0x01].z - verpos[0x00].z;

            v0v2.x = verpos[0x02].x - verpos[0x00].x;
            v0v2.y = verpos[0x02].y - verpos[0x00].y;
            v0v2.z = verpos[0x02].z - verpos[0x00].z;

            g3dvector_cross ( &v0v1, &v0v2, &facnor );

            fprintf ( fdst, "facet normal %e %e %e\n", facnor.x, 
                                                       facnor.y, 
                                                       facnor.z );
            fprintf ( fdst, "    outer loop\n" );

            for ( i = 0x00; i < 0x03; i++ ) {
                fprintf ( fdst, "        vertex %e %e %e\n", verpos[i].x, 
                                                             verpos[i].y, 
                                                             verpos[i].z );
            }

            fprintf ( fdst, "    endloop\n" );
            fprintf ( fdst, "endfacet\n" );
        }

        ltmpfac = ltmpfac->next;
    }
}

/******************************************************************************/
static void g3dobject_exportStlA_r ( G3DOBJECT *obj, float xmin, 
                                                     float ymin,
                                                     float zmin,
                                                     FILE *fdst,
                                                     uint32_t save_flags ) {
    LIST *ltmpchildren = obj->lchildren;

    if ( obj->type == G3DMESHTYPE ) {
        g3dmesh_exportStlA ( ( G3DMESH * ) obj, xmin, 
                                                ymin, 
                                                zmin, fdst, save_flags );
    }

    while ( ltmpchildren ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;

        g3dobject_exportStlA_r ( child, xmin,
                                        ymin,
                                        zmin, fdst, save_flags );

        ltmpchildren = ltmpchildren->next;
    }
}

/******************************************************************************/
void g3dscene_exportStlA ( G3DSCENE *sce, const char *filename,
                                          const char *comment,  uint32_t save_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sce;
    float xmin = 0.0f, ymin = 0.0f, zmin = 0.0f;
    FILE *fdst;

    if ( ( fdst = fopen ( filename, "wt" ) ) == NULL ) {
        fprintf ( stderr, "g3dscene_exportStlA: could not open %s\n", filename );

        return;
    }

    /*if ( comment ) fprintf ( fdst, "%s", comment );*/

    fprintf ( fdst, "solid Gloss3d_Scene\n" );

    g3dobject_getMin_r ( ( G3DOBJECT * ) sce, &xmin, &ymin, &zmin );

    g3dobject_exportStlA_r ( ( G3DOBJECT * ) sce, xmin, 
                                                  ymin, 
                                                  zmin, fdst, save_flags );

    fprintf ( fdst, "endsolid Gloss3d_Scene" );

    fclose ( fdst );
}

