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

/******************************************************************************/
#define EDITPEMITTERX "     X"
#define EDITPEMITTERY "     Y"
#define EDITPEMITTERZ "     Z"

#define EDITPEMITTERGENERAL         "Particle Emitter"
#define EDITPEMITTERINITIAL         "Initial"
#define EDITPEMITTERFINAL           "Final"
#define EDITPEMITTERINITIALVAR      "Variation"
#define EDITPEMITTERFORCES          "Forces"

#define EDITPEMITTERSTARTATFRAME           "Start at frame"
#define EDITPEMITTERENDATFRAME             "End at frame"
#define EDITPEMITTERRADIUS                 "Radius"
#define EDITPEMITTERSHAPE                  "Shape"
#define EDITPEMITTERPARTICLELIFETIME       "Lifetime"
#define EDITPEMITTERPARTICLELIFETIMEVAR    "Lifetime variation"
#define EDITPEMITTERPARTICLESPERFRAME      "Particles per frame"
#define EDITPEMITTERMAXPREVIEWS            "Max particles preview"
#define EDITPEMITTERDISPLAYPARTICULES      "Display particles"

#define EDITPEMITTERACCELX           "Accel. X"
#define EDITPEMITTERACCELY           "Accel. Y"
#define EDITPEMITTERACCELZ           "Accel. Z"

#define EDITPEMITTERSPEEDX           "Speed X"
#define EDITPEMITTERSPEEDY           "Speed Y"
#define EDITPEMITTERSPEEDZ           "Speed Z"

#define EDITPEMITTERSCALINGX         "Scaling X"
#define EDITPEMITTERSCALINGY         "Scaling Y"
#define EDITPEMITTERSCALINGZ         "Scaling Z"

#define EDITPEMITTERROTATIONX        "Rotation X"
#define EDITPEMITTERROTATIONY        "Rotation Y"
#define EDITPEMITTERROTATIONZ        "Rotation Z"

#define EDITPEMITTERTRANSPARENCY    "Transparency"

#define EDITPEMITTERGRAVITYX         "Gravity X"
#define EDITPEMITTERGRAVITYY         "Gravity Y"
#define EDITPEMITTERGRAVITYZ         "Gravity Z"

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
static GTK3G3DUIPARTICLEEMITTEREDIT *gtk3_g3duiparticleemitteredit_new ( GTK3G3DUI *gtk3gui ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = calloc ( 0x01, sizeof ( GTK3G3DUIPARTICLEEMITTEREDIT ) );

    if ( gtk3ped == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3ped->core.gui = ( G3DUI * ) gtk3gui;



    return gtk3ped; 
}

/******************************************************************************/
static void updateForcesPanel ( GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped ) {
    gtk3ped->core.gui->lock = 0x01;

    if ( gtk3ped->core.editedPEmitter ) {
        gtk_spin_button_set_value  ( gtk3ped->gravityForceXEntry, 
                                     gtk3ped->core.editedPEmitter->gravity.x );
        gtk_spin_button_set_value  ( gtk3ped->gravityForceYEntry, 
                                     gtk3ped->core.editedPEmitter->gravity.y );
        gtk_spin_button_set_value  ( gtk3ped->gravityForceZEntry, 
                                     gtk3ped->core.editedPEmitter->gravity.z );

    }

    gtk3ped->core.gui->lock = 0x00;
}

/******************************************************************************/
static void gravityForceXCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float gravityForceX = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_gravityForceX ( &gtk3ped->core,
                                                 gravityForceX );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void gravityForceYCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float gravityForceY = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_gravityForceY ( &gtk3ped->core,
                                                 gravityForceY );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void gravityForceZCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float gravityForceZ = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_gravityForceZ ( &gtk3ped->core,
                                                 gravityForceZ );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}


/******************************************************************************/
static void createForcesPanel ( GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped,
                                gint                          x,
                                gint                          y,
                                gint                          width,
                                gint                          height ) {
    GtkFixed *pan = ui_createTab ( gtk3ped->notebook,
                                     gtk3ped,
                                     EDITPEMITTERFORCES,
                                     CLASS_MAIN,
                                     x, y, width, height );

    gtk3ped->gravityForceXEntry  = ui_createFloatText ( pan,
                                                        gtk3ped,
                                                        EDITPEMITTERGRAVITYX,
                                                        CLASS_MAIN,
                                                        -FLT_MAX, FLT_MAX,
                                                        0, 0, 96, 96, 20,
                                                        gravityForceXCbk );

    gtk3ped->gravityForceYEntry  = ui_createFloatText ( pan,
                                                        gtk3ped,
                                                        EDITPEMITTERGRAVITYY,
                                                        CLASS_MAIN,
                                                        -FLT_MAX, FLT_MAX,
                                                        0, 24, 96, 96, 20,
                                                        gravityForceYCbk );

    gtk3ped->gravityForceZEntry  = ui_createFloatText ( pan,
                                                        gtk3ped,
                                                        EDITPEMITTERGRAVITYZ,
                                                        CLASS_MAIN,
                                                       -FLT_MAX, FLT_MAX,
                                                        0, 48, 96, 96, 20,
                                                        gravityForceZCbk );

}


