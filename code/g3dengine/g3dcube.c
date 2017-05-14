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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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
#include <g3dengine/g3dengine.h>

/*******************************************************************************
Holy Sh*** I'd never thought programming a dynamic cube creation function was so
complex ! That's the part that took me like 4 days !
*******************************************************************************/

/******************************************************************************/
void g3dcube_computeNormals ( G3DPRIMITIVE *pri ) {
#ifdef youpitralala
    uint32_t i;

    for ( i = 0x00; i < pri->nbver; i++ ) {
        G3DVERTEX *ver = &pri->ver[i];

        memcpy ( &ver->nor, &ver->pos, sizeof ( G3DVECTOR ) );

        g3dvector_normalize ( &ver->nor, NULL );
    }
#endif
}

/******************************************************************************/
#ifdef G3DCUBEEDGEDESIGN

      4______________5
      /|            /|
     / |           / |
    0__|__________1  |
    |  |          |  |
    |  |          |  |
    |  |          |  |
    |  |          |  |
    |  |          |  |
    |  |7_________|__|6
    |  /          |  /
    | /           | /
    |/____________|/
    3             2

#endif

/******************************************************************************/
void g3dcubedge_free ( G3DCUBEEDGE *cedg ) {
    if ( cedg->ver ) free ( cedg->ver );

    free ( cedg );
}

/*******************************************************************************
beg = the first vertex of our edge 
end = the last vertex of our edge
mem = a memory addr to store interpolated vertices coords.
nbcut = number of interpolated vertices in our edge.
*******************************************************************************/
G3DCUBEEDGE *g3dcubeedge_new ( G3DMESH *mes, G3DVERTEX *beg, 
                                             G3DVERTEX *end, 
                                             uint32_t nbslice,
                                             float norx, 
                                             float nory,
                                             float norz ) {
    uint32_t structsize = sizeof ( G3DCUBEEDGE );
    G3DCUBEEDGE *cedg = ( G3DCUBEEDGE * ) calloc ( 0x01, structsize );
    float posx = beg->pos.x, posy = beg->pos.y, posz = beg->pos.z;
    uint32_t nbcut  = nbslice - 0x01;
    float difx = ( end->pos.x - beg->pos.x ) / nbslice,
          dify = ( end->pos.y - beg->pos.y ) / nbslice,
          difz = ( end->pos.z - beg->pos.z ) / nbslice;
    uint32_t i;

    if ( cedg == NULL ) {
        fprintf ( stderr, "g3dcubeedge_new: memory allocation failed\n" );

        return NULL;
    }

    cedg->ver = ( G3DVERTEX ** ) calloc ( nbslice + 0x02, sizeof ( G3DVERTEX * ) );

    cedg->ver[cedg->nbver++] = beg;

    for ( i = 0x00; i < nbcut; i++ ) {
        G3DVERTEX *ver;

        posx += difx;
        posy += dify;
        posz += difz;

        ver = g3dvertex_new ( posx, posy, posz );

        cedg->ver[cedg->nbver++] = ver;

        g3dmesh_addVertex ( mes, ver );

        ver->nor.x = norx;
        ver->nor.y = nory;
        ver->nor.z = norz;
    }

    cedg->ver[cedg->nbver++] = end;


    return cedg;
}

/******************************************************************************/
void g3dcubeedge_makeFaces ( G3DMESH *mes, G3DCUBEEDGE *edg0,
                                           G3DCUBEEDGE *edg1,
                                           uint32_t clockwise ) {
    uint32_t i;

    for ( i = 0x00; ( i < edg0->nbver - 0x01 ) &&      /*** Skipt last vertex ***/
                    ( i < edg1->nbver - 0x01 ); i++ ) {/*** Skipt last vertex ***/
        uint32_t n = ( i + 0x01 );
        G3DFACE *fac;

        if ( clockwise ) {
            G3DVERTEX *ver[0x04] = { ( G3DVERTEX * ) edg0->ver[n],
                                     ( G3DVERTEX * ) edg1->ver[n],
                                     ( G3DVERTEX * ) edg1->ver[i],
                                     ( G3DVERTEX * ) edg0->ver[i] };

            g3dmesh_addFace ( mes, g3dface_new ( ver, 0x04 ) );
        } else {
            G3DVERTEX *ver[0x04] = { ( G3DVERTEX * ) edg0->ver[i],
                                     ( G3DVERTEX * ) edg1->ver[i],
                                     ( G3DVERTEX * ) edg1->ver[n],
                                     ( G3DVERTEX * ) edg0->ver[n] };

            g3dmesh_addFace ( mes, g3dface_new ( ver, 0x04 ) );
        }
    }
}

