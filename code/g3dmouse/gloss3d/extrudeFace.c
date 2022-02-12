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
#include <g3dmouse.h>

/******************************************************************************/
/* each function must return FALSE for redrawing the OGL Widget it belongs to */
/* only or TRUE to redraw all OGL Widgets                                     */
/******************************************************************************/

static uint32_t extrudeFace_init ( G3DMOUSETOOL *mou, 
                                   G3DSCENE     *sce, 
                                   G3DCAMERA    *cam,
                                   G3DURMANAGER *urm, 
                                   uint64_t      engine_flags );
static uint32_t extrudeInner_init ( G3DMOUSETOOL *mou,
                                    G3DSCENE     *sce, 
                                    G3DCAMERA    *cam,
                                    G3DURMANAGER *urm, 
                                    uint64_t      engine_flags );
static int extrudeFace_tool  ( G3DMOUSETOOL *mou, 
                               G3DSCENE     *sce, 
                               G3DCAMERA    *cam,
                               G3DURMANAGER *urm, 
                               uint64_t      engine_flags,
                               G3DEvent     *event );

/******************************************************************************/
G3DMOUSETOOLEXTRUDEFACE *g3dmousetoolextrudeface_new ( ) {
    uint32_t structsize = sizeof ( G3DMOUSETOOLEXTRUDEFACE );
    void *memarea = calloc ( 0x01, structsize );
    G3DMOUSETOOLEXTRUDEFACE *ef =  ( G3DMOUSETOOLEXTRUDEFACE * ) memarea;

    if ( ef == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    g3dmousetool_init ( ( G3DMOUSETOOL * ) ef,
                                           EXTRUDEFACETOOL,
                                           's',
                                           NULL,
                                           extrudeFace_init,
                                           NULL,
                                           extrudeFace_tool,
                                           0x00 );

    return ef;
}

/******************************************************************************/
G3DMOUSETOOLEXTRUDEFACE *g3dmousetoolextrudeinner_new ( ) {
    uint32_t structsize = sizeof ( G3DMOUSETOOLEXTRUDEFACE );
    void *memarea = calloc ( 0x01, structsize );
    G3DMOUSETOOLEXTRUDEFACE *ef =  ( G3DMOUSETOOLEXTRUDEFACE * ) memarea;

    if ( ef == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    g3dmousetool_init ( ( G3DMOUSETOOL * ) ef,
                                           EXTRUDEINNERTOOL,
                                           's',
                                           NULL,
                                           extrudeInner_init,
                                           NULL,
                                           extrudeFace_tool,
                                           0x00 );

    return ef;
}

/******************************************************************************/
static uint32_t extrudeFace_init ( G3DMOUSETOOL *mou, 
                                   G3DSCENE     *sce, 
                                   G3DCAMERA    *cam,
                                   G3DURMANAGER *urm, 
                                   uint64_t      engine_flags ) {
    G3DMOUSETOOLEXTRUDEFACE *ef = ( G3DMOUSETOOLEXTRUDEFACE * ) mou;

    ef->inner = 0x00;

    return 0x00;
}

/******************************************************************************/
static uint32_t extrudeInner_init ( G3DMOUSETOOL *mou,
                                    G3DSCENE     *sce, 
                                    G3DCAMERA    *cam,
                                    G3DURMANAGER *urm, 
                                    uint64_t      engine_flags ) {
    G3DMOUSETOOLEXTRUDEFACE *ef = ( G3DMOUSETOOLEXTRUDEFACE * ) mou;

    ef->inner = 0x01;

    return 0x00;
}

/******************************************************************************/
static int extrudeFace_tool  ( G3DMOUSETOOL *mou, 
                               G3DSCENE     *sce, 
                               G3DCAMERA    *cam,
                               G3DURMANAGER *urm, 
                               uint64_t      engine_flags,
                               G3DEvent     *event ) {
    static GLdouble MVX[0x10], PJX[0x10];
    static GLint VPX[0x04];
    static int32_t xold, yold;
    static double objx, objy, objz,
                  winx, winy, winz;
    static G3DVECTOR center;
    static G3DOBJECT *obj;
    static G3DMESH *mes;
    static LIST *ldir; /*** list of rail vectors ***/
    /*** Remeber the items for undoing-redoing ***/
    static LIST *loriver = NULL;
    static LIST *loldfac = NULL;
    static LIST *lnewver = NULL;
    static LIST *lnewfac = NULL;
    /*** Remember the position for undoing-redoing ***/
    static G3DVECTOR *oldpos;
    static G3DVECTOR *newpos;
    /*** for updating mesh precomputed values (catmull-clark) ***/
    static LIST *lselver = NULL,
                *lseledg = NULL,
                *lselfac = NULL, 
                *lselsub = NULL,
                *lver    = NULL;
    static uint32_t nbver;

    G3DMOUSETOOLEXTRUDEFACE *ef = ( G3DMOUSETOOLEXTRUDEFACE * ) mou;

    /*** This tool can only be used in face mode ***/
    if ( ( engine_flags & VIEWFACE ) == 0x00 ) return FALSE;

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            xold = bev->x;
            yold = bev->y;

            obj = g3dscene_getLastSelected ( sce );

            if ( obj ) {
                if ( obj->type & MESH ) {
                    LIST *ltmpver;

                    mes = ( G3DMESH * ) obj;

                    g3dmesh_extrude ( mes, &loriver, &loldfac, 
                                           &lnewver, &lnewfac );

                    mes->obj.update_flags |= ( UPDATEFACEPOSITION |
                                               UPDATEFACENORMAL   |
                                               UPDATEVERTEXNORMAL |
                                               COMPUTEUVMAPPING   |
                                               RESETMODIFIERS );

                    /*** regenerate subdivision buffer ***/
                    g3dmesh_update ( mes, engine_flags );

                    ltmpver = lver = g3dmesh_getVertexListFromSelectedFaces ( mes );

                    nbver = list_count ( lver );

                    if ( mes->onGeometryMove ) {
                        mes->onGeometryMove ( mes, lver,
                                                   NULL, 
                                                   NULL, 
                                                   G3DMODIFYOP_STARTUPDATE,
                                                   engine_flags );
                    }

                    /*g3dmesh_modify ( mes,
                                     G3DMODIFYOP_STARTUPDATE,
                                     engine_flags );*/

                    /*** for undo redo ***/
                    g3dvertex_copyPositionFromList ( lver, &oldpos ); 

                    while ( ltmpver ) {
                        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                        G3DVECTOR dir;

                        if ( ef->inner ) {
                            /*dir.x = ( pivot.x - ver->pos.x );
                            dir.y = ( pivot.y - ver->pos.y );
                            dir.z = ( pivot.z - ver->pos.z );*/

                            g3dvertex_getVectorFromSelectedFaces ( ver, &dir );
                        } else {
                            g3dvertex_getNormalFromSelectedFaces ( ver, &dir );
                        } 

                        list_append ( &ldir, g3dvector_new ( dir.x,
                                                             dir.y,
                                                             dir.z, 1.0f ) );

                        ltmpver = ltmpver->next;
                    }
                }
            }
        } return REDRAWVIEW;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;
            uint32_t ctrlClick = ( mev->state & G3DControlMask ) ? 1 : 0;
            float factor = ( ctrlClick ) ? 500.0f : 50.0f;

            if ( obj ) {
                if ( obj->type == G3DMESHTYPE ) {
                    G3DMESH *mes = ( G3DMESH * ) obj;
                    LIST *ltmpver = lver, *ltmpdir = ldir;
                    float diff = ( float ) mev->x - xold;

                    memset ( &sce->csr.pivot, 0x00, sizeof ( G3DVECTOR ) );

                    while ( ltmpver ) {
                        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                        G3DVECTOR *dir = ( G3DVECTOR * ) ltmpdir->data;


                        ver->pos.x = ver->pos.x + ( dir->x * diff / factor );
                        ver->pos.y = ver->pos.y + ( dir->y * diff / factor );
                        ver->pos.z = ver->pos.z + ( dir->z * diff / factor );

                        sce->csr.pivot.x += ver->pos.x;
                        sce->csr.pivot.y += ver->pos.y;
                        sce->csr.pivot.z += ver->pos.z;

                        ltmpver = ltmpver->next;
                        ltmpdir = ltmpdir->next;
                    }

                    if ( nbver ) {
                        sce->csr.pivot.x /= nbver;
                        sce->csr.pivot.y /= nbver;
                        sce->csr.pivot.z /= nbver;
                    }
                }

                /*g3dmesh_modify ( mes,
                                 G3DMODIFYOP_UPDATE,
                                 engine_flags );*/

                if ( mes->onGeometryMove ) {
                     mes->onGeometryMove ( mes, lselver, 
                                                NULL, 
                                                lselfac, 
                                                G3DMODIFYOP_UPDATE,
                                                engine_flags );
                }
            }

            xold = mev->x;
            yold = mev->y;
        } return REDRAWVIEW;

        case G3DButtonRelease : {
            g3dvertex_copyPositionFromList ( lver, &newpos );

            g3durm_mesh_extrude ( urm, mes, lver, 
                                            loldfac,
                                            lnewver,
                                            lnewfac,
                                            oldpos,
                                            newpos,
                                            REDRAWVIEW );

            if ( mes->onGeometryMove ) {
                mes->onGeometryMove ( mes, lver,
                                           NULL, 
                                           NULL, 
                                           G3DMODIFYOP_ENDUPDATE,
                                           engine_flags );
            }

            /*g3dmesh_modify ( mes,
                             G3DMODIFYOP_ENDUPDATE,
                             engine_flags );*/

            g3dmesh_updateBbox ( mes );

            /*** reset pointers because those variables are static ***/
            loriver = loldfac = lnewver = lnewfac = NULL;
            /*** reset pointers because those variables are static ***/
            oldpos = newpos = NULL;

            list_free ( &lselver, NULL );
            list_free ( &lseledg, NULL );
            list_free ( &lselfac, NULL );
            list_free ( &lselsub, NULL );

            /*list_free ( &lver, NULL           );*/
            list_free ( &ldir, g3dvector_free );

            obj = NULL;
        } return REDRAWVIEW;

        default : {

        } break;
    }

    return FALSE;
}
