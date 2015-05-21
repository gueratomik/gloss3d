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
uint32_t g3dface_catmull_clark_drawV2 ( G3DSUBDIVISIONTHREAD *std,
                                        G3DSUBVERTEX  *subvertab, uint32_t nbsubver,
                                        G3DSUBEDGE    *subedgtab, uint32_t nbsubedg,
                                        G3DSUBFACE    *subfactab, uint32_t nbsubfac,
                                        G3DFACE       *ancestor,
                /*** get triangles ***/ G3DRTTRIANGLE **rttriptr,
                /*** get quads     ***/ G3DRTQUAD     **rtquaptr,
                /*** get uvws      ***/ G3DRTUVSET    **rtuvsptr,
                                        uint32_t curdiv,
                                        uint32_t object_flags,
                                        uint32_t engine_flags ) {
    G3DSUBVERTEX *newsubvermem = std->newsubvermem, *newsubverptr = newsubvermem;
    G3DSUBEDGE   *newsubedgmem = std->newsubedgmem, *newsubedgptr = newsubedgmem;
    G3DSUBFACE   *newsubfacmem = std->newsubfacmem, *newsubfacptr = newsubfacmem;
    /*uint32_t nbnewsubver, nbnewsubedg, nbnewsubfac;*/
    uint32_t i, nbfacnew = 0x00;
    uint32_t totsubver = 0x00, totsubfac = 0x00;

    if ( meminit ) {

    } else {*/
        newsubvermem = newsubverptr = calloc ( 16384, sizeof ( G3DSUBVERTEX ) );
        newsubedgmem = newsubedgptr = calloc ( 16384, sizeof ( G3DSUBEDGE   ) );
        newsubfacmem = newsubfacptr = calloc ( 16384, sizeof ( G3DSUBFACE   ) );

        freesubverptr = newsubvermem;
        freesubedgptr = newsubedgmem; 
        freesubfacptr = newsubfacmem;


    while ( curdiv-- ) {
        memset ( newsubvermem, 0x00, sizeof ( G3DSUBVERTEX ) * ( nbsubfac + 
                                                                 nbsubedg +
                                                                 nbsubver );
        memset ( newsubedgmem, 0x00, sizeof ( G3DSUBEDGE   ) * ( nbsubfac * 4 + 
                                                                 nbsubedg * 2 );
        memset ( newsubfacmem, 0x00, sizeof ( G3DSUBFACE   ) * ( nbsubfac * 4 );


        for ( i = 0x00; i < nbsubver; i++ ) {
          if ( subvertab[i].ver.flags & VERTEXMALLOCFACES ) newsubverptr->ver.flags |= VERTEXMALLOCFACES;

          if ( subvertab[i].ver.flags & VERTEXTOPOLOGY ) {
            memcpy ( &newsubverptr->ver.pos, &subvertab[i].ver.pos, sizeof ( G3DVECTOR ) );
            newsubverptr->ver.flags |= VERTEXTOPOLOGY;
            subvertab[i].ver.subver = newsubverptr++;
          }
        }

        /*** assign mid-edge vertices ***/
        for ( i = 0x00; i < nbsubedg; i++ ) {
            newsubverptr->ver.pos.x = ( subedgtab[i].edg.ver[0x00]->pos.x +
                                        subedgtab[i].edg.ver[0x01]->pos.x ) / 2;
            newsubverptr->ver.pos.y = ( subedgtab[i].edg.ver[0x00]->pos.y +
                                        subedgtab[i].edg.ver[0x01]->pos.y ) / 2;
            newsubverptr->ver.pos.z = ( subedgtab[i].edg.ver[0x00]->pos.z +
                                        subedgtab[i].edg.ver[0x01]->pos.z ) / 2;

            if ( subedgtab[i].edg.flags & EDGEMALLOCFACES ) newsubedgptr->edg.flags |= EDGEMALLOCFACES;

            if ( ( subedgtab[i].edg.ver[0x00]->flags & VERTEXTOPOLOGY ) &&
                 ( subedgtab[i].edg.ver[0x01]->flags & VERTEXTOPOLOGY ) ) {
                newsubverptr->ver.flags |= VERTEXTOPOLOGY;
            }

            subedgtab[i].edg.subver = newsubverptr++;

            if ( subedgtab[i].edg.ver[0x00]->flags & VERTEXTOPOLOGY ) {
                newsubedgptr->edg.ver[0x00] = subedgtab[i].edg.ver[0x00]->subver;
                newsubedgptr->edg.ver[0x01] = subedgtab[i].edg.subver;
    /** TOPO **/if ( curdiv ) g3dsubvertex_addEdge ( subedgtab[i].edg.subver, newsubedgptr );
                subedgtab[i].edg.subedg[0x00] = newsubedgptr;
                newsubedgptr++;
            }

            if ( subedgtab[i].edg.ver[0x01]->flags & VERTEXTOPOLOGY ) {
                newsubedgptr->edg.ver[0x00] = subedgtab[i].edg.ver[0x01]->subver;
                newsubedgptr->edg.ver[0x01] = subedgtab[i].edg.subver;
    /** TOPO **/if ( curdiv ) g3dsubvertex_addEdge ( subedgtab[i].edg.subver, newsubedgptr );
                subedgtab[i].edg.subedg[0x01] = newsubedgptr;
                newsubedgptr++;
            }
        }

        /*** assign face-center vertices ***/
        for ( i = 0x00; i < nbsubfac; i++ ) {
            newsubverptr->ver.pos.x = ( subfactab[i].fac.ver[0x00]->pos.x +
                                        subfactab[i].fac.ver[0x01]->pos.x +
                                        subfactab[i].fac.ver[0x02]->pos.x +
                                        subfactab[i].fac.ver[0x03]->pos.x ) / 4;
            newsubverptr->ver.pos.y = ( subfactab[i].fac.ver[0x00]->pos.y +
                                        subfactab[i].fac.ver[0x01]->pos.y +
                                        subfactab[i].fac.ver[0x02]->pos.y +
                                        subfactab[i].fac.ver[0x03]->pos.y ) / 4;
            newsubverptr->ver.pos.z = ( subfactab[i].fac.ver[0x00]->pos.z +
                                        subfactab[i].fac.ver[0x01]->pos.z +
                                        subfactab[i].fac.ver[0x02]->pos.z +
                                        subfactab[i].fac.ver[0x03]->pos.z ) / 4;

            if ( ( subfactab[i].fac.ver[0x00]->flags & VERTEXTOPOLOGY ) &&
                 ( subfactab[i].fac.ver[0x01]->flags & VERTEXTOPOLOGY ) &&
                 ( subfactab[i].fac.ver[0x02]->flags & VERTEXTOPOLOGY ) &&
                 ( subfactab[i].fac.ver[0x03]->flags & VERTEXTOPOLOGY ) ) {
                newsubverptr->ver.flags |= VERTEXTOPOLOGY;
            }

            subfactab[i].fac.subver = newsubverptr++;
        }

        /*** assign mid-edge to face-center edges ***/
        for ( i = 0x00; i < nbsubedg; i++ ) {
            LIST *ltmpfac = subedgtab[i].edg.lfac;

            while ( ltmpfac ) {
                G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

                newsubedgptr->edg.ver[0x00] = subedgtab[i].edg.subver;
                newsubedgptr->edg.ver[0x01] = fac->subver;
    /** TOPO **/if ( curdiv ) g3dsubvertex_addEdge ( subedgtab[i].edg.subver, newsubedgptr );
    /** TOPO **/if ( curdiv ) g3dsubvertex_addEdge ( fac->subver            , newsubedgptr );
                if ( fac->edg[0x00] == &subedgtab[i] ) fac->innedg[0x00] = newsubedgptr;
                if ( fac->edg[0x01] == &subedgtab[i] ) fac->innedg[0x01] = newsubedgptr;
                if ( fac->edg[0x02] == &subedgtab[i] ) fac->innedg[0x02] = newsubedgptr;
                if ( fac->edg[0x03] == &subedgtab[i] ) fac->innedg[0x03] = newsubedgptr;
                newsubedgptr++;

                ltmpfac = ltmpfac->next;
            }
        }

        for ( i = 0x00; i < nbsubver; i++ ) {
          if ( subvertab[i].ver.flags & VERTEXTOPOLOGY ) {
            LIST *ltmpfac = subvertab[i].ver.lfac;

            while ( ltmpfac ) {
                G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                uint32_t verID, prvID;

                if ( fac->ver[0x00] == &subvertab[i] ) { verID = 0x00; prvID = 0x03; }
                if ( fac->ver[0x01] == &subvertab[i] ) { verID = 0x01; prvID = 0x00; }
                if ( fac->ver[0x02] == &subvertab[i] ) { verID = 0x02; prvID = 0x01; }
                if ( fac->ver[0x03] == &subvertab[i] ) { verID = 0x03; prvID = 0x02; }

                if ( curdiv ) {
                newsubfacptr->fac.edg[0x00] = g3dedge_getSubEdge ( fac->edg[verID], subvertab[i].ver.subver, fac->edg[verID]->subver );
                newsubfacptr->fac.edg[0x01] = fac->innedg[verID];
                newsubfacptr->fac.edg[0x02] = fac->innedg[prvID];
                newsubfacptr->fac.edg[0x03] = g3dedge_getSubEdge ( fac->edg[prvID], subvertab[i].ver.subver, fac->edg[prvID]->subver );

                g3dsubedge_addFace ( newsubfacptr->fac.edg[0x00], newsubfacptr );
                g3dsubedge_addFace ( newsubfacptr->fac.edg[0x01], newsubfacptr );
                g3dsubedge_addFace ( newsubfacptr->fac.edg[0x02], newsubfacptr );
                g3dsubedge_addFace ( newsubfacptr->fac.edg[0x03], newsubfacptr );
                }

                newsubfacptr->fac.ver[0x00] = subvertab[i].ver.subver;
                newsubfacptr->fac.ver[0x01] = fac->edg[verID]->subver;
                newsubfacptr->fac.ver[0x02] = fac->subver;
                newsubfacptr->fac.ver[0x03] = fac->edg[prvID]->subver;

                g3dsubvertex_addFace ( newsubfacptr->fac.ver[0x00], newsubfacptr );
                g3dsubvertex_addFace ( newsubfacptr->fac.ver[0x01], newsubfacptr );
                g3dsubvertex_addFace ( newsubfacptr->fac.ver[0x02], newsubfacptr );
                g3dsubvertex_addFace ( newsubfacptr->fac.ver[0x03], newsubfacptr );

                if ( curdiv == 0x00 ) g3dface_normal ( newsubfacptr );

                if ( fac->flags & FACEORIGINAL ) {
                    newsubfacptr->fac.flags |= FACEORIGINAL;
                }

                newsubfacptr++;


                ltmpfac = ltmpfac->next;
            }
          }
        }

        nbsubver      = ((char*)newsubverptr-(char*)newsubvermem)/sizeof(G3DSUBVERTEX);
        subvertab     = newsubvermem;
        newsubvermem += nbsubver;

        nbsubedg      = ((char*)newsubedgptr-(char*)newsubedgmem)/sizeof(G3DSUBEDGE);
        subedgtab     = newsubedgmem;
        newsubedgmem += nbsubedg;

        nbsubfac      = ((char*)newsubfacptr-(char*)newsubfacmem)/sizeof(G3DSUBFACE);
        subfactab     = newsubfacmem;
        newsubfacmem += nbsubfac;
    }


        /*** draw vertices ***/
        /*glBegin ( GL_POINTS );
        for ( i = 0x00; i < nbsubver; i++ ) {
            glVertex3f ( subvertab[i].ver.pos.x, 
                         subvertab[i].ver.pos.y,
                         subvertab[i].ver.pos.z );
        }
        glEnd ( );*/

        for ( i = 0x00; i < nbsubver; i++ ) {
            if ( subvertab[i].ver.flags & VERTEXTOPOLOGY ) {
                g3dvertex_normal ( &subvertab[i], 0x00 );
            }
        }

        for ( i = 0x00; i < nbsubfac; i++ ) {
            if ( subfactab[i].fac.flags & FACEORIGINAL ) {
                g3dface_convert ( &subfactab[i], ancestor,
                                                 rttriptr, 
                                                 rtquaptr,
                                                 rtuvsptr, 
                                                 object_flags, 
                                                 engine_flags );

                totsubfac++;
            }
        }

        /*** draw faces ***/
        /*glBegin ( GL_QUADS );
        for ( i = 0x00; i < nbsubfac; i++ ) {
            if ( subfactab[i].fac.flags & FACEORIGINAL ) {
                glNormal3fv ( &subfactab[i].fac.ver[0x00]->nor );
                glVertex3fv ( &subfactab[i].fac.ver[0x00]->pos );
                glNormal3fv ( &subfactab[i].fac.ver[0x01]->nor );
                glVertex3fv ( &subfactab[i].fac.ver[0x01]->pos );
                glNormal3fv ( &subfactab[i].fac.ver[0x02]->nor );
                glVertex3fv ( &subfactab[i].fac.ver[0x02]->pos );
                glNormal3fv ( &subfactab[i].fac.ver[0x03]->nor );
                glVertex3fv ( &subfactab[i].fac.ver[0x03]->pos );
            }
        }
        glEnd ( );*/

    /*free ( freesubverptr );
    free ( freesubedgptr );
    free ( freesubfacptr );*/

    return totsubfac;
}




