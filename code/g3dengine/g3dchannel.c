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
void g3dchannel_getNormal ( G3DCHANNEL *cha, 
                            float       u,
                            float       v,
                            G3DVECTOR  *nor,
                            uint32_t    repeat,
                            float       precU,
                            float       precV ) {
    uint32_t channelFlags = cha->flags;

    g3dvector_init ( nor, 0.0f, 0.0f, 1.0f, 1.0f );

    if ( repeat == 0x00 ) {
        if ( ( u < 0.0f ) || ( u > 1.0f ) ||
             ( v < 0.0f ) || ( v > 1.0f ) ) {
            channelFlags = USESOLIDCOLOR;
        }
    }

    if ( channelFlags & USEIMAGECOLOR ) {
        G3DIMAGE *colimg = cha->image;
        if ( colimg && colimg->width && colimg->height ) {
            int32_t imgx = ((int32_t)((float)u * colimg->width  )) % colimg->width;
            int32_t imgy = ((int32_t)((float)v * colimg->height )) % colimg->height;

            if ( imgx < 0x00 ) imgx = colimg->width  - imgx;
            if ( imgy < 0x00 ) imgy = colimg->height - imgy;

            g3dimage_getNormal ( colimg, imgx, imgy, nor );
        }
    }

    if ( channelFlags & USEPROCEDURAL ) {
        G3DPROCEDURAL *proc = cha->proc;
        G3DCOLOR color;

        if ( proc ) {
            g3dprocedural_getNormal ( proc, u, v, nor, precU, precV );
        }
    }
}

/******************************************************************************/
G3DPROCEDURAL *g3dchannel_setProcedural ( G3DCHANNEL    *cha, 
                                          G3DPROCEDURAL *proc ) {
    G3DPROCEDURAL *previous = cha->proc;

    cha->proc = proc;

    return previous;
}

/******************************************************************************/
void g3dchannel_enableProcedural ( G3DCHANNEL *cha ) {
        cha->flags &= (~USECHANNELMASK); /*** clear flags first ***/

        cha->flags |= USEPROCEDURAL;
}

/******************************************************************************/
void g3dchannel_enableImageColor ( G3DCHANNEL *cha ) {
        cha->flags &= (~USECHANNELMASK); /*** clear flags first ***/

        cha->flags |= USEIMAGECOLOR;
}

/******************************************************************************/
void g3dchannel_enableSolidColor ( G3DCHANNEL *cha ) {
        cha->flags &= (~USECHANNELMASK); /*** clear flags first ***/

        cha->flags |= USESOLIDCOLOR;
}

/******************************************************************************/
void g3dchannel_getColor ( G3DCHANNEL *cha, float    u,
                                            float    v,
                                            G3DRGBA *rgba,
                                            uint32_t repeat ) {
    uint32_t channelFlags = cha->flags;

    if ( repeat == 0x00 ) {
        if ( ( u < 0.0f ) || ( u > 1.0f ) ||
             ( v < 0.0f ) || ( v > 1.0f ) ) {
            channelFlags = USESOLIDCOLOR;
        }
    }

    if ( channelFlags & USESOLIDCOLOR ) {
        g3dcolor_toRGBA ( &cha->solid, rgba );
    }

    if ( channelFlags & USEIMAGECOLOR ) {
        G3DIMAGE *colimg = cha->image;
        if ( colimg && colimg->width && colimg->height ) {
            int32_t imgx = ((int32_t)((float)u * colimg->width  )) % colimg->width;
            int32_t imgy = ((int32_t)((float)v * colimg->height )) % colimg->height;

            if ( imgx < 0x00 ) imgx = colimg->width  - imgx;
            if ( imgy < 0x00 ) imgy = colimg->height - imgy;

            uint32_t offset = ( imgy * colimg->bytesPerLine  ) +
                              ( imgx * colimg->bytesPerPixel );

            rgba->r = colimg->data[offset+0x00];
            rgba->g = colimg->data[offset+0x01];
            rgba->b = colimg->data[offset+0x02];
        } else {
            g3dcolor_toRGBA ( &cha->solid, rgba );
        }
    }

    if ( channelFlags & USEPROCEDURAL ) {
        G3DPROCEDURAL *proc = cha->proc;
        G3DCOLOR color;

        if ( proc ) {
            proc->getColor ( proc, u, v, 0.0f, &color );

            g3dcolor_toRGBA ( &color, rgba );
        } else {
            g3dcolor_toRGBA ( &cha->solid, rgba );
        }
    }
}
