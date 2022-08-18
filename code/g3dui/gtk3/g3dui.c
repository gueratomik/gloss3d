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

static void gtk3_updateAllCurrentMouseTools ( );

/******************************************************************************/
GTK3G3DUI *gtk3_getUI ( ) {
    static GTK3G3DUI gtk3gui;

    return &gtk3gui;
}

/******************************************************************************/
void g3duirectangle_toGdkRec ( G3DUIRECTANGLE *in, 
                               GdkRectangle   *out  ) {
    out->x      = in->x;
    out->y      = in->y;
    out->width  = in->width;
    out->height = in->height;
}

/******************************************************************************/
static void menuItemCallback ( GtkWidget *widget, 
                               gpointer   user_data ) {
    GTK3G3DUIMENU *gtk3node = ( GTK3G3DUIMENU * ) user_data;

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( gtk3node->node->callback ) {
        uint64_t ret = gtk3node->node->callback ( gtk3node->node, 
                                                  gtk3node->data );

        gtk3_interpretUIReturnFlags ( ret );
    }
}

/******************************************************************************/
GTK3G3DUIMENU *gtk3_parseMenu_r ( G3DUIMENU *node, 
                                  void      *data ) {
    uint32_t structSize = sizeof ( GTK3G3DUIMENU ); 
    GTK3G3DUIMENU *gtk3node = ( GTK3G3DUIMENU * ) calloc ( 0x01, structSize );
    GTK3G3DUI *gtk3gui = gtk3_getUI ( );
    G3DUI *gui = ( G3DUI * ) gtk3gui;

    gtk3node->node      = node;
    gtk3node->data      = data;
    gtk3node->node->gui = gui;

    switch ( node->type ) {
        case G3DUIMENUTYPE_SEPARATOR :
            gtk3node->item = gtk_separator_menu_item_new ( );
        break;

        case G3DUIMENUTYPE_TOGGLEBUTTON :
            gtk3node->item = gtk_check_menu_item_new_with_mnemonic ( node->name );

            if ( node->callback ) {
                g_signal_connect ( G_OBJECT ( gtk3node->item ), 
                                   "toggled", 
                                   menuItemCallback, 
                                   gtk3node );
            }
        break;

        case G3DUIMENUTYPE_PUSHBUTTON :
            node->item = gtk_menu_item_new_with_mnemonic ( node->name );

            if ( node->callback ) {
                g_signal_connect ( G_OBJECT ( gtk3node->item ), 
                                   "activate", 
                                   menuItemCallback, 
                                   gtk3node );
            }
        break;

        case G3DUIMENUTYPE_MENUBAR : {
            uint32_t i = 0x00;

            gtk3node->menu = gtk_menu_bar_new ( );

            gtk_widget_set_name ( gtk3node->menu, node->name );

            while ( node->nodes[i] != NULL ) {
                GTK3G3DUIMENU *child = gtk3_parseMenu_r ( node->nodes[i] );

                list_insert ( &gtk3node->lchildren, child );

                i++;
            }

            gtk_widget_show ( gtk3node->menu );
        } break;

        case G3DUIMENUTYPE_SUBMENU : {
            uint32_t i = 0x00;

            gtk3node->menu = gtk_menu_new ( );

            gtk_widget_set_name ( gtk3node->menu, gtk3node->name );

            gtk3node->item = gtk_menu_item_new_with_mnemonic ( node->name );

            gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( gtk3node->item ), gtk3node->menu );

            while ( node->nodes[i] != NULL ) {
                GTK3G3DUIMENU *child = gtk3_parseMenu_r ( node->nodes[i] );

                gtk_menu_shell_append ( GTK_MENU_SHELL ( gtk3node->menu ), 
                                        child->item );

                list_insert ( &gtk3node->lchildren, child );

                i++;
            }

            gtk_widget_show ( gtk3node->menu );

            gtk_widget_set_size_request ( gtk3node->menu, 0x60, 24 );
        } break;

        default :
        break;
    }

    if ( node->type == G3DUIMENUTYPE_SUBMENU  ) {
        int height = gtk_widget_get_allocated_height ( gtk3node->item );
        GdkRectangle gdkrec = { 0, 0, 0x60, height };

        /*gtk_widget_set_halign ( node->item, GTK_ALIGN_CENTER );*/

    /*gtk_widget_size_allocate ( node->item, &gdkrec );*/

        gtk_widget_set_size_request ( gtk3node->item, 0x60, height );
    }

    /*gtk_widget_set_name ( gtk3node->item, node->name );*/
    gtk_widget_show ( gtk3node->item );


    return gtk3node;
}

