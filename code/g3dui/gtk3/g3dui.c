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
typedef struct _GTK3G3DUIMAIN {
    G3DUIMAIN        grp;
    GtkWidget       *layout;
    GtkWidget       *toolBar;
    GtkWidget       *quad;
    GtkWidget       *timeBoard;
    GtkWidget       *board;
} GTK3G3DUIMAIN;

/******************************************************************************/
static GTK3G3DUIMAIN *gtk3g3duimain_new ( ) {
    GTK3G3DUIMAIN *mui = calloc ( 0x01, sizeof ( GTK3G3DUIMAIN ) );

    if ( mui == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );
    }

    return mui; 
}

/******************************************************************************/
static void dispatchGLMenuButton ( G3DUI        *gui, 
                                   G3DMOUSETOOL *mou, 
                                       uint32_t      tool_flags ) {
    uint32_t vertexModeSplineFlags  = ( VERTEXMODETOOL | SPLINETOOL  ),
             vertexModeMeshFlags    = ( VERTEXMODETOOL | MESHTOOL    ),
             edgeModeMeshFlags      = ( EDGEMODETOOL   | MESHTOOL    ),
             faceModeMeshFlags      = ( FACEMODETOOL   | MESHTOOL    ),
             sculptModeMeshFlags    = ( SCULPTMODETOOL | MESHTOOL    ),
             vertexModeMorpherFlags = ( VERTEXMODETOOL | MORPHERTOOL );

    if ( tool_flags & ( OBJECTMODETOOL ) ) {
        gtk3_gui_addMenuListButton ( gui, 
                                     gui->lObjectModeMenu,
                                     mou );
    }

    if ( ( tool_flags & vertexModeMeshFlags ) == vertexModeMeshFlags ) {
        gtk3_gui_addMenuListButton ( gui, 
                                     gui->lVertexModeMeshMenu,
                                     mou );
    }

    if ( ( tool_flags & edgeModeMeshFlags ) == edgeModeMeshFlags ) {
        gtk3_gui_addMenuListButton ( gui,
                                     gui->lEdgeModeMeshMenu,
                                     mou );
    }

    if ( ( tool_flags & faceModeMeshFlags ) == faceModeMeshFlags ) {
        gtk3_gui_addMenuListButton ( gui,
                                     gui->lFaceModeMeshMenu,
                                     mou );
    }

    if ( ( tool_flags & sculptModeMeshFlags ) == sculptModeMeshFlags ) {
        gtk3_gui_addMenuListButton ( gui,
                                     gui->lSculptModeMeshMenu,
                                     mou );
    }

    if ( ( tool_flags & vertexModeSplineFlags ) == vertexModeSplineFlags ) {
        gtk3_gui_addMenuListButton ( gui,
                                     gui->lVertexModeSplineMenu,
                                     mou );
    }

    if ( ( tool_flags & vertexModeMorpherFlags ) == vertexModeMorpherFlags ) {
        gtk3_gui_addMenuListButton ( gui,
                                     gui->lVertexModeMorpherMenu,
                                     mou );
    }
}

