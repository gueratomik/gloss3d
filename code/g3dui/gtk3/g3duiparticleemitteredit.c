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
#include <g3dui_gtk3.h>



#define EDITPEMITTERX "     X"
#define EDITPEMITTERY "     Y"
#define EDITPEMITTERZ "     Z"

#define EDITPEMITTERGENERAL         "General"
#define EDITPEMITTERINITIAL         "Initial"
#define EDITPEMITTERFINAL           "Final"
#define EDITPEMITTERINITIALVAR      "Variation"

#define EDITPEMITTERSTARTATFRAME           "Start at frame"
#define EDITPEMITTERENDATFRAME             "End at frame"
#define EDITPEMITTERRADIUS                 "Radius"
#define EDITPEMITTERSHAPE                  "Shape"
#define EDITPEMITTERPARTICLELIFETIME       "Lifetime"
#define EDITPEMITTERPARTICLELIFETIMEVAR    "Lifetime variation"
#define EDITPEMITTERPARTICLESPERFRAME      "Particles per frame"
#define EDITPEMITTERMAXPREVIEWS            "Max particles preview"
#define EDITPEMITTERDISPLAYPARTICULES      "Display particles"

#define EDITPEMITTERACCEL           "Acceleration"
#define EDITPEMITTERSPEED           "Speed"
#define EDITPEMITTERSCALING         "Scaling"
#define EDITPEMITTERROTATION        "Rotation"
#define EDITPEMITTERTRANSPARENCY    "Transp."

#define EDITPEMITTERINITIALANGLEVAR        "Initial angle variation"

#define EDITPEMITTERINITIALACCELVAR        "Initial acceleration variation"
#define EDITPEMITTERINITIALSPEEDVAR        "Initial speed variation"
#define EDITPEMITTERINITIALSCALINGVAR      "Initial scaling variation"
#define EDITPEMITTERINITIALROTATIONVAR     "Initial rotation variation"
#define EDITPEMITTERINITIALTRANSPARENCYVAR "Initial transparency variation"

#define EDITPEMITTERFINALACCEL             "Final acceleration"
#define EDITPEMITTERFINALSPEED             "Final speed"
#define EDITPEMITTERFINALSCALING           "Final scaling"
#define EDITPEMITTERFINALROTATION          "Final roation"
#define EDITPEMITTERFINALTRANSPARENCY      "Final transparency"

/******************************************************************************/
typedef struct _G3DUIPARTICLEEMITTEREDIT {
    G3DUIWIDGETGROUP    grp;

    GtkWidget          *main;

    GtkWidget          *startAtFrameEntry;
    GtkWidget          *endAtFrameEntry;
    GtkWidget          *radiusEntry;
    GtkWidget          *shapeCombo;
    GtkWidget          *lifetimeEntry;
    GtkWidget          *ppfEntry;
    GtkWidget          *maxPreviewsEntry;
    GtkWidget          *displayPartToggle;

    GtkWidget          *initialAccelXEntry;
    GtkWidget          *initialAccelYEntry;
    GtkWidget          *initialAccelZEntry;
    GtkWidget          *initialSpeedXEntry;
    GtkWidget          *initialSpeedYEntry;
    GtkWidget          *initialSpeedZEntry;
    GtkWidget          *initialScaXEntry;
    GtkWidget          *initialScaYEntry;
    GtkWidget          *initialScaZEntry;
    GtkWidget          *initialRotXEntry;
    GtkWidget          *initialRotYEntry;
    GtkWidget          *initialRotZEntry;
    GtkWidget          *initialTranspEntry;

    GtkWidget          *initialVarAngleEntry;
    GtkWidget          *initialVarLifetimeEntry;

    GtkWidget          *initialVarAccelXEntry;
    GtkWidget          *initialVarAccelYEntry;
    GtkWidget          *initialVarAccelZEntry;
    GtkWidget          *initialVarSpeedXEntry;
    GtkWidget          *initialVarSpeedYEntry;
    GtkWidget          *initialVarSpeedZEntry;
    GtkWidget          *initialVarScaXEntry;
    GtkWidget          *initialVarScaYEntry;
    GtkWidget          *initialVarScaZEntry;
    GtkWidget          *initialVarRotXEntry;
    GtkWidget          *initialVarRotYEntry;
    GtkWidget          *initialVarRotZEntry;
    GtkWidget          *initialVarTranspEntry;

    GtkWidget          *finalAccelXEntry;
    GtkWidget          *finalAccelYEntry;
    GtkWidget          *finalAccelZEntry;
    GtkWidget          *finalSpeedXEntry;
    GtkWidget          *finalSpeedYEntry;
    GtkWidget          *finalSpeedZEntry;
    GtkWidget          *finalScaXEntry;
    GtkWidget          *finalScaYEntry;
    GtkWidget          *finalScaZEntry;
    GtkWidget          *finalRotXEntry;
    GtkWidget          *finalRotYEntry;
    GtkWidget          *finalRotZEntry;
    GtkWidget          *finalTranspEntry;

    G3DPARTICLEEMITTER *editedPEmitter;
} G3DUIPARTICLEEMITTEREDIT;

/******************************************************************************/
static G3DUIPARTICLEEMITTEREDIT *g3duiparticleemitteredit_new ( G3DUI *gui ) {
    G3DUIPARTICLEEMITTEREDIT *ped = calloc ( 0x01, sizeof ( G3DUIPARTICLEEMITTEREDIT ) );

    if ( ped == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );
    }

    ped->grp.gui = gui;


    return ped; 
}