/******************************************************************************/
void g3dcubeedge_interpolate ( G3DMESH *mes, G3DCUBEEDGE *edgu0,
                                             G3DCUBEEDGE *edgu1,
                                             G3DCUBEEDGE *edgv0,
                                             G3DCUBEEDGE *edgv1,
                                             uint32_t nbslice, 
                                             uint32_t clockwise,
                                             float norx,
                                             float nory,
                                             float norz,
                                             LIST **lcedg ) {
    G3DCUBEEDGE *cedg0 = edgu0, 
                *cedg1 = NULL;
    uint32_t i;

    for ( i = 0x01; ( i < edgv0->nbver - 0x01 ) &&      /*** Skipt last vertex ***/
                    ( i < edgv1->nbver - 0x01 ); i++ ) {/*** Skipt last vertex ***/
        G3DVERTEX *verv0 = ( G3DVERTEX * ) edgv0->ver[i];
        G3DVERTEX *verv1 = ( G3DVERTEX * ) edgv1->ver[i];

        cedg1 = g3dcubeedge_new ( mes, verv0, verv1, nbslice, norx, nory, norz );

        g3dcubeedge_makeFaces ( mes, cedg0, cedg1, clockwise );

        list_insert ( lcedg, cedg1 ); /*** for later freeing ***/

        /*** prepare next step ***/
        cedg0 = cedg1;
    }

    /*** last row ***/
    g3dcubeedge_makeFaces ( mes, cedg0, edgu1, clockwise );
}

