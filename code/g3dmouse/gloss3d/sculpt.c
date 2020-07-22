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

static int sculpt_tool ( G3DMOUSETOOL *mou, 
                         G3DSCENE     *sce, 
                         G3DCAMERA    *cam,
                         G3DURMANAGER *urm, 
                         uint64_t      engine_flags, 
                         G3DEvent     *event );

/******************************************************************************/
uint32_t directionChange ( uint32_t x, uint32_t y ) {
    static uint32_t oldx, oldy;
    static G2DVECTOR olddir;
    static int32_t oldres;
    G2DVECTOR dir = { x - oldx, y - oldy };
    int32_t res;
    uint32_t change = 0;

    res = g2dvector_scalar ( &dir, &olddir );

    if ( res * oldres < 0 ) change = 0x01;

    oldres = res;

    olddir.x = dir.x;
    olddir.y = dir.y;

    oldx = x;
    oldy = y;

    return change;
}

/******************************************************************************/
G3DMOUSETOOLSCULPT *sculptTool_new ( ) {
    uint32_t structsize = sizeof ( G3DMOUSETOOLSCULPT );

    G3DMOUSETOOLSCULPT *st =  ( G3DMOUSETOOLSCULPT * ) calloc ( 0x01, structsize );

    if ( st == NULL ) {
        fprintf ( stderr, "sculptTool_new: Memory allocation failed\n" );

        return NULL;
    }

    st->only_visible = 0x01;
    st->pressure     = 0.5f;
    st->radius       = 0x20;

    return st;
}

#define BUFFERSIZE 0x10000

