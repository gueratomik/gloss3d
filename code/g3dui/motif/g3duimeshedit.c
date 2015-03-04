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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
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
#include <g3dui_motif.h>


#ifdef OLDCODE
/******************************************************************************/
static void updateWeightGroupList ( Widget );
void updateSubdivisionRender ( Widget );
void updateSubdivisionView ( Widget );

/******************************************************************************/
static uint64_t getAvailableMemory ( ) {
    uint64_t memsize = 0x00;
    char str[0x100];
    FILE *fp;

    if ( ( fp =  fopen ( "/proc/meminfo", "r" ) ) == NULL ) {
        fprintf ( stderr, "Could not open /proc/meminfo\n" );

        return 0x00;
    }

    while ( feof ( fp ) == 0x00 ) {
        fgets ( str, sizeof ( str ), fp );

        if ( strstr ( str, "MemTotal" ) ) {
            sscanf ( str, "MemTotal: %llu kB", &memsize  );

            fclose ( fp );

            return ( memsize * 1024 );
        }

        memset ( str, 0x00, sizeof ( str ) );
    }

    fclose ( fp );

    return 0x00;
}

/******************************************************************************/
G3DUICONFIRMSUBDIV *g3duiconfirmsubdiv_new ( G3DMESH *mes, uint32_t level ) {
    uint32_t size = sizeof ( G3DUICONFIRMSUBDIV );
    G3DUICONFIRMSUBDIV *csd = ( G3DUICONFIRMSUBDIV * ) calloc ( 0x01, size );

    if ( csd == NULL ) {
        fprintf ( stderr, "g3duiconfirmsubdiv_new: mem allocation failed\n" );

        return NULL;
    }

    csd->mes   = mes;
    csd->level = level;

    return csd;
}

/******************************************************************************/
void confirmsubdivokcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmAnyCallbackStruct *abs = ( XmAnyCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DUICONFIRMSUBDIV *csd;
    XmString str;

    XtVaGetValues ( w, XmNmessageString, &str,
                       XmNuserData, &csd, NULL );

    g3dui_setHourGlass ( gui );

    g3dmesh_setSubdivisionLevel ( csd->mes, csd->level, gui->flags );

    g3dui_unsetHourGlass ( gui );

    XtUnmanageChild ( w );

    XmStringFree ( str );

    free ( csd );
}

/******************************************************************************/
void confirmsubdivcancelcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmAnyCallbackStruct *abs = ( XmAnyCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DUICONFIRMSUBDIV *csd;
    XmString str;

    XtVaGetValues ( w, XmNmessageString, &str,
                       XmNuserData, &csd, NULL );

    XtUnmanageChild ( w );

    XmStringFree ( str );

    free ( csd );

    g3dui_updateAllCurrentEdit ( gui );
}
#endif

/******************************************************************************/
static void deleteWeightGroupCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duiweightgrouplist_deleteWeightGroupCbk ( gui );
}

/******************************************************************************/
static void addWeightGroupCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duiweightgrouplist_addWeightGroupCbk ( gui );
}

/******************************************************************************/
static void nameCbk ( Widget w, XtPointer client, XtPointer call ) {
    char *value = XmTextGetString ( w );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duiweightgrouplist_nameCbk ( gui, value );

    XtFree ( value );
}

/******************************************************************************/
static void setAdaptiveAngleCbk ( Widget w, XtPointer client, XtPointer call ) {
    char *value = XmTextGetString ( w );
    float angle = strtof ( value, NULL );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duimeshedit_setAdaptiveAngleCbk ( gui, angle );

    XtFree ( value );
}

/******************************************************************************/
static void useAdativeCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duimeshedit_useAdaptiveCbk ( gui );
}

/******************************************************************************/
static void useIsoLinesCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duimeshedit_useIsoLinesCbk ( gui );
}

/******************************************************************************/
static void subdivRenderCbk ( Widget w, XtPointer client, XtPointer call ) {
    Widget parent = XtParent ( w );
    char *value = XmTextGetString ( w );
    int level = strtol ( value, NULL, 10 );
    G3DUI *gui = ( G3DUI * ) client;

    if ( level >= 0x00 ) {
        common_g3duimeshedit_subdivRenderCbk ( gui, level );
    } else {
        updateMeshEdit ( parent, gui );
    }

    XtFree ( value );
}