/******************************************************************************/
void gtk3_updateMenu_r ( GTK3G3DUIMENU *gtk3node,
                         G3DUI         *gui ) {
    G3DUIMENU *node = ( G3DUIMENU * ) gtk3node;

    switch ( node->type ) {
        case G3DUIMENUTYPE_SEPARATOR :
        break;

        case G3DUIMENUTYPE_TOGGLEBUTTON :

        break;

        case G3DUIMENUTYPE_PUSHBUTTON :
        break;

        case G3DUIMENUTYPE_MENUBAR :
        case G3DUIMENUTYPE_SUBMENU : {
            uint32_t i = 0x00;

            while ( node->nodes[i] != NULL ) {
                updateMenu_r ( ( GTK3G3DUIMENU * ) node->nodes[i], gui );

                i++;
            }
        } break;

        default :
        break;
    }

    if ( node->item ) {
        if ( node->condition ) {
            if ( node->condition ( gui ) == 0x00 ) {
                gtk_widget_set_sensitive ( gtk3node->item, FALSE );
            } else {
                gtk_widget_set_sensitive ( gtk3node->item, TRUE  );
            }
        } else {
            gtk_widget_set_sensitive ( gtk3node->item, TRUE  );
        }
    }
}

/******************************************************************************/
void gtk3_setMouseTool ( GtkWidget *widget, 
                         gpointer   user_data ) {
    GTK3G3DUI *gtk3gui = gtk3_getUI ( );
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    const char  *name = gtk_widget_get_name ( widget );
    G3DMOUSETOOL *mou = g3dui_getMouseTool ( gui, name );
    G3DCAMERA *cam = g3dui_getCurrentViewCamera ( gui );

    if ( gui->lock ) return;

    if ( mou ) {
        g3dui_setMouseTool ( gui, cam, mou );

        if ( ( mou->flags & MOUSETOOLNOCURRENT ) == 0x00 ) {
            /*** Remember that widget ID, for example to be unset when a toggle button 
            from another parent widget is called (because XmNradioBehavior won't talk
            to other parent widget ***/
            if ( gtk3gui->curmou && widget != gtk3gui->curmou ) {
                gui->lock = 0x01;

                if ( GTK_IS_TOGGLE_TOOL_BUTTON ( gtk3gui->curmou ) ) {
                    GtkToggleToolButton *ttb = GTK_TOGGLE_TOOL_BUTTON(gtk3gui->curmou);

                    gtk_toggle_tool_button_set_active ( ttb, FALSE );
                }

                gui->lock = 0x00;
                /*XtVaSetValues ( ggt->curmou, XmNset, False, NULL );*/
            }

            gtk3gui->curmou = widget;

           gtk3_updateAllCurrentMouseTools ( gui );
        }
    } else {
        fprintf ( stderr, "No such mousetool %s\n", name );
    }
}

