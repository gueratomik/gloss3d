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
#ifndef _G3DSPLINEREVOLVER_H_
#define _G3DSPLINEREVOLVER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _G3DSPLINEREVOLVERVTABLE G3DSPLINEREVOLVERVTABLE;

/**
 * @struct G3DSPLINEREVOLVER
 * @brief A modifier to revolve a spline
 */
typedef struct _G3DSPLINEREVOLVER {
    G3DMODIFIER   mod;  /*** Bone inherits G3DOBJECT        ***/
    /* for the new topology */
    G3DSUBVERTEX *subvertexArray; /* SUB* struct have embedded topology */
    G3DSUBFACE   *subfaceArray;   /* SUB* struct have embedded topology */
    G3DSUBEDGE   *subedgeArray;   /* SUB* struct have embedded topology */
    LIST         *subfaceList;    /* An array simulating a list */
    LIST         *subedgeList;    /* An array simulating a list */
    LIST         *subvertexList;  /* An array simulating a list */
    uint32_t      subvertexCount;
    uint32_t      subedgeCount;
    uint32_t      subfaceCount;
    uint32_t      stepCount;
    uint32_t      divisionCount;
    G3DUVSET     *uvset;
} G3DSPLINEREVOLVER;

/******************************************************************************/

G3DSPLINEREVOLVER *g3dsplinerevolver_new ( uint32_t id, char *name );
void g3dsplinerevolver_init ( G3DSPLINEREVOLVER       *srv,
                              uint32_t                 type,
                              uint32_t                 id,
                              const char              *name,
                              uint32_t                 object_flags,
                              G3DSPLINEREVOLVERVTABLE *vtable );
uint32_t g3dsplinerevolver_reshape ( G3DSPLINEREVOLVER *srv,
                                     uint64_t           engine_flags );

#ifdef __cplusplus
}
#endif

#endif
