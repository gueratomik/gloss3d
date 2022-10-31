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
#include <g3dui.h>

/******************************************************************************/
void g3duimateriallist_init ( G3DUIMATERIALLIST *matlist,
                              G3DUI             *gui,
                              uint32_t           image_width,
                              uint32_t           image_height,
                              uint32_t           preview_per_line ) {
    /*mdata->matmap = common_g3duimaterialmap_new ( image_width, 
                                                   image_height );*/

    matlist->gui = gui;

    matlist->preview_border      = 0x02;
    matlist->preview_name_height = BUTTONSIZE;
    matlist->preview_per_line    = preview_per_line;
    matlist->image_width         = image_width;
    matlist->image_height        = image_height;
    matlist->preview_width       = image_width  + ( matlist->preview_border * 0x02 );
    matlist->preview_height      = image_height + ( matlist->preview_border * 0x02 ) +
                                                    matlist->preview_name_height;

}

/******************************************************************************/
G3DUIMATERIALPREVIEW *g3duimateriallist_pickPreview ( G3DUIMATERIALLIST *matlist,
                                                      uint32_t           x,
                                                      uint32_t           y ) {
    LIST *ltmppreview = matlist->lpreview;

    while ( ltmppreview ) {
        G3DUIMATERIALPREVIEW *preview = ( G3DUIMATERIALPREVIEW * ) ltmppreview->data;

        if ( ( x > preview->rec.x ) && ( x < ( preview->rec.x + 
                                               preview->rec.width  ) ) &&
             ( y > preview->rec.y ) && ( y < ( preview->rec.y + 
                                               preview->rec.height ) ) ) {
            return preview;
        }

        ltmppreview = ltmppreview->next;
    }

    return NULL;
}

/******************************************************************************/
uint32_t g3duimateriallist_arrangePreviews ( G3DUIMATERIALLIST *matlist ) {
    LIST *ltmppreview = matlist->lpreview;
    uint32_t maxheight = 0x00;
    uint32_t maxwidth  = ( matlist->preview_width * matlist->preview_per_line );
    uint32_t win_x     = 0x00;
    uint32_t win_y     = 0x00;

    while ( ltmppreview ) {
        G3DUIMATERIALPREVIEW *preview = ( G3DUIMATERIALPREVIEW * ) ltmppreview->data;

        preview->rec.x = win_x;
        preview->rec.y = win_y;

        preview->rec.width  = matlist->preview_width  + 
                            ( matlist->preview_border * 2 );
        preview->rec.height = matlist->preview_height + 
                            ( matlist->preview_border * 2 ) + 
                              matlist->preview_name_height;

        if ( win_y + preview->rec.height > maxheight ) {
            maxheight = win_y + preview->rec.height;
        }

        if ( win_x + preview->rec.width >= maxwidth ) {
            win_y += preview->rec.height;

            win_x  = 0x00;
        } else {
            win_x += preview->rec.width;
        }
 
        ltmppreview = ltmppreview->next;
    }

    return ( maxheight << 0x10 ) | maxwidth;
}

/******************************************************************************/
G3DUIMATERIALPREVIEW *g3duimateriallist_getPreview ( G3DUIMATERIALLIST *matlist,
                                                     G3DMATERIAL       *mat ) {
    LIST *ltmppreview = matlist->lpreview;

    while ( ltmppreview ) {
        G3DUIMATERIALPREVIEW *preview = ( G3DUIMATERIALPREVIEW * ) ltmppreview->data;

        if ( preview->mat == mat ) return preview;

        ltmppreview = ltmppreview->next;
    }

    return NULL;
}


/******************************************************************************/
void g3duimateriallist_removeMaterial ( G3DUIMATERIALLIST *matlist,
                                        G3DSCENE          *sce,
                                        G3DURMANAGER      *urm,
                                        G3DMATERIAL       *mat ) {
    G3DUIMATERIALPREVIEW *preview = g3duimateriallist_getPreview ( matlist,
                                                                   mat );
 
    if ( preview ) {
        g3durm_scene_removeMaterial ( urm, 
                                      sce, 
                                      mat, 
                                      0x00,
                                      REDRAWOBJECTLIST | UPDATEMATERIALLIST  );

        g3duimateriallist_removePreview ( matlist, preview );
    }
}


/******************************************************************************/
void g3duimateriallist_removePreview ( G3DUIMATERIALLIST    *matlist, 
                                       G3DUIMATERIALPREVIEW *preview ) {
    list_remove ( &matlist->lpreview, preview );
}

/******************************************************************************/
void g3duimateriallist_addPreview ( G3DUIMATERIALLIST    *matlist, 
                                    G3DUIMATERIALPREVIEW *preview ) {
    list_insert ( &matlist->lpreview, preview );
}

