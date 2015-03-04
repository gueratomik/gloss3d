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
#include <g3dui.h>

/******************************************************************************/
MATERIALLISTDATA *common_materiallistdata_new ( uint32_t image_width,
                                                uint32_t image_height ) {
    uint32_t structsize = sizeof ( MATERIALLISTDATA );
    MATERIALLISTDATA *mdata = ( MATERIALLISTDATA * ) calloc ( 0x01, structsize );

    if ( mdata == NULL ) {
        fprintf ( stderr, "g3duimateriallist_new: memory allocation failed\n" );

        return NULL;
    }

    mdata->matmap = common_g3duimaterialmap_new ( image_width, 
                                                   image_height );
    mdata->preview_border = 0x04;
    mdata->preview_name_height = BUTTONSIZE;
    mdata->preview_width  = image_width  + ( mdata->preview_border * 0x02 );
    mdata->preview_height = image_height + ( mdata->preview_border * 0x02 ) +
                                             mdata->preview_name_height;


    return mdata;
}

/******************************************************************************/
G3DUIMATERIALMAP *common_g3duimaterialmap_new ( uint32_t width,
                                                uint32_t height ) {
    uint32_t structsize = sizeof ( G3DUIMATERIALMAP );
    uint32_t pixelsize  = sizeof ( G3DUIMATERIALPIXEL );
    G3DUIMATERIALMAP *matmap = ( G3DUIMATERIALMAP * ) calloc ( 0x01, structsize );
    uint32_t nbpix = ( width * height );

    if ( matmap == NULL ) {
        fprintf ( stderr, "g3duimaterialmap_new: memory allocation failed\n" );

        return NULL;
    }

    matmap->pixel  = ( G3DUIMATERIALPIXEL * ) calloc ( nbpix, pixelsize );

    matmap->width  = width;
    matmap->height = height;

    /*** build the material sphere vector map ***/
    common_g3duimaterialmap_buildSphere ( matmap, width, height, 0.8f );


    return matmap;
}


/******************************************************************************/
void common_g3duimaterialmap_buildSphere ( G3DUIMATERIALMAP *matmap,
                                           uint32_t width,
                                           uint32_t height,
                                           float    rad ) {
    uint32_t winx, winy;
    /*** 2 lights vector ***/
    G3DVECTOR luxpos[0x02] = { {  200.0f,  200.0f, -400.0f, 1.0f },
                               { -400.0f, -400.0f,    0.0f, 1.0f } };
    G3DVECTOR campos = { 0.0f, 0.0f, -1.0f, 1.0f };
    G3DUIMATERIALPIXEL *pixel = matmap->pixel;

    for ( winy = 0x00; winy < height; winy++ ) {
        float objy = ( float ) ( - 2.0 * winy ) / height + 1.0f;

        for ( winx = 0x00; winx < width; winx++ ) {
            float objx = ( float ) ( 2.0 * winx ) / width - 1.0f;
            float equz =  ( rad * rad ) - ( ( objx * objx ) + ( objy * objy ) );
            G3DUIMATERIALPIXEL *p = pixel++;

            if ( equz >= 0.0f ) {
                /*** there are 2 potential Z, we use the one closer to the ***/
                /*** cam, which is the negative one **/
                float objz = - sqrt ( equz );
                float u, v;
                uint32_t i;

                p->pos.x = p->nor.x = objx;
                p->pos.y = p->nor.y = objy;
                p->pos.z = p->nor.z = objz;

                g3dvector_normalize ( &p->nor, NULL );

        /*** http://en.wikipedia.org/wiki/UV_mapping#Finding_UV_on_a_sphere ***/
                p->u =   ( atan2f( p->nor.z, p->nor.x ) / ( 2.0f * M_PI ) ) + 0.5f;
                p->v = - ( asin  ( p->nor.y )           / ( M_PI        ) ) + 0.5f;

                p->diff = 0.0f;
                p->spec = 0.0f;

                /*** there are 2 sources of light ***/
                for ( i = 0x00; i < 0x02; i++ ) {
                    G3DVECTOR ptolux = { ( luxpos[i].x - p->pos.x ),
                                         ( luxpos[i].y - p->pos.y ),
                                         ( luxpos[i].z - p->pos.z ) };
                    G3DVECTOR ptocam = { ( campos.x - p->pos.x ),
                                         ( campos.y - p->pos.y ),
                                         ( campos.z - p->pos.z ) };
                    G3DVECTOR refray;
                    G3DVECTOR raydir;
                    float spec;
                    float diff;

                    g3dvector_normalize ( &ptolux, NULL );

                    raydir.x = - ptolux.x;
                    raydir.y = - ptolux.y;
                    raydir.z = - ptolux.z;

                    /*** Specular reflection ***/
                    if ( g3dvector_reflect ( &raydir, &p->nor, &refray ) ) {
                        g3dvector_normalize ( &refray, NULL );
                        g3dvector_normalize ( &ptocam, NULL );

                        spec = g3dvector_scalar  ( &refray, &ptocam );

                        if ( spec > 0.0f ) p->spec += spec;
                    }


                    /***************************/
                    diff = g3dvector_scalar  ( &ptolux, &p->nor );

                    if ( diff > 0.0f ) p->diff += diff;
                }

                p->used = 0x01;
            } else {
                p->used = 0x00;
            }
        }
    }
}

