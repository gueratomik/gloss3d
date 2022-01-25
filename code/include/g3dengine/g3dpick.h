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
#ifndef _G3DPICK_H_
#define _G3DPICK_H_

#ifdef __cplusplus
extern "C" {
#endif

#define G3DPICK_ACTIONFUNC(f) ((uint32_t(*)(uint64_t, \
                                            void *))f)

/******************************************************************************/
/**
 * draw a Point for picking.
 * @param x X coordinates.
 * @param y Y coordinates.
 * @param z Z coordinates.
 * @return Returns 1 if the point was drawn, 0 otherwise.
 */
uint32_t g3dpick_drawPoint ( float x, float y, float z );

/**
 * draw a Line for picking.
 * @param x1 X coordinates of start point.
 * @param y1 Y coordinates of start point.
 * @param z1 Z coordinates of start point.
 * @param x2 X coordinates of end point.
 * @param y2 Y coordinates of end point.
 * @param z2 Z coordinates of end point.
 * @return Returns 1 if theline was drawn, 0 otherwise.
 */
uint32_t g3dpick_drawLine ( float x1, float y1, float z1,
                            float x2, float y2, float z2 );

/**
 * draw a Face for picking.
 * @param nbver Number of vertices in the face.
 * @param x1 X coordinates for point 1.
 * @param y1 Y coordinates for point 1.
 * @param z1 Z coordinates for point 1.
 * @param x2 X coordinates for point 2.
 * @param y2 Y coordinates for point 2.
 * @param z2 Z coordinates for point 2.
 * @param x3 X coordinates for point 3.
 * @param y3 Y coordinates for point 3.
 * @param z3 Z coordinates for point 3.
 * @param x4 X coordinates for point 4.
 * @param y4 Y coordinates for point 4.
 * @param z4 Z coordinates for point 4.
 * @return Returns 1 if the face was drawn, 0 otherwise.
 */
uint32_t g3dpick_drawFace ( uint32_t nbver,
                            float x1, float y1, float z1,
                            float x2, float y2, float z2,
                            float x3, float y3, float z3,
                            float x4, float y4, float z4 );

/**
 * Clear buffers.
 */
void g3dpick_clear ( );

/**
 * Set the modelview matrix.
 * @param MVX The modelview matrix.
 */
void g3dpick_setModelviewMatrix  ( double *MVX );

/**
 * Set the projection matrix.
 * @param MVX The projection matrix.
 */
void g3dpick_setProjectionMatrix ( double *PJX );

/**
 * Set the viewport matrix.
 * @param MVX The viewport matrix.
 */
void g3dpick_setViewportMatrix   ( int    *VPX );

/**
 * Set the area matrix.
 * @param AMX The area matrix.
 */
void g3dpick_setAreaMatrix   ( int    *AMX );

/**
 * Set the callback when primitive is drawn.
 * @param action The callback.
 * @param data User data pointer sent to the callback when called.
 */
void g3dpick_setAction ( uint32_t (*action)(uint64_t, void *), void *data );

/**
 * Set the name parameter sent to the callback.
 * @param name The name.
 */
void g3dpick_setName ( uint64_t name );

/**
 * Set epsilon for Z comparison.
 * @param epsilon epsilon value.
 */
void g3dpick_setEpsilon ( float epsilon );

void g3dpick_loadMatrix ( double *MVX );

void g3dpick_enableDepthTest ( );
void g3dpick_disableDepthTest ( );

void g3dpick_multModelviewMatrix ( double *MVX );

#ifdef __cplusplus
}
#endif

#endif
