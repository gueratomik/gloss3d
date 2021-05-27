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
void q3dfilter_enable ( Q3DFILTER *fil ) {
    fil->flags |= ENABLEFILTER;
}

/******************************************************************************/
void q3dfilter_disable ( Q3DFILTER *fil ) {
    fil->flags &= (~ENABLEFILTER);
}

/******************************************************************************/
void q3dfilter_setType ( Q3DFILTER *fil, uint32_t type ) {
    fil->type = type;
}

/******************************************************************************/
Q3DFILTER *q3dfilter_getByName ( LIST *lfil, char *name ) {
    LIST *ltmpfil = lfil;

    while ( ltmpfil ) {
        Q3DFILTER *fil = ( Q3DFILTER * ) ltmpfil->data;

        if ( strcmp ( fil->name, name ) == 0x00 ) {
            return fil;
        }

        ltmpfil = ltmpfil->next;
    }

    return NULL;
}

/******************************************************************************/
Q3DFILTER *q3dfilter_new ( uint32_t   type, 
                           char      *name,
                           uint32_t (*draw)( Q3DFILTER *,
                                             Q3DJOB    *,
                         /* Frame ID */      float,
                         /*   Image data  */ unsigned char *,
                         /* From scanline */ uint32_t,
                         /*  To scanline  */ uint32_t,
                         /*     Depth     */ uint32_t,
                         /*     Width     */ uint32_t ),
                                             void (*Free)( Q3DFILTER * ),
                                             void  *data ) {
    Q3DFILTER *fil = ( Q3DFILTER * ) calloc ( 0x01, sizeof ( Q3DFILTER ) );

    if ( fil == NULL ) {
        fprintf ( stderr, "q3dfilter_new: memory allocation failed\n" );

        return NULL;
    }

    q3dfilter_setType ( fil, type );

    fil->draw  = draw;
    fil->data  = data;
    fil->free  = Free;
    fil->name  = name;

    q3dfilter_enable ( fil );


    return fil;
}

/******************************************************************************/
void q3dfilter_free ( Q3DFILTER *fil ) {
    printf ( "freeing filter \"%s\" ...", fil->name );

    if ( fil->free ) {
        fil->free ( fil );

        printf ( "done\n" );
    } else {
        printf ( "not required\n" );
    }

    free ( fil );
}
