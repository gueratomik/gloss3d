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
#include <g3dengine/g3dengine.h>

/******************************************************************************/
LIST *g3duvset_getUVsFromList ( LIST *luvset ) {
    LIST *ltmpuvset = luvset;
    LIST *luv = NULL;

    while ( ltmpuvset ) {
        G3DUVSET *uvset = ( G3DUVSET * ) ltmpuvset->data;
        uint32_t i;

        for ( i = 0x00; i < uvset->nbuv; i++ ) {
            list_insert ( &luv, &uvset->veruv[i] );
        }

        ltmpuvset = ltmpuvset->next;
    }

    return luv;
}

/******************************************************************************/
void g3duvset_unsetSelected ( G3DUVSET *uvset ) {
    uvset->flags &= ~(UVSETSELECTED);
}

/******************************************************************************/
void g3duvmap_unselectUVSet ( G3DUVMAP *uvmap, G3DUVSET *uvset ) {
    list_remove ( &uvmap->lseluvset, uvset );

    uvset->flags &= ~(UVSETSELECTED);
}

/******************************************************************************/
void g3duvmap_selectUVSet ( G3DUVMAP *uvmap, G3DUVSET *uvset ) {
    list_insert ( &uvmap->lseluvset, uvset ); 

    uvset->flags |= UVSETSELECTED;
}

/******************************************************************************/
void g3duvmap_unselectAllUVSets ( G3DUVMAP *uvmap ) {
    list_free ( &uvmap->lseluvset, LIST_FUNCDATA ( g3duvset_unsetSelected ) );
}

/******************************************************************************/
uint32_t g3duv_copyUVFromList ( LIST *luv, G3DUV **curuv ) {
    uint32_t nbuv = list_count ( luv );
    LIST *ltmpuv = luv;
    int i = 0x00;

    (*curuv) = calloc ( nbuv, sizeof ( G3DUV ) );

    while ( ltmpuv ) {
        G3DUV *uv = ( G3DUV * ) ltmpuv->data;

        (*curuv)[i].u = uv->u;
        (*curuv)[i].v = uv->v;

        i++;

        ltmpuv = ltmpuv->next;
    }

    return nbuv;
}

/******************************************************************************/
void g3duv_unsetSelected ( G3DUV *uv ) {
    uv->flags &= ~(UVSELECTED);
}

/******************************************************************************/
void g3duvmap_unselectUV ( G3DUVMAP *uvmap, G3DUV *uv ) {
    list_remove ( &uvmap->lseluv, uv );

    uv->flags &= ~(UVSELECTED);
}

/******************************************************************************/
void g3duvmap_selectUV ( G3DUVMAP *uvmap, G3DUV *uv ) {
    list_insert ( &uvmap->lseluv, uv ); 

    uv->flags |= UVSELECTED;
}

/******************************************************************************/
void g3duvmap_unselectAllUVs ( G3DUVMAP *uvmap ) {
    list_free ( &uvmap->lseluv, LIST_FUNCDATA ( g3duv_unsetSelected ) );
}

/******************************************************************************/
uint32_t g3duvmap_isFixed ( G3DUVMAP *map ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) map;

    if ( obj->flags & UVMAPFIXED ) return 0x01;

    return 0x00;
}

/******************************************************************************/
uint32_t g3duv_getAverageFromList ( LIST *luv, G3DUV *uvout ) {
    uint32_t nbuv = 0x00;
    LIST *ltmpuv = luv;

    uvout->u = 0.0f;
    uvout->v = 0.0f;

    while ( ltmpuv ) {
        G3DUV *uv = ( G3DUV * ) ltmpuv->data;

        uvout->u += uv->u;
        uvout->v += uv->v;

        ltmpuv = ltmpuv->next;

        nbuv++;
    }

    if ( nbuv ) {
        uvout->u /= nbuv;
        uvout->v /= nbuv;
    }

    return nbuv;
}

/******************************************************************************/
/*G3DUV *g3duv_new ( G3DUVSET *uvs ) {
    G3DUV *uv = ( G3DUV * ) calloc ( 0x01, sizeof ( G3DUV ) ) ;

    if ( uv == NULL ) {
        fprintf ( stderr, "g3duv_new(): memory allocation failed\n" );

        return NULL;
    }

    uv->set = uvs;


    return uv;
}*/

