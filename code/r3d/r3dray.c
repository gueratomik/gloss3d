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
#include <r3d.h>

/*** Thanks to this website for the clear explanations :   ***/
/*** http://tomdalling.com/blog/modern-opengl/             ***/
/*** 07-more-lighting-ambient-specular-attenuation-gamma/  ***/

/******************************************************************************/
void r3dray_getHitFaceColor ( R3DRAY *ray, R3DFACE *rfc,
                                           R3DRGBA *rgba,
                                           LIST    *ltex ) {
    LIST *ltmpuvs = rfc->ancestorFace->luvs;
    uint32_t i;

    if ( ltmpuvs == NULL ) rgba->r = rgba->g = rgba->b = MESHCOLORUB;

    while ( ltmpuvs ) {
        G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;
        /* to uncomment */
        G3DRTUV *rtuvmem = /*rfc->ancestorFace->rtuvmem*/NULL;
        float avgu =       ( ( rtuvmem[rfc->ver[0x00]->id].u * ray->ratio[0x00] ) +
                             ( rtuvmem[rfc->ver[0x01]->id].u * ray->ratio[0x01] ) +
                             ( rtuvmem[rfc->ver[0x02]->id].u * ray->ratio[0x02] ) ),

              avgv =       ( ( rtuvmem[rfc->ver[0x00]->id].v * ray->ratio[0x00] ) +
                             ( rtuvmem[rfc->ver[0x01]->id].v * ray->ratio[0x01] ) +
                             ( rtuvmem[rfc->ver[0x02]->id].v * ray->ratio[0x02] ) );
        LIST *ltmptex = ltex;


        while ( ltmptex ) {
            G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;
            G3DMATERIAL *mat = tex->mat;

            if ( tex->map == uvs->map ) {
                if ( mat->flags & DIFFUSE_USEIMAGECOLOR ) {
                    G3DIMAGE *colimg = mat->diffuse.image;

                    if ( colimg ) {
                        int32_t imgx = ((int32_t)((float)avgu * colimg->width  )) % colimg->width;
                        int32_t imgy = ((int32_t)((float)avgv * colimg->height )) % colimg->height;

                        if ( imgx < 0x00 ) imgx = colimg->width  - imgx;
                        if ( imgy < 0x00 ) imgy = colimg->height - imgy;

                        uint32_t offset = ( imgy * colimg->bytesperline  ) +
                                          ( imgx * colimg->bytesperpixel );

                        rgba->r += colimg->data[offset+0x00];
                        rgba->g += colimg->data[offset+0x01];
                        rgba->b += colimg->data[offset+0x02];
                    } else {
                        g3dcolor_toRGBA ( &mat->diffuse.solid, rgba );
                    }
                }

                if ( mat->flags & DIFFUSE_USESOLIDCOLOR ) {
                    g3dcolor_toRGBA ( &mat->diffuse.solid, rgba );
                }

                if ( mat->flags & DIFFUSE_USEPROCEDURAL ) {
                    G3DPROCEDURAL *proc = mat->diffuse.proc;

                    if ( proc ) {
                        proc->getColor ( proc, avgu, avgv, 0.0f, rgba );
                    } else {
                        g3dcolor_toRGBA ( &mat->diffuse.solid, rgba );
                    }
                }
            }

            ltmptex = ltmptex->next;
        }

        ltmpuvs = ltmpuvs->next;
    }
}

/******************************************************************************/
uint32_t r3dray_reflect ( R3DRAY *ray, R3DRAY *rout ) {
    float dot = g3dvector_scalar ( ( G3DVECTOR * ) &ray->dir,
                                   ( G3DVECTOR * ) &ray->nor );
    if ( dot > 0.0f ) {
        return 0x00;
    } else {
        float dotby2 = dot * 2.0f;

        memset ( rout, 0x00, sizeof ( R3DRAY ) );

        /*** init the depth value for depth sorting ***/
        rout->distance = INFINITY;

        /*** reflected ray origin is parent ray intersection point ***/
        memcpy ( &rout->ori, &ray->pnt, sizeof ( R3DDOUBLEPOINT ) );

        /*** reflection formula ***/
        rout->dir.x = ray->dir.x - ( dotby2 * ray->nor.x );
        rout->dir.y = ray->dir.y - ( dotby2 * ray->nor.y );
        rout->dir.z = ray->dir.z - ( dotby2 * ray->nor.z );

        r3dtinyvector_normalize ( &rout->dir, NULL );

    }

    return 0x01;
}

