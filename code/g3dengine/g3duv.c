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

/******************************************************************************/
G3DUV *g3duv_new ( G3DUVSET *uvs ) {
    G3DUV *uv = ( G3DUV * ) calloc ( 0x01, sizeof ( G3DUV ) ) ;

    if ( uv == NULL ) {
        fprintf ( stderr, "g3duv_new(): memory allocation failed\n" );

        return NULL;
    }

    uv->set = uvs;


    return uv;
}

/******************************************************************************/
G3DUVSET *g3duvset_new ( G3DUVMAP *map ) {
    G3DUVSET *uvs = ( G3DUVSET * ) calloc ( 0x01, sizeof ( G3DUVSET ) );

    if ( uvs == NULL ) {
        fprintf ( stderr, "g3duvset_new(): memory allocation failed\n" );

        return NULL; 
    }

    uvs->map = map;


    return uvs;
}

/******************************************************************************/
void g3duvmap_fix   ( G3DUVMAP *map ) {
    G3DOBJECT *objmap  = ( G3DOBJECT * ) map;

    objmap->flags |= UVMAPFIXED;
}

/******************************************************************************/
void g3duvmap_unfix ( G3DUVMAP *map ) {
    G3DOBJECT *objmap  = ( G3DOBJECT * ) map;

    objmap->flags &= (~UVMAPFIXED);
}

/******************************************************************************/
/*** Prepares the mid and center UV coordinates, used when subdividing ***/
void g3duvset_subdivide ( G3DUVSET *uvs, G3DFACE *fac ) {
    float cenu = 0.0f, cenv = 0.0f; /*** face centr UV ***/
    uint32_t i;

    /*** edge mid-point UV ***/
    for ( i = 0x00; i < fac->nbver; i++ ) {
        uint32_t n = ( i + 0x01 ) % fac->nbver;

        uvs->veruv[i].set = uvs;

        uvs->miduv[i].set = uvs;
        uvs->miduv[i].u   = ( uvs->veruv[i].u + uvs->veruv[n].u ) * 0.5f;
        uvs->miduv[i].v   = ( uvs->veruv[i].v + uvs->veruv[n].v ) * 0.5f;

        cenu += uvs->veruv[i].u;
        cenv += uvs->veruv[i].v;
    }

    /*** face center vertex UV. Hidden to the user, useful for subdivs ***/
    uvs->cenuv.set = uvs;
    uvs->cenuv.u   = cenu / fac->nbver;
    uvs->cenuv.v   = cenv / fac->nbver;


    /*printf ( "%s %f %f\n", __func__, uvs->cenuv.u, uvs->cenuv.v );
    for ( i = 0x00; i < fac->nbver; i++ ) {
        printf ( "%s %f %f\n", __func__, uvs->veruv[i].u, uvs->veruv[i].v );
        printf ( "%s %f %f\n", __func__, uvs->miduv[i].u, uvs->miduv[i].v );
    }*/
}

/******************************************************************************/
/*** Map a face when the UVMAP is fixed. This implies that the face is ***/
/*** surrounded by faces whose UVSets are set ***/
void g3duvmap_insertFace ( G3DUVMAP *map, G3DFACE *fac ) {
    G3DOBJECT *objmap  = ( G3DOBJECT * ) map;
    G3DUVSET *uvs;
    uint32_t i;
    uint32_t adduvs = 0x00;

    /*** We have to check the uvset does not exist because if we undo/redo, ***/
    /*** this function will be called again ***/
    if ( ( uvs = g3dface_getUVSet ( fac, map ) ) == NULL ) {
        uvs = g3duvset_new ( map );

        adduvs = 0x01;
    }

    /*** Dont project if UVMAP is baked, average using the neighbour uvs ***/
    if ( objmap->flags & UVMAPFIXED ) {
        for ( i = 0x00; i < fac->nbver; i++ ) {
            float u, v;

            g3dvertex_getAverageUV ( fac->ver[i], map, &u, &v );

            uvs->veruv[i].u   = u;
            uvs->veruv[i].v   = v;
        }

        /*** Prepare the subdivided UV coords **/
        g3duvset_subdivide ( uvs, fac );

        /*** This must be called after g3dvertex_getAverageUV(), otherwise ***/
        /*** UVS structure values is going to be part of the averaging !   ***/
        if ( adduvs ) g3dface_addUVSet ( fac, uvs );
    } else {
        if ( adduvs ) g3dface_addUVSet ( fac, uvs );

        g3duvmap_mapFace ( map, fac );
    }
}

