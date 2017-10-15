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

/******************************************************************************/
uint32_t filterdof_draw ( R3DFILTER *fil,
                          R3DSCENE *rsce,
                          float frameID,
                          unsigned char (*img)[0x03], 
                          uint32_t from, 
                          uint32_t to, 
                          uint32_t depth, 
                          uint32_t width ) {
    R3DDOF *rdf = ( R3DDOF * ) fil->data;
    uint32_t i, j;

    for ( i = from; i <= to; i++ ) {
        for ( j = 0; j < width; j++ ) {
            r3ddof_blurifyPixel ( rdf, j, i, img, rsce->area.zBuffer );
        }
    }

    memcpy ( img, rdf->dofimg, rdf->width * rdf->height * 0x03);

    return 0x00;
}

/******************************************************************************/
void filterdof_free ( R3DFILTER *fil ) {
    R3DDOF *rdf = ( R3DDOF * ) fil->data;

    r3ddof_free ( rdf );
}

/******************************************************************************/
R3DFILTER *r3dfilter_dof_new ( uint32_t width,
                               uint32_t height,
                               float    near,
                               float    far,
                               uint32_t radius ) {
    R3DFILTER *fil;

    fil = r3dfilter_new ( FILTERIMAGE, DOFFILTERNAME,
                                       filterdof_draw,
                                       filterdof_free,
                                       r3ddof_new ( width, 
                                                    height,
                                                    near,
                                                    far,
                                                    radius ) );

    return fil;
}

/******************************************************************************/
R3DDOF *r3ddof_new ( uint32_t width,
                     uint32_t height,
                     float    near,
                     float    far,
                     uint32_t radius ) {
    R3DDOF *rdf = calloc ( 0x01, sizeof ( R3DDOF ) );
    int i;

    if ( rdf == NULL ) {
        fprintf ( stderr, "r3ddof_new: calloc failed\n");

        return NULL;
    }

    rdf->width = width;
    rdf->height = height;
    rdf->radius = radius;
    rdf->near = near;
    rdf->far = far;

    rdf->dofimg = calloc ( width * height, 0x03 * sizeof ( char ) );

    if ( rdf->dofimg == NULL ) {
        fprintf ( stderr, "r3ddof_new: calloc failed\n");

        free ( rdf );

        return NULL;
    }

    return rdf;
}

/******************************************************************************/
void r3ddof_blurifyPixel ( R3DDOF  *rdf,
                           uint32_t x,
                           uint32_t y,
                           unsigned char (*imgptr)[0x03],
                           float          *zBuffer ) {
    uint32_t offset = ( y * rdf->width ) + x;
    uint32_t R = 0, G = 0, B = 0;
    uint32_t nbdof = 0;
    int i, j;
    uint32_t radius = rdf->radius; /* default value */

    if ( zBuffer[offset] < rdf->near ) {
        radius = 0; 
    }

    if ( ( zBuffer[offset] > rdf->near ) && ( zBuffer[offset] < rdf->far ) ) {
        radius = rdf->radius * ( ( zBuffer[offset] - rdf->near ) / ( rdf->far - rdf->near ) ); 

   }


    for ( i  = ( (int) y - radius ); 
          i <= ( (int) y + radius ); i++ ) {
        for ( j  = ( (int) x - radius ); 
              j <= ( (int) x + radius ); j++ ) {
                uint32_t doffset = ( i * rdf->width ) + j;

            if ( ( i >= 0 ) && ( i < rdf->height ) &&
                 ( j >= 0 ) && ( j < rdf->width  ) &&
                 ( zBuffer[doffset] >= zBuffer[offset] )  ) {


                R += imgptr[doffset][0];
                G += imgptr[doffset][1];
                B += imgptr[doffset][2];

                nbdof++;
            }
        }
    }

    if ( nbdof ) {
       rdf->dofimg[offset][0] = R / nbdof;
       rdf->dofimg[offset][1] = G / nbdof;
       rdf->dofimg[offset][2] = B / nbdof;
    } else {
       rdf->dofimg[offset][0] = imgptr[offset][0];
       rdf->dofimg[offset][1] = imgptr[offset][1];
       rdf->dofimg[offset][2] = imgptr[offset][2];
    }
}


/******************************************************************************/
void r3ddof_free ( R3DDOF *rdf ) {
    if ( rdf->dofimg ) free ( rdf->dofimg );

    free ( rdf );
}
