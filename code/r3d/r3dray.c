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
#include <r3d.h>

/*** Thanks to this website for the clear explanations :   ***/
/*** http://tomdalling.com/blog/modern-opengl/             ***/
/*** 07-more-lighting-ambient-specular-attenuation-gamma/  ***/

/******************************************************************************/
void r3dray_outline ( R3DRAY *ray, R3DMESH *rms,
                                   R3DFACE *rfc,
                                   R3DRGBA *diffuse,
                                   uint32_t wireframeColor,
                                   float    wireframeThickness,
                                   uint32_t query_flags ) {
    R2DPOINT p[0x04];
    uint32_t nbp;
    uint32_t i;

    /*** we need to rebuild the quads otherwise there are some artefacts ***/
    if ( rfc->flags & RFACEFROMQUAD ) {
        R3DFACE *rfcone = rfc;
            /*** in case rfc is created from a quad that was splitted in ***/
            /*** 2 triangles, the next triangle is the second part of ***/
            /*** the split ***/
        R3DFACE *rfctwo = ( rfc->flags & RFACEFROMQUADONE ) ? rfc + 0x01 :
                                                              rfc - 0x01;

        p[0x00].x = rms->rver[rfcone->rverID[0x00]].scr.x;
        p[0x00].y = rms->rver[rfcone->rverID[0x00]].scr.y;

        p[0x01].x = rms->rver[rfcone->rverID[0x01]].scr.x;
        p[0x01].y = rms->rver[rfcone->rverID[0x01]].scr.y;

        p[0x02].x = rms->rver[rfctwo->rverID[0x00]].scr.x;
        p[0x02].y = rms->rver[rfctwo->rverID[0x00]].scr.y;

        p[0x03].x = rms->rver[rfctwo->rverID[0x01]].scr.x;
        p[0x03].y = rms->rver[rfctwo->rverID[0x01]].scr.y;

        nbp = 0x04;
    } else {
        p[0x00].x = rms->rver[rfc->rverID[0x00]].scr.x;
        p[0x00].y = rms->rver[rfc->rverID[0x00]].scr.y;

        p[0x01].x = rms->rver[rfc->rverID[0x01]].scr.x;
        p[0x01].y = rms->rver[rfc->rverID[0x01]].scr.y;

        p[0x02].x = rms->rver[rfc->rverID[0x02]].scr.x;
        p[0x02].y = rms->rver[rfc->rverID[0x02]].scr.y;

        nbp = 0x03;
    }

    for ( i = 0x00; i < nbp; i++ ) {
        uint32_t n = ( i + 0x01 ) % nbp;
        int32_t x1 = p[i].x,
                y1 = p[i].y,
                x2 = p[n].x,
                y2 = p[n].y;
        float dist;

        /* skip the central edge for former quads */
        /*if ( ( rfc->flags & RFACEFROMQUAD ) && 
             ( ( i == 0x02 ) && ( n == 0x00 ) ) ) {
            continue;
        }*/

    /*https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line */
        dist = abs ( ( ( y2 - y1 ) * ray->x ) - 
                     ( ( x2 - x1 ) * ray->y ) + ( x2 * y1 ) - 
                                                ( y2 * x1 ) ) /
               sqrt ( (( y2 - y1 ) * ( y2 - y1 )) + 
                      (( x2 - x1 ) * ( x2 - x1 )) );

        if ( dist <= wireframeThickness ) {
            unsigned char R = ( wireframeColor & 0x00FF0000 ) >> 0x10,
                          G = ( wireframeColor & 0x0000FF00 ) >> 0x08,
                          B = ( wireframeColor & 0x000000FF ) >> 0x00,
                          A = 0x00;

            /* if lighting if enabled for outlining, we can render    */
            /* the outlines with some sort of antialiasing effect     */
            /* by combining with the object material color. It does   */
            /* not work when outline lighting is disabled because     */
            /* the diffuse color we use is from BEFORE the lighting   */
            /* and thus the antialised pixel will look different from */
            /* the final material aspect with the lighting. */
            float mix = dist / wireframeThickness;

            diffuse->r = (float) diffuse->r * mix + (float) ( 1.0f - mix ) * R;
            diffuse->g = (float) diffuse->g * mix + (float) ( 1.0f - mix ) * G;
            diffuse->b = (float) diffuse->b * mix + (float) ( 1.0f - mix ) * B;
            diffuse->a = (float) diffuse->a * mix + (float) ( 1.0f - mix ) * A;

            ray->flags |= OUTLINED;

            /* commented: a ray could be close to each edge */
            /*return;*/
        }
    }
}

