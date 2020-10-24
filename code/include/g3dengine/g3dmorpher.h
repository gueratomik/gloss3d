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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/

/**
 * @file
 */

/******************************************************************************/
#ifndef _G3DMORPHER_H_
#define _G3DMORPHER_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
typedef struct _G3DMORPHERVERTEXPOSE {
    G3DVECTOR pos;
    G3DVECTOR nor;
    uint32_t  enabled;
} G3DMORPHERVERTEXPOSE;

/******************************************************************************/
typedef struct _G3DMORPHERMESHPOSE {
    char                 *name;
    G3DMORPHERVERTEXPOSE *vpose;
} G3DMORPHERMESHPOSE;

/**
 * @struct G3DMORPHER
 * @brief A modifier to revolve a spline
 */
typedef struct _G3DMORPHER {
    G3DMODIFIER         mod;  /*** G3DMORPHER inherits G3DMODIFIER        ***/
    uint32_t            extensionName;
    LIST               *lver;
    uint32_t            verID;
    LIST               *lmpose;
    G3DMORPHERMESHPOSE *selmpose;
} G3DMORPHER;

G3DMORPHERMESHPOSE *g3dmorpherpose_new ( uint32_t nbver );
void g3dmorpher_addVertex ( G3DMORPHER *mpr,
                            G3DVERTEX  *ver );
void g3dmorpher_selectMeshPoseByRank ( G3DMORPHER *mpr,
                                       uint32_t    rank );
G3DMORPHERVERTEXPOSE *g3dmorpher_getVertexPose ( G3DMORPHER *mpr,
                                                 G3DVERTEX  *ver );
G3DMORPHER *g3dmorpher_new ( uint32_t id, 
                             char    *name, 
                             uint64_t engine_flags );

#ifdef __cplusplus
}
#endif

#endif
