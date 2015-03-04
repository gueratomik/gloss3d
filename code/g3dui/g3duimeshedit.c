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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2014         */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

/******************************************************************************/
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

/******************************************************************************/
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

/******************************************************************************/
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

/******************************************************************************/
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/PushB.h>
#include <Xm/Manager.h>
#include <Xm/ManagerP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/Scale.h>
#include <Xm/Text.h>
#include <Xm/List.h>
#include <Xm/ScrolledW.h>
#include <Xm/MessageB.h>
#include <Xm/Frame.h>

/******************************************************************************/
#include <X11/xpm.h>

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/GLwMDrawA.h>

/******************************************************************************/
#include <list.h>
#include <g3d.h>
#include <r3d.h>
#include <r3dnet.h>
#include <g3durmanager.h>
#include <g3dmouse.h>
#include <g3dui.h>

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

    updateAllCurrentEdit ( gui );
}

/******************************************************************************/
static void incsubrendcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    Widget parent = XtParent ( w );
    Widget subFrm = XtParent ( parent );

    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        int32_t subdiv = mes->subdiv_render + 0x01;

        arrowSetValue ( parent, subdiv );

        if ( obj->flags & SYNCSUBDIVISION ) {
            /*** There is no need to set the subdivision level in the ***/
            /*** callback because the textWidget's callback is goind  ***/
            /*** to be called by updateSubdivisionRender ***/
            /*g3dmesh_setSubdivisionLevel ( mes, level, gui->flags );*/

            updateSubdivisionView ( subFrm );
        }
    }

    redrawGLViews ( gui );
}

/******************************************************************************/
static void subrendcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    Widget parent = XtParent ( w );
    Widget subFrm = XtParent ( parent );

    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        char *value = XmTextGetString ( w );
        int32_t level = strtol ( value, NULL, 10 );
        G3DMESH *mes = ( G3DMESH * ) obj;

        if ( level >= 0x00 ) {
            mes->subdiv_render = level;

            if ( obj->flags & SYNCSUBDIVISION ) {
                /*** There is no need to set the subdivision level in the ***/
                /*** callback because the textWidget's callback is goind  ***/
                /*** to be called by updateSubdivisionRender ***/
                /*g3dmesh_setSubdivisionLevel ( mes, level, gui->flags );*/

                updateSubdivisionView ( subFrm );
            }
        }
    }

    redrawGLViews ( gui );
}

/******************************************************************************/
static void decsubrendcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    Widget parent = XtParent ( w );
    Widget subFrm = XtParent ( parent );

    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        int32_t subdiv = mes->subdiv_render - 0x01;

        if ( subdiv >= 0x00 ) {
            mes->subdiv_render = subdiv;

            arrowSetValue ( parent, subdiv );

            if ( obj->flags & SYNCSUBDIVISION ) {
                /*** There is no need to set the subdivision level in the ***/
                /*** callback because the textWidget's callback is goind  ***/
                /*** to be called by updateSubdivisionRender ***/
                /*g3dmesh_setSubdivisionLevel ( mes, subdiv, gui->flags );*/

                updateSubdivisionView ( subFrm );
            }
        }
    }

    redrawGLViews ( gui );
}

/******************************************************************************/
static void incsubcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    Widget parent = XtParent ( w );
    Widget subFrm = XtParent ( parent );

    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        int32_t subdiv = mes->subdiv + 0x01;

        arrowSetValue ( parent, subdiv );

        if ( obj->flags & SYNCSUBDIVISION ) {
            /*** There is no need to set the subdivision level in the ***/
            /*** callback because the textWidget's callback is goind  ***/
            /*** to be called by updateSubdivisionRender ***/
            /*g3dmesh_setSubdivisionLevel ( mes, subdiv, gui->flags );*/

            updateSubdivisionRender ( subFrm );
        }
    }

    redrawGLViews ( gui );
}

