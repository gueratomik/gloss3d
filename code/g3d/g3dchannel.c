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
#include <g3d.h>

/******************************************************************************/
void g3dchannel_getColor ( G3DCHANNEL *cha, float    u,
                                            float    v,
                                            G3DRGBA *rgba ) {
    if ( cha->flags & USEIMAGECOLOR ) {
        G3DIMAGE *colimg = cha->image;
        if ( colimg ) {
            int32_t imgx = ((int32_t)((float)u * colimg->width  )) % colimg->width;
            int32_t imgy = ((int32_t)((float)v * colimg->height )) % colimg->height;

            if ( imgx < 0x00 ) imgx = colimg->width  - imgx;
            if ( imgy < 0x00 ) imgy = colimg->height - imgy;

            uint32_t offset = ( imgy * colimg->bytesperline  ) +
                              ( imgx * colimg->bytesperpixel );

            rgba->r = colimg->data[offset+0x00];
            rgba->g = colimg->data[offset+0x01];
            rgba->b = colimg->data[offset+0x02];
        } else {
            g3dcolor_toRGBA ( &cha->solid, rgba );
        }
    }

    if ( cha->flags & USESOLIDCOLOR ) {
        g3dcolor_toRGBA ( &cha->solid, rgba );
    }

    if ( cha->flags & USEPROCEDURAL ) {
        G3DPROCEDURAL *proc = cha->proc;

        if ( proc ) {
            proc->getColor ( proc, u, v, 0.0f, rgba );
        } else {
            g3dcolor_toRGBA ( &cha->solid, rgba );
        }
    }
}