/******************************************************************************/
static void updateInitialVarPanel ( G3DUIPARTICLEEMITTEREDIT *ped ) {
    ped->grp.gui->lock = 0x01;

    if ( ped->editedPEmitter ) {
        gtk_widget_set_sensitive ( ped->initialVarAccelXEntry, TRUE );
        gtk_widget_set_sensitive ( ped->initialVarAccelYEntry, TRUE );
        gtk_widget_set_sensitive ( ped->initialVarAccelZEntry, TRUE );
        gtk_widget_set_sensitive ( ped->initialVarSpeedXEntry, TRUE );
        gtk_widget_set_sensitive ( ped->initialVarSpeedYEntry, TRUE );
        gtk_widget_set_sensitive ( ped->initialVarSpeedZEntry, TRUE );
        gtk_widget_set_sensitive ( ped->initialVarScaXEntry  , TRUE );
        gtk_widget_set_sensitive ( ped->initialVarScaYEntry  , TRUE );
        gtk_widget_set_sensitive ( ped->initialVarScaZEntry  , TRUE );
        gtk_widget_set_sensitive ( ped->initialVarRotXEntry  , TRUE );
        gtk_widget_set_sensitive ( ped->initialVarRotYEntry  , TRUE );
        gtk_widget_set_sensitive ( ped->initialVarRotZEntry  , TRUE );
        gtk_widget_set_sensitive ( ped->initialVarTranspEntry, TRUE );

        gtk_spin_button_set_value  ( ped->initialVarAccelXEntry, 
                                     ped->editedPEmitter->initialVarAccel.x );
        gtk_spin_button_set_value  ( ped->initialVarAccelYEntry, 
                                     ped->editedPEmitter->initialVarAccel.y );
        gtk_spin_button_set_value  ( ped->initialVarAccelZEntry, 
                                     ped->editedPEmitter->initialVarAccel.z );

        gtk_spin_button_set_value  ( ped->initialVarSpeedXEntry, 
                                     ped->editedPEmitter->initialVarSpeed.x );
        gtk_spin_button_set_value  ( ped->initialVarSpeedYEntry, 
                                     ped->editedPEmitter->initialVarSpeed.y );
        gtk_spin_button_set_value  ( ped->initialVarSpeedZEntry, 
                                     ped->editedPEmitter->initialVarSpeed.z );

        gtk_spin_button_set_value  ( ped->initialVarScaXEntry, 
                                     ped->editedPEmitter->initialVarScaling.x );
        gtk_spin_button_set_value  ( ped->initialVarScaYEntry, 
                                     ped->editedPEmitter->initialVarScaling.y );
        gtk_spin_button_set_value  ( ped->initialVarScaZEntry, 
                                     ped->editedPEmitter->initialVarScaling.z );

        gtk_spin_button_set_value  ( ped->initialVarRotXEntry, 
                                     ped->editedPEmitter->initialVarRotation.x );
        gtk_spin_button_set_value  ( ped->initialVarRotYEntry, 
                                     ped->editedPEmitter->initialVarRotation.y );
        gtk_spin_button_set_value  ( ped->initialVarRotZEntry, 
                                     ped->editedPEmitter->initialVarRotation.z );

        gtk_spin_button_set_value  ( ped->initialVarTranspEntry, 
                                     ped->editedPEmitter->initialVarTransparency );
    } else {
        gtk_widget_set_sensitive ( ped->initialVarAccelXEntry, FALSE );
        gtk_widget_set_sensitive ( ped->initialVarAccelYEntry, FALSE );
        gtk_widget_set_sensitive ( ped->initialVarAccelZEntry, FALSE );
        gtk_widget_set_sensitive ( ped->initialVarSpeedXEntry, FALSE );
        gtk_widget_set_sensitive ( ped->initialVarSpeedYEntry, FALSE );
        gtk_widget_set_sensitive ( ped->initialVarSpeedZEntry, FALSE );
        gtk_widget_set_sensitive ( ped->initialVarScaXEntry  , FALSE );
        gtk_widget_set_sensitive ( ped->initialVarScaYEntry  , FALSE );
        gtk_widget_set_sensitive ( ped->initialVarScaZEntry  , FALSE );
        gtk_widget_set_sensitive ( ped->initialVarRotXEntry  , FALSE );
        gtk_widget_set_sensitive ( ped->initialVarRotYEntry  , FALSE );
        gtk_widget_set_sensitive ( ped->initialVarRotZEntry  , FALSE );
        gtk_widget_set_sensitive ( ped->initialVarTranspEntry, FALSE );
    }

    ped->grp.gui->lock = 0x00;
}

/******************************************************************************/
static void initialVarTranspCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialVarTransp = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialVarTranspCbk ( ped->grp.gui,
                                                              ped->editedPEmitter,
                                                              initialVarTransp );
    }
}

/******************************************************************************/
static void initialVarRotXCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialVarRotX = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialVarRotXCbk ( ped->grp.gui,
                                                            ped->editedPEmitter,
                                                            initialVarRotX );
    }
}

/******************************************************************************/
static void initialVarRotYCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialVarRotY = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialVarRotYCbk ( ped->grp.gui,
                                                            ped->editedPEmitter,
                                                            initialVarRotY );
    }
}

/******************************************************************************/
static void initialVarRotZCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialVarRotZ = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialVarRotZCbk ( ped->grp.gui,
                                                            ped->editedPEmitter,
                                                            initialVarRotZ );
    }
}

/******************************************************************************/
static void initialVarScaXCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialVarScaX = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialVarScaXCbk ( ped->grp.gui,
                                                            ped->editedPEmitter,
                                                            initialVarScaX );
    }
}

/******************************************************************************/
static void initialVarScaYCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialVarScaY = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialVarScaYCbk ( ped->grp.gui,
                                                            ped->editedPEmitter,
                                                            initialVarScaY );
    }
}

/******************************************************************************/
static void initialVarScaZCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialVarScaZ = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialVarScaZCbk ( ped->grp.gui,
                                                            ped->editedPEmitter,
                                                            initialVarScaZ );
    }
}

/******************************************************************************/
static void initialVarSpeedXCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialVarSpeedX = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialVarSpeedXCbk ( ped->grp.gui,
                                                              ped->editedPEmitter,
                                                              initialVarSpeedX );
    }
}