/******************************************************************************/
G3DUIMATERIALMAP *g3duimaterialmap_new ( uint32_t width,
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

    return matmap;
}

/******************************************************************************/
void g3duimaterialmap_buildSphere ( G3DUIMATERIALMAP *matmap,
                                    G3DMATERIAL      *mat,
                                    float             radius ) {
    uint32_t width = matmap->width;
    uint32_t height = matmap->height;
    uint32_t winx, winy;
    /*** 2 lights vector ***/
    G3DVECTOR luxpos[0x02] = { {  200.0f,  200.0f, -400.0f, 1.0f },
                               { -400.0f, -400.0f,    0.0f, 1.0f } };
    G3DVECTOR campos = { 0.0f, 0.0f, -1.0f, 1.0f };
    G3DUIMATERIALPIXEL *pixel = matmap->pixel;
    G3DRGBA rgba;

    for ( winy = 0x00; winy < height; winy++ ) {
        float objy = ( float ) ( - 2.0 * winy ) / height + 1.0f;

        for ( winx = 0x00; winx < width; winx++ ) {
            float objx = ( float ) ( 2.0 * winx ) / width - 1.0f;
            float equz =  ( radius * radius ) - ( ( objx * objx ) + ( objy * objy ) );
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

                if ( mat->bump.flags & USECHANNEL ) {
                    G3DVECTOR tangent;
                    G3DVECTOR binomial;
                    G3DVECTOR bumpnor, finbump;
                    double TBN[0x09];
                    float strength = mat->bump.solid.r;

                    tangent.x = p->nor.x + 0.1f;
                    tangent.y = p->nor.y;
                    tangent.z = - ( ( ( tangent.x * p->nor.x ) +
                                      ( tangent.y * p->nor.y ) ) / p->nor.z );

                    g3dvector_normalize ( &tangent, NULL );
                    g3dvector_cross     ( &p->nor, &tangent, &binomial );

                    TBN[0x00] = tangent.x;
                    TBN[0x01] = tangent.y;
                    TBN[0x02] = tangent.z;
                    TBN[0x03] = binomial.x;
                    TBN[0x04] = binomial.y;
                    TBN[0x05] = binomial.z;
                    TBN[0x06] = p->nor.x;
                    TBN[0x07] = p->nor.y;
                    TBN[0x08] = p->nor.z;

                    g3dchannel_getNormal ( &mat->bump, 
                                            p->u,
                                            p->v, 
                                           &bumpnor, 
                                            0x00,
                                            0.01f,
                                            0.01f,
                                            0x00 );

                    g3dvector_matrix3 ( &bumpnor, TBN, &finbump );

                    p->nor.x = ( p->nor.x * ( 1.0f - strength ) ) + ( finbump.x * strength );
                    p->nor.y = ( p->nor.y * ( 1.0f - strength ) ) + ( finbump.y * strength );
                    p->nor.z = ( p->nor.z * ( 1.0f - strength ) ) + ( finbump.z * strength );
                }

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
void g3duimaterialmap_fillData ( G3DUIMATERIALMAP *matmap,
                                 G3DMATERIAL      *mat,
                                 unsigned char (*data)[3] ) {
    G3DUIMATERIALPIXEL *pixel = matmap->pixel;
    static int GRAY = 0x40;
    uint32_t x, y;

    for ( y = 0x00; y < matmap->height; y++ ) {
        for ( x = 0x00; x < matmap->width; x++ ) {
            G3DUIMATERIALPIXEL *p = pixel++;

            if ( p->used ) {
                uint32_t R = 0x00, G = 0x00, B = 0x00;
                float rf, gf, bf;
                uint32_t col = 0x00;

                if ( mat->diffuse.flags & USECHANNEL ) {
                    G3DRGBA rgba;

                    g3dchannel_getColor ( &mat->diffuse, p->u, p->v, &rgba, 0x01 );

                    R = rgba.r;
                    G = rgba.g;
                    B = rgba.b;
                }

                if ( mat->alpha.flags & USECHANNEL ) {
                    float trans = 1.0f - mat->alphaOpacity;
                    G3DRGBA rgba;

                    g3dchannel_getColor ( &mat->alpha, p->u, p->v, &rgba, 0x01 );

                    rgba.r = ( rgba.r * R ) >> 0x08;
                    rgba.g = ( rgba.g * G ) >> 0x08;
                    rgba.b = ( rgba.b * B ) >> 0x08;

                    R = ( (int32_t) GRAY * trans ) + ( rgba.r * mat->alphaOpacity );
                    G = ( (int32_t) GRAY * trans ) + ( rgba.g * mat->alphaOpacity );
                    B = ( (int32_t) GRAY * trans ) + ( rgba.b * mat->alphaOpacity );
                }

                R = ( R * p->diff );
                G = ( G * p->diff );
                B = ( B * p->diff );

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