/******************************************************************************/
/************** http://http.developer.nvidia.com/Cg/refract.html **************/
/******************************************************************************/
void r3dray_refract ( R3DRAY *ray, R3DRAY *rout, float eta ) {
    float cosi = -g3dvector_scalar ( ( G3DVECTOR * ) &ray->dir, 
                                     ( G3DVECTOR * ) &ray->nor );
    float cost = 1.0f - ( ( eta * eta ) * ( 1.0f - ( cosi * cosi ) ) );

    memset ( rout, 0x00, sizeof ( R3DRAY ) );

    /*** init the depth value for depth sorting ***/
    rout->distance = INFINITY;

    /*** refracted ray origin is parent ray intersection point ***/
    memcpy ( &rout->ori, &ray->pnt, sizeof ( R3DDOUBLEPOINT ) );

    if ( cost > 0.0f ) {
        float cabs = sqrt ( cost );

        rout->dir.x = (eta * ray->dir.x) + (((eta * cosi) - cabs) * ray->nor.x);
        rout->dir.y = (eta * ray->dir.y) + (((eta * cosi) - cabs) * ray->nor.y);
        rout->dir.z = (eta * ray->dir.z) + (((eta * cosi) - cabs) * ray->nor.z);
    } else {
        memcpy ( &rout->dir, &ray->dir, sizeof ( R3DVECTOR ) );
    }

    r3dtinyvector_normalize ( &rout->dir, NULL );
}

/******************************************************************************/
uint32_t r3dray_illumination ( R3DRAY *ray, R3DSCENE *rsce, 
                                            R3DRGBA  *col,
                                            R3DRGBA  *spc,
                                            R3DFACE  *discard,
                                            uint32_t  nbhop,
                                            LIST     *ltex ) {
    LIST *ltmptex = ltex;
    LIST *ltmprlt = rsce->lrlt;
    LIST *lrob = rsce->lrob;
    R3DCAMERA *rcam = rsce->area.rcam;

    while ( ltmprlt ) {
        R3DLIGHT *rlt = ( R3DLIGHT * ) ltmprlt->data;
        R3DRAY luxray, spcray, refray, camray;
        float dot;

        memset ( &luxray, 0x00, sizeof ( R3DRAY ) );

        /*memcpy ( &luxray.ori, &ray->pnt, sizeof ( R3DDOUBLEPOINT ) );*/

        luxray.ori.x = ray->pnt.x;
        luxray.ori.y = ray->pnt.y;
        luxray.ori.z = ray->pnt.z;

        luxray.dir.x = ( rlt->pos.x - luxray.ori.x );
        luxray.dir.y = ( rlt->pos.y - luxray.ori.y );
        luxray.dir.z = ( rlt->pos.z - luxray.ori.z );

        g3dtinyvector_normalize ( &luxray.dir, NULL );

        luxray.distance  = INFINITY;
        luxray.intensity = 1.0f;
        luxray.flags     = 0x00;

        dot = g3dvector_scalar ( ( G3DVECTOR * ) &luxray.dir, 
                                 ( G3DVECTOR * ) &ray->nor );

        if ( dot > 0.0f ) {
            /*** first find intersection closest point ***/
            r3dray_shoot ( &luxray, rsce, discard, nbhop, RAYQUERYHIT | RAYLUXRAY | RAYQUERYIGNOREBACKFACE );

            /*** Prevents black dots (micro-shadowing) ***/
            if ( luxray.flags & INTERSECT ) {
                R3DVECTOR dif = { ( ray->pnt.x - luxray.pnt.x ),
                                  ( ray->pnt.y - luxray.pnt.y ),
                                  ( ray->pnt.z - luxray.pnt.z ) };

                if ( g3dtinyvector_length ( &dif ) < 0.001f ) {
                    luxray.flags &= (~INTERSECT);

                    luxray.distance  = INFINITY;
                }
            }

            if ( ( luxray.flags & INTERSECT ) == 0x00 ) {
                float rate = fabs ( dot ) * rlt->lig->intensity;

                col->r += ( rlt->lig->diffcol.r * rate ),
                col->g += ( rlt->lig->diffcol.g * rate ),
                col->b += ( rlt->lig->diffcol.b * rate );
            }
        }

        while ( ltmptex ) {
            G3DTEXTURE  *tex = ( G3DTEXTURE * ) ltmptex->data;
            G3DMATERIAL *mat = tex->mat;

            /*** specular ***/
            if ( mat ) {
                R3DVECTOR camray;

                memcpy ( &spcray.nor, &ray->nor, sizeof ( R3DVECTOR      ) );
                memcpy ( &spcray.pnt, &ray->pnt, sizeof ( R3DDOUBLEPOINT ) );

                spcray.dir.x = ( ray->pnt.x - rlt->pos.x );
                spcray.dir.y = ( ray->pnt.y - rlt->pos.y );
                spcray.dir.z = ( ray->pnt.z - rlt->pos.z );

                r3dtinyvector_normalize ( &spcray.dir, NULL );

                if ( r3dray_reflect ( &spcray, &refray ) ) {

    /*** COMMENTED: Normalizing already in r3dray_reflect() ***/
                    /*r3dtinyvector_normalize ( &refray.dir, NULL );*/

                    camray.x = ( rcam->pos.x - ray->pnt.x );
                    camray.y = ( rcam->pos.y - ray->pnt.y );
                    camray.z = ( rcam->pos.z - ray->pnt.z );

                    r3dtinyvector_normalize ( &camray, NULL );

                    dot = g3dvector_scalar ( ( G3DVECTOR * ) &refray.dir,
                                             ( G3DVECTOR * ) &camray );

                    if ( dot > 0.0f ) {
                        float specularCoefficient = pow ( dot, mat->shininess );
                            uint32_t R = ( rlt->lig->speccol.r * mat->specular_level * mat->specular.solid.r * specularCoefficient ),
                                     G = ( rlt->lig->speccol.g * mat->specular_level * mat->specular.solid.g * specularCoefficient ),
                                     B = ( rlt->lig->speccol.b * mat->specular_level * mat->specular.solid.b * specularCoefficient );

                        spc->r += R;
                        spc->g += G,
                        spc->b += B;
                    }

                }

                ltmptex = ltmptex->next;
            }
        }

        ltmprlt = ltmprlt->next;
    }

    if ( spc->r > 0xFF ) spc->r = 0xFF;
    if ( spc->g > 0xFF ) spc->g = 0xFF;
    if ( spc->b > 0xFF ) spc->b = 0xFF;


    return 0x00;
}

