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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
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
#include <g3d.h>

/******************************************************************************/
void g3dtorus_boundaries ( G3DPRIMITIVE *pri ) {
    TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;
    G3DOBJECT *obj = ( G3DOBJECT * ) pri;
    float radius = ( tds->extrad + tds->intrad );

    switch ( tds->orientation ) {
        case PLANEXY :
            obj->bbox.xmax =  radius;
            obj->bbox.xmin = -radius;
            obj->bbox.ymax =  radius;
            obj->bbox.ymin = -radius;
            obj->bbox.zmax =  tds->intrad;
            obj->bbox.zmin = -tds->intrad;
        break;

        case PLANEYZ :
            obj->bbox.ymax =  radius;
            obj->bbox.ymin = -radius;
            obj->bbox.zmax =  radius;
            obj->bbox.zmin = -radius;
            obj->bbox.xmax =  tds->intrad;
            obj->bbox.xmin = -tds->intrad;
        break;

        case PLANEZX :
            obj->bbox.zmax =  radius;
            obj->bbox.zmin = -radius;
            obj->bbox.xmax =  radius;
            obj->bbox.xmin = -radius;
            obj->bbox.ymax =  tds->intrad;
            obj->bbox.ymin = -tds->intrad;
        break;

        default :
        break;
    }
}

/******************************************************************************/
void g3dtorus_build ( G3DPRIMITIVE *pri, int orientation,
                                         int slice, int cap,
                                         float extrad, float intrad ) {
    TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;
    G3DOBJECT *obj = ( G3DOBJECT * ) pri;
    G3DMESH *mes = ( G3DMESH * ) pri;
    float difint = ( ( intrad * 2.0f ) / cap    ),
          difext = ( ( extrad * 2.0f ) / slice  );
    float sliang = 0.0f,
          capang = 0.0f;
    float difsli = ( float ) M_PI * 2.0f / slice,
          difcap = ( float ) M_PI * 2.0f / cap;
    uint32_t nbver = ( slice * cap );
    G3DVERTEX **ver = ( G3DVERTEX ** ) calloc ( nbver, sizeof ( G3DVERTEX * ) );
    uint32_t vid = 0x00;
    G3DFACE *fac;
    int i, j;

    tds->slice  = slice;
    tds->cap    = cap;
    tds->intrad = intrad;
    tds->extrad = extrad;
    tds->orientation = orientation;

    g3dmesh_empty ( mes );

    for ( i = 0x00; i < tds->slice; i++, sliang += difsli ) {
        float capang = 0.0f;

        for ( j = 0x00; j < tds->cap; j++, capang += difcap, vid++ ) {
            float radius = (  extrad + ( cos ( capang ) * intrad ) );
            float posu, posv, posw;
            float noru, norv, norw;
            float cenu, cenv, cenw;
            G3DVECTOR nor, pos;

            posu = ( cos ( sliang ) * radius );
            posv = ( sin ( sliang ) * radius );
            posw = ( sin ( capang ) * intrad );

            cenu = ( cos ( sliang ) * extrad );
            cenv = ( sin ( sliang ) * extrad );
            cenw = 0.0f;

            noru = ( posu - cenu ) / intrad;
            norv = ( posv - cenv ) / intrad;
            norw = ( posw - cenw ) / intrad;

            switch ( tds->orientation ) {
                case TORUSXY :
                    g3dvector_init ( &pos, posu, posv, posw, 1.0f );
                    g3dvector_init ( &nor, noru, norv, norw, 1.0f );
                break;

                case TORUSYZ :
                    g3dvector_init ( &pos, posw, posu, posv, 1.0f );
                    g3dvector_init ( &nor, norw, noru, norv, 1.0f );
                break;

                case TORUSZX :
                    g3dvector_init ( &pos, posv, posw, posu, 1.0f );
                    g3dvector_init ( &nor, norv, norw, noru, 1.0f );
                break;

                default :
                break;
            }

            ver[vid] = g3dvertex_new ( pos.x, pos.y, pos.z );
            g3dvector_init    ( &ver[vid]->nor, nor.x, nor.y, nor.z, 1.0f );
            g3dmesh_addVertex ( mes, ver[vid] );
        }
    }

    for ( i = 0x00; i < tds->slice; i++ ) {
        int n = ( i + 0x01 ) % tds->slice;

        for ( j = 0x00; j < tds->cap; j++ ) {
            int o = ( j + 0x01 ) % tds->cap;
            G3DFACE *fac = g3dquad_new ( ver[(n*tds->cap)+j],
                                         ver[(n*tds->cap)+o],
                                         ver[(i*tds->cap)+o],
                                         ver[(i*tds->cap)+j] );

            g3dmesh_addFace ( mes, fac );
        }
    }

    g3dtorus_boundaries  ( pri );

    free ( ver );
}

/******************************************************************************/
G3DPRIMITIVE *g3dtorus_new ( uint32_t id, char *name,
                                          int orientation,
                                          int slice,
                                          int cap,
                                          float extrad,
                                          float intrad ) {
    TORUSDATASTRUCT *data;
    G3DPRIMITIVE *pri;

    data = ( TORUSDATASTRUCT * ) calloc ( 0x01, sizeof ( TORUSDATASTRUCT ) );

    if ( data == NULL ) {
        fprintf ( stderr, "g3dtorus_new: plane calloc failed\n" );

        return NULL;
    }

    pri = g3dprimitive_new ( id, name, data, sizeof ( TORUSDATASTRUCT ) );

    ((G3DOBJECT*)pri)->type = G3DTORUSTYPE;

    g3dtorus_build ( pri, orientation, slice, cap, extrad, intrad );


    return pri;
}

