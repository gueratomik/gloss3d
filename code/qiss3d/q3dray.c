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

    if ( /*query_flags & Q3DRAY_PRIMARY_BIT */0) {
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
                qmes = ( Q3DMESH * ) qobj;
                mes  = ( G3DMESH * ) q3dobject_getObject ( qobj );
                qtri = &qmes->qtri[zout.qtriID];
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

                qtri = qray->surface;
            }
        }
    }

    if ( qmes ) {
        Q3DRGBA diffuse, specular, reflection, refraction, alpha;
        Q3DVECTOR3F bump = { .x = 0.0f, 
                             .y = 0.0f, 
                             .z = 0.0f };

        /*if ( q3dray_getSurfaceColor ( qray, 
                                      qobj,
                                      qtri,
                                     &qjob->qarea,
                                      qjob->qrsg->background.widthRatio,
                                     &diffuse,
                                     &specular,
                                     &bump,
                                     &reflection,
                                     &refraction,
                                     &alpha,
                                      mes->ltex, 
                                      query_flags ) ) {
            return ( ( uint32_t ) ( diffuse.r << 0x10 ) | 
                                  ( diffuse.g << 0x08 ) | 
                                  ( diffuse.b         ) );
        }*/ return 0xFF;
    }

    return qjob->qrsg->background.color;
}
