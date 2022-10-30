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

#define EDITPICKTOOLVISIBLE        "Only visible"
#define EDITPICKTOOLWEIGHTFRAME    "Weight Painting"
#define EDITPICKTOOLWEIGHTSTRENGTH "Strength"
#define EDITPICKTOOLWEIGHTRADIUS   "Radius"

/******************************************************************************/
static GTK3G3DUIPICKTOOLEDIT *gtk3_g3duipicktooledit_new ( GTK3G3DUI *gtk3gui ) {
    uint32_t memsize = sizeof ( GTK3G3DUIPICKTOOLEDIT );
    GTK3G3DUIPICKTOOLEDIT *gtk3pickedit = calloc ( 0x01, memsize );

    if ( gtk3pickedit == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3pickedit->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3pickedit; 
}

/******************************************************************************/
static void paintRadiusCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIPICKTOOLEDIT *gtk3pickedit = ( GTK3G3DUIPICKTOOLEDIT * ) user_data;
    int radius = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents a loop ***/
    if ( gtk3pickedit->core.gui->lock ) return;

    g3duipicktooledit_paintRadiusCbk ( &gtk3pickedit->core, radius );
}

/******************************************************************************/
static void setWeightCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIPICKTOOLEDIT *gtk3pickedit = ( GTK3G3DUIPICKTOOLEDIT * ) user_data;
    float weight = ( float ) gtk_range_get_value ( GTK_RANGE(widget) );

    /*** prevents a loop ***/
    if ( gtk3pickedit->core.gui->lock ) return;

    g3duipicktooledit_setWeightCbk ( &gtk3pickedit->core, weight / 100.0f );
}

/******************************************************************************/
static void onlyVisibleCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIPICKTOOLEDIT *gtk3pickedit = ( GTK3G3DUIPICKTOOLEDIT * ) user_data;
    int visible_only = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    /*** prevents a loop ***/
    if ( gtk3pickedit->core.gui->lock ) return;

    g3duipicktooledit_onlyVisibleCbk ( &gtk3pickedit->core, visible_only );
}

/******************************************************************************/
void gtk3_g3duipicktooledit_update ( GTK3G3DUIPICKTOOLEDIT *gtk3pickedit ) {
    G3DUI *gui = gtk3pickedit->core.gui;
    G3DUIMOUSETOOL *mtl = gui->curmou;

    /*** prevent a loop ***/
    gtk3pickedit->core.gui->lock = 0x01;

    if ( mtl ) {
        G3DMOUSETOOLPICK *pt = ( G3DMOUSETOOLPICK * ) mtl->tool;

        if ( pt->only_visible ) {
            gtk_toggle_button_set_active ( gtk3pickedit->visibleToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( gtk3pickedit->visibleToggle, FALSE );
        }

        if ( ( strcmp ( pt->tool.name, MOVETOOL     ) == 0x00 ) ||
             ( strcmp ( pt->tool.name, ROTATETOOL   ) == 0x00 ) ||
             ( strcmp ( pt->tool.name, SCALETOOL    ) == 0x00 ) ||
             ( strcmp ( pt->tool.name, PICKUVTOOL   ) == 0x00 ) ||
             ( strcmp ( pt->tool.name, MOVEUVTOOL   ) == 0x00 ) ||
             ( strcmp ( pt->tool.name, ROTATEUVTOOL ) == 0x00 ) ||
             ( strcmp ( pt->tool.name, SCALEUVTOOL  ) == 0x00 ) ) {
            gtk_widget_hide ( gtk_widget_get_parent ( gtk3pickedit->weightFrame ) );
        } else {
            gtk_widget_show ( gtk_widget_get_parent ( gtk3pickedit->weightFrame ) );

            if ( gui->engine_flags & VIEWSKIN ) {
                gtk_widget_set_sensitive ( gtk3pickedit->weightFrame, TRUE  );
            } else {
                gtk_widget_set_sensitive ( gtk3pickedit->weightFrame, FALSE );
            }

            gtk_range_set_value ( gtk3pickedit->weightStrengthScale, ( pt->weight * 100.0f ) );

            gtk_spin_button_set_value ( gtk3pickedit->weightRadiusEntry, pt->radius );
        }
    }

    gtk3pickedit->core.gui->lock = 0x00;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIPICKTOOLEDIT *gtk3pickedit = ( GTK3G3DUIPICKTOOLEDIT * ) user_data;

    free ( gtk3pickedit );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIPICKTOOLEDIT *gtk3pickedit = ( GTK3G3DUIPICKTOOLEDIT * ) user_data;

    gtk3_g3duipicktooledit_update ( gtk3pickedit );
}

/******************************************************************************/
GTK3G3DUIPICKTOOLEDIT *gtk3_g3duipicktooledit_create ( GtkWidget *parent, 
                                                       GTK3G3DUI *gtk3gui,
                                                       char      *name ) {
    GTK3G3DUIPICKTOOLEDIT *gtk3pickedit = gtk3_g3duipicktooledit_new ( gtk3gui );

    GtkWidget *fixed = ui_gtk_fixed_new ( CLASS_MAIN );
    GtkWidget *weightFixed = ui_gtk_fixed_new ( CLASS_MAIN );

    gtk3pickedit->fixed = fixed;

    gtk_widget_set_size_request ( gtk3pickedit->fixed, 310, 150 );

    g_signal_connect ( G_OBJECT (fixed), "realize", G_CALLBACK (Realize), gtk3pickedit );
    g_signal_connect ( G_OBJECT (fixed), "destroy", G_CALLBACK (Destroy), gtk3pickedit );

    gtk3pickedit->visibleToggle       = ui_createToggleLabel ( fixed, 
                                                               gtk3pickedit,
                                                               EDITPICKTOOLVISIBLE,
                                                               CLASS_MAIN,
                                                               0, 0, 96, 96, 20,
                                                               onlyVisibleCbk );

    gtk3pickedit->weightFrame         = ui_gtk_frame_new ( CLASS_MAIN,
                                                           EDITPICKTOOLWEIGHTFRAME );

    gtk_fixed_put ( fixed, gtk3pickedit->weightFrame, 0, 24 );
    gtk_container_add ( gtk3pickedit->weightFrame, weightFixed );

    gtk3pickedit->weightStrengthScale = ui_createHorizontalScale ( weightFixed,
                                                                   gtk3pickedit, 
                                                                   EDITPICKTOOLWEIGHTSTRENGTH, 
                                                                   CLASS_MAIN,
                                                                   0, 0, 96, 200, 20,
                                                                   0.0f, 100.0f, 1.0f,
                                                                   setWeightCbk );

    gtk3pickedit->weightRadiusEntry   = ui_createIntegerText ( weightFixed, 
                                                               gtk3pickedit, 
                                                               EDITPICKTOOLWEIGHTRADIUS, 
                                                               CLASS_MAIN,
                                                               0, 256,
                                                               0, 24, 96, 96, 20,
                                                               paintRadiusCbk );


    gtk_widget_show ( gtk3pickedit->weightFrame );
    gtk_widget_show ( weightFixed );
    gtk_widget_show ( fixed );


    return gtk3pickedit;
}