/******************************************************************************/
static void decsubcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    Widget parent = XtParent ( w );
    Widget subFrm = XtParent ( parent );

    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        int32_t subdiv = mes->subdiv - 0x01;

        if ( subdiv >= 0x00 ) {
            mes->subdiv = subdiv;

            arrowSetValue ( parent, subdiv );

            if ( obj->flags & SYNCSUBDIVISION ) {
                /*** There is no need to set the subdivision level in the ***/
                /*** callback because the textWidget's callback is goind  ***/
                /*** to be called by updateSubdivisionRender ***/
                /*g3dmesh_setSubdivisionLevel ( mes, subdiv, gui->flags );*/

                updateSubdivisionRender ( subFrm );
            }
        }
    }

    redrawGLViews ( gui );
}

/******************************************************************************/
static void subcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    Widget parent = XtParent ( w );
    Widget subFrm = XtParent ( parent );

    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        char *value = XmTextGetString ( w );
        uint32_t level = strtol ( value, NULL, 10 );
        uint64_t memsize = getAvailableMemory ( );
        uint64_t needmem = g3dmesh_evalSubdivisionBuffer ( mes, level, 
                                                                gui->flags );


        if ( needmem == 0xFFFFFFFFFFFFFFFF ) {
            XmString str;
            Widget mdw;

            str = XmStringCreateLocalized ( "Subdivision level too high !" );

            mdw = XmCreateMessageDialog ( gui->top, "Warning", NULL, 0x00 );

            XtVaSetValues ( mdw, XmNmessageString, str,
                                 NULL );

            XtManageChild ( mdw );
            XtPopup ( XtParent ( mdw ), XtGrabNone );

            updateAllCurrentEdit ( gui );
        /*** Display some warning if the needed memory is > Total memory/16 ***/
        } else if ( needmem > ( memsize >> 0x04 ) ) {
            G3DUICONFIRMSUBDIV *csd = g3duiconfirmsubdiv_new ( mes, level );
            XmString str;
            Widget mdw;

            str = XmStringCreateLocalized ( "Warning: this may slow down\n"
                                            "your computer A LOT! Continue?" );

            mdw = XmCreateMessageDialog ( gui->top, "Warning", NULL, 0x00 );

            XtVaSetValues ( mdw, XmNuserData, csd,
                                 XmNmessageString, str,
                                 NULL );

            XtAddCallback ( mdw, XmNokCallback    , confirmsubdivokcbk    , gui );
            XtAddCallback ( mdw, XmNcancelCallback, confirmsubdivcancelcbk, gui );

            XtManageChild ( mdw );
            XtPopup ( XtParent ( mdw ), XtGrabNone );
        } else {
            g3dui_setHourGlass ( gui );

            g3dmesh_setSubdivisionLevel ( mes, level, gui->flags );

            if ( obj->flags & SYNCSUBDIVISION ) {
                updateSubdivisionRender ( subFrm );
            }

            g3dui_unsetHourGlass ( gui );
        }


        redrawGLViews ( gui );
    }
}

