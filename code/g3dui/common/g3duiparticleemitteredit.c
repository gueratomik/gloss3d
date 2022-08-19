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

#define LISTLOOP(gpeedit,var,val)                                   \
    G3DUI *gui = gpeedit->gui;                                      \
    G3DSCENE *sce = gui->sce;                                       \
    LIST *ltmpselobj = sce->lsel;                                   \
                                                                    \
    while ( ltmpselobj ) {                                          \
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;          \
                                                                    \
        if ( sel->type == G3DPARTICLEEMITTERTYPE ) {                \
            G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) sel; \
                                                                    \
            pem->var = val;                                         \
        }                                                           \
                                                                    \
        ltmpselobj = ltmpselobj->next;                              \
    }

/******************************************************************************/
uint64_t g3duiparticleemitteredit_gravityForceXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     g ) {

    LISTLOOP(gpeedit, gravity.x, g);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_gravityForceYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     g ) {
    LISTLOOP(gpeedit, gravity.y, g);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_gravityForceZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     g ) {

    LISTLOOP(gpeedit, gravity.z, g);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarTranspCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                        float                     initialVarTransp ) {
    LISTLOOP(gpeedit, initialVarTransparency, initialVarTransp);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarRotXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                      float                     initialVarRotX ) {
    LISTLOOP(gpeedit, initialVarRotation.x, initialVarRotX);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarRotYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                      float                     initialVarRotY ) {
    LISTLOOP(gpeedit, initialVarRotation.y, initialVarRotY);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarRotZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                      float                     initialVarRotZ ) {
    LISTLOOP(gpeedit, initialVarRotation.z, initialVarRotZ);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarScaXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                      float                     initialVarScaX ) {

    LISTLOOP(gpeedit, initialVarScaling.x, initialVarScaX);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarScaYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                      float                     initialVarScaY ) {
    LISTLOOP(gpeedit, initialVarScaling.y, initialVarScaY);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarScaZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                      float                     initialVarScaZ ) {
    LISTLOOP(gpeedit, initialVarScaling.z, initialVarScaZ);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarSpeedXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                        float                     initialVarSpeedX ) {
    LISTLOOP(gpeedit, initialVarSpeed.x, initialVarSpeedX);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarSpeedYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                        float                     initialVarSpeedY ) {
    LISTLOOP(gpeedit, initialVarSpeed.y, initialVarSpeedY);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarSpeedZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                        float                     initialVarSpeedZ ) {
    LISTLOOP(gpeedit, initialVarSpeed.z, initialVarSpeedZ);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarAccelXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                        float                     initialVarAccelX ) {
    LISTLOOP(gpeedit, initialVarAccel.x, initialVarAccelX);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarAccelYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                        float                     initialVarAccelY ) {
    LISTLOOP(gpeedit, initialVarAccel.y, initialVarAccelY);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialVarAccelZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                        float                     initialVarAccelZ ) {
    LISTLOOP(gpeedit, initialVarAccel.z, initialVarAccelZ);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalTranspCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalTransp ) {
    LISTLOOP(gpeedit, finalTransparency, finalTransp);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalRotXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                 float                     finalRotX ) {
    LISTLOOP(gpeedit, finalRotation.x, finalRotX);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalRotYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                 float                     finalRotY ) {
    LISTLOOP(gpeedit, finalRotation.y, finalRotY);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalRotZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                 float                     finalRotZ ) {
    LISTLOOP(gpeedit, finalRotation.z, finalRotZ);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalScaXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                 float                     finalScaX ) {
    LISTLOOP(gpeedit, finalScaling.x, finalScaX);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalScaYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                 float                     finalScaY ) {
    LISTLOOP(gpeedit, finalScaling.y, finalScaY);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalScaZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                 float                     finalScaZ ) {
    LISTLOOP(gpeedit, finalScaling.z, finalScaZ);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalSpeedXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalSpeedX ) {
    LISTLOOP(gpeedit, finalSpeed.x, finalSpeedX);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalSpeedYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalSpeedY ) {
    LISTLOOP(gpeedit, finalSpeed.y, finalSpeedY);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalSpeedZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalSpeedZ ) {
    LISTLOOP(gpeedit, finalSpeed.z, finalSpeedZ);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalAccelXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalAccelX ) {
    LISTLOOP(gpeedit, finalAccel.x, finalAccelX);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalAccelYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalAccelY ) {
    LISTLOOP(gpeedit, finalAccel.y, finalAccelY);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_finalAccelZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     finalAccelZ ) {
    LISTLOOP(gpeedit, finalAccel.z, finalAccelZ);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialTranspCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialTransp ) {
    LISTLOOP(gpeedit, initialTransparency, initialTransp);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialRotXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialRotX ) {
    LISTLOOP(gpeedit, initialRotation.x, initialRotX);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialRotYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialRotY ) {
    LISTLOOP(gpeedit, initialRotation.y, initialRotY);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialRotZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialRotZ ) {
    LISTLOOP(gpeedit, initialRotation.z, initialRotZ);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialScaXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialScaX ) {
    LISTLOOP(gpeedit, initialScaling.x, initialScaX);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialScaYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialScaY ) {
    LISTLOOP(gpeedit, initialScaling.y, initialScaY);
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialScaZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   float                     initialScaZ ) {
    LISTLOOP(gpeedit, initialScaling.z, initialScaZ);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialSpeedXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialSpeedX ) {
    LISTLOOP(gpeedit, initialSpeed.x, initialSpeedX);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialSpeedYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialSpeedY ) {
    LISTLOOP(gpeedit, initialSpeed.y, initialSpeedY);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialSpeedZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialSpeedZ ) {
    LISTLOOP(gpeedit, initialSpeed.z, initialSpeedZ);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialAccelXCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialAccelX ) {
    LISTLOOP(gpeedit, initialAccel.x, initialAccelX);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialAccelYCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialAccelY ) {
    LISTLOOP(gpeedit, initialAccel.y, initialAccelY);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_initialAccelZCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                     float                     initialAccelZ ) {
    LISTLOOP(gpeedit, initialAccel.z, initialAccelZ);

    return 0x00;
}










