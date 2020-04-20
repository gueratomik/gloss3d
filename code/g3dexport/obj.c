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
#include <g3dexport.h>

/******************************************************************************/
void g3dmesh_exportObj ( G3DMESH *mes, FILE *fdst, uint32_t save_flags ) {
    G3DOBJECT *objmes = ( G3DOBJECT * ) mes;
    LIST *ltmpver = mes->lver;
    LIST *ltmpfac = mes->lfac;

    fprintf ( fdst, "o %s\n", objmes->name );

    /*** Get sure IDs are contiguous ***/
    g3dmesh_renumberVertices ( mes );
    g3dmesh_renumberFaces    ( mes );

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        fprintf ( fdst, "v %f %f %f\n", ver->pos.x, ver->pos.y, ver->pos.z );

        ltmpver = ltmpver->next;
    }

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        if ( fac->nbver == 0x03 ) {
            fprintf ( fdst, "f %d %d %d\n"   , ( fac->ver[0x00]->id + 0x01 ),
                                               ( fac->ver[0x01]->id + 0x01 ),
                                               ( fac->ver[0x02]->id + 0x01 ) );
        } else {
            if ( save_flags & EXPORTOBJTRIANGULATE ) {
                fprintf ( fdst, "f %d %d %d\n", ( fac->ver[0x00]->id + 0x01 ),
                                                ( fac->ver[0x01]->id + 0x01 ),
                                                ( fac->ver[0x02]->id + 0x01 ) );

                fprintf ( fdst, "f %d %d %d\n", ( fac->ver[0x02]->id + 0x01 ),
                                                ( fac->ver[0x03]->id + 0x01 ),
                                                ( fac->ver[0x00]->id + 0x01 ) );
            } else {
                fprintf ( fdst, "f %d %d %d %d\n", ( fac->ver[0x00]->id + 0x01 ),
                                                   ( fac->ver[0x01]->id + 0x01 ),
                                                   ( fac->ver[0x02]->id + 0x01 ),
                                                   ( fac->ver[0x03]->id + 0x01 ) );
            }
        }

        ltmpfac = ltmpfac->next;
    }
}

/******************************************************************************/
static void g3dobject_exportObj_r ( G3DOBJECT *obj, FILE *fdst, uint32_t save_flags ) {
    LIST *ltmpchildren = obj->lchildren;

    if ( obj->type == G3DMESHTYPE ) {
        g3dmesh_exportObj ( ( G3DMESH * ) obj, fdst, save_flags );
    }

    while ( ltmpchildren ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;

        g3dobject_exportObj_r ( child, fdst, save_flags );

        ltmpchildren = ltmpchildren->next;
    }
}

/******************************************************************************/
void g3dscene_exportObj ( G3DSCENE *sce, const char *filename,
                                         const char *comment,  uint32_t save_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sce;
    FILE *fdst;

    if ( ( fdst = fopen ( filename, "wt" ) ) == NULL ) {
        fprintf ( stderr, "g3dscene_exportObj: could not open %s\n", filename );

        return;
    }

    if ( comment ) fprintf ( fdst, "%s", comment );

    g3dobject_exportObj_r ( ( G3DOBJECT * ) sce, fdst, save_flags );


    fclose ( fdst );
}


