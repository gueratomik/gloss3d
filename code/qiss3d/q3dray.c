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
/*** Perfect spheres cannot shadow themselves. This helps us preventing self **/
/*** shadowing that generates lots of small dots despite all precautions ***/
static uint32_t excludeIfPerfectSphere ( Q3DOBJECT *qobj, void *data ) {
    Q3DOBJECT *hitqobj = ( Q3DOBJECT * ) data;

    if ( qobj == hitqobj ) {
        if ( hitqobj->obj->type == G3DSPHERETYPE ) {
            if ( hitqobj->obj->flags & SPHEREISPERFECT ) {
                return 0x00;
            }
        }
    }

    return 0x01;
}

/******************************************************************************/
/************** http://http.developer.nvidia.com/Cg/refract.html **************/
/******************************************************************************/
static void q3dray_refract ( Q3DRAY *qray,
                             Q3DRAY *qout,
                             float   eta ) {
    float cosi = -q3dvector3f_scalar ( &qray->dir, &qray->isx.dir );
    float cost = 1.0f - ( ( eta * eta ) * ( 1.0f - ( cosi * cosi ) ) );

    memset ( qout, 0x00, sizeof ( Q3DRAY ) );

    /*** init the depth value for depth sorting ***/
    qout->distance = INFINITY;

    /*** refracted ray origin is parent ray intersection point ***/
    memcpy ( &qout->src, &qray->isx.src, sizeof ( Q3DVECTOR3F ) );

    qout->x = qray->x;
    qout->y = qray->y;

    if ( cost > 0.0f ) {
        float cabs = sqrt ( cost );

        qout->dir.x = (eta * qray->dir.x) + (((eta * cosi) - cabs) * qray->isx.dir.x);
        qout->dir.y = (eta * qray->dir.y) + (((eta * cosi) - cabs) * qray->isx.dir.y);
        qout->dir.z = (eta * qray->dir.z) + (((eta * cosi) - cabs) * qray->isx.dir.z);
    } else {
        memcpy ( &qout->dir, &qray->dir, sizeof ( Q3DVECTOR3F ) );
    }

    q3dvector3f_normalize ( &qout->dir, NULL );
}

/******************************************************************************/
static uint32_t q3dray_reflect ( Q3DRAY          *qray,
                                 Q3DRAY          *qout ) {
    float dot = q3dvector3f_scalar ( ( Q3DVECTOR3F * ) &qray->dir,
                                     ( Q3DVECTOR3F * ) &qray->isx.dir );

    if ( dot > 0.0f ) {
        return 0x00;
    } else {
        float dotby2 = dot * 2.0f;

        memset ( qout, 0x00, sizeof ( Q3DRAY ) );

        /*** init the depth value for depth sorting ***/
        qout->distance = INFINITY;

        /*** reflected ray origin is parent ray intersection point ***/
        memcpy ( &qout->src, &qray->isx.src, sizeof ( Q3DVECTOR3F ) );

        qout->x = qray->x;
        qout->y = qray->y;

        /*** reflection formula ***/
        qout->dir.x = qray->dir.x - ( dotby2 * qray->isx.dir.x );
        qout->dir.y = qray->dir.y - ( dotby2 * qray->isx.dir.y );
        qout->dir.z = qray->dir.z - ( dotby2 * qray->isx.dir.z );

        q3dvector3f_normalize ( &qout->dir, NULL );
    }

    return 0x01;
}

/******************************************************************************/
void q3dray_bump ( Q3DRAY      *qray,
                   float        frame,
                   LIST        *ltex ) {
    LIST *ltmptex = ltex;
    Q3DVECTOR3F totbump = { 0.0f, 0.0f, 0.0f };
    uint32_t total = 0x00;

    while ( ltmptex ) {
        G3DTEXTURE  *tex = ( G3DTEXTURE * ) ltmptex->data;
        G3DMATERIAL *mat = tex->mat;
        uint32_t repeat = ( tex->flags & TEXTUREREPEATED ) ? 0x01 : 0x00;

        if ( tex->map ) {
            uint32_t mapID = tex->map->mapID;
            float avgu = 0.0f,
                  avgv = 0.0f;

            switch ( tex->map->projection ) {
                /*
                 * In background mapping mode, we don't want any perspective
                 * distorsion applied to the texture. That's why we retrieve
                 * the UV coordinates from the screen.
                 */
                case UVMAPBACKGROUND : {
                    /*** currently unsupported ****/
                } break;

                default: {
                    avgu = ( ( qray->isx.qsur->tri.quvs[mapID].uv[0x00].u * qray->ratio[0x00] ) +
                             ( qray->isx.qsur->tri.quvs[mapID].uv[0x01].u * qray->ratio[0x01] ) +
                             ( qray->isx.qsur->tri.quvs[mapID].uv[0x02].u * qray->ratio[0x02] ) ),
                    avgv = ( ( qray->isx.qsur->tri.quvs[mapID].uv[0x00].v * qray->ratio[0x00] ) +
                             ( qray->isx.qsur->tri.quvs[mapID].uv[0x01].v * qray->ratio[0x01] ) +
                             ( qray->isx.qsur->tri.quvs[mapID].uv[0x02].v * qray->ratio[0x02] ) );
                } break;
            }

            if ( mat ) {
                if ( mat->flags & BUMP_ENABLED ) {
                    float strength = mat->bump.solid.r;
                    Q3DVECTOR3F tangent;
                    Q3DVECTOR3F binomial;
                    G3DVECTOR bumpnor;
                    G3DVECTOR finbump;
                    Q3DMESH *qmes = ( Q3DMESH * ) qray->isx.qobj;
                    Q3DVERTEX *qver = q3dmesh_getVertices ( qmes, frame );
                    Q3DVECTOR3F *p0 = &qver[qray->isx.qsur->tri.qverID[0x00]].pos,
                                *p1 = &qver[qray->isx.qsur->tri.qverID[0x01]].pos,
                                *p2 = &qver[qray->isx.qsur->tri.qverID[0x02]].pos;
                    Q3DUVSET *quvs = &qray->isx.qsur->tri.quvs[mapID];
                    Q3DUV *w0 = &quvs->uv[0x00],
                          *w1 = &quvs->uv[0x01],
                          *w2 = &quvs->uv[0x02];
                    double TBN[0x09];
                /*** see "Computing Tangent Space Basis Vectors for an Arbitrary Mesh" ***/
                    /*** XYZ system ***/
                    float x1 = p1->x - p0->x;
                    float x2 = p2->x - p0->x;
                    float y1 = p1->y - p0->y;
                    float y2 = p2->y - p0->y;
                    float z1 = p1->z - p0->z;
                    float z2 = p2->z - p0->z;
                    /*** UVs  system ****/
                    float s1 = w1->u - w0->u;
                    float s2 = w2->u - w0->u;
                    float t1 = w1->v - w0->v;
                    float t2 = w2->v - w0->v;
                    float r = 1.0f / (s1 * t2 - s2 * t1 );
                    tangent.x = (t2 * x1 - t1 * x2) * r;
                    tangent.y = (t2 * y1 - t1 * y2) * r;
                    tangent.z = (t2 * z1 - t1 * z2) * r;

                    binomial.x = (s1 * x2 - s2 * x1) * r, 
                    binomial.y = (s1 * y2 - s2 * y1) * r,
                    binomial.z = (s1 * z2 - s2 * z1) * r;

                    q3dvector3f_normalize ( &tangent , NULL );
                    q3dvector3f_normalize ( &binomial, NULL );

                    TBN[0x00] = tangent.x;
                    TBN[0x01] = tangent.y;
                    TBN[0x02] = tangent.z;
                    TBN[0x03] = binomial.x;
                    TBN[0x04] = binomial.y;
                    TBN[0x05] = binomial.z;
                    TBN[0x06] = qray->isx.dir.x;
                    TBN[0x07] = qray->isx.dir.y;
                    TBN[0x08] = qray->isx.dir.z;

                    g3dchannel_getNormal ( &mat->bump,
                                            avgu,
                                            avgv,
                                           &bumpnor,
                                            repeat,
                                            0.01f,
                                            0.01f,
                                            0x00 );

                    g3dvector_matrix3 ( &bumpnor, TBN, &finbump );

                    totbump.x += ( qray->isx.dir.x * ( 1.0f - strength ) ) + ( finbump.x * strength );
                    totbump.y += ( qray->isx.dir.y * ( 1.0f - strength ) ) + ( finbump.y * strength );
                    totbump.z += ( qray->isx.dir.z * ( 1.0f - strength ) ) + ( finbump.z * strength );

                    total++;
                }
            }
        }

        ltmptex = ltmptex->next;
    }

    if ( total ) {
        qray->isx.dir.x = totbump.x / total;
        qray->isx.dir.y = totbump.y / total;
        qray->isx.dir.z = totbump.z / total;
    }
}