/******************************************************************************/
static void subdivLevelCbk ( Widget w, XtPointer client, XtPointer call ) {
    Widget parent = XtParent ( w );
    char *value = XmTextGetString ( w );
    int level = strtol ( value, NULL, 10 );
    G3DUI *gui = ( G3DUI * ) client;

    if ( level >= 0x00 ) {
        common_g3duimeshedit_subdivLevelCbk ( gui, level );
    } else {
        updateMeshEdit ( parent, gui );
    }

    XtFree ( value );
}

/******************************************************************************/
/*** Pass the subdivision frame as arg please, not the whole meshedit frame ***/
void updateSubdivisionForm ( Widget w, G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent loop when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        WidgetList children;
        Cardinal nc;
        int i;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
            char *name = XtName ( child );
            char buf[0x10];

            if ( XtClass ( child ) == xmTextWidgetClass ) {
                if ( strcmp ( name, EDITMESHSUBDIVVIEW ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%d", mes->subdiv );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITMESHSUBDIVREND ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%d", mes->subdiv_render );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITMESHADVANGLE ) == 0x00 ) {
                    char buf[0x10];

                    snprintf ( buf, 0x10, "%.2f", mes->advang );

                    XmTextSetString ( child, buf );
                }
            }

            if ( XtClass ( child ) == xmToggleButtonWidgetClass ) {
                if ( strcmp ( name, EDITMESHSUBDIVMODE ) == 0x00 ) {
                    if ( ((G3DOBJECT*)mes)->flags & BUFFEREDSUBDIVISION ) {
                        XmToggleButtonSetState ( child, True, False );
                    } else {
                        XmToggleButtonSetState ( child, False, False );
                    }
                }

                if ( strcmp ( name, EDITMESHADAPTIVE ) == 0x00 ) {
                    if ( ((G3DOBJECT*)mes)->flags & MESHUSEADAPTIVE ) {
                        XmToggleButtonSetState ( child, True, False );
                    } else {
                        XmToggleButtonSetState ( child, False, False );
                    }
                }

                if ( strcmp ( name, EDITMESHISOLINES ) == 0x00 ) {
                    if ( ((G3DOBJECT*)mes)->flags & MESHUSEISOLINES ) {
                        XmToggleButtonSetState ( child, True, False );
                    } else {
                        XmToggleButtonSetState ( child, False, False );
                    }
                }

                if ( strcmp ( name, EDITMESHSUBDIVSYNC ) == 0x00 ) {
                    if ( ((G3DOBJECT*)mes)->flags & SYNCSUBDIVISION ) {
                        XmToggleButtonSetState ( child, True, False );
                    } else {
                        XmToggleButtonSetState ( child, False, False );
                    }
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
/*** Pass the subdivision frame as arg please, not the whole meshedit frame ***/
void updateSubdivisionFrame ( Widget w, G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent loop when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        WidgetList children;
        Cardinal nc;
        int i;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
            char *name = XtName ( child );
            if ( XtClass ( child ) == xmFormWidgetClass ) {
                if ( strcmp ( name, EDITMESHSUBDIVISION ) == 0x00 ) {
                    updateSubdivisionForm ( child, gui );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void createSubdivisionFrame ( Widget parent, G3DUI *gui,  
                                                    Dimension x,
                                                    Dimension y,
                                                    Dimension width,
                                                    Dimension height ) {
    Widget sdf;

    sdf = createFrame ( parent, gui, EDITMESHSUBDIVISION, x,   y, width, 
                                                                  height );

    createIntegerText ( sdf   , gui, EDITMESHSUBDIVVIEW , 0,   0, 128, 32,
                                                          subdivLevelCbk );

    createIntegerText ( sdf   , gui, EDITMESHSUBDIVREND , 0,  24, 128, 32,
                                                          subdivRenderCbk );

    createToggleLabel ( sdf   , gui, EDITMESHADAPTIVE   , 4,  48, 192, 20,
                                                          useAdativeCbk );

    createFloatText   ( sdf   , gui, EDITMESHADVANGLE,    4,  72,  96, 64,
                                                          setAdaptiveAngleCbk );

    createToggleLabel ( sdf   , gui, EDITMESHISOLINES   , 4,  96, 200, 20,
                                                          useIsoLinesCbk );
}

/******************************************************************************/
/*** Pass the subdivision frame as arg please, not the whole meshedit frame ***/
void updateWeightgroupForm ( Widget w, G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent loop when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        WidgetList children;
        Cardinal nc;
        int i;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
            char *name = XtName ( child );
            char buf[0x10];

            if ( XtClass ( child ) == xmTextWidgetClass ) {
                if ( strcmp ( name, EDITWEIGHTGROUPNAME ) == 0x00 ) {
                    if ( mes->curgrp ) {
                        XmTextSetString ( child, mes->curgrp->name );
                    }
                }
            }
        }
    }

    /*** It's not the best place for that but I don't ***/
    /*** have time to figure out a better solution. ***/
    g3dui_redrawAllWeightGroupList ( gui );


    gui->lock = 0x00;
}

/******************************************************************************/
/*** Pass the subdivision frame as arg please, not the whole meshedit frame ***/
void updateWeightgroupFrame ( Widget w, G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent loop when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        WidgetList children;
        Cardinal nc;
        int i;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
            char *name = XtName ( child );
            if ( XtClass ( child ) == xmFormWidgetClass ) {
                if ( strcmp ( name, EDITMESHWEIGHTGROUP ) == 0x00 ) {
                    updateWeightgroupForm ( child, gui );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void createWeightgroupFrame ( Widget parent, G3DUI *gui,  
                                                    Dimension x,
                                                    Dimension y,
                                                    Dimension width,
                                                    Dimension height ) {
    Widget wgf;

    wgf = createFrame ( parent, gui, EDITMESHWEIGHTGROUP,  x, y, width, 
                                                                 height );

    /*** We have to encapsulate widget into a bulletin board widget because ***/
    /*** XmFrame always wants to resize itself. This way, it does not resize***/
    /*brd = createBoard ( sdf, gui, EDITMESHVERTEXGROUP,  4,   20, 278, 120 );*/

    createCharText ( wgf, gui, EDITWEIGHTGROUPNAME, 0, 0, 96, 132, nameCbk );

    createWeightgroupList ( wgf, gui, EDITWEIGHTGROUPLIST,  0, 36, 218, 128 );

    createPushButton ( wgf, gui, "+", 246,  36, 32, 32, addWeightGroupCbk    );
    createPushButton ( wgf, gui, "-", 246, 132, 32, 32, deleteWeightGroupCbk );
}

/******************************************************************************/
void updateMeshEdit ( Widget w, G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent loop when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        WidgetList children;
        Cardinal nc;
        int i;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
            char *name = XtName ( child );
            if ( XtClass ( child ) == xmFrameWidgetClass ) {
                if ( strcmp ( name, EDITMESHSUBDIVISION ) == 0x00 ) {
                    updateSubdivisionFrame ( child, gui );
                }

                if ( strcmp ( name, EDITMESHWEIGHTGROUP ) == 0x00 ) {
                    updateWeightgroupFrame ( child, gui );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
Widget createMeshEdit ( Widget parent, G3DUI *gui, 
                                       char *name,
                                       Dimension x,
                                       Dimension y,
                                       Dimension width,
                                       Dimension height ) {
    Pixel background, foreground;
    Widget frm;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    frm = XmVaCreateForm ( parent, name,
                                   XtNx, x,
                                   XtNy, y,
                                   XtNwidth , width,
                                   XtNheight, height,
                                   XmNforeground, foreground,
                                   XmNbackground, background,
                                   XmNborderWidth, 0x00,
                                   NULL );

    createSubdivisionFrame ( frm, gui,   0,   0, 286, 140 );
    createWeightgroupFrame ( frm, gui,   0, 144, 286, 200 );

    XtManageChild ( frm );

    return frm;
}