/******************************************************************************/
void g3duvset_free ( G3DUVSET *uvset ) {
    free ( uvset );
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
void g3duvmap_insertFace ( G3DUVMAP *map, G3DMESH *mes, G3DFACE *fac ) {
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

        g3duvmap_mapFace ( map, mes, fac );
    }
}

/******************************************************************************/
/* this is done in realtime, hence called directly by g3dface_bindMaterials */
void g3duvset_mapFaceWithBackgroundProjection ( G3DUVSET *uvs, 
                                                G3DFACE  *fac,
                                                uint64_t engine_flags ) {
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
        /*uvs->veruv[i].q = 0.0f;*/
    }
}

/******************************************************************************/
void g3duvmap_getSphericalUV ( G3DUVMAP  *map, 
                               G3DMESH   *mes, 
                               G3DVECTOR *lpos,
                               float     *u, 
                               float     *v ) {
    G3DOBJECT *objmap  = ( G3DOBJECT * ) map;
    G3DOBJECT *parent  = ( G3DOBJECT * ) mes;
    G3DVECTOR verWldPos; /*** position in mesh world coordinates ***/
    G3DVECTOR verLocPos; /*** position in map  local coordinates ***/
    G3DVECTOR nor;

    g3dvector_matrixf ( lpos      , parent->worldMatrix , &verWldPos );
    g3dvector_matrixf ( &verWldPos, objmap->inverseWorldMatrix, &verLocPos );

    g3dvector_normalize ( &verLocPos, NULL );

    /*** Why did I do that ?? I don't remember ***/
    verLocPos.x = ( float ) ( ( int ) ( verLocPos.x * 10000 ) ) / 10000;
    verLocPos.y = ( float ) ( ( int ) ( verLocPos.y * 10000 ) ) / 10000;
    verLocPos.z = ( float ) ( ( int ) ( verLocPos.z * 10000 ) ) / 10000;


    /*** http://en.wikipedia.org/wiki/UV_mapping#Finding_UV_on_a_sphere ***/
/*
    printf("atan2: %f - z:%f, x:%f\n", atan2f( verLocPos.z, verLocPos.x ), verLocPos.z, verLocPos.x );
*/
    (*u) =   ( atan2f ( verLocPos.z, verLocPos.x ) / ( 2.0f * M_PI ) ) + 0.5f;
    (*v) = - ( asin  ( verLocPos.y )              / ( M_PI        ) ) + 0.5f;

    /*** for some unknown reason, printf prints 1.0f even if (*u) does not ***/
    /*** match the condition below when comparing to 1.0f. So I compare to ***/
    /*** 0.99999999f ***/
    /*if ( (*u) >= 0.999f ) (*u) = 0.0f;
    if ( (*v) >= 0.999f ) (*v) = 0.0f;*/

    /*g3dvector_print (  lpos );*/
/*    g3dvector_print ( &verLocPos );
    printf("u:%f v:%f\n\n", (*u), (*v));*/
/*
    if ( uvs->veruv[i].u < 0.0f ) uvs->veruv[i].u = 0.0f;
    if ( uvs->veruv[i].u > 1.0f ) uvs->veruv[i].u = 1.0f;
    if ( uvs->veruv[i].v < 0.0f ) uvs->veruv[i].v = 0.0f;
    if ( uvs->veruv[i].v > 1.0f ) uvs->veruv[i].v = 1.0f; */

/*printf("U: %f V:%f\n", uvs->veruv[i].u, uvs->veruv[i].v );*/
}

