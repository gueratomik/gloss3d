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
/*** Note: Source is in World coordinates ***/
uint32_t q3dray_illumination ( Q3DINTERSECTION *wisx,
                               Q3DJOB          *qjob,
                               Q3DVECTOR3F     *bumpnor,
                               Q3DRGBA         *diffuse,
                               Q3DRGBA         *specular,
                               uint32_t         nbhop ) {
    static Q3DVECTOR3F pzero = { .x = 0.0f, .y = 0.0f, .z = 0.0f };
    LIST *ltmpqlig = qjob->qsce->llights;

    memset ( diffuse , 0x00, sizeof ( Q3DRGBA ) );
    memset ( specular, 0x00, sizeof ( Q3DRGBA ) );

    while ( ltmpqlig ) {
        Q3DLIGHT  *qlig    = ( Q3DLIGHT *  ) ltmpqlig->data;
        Q3DOBJECT *qobjlig = ( Q3DOBJECT * ) qlig;
        G3DLIGHT  *lig     = ( G3DLIGHT *  ) q3dobject_getObject ( qobjlig );
         /*** QLight world position ***/
        Q3DVECTOR3F qligwpos;
        Q3DRAY luxqray;
        float dot;

        q3dvector3f_matrix ( &pzero, qobjlig->obj->wmatrix, &qligwpos );

        luxqray.src.x   = wisx->src.x;
        luxqray.src.y   = wisx->src.y;
        luxqray.src.z   = wisx->src.z;

        luxqray.dir.x   = qligwpos.x - luxqray.src.x;
        luxqray.dir.y   = qligwpos.y - luxqray.src.y;
        luxqray.dir.z   = qligwpos.z - luxqray.src.z;

        luxqray.distance = INFINITY;
        luxqray.flags    = 0x00;

        q3dvector3f_normalize ( &luxqray.dir, NULL );

        dot = q3dvector3f_scalar ( &luxqray.dir, &wisx->dir );

        if ( dot > 0.0f ) {
            float rate = dot * lig->intensity;

            diffuse->r += ( lig->diffuseColor.r * rate ),
            diffuse->g += ( lig->diffuseColor.g * rate ),
            diffuse->b += ( lig->diffuseColor.b * rate );
        }

        ltmpqlig = ltmpqlig->next;
    }
}

