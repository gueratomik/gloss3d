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
void g3dsubindex_buildFromQuad ( G3DSUBINDEX *subindexes, 
                                 uint32_t subdiv_level ) {
    G3DSYSINFO *sif = g3dsysinfo_get ( );
    /*** Get the temporary subdivision arrays for CPU #0 ***/
    G3DSUBDIVISION *sdv = g3dsysinfo_getSubdivision ( sif, 0x00 );
    uint32_t nbfac = pow ( 0x04, subdiv_level );
    /*** we use the G3DSUB version so we don't have to free memory ***/
    G3DSUBVERTEX subver[0x04];
    G3DSUBEDGE   subedg[0x04];
    G3DSUBFACE   subfac;

    memset ( &subver, 0x00, sizeof ( G3DSUBVERTEX ) * 0x04 );
    memset ( &subedg, 0x00, sizeof ( G3DSUBEDGE   ) * 0x04 );
    memset ( &subfac, 0x00, sizeof ( G3DSUBFACE   ) );

    subindexes->qua = (uint32_t(*)[4]) calloc ( nbfac, sizeof ( uint32_t ) * 0x04 );

    subedg[0x00].edg.ver[0x00] = (G3DVERTEX*)&subver[0x00];
    subedg[0x00].edg.ver[0x01] = (G3DVERTEX*)&subver[0x01];
    subedg[0x01].edg.ver[0x00] = (G3DVERTEX*)&subver[0x01];
    subedg[0x01].edg.ver[0x01] = (G3DVERTEX*)&subver[0x02];
    subedg[0x02].edg.ver[0x00] = (G3DVERTEX*)&subver[0x02];
    subedg[0x02].edg.ver[0x01] = (G3DVERTEX*)&subver[0x03];
    subedg[0x03].edg.ver[0x00] = (G3DVERTEX*)&subver[0x03];
    subedg[0x03].edg.ver[0x01] = (G3DVERTEX*)&subver[0x00];

    subfac.fac.ver[0x00] = (G3DVERTEX*)&subver[0x00];
    subfac.fac.ver[0x01] = (G3DVERTEX*)&subver[0x01];
    subfac.fac.ver[0x02] = (G3DVERTEX*)&subver[0x02];
    subfac.fac.ver[0x03] = (G3DVERTEX*)&subver[0x03];

    subfac.fac.edg[0x00] = (G3DEDGE*)&subedg[0x00];
    subfac.fac.edg[0x01] = (G3DEDGE*)&subedg[0x01];
    subfac.fac.edg[0x02] = (G3DEDGE*)&subedg[0x02];
    subfac.fac.edg[0x03] = (G3DEDGE*)&subedg[0x03];

    subfac.fac.nbver     = 0x04;

    g3dsubvertex_addFace ( &subver[0x00], (G3DFACE*)&subfac );
    g3dsubvertex_addFace ( &subver[0x01], (G3DFACE*)&subfac );
    g3dsubvertex_addFace ( &subver[0x02], (G3DFACE*)&subfac );
    g3dsubvertex_addFace ( &subver[0x03], (G3DFACE*)&subfac );

    g3dsubedge_addFace ( &subedg[0x00], (G3DFACE*)&subfac );
    g3dsubedge_addFace ( &subedg[0x01], (G3DFACE*)&subfac );
    g3dsubedge_addFace ( &subedg[0x02], (G3DFACE*)&subfac );
    g3dsubedge_addFace ( &subedg[0x03], (G3DFACE*)&subfac );

    g3dsubvertex_addEdge ( &subver[0x00], (G3DEDGE*)&subedg[0x03] );
    g3dsubvertex_addEdge ( &subver[0x00], (G3DEDGE*)&subedg[0x00] );
    g3dsubvertex_addEdge ( &subver[0x01], (G3DEDGE*)&subedg[0x00] );
    g3dsubvertex_addEdge ( &subver[0x01], (G3DEDGE*)&subedg[0x01] );
    g3dsubvertex_addEdge ( &subver[0x02], (G3DEDGE*)&subedg[0x01] );
    g3dsubvertex_addEdge ( &subver[0x02], (G3DEDGE*)&subedg[0x02] );
    g3dsubvertex_addEdge ( &subver[0x03], (G3DEDGE*)&subedg[0x02] );
    g3dsubvertex_addEdge ( &subver[0x03], (G3DEDGE*)&subedg[0x03] );

    g3dsubdivisionV3_subdivide ( sdv, NULL, (G3DFACE*)&subfac, NULL,
                                            NULL,
                                            NULL,
                                            NULL,
                                            NULL,
                                            NULL,
                                            NULL,
                                            NULL,
                                            NULL,
                                            subindexes->qua,
                                            NULL,
                                            subdiv_level,
                                            SUBDIVISIONINDEX,
                                            0x00 );
}

