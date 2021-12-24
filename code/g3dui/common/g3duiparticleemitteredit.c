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
#include <g3dui.h>

/******************************************************************************/
void common_g3duiparticleemitteredit_initialVarTranspCbk ( G3DUI              *gui,
                                                           G3DPARTICLEEMITTER *pem,
                                                           float               initialVarTransp ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialVarTransparency = initialVarTransp;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialVarRotXCbk ( G3DUI              *gui,
                                                         G3DPARTICLEEMITTER *pem,
                                                         float               initialVarRotX ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialVarRotation.x = initialVarRotX;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialVarRotYCbk ( G3DUI              *gui,
                                                         G3DPARTICLEEMITTER *pem,
                                                         float               initialVarRotY ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialVarRotation.y = initialVarRotY;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialVarRotZCbk ( G3DUI              *gui,
                                                         G3DPARTICLEEMITTER *pem,
                                                         float               initialVarRotZ ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialVarRotation.z = initialVarRotZ;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialVarScaXCbk ( G3DUI              *gui,
                                                         G3DPARTICLEEMITTER *pem,
                                                         float               initialVarScaX ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialVarScaling.x = initialVarScaX;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialVarScaYCbk ( G3DUI              *gui,
                                                         G3DPARTICLEEMITTER *pem,
                                                         float               initialVarScaY ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialVarScaling.y = initialVarScaY;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialVarScaZCbk ( G3DUI              *gui,
                                                         G3DPARTICLEEMITTER *pem,
                                                         float               initialVarScaZ ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialVarScaling.z = initialVarScaZ;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialVarSpeedXCbk ( G3DUI              *gui,
                                                           G3DPARTICLEEMITTER *pem,
                                                           float               initialVarSpeedX ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialVarSpeed.x = initialVarSpeedX;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialVarSpeedYCbk ( G3DUI              *gui,
                                                           G3DPARTICLEEMITTER *pem,
                                                           float               initialVarSpeedY ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialVarSpeed.y = initialVarSpeedY;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialVarSpeedZCbk ( G3DUI              *gui,
                                                           G3DPARTICLEEMITTER *pem,
                                                           float               initialVarSpeedZ ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialVarSpeed.z = initialVarSpeedZ;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialVarAccelXCbk ( G3DUI              *gui,
                                                           G3DPARTICLEEMITTER *pem,
                                                           float               initialVarAccelX ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialVarAccel.x = initialVarAccelX;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialVarAccelYCbk ( G3DUI              *gui,
                                                           G3DPARTICLEEMITTER *pem,
                                                           float               initialVarAccelY ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialVarAccel.y = initialVarAccelY;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialVarAccelZCbk ( G3DUI              *gui,
                                                           G3DPARTICLEEMITTER *pem,
                                                           float               initialVarAccelZ ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialVarAccel.z = initialVarAccelZ;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_finalTranspCbk ( G3DUI              *gui,
                                                      G3DPARTICLEEMITTER *pem,
                                                      float               finalTransp ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->finalTransparency = finalTransp;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_finalRotXCbk ( G3DUI              *gui,
                                                    G3DPARTICLEEMITTER *pem,
                                                    float               finalRotX ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->finalRotation.x = finalRotX;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_finalRotYCbk ( G3DUI              *gui,
                                                    G3DPARTICLEEMITTER *pem,
                                                    float               finalRotY ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->finalRotation.y = finalRotY;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_finalRotZCbk ( G3DUI              *gui,
                                                    G3DPARTICLEEMITTER *pem,
                                                    float               finalRotZ ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->finalRotation.z = finalRotZ;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_finalScaXCbk ( G3DUI              *gui,
                                                      G3DPARTICLEEMITTER *pem,
                                                      float               finalScaX ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->finalScaling.x = finalScaX;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_finalScaYCbk ( G3DUI              *gui,
                                                    G3DPARTICLEEMITTER *pem,
                                                    float               finalScaY ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->finalScaling.y = finalScaY;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_finalScaZCbk ( G3DUI              *gui,
                                                      G3DPARTICLEEMITTER *pem,
                                                      float               finalScaZ ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->finalScaling.z = finalScaZ;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_finalSpeedXCbk ( G3DUI              *gui,
                                                      G3DPARTICLEEMITTER *pem,
                                                      float               finalSpeedX ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->finalSpeed.x = finalSpeedX;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_finalSpeedYCbk ( G3DUI              *gui,
                                                      G3DPARTICLEEMITTER *pem,
                                                      float               finalSpeedY ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->finalSpeed.y = finalSpeedY;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_finalSpeedZCbk ( G3DUI              *gui,
                                                      G3DPARTICLEEMITTER *pem,
                                                      float               finalSpeedZ ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->finalSpeed.z = finalSpeedZ;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_finalAccelXCbk ( G3DUI              *gui,
                                                      G3DPARTICLEEMITTER *pem,
                                                      float               finalAccelX ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->finalAccel.x = finalAccelX;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_finalAccelYCbk ( G3DUI              *gui,
                                                        G3DPARTICLEEMITTER *pem,
                                                        float               finalAccelY ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->finalAccel.y = finalAccelY;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_finalAccelZCbk ( G3DUI              *gui,
                                                      G3DPARTICLEEMITTER *pem,
                                                      float               finalAccelZ ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->finalAccel.z = finalAccelZ;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialTranspCbk ( G3DUI              *gui,
                                                        G3DPARTICLEEMITTER *pem,
                                                        float               initialTransp ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialTransparency = initialTransp;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialRotXCbk ( G3DUI              *gui,
                                                      G3DPARTICLEEMITTER *pem,
                                                      float               initialRotX ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialRotation.x = initialRotX;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialRotYCbk ( G3DUI              *gui,
                                                      G3DPARTICLEEMITTER *pem,
                                                      float               initialRotY ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialRotation.y = initialRotY;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialRotZCbk ( G3DUI              *gui,
                                                      G3DPARTICLEEMITTER *pem,
                                                      float               initialRotZ ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialRotation.z = initialRotZ;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialScaXCbk ( G3DUI              *gui,
                                                      G3DPARTICLEEMITTER *pem,
                                                      float               initialScaX ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialScaling.x = initialScaX;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialScaYCbk ( G3DUI              *gui,
                                                      G3DPARTICLEEMITTER *pem,
                                                      float               initialScaY ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialScaling.y = initialScaY;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialScaZCbk ( G3DUI              *gui,
                                                      G3DPARTICLEEMITTER *pem,
                                                      float               initialScaZ ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialScaling.z = initialScaZ;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialSpeedXCbk ( G3DUI              *gui,
                                                        G3DPARTICLEEMITTER *pem,
                                                        float               initialSpeedX ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialSpeed.x = initialSpeedX;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialSpeedYCbk ( G3DUI              *gui,
                                                        G3DPARTICLEEMITTER *pem,
                                                        float               initialSpeedY ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialSpeed.y = initialSpeedY;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialSpeedZCbk ( G3DUI              *gui,
                                                        G3DPARTICLEEMITTER *pem,
                                                        float               initialSpeedZ ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialSpeed.z = initialSpeedZ;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialAccelXCbk ( G3DUI              *gui,
                                                        G3DPARTICLEEMITTER *pem,
                                                        float               initialAccelX ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialAccel.x = initialAccelX;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialAccelYCbk ( G3DUI              *gui,
                                                        G3DPARTICLEEMITTER *pem,
                                                        float               initialAccelY ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialAccel.y = initialAccelY;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_initialAccelZCbk ( G3DUI              *gui,
                                                        G3DPARTICLEEMITTER *pem,
                                                        float               initialAccelZ ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->initialAccel.z = initialAccelZ;
}










/******************************************************************************/
void common_g3duiparticleemitteredit_startAtFrameCbk ( G3DUI              *gui,
                                                       G3DPARTICLEEMITTER *pem,
                                                       uint32_t            startAtFrame ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->startAtFrame = startAtFrame;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_endAtFrameCbk ( G3DUI              *gui,
                                                     G3DPARTICLEEMITTER *pem,
                                                     uint32_t            endAtFrame ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->endAtFrame = endAtFrame;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_radiusCbk ( G3DUI              *gui,
                                                 G3DPARTICLEEMITTER *pem,
                                                 float               radius ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->radius = radius;

    g3dparticleemitter_reset ( pem );
}

/******************************************************************************/
void common_g3duiparticleemitteredit_lifetimeCbk ( G3DUI              *gui,
                                                   G3DPARTICLEEMITTER *pem,
                                                   uint32_t            lifetime ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->particleLifetime = lifetime;

    g3dparticleemitter_reset ( pem );
}

/******************************************************************************/
void common_g3duiparticleemitteredit_ppfCbk ( G3DUI              *gui,
                                              G3DPARTICLEEMITTER *pem,
                                              float               ppf ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->particlesPerFrame = ppf;

    g3dparticleemitter_reset ( pem );
}

/******************************************************************************/
void common_g3duiparticleemitteredit_maxPreviewsCbk ( G3DUI              *gui,
                                                      G3DPARTICLEEMITTER *pem,
                                                      uint32_t            maxPreviews ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    pem->maxPreviewsPerFrame = maxPreviews;
}

/******************************************************************************/
void common_g3duiparticleemitteredit_displayPartCbk ( G3DUI              *gui, 
                                                      G3DPARTICLEEMITTER *pem ) {
    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( pem->obj.flags & DISPLAYPARTICLES ) {
        pem->obj.flags &= (~DISPLAYPARTICLES);
    } else {
        pem->obj.flags |= DISPLAYPARTICLES;
    }
}