/******************************************************************************/
/* this is done in realtime, hence called directly by g3dface_bindMaterials */
void g3duvset_mapFaceWithBackgroundProjection ( G3DUVSET *uvs, 
                                                G3DFACE  *fac,
                                                uint32_t  engine_flags ) {
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
    uint32_t renderRectangleWidth  = sysinfo->renderRectangle[0x02].x -
                                     sysinfo->renderRectangle[0x00].x,
             renderRectangleHeight = sysinfo->renderRectangle[0x02].y -
                                     sysinfo->renderRectangle[0x00].y;
    double MVX[0x10], PJX[0x10];
    int    VPX[0x04] = { 0 }, i;

    glGetDoublev  ( GL_MODELVIEW_MATRIX , MVX );
    glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
    glGetIntegerv ( GL_VIEWPORT         , VPX );

    for ( i = 0x00; i < fac->nbver; i++ ) {
        G3DVERTEX *ver = fac->ver[i];
        double winX, winY, winZ;
        G3DVECTOR verwpos;
        G3DVECTOR locpos;

        /*
         * we could use the VPX and PJX of the final render camera
         * but we don't have access to the current camera. So I use
         * this tricks.
         */
        gluProject ( ver->pos.x, 
                     ver->pos.y, 
                     ver->pos.z,
                     MVX,
                     PJX,
                     VPX, 
                    &winX,
                    &winY,
                    &winZ );

        winX =             winX - sysinfo->renderRectangle[0x00].x;
        winY = VPX[0x03] - winY - sysinfo->renderRectangle[0x00].y;

        uvs->veruv[i].u = winX / renderRectangleWidth;
        uvs->veruv[i].v = winY / renderRectangleHeight;
    }
}

/******************************************************************************/
void g3duvmap_mapFace ( G3DUVMAP *map, G3DFACE *fac ) {
    G3DUVSET *uvs = g3dface_getUVSet ( fac, map );
    G3DOBJECT *objmap  = ( G3DOBJECT * ) map;
    G3DOBJECT *parent  = objmap->parent;
    float xdiameter = map->pln.xradius * 2.0f,
          ydiameter = map->pln.yradius * 2.0f;
    uint32_t i;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        G3DVERTEX *ver = fac->ver[i];
        G3DVECTOR verwpos;
        G3DVECTOR locpos;

        /*** convert vector position to uvwmap coordinates system ***/
        g3dvector_matrix ( &ver->pos, parent->wmatrix, &verwpos );

        g3dvector_matrix ( &verwpos , objmap->iwmatrix, &locpos );

        switch ( map->projection ) {
            case UVMAPSPHERICAL : {
                G3DVECTOR locnor;

                memcpy ( &locnor, &locpos, sizeof ( G3DVECTOR ) );

                g3dvector_normalize ( &locnor, NULL );

                /*locnor.x = -locnor.x;
                locnor.y = -locnor.y;
                locnor.z = -locnor.z;*/

/*printf("%f %f %f\n", locnor.x, locnor.y, locnor.z );*/
                /*** http://en.wikipedia.org/wiki/UV_mapping#Finding_UV_on_a_sphere ***/
                uvs->veruv[i].u =   ( atan2f( locnor.z, locnor.x ) / ( 2.0f * M_PI ) ) + 0.5f;
                uvs->veruv[i].v = - ( asin  ( locnor.y )           / ( M_PI        ) ) + 0.5f;


                if ( uvs->veruv[i].u < 0.0f ) uvs->veruv[i].u = 0.0f;
                if ( uvs->veruv[i].u > 1.0f ) uvs->veruv[i].u = 1.0f;
                if ( uvs->veruv[i].v < 0.0f ) uvs->veruv[i].v = 0.0f;
                if ( uvs->veruv[i].v > 1.0f ) uvs->veruv[i].v = 1.0f;

/*printf("U: %f V:%f\n", uvs->veruv[i].u, uvs->veruv[i].v );*/
            } break;

            case UVMAPCYLINDRICAL : {
                G3DVECTOR locnor;

                memcpy ( &locnor, &locpos, sizeof ( G3DVECTOR ) );

                g3dvector_normalize ( &locnor, NULL );

                /*** http://en.wikipedia.org/wiki/UV_mapping#Finding_UV_on_a_sphere ***/
                uvs->veruv[i].u = ( atan2f( locnor.z, locnor.x ) / ( 2.0f * M_PI ) ) + 0.5f;
                uvs->veruv[i].v = ( locpos.y + map->pln.yradius ) / ydiameter;
            } break;

            case UVMAPFLAT :
            default :
                uvs->veruv[i].u = ( locpos.x + map->pln.xradius ) / xdiameter;
                uvs->veruv[i].v = ( locpos.y + map->pln.yradius ) / ydiameter;
            break;
        }
    }

    /*** Prepare the subdivided UV coords **/
    g3duvset_subdivide ( uvs, fac );
}