/******************************************************************************/
static void interpretMouseToolReturnFlags ( G3DUI   *gui, 
                                            uint32_t msk ) {
    if ( msk & REDRAWVIEW ) {
        gtk3_g3dui_redrawGLViews ( gui );
    }

    if ( msk & REDRAWMATERIALLIST ) {
        gtk3_g3dui_redrawMaterialList ( gui );
    }

    if ( msk & REBUILDMATERIALLIST ) {
        gtk3_g3dui_clearMaterials     ( gui );
        gtk3_g3dui_importMaterials    ( gui );
        gtk3_g3dui_redrawMaterialList ( gui );
    }

    if ( msk & REDRAWUVMAPEDITOR ) {
        gtk3_g3dui_redrawUVMapEditors ( gui );
    }

    if ( msk & REDRAWLIST ) {
        gtk3_g3dui_redrawObjectList ( gui );
        gtk3_g3dui_updateMenuBar    ( gui );
    }

    if ( msk & REDRAWCURRENTOBJECT ) {
        gtk3_g3dui_updateAllCurrentEdit ( gui );
    }

    if ( msk & REDRAWTIMELINE ) {
        gtk3_g3dui_redrawTimeline ( gui );
    }

    if ( msk & REDRAWCOORDS ) {
        gtk3_g3dui_updateCoords ( gui );
    }

    if ( msk & NOBUFFEREDSUBDIVISION ) {
        /*** this should be replace by some MEANINGFUL mask ***/
        gui->engine_flags |= ONGOINGANIMATION;
    }

    if ( msk & BUFFEREDSUBDIVISIONOK ) {
        /*** this should be replace by some MEANINGFUL mask ***/
        gui->engine_flags &= (~ONGOINGANIMATION);
    }
}

