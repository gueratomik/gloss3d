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
#include <g3dmouse.h>

/******************************************************************************/
/* each function must return FALSE for redrawing the OGL Widget it belongs to */
/* only or TRUE to redraw all OGL Widgets                                     */
/******************************************************************************/
G3DEXTRUDEFACE *extrudeFace_new ( ) {
    uint32_t structsize = sizeof ( G3DEXTRUDEFACE );

    G3DEXTRUDEFACE *ef =  ( G3DEXTRUDEFACE * ) calloc ( 0x04, structsize );

    if ( ef == NULL ) {
        fprintf ( stderr, "extrudeFace_new: Memory allocation failed\n" );
    }


    return ef;
}

/******************************************************************************/
uint32_t extrudeFace_init ( G3DMOUSETOOL *mou, G3DSCENE *sce, 
                                               G3DCAMERA *cam,
                                               G3DURMANAGER *urm, 
                                               uint32_t engine_flags ) {
    if ( mou->data == NULL ) {
        G3DEXTRUDEFACE *ef =  extrudeFace_new ( );

        ef->inner = 0x00;

        mou->data = ef;
    }

    return 0x00;
}

/******************************************************************************/
uint32_t extrudeInner_init ( G3DMOUSETOOL *mou, G3DSCENE *sce, 
                                                G3DCAMERA *cam,
                                                G3DURMANAGER *urm, 
                                                uint32_t engine_flags ) {
    if ( mou->data == NULL ) {
        G3DEXTRUDEFACE *ef =  extrudeFace_new ( );

        ef->inner = 0x01;

        mou->data = ef;
    }

    return 0x00;
}

/******************************************************************************/
int extrudeFace_tool  ( G3DMOUSETOOL *mou, G3DSCENE *sce, 
                                           G3DCAMERA *cam,
                                           G3DURMANAGER *urm, 
                                           uint32_t flags,
                                           G3DEvent *event ) {
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

    G3DEXTRUDEFACE *ef = ( G3DEXTRUDEFACE * ) mou->data;

    /*** This tool can only be used in face mode ***/
    if ( ( flags & VIEWFACE ) == 0x00 ) return FALSE;

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            xold = bev->x;
            yold = bev->y;

            obj = g3dscene_getLastSelected ( sce );

            if ( obj ) {
                if ( obj->type & MESH ) {
                    LIST *ltmpver;
                    G3DVECTOR pivot;

                    mes = ( G3DMESH * ) obj;

                    g3dmesh_extrude ( mes, &loriver, &loldfac, 
                                           &lnewver, &lnewfac );

                    /*** regenerate subdivision buffer ***/
                    g3dmesh_update ( mes, NULL,
                                          NULL,
                                          NULL,
                                          NULL,
                                          COMPUTEFACEPOSITION |
                                          COMPUTEFACENORMAL   |
                                          COMPUTEEDGEPOSITION |
                                          COMPUTEVERTEXNORMAL |
                                          COMPUTEUVMAPPING    |
                                          REALLOCSUBDIVISION  |
                                          COMPUTESUBDIVISION, flags );

                    ltmpver = lver = g3dmesh_getVertexListFromSelectedFaces ( mes );

                    lselfac = g3dvertex_getFacesFromList  ( lver    );
                    lseledg = g3dface_getEdgesFromList    ( lselfac );
                    lselver = g3dface_getVerticesFromList ( lselfac );

                    if ( mes->subdiv && (((G3DOBJECT*)mes)->flags & BUFFEREDSUBDIVISION) ) {
                        lselsub = g3dvertex_getAreaFacesFromList ( lver );
                    }

                    /*if ( ef->inner ) {
                        g3dvertex_getAveragePositionFromList ( lver, &pivot );
                    }*/

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

            if ( obj ) {
                if ( obj->type == G3DMESHTYPE ) {
                    G3DMESH *mes = ( G3DMESH * ) obj;
                    LIST *ltmpver = lver, *ltmpdir = ldir;

                    while ( ltmpver ) {
                        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                        G3DVECTOR *dir = ( G3DVECTOR * ) ltmpdir->data;
                        float diff = ( float ) mev->x - xold;

                        ver->pos.x = ver->pos.x + ( dir->x * diff / 50.0f );
                        ver->pos.y = ver->pos.y + ( dir->y * diff / 50.0f );
                        ver->pos.z = ver->pos.z + ( dir->z * diff / 50.0f );

                        ltmpver = ltmpver->next;
                        ltmpdir = ltmpdir->next;
                    }
                }

                g3dmesh_update ( mes, lselver,
                                      lseledg,
                                      lselfac,
                                      lselsub,
                                      COMPUTEFACEPOSITION |
                                      COMPUTEFACENORMAL   |
                                      COMPUTEEDGEPOSITION |
                                      COMPUTEVERTEXNORMAL |
                                      COMPUTEUVMAPPING    |
                                      COMPUTESUBDIVISION, flags );
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
