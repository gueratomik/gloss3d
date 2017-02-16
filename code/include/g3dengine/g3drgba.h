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
 * @file g3drgba.h
 */

/******************************************************************************/
typedef struct _G3DRGBA {
    uint32_t r, g, b, a;
} G3DRGBA;

/**
 * Init rgba structure with values passed as parameters.
 * @param rgba a pointer to a G3DRGBA structure.
 * @param r Red color int value (range 0-255).
 * @param g Green color int value (range 0-255).
 * @param b Blue color int value (range 0-255).
 * @param a Alpha color int value (range 0-255).
 */
void g3drgba_init ( G3DRGBA *rgba, uint32_t r, uint32_t g, 
                                   uint32_t b, uint32_t a );

/**
 * Convert G3DCOLOR (float values) to G3DRGBA (unsigned int values).
 * Float values are multiplied by 255
 * @param a pointer to a G3DCOLOR structure.
 * @param a pointer to a G3DRGBA structure.
 */
void g3dcolor_toRGBA ( G3DCOLOR *color, G3DRGBA *rgba );

/**
 * Convert G3DRGBA (int values) to G3DCOLOR (float values).
 * Unsigned int values are divided by 255
 * @param a pointer to a G3DRGBA structure.
 * @param a pointer to a G3DCOLOR structure.
 */
void g3drgba_toColor ( G3DRGBA *rgba, G3DCOLOR *color );

/**
 * Convert G3DRGBA to a single unsigned int color.
 * @param a pointer to a G3DRGBA structure.
 * @return the color in unsigned int 8-8-8-8 format.
 */
uint32_t g3drgba_toLong ( G3DRGBA *rgba );