/******************************************************************************/
void q3dray_specular ( Q3DRAY      *qray,
                       Q3DLIGHT    *qlig,
                       Q3DCAMERA   *qcam,
                       Q3DRGBA     *specular ) {
    if ( qray->isx.qobj->obj->type & MESH ) {
        G3DMESH *mes = ( G3DMESH * ) qray->isx.qobj->obj;
        LIST *ltmptex = mes->ltex;
        uint32_t total = 0x00;

        while ( ltmptex ) {
            G3DTEXTURE  *tex = ( G3DTEXTURE * ) ltmptex->data;
            G3DMATERIAL *mat = tex->mat;
            uint32_t repeat = ( tex->flags & TEXTUREREPEATED ) ? 0x01 : 0x00;
            Q3DRGBA retval;
            Q3DRAY spcqray, refqray;

            if ( tex->map ) {
                uint32_t mapID = tex->map->mapID;
                float avgu = 0.0f,
                      avgv = 0.0f;

                switch ( tex->map->projection ) {
                    /*
                     * In background mapping mode, we don't want any perspective
                     * distorsion applied to the texture. That's why we retrieve
                     * the UV coordinates from the screen.
                     */
                    case UVMAPBACKGROUND : {
                        /*** currently unsupported ****/
                    } break;

                    default: {
                        avgu = ( ( qray->isx.qsur->tri.quvs[mapID].uv[0x00].u * qray->ratio[0x00] ) +
                                 ( qray->isx.qsur->tri.quvs[mapID].uv[0x01].u * qray->ratio[0x01] ) +
                                 ( qray->isx.qsur->tri.quvs[mapID].uv[0x02].u * qray->ratio[0x02] ) ),
                        avgv = ( ( qray->isx.qsur->tri.quvs[mapID].uv[0x00].v * qray->ratio[0x00] ) +
                                 ( qray->isx.qsur->tri.quvs[mapID].uv[0x01].v * qray->ratio[0x01] ) +
                                 ( qray->isx.qsur->tri.quvs[mapID].uv[0x02].v * qray->ratio[0x02] ) );
                    } break;
                }

                if ( mat ) {
                    if ( mat->flags & SPECULAR_ENABLED ) {
                        g3dchannel_getColor ( &mat->specular, 
                                               avgu, 
                                               avgv, 
                                              &retval,
                                               repeat );

                        spcqray.src.x = qlig->wpos.x;
                        spcqray.src.y = qlig->wpos.y;
                        spcqray.src.z = qlig->wpos.z;

                        spcqray.dir.x = ( qray->isx.src.x - qlig->wpos.x );
                        spcqray.dir.y = ( qray->isx.src.y - qlig->wpos.y );
                        spcqray.dir.z = ( qray->isx.src.z - qlig->wpos.z );

                        spcqray.isx.src.x = qray->isx.src.x;
                        spcqray.isx.src.y = qray->isx.src.y;
                        spcqray.isx.src.z = qray->isx.src.z;

                        spcqray.isx.dir.x = qray->isx.dir.x;
                        spcqray.isx.dir.y = qray->isx.dir.y;
                        spcqray.isx.dir.z = qray->isx.dir.z;

                        q3dvector3f_normalize ( &spcqray.dir, NULL );

                        if ( q3dray_reflect ( &spcqray, &refqray ) ) {
                            Q3DVECTOR3F qcamdir;
                            float dot;

                            qcamdir.x = ( qcam->wpos.x - qray->isx.src.x );
                            qcamdir.y = ( qcam->wpos.y - qray->isx.src.y );
                            qcamdir.z = ( qcam->wpos.z - qray->isx.src.z );

                            q3dvector3f_normalize ( &qcamdir, NULL );

                            dot = q3dvector3f_scalar ( &refqray.dir,
                                                       &qcamdir ) /** spotFactor*/;

                            if ( dot > 0.0f ) {
                                G3DCOLOR color = { .r = ( float ) retval.r / 255,
                                                   .g = ( float ) retval.g / 255,
                                                   .b = ( float ) retval.b / 255 };
                                G3DLIGHT *lig = q3dobject_getObject ( qlig );
                                float specularCoefficient = pow ( dot, mat->shininess );
                                    uint32_t R = ( lig->specularColor.r * mat->specular_level * color.r * specularCoefficient ),
                                             G = ( lig->specularColor.g * mat->specular_level * color.g * specularCoefficient ),
                                             B = ( lig->specularColor.b * mat->specular_level * color.b * specularCoefficient );

                                specular->r += R;
                                specular->g += G;
                                specular->b += B;
                            }
                        }
                    }
                }
            }

            ltmptex = ltmptex->next;
        }

        if ( specular->r > 0xFF ) specular->r = 0xFF;
        if ( specular->g > 0xFF ) specular->g = 0xFF;
        if ( specular->b > 0xFF ) specular->b = 0xFF;
        if ( specular->a > 0xFF ) specular->a = 0xFF;
    }
}