/******************************************************************************/
/*** Pass the subdivision frame as arg please, not the whole meshedit frame ***/
void updateSubdivisionFrame ( Widget w ) {
    WidgetList children;
    G3DOBJECT *obj;
    G3DSCENE *sce;
    G3DUI *gui;

    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    sce = gui->sce;
    obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        Cardinal nc;
        int i;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
            char *name = XtName ( child );
            if ( XtClass ( child ) == xmFormWidgetClass ) {
                if ( strcmp ( name, EDITMESHSUBDIVVIEW ) == 0x00 ) {
                    arrowSetValue ( child, mes->subdiv );
                }

                if ( strcmp ( name, EDITMESHSUBDIVREND ) == 0x00 ) {
                    arrowSetValue ( child, mes->subdiv_render );
                }
            }

            if ( XtClass ( child ) == xmTextWidgetClass ) {
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
void updateSubdivisionRender ( Widget w ) {
    WidgetList children;
    G3DOBJECT *obj;
    G3DSCENE *sce;
    G3DUI *gui;

    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    sce = gui->sce;
    obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        Cardinal nc;
        int i;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
            char *name = XtName ( child );

            if ( XtClass ( child ) == xmFormWidgetClass ) {
                if ( strcmp ( name, EDITMESHSUBDIVREND ) == 0x00 ) {
                    arrowSetValue ( child, mes->subdiv_render );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
/*** Pass the subdivision frame as arg please, not the whole meshedit frame ***/
void updateSubdivisionView ( Widget w ) {
    WidgetList children;
    G3DOBJECT *obj;
    G3DSCENE *sce;
    G3DUI *gui;

    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    sce = gui->sce;
    obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        Cardinal nc;
        int i;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
            char *name = XtName ( child );
            if ( XtClass ( child ) == xmFormWidgetClass ) {
                if ( strcmp ( name, EDITMESHSUBDIVVIEW ) == 0x00 ) {
                    arrowSetValue ( child, mes->subdiv );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
void updateWeightGroupsBoard ( Widget w ) {
    WidgetList children;
    G3DOBJECT *obj;
    G3DSCENE *sce;
    G3DUI *gui;

    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    sce = gui->sce;
    obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        Cardinal nc;
        int i;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
            char *name = XtName ( child );

            /*if ( mes->curgrp && ( XtClass ( child ) == xmTextWidgetClass ) ) {
                if ( strcmp ( name, EDITWEIGHTGROUPNAME   ) == 0x00 ) {
                    char buf[0x10];
                    snprintf ( buf, 0x10, "%d", mes->curgrp->name );

                    XmTextSetString ( child, buf );
                }
            }*/

            if ( XtClass ( child ) == xmScrolledWindowWidgetClass ) {
                Widget lst;

                XtVaGetValues ( child, XmNworkWindow, &lst, NULL );

                if ( strcmp ( XtName ( lst ), EDITMESHVERTEXGROUP ) == 0x00 ) {
                    updateWeightGroupList ( lst );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
void updateWeightGroupsFrame ( Widget w ) {
    WidgetList children;
    G3DOBJECT *obj;
    G3DSCENE *sce;
    G3DUI *gui;

    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    sce = gui->sce;
    obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        Cardinal nc;
        int i;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
            char *name = XtName ( child );

            if ( XtClass ( child ) == xmBulletinBoardWidgetClass ) {
                updateWeightGroupsBoard ( child );
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
void updateMeshEdit ( Widget w ) {
    WidgetList children;
    G3DOBJECT *obj;
    G3DSCENE *sce;
    G3DUI *gui;

    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    sce = gui->sce;
    obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        Cardinal nc;
        int i;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
            char *name = XtName ( child );

            if ( XtClass ( child ) == xmFrameWidgetClass ) {
                if ( strcmp ( XtName ( child ), EDITMESHSUBDIVISION ) == 0x00 ) {
                    updateSubdivisionFrame ( child );
                }

                if ( strcmp ( XtName ( child ), EDITMESHVERTEXGROUP ) == 0x00 ) {
                    updateWeightGroupsFrame ( child );
                }
            }

            /*if ( mes->curgrp && ( XtClass ( child ) == xmTextWidgetClass ) ) {
                if ( strcmp ( name, EDITWEIGHTGROUPNAME   ) == 0x00 ) {
                    char buf[0x10];
                    snprintf ( buf, 0x10, "%d", mes->curgrp->name );

                    XmTextSetString ( child, buf );
                }
            }*/
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
Widget meshEdit_getWeightgroupNameField ( Widget w ) {
    WidgetList children;
    Cardinal nc;
    int i;

    XtVaGetValues ( w, XmNchildren   , &children, 
                       XmNnumChildren, &nc, 
                       NULL );

    for ( i = 0x00; i < nc; i ++ ) {
        Widget child = children[i];

        if (        ( XtClass ( child ) == xmTextWidgetClass ) && 
             strcmp ( XtName  ( child ), EDITWEIGHTGROUPNAME ) == 0x00 ) {

            return child;
        }
    }
}

/******************************************************************************/
Widget weightGroup_getListWidget ( Widget w ) {
    WidgetList children;
    Cardinal nc;
    int i;

    XtVaGetValues ( w, XmNchildren   , &children, 
                       XmNnumChildren, &nc, 
                       NULL );

    for ( i = 0x00; i < nc; i ++ ) {
        Widget child = children[i];

        if ( XtClass ( child ) == xmScrolledWindowWidgetClass ) {
            Widget lst;

            XtVaGetValues ( child, XmNworkWindow, &lst, NULL );

            if ( strcmp ( XtName ( lst ), EDITMESHVERTEXGROUP ) == 0x00 ) {

                return lst;
            }
        }
    }

    return NULL;
}

/******************************************************************************/
static void updateWeightGroupList ( Widget w ) {
    G3DOBJECT *obj;                              
    G3DSCENE *sce;
    G3DUI *gui;

    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    sce = gui->sce;
    obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        LIST *ltmp = mes->lgrp;

        XmListDeleteAllItems ( w );

        while ( ltmp ) {
            G3DWEIGHTGROUP *grp = ( G3DWEIGHTGROUP * ) ltmp->data;
            XmString str = XmStringCreate ( grp->name, XmFONTLIST_DEFAULT_TAG );

            XmListAddItem ( w, str, 0x00 );

            XmStringFree ( str );

            ltmp = ltmp->next;
        }
    }
}

/******************************************************************************/
static void addvergrpcbk ( Widget w, XtPointer client, XtPointer call ) {
    Widget parent = XtParent ( w );
    WidgetList children;
    G3DOBJECT *obj;
    G3DSCENE *sce;
    Widget grplst;
    G3DUI *gui;

    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    sce = gui->sce;
    obj = g3dscene_getSelectedObject ( sce );

    grplst = weightGroup_getListWidget ( parent );

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        char buf[0x20];

        snprintf ( buf, 0x20, "VertexWeightGroup%02i", mes->nbgrp );

        g3dmesh_addWeightGroup ( mes, g3dweightgroup_new ( mes, buf ) );

        updateWeightGroupList ( grplst );
    }
}

/******************************************************************************/
static void delvergrpcbk ( Widget w, XtPointer client, XtPointer call ) {
    Widget parent = XtParent ( w );
    WidgetList children;
    G3DOBJECT *obj;
    G3DSCENE *sce;
    Widget grplst;
    G3DUI *gui;

    XtVaGetValues ( parent, XmNuserData, &gui, NULL );

    sce = gui->sce;
    obj = g3dscene_getSelectedObject ( sce );

    grplst = weightGroup_getListWidget ( parent );

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        if ( mes->curgrp ) {
            g3dweightgroup_empty ( mes->curgrp );

            g3dmesh_removeWeightGroup ( mes, mes->curgrp );

            updateWeightGroupList ( grplst );
        }
    }
}

/******************************************************************************/
static void selvergrpcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmListCallbackStruct *lcs = ( XmListCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( XtParent ( w ) ); /*** Parent is scroll window */
    G3DOBJECT *obj;                               /* so we need 1 level upper */
    G3DSCENE *sce;
    Widget txtname = meshEdit_getWeightgroupNameField ( parent );

    gui->lock = 0x01;

    sce = gui->sce;
    obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        /*** deselect previsouly selected weightgroup ***/
        if ( mes->curgrp ) {
            G3DWEIGHTGROUP *curgrp = mes->curgrp;
/*** TODO - check if we are in buffered mode. Otherwise this is not needed ***/
            LIST *lsub = g3dvertex_getFacesFromList ( curgrp->lver );

            g3dmesh_unselectWeightGroup ( mes, curgrp );

            /*** update vertex painting ***/
            g3dmesh_update ( mes, NULL,
                                  NULL,
                                  NULL,
                                  lsub,
                                  COMPUTESUBDIVISION, gui->flags );

            list_free ( &lsub, NULL );
        }

        if ( lcs->reason == XmCR_SINGLE_SELECT ) {
            int position = ( lcs->item_position - 0x01 );
            G3DWEIGHTGROUP *grp = list_seekbyrank ( mes->lgrp, position );
/*** TODO - check if we are in buffered mode. Otherwise this is not needed ***/
            LIST *lsub = g3dvertex_getFacesFromList ( grp->lver );
            char buf[0x100];

            snprintf ( buf, 0x100, "%s", grp->name );

            g3dmesh_selectWeightGroup ( mes, grp );

            /*** update vertex painting ***/
            g3dmesh_update ( mes, NULL,
                                  NULL,
                                  NULL,
                                  lsub,
                                  COMPUTESUBDIVISION, gui->flags );

            list_free ( &lsub, NULL );

            XmTextSetString ( txtname, buf );
        }

        redrawGLViews ( gui );
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void subdivsynccbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( gui->lock ) return;

    if ( obj ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        if ( obj->flags & SYNCSUBDIVISION ) {
            g3dmesh_unsetSyncSubdivision ( mes );
        } else {
            g3dmesh_setSyncSubdivision   ( mes );
        }
    }
}

/******************************************************************************/
static void adaptivecbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( gui->lock ) return;

    if ( obj ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        if ( obj->flags & MESHUSEADAPTIVE ) {
            obj->flags &= (~MESHUSEADAPTIVE);
        } else {
            obj->flags |= MESHUSEADAPTIVE;
        }

        g3dui_setHourGlass ( gui );

        /*** rebuild using adaptive subdivision (or not) ***/
        g3dmesh_update ( mes, NULL,
                              NULL,
                              NULL,
                              NULL,
                              COMPUTESUBDIVISION, gui->flags );

        g3dui_unsetHourGlass ( gui );

        redrawGLViews ( gui );
    }
}

/******************************************************************************/
static void advangcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( gui->lock ) return;

    if ( obj ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        char *value = XmTextGetString ( w );
        float angle = strtof ( value, NULL );

        mes->advang = angle;

        g3dui_setHourGlass ( gui );

        /*** rebuild using adaptive subdivision (or not) ***/
        g3dmesh_update ( mes, NULL,
                              NULL,
                              NULL,
                              NULL,
                              COMPUTESUBDIVISION, gui->flags );

        g3dui_unsetHourGlass ( gui );

        redrawGLViews ( gui );
    }
}

/******************************************************************************/
static void subdivmodecbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3dui_setHourGlass ( gui );

        if ( obj->flags & BUFFEREDSUBDIVISION ) {
            g3dmesh_unsetBufferedSubdivision ( mes );
        } else {
            g3dmesh_setBufferedSubdivision ( mes, gui->flags );
        }

        g3dui_unsetHourGlass ( gui );
    }
}


/******************************************************************************/
static void isolinescbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3dui_setHourGlass ( gui );

        if ( obj->flags & MESHUSEISOLINES ) {
            obj->flags &= (~MESHUSEISOLINES);
        } else {
            obj->flags |= MESHUSEISOLINES;
        }

        /*** rebuild using adaptive subdivision (or not) ***/
        g3dmesh_update ( mes, NULL,
                              NULL,
                              NULL,
                              NULL,
                              REALLOCSUBDIVISION |
                              COMPUTESUBDIVISION, gui->flags );

        g3dui_unsetHourGlass ( gui );

        redrawGLViews ( gui );
    }
}

/******************************************************************************/
static void namecbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        char *value = XmTextGetString ( w );
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3dweightgroup_name ( mes->curgrp, value );

        XtFree ( value );

        updateAllCurrentEdit ( gui );
    }
}

/******************************************************************************/
Widget createMeshEdit ( Widget parent  , G3DUI *gui, char *name,
                        Dimension x    , Dimension y,
                        Dimension width, Dimension height ) {
    Pixel white = XWhitePixel ( XtDisplay ( parent ), 0x00 ),
          black = XBlackPixel ( XtDisplay ( parent ), 0x00 );
    Pixel background, foreground = 0x00;
    Widget frm, grplst, grpadd, grpdel, sdf, brd;
    Arg args[]= { { XmNx                     , ( XtArgVal )    0            },
                  { XmNy                     , ( XtArgVal )    0            },
                  { XmNwidth                 , ( XtArgVal )  218            },
                  /*{ XmNheight                , ( XtArgVal )   80            },*/
                  { XmNhighlightThickness    , ( XtArgVal ) 0x01            },
                  { XmNshadowThickness       , ( XtArgVal ) 0x01            },
                  { XmNfontList              , ( XtArgVal ) gui->fontlist   },
                  { XmNlistMarginHeight      , ( XtArgVal ) 0x01            },
                  { XmNlistMarginWidth       , ( XtArgVal ) 0x01            },
                  { XmNtraversalOn           , ( XtArgVal ) False           },
                  { XtNbackground            , ( XtArgVal ) white           },
                  { XtNforeground            , ( XtArgVal ) black           },
                  { XmNuserData              , ( XtArgVal ) gui             },
                  { XmNvisualPolicy          , ( XtArgVal ) XmCONSTANT      },
                  { XmNscrollingPolicy       , ( XtArgVal ) XmCONSTANT      },
                  { XmNlistSizePolicy        , ( XtArgVal ) XmCONSTANT      },
                  { XmNscrollBarDisplayPolicy, ( XtArgVal ) XmAS_NEEDED     },
                  { XmNframeChildType        , ( XtArgVal ) XmFRAME_GENERIC_CHILD },
                  { XmNselectionPolicy       , ( XtArgVal ) XmSINGLE_SELECT },
                  { XmNvisibleItemCount      , ( XtArgVal ) 0x05            } };

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
                                   XmNuserData, gui,
                                   NULL );

    sdf = createFrame ( frm, gui, EDITMESHSUBDIVISION,  0,   0, 286, 140 );

    /*createToggleLabel  ( sdf,     EDITMESHSUBDIVMODE,   4,  16, 192, 20, subdivmodecbk );*/

    createSimpleLabel ( sdf,      EDITMESHSUBDIVVIEW,   4,  36, 128 );
    createArrow       ( sdf, gui, EDITMESHSUBDIVVIEW, 132,  36, 128, 20,
                                                      incsubcbk, 
                                                      decsubcbk,
                                                      subcbk );

    createToggleButton ( sdf,     EDITMESHSUBDIVSYNC, 230,  46,  20, 20,
                                                      subdivsynccbk );

    createSimpleLabel ( sdf,      EDITMESHSUBDIVREND,   4,  56, 128 );
    createArrow       ( sdf, gui, EDITMESHSUBDIVREND, 132,  56, 128, 20,
                                                      incsubrendcbk,
                                                      decsubrendcbk,
                                                      subrendcbk );

    createToggleLabel  ( sdf,     EDITMESHADAPTIVE,     4,  76, 192, 20,
                                                      adaptivecbk );

    createFloatText   ( sdf,      EDITMESHADVANGLE,     4,  96,  96, 64,
                                                      advangcbk  );

    createSimpleLabel ( sdf,      "Degrees",          164,  96,  64 );

    createToggleLabel  ( sdf,     EDITMESHISOLINES,     4, 116, 150, 20,
                                                      isolinescbk );

    sdf = createFrame ( frm, gui, EDITMESHVERTEXGROUP,  0, 144, 286, 144 );

    /*** We have to encapsulate widget into a bulletin board widget because ***/
    /*** XmFrame always wants to resize itself. This way, it does not resize***/
    brd = createBoard ( sdf, gui, EDITMESHVERTEXGROUP,  4,   20, 278, 120 );

    grplst = XmCreateScrolledList ( brd, EDITMESHVERTEXGROUP, 
                                    args, XtNumber ( args ) );

    XtManageChild ( grplst );

    XtAddCallback ( grplst, XmNsingleSelectionCallback, selvergrpcbk, gui );

    createSimplePushButton ( brd, "+", 246,  0, 32, 32, addvergrpcbk  );
    createSimplePushButton ( brd, "-", 246, 64, 32, 32, delvergrpcbk  );

    createCharText ( brd, EDITWEIGHTGROUPNAME, 4, 100, 70, 200, namecbk );


    XtManageChild ( frm );

    return frm;
}