/******************************************************************************/
void gtk3_setHourGlass ( ) {
    GTK3G3DUI *gtk3gui = gtk3_getUI ( );
    G3DUI *gui = ( G3DUI * ) gtk3gui;

    /*GtkWindow  *win  = gtk_widget_get_parent(ggt->top);
    GtkWidget  *foc  = gtk_window_get_focus ( win );
    GdkWindow  *pwin = gtk_widget_get_parent_window ( foc );
    GdkDisplay *dis  = gdk_window_get_display ( pwin );*/

    GdkDisplay       *dis = gdk_display_get_default();
    GdkDeviceManager *mgr = gdk_display_get_device_manager ( dis );
    GdkDevice        *dev = gdk_device_manager_get_client_pointer ( mgr );
    GdkCursor        *cur = gdk_cursor_new_for_display ( dis, GDK_WATCH );
    gint x, y;

    gtk3gui->winAtPosition = gdk_device_get_window_at_position ( dev, &x, &y ); 
    /* set watch cursor */

#ifdef unused
    gdk_device_grab ( dev,
                      ggt->winAtPosition,
                      GDK_OWNERSHIP_WINDOW,
                      FALSE,
                      0x00,
                      cur,
                      GDK_CURRENT_TIME );
#endif
    gdk_window_set_cursor ( gtk3gui->winAtPosition, cur );
    gdk_display_sync ( dis );
    gdk_cursor_unref ( cur );
    /** must flush **/
    gdk_flush ( );

}

/******************************************************************************/
void gtk3_unsetHourGlass ( ) {
    GTK3G3DUI *gtk3gui = gtk3_getUI ( );
    G3DUI *gui = ( G3DUI * ) gtk3gui;

    /*GdkDisplay       *dis = gdk_display_get_default();
    GdkDeviceManager *mgr = gdk_display_get_device_manager ( dis );
    GdkDevice        *dev = gdk_device_manager_get_client_pointer ( mgr );
    GdkCursor        *cur = gdk_cursor_new_for_display ( dis, GDK_WATCH );
    gint x, y;

    ggt->winAtPosition = gdk_device_get_window_at_position ( dev, &x, &y ); */
    /* return to normal */
    gdk_window_set_cursor ( gtk3gui->winAtPosition, NULL );

#ifdef unused
    gdk_device_ungrab ( dev, GDK_CURRENT_TIME );
#endif
}

/******************************************************************************/
static void gtk3_updateAllCurrentMouseTools ( ) {
    GTK3G3DUI *gtk3gui = gtk3_getUI ( );
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    LIST *ltmpmtools = gtk3gui->lmtools;

    while ( ltmpmtools ) {
        GtkWidget *mtool = ( GtkWidget * ) ltmpmtools->data;

        updateCurrentMouseTool ( mtool, gui );


        ltmpmtools = ltmpmtools->next;
    }
}

/******************************************************************************/
static void gtk3_updateKeyEdit ( ) {
    GTK3G3DUI *gtk3gui = gtk3_getUI ( );
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    LIST *ltmpkeyedit = gtk3gui->lkeyedit;

    while ( ltmpkeyedit ) {
        GtkWidget *keyedit = ( GtkWidget * ) ltmpkeyedit->data;

        updateKeyEdit ( keyedit, gui );


        ltmpkeyedit = ltmpkeyedit->next;
    }
}

/******************************************************************************/
static void gtk3_updateSelectedMaterialPreview ( ) {
    GTK3G3DUI *gtk3gui = gtk3_getUI ( );
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    LIST *ltmpmatlist = gtk3gui->lmatlist;
    G3DMATERIAL *mat = gui->selmat;

    if ( mat ) {
        while ( ltmpmatlist ) {
            GtkWidget *matlst = ( GtkWidget * ) ltmpmatlist->data;

            g3duimateriallist_updatePreview ( matlst, mat );


            ltmpmatlist = ltmpmatlist->next;
        }
    }
}

/******************************************************************************/
static void gtk3_clearMaterials ( ) {
    GTK3G3DUI *gtk3gui = gtk3_getUI ( );
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    LIST *ltmpmatlist = gtk3gui->lmatlist;

    while ( ltmpmatlist ) {
        GtkWidget *matlst = ( GtkWidget * ) ltmpmatlist->data;

        g3duimateriallist_removeAllPreviews ( matlst );


        ltmpmatlist = ltmpmatlist->next;
    }
}

/******************************************************************************/
static void gtk3_importMaterials ( ) {
    GTK3G3DUI *gtk3gui = gtk3_getUI ( );
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    LIST *ltmpmatlist = gtk3gui->lmatlist;

    while ( ltmpmatlist ) {
        GtkWidget *matlst = ( GtkWidget * ) ltmpmatlist->data;

        g3duimateriallist_importFromScene ( matlst, gui->sce );


        ltmpmatlist = ltmpmatlist->next;
    }
}

