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
#ifndef _G3DTAG_H_
#define _G3DTAG_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct G3DTAG
 * @brief .
 */
typedef struct _G3DTAG {
    uint32_t    id;
    char       *name;
    void      (*free)          ( struct _G3DTAG * );
    uint32_t  (*preTransform)  ( struct _G3DTAG *, 
                                         G3DOBJECT *, 
                                         uint64_t );
    uint32_t  (*postTransform) ( struct _G3DTAG *, 
                                         G3DOBJECT *,
                                         uint64_t );

    uint32_t  (*preAnim) ( struct _G3DTAG *,
                                   G3DOBJECT *, 
                                   float,
                                   uint64_t );
    uint32_t  (*postAnim)( struct _G3DTAG *, 
                                   G3DOBJECT *, 
                                   float,
                                   uint64_t );

    uint32_t (*preDraw)  ( struct _G3DTAG *, 
                                   G3DOBJECT *, 
                                   G3DCAMERA *, 
                                   uint64_t );
    uint32_t (*postDraw) ( struct _G3DTAG *, 
                                   G3DOBJECT *, 
                                   G3DCAMERA *, 
                                   uint64_t );
} G3DTAG;

/******************************************************************************/
typedef struct _G3DVIBRATORTAG {
    G3DTAG tag;
    G3DVECTOR posAmp; /* Amplitude */
    G3DVECTOR rotAmp; /* Amplitude */
    G3DVECTOR scaAmp; /* Amplitude */
    uint32_t  freq;
    G3DVECTOR pos;
    G3DVECTOR rot;
    G3DVECTOR sca;
} G3DVIBRATORTAG;

/******************************************************************************/
typedef struct _G3DTRACKERTAG {
    G3DTAG tag;
} G3DTRACKERTAG;

/******************************************************************************/
typedef struct _G3DEMITTERTAG {
    G3DTAG tag;
} G3DEMITTERTAG;

/******************************************************************************/

G3DTAG *g3dvibratortag_new ( uint32_t id );


#ifdef __cplusplus
}
#endif

#endif