/******************************************************************************/
static void initialVarSpeedYCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialVarSpeedY = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialVarSpeedYCbk ( ped->grp.gui,
                                                              ped->editedPEmitter,
                                                              initialVarSpeedY );
    }
}

/******************************************************************************/
static void initialVarSpeedZCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialVarSpeedZ = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialVarSpeedZCbk ( ped->grp.gui,
                                                              ped->editedPEmitter,
                                                              initialVarSpeedZ );
    }
}

/******************************************************************************/
static void initialVarAccelXCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialVarAccelX = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialVarAccelXCbk ( ped->grp.gui,
                                                              ped->editedPEmitter,
                                                              initialVarAccelX );
    }
}

/******************************************************************************/
static void initialVarAccelYCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialVarAccelY = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialVarAccelYCbk ( ped->grp.gui,
                                                              ped->editedPEmitter,
                                                              initialVarAccelY );
    }
}

/******************************************************************************/
static void initialVarAccelZCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialVarAccelZ = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialVarAccelZCbk ( ped->grp.gui,
                                                              ped->editedPEmitter,
                                                              initialVarAccelZ );
    }
}

/******************************************************************************/
static GtkWidget *createInitialVarPanel ( GtkWidget                *parent, 
                                       G3DUIPARTICLEEMITTEREDIT *ped,
                                       char                     *name,
                                       gint                      x,
                                       gint                      y,
                                       gint                      width,
                                       gint                      height ) {
    GtkWidget *pan = createPanel ( parent, ped, name, x, y, width, height );

    ped->initialVarTranspEntry  = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERTRANSPARENCY,
                                                 0.0f, FLT_MAX,
                                                 0, 0, 64, 32,
                                                 initialVarTranspCbk );

    createSimpleLabel ( pan, 
                        ped,
                        EDITPEMITTERX,
                        64, 24, 64, 20 );

    createSimpleLabel ( pan, 
                        ped,
                        EDITPEMITTERY,
                        144, 24, 64, 20 );

    createSimpleLabel ( pan, 
                        ped,
                        EDITPEMITTERZ,
                        224, 24, 64, 20 );


    createSimpleLabel ( pan, 
                        ped,
                        EDITPEMITTERACCEL,
                        0, 48, 64, 20 );

    ped->initialVarAccelXEntry  = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERX,
                                                 0.0f, FLT_MAX,
                                                 64, 48, 0, 32,
                                                 initialVarAccelXCbk );

    ped->initialVarAccelYEntry  = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERY,
                                                 0.0f, FLT_MAX,
                                                 144, 48, 0, 32,
                                                 initialVarAccelYCbk );

    ped->initialVarAccelZEntry  = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERZ,
                                                 0.0f, FLT_MAX,
                                                 224, 48, 0, 32,
                                                 initialVarAccelZCbk );


    createSimpleLabel ( pan, 
                        ped,
                        EDITPEMITTERSPEED,
                        0, 72, 64, 20 );

    ped->initialVarSpeedXEntry  = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERX,
                                                 0.0f, FLT_MAX,
                                                 64, 72, 0, 32,
                                                 initialVarSpeedXCbk );

    ped->initialVarSpeedYEntry  = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERY,
                                                 0.0f, FLT_MAX,
                                                 144, 72, 0, 32,
                                                 initialVarSpeedYCbk );

    ped->initialVarSpeedZEntry  = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERZ,
                                                 0.0f, FLT_MAX,
                                                 224, 72, 0, 32,
                                                 initialVarSpeedZCbk );

    createSimpleLabel ( pan, 
                        ped,
                        EDITPEMITTERSCALING,
                        0, 96, 64, 20 );

    ped->initialVarScaXEntry    = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERX,
                                                 0.0f, FLT_MAX,
                                                 64, 96, 0, 32,
                                                 initialVarScaXCbk );

    ped->initialVarScaYEntry    = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERY,
                                                 0.0f, FLT_MAX,
                                                 144, 96, 0, 32,
                                                 initialVarScaYCbk );

    ped->initialVarScaZEntry    = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERZ,
                                                 0.0f, FLT_MAX,
                                                 224, 96, 0, 32,
                                                 initialVarScaZCbk );


    createSimpleLabel ( pan, 
                        ped,
                        EDITPEMITTERROTATION,
                        0, 120, 64, 20 );

    ped->initialVarRotXEntry    = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERX,
                                                 0.0f, FLT_MAX,
                                                 64, 120, 0, 32,
                                                 initialVarRotXCbk );

    ped->initialVarRotYEntry    = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERY,
                                                 0.0f, FLT_MAX,
                                                 144, 120, 0, 32,
                                                 initialVarRotYCbk );

    ped->initialVarRotZEntry    = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERZ,
                                                 0.0f, FLT_MAX,
                                                 224, 120, 0, 32,
                                                 initialVarRotZCbk );




    return pan;
}