/******************************************************************************/
uint32_t r3dray_intersectPlane ( R3DRAY *ray, R3DPLANE *pla, R3DPOINT *pt ) {
    float vo = ( pla->x * ray->ori.x ) +
               ( pla->y * ray->ori.y ) +
               ( pla->z * ray->ori.z ) + pla->w,
          vd = ( pla->x * ray->dir.x ) + 
               ( pla->y * ray->dir.y ) +
               ( pla->z * ray->dir.z );
    float t;

    if ( vd == 0.0f ) return 0x00;

    t = - ( vo / vd );

    /*if ( t > 0.0f ) {*/
        /*if ( vd < 0.0f ) {*/
            pt->x = ray->ori.x + ( ray->dir.x * t );
            pt->y = ray->ori.y + ( ray->dir.y * t );
            pt->z = ray->ori.z + ( ray->dir.z * t );
            /*** store the distance we got ***/
            pt->w = t;

            return 0x01;
        /*}*/
    /*}*/

    return 0x00;
}

/******************************************************************************/
uint32_t r3dray_intersectOctree ( R3DRAY *ray, R3DOCTREE *rot,/*
                                               R3DPOINT  *raymin,
                                               R3DPOINT  *raymax,*/
                                               R3DFACE   *discard, 
                                               uint32_t   query_flags ) {

    uint32_t hit = 0x00;

    if ( rot->children[0x00] ) {
        R3DPOINT newmin, newmax;
        int i;

        for ( i = 0x00; i < 0x08; i++ ) {
            R3DOCTREE *octkid = rot->children[i];

            /*** if our child octree does not contain any triangle, skip it ***/
            /*** (and no children) ***/
            if ( octkid->nbrfc || octkid->children[0x00] ) {
                R3DPOINT ptin, ptout;

                if ( r3dray_inOctreeSimple ( ray, octkid ) ) {
                /*if ( r3dray_inOctree ( ray, octkid, &ptin, &ptout ) ) {
                    r3dpoint_getMinMax ( &ptin, &ptout, raymin, raymax );*/

                    hit += r3dray_intersectOctree ( ray,
                                                    octkid,/*
                                                    raymin,
                                                    raymax,*/
                                                    discard,
                                                    query_flags );
                }
            }
        }
    } else {
        uint32_t i;

        for ( i = 0x00; i < rot->nbrfc; i++ ){
            R3DFACE *rfc = rot->rfcarray[i];

            if ( rfc != discard ) {
                /*float xmin, ymin, zmin, 
                      xmax, ymax, zmax;

                r3dface_getMinMax ( rfc, &xmin, &ymin, &zmin, 
                                         &xmax, &ymax, &zmax );

                if ( ( ( xmin < raymax->x ) && ( xmax > raymin->x ) ) &&
                     ( ( ymin < raymax->y ) && ( ymax > raymin->y ) ) &&
                     ( ( zmin < raymax->z ) && ( zmax > raymin->z ) ) ) {*/
                    if ( r3dface_intersectRay ( rfc, ray, query_flags ) ) {
                        hit = 0x01;
                    }
                /*}*/
            }
        }
    }

    return hit;
}