/******************************************************************************/
static void updateFinalPanel ( GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped ) {
    gtk3ped->core.gui->lock = 0x01;

    if ( gtk3ped->core.editedPEmitter ) {
        gtk_spin_button_set_value  ( gtk3ped->finalAccelXEntry, 
                                     gtk3ped->core.editedPEmitter->finalAccel.x );
        gtk_spin_button_set_value  ( gtk3ped->finalAccelYEntry, 
                                     gtk3ped->core.editedPEmitter->finalAccel.y );
        gtk_spin_button_set_value  ( gtk3ped->finalAccelZEntry, 
                                     gtk3ped->core.editedPEmitter->finalAccel.z );

        gtk_spin_button_set_value  ( gtk3ped->finalSpeedXEntry, 
                                     gtk3ped->core.editedPEmitter->finalSpeed.x );
        gtk_spin_button_set_value  ( gtk3ped->finalSpeedYEntry, 
                                     gtk3ped->core.editedPEmitter->finalSpeed.y );
        gtk_spin_button_set_value  ( gtk3ped->finalSpeedZEntry, 
                                     gtk3ped->core.editedPEmitter->finalSpeed.z );

        gtk_spin_button_set_value  ( gtk3ped->finalScaXEntry, 
                                     gtk3ped->core.editedPEmitter->finalScaling.x );
        gtk_spin_button_set_value  ( gtk3ped->finalScaYEntry, 
                                     gtk3ped->core.editedPEmitter->finalScaling.y );
        gtk_spin_button_set_value  ( gtk3ped->finalScaZEntry, 
                                     gtk3ped->core.editedPEmitter->finalScaling.z );

        gtk_spin_button_set_value  ( gtk3ped->finalRotXEntry, 
                                     gtk3ped->core.editedPEmitter->finalRotation.x );
        gtk_spin_button_set_value  ( gtk3ped->finalRotYEntry, 
                                     gtk3ped->core.editedPEmitter->finalRotation.y );
        gtk_spin_button_set_value  ( gtk3ped->finalRotZEntry, 
                                     gtk3ped->core.editedPEmitter->finalRotation.z );

        gtk_spin_button_set_value  ( gtk3ped->finalTranspEntry, 
                                     gtk3ped->core.editedPEmitter->finalTransparency );
    }

    gtk3ped->core.gui->lock = 0x00;
}

