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
#ifndef _G3DSYSINFO_H_
#define _G3DSYSINFO_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct G3DSYSINFO
 * @brief A structure to store system-type info (number of CPUs, etc...).
 */
typedef struct _G3DSYSINFO {
    FT_Library       ftlib;
    uint32_t         nbcpu;
    char            *ffmpegPath;
    char            *ffplayPath;
    char            *ffprobePath;
    G3DSUBDIVISION **subdivisions; /*** one per core ***/
    LIST            *lanimatedImages;
     /* 
      * Although it doesnt make much sense, backgroundImage is located in 
      * G3DSYSINFO because we need to be able to access it for texture mapping.
      * with "background" projection. 
      */
    G3DIMAGE        *backgroundImage;
} G3DSYSINFO;

/******************************************************************************/

/**
 * Get a pointer to a G3DSUBDIVISION  structure. The structure is used for
 * per-core catmull-clark subdivisions. It contains the array used by the
 * subdivision algorithm. The structure must not be freed.
 * @param a pointer to the G3DSYSINFO structure.
 * @param the ID of the CPU for which holds the structure (one per CPU).
 * @return the allocated G3DSUBDIVISION. Must not be freed.
 */
G3DSUBDIVISION *g3dsysinfo_getSubdivision ( G3DSYSINFO *sif, 
                                            uint32_t    cpuID );

/**
 * Get a pointer to the G3DSYSINFO structure. The structure is allocated 
 * at the first call of this function and must not be freed.
 * @return a pointer to the G3DSYSINFO structure that MUST NOT be freed.
 */
G3DSYSINFO *g3dsysinfo_get ( );

/**
 * Animated animated textures.
 * @param a pointer to the G3DSYSINFO structure.
 * @param the start frame of the 3D animation.
 * @param the current frame of the 3D animation.
 * @param the last frame of the 3D animation.
 * @param the frame rate of the 3D animation.
 * @param 3D engine flags.
 */
void g3dsysinfo_processAnimatedImages ( G3DSYSINFO *info, 
                                        float       sceneStartFrame,
                                        float       sceneCurrentFrame,
                                        float       sceneEndFrame,
                                        float       sceneFramesPerSecond,
                                        uint32_t    engine_flags );

/**
 * Reset the g3dsysinfo structure to its default values.
 * @param a pointer to the G3DSYSINFO structure.
 */
void g3dsysinfo_reset ( G3DSYSINFO *sysinfo );

#ifdef __cplusplus
}
#endif

#endif