/******************************************************************************/
void sculpt_pick ( G3DMOUSETOOLSCULPT *st, 
                   G3DMESH            *mes,
                   G3DCAMERA          *cam,
                   G3DVECTOR          *dir,
                   uint64_t            engine_flags ) {
#ifdef REFURSBISH
    int MX = st->coord[0x00] + ( ( st->coord[0x02] - st->coord[0x00] ) >> 0x01 ),
        MY = st->coord[0x01] + ( ( st->coord[0x03] - st->coord[0x01] ) >> 0x01 ),
        MW = ( st->coord[0x02] - st->coord[0x00] ), /*** width ***/
        MH = ( st->coord[0x03] - st->coord[0x01] ); /*** height ***/
    G3DVECTOR vec = { 0.f, 0.f, 0.f, 1.0f };
    uint32_t nbFaces, nbEdges, nbVertices;
    static GLint VPX[0x04];
    static double PJX[0x10];
    int middle = 0x00;
    GLuint buffer[BUFFERSIZE], *ptr = buffer;
    GLint min, max;
    uint32_t i, j;
    GLint hits;
    G3DSYSINFO *sif = g3dsysinfo_get ( );
    /*** Get the temporary subdivision arrays for CPU #0 ***/
    G3DSUBDIVISION *sdv = g3dsysinfo_getSubdivision ( sif, 0x00 );
    G3DDOUBLEVECTOR near;
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    uint32_t triFaces, triEdges, triVertices;
    uint32_t quaFaces, quaEdges, quaVertices;
    uint32_t subdiv = mes->subdiv;
    LIST *ltmpfac = mes->lfac;
    LIST *lfac = NULL, *lextfac = NULL;
    uint32_t dirchange;
    LIST *ltmpscf = st->lscf;

    g3dtriangle_evalSubdivision ( subdiv, &triFaces, &triEdges, &triVertices );
    g3dquad_evalSubdivision     ( subdiv, &quaFaces, &quaEdges, &quaVertices );

    /*if ( MW == 0x00 ) { MW = 0x30; middle = 0x01; };
    if ( MH == 0x00 ) { MH = 0x30; middle = 0x01; };*/

    glSelectBuffer ( BUFFERSIZE, buffer );

    glRenderMode ( GL_SELECT );

    glInitNames ( );
    glPushName ( 0xFFFFFFFF );

    glGetIntegerv ( GL_VIEWPORT, VPX );

    glMatrixMode ( GL_PROJECTION );
    glPushMatrix ( );
    glLoadIdentity ( );
    gluPickMatrix ( MX, MY, st->radius, st->radius, VPX );
    g3dcamera_project ( cam, engine_flags );
    glGetDoublev ( GL_PROJECTION_MATRIX, PJX );

    glMatrixMode ( GL_MODELVIEW );
    glPushMatrix ( );
    glLoadIdentity ( );
    g3dcamera_view ( cam, 0x00 );

    glMultMatrixd ( ((G3DOBJECT*)mes)->wmatrix );

    if ( directionChange ( MX, MY ) ) {
        while ( ltmpscf ) {
            G3DHEIGHTMAP *htm = ( G3DHEIGHTMAP * ) ltmpscf->data;

            for ( i = 0x00; i < htm->maxheights; i++ ) {
                 htm->heights[i].flags &= ~(0x80000000);
            }

            ltmpscf = ltmpscf->next;
        }
    }

    if ( ( obj->flags & BUFFEREDSUBDIVISION ) ) {
        if ( mes->subdiv ) {
            ltmpfac = mes->lfac;

            while ( ltmpfac ) {
                G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                uint32_t nbrtver = ( fac->nbver == 0x04 ) ? quaVertices : triVertices;

                for ( i = 0x00; i < nbrtver; i++ ) {
                    glLoadName ( ( fac->id << 0x10 ) | i );
                    glBegin ( GL_POINTS );
                    glVertex3fv ( &fac->rtvermem[i].pos );
                    glEnd ( );
                }

                ltmpfac = ltmpfac->next;
            }
        }
    } else {
        g3dmesh_drawSubdividedObject ( mes, engine_flags | SELECTMODE );
    }

    hits = glRenderMode ( GL_RENDER );
    /*printf ("hits:%d\n", hits );*/

    for ( i = 0x00; i < hits; i++ ) {
        GLuint nbname = (*ptr++);

        min = *(ptr++); /** D0 NOT  COMMENT THIS ***/
        max = *(ptr++); /** D0 NOT  COMMENT THIS ***/
    /*printf ("nbname:%d\n", nbname );*/
        for ( j = 0x00; j < nbname; j++ ) {
            GLuint name = (*ptr++);

          if ( name != 0xFFFFFFFF ) {
            uint32_t facID = name >> 0x10;
            uint32_t verID = name & 0x000FFFF;
            G3DFACE *fac = mes->faceindex[facID];

            /*** prepare redrawing ***/
            if ( list_seek ( lfac, fac ) == NULL ) {
                list_insert ( &lfac, fac );
            }

            if ( fac->heightmap ) {
                uint32_t nbheights = ( fac->nbver == 0x04 ) ? quaVertices : triVertices;

                if ( fac->heightmap->maxheights < nbheights ) {
                    g3dheightmap_realloc ( fac->heightmap, nbheights );
                }
            } else {
                if ( fac->nbver == 0x03 ) fac->heightmap = g3dheightmap_new ( triVertices );
                if ( fac->nbver == 0x04 ) fac->heightmap = g3dheightmap_new ( quaVertices );

                mes->nbhtm++;
            }

            if ( list_seek ( st->lscf, fac->heightmap ) == NULL ) {
                list_insert ( &st->lscf, fac->heightmap );
            }

            fac->heightmap->heights[verID].flags |= HEIGHTSET;

            if ( (fac->heightmap->heights[verID].flags & 0x80000000) == 0x00 ) {
                if ( st->ctrl_key ) {
                    fac->heightmap->heights[verID].elevation -= (st->pressure * 0.01f);
                } else {
                    fac->heightmap->heights[verID].elevation += (st->pressure * 0.01f);
                }
/*printf("%\d %d\n", facID, verID );*/
                fac->heightmap->heights[verID].flags |= 0x80000000; /*** no more change allowed ***/
            }

            /*list_insert ( &lis, ( void * ) cname );*/
            /*printf ("facID:%d verID:%d\n", facID, verID );*/
        }
      }
    }

    lextfac = g3dface_getNeighbourFacesFromList ( lfac );

    /*g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          NULL,
                          COMPUTESUBDIVISION, engine_flags );*/


    ltmpfac = lextfac;
    /*ltmpfac = lfac;*/


    if ( lextfac ) g3dmesh_fillSubdividedFaces ( mes, lextfac, engine_flags );

    /*while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        uint32_t (*subindex)[0x04] = g3dsubindex_get ( fac->nbver, mes->subdiv );

        g3dsubdivisionV3_subdivide ( sdv, fac,
                                          fac->rtfacmem,
                                          fac->rtedgmem,
                                          fac->rtvermem,
                                          subindex,
                                          mes->subdiv,
                                          ((G3DOBJECT*)mes)->flags,
                                          engine_flags );

        ltmpfac = ltmpfac->next;
    }*/

    list_free ( &lextfac, NULL );
    list_free ( &lfac, NULL );
#endif
}

