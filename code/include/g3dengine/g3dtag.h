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

#define G3DTAGVIBRATORTYPE 0x01
#define G3DTAGTRACKERTYPE  0x02
#define G3DTAGTARGETTYPE   0x03

#define TAGSELECTED ( 1 << 0 )
#define TAGHIDDEN   ( 1 << 1 )


#define TAG_FREEFUNC(f)       ((void(*)(G3DTAG*))f)
#define TAG_PREANIMFUNC(f)    ((uint32_t(*)(G3DTAG*, \
                                            G3DOBJECT *, \
                                            float, \
                                            uint64_t ))f)
#define TAG_POSTANIMFUNC(f)   ((uint32_t(*)(G3DTAG*, \
                                            G3DOBJECT *, \
                                            float, \
                                            uint64_t ))f)

#define TAG_ADDFUNC(f)        ((uint32_t(*)(G3DTAG *, \
                                            G3DOBJECT *, \
                                            uint64_t ))f)
#define TAG_REMOVEFUNC(f)     ((uint32_t(*)(G3DTAG *, \
                                            G3DOBJECT *, \
                                            uint64_t ))f)
#define TAG_TRANSFORMFUNC(f)  ((uint32_t(*)(G3DTAG *, \
                                            G3DOBJECT *, \
                                            uint64_t ))f)

/**
 * @struct G3DTAG
 * @brief .
 */
typedef struct _G3DTAG {
    uint32_t    id;
    uint32_t    flags;
    uint32_t    type;
    char       *name;
    uint32_t  (*add)      ( struct _G3DTAG *, 
                                    G3DOBJECT *, 
                                    uint64_t );
    uint32_t  (*remove)   ( struct _G3DTAG *, 
                                    G3DOBJECT *, 
                                    uint64_t );
    void      (*free)     ( struct _G3DTAG * );
    uint32_t  (*transform)( struct _G3DTAG *, 
                                    G3DOBJECT *, 
                                    uint64_t );
    uint32_t  (*preAnim)  ( struct _G3DTAG *,
                                    G3DOBJECT *, 
                                    float,
                                    uint64_t );
    uint32_t  (*postAnim) ( struct _G3DTAG *, 
                                    G3DOBJECT *, 
                                    float,
                                    uint64_t );

    uint32_t (*preDraw)   ( struct _G3DTAG *, 
                                    G3DOBJECT *, 
                                    G3DCAMERA *, 
                                    uint64_t );
    uint32_t (*postDraw)  ( struct _G3DTAG *, 
                                    G3DOBJECT *, 
                                    G3DCAMERA *, 
                                    uint64_t );
} G3DTAG;

/******************************************************************************/
typedef struct _G3DVIBRATORTAG {
    G3DTAG tag;
    G3DVECTOR3F posAmp; /* Amplitude */
    G3DVECTOR3F rotAmp; /* Amplitude */
    G3DVECTOR3F scaAmp; /* Amplitude */
    uint32_t    freq;
    G3DVECTOR3F pos;
    G3DVECTOR3F rot;
    G3DVECTOR3F sca;
} G3DVIBRATORTAG;

/******************************************************************************/
#define TARGET_XAXIS 0x00
#define TARGET_YAXIS 0x01
#define TARGET_ZAXIS 0x02

typedef struct _G3DTRACKERTAG {
    G3DTAG       tag;
    G3DSCENE    *sce;
    G3DOBJECT   *tracker;
    G3DOBJECT   *target;
    uint32_t     targetTagID;
    uint32_t     orientation;
} G3DTRACKERTAG;

/******************************************************************************/
typedef struct _G3DTARGETTAG {
    G3DTAG       tag;
    G3DSCENE    *sce;
    G3DOBJECT   *tracker;
    uint32_t     trackerTagID;
} G3DTARGETTAG;

/******************************************************************************/
typedef struct _G3DEMITTERTAG {
    G3DTAG tag;
} G3DEMITTERTAG;

/******************************************************************************/
void g3dtag_free ( G3DTAG *tag );

/******************************************************************************/
G3DTAG *g3dvibratortag_new ( uint32_t id );
G3DTAG *g3dtargettag_new ( uint32_t   id, 
                           G3DSCENE  *sce,
                           G3DOBJECT *tracker );
G3DTAG *g3dtrackertag_new ( uint32_t  id,
                            G3DSCENE  *sce );
void g3dtrackertag_setOrientation ( G3DTRACKERTAG *ttag,
                                    uint32_t       orientation );
void g3dtrackertag_setTarget ( G3DTRACKERTAG *ttag,
                               G3DOBJECT   *tracker,
                               G3DOBJECT   *target,
                               uint64_t     engine_flags );

#ifdef __cplusplus
}
#endif

#endif