/******************************************************************************/
/*** Note: Source is in World coordinates ***/
uint32_t q3dray_illumination ( Q3DRAY          *qray,
                               Q3DJOB          *qjob,
                               Q3DSURFACE      *sdiscard,
                               float            frame,
                               Q3DRGBA         *diffuse,
                               Q3DRGBA         *specular,
                               uint32_t         nbhop,
                               uint32_t         query_flags ) {
    static Q3DVECTOR3F pzero = { .x = 0.0f, .y = 0.0f, .z = 0.0f };
    LIST *ltmpqlig = qjob->qsce->llights;

    memset ( diffuse , 0x00, sizeof ( Q3DRGBA ) );

    while ( ltmpqlig ) {
        Q3DLIGHT  *qlig    = ( Q3DLIGHT *  ) ltmpqlig->data;
        Q3DOBJECT *qobjlig = ( Q3DOBJECT * ) qlig;
        G3DLIGHT  *lig     = ( G3DLIGHT *  ) q3dobject_getObject ( qobjlig );
         /*** QLight world position ***/
        float distancetoLight;
        Q3DVECTOR3F qligwpos;
        Q3DRAY luxqray;
        Q3DRAY refqray;
        Q3DRAY spcqray;
        float shadow = 0.0f;
        float dot;
        uint32_t i;

        q3dvector3f_matrix ( &pzero, qobjlig->obj->wmatrix, &qligwpos );

        luxqray.src.x   = qray->isx.src.x;
        luxqray.src.y   = qray->isx.src.y;
        luxqray.src.z   = qray->isx.src.z;

        luxqray.dir.x   = qligwpos.x - luxqray.src.x;
        luxqray.dir.y   = qligwpos.y - luxqray.src.y;
        luxqray.dir.z   = qligwpos.z - luxqray.src.z;

        luxqray.distance = INFINITY;
        luxqray.flags    = 0x00;

        q3dvector3f_normalize ( &luxqray.dir, &distancetoLight );

        dot = q3dvector3f_scalar ( &luxqray.dir, &qray->isx.dir );

        if ( query_flags & RAYQUERYHARDSHADOWS ) {
            if ( ((G3DOBJECT*)lig)->flags & LIGHTCASTSHADOWS ) {
                if ( dot > 0.0f ) {
                    if ( ((G3DOBJECT*)lig)->flags & LIGHTHARDSHADOWS ) {
                        q3dray_shoot_r ( &luxqray, 
                                          qjob,
                                          sdiscard,
                          /** problem here with instanciated objects. To solve later ***/
                                          excludeIfPerfectSphere,
                                          qray->isx.qobj,
                                          frame,
                                          nbhop,
                                          RAYQUERYHIT );

                        if ( luxqray.flags & Q3DRAY_HAS_HIT_BIT ) {
                            if ( luxqray.distance < distancetoLight ) {
                                shadow = 1.0f;
                            }
                        }
                    }
                }
            }
        }

        if ( query_flags & RAYQUERYSOFTSHADOWSPASS1 ) {
            if ( ((G3DOBJECT*)lig)->flags & LIGHTSOFTSHADOWS ) {
                Q3DRAY areaqray = { .src = { .x = luxqray.src.x,
                                             .y = luxqray.src.y,
                                             .z = luxqray.src.z } };
                float areashadow = 0.0f;

                for ( i = 0x00; i < lig->shadowSample; i++ ) {
                    Q3DVECTOR3F areadst;
                    float areadot;

                    q3dplane_getRandomPoint ( &luxqray.dir,
                                              &qligwpos,
                                              lig->shadowRadius,
                                              &areadst );

                    areaqray.dir.x = areadst.x - areaqray.src.x;
                    areaqray.dir.y = areadst.y - areaqray.src.y;
                    areaqray.dir.z = areadst.z - areaqray.src.z;

                    areaqray.distance = INFINITY;
                    areaqray.flags    = 0x00;

                    q3dvector3f_normalize ( &areaqray.dir, &distancetoLight );

                    areadot = q3dvector3f_scalar ( &areaqray.dir, &qray->isx.dir );

                    if ( areadot > 0.0f ) {
                        q3dray_shoot_r ( &areaqray, 
                                          qjob,
                                          sdiscard,
                                          excludeIfPerfectSphere,
                                          qray->isx.qobj,
                                          frame,
                                          nbhop,
                                          RAYQUERYHIT );

                        if ( areaqray.flags & Q3DRAY_HAS_HIT_BIT ) {
                            if ( areaqray.distance < distancetoLight ) {
                                areashadow += 1.0f;
                            }
                        }
                    }
                }

                shadow = ( float ) areashadow / lig->shadowSample;

                if ( qray->flags & Q3DRAY_PRIMARY_BIT ) {
                    uint32_t offset = ( qray->y * qjob->qarea.width ) + 
                                        qray->x;

                    qjob->qarea.qssh[offset].shadow += ( shadow );

                    qjob->qarea.qssh[offset].count++;
                }
            }
        }

        if ( query_flags & RAYQUERYSOFTSHADOWSPASS2 ) {
            if ( qray->flags & Q3DRAY_PRIMARY_BIT ) {
                if ( ((G3DOBJECT*)lig)->flags & LIGHTSOFTSHADOWS ) {
                    uint32_t offset = ( qray->y * qjob->qarea.width ) + 
                                        qray->x;

                    if ( qjob->qarea.qssh[offset].count ) {
                        shadow += qjob->qarea.qssh[offset].average;
                    } else {
                        shadow += 0.0f;
                    }
                }
            }
        }

        if ( shadow > 1.0f ) shadow = 1.0f;

        if ( query_flags & RAYQUERYSHADING ) {
            if ( lig->obj.flags & SPOTLIGHT ) {
                float spotAngle;
                float spotDot;
                float spotFactor;
                Q3DVECTOR3F invLuxRay = { .x = -luxqray.dir.x,
                                          .y = -luxqray.dir.y,
                                          .z = -luxqray.dir.z };

                spotDot = q3dvector3f_scalar ( &invLuxRay,
                                               &qlig->zvec );

                if ( spotDot > 0.0f ) {
                    spotAngle = acos ( spotDot )  * 180 / M_PI;

                    if ( spotAngle < lig->spotAngle ) {
                        spotFactor = 1.0f;
                    } else {
                        if ( spotAngle < ( lig->spotAngle + lig->spotFadeAngle ) ) {
                            spotFactor = 1.0f - ( ( spotAngle - lig->spotAngle ) / 
                                                                lig->spotFadeAngle );
                        } else {
                            spotFactor = 0.0f;
                        }
                    }
                }

                dot *= spotFactor;
            }

            if ( dot > 0.0f ) {
                float rate = dot * lig->intensity;

                q3dray_specular ( qray, qlig, qjob->qcam, specular );

                diffuse->r += ( ( lig->diffuseColor.r * rate ) * ( 1.0f - shadow ) ),
                diffuse->g += ( ( lig->diffuseColor.g * rate ) * ( 1.0f - shadow ) ),
                diffuse->b += ( ( lig->diffuseColor.b * rate ) * ( 1.0f - shadow ) );
            }
        }

        ltmpqlig = ltmpqlig->next;
    }

    if ( diffuse->r > 0xFF ) diffuse->r = 0xFF;
    if ( diffuse->g > 0xFF ) diffuse->g = 0xFF;
    if ( diffuse->b > 0xFF ) diffuse->b = 0xFF;
}