/******************************************************************************/
static void sculpt_free ( void *data ) {

}

/******************************************************************************/
static int sculpt_tool ( G3DMOUSETOOL *mou, 
                         G3DSCENE     *sce, 
                         G3DCAMERA    *cam,
                         G3DURMANAGER *urm, 
                         uint64_t      engine_flags, 
                         G3DEvent     *event ) {
    G3DMOUSETOOLSCULPT *st = ( G3DMOUSETOOLSCULPT * ) mou;
    static GLint VPX[0x04];
    static G3DOBJECT *obj;
    static GLdouble MVX[0x10], PJX[0x10];

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            obj = g3dscene_getLastSelected ( sce );

            /*** Sculpting is only available to buffer subdivided objects because ***/
            /*** we can only retrieve the Z values thanks to the coordinates stored***/
            /*** in the rtvermem buffer. ***/
            /*if ( ( obj->flags & BUFFEREDSUBDIVISION ) == 0x00 ) {
                printf ( stderr, "sculpt_tool: Sculpting is only available to "
                                 "buffer subdivided objects" );

                obj = NULL;

                return 0x00;
            }*/

            /*glGetIntegerv ( GL_VIEWPORT, VPX );*/
        } break;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;
            G3DDOUBLEVECTOR nearvec, farvec;
            G3DVECTOR dir;

            st->ctrl_key = ( mev->state & G3DControlMask ) ? 0x01 : 0x00;

            if ( obj && ( obj->type & MESH ) ) {
                glPushMatrix ( );
                glMultMatrixd ( obj->wmatrix );
                glGetDoublev  ( GL_MODELVIEW_MATRIX, MVX  );
                glPopMatrix ( );

                glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
                glGetIntegerv ( GL_VIEWPORT, VPX );

                gluUnProject ( ( GLdouble ) mev->x,
                               ( GLdouble ) VPX[0x03] - mev->y,
                               ( GLdouble ) 0,
                               MVX, PJX, VPX,
                               &nearvec.x, &nearvec.y, &nearvec.z );

                gluUnProject ( ( GLdouble ) mev->x,
                               ( GLdouble ) VPX[0x03] - mev->y,
                               ( GLdouble ) 1,
                               MVX, PJX, VPX,
                               &farvec.x, &farvec.y, &farvec.z );

                dir.x = farvec.x - nearvec.x;
                dir.y = farvec.y - nearvec.y;
                dir.z = farvec.z - nearvec.z;

                g3dvector_normalize ( &dir, NULL );

                if ( engine_flags & VIEWSCULPT ) {
                    st->coord[0x00] = st->coord[0x02] = mev->x;
                    st->coord[0x01] = st->coord[0x03] = VPX[0x03] - mev->y;

                    G3DMESH *mes = ( G3DMESH * ) obj;

                    sculpt_pick ( st, mes, cam, &dir, engine_flags );
                }
            }
        } break;

        case G3DButtonRelease : {
            list_free ( &st->lscf, NULL );

            obj = NULL;
        } break;

        default:
        break;
    }

    return REDRAWVIEW;
}
