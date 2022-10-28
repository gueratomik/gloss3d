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
#define FOR_EACH_SELECTED_PARTICLEEMITTER                 \
    G3DUI *gui = gpeedit->gui;                             \
    G3DSCENE *sce = gui->sce;                              \
    LIST *ltmpselobj = sce->lsel;                          \
                                                           \
    while ( ltmpselobj ) {                                 \
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data; \
                                                           \
        if ( sel->type == G3DPARTICLEEMITTERTYPE ) {                 \
            G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) sel;            \


#define END_FOR                                            \
        }                                                  \
                                                           \
        ltmpselobj = ltmpselobj->next;                     \
    }

/******************************************************************************/
uint64_t g3duiparticleemitteredit_gravityForceX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                  float                     g ) {

    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->gravity.x = g;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->gravity.x = g;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_gravityForceY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                  float                     g ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->gravity.y = g;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->gravity.y = g;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_gravityForceZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                  float                     g ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->gravity.z = g;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->gravity.z = g;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarTransp ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialVarTransp ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialVarTransparency = initialVarTransp;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialVarTransparency = initialVarTransp;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarRotX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialVarRotX ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialVarRotation.x = initialVarRotX;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialVarRotation.x = initialVarRotX;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarRotY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialVarRotY ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialVarRotation.y = initialVarRotY;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialVarRotation.y = initialVarRotY;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarRotZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialVarRotZ ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialVarRotation.z = initialVarRotZ;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialVarRotation.z = initialVarRotZ;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarScaX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialVarScaX ) {

    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialVarScaling.x = initialVarScaX;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialVarScaling.x = initialVarScaX;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarScaY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialVarScaY ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialVarScaling.y = initialVarScaY;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialVarScaling.y = initialVarScaY;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarScaZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialVarScaZ ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialVarScaling.z = initialVarScaZ;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialVarScaling.z = initialVarScaZ;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarSpeedX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialVarSpeedX ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialVarSpeed.x = initialVarSpeedX;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialVarSpeed.x = initialVarSpeedX;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarSpeedY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialVarSpeedY ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialVarSpeed.y = initialVarSpeedY;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialVarSpeed.y = initialVarSpeedY;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarSpeedZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialVarSpeedZ ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialVarSpeed.z = initialVarSpeedZ;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialVarSpeed.z = initialVarSpeedZ;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarAccelX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialVarAccelX ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialVarAccel.x = initialVarAccelX;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialVarAccel.x = initialVarAccelX;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarAccelY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialVarAccelY ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialVarAccel.y = initialVarAccelY;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialVarAccel.y = initialVarAccelY;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarAccelZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialVarAccelZ ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialVarAccel.z = initialVarAccelZ;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialVarAccel.z = initialVarAccelZ;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalTransp ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                float                     finalTransp ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->finalTransparency = finalTransp;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->finalTransparency = finalTransp;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalRotX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                              float                     finalRotX ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->finalRotation.x = finalRotX;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->finalRotation.x = finalRotX;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalRotY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                              float                     finalRotY ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->finalRotation.y = finalRotY;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->finalRotation.y = finalRotY;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalRotZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                              float                     finalRotZ ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->finalRotation.z = finalRotZ;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->finalRotation.z = finalRotZ;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalScaX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                 float                     finalScaX ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->finalScaling.x = finalScaX;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->finalScaling.x = finalScaX;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalScaY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                 float                     finalScaY ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->finalScaling.y = finalScaY;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->finalScaling.y = finalScaY;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalScaZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                 float                     finalScaZ ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->finalScaling.z = finalScaZ;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->finalScaling.z = finalScaZ;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalSpeedX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalSpeedX ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->finalSpeed.x = finalSpeedX;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->finalSpeed.x = finalSpeedX;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalSpeedY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalSpeedY ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->finalSpeed.y = finalSpeedY;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->finalSpeed.y = finalSpeedY;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalSpeedZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalSpeedZ ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->finalSpeed.z = finalSpeedZ;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->finalSpeed.z = finalSpeedZ;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalAccelX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalAccelX ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->finalAccel.x = finalAccelX;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->finalAccel.x = finalAccelX;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalAccelY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalAccelY ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->finalAccel.y = finalAccelY;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->finalAccel.y = finalAccelY;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalAccelZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalAccelZ ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->finalAccel.z = finalAccelZ;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->finalAccel.z = finalAccelZ;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialTransp ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialTransp ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialTransparency = initialTransp;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialTransparency = initialTransp;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialRotX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialRotX ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialRotation.x = initialRotX;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialRotation.x = initialRotX;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialRotY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialRotY ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialRotation.y = initialRotY;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialRotation.y = initialRotY;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialRotZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialRotZ ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialRotation.z = initialRotZ;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialRotation.z = initialRotZ;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialScaX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialScaX ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialScaling.x = initialScaX;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialScaling.x = initialScaX;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialScaY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialScaY ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialScaling.y = initialScaY;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialScaling.y = initialScaY;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialScaZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialScaZ ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialScaling.z = initialScaZ;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialScaling.z = initialScaZ;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialSpeedX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialSpeedX ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialSpeed.x = initialSpeedX;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialSpeed.x = initialSpeedX;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialSpeedY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialSpeedY ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialSpeed.y = initialSpeedY;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialSpeed.y = initialSpeedY;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialSpeedZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialSpeedZ ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialSpeed.z = initialSpeedZ;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialSpeed.z = initialSpeedZ;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialAccelX ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialAccelX ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialAccel.x = initialAccelX;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialAccel.x = initialAccelX;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialAccelY ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialAccelY ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialAccel.y = initialAccelY;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialAccel.y = initialAccelY;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialAccelZ ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialAccelZ ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->initialAccel.z = initialAccelZ;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->initialAccel.z = initialAccelZ;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_startAtFrame ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                 uint32_t                  startAtFrame ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->startAtFrame = startAtFrame;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->startAtFrame = startAtFrame;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_endAtFrame ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                               uint32_t                  endAtFrame ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->endAtFrame = endAtFrame;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->endAtFrame = endAtFrame;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_radius ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                           float                     radius ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->radius = radius;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->radius = radius;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_lifetime ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                uint32_t                  lifetime ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->particleLifetime = lifetime;

        g3dparticleemitter_reset ( gpeedit->editedPEmitter );
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->particleLifetime = lifetime;

        g3dparticleemitter_reset ( pem );
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_ppf ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                           float                     ppf ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->particlesPerFrame = ppf;

        g3dparticleemitter_reset ( gpeedit->editedPEmitter );
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->particlesPerFrame = ppf;

        g3dparticleemitter_reset ( pem );
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_maxPreviews ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   uint32_t                  maxPreviews ) {
    if ( gpeedit->forKey ) {
        gpeedit->editedPEmitter->maxPreviewsPerFrame = maxPreviews;
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        pem->maxPreviewsPerFrame = maxPreviews;
END_FOR
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_displayPart ( G3DUIPARTICLEEMITTEREDIT *gpeedit ) {
    if ( gpeedit->forKey ) {
        if ( gpeedit->editedPEmitter->obj.flags & DISPLAYPARTICLES ) {
            gpeedit->editedPEmitter->obj.flags &= (~DISPLAYPARTICLES);
        } else {
            gpeedit->editedPEmitter->obj.flags |= DISPLAYPARTICLES;
        }
    } else {
FOR_EACH_SELECTED_PARTICLEEMITTER
        if ( pem->obj.flags & DISPLAYPARTICLES ) {
            pem->obj.flags &= (~DISPLAYPARTICLES);
        } else {
            pem->obj.flags |= DISPLAYPARTICLES;
        }
END_FOR
    }

    return REDRAWVIEW;
}
