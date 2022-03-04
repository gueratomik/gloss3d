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
#include <g3durmanager.h>

/******************************************************************************/
void urmsculptfaceextension_free ( URMSCULPTFACEEXTENSION *usfe ) {
    if ( usfe->pos ) free ( usfe->pos );
    if ( usfe->nor ) free ( usfe->nor );

    free ( usfe );
}

/******************************************************************************/
URMSCULPTFACEEXTENSION *urmsculptfaceextension_seek ( LIST                   *lusf,
                                                      G3DFACESCULPTEXTENSION *fse ) {
    LIST *ltmpusf = lusf;

    while ( ltmpusf ) {
        URMSCULPTFACEEXTENSION *usf = ( URMSCULPTFACEEXTENSION * ) ltmpusf->data;

        if ( usf->fse == fse ) return usf;

        ltmpusf = ltmpusf->next;
    }

    return NULL;
}

/******************************************************************************/
URMSCULPTFACEEXTENSION *urmsculptfaceextension_new ( G3DFACESCULPTEXTENSION *fse,
                                                     G3DFACE                *fac ) {
    URMSCULPTFACEEXTENSION *usf = ( URMSCULPTFACEEXTENSION * ) calloc ( 0x01, sizeof ( URMSCULPTFACEEXTENSION ) );

    if ( usf == NULL ) {
        fprintf ( stderr, "%s : calloc failed\n", __func__ );

        return NULL;
    }

    usf->fse = fse;
    usf->fac = fac;

    usf->pos = calloc ( fse->nbver, sizeof ( G3DVECTOR ) );
    usf->nor = calloc ( fse->nbver, sizeof ( G3DVECTOR ) );


    return usf;
}

/******************************************************************************/
URMSCULPTFACE *urmsculptface_new ( G3DSUBDIVIDER *sdr,
                                   uint64_t       engine_flags ) {
    uint32_t structsize = sizeof ( URMSCULPTFACE );

    URMSCULPTFACE *usf = ( URMSCULPTFACE * ) calloc ( 0x01, structsize );

    if ( usf == NULL ) {
        fprintf ( stderr, "%s: memory allocation falied\n", __func__ );

        return NULL;
    }

    usf->sdr   = sdr;

    return usf;
}

/******************************************************************************/
void urmsculptface_free ( URMSCULPTFACE *usf ) {
    list_free ( &usf->lusfe, LIST_FUNCDATA(urmsculptfaceextension_free) );

    free ( usf );
}

/******************************************************************************/
void sculptFace_free ( void    *data, 
                       uint32_t commit ) {
    URMSCULPTFACE *usf = ( URMSCULPTFACE * ) data;

    urmsculptface_free ( usf );
}

/******************************************************************************/
void sculptFace_undo ( G3DURMANAGER *urm, 
                       void         *data, 
                       uint64_t      engine_flags ) {
    URMSCULPTFACE *usf = ( URMSCULPTFACE * ) data;
    LIST *ltmpusfe = usf->lusfe;
    LIST *lsubfac = NULL;

    while ( ltmpusfe ) {
        URMSCULPTFACEEXTENSION *usfe = ( URMSCULPTFACEEXTENSION * ) ltmpusfe->data;

        if ( usfe->fse->nbver ) {
            G3DVECTOR *tmp = calloc ( usfe->fse->nbver, sizeof ( G3DVECTOR ) );

            memcpy ( tmp, 
                     usfe->fse->pos, 
                     usfe->fse->nbver * sizeof ( G3DVECTOR ) );

            memcpy ( usfe->fse->pos,
                     usfe->pos,
                     usfe->fse->nbver * sizeof ( G3DVECTOR ) );

            memcpy ( usfe->pos, 
                     tmp, 
                     usfe->fse->nbver * sizeof ( G3DVECTOR ) );

            list_insert ( &lsubfac, usfe->fac );

            free ( tmp );
        }

        ltmpusfe = ltmpusfe->next;
    }

    g3dsubdivider_fillBuffers ( usf->sdr,
                                lsubfac,
                                engine_flags );

    list_free ( &lsubfac, NULL );
}

/******************************************************************************/
void sculptFace_redo ( G3DURMANAGER *urm, 
                       void         *data, 
                       uint64_t      engine_flags ) {
    sculptFace_undo ( urm, data, engine_flags );
}

/******************************************************************************/
URMSCULPTFACE *g3durm_mesh_sculptFace ( G3DURMANAGER  *urm,
                                        G3DSUBDIVIDER *sdr,
                                        uint64_t       engine_flags,
                                        uint32_t       return_flags ) {
    LIST *lnewuvset = NULL;
    URMSCULPTFACE *usf;

    usf = urmsculptface_new ( sdr, engine_flags );

    /*** Rebuild the mesh with modifiers (e.g for displacement) ***/
    /* Commented out. this is called in g3dmesh_sculptFace() */
    /*g3dmesh_update ( mes, 
                     NULL,
                     NULL,
                     NULL,
                     RESETMODIFIERS, engine_flags );*/

    g3durmanager_push ( urm, 
                        sculptFace_undo,
                        sculptFace_redo,
                        sculptFace_free, 
                        usf,
                        return_flags );

    return usf;
}