/******************************************************************************/
static void updateFinalPanel ( G3DUIPARTICLEEMITTEREDIT *ped ) {
    ped->grp.gui->lock = 0x01;

    if ( ped->editedPEmitter ) {
        gtk_widget_set_sensitive ( ped->finalAccelXEntry, TRUE );
        gtk_widget_set_sensitive ( ped->finalAccelYEntry, TRUE );
        gtk_widget_set_sensitive ( ped->finalAccelZEntry, TRUE );
        gtk_widget_set_sensitive ( ped->finalSpeedXEntry, TRUE );
        gtk_widget_set_sensitive ( ped->finalSpeedYEntry, TRUE );
        gtk_widget_set_sensitive ( ped->finalSpeedZEntry, TRUE );
        gtk_widget_set_sensitive ( ped->finalScaXEntry  , TRUE );
        gtk_widget_set_sensitive ( ped->finalScaYEntry  , TRUE );
        gtk_widget_set_sensitive ( ped->finalScaZEntry  , TRUE );
        gtk_widget_set_sensitive ( ped->finalRotXEntry  , TRUE );
        gtk_widget_set_sensitive ( ped->finalRotYEntry  , TRUE );
        gtk_widget_set_sensitive ( ped->finalRotZEntry  , TRUE );
        gtk_widget_set_sensitive ( ped->finalTranspEntry, TRUE );

        gtk_spin_button_set_value  ( ped->finalAccelXEntry, 
                                     ped->editedPEmitter->finalAccel.x );
        gtk_spin_button_set_value  ( ped->finalAccelYEntry, 
                                     ped->editedPEmitter->finalAccel.y );
        gtk_spin_button_set_value  ( ped->finalAccelZEntry, 
                                     ped->editedPEmitter->finalAccel.z );

        gtk_spin_button_set_value  ( ped->finalSpeedXEntry, 
                                     ped->editedPEmitter->finalSpeed.x );
        gtk_spin_button_set_value  ( ped->finalSpeedYEntry, 
                                     ped->editedPEmitter->finalSpeed.y );
        gtk_spin_button_set_value  ( ped->finalSpeedZEntry, 
                                     ped->editedPEmitter->finalSpeed.z );

        gtk_spin_button_set_value  ( ped->finalScaXEntry, 
                                     ped->editedPEmitter->finalScaling.x );
        gtk_spin_button_set_value  ( ped->finalScaYEntry, 
                                     ped->editedPEmitter->finalScaling.y );
        gtk_spin_button_set_value  ( ped->finalScaZEntry, 
                                     ped->editedPEmitter->finalScaling.z );

        gtk_spin_button_set_value  ( ped->finalRotXEntry, 
                                     ped->editedPEmitter->finalRotation.x );
        gtk_spin_button_set_value  ( ped->finalRotYEntry, 
                                     ped->editedPEmitter->finalRotation.y );
        gtk_spin_button_set_value  ( ped->finalRotZEntry, 
                                     ped->editedPEmitter->finalRotation.z );

        gtk_spin_button_set_value  ( ped->finalTranspEntry, 
                                     ped->editedPEmitter->finalTransparency );
    } else {
        gtk_widget_set_sensitive ( ped->finalAccelXEntry, FALSE );
        gtk_widget_set_sensitive ( ped->finalAccelYEntry, FALSE );
        gtk_widget_set_sensitive ( ped->finalAccelZEntry, FALSE );
        gtk_widget_set_sensitive ( ped->finalSpeedXEntry, FALSE );
        gtk_widget_set_sensitive ( ped->finalSpeedYEntry, FALSE );
        gtk_widget_set_sensitive ( ped->finalSpeedZEntry, FALSE );
        gtk_widget_set_sensitive ( ped->finalScaXEntry  , FALSE );
        gtk_widget_set_sensitive ( ped->finalScaYEntry  , FALSE );
        gtk_widget_set_sensitive ( ped->finalScaZEntry  , FALSE );
        gtk_widget_set_sensitive ( ped->finalRotXEntry  , FALSE );
        gtk_widget_set_sensitive ( ped->finalRotYEntry  , FALSE );
        gtk_widget_set_sensitive ( ped->finalRotZEntry  , FALSE );
        gtk_widget_set_sensitive ( ped->finalTranspEntry, FALSE );
    }

    ped->grp.gui->lock = 0x00;
}

/******************************************************************************/
static void finalTranspCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float finalTransp = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_finalTranspCbk ( ped->grp.gui,
                                                         ped->editedPEmitter,
                                                         finalTransp );
    }
}

/******************************************************************************/
static void finalRotXCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float finalRotX = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_finalRotXCbk ( ped->grp.gui,
                                                       ped->editedPEmitter,
                                                       finalRotX );
    }
}

/******************************************************************************/
static void finalRotYCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float finalRotY = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_finalRotYCbk ( ped->grp.gui,
                                                       ped->editedPEmitter,
                                                       finalRotY );
    }
}

/******************************************************************************/
static void finalRotZCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float finalRotZ = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_finalRotZCbk ( ped->grp.gui,
                                                       ped->editedPEmitter,
                                                       finalRotZ );
    }
}

/******************************************************************************/
static void finalScaXCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float finalScaX = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_finalScaXCbk ( ped->grp.gui,
                                                       ped->editedPEmitter,
                                                       finalScaX );
    }
}

/******************************************************************************/
static void finalScaYCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float finalScaY = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_finalScaYCbk ( ped->grp.gui,
                                                       ped->editedPEmitter,
                                                       finalScaY );
    }
}

/******************************************************************************/
static void finalScaZCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float finalScaZ = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_finalScaZCbk ( ped->grp.gui,
                                                       ped->editedPEmitter,
                                                       finalScaZ );
    }
}

/******************************************************************************/
static void finalSpeedXCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float finalSpeedX = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_finalSpeedXCbk ( ped->grp.gui,
                                                         ped->editedPEmitter,
                                                         finalSpeedX );
    }
}

/******************************************************************************/
static void finalSpeedYCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float finalSpeedY = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_finalSpeedYCbk ( ped->grp.gui,
                                                         ped->editedPEmitter,
                                                         finalSpeedY );
    }
}

/******************************************************************************/
static void finalSpeedZCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float finalSpeedZ = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_finalSpeedZCbk ( ped->grp.gui,
                                                         ped->editedPEmitter,
                                                         finalSpeedZ );
    }
}

/******************************************************************************/
static void finalAccelXCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float finalAccelX = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_finalAccelXCbk ( ped->grp.gui,
                                                         ped->editedPEmitter,
                                                         finalAccelX );
    }
}

/******************************************************************************/
static void finalAccelYCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float finalAccelY = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_finalAccelYCbk ( ped->grp.gui,
                                                         ped->editedPEmitter,
                                                         finalAccelY );
    }
}

/******************************************************************************/
static void finalAccelZCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float finalAccelZ = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_finalAccelZCbk ( ped->grp.gui,
                                                         ped->editedPEmitter,
                                                         finalAccelZ );
    }
}