/******************************************************************************/
void g3dcube_build ( G3DPRIMITIVE *pri, int nbslicex, 
                                        int nbslicey, 
                                        int nbslicez,
                                        float radius ) {
    /*** Original cube vertices ( 8 boundary vertices ) ***/
    G3DVERTEX *ver[0x08] = { g3dvertex_new ( -radius,  radius,  radius ),
                             g3dvertex_new (  radius,  radius,  radius ),
                             g3dvertex_new (  radius, -radius,  radius ),
                             g3dvertex_new ( -radius, -radius,  radius ),
                             g3dvertex_new ( -radius,  radius, -radius ),
                             g3dvertex_new (  radius,  radius, -radius ),
                             g3dvertex_new (  radius, -radius, -radius ),
                             g3dvertex_new ( -radius, -radius, -radius ) };
    G3DVECTOR nor[0x08] = { { .x = -0.577f, .y =  0.577f, .z =  0.577f },
                            { .x =  0.577f, .y =  0.577f, .z =  0.577f },
                            { .x =  0.577f, .y = -0.577f, .z =  0.577f },
                            { .x = -0.577f, .y = -0.577f, .z =  0.577f },
                            { .x = -0.577f, .y =  0.577f, .z = -0.577f },
                            { .x =  0.577f, .y =  0.577f, .z = -0.577f },
                            { .x =  0.577f, .y = -0.577f, .z = -0.577f },
                            { .x = -0.577f, .y = -0.577f, .z = -0.577f } };
    /*** In total, 12 edges ***/
    G3DCUBEEDGE *edg[0x0C] = { NULL };
    CUBEDATASTRUCT *data = pri->data;
    G3DOBJECT *obj = ( G3DOBJECT * ) pri;
    G3DMESH *mes = ( G3DMESH * ) pri;
    LIST *lcedg = NULL;
    uint32_t i, j;

    data->nbx    = nbslicex;
    data->nby    = nbslicey;
    data->nbz    = nbslicez;
    data->radius = radius;

    g3dmesh_empty ( mes );

    for ( i = 0x00; i < 0x08; i++ ) {
        g3dmesh_addVertex ( mes, ver[i] );

        g3dvector_init ( &ver[i]->nor, nor[i].x, nor[i].y, nor[i].z, 1.0f );
    }

    /****** 4 X-Axis edges ******/
    edg[0x00] = g3dcubeedge_new ( mes, ver[0x00], ver[0x01], nbslicex, 0.0f, 0.707f, 0.707f );
    edg[0x01] = g3dcubeedge_new ( mes, ver[0x03], ver[0x02], nbslicex, 0.0f,-0.707f, 0.707f );
    edg[0x02] = g3dcubeedge_new ( mes, ver[0x04], ver[0x05], nbslicex, 0.0f, 0.707f,-0.707f );
    edg[0x03] = g3dcubeedge_new ( mes, ver[0x07], ver[0x06], nbslicex, 0.0f,-0.707f,-0.707f );

    /****** 4 Y-Axis edges ******/
    edg[0x04] = g3dcubeedge_new ( mes, ver[0x00], ver[0x03], nbslicey,-0.707f, 0.0f, 0.707f );
    edg[0x05] = g3dcubeedge_new ( mes, ver[0x01], ver[0x02], nbslicey, 0.707f, 0.0f, 0.707f );
    edg[0x06] = g3dcubeedge_new ( mes, ver[0x04], ver[0x07], nbslicey,-0.707f, 0.0f,-0.707f );
    edg[0x07] = g3dcubeedge_new ( mes, ver[0x05], ver[0x06], nbslicey, 0.707f, 0.0f,-0.707f );

    /****** 4 Z-Axis edges ******/
    edg[0x08] = g3dcubeedge_new ( mes, ver[0x00], ver[0x04], nbslicez,-0.707f, 0.707f, 0.0f );
    edg[0x09] = g3dcubeedge_new ( mes, ver[0x01], ver[0x05], nbslicez, 0.707f, 0.707f, 0.0f );
    edg[0x0A] = g3dcubeedge_new ( mes, ver[0x03], ver[0x07], nbslicez,-0.707f,-0.707f, 0.0f );
    edg[0x0B] = g3dcubeedge_new ( mes, ver[0x02], ver[0x06], nbslicez, 0.707f,-0.707f, 0.0f );

    /****** 2 X-Y Plane faces ******/
    g3dcubeedge_interpolate ( mes, edg[0x00], edg[0x01], 
                                   edg[0x04], edg[0x05], nbslicex, 0x00, 0.0f, 0.0f, 1.0f, &lcedg );

    g3dcubeedge_interpolate ( mes, edg[0x02], edg[0x03], 
                                   edg[0x06], edg[0x07], nbslicex, 0x01, 0.0f, 0.0f,-1.0f, &lcedg );

    /****** 2 Y-Z Plane faces ******/
    g3dcubeedge_interpolate ( mes, edg[0x04], edg[0x06], 
                                   edg[0x08], edg[0x0A], nbslicey, 0x00,-1.0f, 0.0f, 0.0f, &lcedg );

    g3dcubeedge_interpolate ( mes, edg[0x05], edg[0x07], 
                                   edg[0x09], edg[0x0B], nbslicey, 0x01, 1.0f, 0.0f, 0.0f, &lcedg );

    /****** 2 Z-X Plane faces ******/
    g3dcubeedge_interpolate ( mes, edg[0x08], edg[0x09], 
                                   edg[0x00], edg[0x02], nbslicez, 0x00, 0.0f, 1.0f, 0.0f, &lcedg );

    g3dcubeedge_interpolate ( mes, edg[0x0A], edg[0x0B], 
                                   edg[0x01], edg[0x03], nbslicez, 0x01, 0.0f,-1.0f, 0.0f, &lcedg );

    /*** Freememory ***/
    list_free ( &lcedg, (void (*)(void *)) g3dcubedge_free );
    for ( i = 0x00; i < 0x0C; i++ ) g3dcubedge_free ( edg[i] );
    /******************/

    obj->bbox.xmax = obj->bbox.ymax = obj->bbox.zmax =  radius;
    obj->bbox.xmin = obj->bbox.ymin = obj->bbox.zmin = -radius;
}

/******************************************************************************/
G3DPRIMITIVE *g3dcube_new ( uint32_t id, char *name,
                                         int nbx, int nby, int nbz,
                                         float radius ) {
    CUBEDATASTRUCT *data;
    G3DPRIMITIVE *pri;

    data = ( CUBEDATASTRUCT * ) calloc ( 0x01, sizeof ( CUBEDATASTRUCT ) );

    if ( data == NULL ) {
        fprintf ( stderr, "g3dcube_new: sphere calloc failed\n" );

        return NULL;
    }

    pri = g3dprimitive_new ( id, name, data, sizeof ( CUBEDATASTRUCT ) );

    ((G3DOBJECT*)pri)->type = G3DCUBETYPE;

    g3dcube_build ( pri, nbx, nby, nbz, radius );

    return pri;
}