/******************************************************************************/
void g3dsubindex_buildFromTriangle ( G3DSUBINDEX *subindexes, 
                                     uint32_t subdiv_level ) {
    G3DSYSINFO *sif = g3dsysinfo_get ( );
    /*** Get the temporary subdivision arrays for CPU #0 ***/
    G3DSUBDIVISION *sdv = g3dsysinfo_getSubdivision ( sif, 0x00 );
    uint32_t nbfac = pow ( 0x04, subdiv_level );
    /*** we use the G3DSUB version so we don't have to free memory ***/
    G3DSUBVERTEX subver[0x03];
    G3DSUBEDGE   subedg[0x03];
    G3DSUBFACE   subfac;

    memset ( &subver, 0x00, sizeof ( G3DSUBVERTEX ) * 0x03 );
    memset ( &subedg, 0x00, sizeof ( G3DSUBEDGE   ) * 0x03 );
    memset ( &subfac, 0x00, sizeof ( G3DSUBFACE   ) );

    subindexes->tri = (uint32_t(*)[4]) calloc ( nbfac, sizeof ( uint32_t ) * 0x04 );

    subedg[0x00].edg.ver[0x00] = (G3DVERTEX*)&subver[0x00];
    subedg[0x00].edg.ver[0x01] = (G3DVERTEX*)&subver[0x01];
    subedg[0x01].edg.ver[0x00] = (G3DVERTEX*)&subver[0x01];
    subedg[0x01].edg.ver[0x01] = (G3DVERTEX*)&subver[0x02];
    subedg[0x02].edg.ver[0x00] = (G3DVERTEX*)&subver[0x02];
    subedg[0x02].edg.ver[0x01] = (G3DVERTEX*)&subver[0x00];

    subfac.fac.ver[0x00] = (G3DVERTEX*)&subver[0x00];
    subfac.fac.ver[0x01] = (G3DVERTEX*)&subver[0x01];
    subfac.fac.ver[0x02] = (G3DVERTEX*)&subver[0x02];

    subfac.fac.edg[0x00] = (G3DEDGE*)&subedg[0x00];
    subfac.fac.edg[0x01] = (G3DEDGE*)&subedg[0x01];
    subfac.fac.edg[0x02] = (G3DEDGE*)&subedg[0x02];

    subfac.fac.nbver     = 0x03;

    g3dsubvertex_addFace ( &subver[0x00], (G3DFACE*)&subfac );
    g3dsubvertex_addFace ( &subver[0x01], (G3DFACE*)&subfac );
    g3dsubvertex_addFace ( &subver[0x02], (G3DFACE*)&subfac );

    g3dsubedge_addFace ( &subedg[0x00], (G3DFACE*)&subfac );
    g3dsubedge_addFace ( &subedg[0x01], (G3DFACE*)&subfac );
    g3dsubedge_addFace ( &subedg[0x02], (G3DFACE*)&subfac );

    g3dsubvertex_addEdge ( &subver[0x00], (G3DEDGE*)&subedg[0x02] );
    g3dsubvertex_addEdge ( &subver[0x00], (G3DEDGE*)&subedg[0x00] );
    g3dsubvertex_addEdge ( &subver[0x01], (G3DEDGE*)&subedg[0x00] );
    g3dsubvertex_addEdge ( &subver[0x01], (G3DEDGE*)&subedg[0x01] );
    g3dsubvertex_addEdge ( &subver[0x02], (G3DEDGE*)&subedg[0x01] );
    g3dsubvertex_addEdge ( &subver[0x02], (G3DEDGE*)&subedg[0x02] );

    g3dsubdivisionV3_subdivide ( sdv, NULL, (G3DFACE*)&subfac, NULL,
                                            NULL,
                                            NULL,
                                            NULL,
                                            NULL,
                                            NULL,
                                            NULL,
                                            NULL,
                                            NULL,
                                            NULL,
                                            subindexes->tri,
                                            subdiv_level,
                                            SUBDIVISIONINDEX,
                                            0x00 );
}

/******************************************************************************/
uint32_t *g3dsubindex_get ( uint32_t nbver, uint32_t subdiv_level ) {
    /*** maximum subdiv level is 0x10. No one will go this deep anyway :) ***/
    static G3DSUBINDEX *subindexes[0x10];

    if ( subdiv_level >= 0x10 ) {
        fprintf ( stderr, "g3dsubindex_get: requested subdiv level too high!\n" );

        return NULL;
    }

    if ( subindexes[subdiv_level] == NULL ) {
        subindexes[subdiv_level] = ( G3DSUBINDEX * ) calloc ( 0x01, sizeof ( G3DSUBINDEX ) );

        if ( subindexes[subdiv_level] == NULL ) {
            fprintf ( stderr, "g3dsubindex_get: calloc failed\n" );

            return NULL;
        }

        g3dsubindex_buildFromQuad     ( subindexes[subdiv_level], subdiv_level );
        g3dsubindex_buildFromTriangle ( subindexes[subdiv_level], subdiv_level );
    }

    if ( nbver == 0x04 ) return (uint32_t*) subindexes[subdiv_level]->qua;
    if ( nbver == 0x03 ) return (uint32_t*) subindexes[subdiv_level]->tri;

    return NULL;
}