/******************************************************************************/
uint64_t g3duiparticleemitteredit_startAtFrameCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                    uint32_t                  startAtFrame ) {
    LISTLOOP(gpeedit, startAtFrame, startAtFrame);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_endAtFrameCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                  uint32_t                  endAtFrame ) {
    LISTLOOP(gpeedit, endAtFrame, endAtFrame);

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_radiusCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                              float                     radius ) {
    G3DUI *gui = gpeedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DPARTICLEEMITTERTYPE ) {
            G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) sel;

            pem->radius = radius;
        }

        ltmpselobj = ltmpselobj->next;
    }


    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_lifetimeCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                uint32_t                  lifetime ) {
    G3DUI *gui = gpeedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DPARTICLEEMITTERTYPE ) {
            G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) sel;

            pem->particleLifetime = lifetime;

            g3dparticleemitter_reset ( pem );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_ppfCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                           float                     ppf ) {
    G3DUI *gui = gpeedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DPARTICLEEMITTERTYPE ) {
            G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) sel;

            pem->particlesPerFrame = ppf;

            g3dparticleemitter_reset ( pem );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_maxPreviewsCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit,
                                                   uint32_t                  maxPreviews ) {
    LISTLOOP(gpeedit, maxPreviewsPerFrame, maxPreviews);


    return 0x00;
}

/******************************************************************************/
uint64_t g3duiparticleemitteredit_displayPartCbk ( G3DUIPARTICLEEMITTEREDIT *gpeedit ) {
    G3DUI *gui = gpeedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DPARTICLEEMITTERTYPE ) {
            G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) sel;

            if ( pem->obj.flags & DISPLAYPARTICLES ) {
                pem->obj.flags &= (~DISPLAYPARTICLES);
            } else {
                pem->obj.flags |= DISPLAYPARTICLES;
            }
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}