/******************************************************************************/
static GtkWidget *createFinalPanel ( GtkWidget                *parent, 
                                       G3DUIPARTICLEEMITTEREDIT *ped,
                                       char                     *name,
                                       gint                      x,
                                       gint                      y,
                                       gint                      width,
                                       gint                      height ) {
    GtkWidget *pan = createPanel ( parent, ped, name, x, y, width, height );

    ped->finalTranspEntry  = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERTRANSPARENCY,
                                                 0.0f, FLT_MAX,
                                                 0, 0, 64, 32,
                                                 finalTranspCbk );

    createSimpleLabel ( pan, 
                        ped,
                        EDITPEMITTERX,
                        64, 24, 64, 20 );

    createSimpleLabel ( pan, 
                        ped,
                        EDITPEMITTERY,
                        144, 24, 64, 20 );

    createSimpleLabel ( pan, 
                        ped,
                        EDITPEMITTERZ,
                        224, 24, 64, 20 );


    createSimpleLabel ( pan, 
                        ped,
                        EDITPEMITTERACCEL,
                        0, 48, 64, 20 );

    ped->finalAccelXEntry  = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERX,
                                                 0.0f, FLT_MAX,
                                                 64, 48, 0, 32,
                                                 finalAccelXCbk );

    ped->finalAccelYEntry  = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERY,
                                                 0.0f, FLT_MAX,
                                                 144, 48, 0, 32,
                                                 finalAccelYCbk );

    ped->finalAccelZEntry  = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERZ,
                                                 0.0f, FLT_MAX,
                                                 224, 48, 0, 32,
                                                 finalAccelZCbk );


    createSimpleLabel ( pan, 
                        ped,
                        EDITPEMITTERSPEED,
                        0, 72, 64, 20 );

    ped->finalSpeedXEntry  = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERX,
                                                 0.0f, FLT_MAX,
                                                 64, 72, 0, 32,
                                                 finalSpeedXCbk );

    ped->finalSpeedYEntry  = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERY,
                                                 0.0f, FLT_MAX,
                                                 144, 72, 0, 32,
                                                 finalSpeedYCbk );

    ped->finalSpeedZEntry  = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERZ,
                                                 0.0f, FLT_MAX,
                                                 224, 72, 0, 32,
                                                 finalSpeedZCbk );

    createSimpleLabel ( pan, 
                        ped,
                        EDITPEMITTERSCALING,
                        0, 96, 64, 20 );

    ped->finalScaXEntry    = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERX,
                                                 0.0f, FLT_MAX,
                                                 64, 96, 0, 32,
                                                 finalScaXCbk );

    ped->finalScaYEntry    = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERY,
                                                 0.0f, FLT_MAX,
                                                 144, 96, 0, 32,
                                                 finalScaYCbk );

    ped->finalScaZEntry    = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERZ,
                                                 0.0f, FLT_MAX,
                                                 224, 96, 0, 32,
                                                 finalScaZCbk );


    createSimpleLabel ( pan, 
                        ped,
                        EDITPEMITTERROTATION,
                        0, 120, 64, 20 );

    ped->finalRotXEntry    = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERX,
                                                 0.0f, FLT_MAX,
                                                 64, 120, 0, 32,
                                                 finalRotXCbk );

    ped->finalRotYEntry    = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERY,
                                                 0.0f, FLT_MAX,
                                                 144, 120, 0, 32,
                                                 finalRotYCbk );

    ped->finalRotZEntry    = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERZ,
                                                 0.0f, FLT_MAX,
                                                 224, 120, 0, 32,
                                                 finalRotZCbk );




    return pan;
}

/******************************************************************************/
static void updateInitialPanel ( G3DUIPARTICLEEMITTEREDIT *ped ) {
    ped->grp.gui->lock = 0x01;

    if ( ped->editedPEmitter ) {
        gtk_widget_set_sensitive ( ped->initialAccelXEntry, TRUE );
        gtk_widget_set_sensitive ( ped->initialAccelYEntry, TRUE );
        gtk_widget_set_sensitive ( ped->initialAccelZEntry, TRUE );
        gtk_widget_set_sensitive ( ped->initialSpeedXEntry, TRUE );
        gtk_widget_set_sensitive ( ped->initialSpeedYEntry, TRUE );
        gtk_widget_set_sensitive ( ped->initialSpeedZEntry, TRUE );
        gtk_widget_set_sensitive ( ped->initialScaXEntry  , TRUE );
        gtk_widget_set_sensitive ( ped->initialScaYEntry  , TRUE );
        gtk_widget_set_sensitive ( ped->initialScaZEntry  , TRUE );
        gtk_widget_set_sensitive ( ped->initialRotXEntry  , TRUE );
        gtk_widget_set_sensitive ( ped->initialRotYEntry  , TRUE );
        gtk_widget_set_sensitive ( ped->initialRotZEntry  , TRUE );
        gtk_widget_set_sensitive ( ped->initialTranspEntry, TRUE );

        gtk_spin_button_set_value  ( ped->initialAccelXEntry, 
                                     ped->editedPEmitter->initialAccel.x );
        gtk_spin_button_set_value  ( ped->initialAccelYEntry, 
                                     ped->editedPEmitter->initialAccel.y );
        gtk_spin_button_set_value  ( ped->initialAccelZEntry, 
                                     ped->editedPEmitter->initialAccel.z );

        gtk_spin_button_set_value  ( ped->initialSpeedXEntry, 
                                     ped->editedPEmitter->initialSpeed.x );
        gtk_spin_button_set_value  ( ped->initialSpeedYEntry, 
                                     ped->editedPEmitter->initialSpeed.y );
        gtk_spin_button_set_value  ( ped->initialSpeedZEntry, 
                                     ped->editedPEmitter->initialSpeed.z );

        gtk_spin_button_set_value  ( ped->initialScaXEntry, 
                                     ped->editedPEmitter->initialScaling.x );
        gtk_spin_button_set_value  ( ped->initialScaYEntry, 
                                     ped->editedPEmitter->initialScaling.y );
        gtk_spin_button_set_value  ( ped->initialScaZEntry, 
                                     ped->editedPEmitter->initialScaling.z );

        gtk_spin_button_set_value  ( ped->initialRotXEntry, 
                                     ped->editedPEmitter->initialRotation.x );
        gtk_spin_button_set_value  ( ped->initialRotYEntry, 
                                     ped->editedPEmitter->initialRotation.y );
        gtk_spin_button_set_value  ( ped->initialRotZEntry, 
                                     ped->editedPEmitter->initialRotation.z );

        gtk_spin_button_set_value  ( ped->initialTranspEntry, 
                                     ped->editedPEmitter->initialTransparency );
    } else {
        gtk_widget_set_sensitive ( ped->initialAccelXEntry, FALSE );
        gtk_widget_set_sensitive ( ped->initialAccelYEntry, FALSE );
        gtk_widget_set_sensitive ( ped->initialAccelZEntry, FALSE );
        gtk_widget_set_sensitive ( ped->initialSpeedXEntry, FALSE );
        gtk_widget_set_sensitive ( ped->initialSpeedYEntry, FALSE );
        gtk_widget_set_sensitive ( ped->initialSpeedZEntry, FALSE );
        gtk_widget_set_sensitive ( ped->initialScaXEntry  , FALSE );
        gtk_widget_set_sensitive ( ped->initialScaYEntry  , FALSE );
        gtk_widget_set_sensitive ( ped->initialScaZEntry  , FALSE );
        gtk_widget_set_sensitive ( ped->initialRotXEntry  , FALSE );
        gtk_widget_set_sensitive ( ped->initialRotYEntry  , FALSE );
        gtk_widget_set_sensitive ( ped->initialRotZEntry  , FALSE );
        gtk_widget_set_sensitive ( ped->initialTranspEntry, FALSE );
    }

    ped->grp.gui->lock = 0x00;
}