/******************************************************************************/
uint32_t q3dray_getSurfaceColor ( Q3DRAY      *qray,
                                  Q3DMESH     *qmes,
                                  Q3DTRIANGLE *qtri,
                                  Q3DAREA     *qarea,
                                  float        backgroundImageWidthRatio,
                                  Q3DRGBA     *defaultColor,
                                  Q3DRGBA     *diffuse,
                                  Q3DRGBA     *specular,
                                  Q3DRGBA     *reflection,
                                  Q3DRGBA     *refraction,
                                  Q3DRGBA     *alpha,
                                  float       *alphaOpacity,
                                  LIST        *ltex,
                                  uint32_t     query_flags ) {
    uint32_t divDiffuse    = 0x00;
    uint32_t divSpecular   = 0x00;
    uint32_t divBump       = 0x00;
    uint32_t divReflection = 0x00;
    uint32_t divRefraction = 0x00;
    uint32_t divAlpha      = 0x00;
    LIST *ltmptex = ltex;
    uint32_t i;

    memset ( diffuse   , 0x00, sizeof ( Q3DRGBA   ) );
    memset ( specular  , 0x00, sizeof ( Q3DRGBA   ) );
    memset ( reflection, 0x00, sizeof ( Q3DRGBA   ) );
    memset ( refraction, 0x00, sizeof ( Q3DRGBA   ) );
    memset ( alpha     , 0x00, sizeof ( Q3DRGBA   ) );

    (*alphaOpacity) = 0.0f;

    while ( ltmptex ) {
        G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;
        G3DMATERIAL *mat = tex->mat;
        uint32_t repeat = ( tex->flags & TEXTUREREPEATED ) ? 0x01 : 0x00;
        float avgu = 0.0f,
              avgv = 0.0f;
        Q3DRGBA retval;

        if ( tex->flags & TEXTURERESTRICTED ) {
            if ( q3dtriangle_hasTextureSlot ( qtri, tex->slotBit  ) == 0x00 ) {
                ltmptex = ltmptex->next;

                continue;
            }
        }

        if ( tex->map && qtri->quvs ) {
            uint32_t mapID = tex->map->mapID;

            switch ( tex->map->projection ) {
                /*
                 * In background mapping mode, we don't want any perspective
                 * distorsion applied to the texture. That's why we retrieve
                 * the UV coordinates from the screen.
                 */
                case UVMAPBACKGROUND : {
                    uint32_t backgroundWidth = ( qarea->width * 
                                                 backgroundImageWidthRatio );
                    int32_t deltaWidth = ( int32_t ) ( qarea->width - backgroundWidth ) / 2;

                    if ( ( qray->x > deltaWidth ) && 
                         ( qray->x < ( qarea->width - deltaWidth ) ) ) {
                        avgu = ( backgroundWidth ) ? ( float ) ( qray->x - deltaWidth ) / backgroundWidth : 0.0f,
                        avgv = ( qarea->height   ) ? ( float )   qray->y / qarea->height : 0.0f;

                        if ( ( avgu < 0.0f ) || ( avgu > 1.0f ) ||
                             ( avgv < 0.0f ) || ( avgv > 1.0f ) ) {
                            return 0x00;
                        }
                    }
                } break;

                default: {
                    avgu = ( ( qtri->quvs[mapID].uv[0x00].u * qray->ratio[0x00] ) +
                             ( qtri->quvs[mapID].uv[0x01].u * qray->ratio[0x01] ) +
                             ( qtri->quvs[mapID].uv[0x02].u * qray->ratio[0x02] ) ),
                    avgv = ( ( qtri->quvs[mapID].uv[0x00].v * qray->ratio[0x00] ) +
                             ( qtri->quvs[mapID].uv[0x01].v * qray->ratio[0x01] ) +
                             ( qtri->quvs[mapID].uv[0x02].v * qray->ratio[0x02] ) );
                } break;
            }
        }

        if ( mat->flags & DIFFUSE_ENABLED ) {
            g3dchannel_getColor ( &mat->diffuse, avgu, avgv, &retval, repeat );

            diffuse->r += retval.r;
            diffuse->g += retval.g;
            diffuse->b += retval.b;
            diffuse->a += retval.a;

            divDiffuse++;
        }

        if ( mat->flags & SPECULAR_ENABLED ) {
            g3dchannel_getColor ( &mat->specular, avgu, avgv, &retval, repeat );

            specular->r += retval.r;
            specular->g += retval.g;
            specular->b += retval.b;
            specular->a += retval.a;

            divSpecular++;
        }

        if ( mat->flags & REFLECTION_ENABLED ) {
            g3dchannel_getColor ( &mat->reflection, avgu, avgv, &retval, repeat );

            reflection->r += retval.r;
            reflection->g += retval.g;
            reflection->b += retval.b;
            reflection->a += retval.a;

            divReflection++;
        }

        if ( mat->flags & ALPHA_ENABLED ) {
            g3dchannel_getColor ( &mat->alpha, avgu, avgv, &retval, repeat );

            alpha->r += retval.r;
            alpha->g += retval.g;
            alpha->b += retval.b;

            (*alphaOpacity) += mat->alphaOpacity;

            divAlpha++;
        }

        if ( mat->flags & REFRACTION_ENABLED ) {
            g3dchannel_getColor ( &mat->refraction, avgu, avgv, &retval, repeat );

            refraction->r += retval.r;
            refraction->g += retval.g;
            refraction->b += retval.b;
            refraction->a += retval.a;

            divRefraction++;
        }

        ltmptex = ltmptex->next;
    }

    if ( divDiffuse ) {
        diffuse->r /= divDiffuse;
        diffuse->g /= divDiffuse;
        diffuse->b /= divDiffuse;
        diffuse->a /= divDiffuse;
    } else {
        diffuse->r = defaultColor->r;
        diffuse->g = defaultColor->g; 
        diffuse->b = defaultColor->b;
    }

    if ( divSpecular ) {
        specular->r /= divSpecular;
        specular->g /= divSpecular;
        specular->b /= divSpecular;
        specular->a /= divSpecular;
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

    if ( divAlpha ) {
        (*alphaOpacity) /= ( divAlpha );

        alpha->r /= ( divAlpha );
        alpha->g /= ( divAlpha );
        alpha->b /= ( divAlpha );

        alpha->a = 0xFF; /*** store if alpha channel is neede or not ***/
    } else {
        (*alphaOpacity) = 1.0f;

     /*** needed for qray->objectTransparency below ***/
        alpha->r = 0xFF;
        alpha->g = 0xFF;
        alpha->b = 0xFF;

        alpha->a = 0x00;
    }

    if ( qray->objectTransparency > 0.0f ) {
        float objectOpacity = ( 1.0f - qray->objectTransparency );

        (*alphaOpacity) *= objectOpacity;

        alpha->r *= objectOpacity;
        alpha->g *= objectOpacity;
        alpha->b *= objectOpacity;

        alpha->a = 0xFF;
    }

    return 0x01;
}

/******************************************************************************/
static uint32_t onlyPerfectSpheres ( Q3DOBJECT *qobj, void *data ) {
    if ( qobj->obj->type == G3DSPHERETYPE ) {
        if ( qobj->obj->flags & SPHEREISPERFECT ) {
            return 0x01;
        }
    }

    return 0x00;
}

/******************************************************************************/
/*************** https://gamedev.stackexchange.com/questions/72528 ************/
void q3dray_outline ( Q3DRAY *qray,
                      Q3DJOB *qjob,
                      double  *WMVX,
                      double  *PJX,
                      int     *VPX,
                      float    zNear,
                      uint32_t wireframeThickness,
                      Q3DRGBA *diffuse,
                      uint32_t wireframeColor,
                      float    frame  ) {
    if ( ( qray->flags & Q3DRAY_HAS_HIT_BIT ) &&
         ( qray->flags & Q3DRAY_PRIMARY_BIT ) ) {
        Q3DOBJECT *qobj = qray->isx.qobj;

        if ( qobj->obj->type & MESH ) {
            Q3DMESH *qmes = ( Q3DMESH * ) qobj;
            Q3DVERTEXSET *qverset = q3dmesh_getVertexSet ( qmes, frame );
            Q3DTRIANGLE *qtri = qray->isx.qsur;
            Q3DVECTOR3F p[0x04];
            uint32_t j, nbver = 0x00;
            float avgdist = 0.0f;
            uint32_t nbdist = 0x00;

            /*** we need to rebuild the quads otherwise there are some artefacts ***/
            if ( qtri->flags & TRIANGLEFROMQUAD ) {
                Q3DTRIANGLE *qtrione = qtri;
                /*** in case rfc is created from a quad that was splitted in ***/
                /*** 2 triangles, the next triangle is the second part of ***/
                /*** the split ***/
                Q3DTRIANGLE *qtritwo = ( qtri->flags & TRIANGLEFROMQUADONE ) ? qtri + 0x01 :
                                                                               qtri - 0x01;

                p[0x00].x = qverset->qver[qtrione->qverID[0x00]].pos.x;
                p[0x00].y = qverset->qver[qtrione->qverID[0x00]].pos.y;
                p[0x00].z = qverset->qver[qtrione->qverID[0x00]].pos.z;

                p[0x01].x = qverset->qver[qtrione->qverID[0x01]].pos.x;
                p[0x01].y = qverset->qver[qtrione->qverID[0x01]].pos.y;
                p[0x01].z = qverset->qver[qtrione->qverID[0x01]].pos.z;

                p[0x02].x = qverset->qver[qtritwo->qverID[0x00]].pos.x;
                p[0x02].y = qverset->qver[qtritwo->qverID[0x00]].pos.y;
                p[0x02].z = qverset->qver[qtritwo->qverID[0x00]].pos.z;

                p[0x03].x = qverset->qver[qtritwo->qverID[0x01]].pos.x;
                p[0x03].y = qverset->qver[qtritwo->qverID[0x01]].pos.y;
                p[0x03].z = qverset->qver[qtritwo->qverID[0x01]].pos.z;

                nbver = 0x04;
            } else {
                p[0x00].x = qverset->qver[qtri->qverID[0x00]].pos.x;
                p[0x00].y = qverset->qver[qtri->qverID[0x00]].pos.y;
                p[0x00].z = qverset->qver[qtri->qverID[0x00]].pos.z;

                p[0x01].x = qverset->qver[qtri->qverID[0x01]].pos.x;
                p[0x01].y = qverset->qver[qtri->qverID[0x01]].pos.y;
                p[0x01].z = qverset->qver[qtri->qverID[0x01]].pos.z;

                p[0x02].x = qverset->qver[qtri->qverID[0x02]].pos.x;
                p[0x02].y = qverset->qver[qtri->qverID[0x02]].pos.y;
                p[0x02].z = qverset->qver[qtri->qverID[0x02]].pos.z;

                nbver = 0x03;
            }


            for ( j = 0x00; j < nbver; j++ ) {
                uint32_t n = ( j + 0x01 ) % nbver;
                Q3DVECTOR3F *vaf = &p[j],
                            *vbf = &p[n];
                Q3DVECTOR3D vad, vbd;

                gluProject ( vaf->x, 
                             vaf->y, 
                             vaf->z,
                             WMVX,
                             PJX,
                             VPX,
                            &vad.x,
                            &vad.y,
                            &vad.z );

                gluProject ( vbf->x, 
                             vbf->y, 
                             vbf->z,
                             WMVX,
                             PJX,
                             VPX,
                            &vbd.x,
                            &vbd.y,
                            &vbd.z );

                vad.y = ( qjob->qarea.height - ( int32_t ) vad.y );
                vbd.y = ( qjob->qarea.height - ( int32_t ) vbd.y );

                int32_t x1 = vad.x,
                        y1 = vad.y,
                        x2 = vbd.x,
                        y2 = vbd.y;
                float dist;

                /* skip the central edge for former quads */
                /*if ( ( rfc->flags & RFACEFROMQUAD ) && 
                     ( ( i == 0x02 ) && ( n == 0x00 ) ) ) {
                    continue;
                }*/

            /*https://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line */
                dist = abs ( ( ( y2 - y1 ) * qray->x ) - 
                             ( ( x2 - x1 ) * qray->y ) + ( x2 * y1 ) - 
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

                    qray->flags |= Q3DRAY_OUTLINED_BIT;
                }
            }
        }

/*
                Q3DVECTOR3F vAB = { .x = qverset->qver[vbID].pos.x - qverset->qver[vaID].pos.x,
                                    .y = qverset->qver[vbID].pos.y - qverset->qver[vaID].pos.y,
                                    .z = qverset->qver[vbID].pos.z - qverset->qver[vaID].pos.z },
                            vAC = { .x = qray->isx.locsrc.x - qverset->qver[vaID].pos.x,
                                    .y = qray->isx.locsrc.y - qverset->qver[vaID].pos.y,
                                    .z = qray->isx.locsrc.z - qverset->qver[vaID].pos.z };
                double scalABAC    = q3dvector3f_scalar ( &vAC, &vAB );
                double scalABAB    = q3dvector3f_scalar ( &vAB, &vAB );

                if ( scalABAB ) {
                    double t = scalABAC / scalABAB;

                    if ( ( t >= 0.0f ) && ( t <= 1.0f ) ) {
                        Q3DVECTOR3F orthoC = { .x = qverset->qver[vaID].pos.x + ( vAB.x * t ),
                                               .y = qverset->qver[vaID].pos.y + ( vAB.y * t ),
                                               .z = qverset->qver[vaID].pos.z + ( vAB.z * t ) };
                        double xscr, yscr, zscr;
                        int32_t ox, oy;
                        int32_t  dist;

                        gluProject ( orthoC.x, 
                                     orthoC.y, 
                                     orthoC.z,
                                     WMVX,
                                     PJX,
                                     VPX,
                                    &xscr,
                                    &yscr,
                                    &zscr );

    yscr = ( qjob->qarea.height - ( int32_t ) yscr );
*/

                        /*if ( zscr > zNear ) {
                            ox = ( (int32_t ) xscr - (int32_t ) qray->x );
                            oy = ( (int32_t ) yscr - (int32_t ) qray->y );

                            dist = sqrt ( ( ox * ox ) + ( oy * oy ) );

                            if ( dist <= wireframeThickness ) {
                                qray->flags |= Q3DRAY_OUTLINED_BIT;
                            }
                        }*/
    }
}

/******************************************************************************/
uint32_t q3dray_shoot_r ( Q3DRAY     *qray, 
                          Q3DJOB     *qjob,
                          Q3DSURFACE *sdiscard,
                          uint32_t  (*cond)(Q3DOBJECT*,
                                            void *),
                          void       *condData,
                          float       frame,
                          uint32_t    nbhop,
                          uint32_t    query_flags ) {
    Q3DOBJECT   *qobj = NULL;
    Q3DMESH     *qmes = NULL;
    Q3DTRIANGLE *qtri = NULL;
    G3DMESH     *mes  = NULL;
    uint32_t raytrace = 0x00;
    uint32_t updateZ = 0x00;

    if ( query_flags & RAYQUERYHIT ) {
        if ( qray->flags & Q3DRAY_PRIMARY_BIT ) {
            Q3DZBUFFER zout;

            /*** Remove the flag for raytraced perfect spheres ***/
            qray->flags &= (~Q3DRAY_PRIMARY_BIT);

            /*** raytrace perfect spheres as they are not rasterized ***/
            q3dray_shoot_r ( qray,
                             qjob,
                             sdiscard,
                             onlyPerfectSpheres,
                             NULL,
                             frame,
                             nbhop,
                             RAYQUERYHIT );

            /*** add it back ***/
            qray->flags |= Q3DRAY_PRIMARY_BIT;

            if ( qray->flags & Q3DRAY_HAS_HIT_BIT ) {
                qobj = qray->isx.qobj;

                qmes = ( Q3DMESH * ) qobj;

                qtri = qray->isx.qsur;
            }

            q3darea_getZBuffer ( &qjob->qarea, 
                                  qray->x, 
                                  qray->y,
                                 &zout );

            if ( zout.z == INFINITY ) {
                /*** not hit ***/
            } else {
                Q3DOBJECT *qhit = q3dscene_getObjectByID ( qjob->qsce, 
                                                           zout.qobjID );

                if ( qhit->obj->type & MESH ) {
                    Q3DRAY locqray;

                    qmes = ( Q3DMESH * ) qhit;

                    memcpy ( &locqray, qray, sizeof ( Q3DRAY ) );

                    q3dvector3f_matrix ( &qray->src, qhit->IMVX, &locqray.src );
                    q3dvector3f_matrix ( &qray->dir, qhit->TMVX, &locqray.dir );

                    /*** me must launch a ray to get surface color & UV coords ***/
                    if ( q3dtriangle_intersect ( &qmes->qtri[zout.qtriID],
                                                  q3dmesh_getVertices( qmes, frame ),
                                                 &locqray,
                                                  query_flags ) ) {

                        qobj = qhit;
                        qtri = &qmes->qtri[zout.qtriID];

                        qray->flags   |= Q3DRAY_HAS_HIT_BIT;

                        qray->color    = locqray.color;
                        qray->distance = locqray.distance;

                        qray->isx.qobj  = qobj;
                        qray->isx.qsur  = qtri;

                        memcpy ( &qray->isx.locsrc, 
                                 &locqray.isx.src, sizeof ( Q3DVECTOR3F ) );

                        q3dvector3f_matrix ( &locqray.isx.src, qhit->obj->wmatrix, &qray->isx.src );
                        q3dvector3f_matrix ( &locqray.isx.dir, qhit->TIMVX       , &qray->isx.dir );

                        qray->ratio[0x00] = locqray.ratio[0x00];
                        qray->ratio[0x01] = locqray.ratio[0x01];
                        qray->ratio[0x02] = locqray.ratio[0x02];
                    } else {
                        /*** because of rasterization imprecision, there are ***/
                        /*** cases, especially around the edges, were the buffer **/
                        /*** is filled but were the ray does not hit. In that ***/
                        /*** case, we have to raytrace the whole object to get ***/
                        /*** the matching triangle. ***/

                        raytrace = 0x01;
                    }
                }
            }
        } else {
            raytrace = 0x01;
        }

        if ( raytrace ) {
            if ( q3dobject_intersect_r ( qjob->qsce,
                                         qray,
                                         sdiscard,
                                         cond,
                                         condData,
                                         frame,
                                         query_flags,
                                    /*render_flags*/0x00 ) ) {
                qobj = qray->isx.qobj;

                if ( qobj->obj->type & MESH ) {
                    qmes = ( Q3DMESH * ) qobj;

                    qtri = qray->isx.qsur;
                }
            }
        }

        if ( qray->flags & Q3DRAY_HAS_HIT_BIT ) {
            Q3DRGBA materialDiffuse    = { qjob->qrsg->defaultColor.r, 
                                           qjob->qrsg->defaultColor.g, 
                                           qjob->qrsg->defaultColor.b, 0x00 }, 
                    materialSpecular   = { 0x00, 0x00, 0x00, 0x00 },
                    materialReflection = { 0x00, 0x00, 0x00, 0x00 },
                    materialRefraction = { 0x00, 0x00, 0x00, 0x00 },
                    materialAlpha      = { 0xFF, 0xFF, 0xFF, 0xFF };
            Q3DRGBA lightDiffuse       = { 0x00, 0x00, 0x00, 0x00 },
                    lightSpecular      = { 0x00, 0x00, 0x00, 0x00 };
            Q3DRGBA diffuse            = { 0x00, 0x00, 0x00, 0x00 };

            if ( query_flags & RAYQUERYSURFACECOLOR ) {
                G3DMESH *mes  = ( G3DMESH * ) q3dobject_getObject ( qobj );
                float alphaOpacity = 1.0f;

                if ( ( qjob->qrsg->flags & DISABLETEXTURING ) == 0x00 ) {
                    q3dray_getSurfaceColor ( qray, 
                                             qobj,
                                             qtri,
                                            &qjob->qarea,
                                             qjob->qrsg->background.widthRatio,
                                            &qjob->qrsg->defaultColor,
                                            &materialDiffuse,
                                            &materialSpecular,
                                            &materialReflection,
                                            &materialRefraction,
                                            &materialAlpha,
                                            &alphaOpacity,
                                             mes->ltex, 
                                             query_flags );


                    q3dray_bump ( qray,
                                  frame,
                                  mes->ltex );
                }

                if ( ( query_flags & RAYQUERYREFLECTION ) && nbhop ) {
                    if ( ( materialReflection.r > 0x00 ) &&
                         ( materialReflection.g > 0x00 ) &&
                         ( materialReflection.b > 0x00 ) ) {
                        Q3DRAY refqray;

                        if ( q3dray_reflect ( qray,
                                             &refqray ) ) {
                            uint32_t rCol = q3dray_shoot_r ( &refqray,
                                                              qjob,
                                                              qtri,
                                                              excludeIfPerfectSphere,
                                                              qobj,
                                                              frame,
                                                            --nbhop,
                                                              query_flags );
                            Q3DRGBA rRefl = { .r = ( rCol & 0xFF0000 ) >> 0x10,
                                              .g = ( rCol & 0x00FF00 ) >> 0x08,
                                              .b = ( rCol & 0x0000FF ) };
                            Q3DRGBA mDiff = { .r = materialDiffuse.r,
                                              .g = materialDiffuse.g,
                                              .b = materialDiffuse.b };
                            Q3DRGBA mRefl = { .r = materialReflection.r,
                                              .g = materialReflection.g,
                                              .b = materialReflection.b };

                            mDiff.r = ( mDiff.r * ( 0xFF - mRefl.r ) ) + 
                                      ( rRefl.r *          mRefl.r ) >> 0x08;
                            mDiff.g = ( mDiff.g * ( 0xFF - mRefl.g ) ) + 
                                      ( rRefl.g *          mRefl.g ) >> 0x08;
                            mDiff.b = ( mDiff.b * ( 0xFF - mRefl.b ) ) + 
                                      ( rRefl.b *          mRefl.b ) >> 0x08;

                            materialDiffuse.r = mDiff.r;
                            materialDiffuse.g = mDiff.g;
                            materialDiffuse.b = mDiff.b;
                        }
                    }
                }

                if ( ( query_flags & RAYQUERYREFRACTION ) && nbhop ) {
                    float refractionStrength = ( ( materialRefraction.r + 
                                                   materialRefraction.g + 
                                                   materialRefraction.b ) / 0x03 ) / 255.0f;
                    uint32_t i;

                    if ( materialAlpha.a ) {
                        G3DCOLOR opac;
                        Q3DRGBA  bkgd;
                        uint32_t retcol;
                        Q3DRAY frcray;

                        g3drgba_toColor  ( &materialAlpha, &opac );

                        opac.r *= alphaOpacity;
                        opac.g *= alphaOpacity;
                        opac.b *= alphaOpacity;

                        if ( ( opac.r < 1.0f ) ||
                             ( opac.g < 1.0f ) ||
                             ( opac.b < 1.0f ) ) {
                            /*** build the refraction ray from the current ray ***/
                            q3dray_refract ( qray, 
                                            &frcray, 
                                             refractionStrength );

                            q3dvector3f_normalize ( &frcray.dir, NULL );

                            retcol = q3dray_shoot_r ( &frcray, 
                                                       qjob, 
                                                       qtri,
                                                       excludeIfPerfectSphere,
                                                       qobj,
                                                       frame,
                                                     --nbhop,
                                                       query_flags );

                            g3drgba_fromLong ( &bkgd, retcol );


                            materialDiffuse.r = ( materialDiffuse.r * opac.r ) + ( bkgd.r * ( 1.0f - opac.r ) );
                            materialDiffuse.g = ( materialDiffuse.g * opac.g ) + ( bkgd.g * ( 1.0f - opac.g ) );
                            materialDiffuse.b = ( materialDiffuse.b * opac.b ) + ( bkgd.b * ( 1.0f - opac.b ) );
                        }
                    }
                }
            }

            /*** if outline lighting is enabled, we draw the outline before ***/
            /*** lighting computation ***/
            if ( query_flags & RAYQUERYOUTLINE ) {
                if ( qray->flags & Q3DRAY_PRIMARY_BIT ) {
                    if ( qjob->qrsg->flags & WIREFRAMELIGHTING ) {
                        Q3DZBUFFER zout;

                        q3darea_getZBuffer ( &qjob->qarea, 
                                              qray->x, 
                                              qray->y,
                                             &zout );

                        q3dray_outline ( qray,
                                         qjob,
                                         qjob->qarea.qzen.WMVX[zout.wmvxID], 
                                         qjob->qcam->PJX,
                                         qjob->qcam->VPX,
                                         0.01f,
                                         qjob->qrsg->wireframe.thickness,
                                        &materialDiffuse,
                                         qjob->qrsg->wireframe.color,
                                         frame  );
                    }
                }
            }

            if ( ( query_flags & RAYQUERYLIGHTING ) &&
                 ( qobj->obj->flags & OBJECTNOSHADING ) == 0x00 ) {
                q3dray_illumination (  qray,
                                       qjob,
                                       qtri,
                                       frame,
                                      &lightDiffuse,
                                      &lightSpecular,
                                       nbhop,
                                       query_flags );
            } else {
                lightDiffuse.r = lightDiffuse.g = lightDiffuse.b = 255;
                lightSpecular.r = lightSpecular.g = lightSpecular.b = 0;
            }

            diffuse.r = ( uint32_t ) ( ( lightDiffuse.r * materialDiffuse.r ) >> 0x08 ) + lightSpecular.r;
            diffuse.g = ( uint32_t ) ( ( lightDiffuse.g * materialDiffuse.g ) >> 0x08 ) + lightSpecular.g;
            diffuse.b = ( uint32_t ) ( ( lightDiffuse.b * materialDiffuse.b ) >> 0x08 ) + lightSpecular.b;

            /*** if outline lighting is disabled, we draw the outline after ***/
            /*** lighting computation ***/
            if ( query_flags & RAYQUERYOUTLINE ) {
                if ( qray->flags & Q3DRAY_PRIMARY_BIT ) {
                    if ( ( qjob->qrsg->flags & WIREFRAMELIGHTING ) == 0x00 ) {
                        Q3DZBUFFER zout;

                        q3darea_getZBuffer ( &qjob->qarea, 
                                              qray->x, 
                                              qray->y,
                                             &zout );

                        q3dray_outline ( qray,
                                         qjob,
                                         qjob->qarea.qzen.WMVX[zout.wmvxID], 
                                         qjob->qcam->PJX,
                                         qjob->qcam->VPX,
                                         0.01f,
                                         qjob->qrsg->wireframe.thickness,
                                        &diffuse,
                                         qjob->qrsg->wireframe.color,
                                         frame  );
                    }
                }
            }

            if ( diffuse.r > 0xFF ) diffuse.r = 0xFF;
            if ( diffuse.g > 0xFF ) diffuse.g = 0xFF;
            if ( diffuse.b > 0xFF ) diffuse.b = 0xFF;

            return ( ( uint32_t ) ( diffuse.r << 0x10 ) | 
                                  ( diffuse.g << 0x08 ) | 
                                  ( diffuse.b         ) );
        }
    }

    if ( qray->flags & Q3DRAY_PRIMARY_BIT ) {
        if ( qjob->qrsg->background.mode & BACKGROUND_IMAGE ) {
            uint32_t backgroundWidth = ( qjob->qarea.width * 
                                         qjob->qrsg->background.widthRatio );
            int32_t deltaWidth = ( int32_t ) ( qjob->qarea.width - backgroundWidth ) / 2;

            if ( ( qray->x > deltaWidth ) && 
                 ( qray->x < ( qjob->qarea.width - deltaWidth ) ) ) {
                float x = ( backgroundWidth   ) ? ( float ) ( qray->x - deltaWidth ) / backgroundWidth : 0.0f,
                      y = ( qjob->qarea.height ) ? ( float ) qray->y / qjob->qarea.height : 0.0f;
                uint32_t inty = ( y * qjob->qrsg->background.image->height ),
                         intx = ( x * qjob->qrsg->background.image->width  );
                uint32_t offset =  ( inty * qjob->qrsg->background.image->width ) + intx;

                switch ( qjob->qrsg->background.image->bytesPerPixel ) {
                    case 0x03 : {
                        unsigned char (*imgdata)[0x03] = qjob->qrsg->background.image->data;
                        uint32_t R = ( uint32_t ) imgdata[offset][0x00],
                                 G = ( uint32_t ) imgdata[offset][0x01],
                                 B = ( uint32_t ) imgdata[offset][0x02];

                        return ( R  << 0x10 ) | ( G  << 0x08 ) | B ;
                    } break;

                    default :
                    break;
                }
            }
        }
    }

    return qjob->qrsg->background.color;
}