/******************************************************************************/
void r3dray_getHitFaceColor ( R3DRAY *ray, R3DMESH *rms,
                                           R3DFACE *rfc,
                                           R3DRGBA *diffuse,
                                           R3DRGBA *specular,
                                           R3DRGBA *bump,
                                           R3DRGBA *reflection,
                                           R3DRGBA *refraction,
                                           LIST    *ltex,
                                           uint32_t query_flags ) {
    uint32_t divDiffuse    = 0x00;
    uint32_t divSpecular   = 0x00;
    uint32_t divBump       = 0x00;
    uint32_t divReflection = 0x00;
    uint32_t divRefraction = 0x00;
    LIST *ltmptex = ltex;
    uint32_t i;

    memset ( diffuse   , 0x00, sizeof ( R3DRGBA ) );
    memset ( specular  , 0x00, sizeof ( R3DRGBA ) );
    memset ( bump      , 0x00, sizeof ( R3DRGBA ) );
    memset ( reflection, 0x00, sizeof ( R3DRGBA ) );
    memset ( refraction, 0x00, sizeof ( R3DRGBA ) );

    if ( ltex == NULL ) { 
        diffuse->r = diffuse->g = diffuse->b = MESHCOLORUB;
    }

    while ( ltmptex ) {
        G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;
        G3DMATERIAL *mat = tex->mat;

        if ( tex->map && rfc->ruvs ) {
            uint32_t mapID = tex->map->mapID;
            R3DRGBA retval;

            float avgu = ( ( rfc->ruvs[mapID].uv[0x00].u * ray->ratio[0x00] ) +
                           ( rfc->ruvs[mapID].uv[0x01].u * ray->ratio[0x01] ) +
                           ( rfc->ruvs[mapID].uv[0x02].u * ray->ratio[0x02] ) ),
                  avgv = ( ( rfc->ruvs[mapID].uv[0x00].v * ray->ratio[0x00] ) +
                           ( rfc->ruvs[mapID].uv[0x01].v * ray->ratio[0x01] ) +
                           ( rfc->ruvs[mapID].uv[0x02].v * ray->ratio[0x02] ) );

            if ( mat->flags & DIFFUSE_ENABLED ) {
                g3dchannel_getColor ( &mat->diffuse   , avgu, avgv, &retval );

                diffuse->r += retval.r;
                diffuse->g += retval.g;
                diffuse->b += retval.b;
                diffuse->a += retval.a;

                divDiffuse++;
            }

            if ( mat->flags & SPECULAR_ENABLED ) {
                g3dchannel_getColor ( &mat->specular  , avgu, avgv, &retval );

                specular->r += retval.r;
                specular->g += retval.g;
                specular->b += retval.b;
                specular->a += retval.a;

                divSpecular++;
            }

            if ( mat->flags & BUMP_ENABLED ) {
                g3dchannel_getColor ( &mat->bump      , avgu, avgv, &retval );

                bump->r += retval.r;
                bump->g += retval.g;
                bump->b += retval.b;
                bump->a += retval.a;

                divBump++;
            }

            if ( mat->flags & REFLECTION_ENABLED ) {
                g3dchannel_getColor ( &mat->reflection, avgu, avgv, &retval );

                reflection->r += retval.r;
                reflection->g += retval.g;
                reflection->b += retval.b;
                reflection->a += retval.a;

                divReflection++;
            }

            if ( mat->flags & REFRACTION_ENABLED ) {
                g3dchannel_getColor ( &mat->refraction, avgu, avgv, &retval );

                refraction->r += retval.r;
                refraction->g += retval.g;
                refraction->b += retval.b;
                refraction->a += retval.a;

                divRefraction++;
            }
        }

        ltmptex = ltmptex->next;
    }

    if ( divDiffuse ) {
        diffuse->r /= divDiffuse;
        diffuse->g /= divDiffuse;
        diffuse->b /= divDiffuse;
        diffuse->a /= divDiffuse;
    }

    if ( divSpecular ) {
        specular->r /= divSpecular;
        specular->g /= divSpecular;
        specular->b /= divSpecular;
        specular->a /= divSpecular;
    }

    if ( divBump ) {
        bump->r /= divBump;
        bump->g /= divBump;
        bump->b /= divBump;
        bump->a /= divBump;
    }

    if ( divReflection ) {
        reflection->r /= divReflection;
        reflection->g /= divReflection;
        reflection->b /= divReflection;
        reflection->a /= divReflection;
    }

    if ( divRefraction ) {
        refraction->r /= divRefraction;
        refraction->g /= divRefraction;
        refraction->b /= divRefraction;
        refraction->a /= divRefraction;
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
        R3DLIGHT  *rlt = ( R3DLIGHT * ) ltmprlt->data;
        R3DOBJECT *rob = ( R3DOBJECT * )rlt;
        G3DOBJECT *objlig = rob->obj;
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
            r3dray_shoot ( &luxray, rsce, discard, nbhop, RAYQUERYHIT | 
                                                          RAYLUXRAY   | 
                                                          RAYQUERYIGNOREBACKFACE );

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

            if ( ( ( objlig->flags & LIGHTCASTSHADOWS ) == 0x00 ) ||
                 ( ( luxray.flags  & INTERSECT        ) == 0x00 ) ) {
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
uint32_t r3dray_intersectOctree ( R3DRAY *ray, R3DMESH   *rms,
                                               R3DOCTREE *rot,/*
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
                                                    rms,
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
                    if ( r3dface_intersectRay ( rfc, rms->rver, ray, query_flags ) ) {
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
                     ( rob->obj->type  & TEXT        ) ||
                     ( rob->obj->type  & SPLINE      ) ||
                     ( rob->obj->type  & PRIMITIVE   ) ) {
                    R3DPOINT ptin, ptout;

                    if ( r3dray_inOctree ( ray, oct, &ptin, &ptout ) ) {
                        /*R3DPOINT raymin, raymax;

                        r3dpoint_getMinMax ( &ptin, &ptout, &raymin, &raymax );*/

                        if ( r3dray_intersectOctree ( ray, rob,
                                                           oct,/* 
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
            G3DRGBA diffuse, specular, bump, reflection, refraction;
            R3DOBJECT *rob = hitrob;
            R3DMESH *rms = ( rob->obj->type & MESH ) ? ( R3DMESH * ) rob : NULL;
            LIST *ltex = ((G3DMESH*)hitrob->obj)->ltex;

            uint32_t nbmap = ( rms ) ? rms->nbmap : 0x00;

            if ( query_flags & RAYSTART ) {
                hitrfc->flags |= RFACEHITATSTART;

                rsce->area.rfc[(ray->y*rsce->area.width)+ray->x] = hitrfc;
            }

            r3dray_getHitFaceColor ( ray, rms,
                                          hitrfc,
                                         &diffuse,
                                         &specular,
                                         &bump,
                                         &reflection,
                                         &refraction,
                                          ltex, 
                                          query_flags );

            /*** if outline lighting is enabled, we draw the outline before ***/
            /*** lighting computation ***/
            if ( query_flags & RAYQUERYOUTLINE ) {
                if ( query_flags & RAYSTART ) {
                    if ( rsce->wireframeLighting ) {
                        r3dray_outline ( ray, rms, 
                                              hitrfc, 
                                             &diffuse,
                                              rsce->wireframeColor,
                                              rsce->wireframeThickness,
                                              query_flags );
                    }
                }
            }

            /*if ( ( ray->flags & OUTLINED ) && 
                 ( rsce->wireframeLighting == 0x00 ) ) {
                return g3drgba_toLong ( &diffuse );
            }*/

            if ( query_flags & RAYQUERYREFLECTION ) {
                float reflectionStrength = ( ( reflection.r + 
                                               reflection.g + 
                                               reflection.b ) / 0x03 ) / 255.0f;
                uint32_t retcol;
                R3DRAY flxray;
                uint32_t i;

                if ( reflectionStrength > 0.0f ) {
                    /*** build the reflexion ray from the current ray ***/
                    if ( r3dray_reflect ( ray, &flxray ) ) {
                        retcol = r3dray_shoot ( &flxray, rsce, 
                                                         hitrfc, 
                                                         nbhop + 0x01,
                                                         RAYQUERYHIT        | 
                                                         RAYQUERYLIGHTING   |
                                                         RAYQUERYREFLECTION |
                                                         RAYQUERYREFRACTION );

                        if ( reflectionStrength == 1.0f ) {
                            diffuse.r = ( retcol & 0x00FF0000 ) >> 0x10;
                            diffuse.g = ( retcol & 0x0000FF00 ) >> 0x08;
                            diffuse.b = ( retcol & 0x000000FF );
                        } else {
                            float sub = ( 1.0f - reflectionStrength );
                            uint32_t REFR = ( retcol & 0x00FF0000 ) >> 0x10,
                                     REFG = ( retcol & 0x0000FF00 ) >> 0x08,
                                     REFB = ( retcol & 0x000000FF );

                            diffuse.r = ( REFR * reflectionStrength ) + ( diffuse.r * sub );
                            diffuse.g = ( REFG * reflectionStrength ) + ( diffuse.g * sub );
                            diffuse.b = ( REFB * reflectionStrength ) + ( diffuse.b * sub );
                        }
                    }
                }
            }
#ifdef unused
            if ( query_flags & RAYQUERYREFRACTION ) {
                uint32_t i;

                for ( i = 0x00; i < nbmap; i++ ) {
                    LIST *ltmptex = ((G3DMESH*)hitrob->obj)->ltex;

                    while ( ltmptex ) {
                        G3DTEXTURE  *tex = ( G3DTEXTURE * ) ltmptex->data;
                        G3DMATERIAL *mat = tex->mat;
                        LIST *ltmpuvs = /*hitrfc->ancestorFace->luvs*/NULL;

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
#endif
            if ( query_flags & RAYQUERYLIGHTING ) {
                r3dray_illumination ( ray, rsce, &col, &spc, hitrfc, nbhop + 0x01, ltex  );
            }

            diffuse.r = ( uint32_t ) ( ( col.r * diffuse.r ) >> 0x08 ) + ( ( spc.r * specular.r ) >> 0x08 );
            diffuse.g = ( uint32_t ) ( ( col.g * diffuse.g ) >> 0x08 ) + ( ( spc.g * specular.g ) >> 0x08 );
            diffuse.b = ( uint32_t ) ( ( col.b * diffuse.b ) >> 0x08 ) + ( ( spc.b * specular.b ) >> 0x08 );

            /*** if outline lighting is disabled, we draw the outline after ***/
            /*** lighting computation ***/
            if ( query_flags & RAYQUERYOUTLINE ) {
                if ( query_flags & RAYSTART ) {
                    if ( rsce->wireframeLighting == 0x00 ) {
                        r3dray_outline ( ray, rms, 
                                              hitrfc, 
                                             &diffuse,
                                              rsce->wireframeColor,
                                              rsce->wireframeThickness,
                                              query_flags );
                    }
                }
            }

            if ( diffuse.r > 0xFF ) diffuse.r = 0xFF;
            if ( diffuse.g > 0xFF ) diffuse.g = 0xFF;
            if ( diffuse.b > 0xFF ) diffuse.b = 0xFF;
            /*** End of the lighting part ***/

            return ( ( uint32_t ) ( diffuse.r << 0x10 ) | 
                                  ( diffuse.g << 0x08 ) | 
                                  ( diffuse.b         ) );
        }
    }

    if ( rsce->backgroundMode & BACKGROUND_IMAGE ) {
        float x = ( rsce->area.width  ) ? ( float ) ray->x / rsce->area.width  : 0.0f,
              y = ( rsce->area.height ) ? ( float ) ray->y / rsce->area.height : 0.0f;
        uint32_t inty = ( y * rsce->backgroundImage.height ),
                 intx = ( x * rsce->backgroundImage.width  );
        uint32_t offset =  ( inty * rsce->backgroundImage.width ) + intx;

        switch ( rsce->backgroundImage.depth ) {
            case 0x03 : {
                unsigned char (*imgdata)[0x03] = rsce->backgroundImage.data;
                uint32_t R = ( uint32_t ) imgdata[offset][0x00],
                         G = ( uint32_t ) imgdata[offset][0x01],
                         B = ( uint32_t ) imgdata[offset][0x02];

                return ( R  << 0x10 ) | ( G  << 0x08 ) | B ;
            } break;

            default :
            break;
        }

        return 0x00;
    }

    return rsce->backgroundColor;
}