/******************************************************************************/
static void finalTranspCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float finalTransp = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_finalTransp ( &gtk3ped->core,
                                                 finalTransp );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void finalRotXCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float finalRotX = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_finalRotX ( &gtk3ped->core,
                                               finalRotX );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void finalRotYCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float finalRotY = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_finalRotY ( &gtk3ped->core,
                                              finalRotY );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void finalRotZCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float finalRotZ = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_finalRotZ ( &gtk3ped->core,
                                              finalRotZ );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void finalScaXCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float finalScaX = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_finalScaX ( &gtk3ped->core,
                                              finalScaX );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void finalScaYCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float finalScaY = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_finalScaY ( &gtk3ped->core,
                                              finalScaY );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void finalScaZCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float finalScaZ = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_finalScaZ ( &gtk3ped->core,
                                              finalScaZ );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void finalSpeedXCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float finalSpeedX = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_finalSpeedX ( &gtk3ped->core,
                                                finalSpeedX );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void finalSpeedYCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float finalSpeedY = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_finalSpeedY ( &gtk3ped->core,
                                                finalSpeedY );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void finalSpeedZCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float finalSpeedZ = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_finalSpeedZ ( &gtk3ped->core,
                                                finalSpeedZ );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void finalAccelXCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float finalAccelX = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_finalAccelX ( &gtk3ped->core,
                                                finalAccelX );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void finalAccelYCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float finalAccelY = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_finalAccelY ( &gtk3ped->core,
                                                finalAccelY );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void finalAccelZCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float finalAccelZ = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_finalAccelZ ( &gtk3ped->core,
                                                finalAccelZ );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void createFinalPanel ( GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped,
                               gint                          x,
                               gint                          y,
                               gint                          width,
                               gint                          height ) {
    GtkFixed *pan = ui_createTab ( gtk3ped->notebook,
                                     gtk3ped,
                                     EDITPEMITTERFINAL,
                                     CLASS_MAIN,
                                     x, y, width, height );

    gtk3ped->finalTranspEntry  = ui_createFloatText ( pan,
                                                    gtk3ped,
                                                    EDITPEMITTERTRANSPARENCY,
                                                    CLASS_MAIN,
                                                    0.0f, FLT_MAX,
                                                    0, 0, 96, 96, 20,
                                                    finalTranspCbk );

    gtk3ped->finalAccelXEntry  = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERACCELX,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 24, 96, 96, 20,
                                                 finalAccelXCbk );

    gtk3ped->finalAccelYEntry  = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERACCELY,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 48, 96, 96, 20,
                                                 finalAccelYCbk );

    gtk3ped->finalAccelZEntry  = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERACCELZ,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 72, 96, 96, 20,
                                                 finalAccelZCbk );


    gtk3ped->finalSpeedXEntry  = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERSPEEDX,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 120, 96, 96, 20,
                                                 finalSpeedXCbk );

    gtk3ped->finalSpeedYEntry  = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERSPEEDY,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 144, 96, 96, 20,
                                                 finalSpeedYCbk );

    gtk3ped->finalSpeedZEntry  = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERSPEEDZ,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 168, 96, 96, 20,
                                                 finalSpeedZCbk );


    gtk3ped->finalScaXEntry    = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERSCALINGX,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 216, 96, 96, 20,
                                                 finalScaXCbk );

    gtk3ped->finalScaYEntry    = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERSCALINGY,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 240, 96, 96, 20,
                                                 finalScaYCbk );

    gtk3ped->finalScaZEntry    = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERSCALINGZ,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 264, 96, 96, 20,
                                                 finalScaZCbk );

    gtk3ped->finalRotXEntry    = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERROTATIONX,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 312, 96, 96, 20,
                                                 finalRotXCbk );

    gtk3ped->finalRotYEntry    = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERROTATIONY,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 336, 96, 96, 20,
                                                 finalRotYCbk );

    gtk3ped->finalRotZEntry    = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERROTATIONZ,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 360, 96, 96, 20,
                                                 finalRotZCbk );
}

/******************************************************************************/
static void updateInitialPanel ( GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped ) {
    gtk3ped->core.gui->lock = 0x01;

    if ( gtk3ped->core.editedPEmitter ) {
        gtk_spin_button_set_value  ( gtk3ped->initialAccelXEntry, 
                                     gtk3ped->core.editedPEmitter->initialAccel.x );
        gtk_spin_button_set_value  ( gtk3ped->initialAccelYEntry, 
                                     gtk3ped->core.editedPEmitter->initialAccel.y );
        gtk_spin_button_set_value  ( gtk3ped->initialAccelZEntry, 
                                     gtk3ped->core.editedPEmitter->initialAccel.z );

        gtk_spin_button_set_value  ( gtk3ped->initialSpeedXEntry, 
                                     gtk3ped->core.editedPEmitter->initialSpeed.x );
        gtk_spin_button_set_value  ( gtk3ped->initialSpeedYEntry, 
                                     gtk3ped->core.editedPEmitter->initialSpeed.y );
        gtk_spin_button_set_value  ( gtk3ped->initialSpeedZEntry, 
                                     gtk3ped->core.editedPEmitter->initialSpeed.z );

        gtk_spin_button_set_value  ( gtk3ped->initialScaXEntry, 
                                     gtk3ped->core.editedPEmitter->initialScaling.x );
        gtk_spin_button_set_value  ( gtk3ped->initialScaYEntry, 
                                     gtk3ped->core.editedPEmitter->initialScaling.y );
        gtk_spin_button_set_value  ( gtk3ped->initialScaZEntry, 
                                     gtk3ped->core.editedPEmitter->initialScaling.z );

        gtk_spin_button_set_value  ( gtk3ped->initialRotXEntry, 
                                     gtk3ped->core.editedPEmitter->initialRotation.x );
        gtk_spin_button_set_value  ( gtk3ped->initialRotYEntry, 
                                     gtk3ped->core.editedPEmitter->initialRotation.y );
        gtk_spin_button_set_value  ( gtk3ped->initialRotZEntry, 
                                     gtk3ped->core.editedPEmitter->initialRotation.z );

        gtk_spin_button_set_value  ( gtk3ped->initialTranspEntry, 
                                     gtk3ped->core.editedPEmitter->initialTransparency );
    }

    gtk3ped->core.gui->lock = 0x00;
}

