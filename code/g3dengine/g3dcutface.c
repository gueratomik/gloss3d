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

/******************************************************************************/
void g3dcutface_free ( G3DCUTFACE *cut ) {
    free ( cut );
}

/******************************************************************************/
G3DCUTFACE *g3dcutface_new ( G3DFACE *fac, G3DCUTEDGE *ced[0x04] ) {
    G3DCUTFACE *cut = ( G3DCUTFACE * ) calloc ( 0x01, sizeof ( G3DCUTFACE ) );
    int i;

    if ( cut == NULL ) {
        fprintf ( stderr, "g3dcutface_new: Memory allocation failed\n" );

        return NULL;
    } 

    cut->fac = fac;

    for ( i = 0x00; i < 0x04; i++ ) {
        cut->ced[i] = ced[i];
    }

    return cut;
}

/******************************************************************************/
uint32_t g3dcutface_getCutEdgeIndexes ( G3DCUTFACE *cut, uint32_t *index,
                                                         uint32_t size  ) {
    uint32_t i, n = 0x00;

    for ( i = 0x00; i < cut->fac->vertexCount; i++ ) {
        if ( cut->ced[i] && n < size ) {
            index[n++] = i;
        }
    }

    /*** returns the number of cut edges ***/
    return n;
}

/******************************************************************************/
uint32_t g3dcutface_divideQuad ( G3DCUTFACE *cut, G3DFACE **kid ) {
    G3DFACE *fac = cut->fac;
    uint32_t index[0x02], nbcut;
    uint32_t nbkid = 0x00;

    nbcut = g3dcutface_getCutEdgeIndexes ( cut, index, 0x02 );

    if ( nbcut == 0x01 ) {
        uint32_t AA = index[0x00],
                 bA = ( AA + 0x03 ) % 0x04,
                 cA = ( AA + 0x02 ) % 0x04,
                 aA = ( AA + 0x01 ) % 0x04;

        kid[nbkid++] = g3dtriangle_new ( cut->ced[AA]->ver,
                                         fac->ver[aA],
                                         fac->ver[cA] );
        kid[nbkid++] = g3dtriangle_new ( fac->ver[cA],
                                         fac->ver[bA],
                                         cut->ced[AA]->ver );
        kid[nbkid++] = g3dtriangle_new ( fac->ver[bA],
                                         fac->ver[AA],
                                         cut->ced[AA]->ver );
    } 

    if ( nbcut == 0x02 ) {
        uint32_t AA = index[0x00],
                 bA = ( AA + 0x03 ) % 0x04,
                 cA = ( AA + 0x02 ) % 0x04,
                 aA = ( AA + 0x01 ) % 0x04,
                 BB = index[0x01];
        uint32_t diff = ( BB - AA );

        if ( diff == 0x01 ) {
            kid[nbkid++] = g3dtriangle_new ( cut->ced[AA]->ver,
                                             fac->ver[aA],
                                             cut->ced[BB]->ver );
            kid[nbkid++] = g3dtriangle_new ( cut->ced[BB]->ver,
                                             fac->ver[cA],
                                             fac->ver[bA] );
            kid[nbkid++] = g3dquad_new ( fac->ver[bA],
                                         fac->ver[AA],
                                         cut->ced[AA]->ver,
                                         cut->ced[BB]->ver );
        } 

        if ( diff == 0x02 ) {
            kid[nbkid++] = g3dquad_new ( cut->ced[AA]->ver,
                                         fac->ver[aA],
                                         fac->ver[cA],
                                         cut->ced[BB]->ver );
            kid[nbkid++] = g3dquad_new ( cut->ced[BB]->ver,
                                         fac->ver[bA],
                                         fac->ver[AA],
                                         cut->ced[AA]->ver );
        }

        if ( diff == 0x03 ) {
            kid[nbkid++] = g3dtriangle_new ( cut->ced[AA]->ver,
                                             fac->ver[aA],
                                             fac->ver[cA] );
            kid[nbkid++] = g3dquad_new ( fac->ver[cA],
                                         fac->ver[bA],
                                         cut->ced[BB]->ver,
                                         cut->ced[AA]->ver );
            kid[nbkid++] = g3dtriangle_new ( cut->ced[AA]->ver,
                                             cut->ced[BB]->ver,
                                             fac->ver[AA] );
        }
    }

    return nbkid;
}

