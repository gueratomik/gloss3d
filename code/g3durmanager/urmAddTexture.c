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
typedef struct _TEXRECORD {
    G3DMESH    *mes;
    G3DTEXTURE *tex;
    G3DUVMAP    *map;
    uint32_t    mapcreated;
    LIST       *luvset;
} TEXRECORD;

/******************************************************************************/
typedef struct _URMADDTEXTURE {
    LIST        *ltrd;
} URMADDTEXTURE;

/******************************************************************************/
static void texrecord_free ( TEXRECORD *trd ) {
    free ( trd );
}

/******************************************************************************/
static TEXRECORD *texrecord_new ( G3DMESH    *mes,
                                  G3DTEXTURE *tex,
                                  uint32_t    mapcreated ) {
    TEXRECORD *trd = ( TEXRECORD * ) calloc ( 0x01, sizeof ( TEXRECORD ) );

    if ( trd == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    trd->mes        = mes;
    trd->tex        = tex;
    /*** Note: g3dmesh_removeTexture() clears the map filed from tex, so we ***/
    /*** have to remeber the pointer here ***/
    trd->map        = tex->map;
    trd->mapcreated = mapcreated;

    return trd;
}

/******************************************************************************/
URMADDTEXTURE *urmaddtexture_new ( uint64_t engine_flags ) {
    uint32_t structsize = sizeof ( URMADDTEXTURE );

    URMADDTEXTURE *uat = ( URMADDTEXTURE * ) calloc ( 0x01, structsize );

    if ( uat == NULL ) {
        fprintf ( stderr, "%s: memory allocation falied\n", __func__ );

        return NULL;
    }

    return uat;
}

/******************************************************************************/
void urmaddtexture_free ( URMADDTEXTURE *uat ) {
    list_free ( &uat->ltrd, LIST_FUNCDATA(texrecord_free) );

    free ( uat );
}

/******************************************************************************/
void addTexture_free ( void *data, uint32_t commit ) {
    URMADDTEXTURE *uat = ( URMADDTEXTURE * ) data;

    if ( commit == 0x00 ) {
        LIST *ltmptrd = uat->ltrd;

        while ( ltmptrd ) {
            TEXRECORD *trd = ( TEXRECORD* ) ltmptrd->data;

            /***If a UVMap was specially created for the occasion, delete it***/
            if ( trd->mapcreated ) {
                g3dobject_free ( ( G3DOBJECT * ) trd->map );
            }

            g3dtexture_free ( trd->tex );

            ltmptrd = ltmptrd->next;
        }
    }

    urmaddtexture_free ( uat );
}

/******************************************************************************/
void addTexture_undo ( G3DURMANAGER *urm, void *data, uint64_t flags ) {
    URMADDTEXTURE *uat = ( URMADDTEXTURE * ) data;
    LIST *ltmptrd = uat->ltrd;

    while ( ltmptrd ) {
        TEXRECORD *trd = ( TEXRECORD * ) ltmptrd->data;

        if ( trd->mapcreated ) {
            g3dmesh_removeUVMap ( trd->mes,
                                  trd->map, 
                                  NULL, 
                                  NULL,
                                  0x00 );
        }

        g3dmesh_removeTexture ( trd->mes, trd->tex );

        trd->mes->obj.update_flags |= RESETMODIFIERS;

        /*** Rebuild the mesh with modifiers (e.g for displacement) ***/
        g3dmesh_update ( trd->mes, flags );

        ltmptrd = ltmptrd->next;
    }
}

/******************************************************************************/
void addTexture_redo ( G3DURMANAGER *urm, void *data, uint64_t flags ) {
    URMADDTEXTURE *uat = ( URMADDTEXTURE * ) data;
    LIST *ltmptrd = uat->ltrd;

    while ( ltmptrd ) {
        TEXRECORD *trd = ( TEXRECORD * ) ltmptrd->data;

        if ( trd->mapcreated ) {
            g3dmesh_addUVMap ( trd->mes,
                               trd->map, 
                               NULL,
                               0x00 );
        }

        g3dmesh_addTexture ( trd->mes, trd->tex );

        trd->mes->obj.update_flags |= RESETMODIFIERS;

        /*** Rebuild the mesh with modifiers (e.g for displacement) ***/
        g3dmesh_update ( trd->mes, flags );

        ltmptrd = ltmptrd->next;
    }
}

/******************************************************************************/
void g3durm_selection_addTexture ( G3DURMANAGER *urm,
                                   LIST         *lobj,
                                   G3DMATERIAL  *mat,
                                   uint64_t      engine_flags,
                                   uint32_t      return_flags ) {
    LIST *ltmpobj = lobj;
    URMADDTEXTURE *uat;

    /*** save state ***/
    uat = urmaddtexture_new ( engine_flags );

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;
        uint32_t mapcreated = 0x00;

        if ( ( obj->type == G3DMESHTYPE ) ||
             ( obj->type == G3DTEXTTYPE ) ||
           ( ( obj->type &  G3DPRIMITIVETYPE ) == G3DPRIMITIVETYPE ) ) {
            G3DMESH  *mes = ( G3DMESH * ) obj;
            G3DUVMAP *map = g3dmesh_getLastUVMap ( mes );
            G3DTEXTURE *tex;

            /*** A textured mesh should ALWAYS have a UVMAP.     ***/
            /*** Likely design issue - to FIX when I have time ***/
            if ( map == NULL ) {
                map = g3duvmap_new ( "UVMap", UVMAPFLAT );

                g3dmesh_addUVMap ( mes, map, NULL, engine_flags );

                g3dmesh_unselectAllUVMaps ( mes );
                g3dmesh_selectUVMap ( mes, map );

                g3duvmap_applyProjection ( map, mes );

                mapcreated = 0x01;
            }

            tex = g3dtexture_new ( ( G3DOBJECT * ) mes, mat, map );

            g3dmesh_addTexture ( mes, tex );

            mes->obj.update_flags |= RESETMODIFIERS;

            /*** Rebuild the mesh with modifiers (e.g for displacement) ***/
            g3dmesh_update ( mes, engine_flags );

            list_insert ( &uat->ltrd, texrecord_new ( mes, 
                                                      tex, 
                                                      mapcreated ) );
        }

        ltmpobj = ltmpobj->next;
    }

    g3durmanager_push ( urm, 
                        addTexture_undo,
                        addTexture_redo,
                        addTexture_free, 
                        uat,
                        return_flags );
}