/******************************************************************************/
uint32_t q3dray_getSurfaceColor ( Q3DRAY      *qray,
                                  Q3DMESH     *qmes,
                                  Q3DTRIANGLE *qtri,
                                  Q3DAREA     *qarea,
                                  float        backgroundImageWidthRatio,
                                  Q3DRGBA     *diffuse,
                                  Q3DRGBA     *specular,
                                  G3DVECTOR   *bump,
                                  Q3DRGBA     *reflection,
                                  Q3DRGBA     *refraction,
                                  Q3DRGBA     *alpha,
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
    memset ( bump      , 0x00, sizeof ( G3DVECTOR ) );
    memset ( reflection, 0x00, sizeof ( Q3DRGBA   ) );
    memset ( refraction, 0x00, sizeof ( Q3DRGBA   ) );
    memset ( alpha     , 0x00, sizeof ( Q3DRGBA   ) );

    while ( ltmptex ) {
        G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;
        G3DMATERIAL *mat = tex->mat;
        uint32_t repeat = ( tex->flags & TEXTUREREPEATED ) ? 0x01 : 0x00;
        float avgu = 0.0f,
              avgv = 0.0f;
        Q3DRGBA retval;

        if ( tex->flags & TEXTURERESTRICTED ) {
            if ( ( qtri->textureSlots & tex->slotBit ) == 0x00 )  {
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

        if ( mat->flags & BUMP_ENABLED ) {
            G3DVECTOR bumpnor;

            g3dchannel_getNormal ( &mat->bump, avgu, avgv, &bumpnor, repeat, 0.01f, 0.01f, 0x00 );

            bump->x += bumpnor.x;
            bump->y += bumpnor.y;
            bump->z += bumpnor.z;

            divBump++;
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

            alpha->a  = 1.0f; /* used as boolean to say alpha is enabled */
            alpha->r += retval.r;
            alpha->g += retval.g;
            alpha->b += retval.b;

            if ( ( mat->alpha.flags & USEIMAGECOLOR ) || 
                 ( mat->alpha.flags & USEPROCEDURAL ) ) {
                alpha->r *= ( mat->alpha.solid.a );
                alpha->g *= ( mat->alpha.solid.a );
                alpha->b *= ( mat->alpha.solid.a );
            }

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
        diffuse->r = diffuse->g = diffuse->b = MESHCOLORUB;
    }

    if ( divSpecular ) {
        specular->r /= divSpecular;
        specular->g /= divSpecular;
        specular->b /= divSpecular;
        specular->a /= divSpecular;
    }

    if ( divBump ) {
        bump->x /= divBump;
        bump->y /= divBump;
        bump->z /= divBump;
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
        alpha->r /= ( divAlpha );
        alpha->g /= ( divAlpha );
        alpha->b /= ( divAlpha );
    }

    return 0x01;
}

/******************************************************************************/
void q3dray_getWorldIntersection ( Q3DRAY          *qray,
                                   float            frame,
                                   Q3DINTERSECTION *wisx ) {

    if ( qray->qobj->obj->type & MESH ) {
        Q3DMESH *qmes = ( Q3DMESH * ) qray->qobj;
        Q3DVERTEX *qver = q3dmesh_getVertices ( qmes, frame );
        Q3DVECTOR3F src = { .x = qray->src.x + ( qray->dir.x * qray->distance ),
                            .y = qray->src.y + ( qray->dir.y * qray->distance ),
                            .z = qray->src.z + ( qray->dir.z * qray->distance ) };
        uint32_t qverID[0x03] = { qray->qsur->tri.qverID[0x00],
                                  qray->qsur->tri.qverID[0x01],
                                  qray->qsur->tri.qverID[0x02] };

        Q3DVECTOR3F dir = { .x = ( qver[qverID[0]].nor.x * qray->ratio[0] ) +
                                 ( qver[qverID[1]].nor.x * qray->ratio[1] ) +
                                 ( qver[qverID[2]].nor.x * qray->ratio[2] ),
                            .y = ( qver[qverID[0]].nor.y * qray->ratio[0] ) +
                                 ( qver[qverID[1]].nor.y * qray->ratio[1] ) +
                                 ( qver[qverID[2]].nor.y * qray->ratio[2] ),
                            .z = ( qver[qverID[0]].nor.z * qray->ratio[0] ) +
                                 ( qver[qverID[1]].nor.z * qray->ratio[1] ) +
                                 ( qver[qverID[2]].nor.z * qray->ratio[2] ) };

        q3dvector3f_matrix ( &src, qray->qobj->IWMVX , &wisx->src );
        q3dvector3f_matrix ( &dir, qray->qobj->TIWMVX, &wisx->dir );

        wisx->qobj = qray->qobj;
        wisx->qsur = qray->qsur;
    }
}

/******************************************************************************/
uint32_t q3dray_shoot_r ( Q3DRAY     *qray, 
                          Q3DJOB     *qjob,
                          Q3DSURFACE *sdiscard,
                          float       frame,
                          uint32_t    nbhop,
                          uint32_t    query_flags ) {
    Q3DOBJECT   *qobj = NULL;
    Q3DMESH     *qmes = NULL;
    Q3DTRIANGLE *qtri = NULL;
    G3DMESH     *mes  = NULL;

    if ( qray->flags & Q3DRAY_PRIMARY_BIT ) {
        Q3DZBUFFER zout;

        q3darea_getZBuffer ( &qjob->qarea, 
                              qray->x, 
                              qray->y,
                             &zout );

        if ( zout.z == INFINITY ) {
            /*** not hit ***/
        } else {
            qobj = q3dscene_getObjectByID ( qjob->qsce, zout.qobjID );

            if ( qobj->obj->type & MESH ) {
                Q3DRAY locqray;

                qmes = ( Q3DMESH * ) qobj;
                mes  = ( G3DMESH * ) q3dobject_getObject ( qobj );
                qtri = &qmes->qtri[zout.qtriID];

                memcpy ( &locqray, qray, sizeof ( Q3DRAY ) );

    q3dvector3f_matrix ( &qray->src, qobj->IMVX, &locqray.src );
    q3dvector3f_matrix ( &qray->dir, qobj->TIMVX, &locqray.dir );

                /*** me must launch a ray to get surface color & UV coords ***/
                if ( q3dtriangle_intersect ( qtri,
                                             q3dmesh_getVertices( qmes, frame ),
                                            &locqray,
                                             query_flags ) ) {
return 0xFF00FF;
                    qray->color    = locqray.color;
                    qray->distance = locqray.distance;

                    qray->qobj     = qobj;
                    qray->qsur     = qtri;

                    qray->ratio[0x00] = locqray.ratio[0x00];
                    qray->ratio[0x01] = locqray.ratio[0x01];
                    qray->ratio[0x02] = locqray.ratio[0x02];
                }
            }
        }
    } else {
        if ( q3dobject_intersect_r ( qjob->qsce,
                                     qray,
                                     frame,
                                     query_flags,
                                /*render_flags*/0x00 ) ) {
            qobj = qray->qobj;

            if ( qobj->obj->type & MESH ) {
                qmes = ( Q3DMESH * ) qobj;
                mes  = ( G3DMESH * ) q3dobject_getObject ( qobj );

                qtri = qray->qsur;
            }
        }
    }

    /*if ( qray->flags & Q3DRAY_HAS_HIT_BIT ) {*/
    if ( qray->qobj && ( qray->qobj->obj->type & MESH ) ) {
        Q3DVECTOR3F materialBump = { .x = 0.0f, 
                                     .y = 0.0f, 
                                     .z = 0.0f };
        Q3DRGBA materialDiffuse, 
                materialSpecular,
                materialReflection,
                materialRefraction,
                materialAlpha;
        Q3DRGBA lightDiffuse, 
                lightSpecular;
        Q3DRGBA diffuse;
        Q3DINTERSECTION wisx;

        q3dray_getWorldIntersection ( qray, frame, &wisx );

        q3dray_getSurfaceColor ( qray, 
                                 qobj,
                                 qtri,
                                &qjob->qarea,
                                 qjob->qrsg->background.widthRatio,
                                &materialDiffuse,
                                &materialSpecular,
                                &materialBump,
                                &materialReflection,
                                &materialRefraction,
                                &materialAlpha,
                                 mes->ltex, 
                                 query_flags );

         q3dray_illumination ( &wisx,
                                qjob,
                                NULL,
                               &lightDiffuse,
                               &lightSpecular,
                                nbhop );

        diffuse.r = ( uint32_t ) ( ( lightDiffuse.r * materialDiffuse.r ) >> 0x08 ) /*+ ( ( lightSpecular.r * materialSpecular.r ) >> 0x08 )*/;
        diffuse.g = ( uint32_t ) ( ( lightDiffuse.g * materialDiffuse.g ) >> 0x08 ) /*+ ( ( lightSpecular.g * materialSpecular.g ) >> 0x08 )*/;
        diffuse.b = ( uint32_t ) ( ( lightDiffuse.b * materialDiffuse.b ) >> 0x08 ) /*+ ( ( lightSpecular.b * materialSpecular.b ) >> 0x08 )*/;

        if ( diffuse.r > 0xFF ) diffuse.r = 0xFF;
        if ( diffuse.g > 0xFF ) diffuse.g = 0xFF;
        if ( diffuse.b > 0xFF ) diffuse.b = 0xFF;

        return ( ( uint32_t ) ( diffuse.r << 0x10 ) | 
                              ( diffuse.g << 0x08 ) | 
                              ( diffuse.b         ) );
    }
    /*}*/

    return /*qjob->qrsg->background.color*/0xFF;
}