/******************************************************************************/
void g3duvmap_getCylindricalUV ( G3DUVMAP  *map, 
                                 G3DMESH   *mes, 
                                 G3DVECTOR *lpos,
                                 float     *u, 
                                 float     *v ) {
    float xdiameter = map->pln.xradius * 2.0f,
          ydiameter = map->pln.yradius * 2.0f;
    G3DOBJECT *objmap  = ( G3DOBJECT * ) map;
    G3DOBJECT *parent  = ( G3DOBJECT * ) mes;
    G3DVECTOR wpos; /*** position in mesh world coordinates ***/
    G3DVECTOR mpos; /*** position in map  local coordinates ***/

    g3dvector_matrixf (  lpos, parent->worldMatrix , &wpos );
    g3dvector_matrixf ( &wpos, objmap->inverseWorldMatrix, &mpos );

    g3dvector_normalize ( &mpos, NULL );

    /*** http://en.wikipedia.org/wiki/UV_mapping#Finding_UV_on_a_sphere ***/
    (*u) = ( atan2f( mpos.z, mpos.x ) / ( 2.0f * M_PI ) ) + 0.5f;
    (*v) = ( mpos.y + map->pln.yradius ) / ydiameter;
}

/******************************************************************************/
void g3duvmap_getFlatUV ( G3DUVMAP  *map, 
                          G3DMESH   *mes, 
                          G3DVECTOR *lpos,
                          float     *u, 
                          float     *v ) {
    float xdiameter = map->pln.xradius * 2.0f,
          ydiameter = map->pln.yradius * 2.0f;
    G3DOBJECT *objmap  = ( G3DOBJECT * ) map;
    G3DOBJECT *parent  = ( G3DOBJECT * ) mes;
    G3DVECTOR wpos; /*** position in mesh world coordinates ***/
    G3DVECTOR mpos; /*** position in map  local coordinates ***/

    g3dvector_matrixf (  lpos, parent->worldMatrix , &wpos );
    g3dvector_matrixf ( &wpos, objmap->inverseWorldMatrix, &mpos );

    (*u) = ( mpos.x + map->pln.xradius ) / xdiameter;
    (*v) = ( mpos.y + map->pln.yradius ) / ydiameter;
}