/******************************************************************************/
uint32_t r3dray_inOctreeSimple ( R3DRAY *ray, R3DOCTREE *rot ) {
    uint32_t j, nbhit = 0x00;
    uint32_t ret;


    /*** if the ray starts in the octree, we already have 1 hit ***/
    if ( ( ( ray->ori.x >= rot->xmin ) && ( ray->ori.x <= rot->xmax ) &&
           ( ray->ori.y >= rot->ymin ) && ( ray->ori.y <= rot->ymax ) &&
           ( ray->ori.z >= rot->zmin ) && ( ray->ori.z <= rot->zmax ) ) ) {
        return 0x01;
    }

    for ( j = 0x00; j < 0x06; j++ ) {
        R3DPOINT vout;

        ret = r3dray_intersectPlane ( ray, &rot->pla[j], &vout );

        if ( ret ) {
            if ( ( ( vout.x >= rot->xmin ) && ( vout.x <= rot->xmax ) &&
                   ( vout.y >= rot->ymin ) && ( vout.y <= rot->ymax ) &&
                   ( vout.z >= rot->zmin ) && ( vout.z <= rot->zmax ) ) ) {
                return 0x01;
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
uint32_t r3dray_inOctree ( R3DRAY *ray, R3DOCTREE *rot,
                                        R3DPOINT *ptin,
                                        R3DPOINT *ptout ) {
    R3DPOINT  vout, *isec = ptin;
    uint32_t j, nbhit = 0x00;
    uint32_t ret;


    /*** if the ray starts in the octree, we already have 1 hit ***/
    if ( ( ( ray->ori.x >= rot->xmin ) && ( ray->ori.x <= rot->xmax ) &&
           ( ray->ori.y >= rot->ymin ) && ( ray->ori.y <= rot->ymax ) &&
           ( ray->ori.z >= rot->zmin ) && ( ray->ori.z <= rot->zmax ) ) ) {
        isec->x = ray->ori.x;
        isec->y = ray->ori.y;
        isec->z = ray->ori.z;

        isec = ptout;

        nbhit = 0x01;
    }

    for ( j = 0x00; j < 0x06; j++ ) {
        ret = r3dray_intersectPlane ( ray, &rot->pla[j], &vout );

        if ( ret ) {
            if ( ( ( vout.x >= rot->xmin ) && ( vout.x <= rot->xmax ) &&
                   ( vout.y >= rot->ymin ) && ( vout.y <= rot->ymax ) &&
                   ( vout.z >= rot->zmin ) && ( vout.z <= rot->zmax ) ) ) {

                if ( nbhit == 0x01 ) isec = ptout;

                isec->x = vout.x;
                isec->y = vout.y;
                isec->z = vout.z;

                nbhit++;
            }
        }
    }

    /*** If we hit the edge, we got only 1 hit. We must fix that ***/
    if ( nbhit == 0x01 ) {
        memcpy ( ptout, ptin, sizeof ( R3DPOINT ) );
    }


    return ( nbhit > 0x00 );
}

/******************************************************************************/
void r3dpoint_getMinMax ( R3DPOINT *ptin, R3DPOINT *ptout, 
                          R3DPOINT *min , R3DPOINT *max ) {
    if ( ptin->x < ptout->x ) {
        min->x = ptin->x;
        max->x = ptout->x;
    } else {
        min->x = ptout->x;
        max->x = ptin->x;
    }

    if ( ptin->y < ptout->y ) {
        min->y = ptin->y;
        max->y = ptout->y;
    } else {
        min->y = ptout->y;
        max->y = ptin->y;
    }

    if ( ptin->z < ptout->z ) {
        min->z = ptin->z;
        max->z = ptout->z;
    } else {
        min->z = ptout->z;
        max->z = ptin->z;
    }
}

/******************************************************************************/
uint32_t r3dray_shoot ( R3DRAY *ray, R3DSCENE *rsce,
                                     R3DFACE  *discard,
                                     uint32_t  nbhop,
                                     uint32_t  query_flags ) {
    R3DRGBA col = { .r = 0x00, .g = 0x00, .b = 0x00, .a = 0xFF };
    R3DRGBA spc = { .r = 0x00, .g = 0x00, .b = 0x00, .a = 0xFF };
    LIST *ltmprob = rsce->lrob;
    R3DOBJECT *hitrob = NULL;
    R3DFACE   *hitrfc = NULL;

    /*** Don't bounce forever ***/
    if ( nbhop < MAXRAYHIT ) {
        /*** the hit test ***/
        if ( query_flags & RAYQUERYHIT ) {
            while ( ltmprob ) {
                R3DOBJECT *rob = ( R3DOBJECT * ) ltmprob->data;
                R3DOCTREE *oct = rob->rot;

                /*** we only render mesh objects ***/
                if ( ( rob->obj->type == G3DMESHTYPE ) ||
                     ( rob->obj->type  & PRIMITIVE   ) ) {
                    R3DPOINT ptin, ptout;

                    if ( r3dray_inOctree ( ray, oct, &ptin, &ptout ) ) {
                        /*R3DPOINT raymin, raymax;

                        r3dpoint_getMinMax ( &ptin, &ptout, &raymin, &raymax );*/

                        if ( r3dray_intersectOctree ( ray, oct,/* 
                                                          &raymin, 
                                                          &raymax,*/
                                                           discard, 
                                                           query_flags ) ) {
                            hitrob = rob;
                            hitrfc = ray->rfc;
                        }
                    }
                }

                ltmprob = ltmprob->next;
            }
        }

        if ( ray->flags & INTERSECT ) {
            G3DRGBA rgba = { 0x00, 0x00, 0x00, 0x00 };
            R3DOBJECT *rob = hitrob;
            R3DMESH *rms = ( rob->obj->type & G3DMESHTYPE ) ? ( R3DMESH * ) rob : NULL;
            LIST *ltex = ((G3DMESH*)hitrob->obj)->ltex;

            uint32_t nbmap = ( rms ) ? rms->nbmap : 0x00;

            if ( query_flags & RAYSTART ) rsce->area.rfc[(ray->y*rsce->area.width)+ray->x] = hitrfc;

            r3dray_getHitFaceColor ( ray, hitrfc, &rgba, ltex );

            if ( query_flags & RAYQUERYREFLECTION ) {
                uint32_t i;

                for ( i = 0x00; i < nbmap; i++ ) {
                    LIST *ltmptex = ((G3DMESH*)hitrob->obj)->ltex;

                    while ( ltmptex ) {
                        G3DTEXTURE  *tex = ( G3DTEXTURE * ) ltmptex->data;
                        G3DMATERIAL *mat = tex->mat;
                        LIST *ltmpuvs = hitrfc->ancestorFace->luvs;

                        while ( ltmpuvs ) {
                            G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;
                            /** TODO: make it cumulative **/
                            if ( ( tex->map == uvs->map ) && mat->reflection_strength ) {
                                uint32_t retcol;
                                R3DRAY flxray;

                                /*** build the reflexion ray from the current ray ***/
                                if ( r3dray_reflect ( ray, &flxray ) ) {
  
                                    retcol = r3dray_shoot ( &flxray, rsce, hitrfc, nbhop + 0x01, RAYQUERYALL );

                                    if ( mat->reflection_strength == 1.0f ) {
                                        rgba.r = ( retcol & 0x00FF0000 ) >> 0x10;
                                        rgba.g = ( retcol & 0x0000FF00 ) >> 0x08;
                                        rgba.b = ( retcol & 0x000000FF );
                                    } else {
                                        float sub = ( 1.0f - mat->reflection_strength );
                                        uint32_t REFR = ( retcol & 0x00FF0000 ) >> 0x10,
                                                 REFG = ( retcol & 0x0000FF00 ) >> 0x08,
                                                 REFB = ( retcol & 0x000000FF );

                                        rgba.r = ( REFR * mat->reflection_strength ) + ( rgba.r * sub );
                                        rgba.g = ( REFG * mat->reflection_strength ) + ( rgba.g * sub );
                                        rgba.b = ( REFB * mat->reflection_strength ) + ( rgba.b * sub );
                                    }
                                }
                            }

                            ltmpuvs = ltmpuvs->next;
                        }

                        ltmptex = ltmptex->next;
                    }
                }
            }

            if ( query_flags & RAYQUERYREFRACTION ) {
                uint32_t i;

                for ( i = 0x00; i < nbmap; i++ ) {
                    LIST *ltmptex = ((G3DMESH*)hitrob->obj)->ltex;

                    while ( ltmptex ) {
                        G3DTEXTURE  *tex = ( G3DTEXTURE * ) ltmptex->data;
                        G3DMATERIAL *mat = tex->mat;
                        LIST *ltmpuvs = hitrfc->ancestorFace->luvs;

                        while ( ltmpuvs ) {
                            G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;
                            if ( ( tex->map == uvs->map ) && mat->transparency_strength ) {
                                uint32_t retcol;
                                R3DRAY frcray;

                                /*** build the reflexion ray from the current ray ***/
                                r3dray_refract ( ray, &frcray, mat->refraction_strength );

                                retcol = r3dray_shoot ( &frcray, rsce, hitrfc, nbhop + 0x01, RAYQUERYALL );

                                if ( mat->transparency_strength == 1.0f ) {
                                    rgba.r = ( retcol & 0x00FF0000 ) >> 0x10;
                                    rgba.g = ( retcol & 0x0000FF00 ) >> 0x08;
                                    rgba.b = ( retcol & 0x000000FF );
                                } else {
                                    float sub = ( 1.0f - mat->transparency_strength );
                                    uint32_t REFR = ( retcol & 0x00FF0000 ) >> 0x10,
                                             REFG = ( retcol & 0x0000FF00 ) >> 0x08,
                                             REFB = ( retcol & 0x000000FF );

                                    rgba.r = ( REFR * mat->transparency_strength ) + ( rgba.r * sub );
                                    rgba.g = ( REFG * mat->transparency_strength ) + ( rgba.g * sub );
                                    rgba.b = ( REFB * mat->transparency_strength ) + ( rgba.b * sub );
                                }
                            }
                            ltmpuvs = ltmpuvs->next;
                        }

                        ltmptex = ltmptex->next;
                    }
                }
            }

            if ( query_flags & RAYQUERYLIGHTING ) {
                r3dray_illumination ( ray, rsce, &col, &spc, hitrfc, nbhop + 0x01, ltex  );
            }

            ray->R = ( uint32_t ) ( ( col.r * rgba.r ) >> 0x08 ) + spc.r;
            ray->G = ( uint32_t ) ( ( col.g * rgba.g ) >> 0x08 ) + spc.g;
            ray->B = ( uint32_t ) ( ( col.b * rgba.b ) >> 0x08 ) + spc.b;

            if ( ray->R > 0xFF ) ray->R = 0xFF;
            if ( ray->G > 0xFF ) ray->G = 0xFF;
            if ( ray->B > 0xFF ) ray->B = 0xFF;
            /*** End of the lighting part ***/

            return ( ( uint32_t ) ( ray->R << 0x10 ) | 
                                  ( ray->G << 0x08 ) | 
                                  ( ray->B         ) );
        }
    }

    return rsce->background;
}









