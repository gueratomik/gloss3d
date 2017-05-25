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
#ifndef _G3DLIGHT_H_
#define _G3DLIGHT_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct G3DLIGHT
 * @brief A structure to store a 3D Light.
 */
typedef struct _G3DLIGHT {
    G3DOBJECT obj;            /* Light inherits G3DOBJECT */
    float     intensity;      /* Light intensity          */
    uint32_t  lid;            /* OpenGL Light ID          */
    G3DRGBA   shadowColor;    /* R-G-B-A                  */
    G3DRGBA   diffuseColor;
    G3DRGBA   specularColor;
    G3DRGBA   ambientColor;
} G3DLIGHT;

/******************************************************************************/
/**
 * create a new light.
 * @param id the object's ID.
 * @param name the object's name.
 * @return the allocated G3DLIGHT.
 */
G3DLIGHT *g3dlight_new ( uint32_t id, 
                         char    *name );

/**
 * Turn on a light in the OpenGL view. It basically calls glEnable(lightID);
 * @param lig a pointer to a light.
 */
void g3dlight_init ( G3DLIGHT *lig );

/**
 * Turn a light off in the OpenGL view.
 * @param lig a pointer to a light.
 */
void g3dlight_zero ( G3DLIGHT *lig );

/**
 * Free a previously allocated light.
 * @param lig a pointer to the previously allocated light.
 */
void g3dlight_free ( G3DLIGHT *lig );

/**
 * Draw a light.
 * @param lig a pointer to a light.
 * @param cam a pointer to the view's camera.
 * @param engine_flags the 3D engine flags.
 * @return the allocated G3DLIGHT.
 */
uint32_t g3dlight_draw ( G3DLIGHT  *lig, 
                         G3DCAMERA *cam,
                         uint32_t   engine_flags );

/**
 * Create a new light from an existing one.
 * @param id the object's ID.
 * @param name the object's name.
 * @param engine_flags the 3D engine flags.
 * @return the cloned G3DLIGHT.
 */
G3DLIGHT *g3dlight_copy ( G3DLIGHT      *lig, 
                          uint32_t       id,
                          unsigned char *name,
                          uint32_t       engine_flags );

#ifdef __cplusplus
}
#endif

#endif
