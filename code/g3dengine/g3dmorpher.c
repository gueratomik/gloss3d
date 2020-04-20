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

#ifdef UNUSED

/******************************************************************************/
void g3dmorpher_setParent ( G3DSUBDIVIDER *sdr, 
                            G3DOBJECT     *parent,
                            uint32_t       engine_flags ) {
    /*if ( g3dobject_isActive ( (G3DOBJECT*) sdr ) ) {
        g3dsubdivider_activate ( sdr, engine_flags );
    }*/
}

/******************************************************************************/
uint32_t g3dmorpher_dump ( G3DSUBDIVIDER *sdr, void (*Alloc)( uint32_t, /* nbver */
                                                              uint32_t, /* nbtris */
                                                              uint32_t, /* nbquads */
                                                              uint32_t, /* nbuv */
                                                              void * ),
                                                  void (*Dump) ( G3DFACE *,
                                                                 void * ),
                                                  void *data,
                                                  uint32_t engine_flags ) {

}

/******************************************************************************/
G3DMESH *g3dmorpher_commit ( G3DMORPHER    *mpr, 
                             uint32_t       commitMeshID,
                             unsigned char *commitMeshName,
                             uint32_t       engine_flags ) {

}

/******************************************************************************/
G3DMORPHER *g3dmorpher_copy ( G3DMORPHER *mpr,
                              uint32_t    engine_flags ) {
}

/******************************************************************************/
void g3dmorpher_startUpdate ( G3DMORPHER *mpr,
                              uint32_t    engine_flags ) {

}

/******************************************************************************/
void g3dmorpher_update ( G3DMORPHER *mpr,
                         uint32_t    engine_flags ) {

}

/******************************************************************************/
void g3dmorpher_endUpdate ( G3DMORPHER *mpr,
                            uint32_t    engine_flags ) {

}

/******************************************************************************/
uint32_t g3dmorpher_modify ( G3DMORPHER *mpr,
                             uint32_t    engine_flags ) {

}

/******************************************************************************/
void g3dmorpher_activate ( G3DMORPHER *mpr,
                           uint32_t    engine_flags ) {

}

/******************************************************************************/
void g3dmorpher_deactivate ( G3DMORPHER *mpr,
                             uint32_t    engine_flags ) {

}

/******************************************************************************/
uint32_t g3dmorpher_draw ( G3DMORPHER *mpr, 
                           G3DCAMERA  *cam,
                           uint32_t    engine_flags ) {

    return MODIFIERTAKESOVER;
}

/******************************************************************************/
void g3dmorpher_init ( G3DMORPHER *mpr, 
                       uint32_t    id, 
                       char       *name, 
                       uint32_t    engine_flags ) {
    G3DMODIFIER *mod = ( G3DMODIFIER * ) mpr;

    g3dmodifier_init ( mod, G3DMORPHERTYPE, id, name, DRAWBEFORECHILDREN,
                                           DRAW_CALLBACK(g3dmorpher_draw),
                                                         NULL,
                                                         NULL,
                                                         NULL,
                                           COPY_CALLBACK(g3dmorpher_copy),
                                       ACTIVATE_CALLBACK(g3dmorpher_activate),
                                     DEACTIVATE_CALLBACK(g3dmorpher_deactivate),
                                         COMMIT_CALLBACK(g3dmorpher_commit),
                                                         NULL,
                                      SETPARENT_CALLBACK(g3dmorpher_setParent),
                                         MODIFY_CALLBACK(g3dmorpher_modify),
                                    STARTUPDATE_CALLBACK(g3dmorpher_startUpdate),
                                         UPDATE_CALLBACK(g3dmorpher_update),
                                      ENDUPDATE_CALLBACK(g3dmorpher_endUpdate) );

    /*((G3DMESH*)sdr)->dump = DUMP_CALLBACK(g3dmorpher_dump);*/
}

/******************************************************************************/
G3DMORPHER *g3dmorpher_new ( uint32_t id, 
                             char    *name, 
                             uint32_t engine_flags ) {
    uint32_t structSize = sizeof ( G3DMORPHER );
    G3DMORPHER *mpr = ( G3DMORPHER * ) calloc ( 0x01, structSize );

    if ( mpr == NULL ) {
        fprintf ( stderr, "g3dmorpher_new: calloc failed\n" );

        return NULL;
    }

    g3dmorpher_init ( mpr, id, name, engine_flags );


    return mpr;
}

#endif