/******************************************************************************/
static void SizeAllocate ( GtkWidget     *widget,
                           GtkAllocation *allocation,
                           gpointer       user_data ) {
    GTK3G3DUIMAIN *mui = ( GTK3G3DUIMAIN * ) user_data;
    GdkRectangle gdkrec;

    g3duimain_sizeAllocate ( &mui->grp, 
                              allocation->width, 
                              allocation->height );

    gdkrec.x      = mui->grp.tbarrec.x;
    gdkrec.y      = mui->grp.tbarrec.y;
    gdkrec.width  = mui->grp.tbarrec.width;
    gdkrec.height = mui->grp.tbarrec.height;

    gtk_layout_move ( widget, 
                      mui->toolBar, 
                      gdkrec.x,
                      gdkrec.y );

    gtk_widget_size_allocate ( mui->toolBar, &gdkrec );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMAIN *mui = ( GTK3G3DUIMAIN * ) user_data;

    free ( mui );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMAIN *mui = ( GTK3G3DUIMAIN * ) user_data;
    GtkCssProvider *provider = gtk_css_provider_new ();
    static const gchar *myCSS = { "* {                      \n"
                                  "    background-color: rgba(192,192,192,1); \n"
    #ifdef __linux__

                                  "    font-family: Lucida Sans; \n"
                                  "    font-size: 10px; \n"
    #endif
    #ifdef __MINGW32__
                                  "    font-family: Calibri; \n"
                                  "    font-size: 10px; \n"
    #endif
                                  "}                        \n"
    #ifdef UBUNTU16
                                  "GtkEntry {               \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
                                  "GtkButton {              \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
                                  "GtkSpinButton {          \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     1px; \n"
                                  "    margin-right:   1px; \n"
                                  "    margin-left:    1px; \n"
                                  "    margin-bottom:  1px; \n"
                                  "    padding-top:    1px; \n"
                                  "    padding-right:  1px; \n"
                                  "    padding-left:   1px; \n"
                                  "    padding-bottom: 1px; \n"
                                  "}                        \n"
                                  "GtkQuad {                \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
                                  "GtkMenuItem {\n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     1px; \n"
                                  "    margin-right:   1px; \n"
                                  "    margin-left:    1px; \n"
                                  "    margin-bottom:  1px; \n"
                                  "    padding-top:    2px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 2px; \n"
                                  "    color: rgba(0,0,0,1); \n"
                                  "    font-size:      6; \n"
                                  "}                        \n"
                                  "GtkMenuItem#Option_Menu {\n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     1px; \n"
                                  "    margin-right:   1px; \n"
                                  "    margin-left:    1px; \n"
                                  "    margin-bottom:  1px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "    font-size:      6; \n"
                                  "}                        \n"
                                  "GtkComboBoxText#Shading_Menu {   \n"
                                  "    border-width:   0px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "    font-size:      6; \n"
                                  "}                        \n"
                                  "GtkToolbar {             \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
    #else
                                  "entry {               \n"
                                  "    min-height:     0px; \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
                                  "scale {               \n"
                                  "    min-height:     0px; \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
                                  "tab {               \n"
                                  "    min-height:     0px; \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  8px; \n"
                                  "    padding-left:   8px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
                                  "button {              \n"
                                  "    min-height:     0px; \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
                                  "spinbutton {          \n"
                                  "    min-height:     0px; \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
                                  "quad {                \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
                                  "menuitem {\n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     1px; \n"
                                  "    margin-right:   1px; \n"
                                  "    margin-left:    1px; \n"
                                  "    margin-bottom:  1px; \n"
                                  "    padding-top:    2px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 2px; \n"
                                  "    font-size:      6px; \n"
                                  "}                        \n"
                                  "menuitem#Option_Menu {\n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     1px; \n"
                                  "    margin-right:   1px; \n"
                                  "    margin-left:    1px; \n"
                                  "    margin-bottom:  1px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "    font-size:      6px; \n"
                                  "}                        \n"
                                  "comboboxtext#Shading_Menu {   \n"
                                  "    border-width:   0px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "    font-size:      6px; \n"
                                  "}                        \n"
                                  "toolbar {             \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
    #endif
                                  };
    GdkDisplay *display = gdk_display_get_default ( );
    GdkScreen  *screen  = gdk_display_get_default_screen ( display );
    #ifdef __linux__
    char *home = getenv ( "HOME" );
    #endif
    #ifdef __MINGW32__
    char *home = getenv ( "USERPROFILE" );
    #endif
    char configFileName[0x100];
    GtkWidget *tab;
    char *loadFile = NULL;
    static G3DUI gui;

    #ifdef __linux__
    snprintf ( configFileName, 0x100, "%s/.gloss3d/gloss3d.conf", home );
    #endif
    #ifdef __MINGW32__
    snprintf ( configFileName, 0x100, "%s\\.gloss3d\\gloss3d.conf", home );
    #endif

    gtk_style_context_add_provider_for_screen ( screen,
                                 GTK_STYLE_PROVIDER (provider),
                                 GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    gtk_css_provider_load_from_data ( provider, myCSS, -1, NULL );

    g_object_unref ( provider );

    mui->grp.gui = &gui;

    mui->grp.gui->interpretMouseToolReturnFlags = interpretMouseToolReturnFlags;
    mui->grp.gui->dispatchGLMenuButton = dispatchGLMenuButton;

    g3dui_init ( mui->grp.gui );

    gtk3_g3dui_updateMenuBar ( mui->grp.gui );
}

/******************************************************************************/
GtkWidget *gtk3_g3dui_createMain ( GtkWidget *parent,
                                   char      *name,
                                   gint       x,
                                   gint       y,
                                   gint       width,
                                   gint       height ) {
    GdkRectangle gdkrec = { 0x00, 0x00, width, height };
    GtkWidget *layout = gtk_layout_new ( NULL, NULL );
    GTK3G3DUIMAIN *mui = gtk3g3duimain_new ( );

    g_object_set_data ( G_OBJECT(layout), "private", (gpointer) mui );

    gtk_widget_set_name ( layout, name );

    gtk_widget_size_allocate ( layout, &gdkrec );

    gtk_container_add ( GTK_CONTAINER(parent), layout );

    gtk_widget_add_events(GTK_WIDGET(layout), GDK_CONFIGURE);

    g_signal_connect ( G_OBJECT (layout), "realize"      , G_CALLBACK (Realize)     , mui );
    g_signal_connect ( G_OBJECT (layout), "destroy"      , G_CALLBACK (Destroy)     , mui );
    g_signal_connect ( G_OBJECT (layout), "size-allocate", G_CALLBACK (SizeAllocate), mui );

    mui->layout = layout;

    mui->toolBar = gtk3_g3dui_createToolBar ( layout, 
                                              NULL,
                                              "toolbar",
                                              0,
                                              0,
                                              width,
                                              32 );


    gtk_widget_show ( layout );

    return layout;
}