/******************************************************************************/
static void gtk3_resizeUVMapEditors ( ) {
    GTK3G3DUI *gtk3gui = gtk3_getUI ( );
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    LIST *ltmp = gtk3gui->luvmapeditor;

    while ( ltmp ) {
        GtkUVMapEditor *guv = ( GtkUVMapEditor * ) ltmp->data;

        /*** resize buffers ***/
        common_m3dui_resizeBuffers ( &guv->mui );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
static void gtk3_redrawUVMapEditors ( ) {
    GTK3G3DUI *gtk3gui = gtk3_getUI ( );
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    LIST *ltmp = gtk3gui->luvmapeditor;

    while ( ltmp ) {
        GtkWidget *guv = ( GtkWidget * ) ltmp->data;
        GtkWidget *area   = gtk_uvmapeditor_getGLArea ( guv );
        GdkRectangle arec;

        arec.x = arec.y = 0x00;
        arec.width = arec.height = 0x01;

        gdk_window_invalidate_rect ( gtk_widget_get_window ( area ), &arec, FALSE );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
static void gtk3_updateGLViewsMenu ( ) {
    GTK3G3DUI *gtk3gui = gtk3_getUI ( );
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    LIST *ltmp = gtk3gui->lglview;

    while ( ltmp ) {
        GtkWidget *glview = ( GtkWidget * ) ltmp->data;

        gtk_view_updateMenu ( glview );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
static void redrawGLViews ( ) {
    GTK3G3DUI *gtk3gui = gtk3_getUI ( );
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    LIST *ltmp = gtk3gui->lglview;

    while ( ltmp ) {
        GtkWidget *glview = ( GtkWidget * ) ltmp->data;
        GtkWidget *area   = gtk_view_getGLArea ( glview );
        GdkRectangle arec;

        arec.x = arec.y = 0x00;
        arec.width = arec.height = 0x01;

        gdk_window_invalidate_rect ( gtk_widget_get_window ( area ), &arec, FALSE );


        ltmp = ltmp->next;
    }
}

/******************************************************************************/
static void dispatchGLMenuButton ( G3DMOUSETOOL *mou, 
                                   uint32_t      tool_flags ) {
    GTK3G3DUI *gtk3gui = gtk3_getUI ( );
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    uint32_t vertexModeSplineFlags  = ( VERTEXMODETOOL | SPLINETOOL  ),
             vertexModeMeshFlags    = ( VERTEXMODETOOL | MESHTOOL    ),
             edgeModeMeshFlags      = ( EDGEMODETOOL   | MESHTOOL    ),
             faceModeMeshFlags      = ( FACEMODETOOL   | MESHTOOL    ),
             sculptModeMeshFlags    = ( SCULPTMODETOOL | MESHTOOL    ),
             vertexModeMorpherFlags = ( VERTEXMODETOOL | MORPHERTOOL );

    if ( tool_flags & ( OBJECTMODETOOL ) ) {
        gtk3_addMenuListButton ( gui, 
                                 gtk3gui->lObjectModeMenu,
                                 mou );
    }

    if ( ( tool_flags & vertexModeMeshFlags ) == vertexModeMeshFlags ) {
        gtk3_addMenuListButton ( gui, 
                                 gtk3gui->lVertexModeMeshMenu,
                                 mou );
    }

    if ( ( tool_flags & edgeModeMeshFlags ) == edgeModeMeshFlags ) {
        gtk3_addMenuListButton ( gui,
                                 gtk3gui->lEdgeModeMeshMenu,
                                 mou );
    }

    if ( ( tool_flags & faceModeMeshFlags ) == faceModeMeshFlags ) {
        gtk3_addMenuListButton ( gui,
                                 gtk3gui->lFaceModeMeshMenu,
                                 mou );
    }

    if ( ( tool_flags & sculptModeMeshFlags ) == sculptModeMeshFlags ) {
        gtk3_addMenuListButton ( gui,
                                 gtk3gui->lSculptModeMeshMenu,
                                 mou );
    }

    if ( ( tool_flags & vertexModeSplineFlags ) == vertexModeSplineFlags ) {
        gtk3_addMenuListButton ( gui,
                                 gtk3gui->lVertexModeSplineMenu,
                                 mou );
    }

    if ( ( tool_flags & vertexModeMorpherFlags ) == vertexModeMorpherFlags ) {
        gtk3_addMenuListButton ( gui,
                                 gtk3gui->lVertexModeMorpherMenu,
                                 mou );
    }
}

/******************************************************************************/
void gtk3_interpretUIReturnFlags ( uint64_t msk ) {
    GTK3G3DUI *gtk3gui = gtk3_getUI ( );
    G3DUI *gui = ( G3DUI * ) gtk3gui;

    if ( msk & REDRAWVIEW ) {
        gtk3_redrawGLViews ( );
    }

    if ( msk & REDRAWVIEWMENU ) {
        gtk3_updateGLViewsMenu ( );
    }

    if ( msk & REDRAWCURRENTMATERIAL ) {
        gtk3_updateMaterialEdit ( );
    }

    if ( msk & REDRAWMATERIALLIST ) {
        gtk3_redrawMaterialList ( );
    }

    if ( msk & REBUILDMATERIALLIST ) {
        gtk3_clearMaterials     ( );
        gtk3_importMaterials    ( );
        gtk3_redrawMaterialList ( );
    }

    if ( msk & REDRAWUVMAPEDITOR ) {
        gtk3_redrawUVMapEditors ( );
    }

    if ( msk & REDRAWLIST ) {
        gtk3_redrawObjectList ( );
        gtk3_updateMenuBar    ( );
    }

    if ( msk & REDRAWCURRENTOBJECT ) {
        gtk3_updateAllCurrentEdit ( );
    }

    if ( msk & REDRAWTIMELINE ) {
        gtk3_redrawTimeline ( );
    }

    if ( msk & REDRAWCOORDS ) {
        gtk3_updateCoords ( );
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
void gtk3_initDefaultMouseTools ( G3DCAMERA *cam ) {
    GTK3G3DUI *gtk3gui = gtk3_getUI ( );
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    G3DMOUSETOOL *mou;

    /********************************/

    mou = ( G3DMOUSETOOL * ) g3dmousetoolpick_new ( );

    g3dui_addMouseTool ( gui, mou );

    /*** Pick is the default mouse tool ***/
    gtk3_interpretUIReturnFlags ( g3dui_setMouseTool ( gui, 
                                                       cam, 
                                                       mou ) );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolcreatesphere_new ( ) );
    dispatchGLMenuButton (      mou, OBJECTMODETOOL |
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolcreatecube_new ( ) );
    dispatchGLMenuButton (      mou, OBJECTMODETOOL | 
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolcreateplane_new ( ) );
    dispatchGLMenuButton (      mou, OBJECTMODETOOL | 
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolcreatecylinder_new ( ) );
    dispatchGLMenuButton (      mou, OBJECTMODETOOL | 
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolcreatetube_new ( ) );
    dispatchGLMenuButton (      mou, OBJECTMODETOOL | 
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolcreatetorus_new ( ) );
    dispatchGLMenuButton (      mou, OBJECTMODETOOL | 
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolcreatebone_new ( ) );
    dispatchGLMenuButton (      mou, OBJECTMODETOOL | 
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolmakeeditable_new ( ) );
    dispatchGLMenuButton (      mou, OBJECTMODETOOL |
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolcutmesh_new ( ) );
    dispatchGLMenuButton (      mou, VERTEXMODETOOL | 
                                     FACEMODETOOL   | 
                                     EDGEMODETOOL   |
                                     MESHTOOL       |
                                     SPLINETOOL     |
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolcreatevertex_new ( ) );
    dispatchGLMenuButton (      mou, VERTEXMODETOOL |
                                     MESHTOOL       |
                                     SPLINETOOL     |
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolrevertspline_new ( ) );
    dispatchGLMenuButton (      mou, VERTEXMODETOOL |
                                     SPLINETOOL     |
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolremovevertexpose_new ( ) );
    dispatchGLMenuButton (      mou, VERTEXMODETOOL |
                                     MORPHERTOOL     |
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolbridge_new ( ) ); 
    dispatchGLMenuButton (      mou, VERTEXMODETOOL |
                                     MESHTOOL       |
                                     SPLINETOOL     |
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolextrudeface_new ( ) );
    dispatchGLMenuButton (      mou, FACEMODETOOL | 
                                     MESHTOOL     |
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolextrudeinner_new ( ) );
    dispatchGLMenuButton (      mou, FACEMODETOOL | 
                                     MESHTOOL     |
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetooluntriangulate_new ( ) );
    dispatchGLMenuButton (      mou, FACEMODETOOL |
                                     MESHTOOL     | 
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetooltriangulate_new ( ) );
    dispatchGLMenuButton (      mou, FACEMODETOOL |
                                     MESHTOOL     | 
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolroundsplinepoint_new ( ) );
    dispatchGLMenuButton (      mou, VERTEXMODETOOL |
                                     SPLINETOOL     | 
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolweldvertices_new ( ) );
    dispatchGLMenuButton (      mou, VERTEXMODETOOL |
                                     MESHTOOL     | 
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolweldneighbours_new ( ) );
    dispatchGLMenuButton (      mou, VERTEXMODETOOL |
                                     MESHTOOL     | 
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolinvertnormal_new ( ) );
    dispatchGLMenuButton (      mou, FACEMODETOOL |
                                     MESHTOOL     |
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolcreatefacegroup_new ( ) ); 
    dispatchGLMenuButton (      mou, FACEMODETOOL | 
                                     MESHTOOL     |
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolsculpt_new ( SCULPTINFLATE ) ); 
    dispatchGLMenuButton (      mou, SCULPTMODETOOL | 
                                     MESHTOOL     |
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolsculpt_new ( SCULPTCREASE ) ); 
    dispatchGLMenuButton (      mou, SCULPTMODETOOL | 
                                     MESHTOOL     |
                                     GLMENUTOOL );

    /********************************/

    /*common_g3dui_addMouseTool ( gui,
                                g3dmousetoolsculpt_new ( SCULPTFLATTEN ), 
                                SCULPTMODETOOL | 
                                MESHTOOL     |
                                GLMENUTOOL );*/

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolsculpt_new ( SCULPTUNSCULPT ) ); 
    dispatchGLMenuButton (      mou, SCULPTMODETOOL | 
                                     MESHTOOL     |
                                     GLMENUTOOL );

    /********************************/

    /*common_g3dui_addMouseTool ( gui,
                                g3dmousetoolsculpt_new ( SCULPTSMOOTH ), 
                                SCULPTMODETOOL | 
                                MESHTOOL     |
                                GLMENUTOOL );*/

    /********************************/

    g3dui_addMouseTool   ( gui, g3dmousetoolmove_new ( ) );

    /********************************/

    g3dui_addMouseTool   ( gui, g3dmousetoolscale_new ( ) );

    /********************************/

    g3dui_addMouseTool   ( gui, g3dmousetoolrotate_new ( ) );

    /********************************/

    g3dui_addMouseTool   ( gui, g3dmousetoolpickUV_new ( ) );

    /********************************/

    g3dui_addMouseTool   ( gui, g3dmousetoolmoveUV_new ( ) );

    /********************************/

    g3dui_addMouseTool   ( gui, g3dmousetoolscaleUV_new ( ) );

    /********************************/

    g3dui_addMouseTool   ( gui, g3dmousetoolrotateUV_new ( ) );

    /********************************/

    g3dui_addMouseTool   ( gui, m3dmousetoolpen_new ( ) );

    /********************************/

    g3dui_addMouseTool   ( gui, m3dmousetooleraser_new ( ) );

    /********************************/

    g3dui_addMouseTool   ( gui, m3dmousetoolselect_new ( ) );

    /********************************/

    g3dui_addMouseTool   ( gui, m3dmousetoolbucket_new ( ) );
}