/******************************************************************************/
static void initialTranspCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float initialTransp = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_initialTransp ( &gtk3ped->core,
                                                 initialTransp );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void initialRotXCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float initialRotX = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_initialRotX ( &gtk3ped->core,
                                               initialRotX );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void initialRotYCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float initialRotY = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_initialRotY ( &gtk3ped->core,
                                              initialRotY );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void initialRotZCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float initialRotZ = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_initialRotZ ( &gtk3ped->core,
                                              initialRotZ );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void initialScaXCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float initialScaX = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_initialScaX ( &gtk3ped->core,
                                              initialScaX );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void initialScaYCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float initialScaY = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_initialScaY ( &gtk3ped->core,
                                              initialScaY );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void initialScaZCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float initialScaZ = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_initialScaZ ( &gtk3ped->core,
                                              initialScaZ );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void initialSpeedXCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float initialSpeedX = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_initialSpeedX ( &gtk3ped->core,
                                                initialSpeedX );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void initialSpeedYCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float initialSpeedY = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_initialSpeedY ( &gtk3ped->core,
                                                initialSpeedY );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void initialSpeedZCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float initialSpeedZ = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_initialSpeedZ ( &gtk3ped->core,
                                                initialSpeedZ );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void initialAccelXCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float initialAccelX = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_initialAccelX ( &gtk3ped->core,
                                                initialAccelX );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void initialAccelYCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float initialAccelY = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_initialAccelY ( &gtk3ped->core,
                                                initialAccelY );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void initialAccelZCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float initialAccelZ = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_initialAccelZ ( &gtk3ped->core,
                                                initialAccelZ );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void createInitialPanel ( GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped,
                                 gint                          x,
                                 gint                          y,
                                 gint                          width,
                                 gint                          height ) {
    GtkFixed *pan = ui_createTab ( gtk3ped->notebook,
                                     gtk3ped,
                                     EDITPEMITTERINITIAL,
                                     CLASS_MAIN,
                                     x, y, width, height );

    gtk3ped->initialTranspEntry  = ui_createFloatText ( pan,
                                                    gtk3ped,
                                                    EDITPEMITTERTRANSPARENCY,
                                                    CLASS_MAIN,
                                                    0.0f, FLT_MAX,
                                                    0, 0, 96, 96, 20,
                                                    initialTranspCbk );

    gtk3ped->initialAccelXEntry  = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERACCELX,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 24, 96, 96, 20,
                                                 initialAccelXCbk );

    gtk3ped->initialAccelYEntry  = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERACCELY,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 48, 96, 96, 20,
                                                 initialAccelYCbk );

    gtk3ped->initialAccelZEntry  = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERACCELZ,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 72, 96, 96, 20,
                                                 initialAccelZCbk );


    gtk3ped->initialSpeedXEntry  = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERSPEEDX,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 120, 96, 96, 20,
                                                 initialSpeedXCbk );

    gtk3ped->initialSpeedYEntry  = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERSPEEDY,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 144, 96, 96, 20,
                                                 initialSpeedYCbk );

    gtk3ped->initialSpeedZEntry  = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERSPEEDZ,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 168, 96, 96, 20,
                                                 initialSpeedZCbk );


    gtk3ped->initialScaXEntry    = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERSCALINGX,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 216, 96, 96, 20,
                                                 initialScaXCbk );

    gtk3ped->initialScaYEntry    = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERSCALINGY,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 240, 96, 96, 20,
                                                 initialScaYCbk );

    gtk3ped->initialScaZEntry    = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERSCALINGZ,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 264, 96, 96, 20,
                                                 initialScaZCbk );

    gtk3ped->initialRotXEntry    = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERROTATIONX,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 312, 96, 96, 20,
                                                 initialRotXCbk );

    gtk3ped->initialRotYEntry    = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERROTATIONY,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 336, 96, 96, 20,
                                                 initialRotYCbk );

    gtk3ped->initialRotZEntry    = ui_createFloatText ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERROTATIONZ,
                                                 CLASS_MAIN,
                                                 0.0f, FLT_MAX,
                                                 0, 360, 96, 96, 20,
                                                 initialRotZCbk );
}

