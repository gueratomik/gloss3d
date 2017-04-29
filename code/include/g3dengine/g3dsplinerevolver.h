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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2013         */
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

/**
 * @struct G3DSPLINEREVOLVER
 * @brief A modifier to revolve a spline
 */
typedef struct _G3DSPLINEREVOLVER {
    G3DMODIFIER   mod;  /*** Bone inherits G3DOBJECT        ***/
    uint32_t      nbsteps;
    uint32_t      nbdivis;
    LIST         *lupdver; /* lit of vertices to update on mesh update */
} G3DSPLINEREVOLVER;

/******************************************************************************/

void g3dsplinerevolver_free ( G3DSPLINEREVOLVER *srv );
uint32_t g3dsplinerevolver_draw ( G3DSPLINEREVOLVER *srv, 
                                  G3DCAMERA         *cam, 
                                  uint32_t           engine_flags );
void g3dsplinerevolver_deactivate ( G3DSPLINEREVOLVER *srv, 
                                    uint32_t           engine_flags );
void g3dsplinerevolver_activate ( G3DSPLINEREVOLVER *srv, 
                                  uint32_t           engine_flags );
uint32_t g3dsplinerevolver_modify ( G3DSPLINEREVOLVER *srv, 
                                    uint32_t           engine_flags );
void g3dsplinerevolver_update ( G3DSPLINEREVOLVER *srv, 
                                uint32_t           engine_flags );
void g3dsplinerevolver_endUpdate ( G3DSPLINEREVOLVER *srv,
                                   uint32_t           engine_flags );
void g3dsplinerevolver_startUpdate ( G3DSPLINEREVOLVER *srv, 
                                     uint32_t           engine_flags );
G3DSPLINEREVOLVER *g3dsplinerevolver_copy ( G3DSPLINEREVOLVER *srv,
                                            uint32_t           engine_flags );
void g3dsplinerevolver_setParent ( G3DSPLINEREVOLVER *srv,
                                   G3DOBJECT         *parent,
                                   uint32_t           engine_flags );
G3DSPLINEREVOLVER *g3dsplinerevolver_new ( uint32_t id, char *name );

#ifdef __cplusplus
}
#endif

#endif