/******************************************************************************/
static void initialTranspCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialTransp = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialTranspCbk ( ped->grp.gui,
                                                           ped->editedPEmitter,
                                                           initialTransp );
    }
}

/******************************************************************************/
static void initialRotXCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialRotX = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialRotXCbk ( ped->grp.gui,
                                                         ped->editedPEmitter,
                                                         initialRotX );
    }
}

/******************************************************************************/
static void initialRotYCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialRotY = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialRotYCbk ( ped->grp.gui,
                                                         ped->editedPEmitter,
                                                         initialRotY );
    }
}

/******************************************************************************/
static void initialRotZCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialRotZ = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialRotZCbk ( ped->grp.gui,
                                                         ped->editedPEmitter,
                                                         initialRotZ );
    }
}

/******************************************************************************/
static void initialScaXCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialScaX = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialScaXCbk ( ped->grp.gui,
                                                         ped->editedPEmitter,
                                                         initialScaX );
    }
}

/******************************************************************************/
static void initialScaYCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialScaY = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialScaYCbk ( ped->grp.gui,
                                                         ped->editedPEmitter,
                                                         initialScaY );
    }
}

/******************************************************************************/
static void initialScaZCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialScaZ = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialScaZCbk ( ped->grp.gui,
                                                           ped->editedPEmitter,
                                                           initialScaZ );
    }
}

/******************************************************************************/
static void initialSpeedXCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialSpeedX = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialSpeedXCbk ( ped->grp.gui,
                                                           ped->editedPEmitter,
                                                           initialSpeedX );
    }
}

/******************************************************************************/
static void initialSpeedYCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialSpeedY = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialSpeedYCbk ( ped->grp.gui,
                                                           ped->editedPEmitter,
                                                           initialSpeedY );
    }
}

/******************************************************************************/
static void initialSpeedZCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialSpeedZ = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialSpeedZCbk ( ped->grp.gui,
                                                           ped->editedPEmitter,
                                                           initialSpeedZ );
    }
}

/******************************************************************************/
static void initialAccelXCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialAccelX = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialAccelXCbk ( ped->grp.gui,
                                                           ped->editedPEmitter,
                                                           initialAccelX );
    }
}

/******************************************************************************/
static void initialAccelYCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialAccelY = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialAccelYCbk ( ped->grp.gui,
                                                           ped->editedPEmitter,
                                                           initialAccelY );
    }
}

/******************************************************************************/
static void initialAccelZCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float initialAccelZ = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_initialAccelZCbk ( ped->grp.gui,
                                                           ped->editedPEmitter,
                                                           initialAccelZ );
    }
}