/******************************************************************************/
static void updateGeneralPanel ( GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped ) {
    gtk3ped->core.gui->lock = 0x01;

    if ( gtk3ped->core.editedPEmitter ) {
        gtk_widget_set_sensitive ( gtk3ped->startAtFrameEntry, TRUE );
        gtk_widget_set_sensitive ( gtk3ped->endAtFrameEntry  , TRUE );
        /*gtk_widget_set_sensitive ( gtk3ped->shapeCombo       , TRUE );*/
        gtk_widget_set_sensitive ( gtk3ped->radiusEntry      , TRUE );
        gtk_widget_set_sensitive ( gtk3ped->lifetimeEntry    , TRUE );
        gtk_widget_set_sensitive ( gtk3ped->ppfEntry         , TRUE );
        gtk_widget_set_sensitive ( gtk3ped->maxPreviewsEntry , TRUE );
        gtk_widget_set_sensitive ( gtk3ped->displayPartToggle, TRUE );

        gtk_spin_button_set_value  ( gtk3ped->startAtFrameEntry, 
                                     gtk3ped->core.editedPEmitter->startAtFrame );

        gtk_spin_button_set_value  ( gtk3ped->endAtFrameEntry, 
                                     gtk3ped->core.editedPEmitter->endAtFrame );

        gtk_spin_button_set_value  ( gtk3ped->radiusEntry, 
                                     gtk3ped->core.editedPEmitter->radius );

        gtk_spin_button_set_value  ( gtk3ped->lifetimeEntry, 
                                     gtk3ped->core.editedPEmitter->particleLifetime );

        gtk_spin_button_set_value  ( gtk3ped->ppfEntry, 
                                     gtk3ped->core.editedPEmitter->particlesPerFrame );

        gtk_spin_button_set_value  ( gtk3ped->maxPreviewsEntry, 
                                     gtk3ped->core.editedPEmitter->maxPreviewsPerFrame );

        if ( gtk3ped->core.editedPEmitter->obj.flags & DISPLAYPARTICLES ) {
            gtk_toggle_button_set_active ( gtk3ped->displayPartToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( gtk3ped->displayPartToggle, FALSE );
        }
    }

    gtk3ped->core.gui->lock = 0x00;
}

/******************************************************************************/
void gtk3_g3duiparticleemitteredit_update ( GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped ) {
    G3DUI *gui = gtk3ped->core.gui;

    gui->lock = 0x01;

    if ( gui->sce ) {
        G3DSCENE *sce = gui->sce;
        uint32_t nbsel = list_count ( sce->lsel );

        if ( nbsel ) {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3ped->notebook), TRUE );

            if ( g3dobjectlist_checkType ( sce->lsel, G3DPARTICLEEMITTERTYPE ) ) {
                gtk3ped->core.multi = ( nbsel > 0x01 ) ? 0x01 : 0x00;

                gtk3ped->core.editedPEmitter = ( G3DPARTICLEEMITTER * ) g3dscene_getLastSelected ( sce );

                if ( gtk3ped->core.editedPEmitter ) {
                    updateGeneralPanel  ( gtk3ped );
                    updateInitialPanel  ( gtk3ped );
                    updateFinalPanel    ( gtk3ped );
                    updateForcesPanel   ( gtk3ped );
                }
            }
        } else {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3ped->notebook), FALSE );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void startAtFrameCbk ( GtkWidget *widget,
                              gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float startAtFrame = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_startAtFrame ( &gtk3ped->core,
                                               startAtFrame );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void endAtFrameCbk ( GtkWidget *widget,
                            gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float endAtFrame = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_endAtFrame ( &gtk3ped->core,
                                              endAtFrame );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void radiusCbk ( GtkWidget *widget,
                        gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float radius = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_radius ( &gtk3ped->core,
                                          radius );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void lifetimeCbk ( GtkWidget *widget,
                          gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float lifetime = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_lifetime ( &gtk3ped->core,
                                            lifetime );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void ppfCbk ( GtkWidget *widget,
                     gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float ppf = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_ppf ( &gtk3ped->core,
                                       ppf );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void maxPreviewsCbk ( GtkWidget *widget,
                             gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float maxPreviews = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_maxPreviews ( &gtk3ped->core,
                                               maxPreviews );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void displayPartCbk  ( GtkWidget *widget, 
                              gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiparticleemitteredit_displayPart ( &gtk3ped->core );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void createGeneralPanel ( GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped,
                                 gint                          x,
                                 gint                          y,
                                 gint                          width,
                                 gint                          height ) {
    GtkFixed *pan = ui_createTab ( gtk3ped->notebook,
                                   gtk3ped,
                                   EDITPEMITTERGENERAL,
                                   CLASS_MAIN,
                                   x, y, width, height );

    gtk3ped->startAtFrameEntry = ui_createIntegerText ( pan,
                                                        gtk3ped,
                                                        EDITPEMITTERSTARTATFRAME,
                                                        CLASS_MAIN,
                                                        INT_MIN, INT_MAX,
                                                        0,   0, 144, 96, 20,
                                                        startAtFrameCbk );

    gtk3ped->endAtFrameEntry   = ui_createIntegerText ( pan,
                                                        gtk3ped,
                                                        EDITPEMITTERENDATFRAME,
                                                        CLASS_MAIN,
                                                        INT_MIN, INT_MAX,
                                                        0,  24, 144, 96, 20,
                                                        endAtFrameCbk );

    gtk3ped->radiusEntry       = ui_createFloatText   ( pan,
                                                        gtk3ped,
                                                        EDITPEMITTERRADIUS,
                                                        CLASS_MAIN,
                                                        0.0f, FLT_MAX,
                                                        0, 48, 144, 96, 20,
                                                        radiusCbk );

    /*ped->shapeCombo        = ui_createFloatText   ( pan,
                                                 gtk3ped,
                                                 EDITPEMITTERSHAPE,
                                                 0.0f, FLT_MAX,
                                                 0, 72, 96, 96,
                                                 spotAngleCbk );*/

    gtk3ped->lifetimeEntry     = ui_createIntegerText ( pan,
                                                        gtk3ped,
                                                        EDITPEMITTERPARTICLELIFETIME,
                                                        CLASS_MAIN,
                                                        0x00, INT_MAX,
                                                        0, 96, 144, 96, 20,
                                                        lifetimeCbk );

    gtk3ped->ppfEntry          = ui_createFloatText   ( pan,
                                                        gtk3ped,
                                                        EDITPEMITTERPARTICLESPERFRAME,
                                                        CLASS_MAIN,
                                                        0.0f, FLT_MAX,
                                                        0, 120, 144, 96, 20,
                                                        ppfCbk );

    gtk3ped->maxPreviewsEntry  = ui_createIntegerText ( pan,
                                                        gtk3ped,
                                                        EDITPEMITTERMAXPREVIEWS,
                                                        CLASS_MAIN,
                                                        0x00, INT_MAX,
                                                        0, 144, 144, 96, 20,
                                                        maxPreviewsCbk );

    gtk3ped->displayPartToggle = ui_createToggleLabel ( pan,
                                                        gtk3ped,
                                                        EDITPEMITTERDISPLAYPARTICULES,
                                                        CLASS_MAIN,
                                                        0, 170, 144, 96, 20,
                                                        displayPartCbk );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;

    free ( gtk3ped );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) user_data;

    gtk3_g3duiparticleemitteredit_update ( gtk3ped );
}

/******************************************************************************/
GTK3G3DUIPARTICLEEMITTEREDIT *gtk3_g3duiparticleemitteredit_create ( GtkWidget *parent,
                                                                     GTK3G3DUI *gtk3gui,
                                                                     char      *name ) {
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3ped = gtk3_g3duiparticleemitteredit_new ( gtk3gui );
    GtkWidget *notebook = gtk_notebook_new ( );

    gtk3ped->notebook = GTK_NOTEBOOK(notebook);

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(notebook), TRUE );

    gtk_widget_set_name ( notebook, name );

    /*gtk_widget_set_size_request ( tab, width, height );*/

    g_signal_connect ( G_OBJECT (notebook), "realize", G_CALLBACK (Realize), gtk3ped );
    g_signal_connect ( G_OBJECT (notebook), "destroy", G_CALLBACK (Destroy), gtk3ped );


    createGeneralPanel    ( gtk3ped, 0, 0, 310, 180 );
    createInitialPanel    ( gtk3ped, 0, 0, 310, 150 );
    createFinalPanel      ( gtk3ped, 0, 0, 310, 150 );
    /*createInitialVarPanel ( gtk3ped, 0, 0, 310, 150 );*/
    createForcesPanel     ( gtk3ped, 0, 0, 310, 150 );


    gtk_widget_show ( notebook );


    return gtk3ped;
}
