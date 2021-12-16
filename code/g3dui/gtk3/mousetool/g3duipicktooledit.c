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
typedef struct _G3DUIPICKTOOLEDIT {
    G3DUIWIDGETGROUP grp;
    GtkWidget       *visibleToggle;
    GtkWidget       *weightStrengthScale;
    GtkWidget       *weightRadiusEntry;
    GtkWidget       *weightFrame;
} G3DUIPICKTOOLEDIT;

/******************************************************************************/
static G3DUIPICKTOOLEDIT *g3duipicktooledit_new ( G3DUI *gui ) {
    G3DUIPICKTOOLEDIT *pte = calloc ( 0x01, sizeof ( G3DUIPICKTOOLEDIT ) );

    if ( pte == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );
    }

    pte->grp.gui = gui;

    return pte; 
}

/******************************************************************************/
static void paintRadiusCbk  ( GtkWidget *widget, gpointer user_data ) {
    int radius = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUIPICKTOOLEDIT *pte = ( G3DUIPICKTOOLEDIT * ) user_data;

    common_g3duipicktooledit_paintRadiusCbk ( pte->grp.gui, radius );
}

/******************************************************************************/
static void setWeightCbk  ( GtkWidget *widget, gpointer user_data ) {
    float weight = ( float ) gtk_range_get_value ( GTK_RANGE(widget) );
    G3DUIPICKTOOLEDIT *pte = ( G3DUIPICKTOOLEDIT * ) user_data;

    common_g3duipicktooledit_setWeightCbk ( pte->grp.gui, weight / 100.0f );
}

/******************************************************************************/
static void onlyVisibleCbk  ( GtkWidget *widget, gpointer user_data ) {
    int visible_only = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    G3DUIPICKTOOLEDIT *pte = ( G3DUIPICKTOOLEDIT * ) user_data;

    common_g3duipicktooledit_onlyVisibleCbk ( pte->grp.gui, visible_only );
}

/******************************************************************************/
void updatePickToolEdit ( GtkWidget    *widget, 
                          G3DMOUSETOOL *tool ) {
    G3DUIPICKTOOLEDIT *pte = ( G3DUIPICKTOOLEDIT * ) 
                                               g_object_get_data ( G_OBJECT(widget),
                                                                   "private" );
    /*** prevent a loop ***/
    pte->grp.gui->lock = 0x01;

    if ( tool ) {
        G3DMOUSETOOLPICK *pt = ( G3DMOUSETOOLPICK * ) tool;

        if ( pt->only_visible ) {
            gtk_toggle_button_set_active ( pte->visibleToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( pte->visibleToggle, FALSE );
        }

        if ( ( strcmp ( tool->name, MOVETOOL     ) == 0x00 ) ||
             ( strcmp ( tool->name, ROTATETOOL   ) == 0x00 ) ||
             ( strcmp ( tool->name, SCALETOOL    ) == 0x00 ) ||
             ( strcmp ( tool->name, PICKUVTOOL   ) == 0x00 ) ||
             ( strcmp ( tool->name, MOVEUVTOOL   ) == 0x00 ) ||
             ( strcmp ( tool->name, ROTATEUVTOOL ) == 0x00 ) ||
             ( strcmp ( tool->name, SCALEUVTOOL  ) == 0x00 ) ) {
            gtk_widget_hide ( gtk_widget_get_parent ( pte->weightFrame ) );
        } else {
            gtk_widget_show ( gtk_widget_get_parent ( pte->weightFrame ) );

            if ( pte->grp.gui->engine_flags & VIEWSKIN ) {
                gtk_widget_set_sensitive ( pte->weightFrame, TRUE  );
            } else {
                gtk_widget_set_sensitive ( pte->weightFrame, FALSE );
            }

            gtk_range_set_value ( pte->weightStrengthScale, ( pt->weight * 100.0f ) );

            gtk_spin_button_set_value ( pte->weightRadiusEntry, pt->radius );
        }
    }

    pte->grp.gui->lock = 0x00;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    G3DUIPICKTOOLEDIT *pte = ( G3DUIPICKTOOLEDIT * ) user_data;

    free ( pte );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    updatePickToolEdit ( widget, NULL );
}

/******************************************************************************/
GtkWidget *createPickToolEdit ( GtkWidget *parent, 
                                G3DUI     *gui,
                                char      *name,
                                gint       x,
                                gint       y,
                                gint       width,
                                gint       height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *frm = gtk_fixed_new ( );
    G3DUIPICKTOOLEDIT *pte = g3duipicktooledit_new ( gui );
    GtkWidget *ptf;

    g_object_set_data ( G_OBJECT(frm), "private", (gpointer) pte );

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), frm, x, y );

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), gui );
    g_signal_connect ( G_OBJECT (frm), "destroy", G_CALLBACK (Destroy), gui );

    pte->visibleToggle = createToggleLabel ( frm, 
                                             pte,
                                             EDITPICKTOOLVISIBLE,
                                             0,
                                             0,
                                             96,
                                             32,
                                             onlyVisibleCbk );

    pte->weightFrame = createFrame ( frm, 
                                     pte, 
                                     EDITPICKTOOLWEIGHTFRAME,
                                     0,
                                     32,
                                     276,
                                     64 );

    pte->weightStrengthScale = createHorizontalScale ( pte->weightFrame,
                                                       pte, 
                                                       EDITPICKTOOLWEIGHTSTRENGTH, 
                                                       0,
                                                       0, 
                                                       256,
                                                       32,
                                                       0.0f,
                                                       100.0f,
                                                       1.0f,
                                                       setWeightCbk );

    pte->weightRadiusEntry = createIntegerText ( pte->weightFrame, 
                                                 pte, 
                                                 EDITPICKTOOLWEIGHTRADIUS, 
                                                 0,                                                       
                                                 256,
                                                 0,
                                                 32,
                                                 96,
                                                 32,
                                                 paintRadiusCbk );


    gtk_widget_show ( frm );


    return frm;
}