/******************************************************************************/
/*** Data must have the same width&height@24 as the vector map ***/
void common_g3duimaterialmap_fillData ( G3DUIMATERIALMAP *matmap,
                                        G3DMATERIAL *mat,
                                        unsigned char (*data)[3] ) {
    G3DUIMATERIALPIXEL *pixel = matmap->pixel;
    static int GRAY = 0x40;
    uint32_t x, y;
    G3DRGBA rgba;

    g3dcolor_toRGBA ( &mat->diffuse.solid, &rgba );

    for ( y = 0x00; y < matmap->height; y++ ) {
        for ( x = 0x00; x < matmap->width; x++ ) {
            G3DUIMATERIALPIXEL *p = pixel++;

            if ( p->used ) {
                uint32_t R = 0x00, G = 0x00, B = 0x00;
                float rf, gf, bf;
                uint32_t col = 0x00;

                if ( mat->flags & DIFFUSE_USEIMAGECOLOR ) {
                    if ( mat->diffuse.image ) {
                        G3DIMAGE *colimg = mat->diffuse.image;
                        uint32_t imgx = ((uint32_t)((float)p->u * colimg->width  * colimg->wratio ));
                        uint32_t imgy = ((uint32_t)((float)p->v * colimg->height * colimg->hratio ));
                        uint32_t offset = ( imgy * colimg->bytesperline  ) +
                                          ( imgx * colimg->bytesperpixel );

                        /*** This depth part should be optimized ***/
                        if ( colimg->depth == 0x18 ) {
                            R = colimg->data[offset+0x00];
                            G = colimg->data[offset+0x01];
                            B = colimg->data[offset+0x02];
                        }

                        if ( colimg->depth == 0x08 ) {
                            R = G = B = colimg->data[offset+0x00];
                        }
                    }
                }

                if ( mat->flags & DIFFUSE_USESOLIDCOLOR ) {
                    R = ( uint32_t ) rgba.r,
                    G = ( uint32_t ) rgba.g,
                    B = ( uint32_t ) rgba.b;
                }

                R = ( R * p->diff );
                G = ( G * p->diff );
                B = ( B * p->diff );

                if ( mat->transparency_strength ) {
                    float trans = mat->transparency_strength;

                    R = GRAY + ( ( (int32_t) R - GRAY ) * ( 1.0f - trans ) );
                    G = GRAY + ( ( (int32_t) G - GRAY ) * ( 1.0f - trans ) );
                    B = GRAY + ( ( (int32_t) B - GRAY ) * ( 1.0f - trans ) );
                }

                /*** Specularity ***/
                if ( p->spec > 0.0f ) {
                    float specularCoefficient = pow ( p->spec, mat->shininess );
                    uint32_t r = ( 255 * mat->specular_level * mat->specular.solid.r * specularCoefficient ),
                             g = ( 255 * mat->specular_level * mat->specular.solid.g * specularCoefficient ),
                             b = ( 255 * mat->specular_level * mat->specular.solid.b * specularCoefficient );
/*printf("applying %d %d %d %f %f %f\n", r, g, b, specularCoefficient, p->spec, mat->shininess );
*/
                    R += r;
                    G += g,
                    B += b;
                }

                if ( R > 0xFF ) R = 0xFF;
                if ( G > 0xFF ) G = 0xFF;
                if ( B > 0xFF ) B = 0xFF;

                data[y*matmap->width+x][0x00] = R;
                data[y*matmap->width+x][0x01] = G;
                data[y*matmap->width+x][0x02] = B;
            } else {
                data[y*matmap->width+x][0x00] = GRAY;
                data[y*matmap->width+x][0x01] = GRAY;
                data[y*matmap->width+x][0x02] = GRAY;
            }
        }
    }
}