/******************************************************************************/
static GtkWidget *createInitialPanel ( GtkWidget                *parent, 
                                       G3DUIPARTICLEEMITTEREDIT *ped,
                                       char                     *name,
                                       gint                      x,
                                       gint                      y,
                                       gint                      width,
                                       gint                      height ) {
    GtkWidget *pan = createPanel ( parent, ped, name, x, y, width, height );

    ped->initialTranspEntry  = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERTRANSPARENCY,
                                                 0.0f, FLT_MAX,
                                                 0, 0, 64, 32,
                                                 initialTranspCbk );

    createSimpleLabel ( pan, 
                        ped,
                        EDITPEMITTERX,
                        64, 24, 64, 20 );

    createSimpleLabel ( pan, 
                        ped,
                        EDITPEMITTERY,
                        144, 24, 64, 20 );

    createSimpleLabel ( pan, 
                        ped,
                        EDITPEMITTERZ,
                        224, 24, 64, 20 );


    createSimpleLabel ( pan, 
                        ped,
                        EDITPEMITTERACCEL,
                        0, 48, 64, 20 );

    ped->initialAccelXEntry  = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERX,
                                                 0.0f, FLT_MAX,
                                                 64, 48, 0, 32,
                                                 initialAccelXCbk );

    ped->initialAccelYEntry  = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERY,
                                                 0.0f, FLT_MAX,
                                                 144, 48, 0, 32,
                                                 initialAccelYCbk );

    ped->initialAccelZEntry  = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERZ,
                                                 0.0f, FLT_MAX,
                                                 224, 48, 0, 32,
                                                 initialAccelZCbk );


    createSimpleLabel ( pan, 
                        ped,
                        EDITPEMITTERSPEED,
                        0, 72, 64, 20 );

    ped->initialSpeedXEntry  = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERX,
                                                 0.0f, FLT_MAX,
                                                 64, 72, 0, 32,
                                                 initialSpeedXCbk );

    ped->initialSpeedYEntry  = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERY,
                                                 0.0f, FLT_MAX,
                                                 144, 72, 0, 32,
                                                 initialSpeedYCbk );

    ped->initialSpeedZEntry  = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERZ,
                                                 0.0f, FLT_MAX,
                                                 224, 72, 0, 32,
                                                 initialSpeedZCbk );

    createSimpleLabel ( pan, 
                        ped,
                        EDITPEMITTERSCALING,
                        0, 96, 64, 20 );

    ped->initialScaXEntry    = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERX,
                                                 0.0f, FLT_MAX,
                                                 64, 96, 0, 32,
                                                 initialScaXCbk );

    ped->initialScaYEntry    = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERY,
                                                 0.0f, FLT_MAX,
                                                 144, 96, 0, 32,
                                                 initialScaYCbk );

    ped->initialScaZEntry    = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERZ,
                                                 0.0f, FLT_MAX,
                                                 224, 96, 0, 32,
                                                 initialScaZCbk );


    createSimpleLabel ( pan, 
                        ped,
                        EDITPEMITTERROTATION,
                        0, 120, 64, 20 );

    ped->initialRotXEntry    = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERX,
                                                 0.0f, FLT_MAX,
                                                 64, 120, 0, 32,
                                                 initialRotXCbk );

    ped->initialRotYEntry    = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERY,
                                                 0.0f, FLT_MAX,
                                                 144, 120, 0, 32,
                                                 initialRotYCbk );

    ped->initialRotZEntry    = createFloatText ( pan,
                                                 ped,
                                                 EDITPEMITTERZ,
                                                 0.0f, FLT_MAX,
                                                 224, 120, 0, 32,
                                                 initialRotZCbk );




    return pan;
}

/******************************************************************************/
static void updateGeneralPanel ( G3DUIPARTICLEEMITTEREDIT *ped ) {
    ped->grp.gui->lock = 0x01;

    if ( ped->editedPEmitter ) {
        gtk_widget_set_sensitive ( ped->startAtFrameEntry, TRUE );
        gtk_widget_set_sensitive ( ped->endAtFrameEntry  , TRUE );
        /*gtk_widget_set_sensitive ( ped->shapeCombo       , TRUE );*/
        gtk_widget_set_sensitive ( ped->radiusEntry      , TRUE );
        gtk_widget_set_sensitive ( ped->lifetimeEntry    , TRUE );
        gtk_widget_set_sensitive ( ped->ppfEntry         , TRUE );
        gtk_widget_set_sensitive ( ped->maxPreviewsEntry , TRUE );
        gtk_widget_set_sensitive ( ped->displayPartToggle, TRUE );

        gtk_spin_button_set_value  ( ped->startAtFrameEntry, 
                                     ped->editedPEmitter->startAtFrame );

        gtk_spin_button_set_value  ( ped->endAtFrameEntry, 
                                     ped->editedPEmitter->endAtFrame );

        gtk_spin_button_set_value  ( ped->radiusEntry, 
                                     ped->editedPEmitter->radius );

        gtk_spin_button_set_value  ( ped->lifetimeEntry, 
                                     ped->editedPEmitter->particleLifetime );

        gtk_spin_button_set_value  ( ped->ppfEntry, 
                                     ped->editedPEmitter->particlesPerFrame );

        gtk_spin_button_set_value  ( ped->maxPreviewsEntry, 
                                     ped->editedPEmitter->maxPreviewsPerFrame );

        if ( ((G3DOBJECT*)ped->editedPEmitter)->flags & DISPLAYPARTICLES ) {
            gtk_toggle_button_set_active ( ped->displayPartToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( ped->displayPartToggle, FALSE );
        }
    } else {
        gtk_widget_set_sensitive ( ped->startAtFrameEntry, FALSE );
        gtk_widget_set_sensitive ( ped->endAtFrameEntry  , FALSE );
        /*gtk_widget_set_sensitive ( ped->shapeCombo       , FALSE );*/
        gtk_widget_set_sensitive ( ped->radiusEntry      , FALSE );
        gtk_widget_set_sensitive ( ped->lifetimeEntry    , FALSE );
        gtk_widget_set_sensitive ( ped->ppfEntry         , FALSE );
        gtk_widget_set_sensitive ( ped->maxPreviewsEntry , FALSE );
        gtk_widget_set_sensitive ( ped->displayPartToggle, FALSE );
    }

    ped->grp.gui->lock = 0x00;
}

/******************************************************************************/
static void startAtFrameCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float startAtFrame = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_startAtFrameCbk ( ped->grp.gui,
                                                          ped->editedPEmitter,
                                                          startAtFrame );
    }

    g3dui_redrawGLViews ( ped->grp.gui );
}

/******************************************************************************/
static void endAtFrameCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float endAtFrame = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_endAtFrameCbk ( ped->grp.gui,
                                                        ped->editedPEmitter,
                                                        endAtFrame );
    }

    g3dui_redrawGLViews ( ped->grp.gui );
}

/******************************************************************************/
static void radiusCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float radius = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_radiusCbk ( ped->grp.gui,
                                                    ped->editedPEmitter,
                                                    radius );
    }

    g3dui_redrawGLViews ( ped->grp.gui );
}