/******************************************************************************/
void g3duvmap_mapFace ( G3DUVMAP *map, G3DMESH *mes, G3DFACE *fac ) {
    G3DUVSET *uvs = g3dface_getUVSet ( fac, map );
    G3DOBJECT *objmap  = ( G3DOBJECT * ) map;
    G3DOBJECT *parent  = ( G3DOBJECT * ) mes;
    uint32_t i;

    if ( map->projection == UVMAPBACKGROUND ) {
        g3duvset_mapFaceWithBackgroundProjection ( uvs, fac, 0x00 );
    } else  {
        for ( i = 0x00; i < fac->nbver; i++ ) {
            G3DVERTEX *ver = fac->ver[i];

            switch ( map->projection ) {
                case UVMAPSPHERICAL : {
                    g3duvmap_getSphericalUV ( map, 
                                              mes, 
                                             &ver->pos,
                                             &uvs->veruv[i].u,
                                             &uvs->veruv[i].v );
                } break;

                case UVMAPCYLINDRICAL : {
                    g3duvmap_getCylindricalUV ( map, 
                                                mes, 
                                               &ver->pos,
                                               &uvs->veruv[i].u,
                                               &uvs->veruv[i].v );
                } break;

                case UVMAPFLAT :
                default :
                    g3duvmap_getFlatUV ( map, 
                                         mes, 
                                        &ver->pos,
                                        &uvs->veruv[i].u,
                                        &uvs->veruv[i].v );
                break;
            }
        }

        /*** Treats Spherical projection distorsion by finding the best ***/
        /*** match compared to the face center's UV coord which should be ***/
        /*** equal to the average of all vertices UVs ***/
        /**  ____________________
            |                   |
            |  * p1         *---|--* <-- this will be incorrectly mapped as p1
            |               |   |  |
            |               | Face |
            |               |   |  |
            |  * p2         *---|--* <-- this will be incorrectly mapped as p2
            |                   |
            |___________________|
                                  */

        if ( ( map->projection == UVMAPSPHERICAL   ) ||
             ( map->projection == UVMAPCYLINDRICAL ) ) {
            float avgu = ( uvs->veruv[0].u + uvs->veruv[1].u + 
                           uvs->veruv[2].u + uvs->veruv[3].u ) / fac->nbver;
            float avgv = ( uvs->veruv[0].v + uvs->veruv[1].v + 
                           uvs->veruv[2].v + uvs->veruv[3].v ) / fac->nbver;
            float facu, facv;

            if ( map->projection == UVMAPSPHERICAL ) {
                g3duvmap_getSphericalUV ( map, mes, &fac->pos, &facu, &facv );
            }

            if ( map->projection == UVMAPCYLINDRICAL ) {
                g3duvmap_getCylindricalUV ( map, mes, &fac->pos, &facu, &facv );
            }

            /*** face center U coord should match the avergae U coord of all ***/
            /*** vertices. If that's not the case, we are in a situation of ***/
            /*** spherical coordinates distorsion. We have to correct it by ***/
            /*** looking for the right match. ***/
            if ( fabs ( facu - avgu ) > 0.1f ) {
                G3DUV *uv = uvs->veruv;
                float testu[0x04][0x02] = { { uv[0].u, 1.0f - uv[0].u },
                                            { uv[1].u, 1.0f - uv[1].u },
                                            { uv[2].u, 1.0f - uv[2].u },
                                            { uv[3].u, 1.0f - uv[3].u } };
                float bestMatch = 1.0f;
                int j, k, l;

                for ( i = 0x00; i < 0x02; i++ ) {
                    for ( j = 0x00; j < 0x02; j++ ) {
                        for ( k = 0x00; k < 0x02; k++ ) {
                            for ( l = 0x00; l < (( fac->nbver == 0x03 ) ? 0x01 : 0x02); l++ ) {
                                float testavgu =  ( testu[0][i] +
                                                    testu[1][j] + 
                                                    testu[2][k] + 
                                                    testu[3][l] ) / fac->nbver;
                                float match = fabs ( facu - testavgu );

                                /*** find a candidate that matches closely ***/
                                /*** the face U  coordinates ***/
                                if ( match < bestMatch ) {
                                    uvs->veruv[0].u = testu[0][i];
                                    uvs->veruv[1].u = testu[1][j];
                                    uvs->veruv[2].u = testu[2][k];
                                    uvs->veruv[3].u = testu[3][l];

                                    bestMatch = match;
                                }
                            }
                        }
                    }
                }
            }

            /*** face center V coord should match the avergae V coord of all ***/
            /*** vertices. If that's not the case, we are in a situation of ***/
            /*** spherical coordinates distorsion. We have to correct it by ***/
            /*** looking for the right match. ***/
            if ( fabs ( facv - avgv ) > 0.1f ) {
                G3DUV *uv = uvs->veruv;
                float testv[0x04][0x02] = { { uv[0].v, 1.0f - uv[0].v },
                                            { uv[1].v, 1.0f - uv[1].v },
                                            { uv[2].v, 1.0f - uv[2].v },
                                            { uv[3].v, 1.0f - uv[3].v } };
                float bestMatch = 1.0f;
                int j, k, l;

                for ( i = 0x00; i < 0x02; i++ ) {
                    for ( j = 0x00; j < 0x02; j++ ) {
                        for ( k = 0x00; k < 0x02; k++ ) {
                            for ( l = 0x00; l < (( fac->nbver == 0x03 ) ? 0x01 : 0x02); l++ ) {
                                float testavgv =  ( testv[0][i] +
                                                    testv[1][j] + 
                                                    testv[2][k] + 
                                                    testv[3][l] ) / fac->nbver;
                                float match = fabs ( facv - testavgv );

                                /*** find a candidate that matches closely ***/
                                /*** the face V  coordinates ***/
                                if ( match < bestMatch ) {
                                    uvs->veruv[0].v = testv[0][i];
                                    uvs->veruv[1].v = testv[1][j];
                                    uvs->veruv[2].v = testv[2][k];
                                    uvs->veruv[3].v = testv[3][l];

                                    bestMatch = match;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /*** Prepare the subdivided UV coords **/
    g3duvset_subdivide ( uvs, fac );
}

/******************************************************************************/
void g3duvmap_adjustFlatProjection ( G3DUVMAP *map, G3DMESH *mes ) {
    G3DOBJECT *objmap = ( G3DOBJECT * ) map;
    G3DOBJECT *objmes = ( G3DOBJECT * ) mes;
    float parx = ( objmes->bbox.xmax + objmes->bbox.xmin ) / 2.0f,
          pary = ( objmes->bbox.ymax + objmes->bbox.ymin ) / 2.0f;

    map->pln.xradius = ( objmes->bbox.xmax - objmes->bbox.xmin ) / 2.0f;
    map->pln.yradius = ( objmes->bbox.ymax - objmes->bbox.ymin ) / 2.0f;

    objmap->pos.x = parx;
    objmap->pos.y = pary;

    g3dobject_updateMatrix ( objmap, 0x00 );
}

/******************************************************************************/
void g3duvmap_applyProjection ( G3DUVMAP *map, G3DMESH *mes ) {
    G3DOBJECT *objmap  = ( G3DOBJECT * ) map;
    G3DMESH   *parmes  = mes;
    float xdiameter = map->pln.xradius * 2.0f,
          ydiameter = map->pln.yradius * 2.0f;
    LIST *ltmpfac = parmes->lfac;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) _GETFACE(parmes,ltmpfac);

        /*** This function assigns a UVSet to the face if needed and ***/
        /*** maps the UV coordinates to the face ***/
        g3duvmap_insertFace ( map, mes, fac );


        _NEXTFACE(parmes,ltmpfac);
    }
}

/******************************************************************************/
uint32_t g3duvmap_draw ( G3DOBJECT *obj, 
                         G3DCAMERA *curcam, 
                         G3DENGINE *engine, 
                         uint64_t   engine_flags ) {
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
            g3dcore_drawXYCircle ( 0.5f, engine_flags );

            glColor3ub ( 0x00, 0x80, 0x00 );
            g3dcore_drawZXCircle ( 0.5f, engine_flags );

            glColor3ub ( 0x80, 0x00, 0x00 );
            g3dcore_drawYZCircle ( 0.5f, engine_flags );
        break;

        case UVMAPCYLINDRICAL :
            glColor3ub ( 0x00, 0x00, 0x80 );
            g3dcore_drawZXCircle ( 0.5f, engine_flags );
        break;

        default :
        break;
    }

    glPopAttrib ( );

    return 0x00;
}

/******************************************************************************/
void g3duvmap_transform ( G3DOBJECT *obj, 
                          uint64_t   engine_flags ) {
    G3DOBJECT *parent = obj->parent;
    G3DUVMAP *map = ( G3DUVMAP * ) obj;

    if ( parent->type  & MESH ) {
        G3DMESH *mes = ( G3DMESH * ) parent;

        if ( ( ( engine_flags     & ONGOINGANIMATION ) == 0x00 ) ||
             ( ( map->projection == UVMAPBACKGROUND  )         ) ) {
            if ( ( obj->flags & UVMAPFIXED ) == 0x00 ) {
                g3duvmap_applyProjection ( map, mes );

                mes->obj.update_flags |= RESETMODIFIERS;

                g3dmesh_update ( mes, engine_flags );
            }
        }
    }
}

/******************************************************************************/
void g3duvmap_init ( G3DUVMAP *map, char *name, uint32_t projection ) {
    G3DOBJECT *objmap = ( G3DOBJECT * ) map;

    g3dobject_init ( objmap, G3DUVMAPTYPE, 0x00, name, 0x00,
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
}

/******************************************************************************/
G3DUVMAP *g3duvmap_new ( char *name, uint32_t projection ) {
    G3DUVMAP *map = ( G3DUVMAP * ) calloc ( 0x01, sizeof ( G3DUVMAP ) );

    if ( map == NULL ) {
        fprintf ( stderr, "g3duvmap_new(): memory allocation failed\n" );

        return NULL; 
    }

    g3duvmap_init ( map, name, projection );


    return map;
}

/******************************************************************************/
void g3duvmap_free ( G3DOBJECT *obj ) {
    G3DUVMAP *map = ( G3DUVMAP * ) obj;
}

/******************************************************************************/
/*void g3duvmap_addMaterial ( G3DUVMAP *map, G3DMATERIAL *mat ) {

    list_insert ( &map->lmat, mat );

    map->nbmat++;
}*/

/******************************************************************************/
/*void g3duvmap_removeMaterial ( G3DUVMAP *map, G3DMATERIAL *mat ) {
    list_remove ( &map->lmat, mat );

    map->nbmat--;
}*/