/******************************************************************************/
void g3duvmap_adjustFlatProjection ( G3DUVMAP *map ) {
    G3DOBJECT *objmap = ( G3DOBJECT * ) map;
    G3DOBJECT *parent = objmap->parent;
    float parx = ( parent->bbox.xmax + parent->bbox.xmin ) / 2.0f,
          pary = ( parent->bbox.ymax + parent->bbox.ymin ) / 2.0f;

    map->pln.xradius = ( parent->bbox.xmax - parent->bbox.xmin ) / 2.0f;
    map->pln.yradius = ( parent->bbox.ymax - parent->bbox.ymin ) / 2.0f;

    objmap->pos.x = parx;
    objmap->pos.y = pary;

    g3dobject_updateMatrix_r ( objmap, 0x00 );
}

/******************************************************************************/
void g3duvmap_applyProjection ( G3DUVMAP *map ) {
    G3DOBJECT *objmap  = ( G3DOBJECT * ) map;
    G3DOBJECT *parent  = objmap->parent;
    G3DMESH   *parmes  = ( G3DMESH * ) parent;
    LIST      *ltmpfac = ( map->facgrp ) ? map->facgrp->lfac : parmes->lfac;
    float xdiameter = map->pln.xradius * 2.0f,
          ydiameter = map->pln.yradius * 2.0f;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        /*** This function assigns a UVSet to the face if needed and ***/
        /*** maps the UV coordinates to the face ***/
        g3duvmap_insertFace ( map, fac );


        ltmpfac = ltmpfac->next;
    }
}

/******************************************************************************/
uint32_t g3duvmap_draw ( G3DOBJECT *obj, G3DCAMERA *curcam, uint32_t engine_flags ) {
    G3DUVMAP *map = ( G3DUVMAP * ) obj;

    if ( ( engine_flags & VIEWUVWMAP ) == 0x00 ) return 0x00;

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glDisable  ( GL_LIGHTING );
    glDisable ( GL_DEPTH_TEST );
    glColor3ub ( 0xFF, 0xFF, 0x00 );

    switch ( map->projection ) {
        case UVMAPFLAT :
            glBegin ( GL_LINE_LOOP );
            glVertex3f (  map->pln.xradius,  map->pln.yradius, 0.0f );
            glVertex3f (  map->pln.xradius, -map->pln.yradius, 0.0f );
            glVertex3f ( -map->pln.xradius, -map->pln.yradius, 0.0f );
            glVertex3f ( -map->pln.xradius,  map->pln.yradius, 0.0f );
            glEnd   ( );
        break;

        case UVMAPSPHERICAL :
            glColor3ub ( 0x00, 0x00, 0x80 );
            g3dcore_drawXYCircle ( engine_flags );

            glColor3ub ( 0x00, 0x80, 0x00 );
            g3dcore_drawZXCircle ( engine_flags );

            glColor3ub ( 0x80, 0x00, 0x00 );
            g3dcore_drawYZCircle ( engine_flags );
        break;

        case UVMAPCYLINDRICAL :
            glColor3ub ( 0x00, 0x00, 0x80 );
            g3dcore_drawZXCircle ( engine_flags );
        break;

        default :
        break;
    }

    glPopAttrib ( );

    return 0x00;
}

/******************************************************************************/
void g3duvmap_transform ( G3DOBJECT *obj, uint32_t flags ) {
    G3DUVMAP *map = ( G3DUVMAP * ) obj;

    g3duvmap_applyProjection ( map );
}

/******************************************************************************/
void g3duvmap_init ( G3DUVMAP *map, char *name, uint32_t policy,
                                                uint32_t projection ) {
    G3DOBJECT *objmap = ( G3DOBJECT * ) map;

    g3dobject_init ( objmap, G3DUVMAPTYPE, 0x00, name, DRAWBEFORECHILDREN,
                                                       g3duvmap_draw,
                                                       g3duvmap_free,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL,
                                                       NULL );

    objmap->transform = g3duvmap_transform;

    /*obj->copy = g3dprimitive_copy;*/

    map->projection = projection;
    map->policy     = policy;
}

/******************************************************************************/
G3DUVMAP *g3duvmap_new ( char *name, uint32_t policy,
                                     uint32_t projection ) {
    G3DUVMAP *map = ( G3DUVMAP * ) calloc ( 0x01, sizeof ( G3DUVMAP ) );

    if ( map == NULL ) {
        fprintf ( stderr, "g3duvmap_new(): memory allocation failed\n" );

        return NULL; 
    }

    g3duvmap_init ( map, name, policy, projection );


    return map;
}

/******************************************************************************/
void g3duvmap_free ( G3DOBJECT *obj ) {
    G3DUVMAP *map = ( G3DUVMAP * ) obj;
}

/******************************************************************************/
void g3duvmap_addMaterial ( G3DUVMAP *map, G3DMATERIAL *mat ) {

    list_insert ( &map->lmat, mat );

    map->nbmat++;
}

/******************************************************************************/
void g3duvmap_removeMaterial ( G3DUVMAP *map, G3DMATERIAL *mat ) {
    list_remove ( &map->lmat, mat );

    map->nbmat--;
}