/******************************************************************************/
static void lifetimeCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float lifetime = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_lifetimeCbk ( ped->grp.gui,
                                                      ped->editedPEmitter,
                                                      lifetime );
    }

    g3dui_redrawGLViews ( ped->grp.gui );
}

/******************************************************************************/
static void ppfCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float ppf = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_ppfCbk ( ped->grp.gui,
                                                 ped->editedPEmitter,
                                                 ppf );
    }

    g3dui_redrawGLViews ( ped->grp.gui );
}

/******************************************************************************/
static void maxPreviewsCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float maxPreviews = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_maxPreviewsCbk ( ped->grp.gui,
                                                         ped->editedPEmitter,
                                                         maxPreviews );
    }

    g3dui_redrawGLViews ( ped->grp.gui );
}

/******************************************************************************/
static void displayPartCbk  ( GtkWidget *widget, 
                              gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_displayPartCbk ( ped->grp.gui, 
                                                         ped->editedPEmitter );
    }

    g3dui_redrawGLViews ( ped->grp.gui );
}

/******************************************************************************/
static GtkWidget *createGeneralPanel ( GtkWidget                *parent, 
                                       G3DUIPARTICLEEMITTEREDIT *ped,
                                       char                     *name,
                                       gint                      x,
                                       gint                      y,
                                       gint                      width,
                                       gint                      height ) {
    GtkWidget *pan = createPanel ( parent, ped, name, x, y, width, height );

    ped->startAtFrameEntry = createIntegerText ( pan,
                                                 ped,
                                                 EDITPEMITTERSTARTATFRAME,
                                                 INT_MIN, INT_MAX,
                                                 0,   0, 120, 24,
                                                 startAtFrameCbk );

    ped->endAtFrameEntry   = createIntegerText ( pan,
                                                 ped,
                                                 EDITPEMITTERENDATFRAME,
                                                 INT_MIN, INT_MAX,
                                                 0,  24, 120, 24,
                                                 endAtFrameCbk );

    ped->radiusEntry       = createFloatText   ( pan,
                                                 ped,
                                                 EDITPEMITTERRADIUS,
                                                 0.0f, FLT_MAX,
                                                 0, 48, 120, 96,
                                                 radiusCbk );

    /*ped->shapeCombo        = createFloatText   ( pan,
                                                 ped,
                                                 EDITPEMITTERSHAPE,
                                                 0.0f, FLT_MAX,
                                                 0, 72, 96, 96,
                                                 spotAngleCbk );*/

    ped->lifetimeEntry     = createIntegerText ( pan,
                                                 ped,
                                                 EDITPEMITTERPARTICLELIFETIME,
                                                 0x00, INT_MAX,
                                                 0, 96, 120, 96,
                                                 lifetimeCbk );

    ped->ppfEntry          = createFloatText   ( pan,
                                                 ped,
                                                 EDITPEMITTERPARTICLESPERFRAME,
                                                 0.0f, FLT_MAX,
                                                 0, 120, 120, 96,
                                                 ppfCbk );

    ped->maxPreviewsEntry  = createIntegerText ( pan,
                                                 ped,
                                                 EDITPEMITTERMAXPREVIEWS,
                                                 0x00, INT_MAX,
                                                 0, 144, 120, 96,
                                                 maxPreviewsCbk );

    ped->displayPartToggle = createToggleLabel ( pan,
                                                 ped,
                                                 EDITPEMITTERDISPLAYPARTICULES,
                                                 0, 170, 192, 18,
                                                 displayPartCbk );

    return pan;
}

/******************************************************************************/
void updateParticleEmitterEdit ( GtkWidget           *w, 
                                 G3DPARTICLEEMITTER  *pem ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) g_object_get_data ( G_OBJECT(w),
                                                                   "private" );

    ped->editedPEmitter = pem;

    updateGeneralPanel    ( ped );
    updateInitialPanel    ( ped );
    updateFinalPanel      ( ped );
    updateInitialVarPanel ( ped );

    /*updateDiffuseColorPanel ( led );
    updateLightGeneralPanel ( led );
    updateShadowsPanel      ( led );*/
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;

    free ( ped );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;

    updateParticleEmitterEdit ( widget, NULL );
}

/******************************************************************************/
GtkWidget *createParticleEmitterEdit ( GtkWidget *parent,
                                       G3DUI     *gui,
                                       char      *name,
                                       gint       x,
                                       gint       y,
                                       gint       width,
                                       gint       height ) {
    GdkRectangle gdkrec = { 0x00, 0x00, width, height };
    GtkWidget *tab = gtk_notebook_new ( );
    G3DUIPARTICLEEMITTEREDIT *ped = g3duiparticleemitteredit_new ( gui );

    g_object_set_data ( G_OBJECT(tab), "private", (gpointer) ped );

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(tab), TRUE );

    gtk_widget_set_name ( tab, name );

    gtk_widget_size_allocate ( tab, &gdkrec );
    /*gtk_widget_set_size_request ( tab, width, height );*/

    gtk_fixed_put ( GTK_FIXED(parent), tab, x, y );

    g_signal_connect ( G_OBJECT (tab), "realize", G_CALLBACK (Realize), ped );
    g_signal_connect ( G_OBJECT (tab), "destroy", G_CALLBACK (Destroy), ped );

    createGeneralPanel    ( tab, ped, EDITPEMITTERGENERAL   , 0, 0, width, height );
    createInitialPanel    ( tab, ped, EDITPEMITTERINITIAL   , 0, 0, width, height );
    createFinalPanel      ( tab, ped, EDITPEMITTERFINAL     , 0, 0, width, height );
    createInitialVarPanel ( tab, ped, EDITPEMITTERINITIALVAR, 0, 0, width, height );
/*
    createFinalPanel     ( tab, ped, EDITSPECULAR    , 0, 0, width, height );
    createVariationPanel ( tab, ped, EDITPEMITTERSHADOWS, 0, 0, width, height );
*/
    /*list_insert ( &gui->lligedit, tab );*/

    gtk_widget_show ( tab );


    return tab;
}