/******************************************************************************/
uint32_t g3dcutface_divideTriangle ( G3DCUTFACE *cut, G3DFACE **kid ) {
    G3DFACE *fac = cut->fac;
    uint32_t index[0x02], nbcut;
    uint32_t nbkid = 0x00;

    nbcut = g3dcutface_getCutEdgeIndexes ( cut, index, 0x02 );

    if ( nbcut == 0x01 ) {
        uint32_t AA = index[0x00],
                 bA = ( AA + 0x02 ) % 0x03,
                 aA = ( AA + 0x01 ) % 0x03;

        kid[nbkid++] = g3dtriangle_new ( cut->ced[AA]->ver,
                                         fac->ver[aA],
                                         fac->ver[bA] );

        kid[nbkid++] = g3dtriangle_new ( fac->ver[bA],
                                         fac->ver[AA],
                                         cut->ced[AA]->ver );
    }

    if ( nbcut == 0x02 ) {
        uint32_t AA = index[0x00],
                 bA = ( AA + 0x02 ) % 0x03,
                 aA = ( AA + 0x01 ) % 0x03,
                 BB = index[0x01];
        uint32_t diff = ( BB - AA );

        if (  diff == 0x01 ) {
            kid[nbkid++] = g3dtriangle_new ( cut->ced[AA]->ver,
                                             fac->ver[aA],
                                             cut->ced[BB]->ver );

            kid[nbkid++] = g3dquad_new ( cut->ced[BB]->ver,
                                         fac->ver[bA],
                                         fac->ver[AA],
                                         cut->ced[AA]->ver );

            
        } 

        if (  diff == 0x02 ) {
            kid[nbkid++] = g3dquad_new ( cut->ced[AA]->ver,
                                         fac->ver[aA],
                                         fac->ver[bA],
                                         cut->ced[BB]->ver );

            kid[nbkid++] = g3dtriangle_new ( cut->ced[BB]->ver,
                                             fac->ver[AA],
                                             cut->ced[AA]->ver );
        }
    }

    return nbkid;
}

/******************************************************************************/
static void g3dcutface_dividUVSets ( G3DCUTFACE *cut, 
                                     G3DFACE   **kid,
                                     uint32_t    nbkid ) {
    LIST *ltmpuvs = cut->fac->uvsetList;

    while ( ltmpuvs ) {
        G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;
        uint32_t i;

        /*** Note: there are at most 3 kids ***/
        for ( i = 0x00; i < nbkid; i++ ) {
            G3DUVSET *kiduvs = g3duvset_new ( uvs->map );
            G3DFACE *kidfac = kid[i];
            uint32_t j;

            g3dface_addUVSet ( kidfac, kiduvs );

            for ( j = 0x00; j < kidfac->vertexCount; j++ ) {
                uint32_t n = ( j + 0x01 ) % kidfac->vertexCount;
                uint32_t k;

                for ( k = 0x00; k < cut->fac->vertexCount; k++ ) {
                    uint32_t l = ( k + 0x01 ) % cut->fac->vertexCount;

                    if ( ( kidfac->ver[j] == cut->fac->ver[k] ) ) {
                        kiduvs->veruv[j].u = uvs->veruv[k].u;
                        kiduvs->veruv[j].v = uvs->veruv[k].v;
                    }

                    if ( cut->ced[k] ) {
                        if ( ( kidfac->ver[n] == cut->ced[k]->ver ) ) {
                            G3DEDGE *edg = cut->ced[k]->edg;
                            float difu = uvs->veruv[l].u - uvs->veruv[k].u,
                                  difv = uvs->veruv[l].v - uvs->veruv[k].v;
                            /*** we must check the "clockwiseness" and then ***/
                            /*** adjust the ratio ***/
                            float ratio = ( cut->fac->ver[k] == 
                                            edg->ver[0x00] ) ? cut->ced[k]->ratio : 
                                                        1.0f - cut->ced[k]->ratio;

                            kiduvs->veruv[n].u = uvs->veruv[k].u + ( difu * ratio );
                            kiduvs->veruv[n].v = uvs->veruv[k].v + ( difv * ratio );
                        }
                    }
                }
            }
        }

        ltmpuvs = ltmpuvs->next;
    }
}

/******************************************************************************/
uint32_t g3dcutface_divide ( G3DCUTFACE *cut, G3DFACE **kid ) {
    G3DFACE *fac = cut->fac;
    uint32_t nbkid = 0x00;

    switch ( fac->vertexCount ) {
        case 0x03 :
            nbkid = g3dcutface_divideTriangle ( cut, kid );
        break;

        case 0x04 :
            nbkid = g3dcutface_divideQuad ( cut, kid );
        break;
    }

    g3dcutface_dividUVSets ( cut, kid, nbkid );


    return nbkid;
}
