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
#include <Xm/CascadeB.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/FileSB.h>
#include <Xm/MessageB.h>

/******************************************************************************/
#include <X11/xpm.h>

/******************************************************************************/
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/GLwMDrawA.h>

/******************************************************************************/
#include <config.h>

/******************************************************************************/
/***************************** SAX XML Library ********************************/
#ifdef HAVE_EXPAT_H
#include <expat.h>
#endif

/******************************************************************************/
#include <list.h>
#include <g3d.h>
#include <g3dexport.h>
#include <r3d.h>
#include <r3dnet.h>
#include <g3durmanager.h>
#include <g3dmouse.h>
#include <g3dui.h>

/******************************************************************************/
void mirrorSkeletoncbk ( Widget w, XtPointer client, XtPointer call ) {
    /*** Moment moment moment ... This function must EXCLUSIVELY be used ***/
    /*** with XmVaCreateSimplePulldownMenu ( ) because the G3DUI struct  ***/
    /*** is retrieved through the parent widget of the clicked button    ***/
    Widget parent = XtParent ( w );
#ifdef __LP64__
    uint64_t orientation = ( uint64_t ) client;
#else
    uint32_t orientation = ( uint32_t ) client;
#endif
    G3DURMANAGER *urm;
    G3DOBJECT *obj;
    G3DSCENE *sce;
    G3DUI *gui;

    XtVaGetValues ( parent, XmNuserData, &gui, NULL );

    sce = gui->sce;
    urm = gui->urm;
    obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type & BONE ) ) {
        G3DBONE *bon = ( G3DBONE * ) obj;

        /*g3dobject_addChild ( obj->parent, */g3dbone_mirror ( bon,
                                                           orientation,
                                                           0x01 );/* );*/
    }
}

/******************************************************************************/
void importobjokcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmFileSelectionBoxCallbackStruct *cbs =
                        ( XmFileSelectionBoxCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = NULL;
    char *filename;

    filename = ( char * ) XmStringUnparse ( cbs->value,
                                            XmFONTLIST_DEFAULT_TAG,
                                            XmCHARSET_TEXT,
                                            XmCHARSET_TEXT,
                                            NULL, 0x00, XmOUTPUT_ALL );

    g3duiOpenOBJFile ( gui, filename );

    XtFree ( filename );

    XtUnmanageChild ( w );

    redrawGLViews ( gui );
    updateCoords ( gui );
    redrawObjectList ( gui );
    updateAllCurrentEdit ( gui );
}


/******************************************************************************/
void exitokcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmAnyCallbackStruct *abs = ( XmAnyCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;

    XtUnmanageChild ( w );

    if ( gui->sce ) {
        g3dobject_free ( ( G3DOBJECT * ) gui->sce );
    }

    /*** More cleaning would be needed ***/

    exit ( EXIT_SUCCESS );
}

/******************************************************************************/
void exitcancelcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmAnyCallbackStruct *abs = ( XmAnyCallbackStruct * ) call;

    XtUnmanageChild ( w );
}

/******************************************************************************/
void exitcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    char buf[0x100];
    XmString str;
    Widget mdw;


    snprintf ( buf, 0x100, "Leave Gloss3D :-( ?" );
    str = XmStringCreateLocalized ( buf );

    mdw = XmCreateMessageDialog ( gui->top, "Confirm", NULL, 0x00 );

    XtVaSetValues ( mdw, XmNmessageString, str, NULL );

    XtAddCallback ( mdw, XmNokCallback    , exitokcbk    , gui );
    XtAddCallback ( mdw, XmNcancelCallback, exitcancelcbk, gui );

    XtManageChild ( mdw );
    XtPopup ( XtParent ( mdw ), XtGrabNone );
}

/******************************************************************************/
void exportobjokcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmFileSelectionBoxCallbackStruct *cbs =
                        ( XmFileSelectionBoxCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = NULL;
    char *filename;

    filename = ( char * ) XmStringUnparse ( cbs->value,
                                            XmFONTLIST_DEFAULT_TAG,
                                            XmCHARSET_TEXT,
                                            XmCHARSET_TEXT,
                                            NULL, 0x00, XmOUTPUT_ALL );

    g3dui_setHourGlass ( gui );

    g3dscene_exportObj ( gui->sce, filename, "#Made with Gloss3D\n\n", 0x00 );

    g3dui_unsetHourGlass ( gui );


    XtFree ( filename );

    XtUnmanageChild ( w );

    redrawGLViews ( gui );
    updateCoords ( gui );
    redrawObjectList ( gui );
    updateAllCurrentEdit ( gui );
}

/******************************************************************************/
void import3dsokcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmFileSelectionBoxCallbackStruct *cbs =
                        ( XmFileSelectionBoxCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = NULL;
    char *filename;

    filename = ( char * ) XmStringUnparse ( cbs->value,
                                            XmFONTLIST_DEFAULT_TAG,
                                            XmCHARSET_TEXT,
                                            XmCHARSET_TEXT,
                                            NULL, 0x00, XmOUTPUT_ALL );

    g3duiOpen3DSFile ( gui, filename );

    XtFree ( filename );

    XtUnmanageChild ( w );

    redrawGLViews ( gui );
    updateCoords ( gui );
    redrawObjectList ( gui );
    updateAllCurrentEdit ( gui );
}

#ifdef HAVE_EXPAT_H
/******************************************************************************/
void importdaeokcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmFileSelectionBoxCallbackStruct *cbs =
                        ( XmFileSelectionBoxCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = NULL;
    char *filename;

    filename = ( char * ) XmStringUnparse ( cbs->value,
                                            XmFONTLIST_DEFAULT_TAG,
                                            XmCHARSET_TEXT,
                                            XmCHARSET_TEXT,
                                            NULL, 0x00, XmOUTPUT_ALL );

    g3duiOpenDAEFile ( gui, filename );

    XtFree ( filename );

    XtUnmanageChild ( w );

    redrawGLViews ( gui );
    updateCoords ( gui );
    redrawObjectList ( gui );
    updateAllCurrentEdit ( gui );
}
#endif

/******************************************************************************/
#ifdef HAVE_C4D_H
void importc4dokcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmFileSelectionBoxCallbackStruct *cbs =
                        ( XmFileSelectionBoxCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = NULL;
    char *filename;

    filename = ( char * ) XmStringUnparse ( cbs->value,
                                            XmFONTLIST_DEFAULT_TAG,
                                            XmCHARSET_TEXT,
                                            XmCHARSET_TEXT,
                                            NULL, 0x00, XmOUTPUT_ALL );

    g3duiOpenC4DFile ( gui, filename );

    XtFree ( filename );

    XtUnmanageChild ( w );

    redrawGLViews ( gui );
    updateCoords ( gui );
    redrawObjectList ( gui );
    updateAllCurrentEdit ( gui );
}
#endif
/******************************************************************************/
void importfilecancelcbk ( Widget w, XtPointer client, XtPointer call ) {
    XtUnmanageChild ( w );
}

/******************************************************************************/
void exportfilecancelcbk ( Widget w, XtPointer client, XtPointer call ) {
    XtUnmanageChild ( w );
}

/******************************************************************************/
void selecttreecbk ( Widget w, XtPointer client, XtPointer call ) {
    /*** Moment moment moment ... This function must EXCLUSIVELY be used ***/
    /*** with XmVaCreateSimplePulldownMenu ( ) because the G3DUI struct  ***/
    /*** is retrieved through the parent widget of the clicked button    ***/
    Widget parent = XtParent ( w );
#ifdef __LP64__
    uint64_t same = ( uint64_t ) client;
#else
    uint32_t same = ( uint32_t ) client;
#endif
    G3DOBJECT *obj;
    Widget dialog;
    G3DSCENE *sce;
    G3DUI *gui;

    XtVaGetValues ( parent, XmNuserData, &gui, NULL );

    sce = gui->sce;
    obj = g3dscene_getSelectedObject ( sce );

    if ( obj ) {
        g3dscene_selectTree ( sce, obj, same );
    } else {
        g3dscene_selectTree ( sce, ( G3DOBJECT * ) sce, same );
    }

    redrawGLViews ( gui ); 
    updateAllCurrentEdit ( gui );
    redrawObjectList ( gui );
    redrawTimeline ( gui );
}

/******************************************************************************/
void importfilecbk ( Widget w, XtPointer client, XtPointer call ) {
    /*** Moment moment moment ... This function must EXCLUSIVELY be used ***/
    /*** with XmVaCreateSimplePulldownMenu ( ) because the G3DUI struct  ***/
    /*** is retrieved through the parent widget of the clicked button    ***/
    Widget parent = XtParent ( w );
#ifdef __LP64__
    uint64_t filetype = ( uint64_t ) client;
#else
    uint32_t filetype = ( uint32_t ) client;
#endif
    Widget dialog;
    G3DUI *gui;

    XtVaGetValues ( parent, XmNuserData, &gui, NULL );

    dialog = XmCreateFileSelectionDialog ( parent, "Open File", 
                                           NULL, 0x00 );

    if ( filetype == 0x00 ) {
        XtAddCallback ( dialog, XmNokCallback, import3dsokcbk, gui );
    }

    if ( filetype == 0x01 ) {
        XtAddCallback ( dialog, XmNokCallback, importobjokcbk, gui );
    }

#ifdef HAVE_EXPAT_H
    if ( filetype == 0x02 ) {
        XtAddCallback ( dialog, XmNokCallback, importdaeokcbk, gui );
    }
#endif

#ifdef HAVE_C4D_H
    if ( filetype == 0x03 ) {
        XtAddCallback ( dialog, XmNokCallback, importc4dokcbk, gui );
    }
#endif

    XtAddCallback ( dialog, XmNcancelCallback, importfilecancelcbk, gui );

    XtManageChild ( dialog );

    redrawGLViews ( gui ); 
    updateAllCurrentEdit ( gui );
}

/******************************************************************************/
void exportfilecbk ( Widget w, XtPointer client, XtPointer call ) {
    /*** Moment moment moment ... This function must EXCLUSIVELY be used ***/
    /*** with XmVaCreateSimplePulldownMenu ( ) because the G3DUI struct  ***/
    /*** is retrieved through the parent widget of the clicked button    ***/
    Widget parent = XtParent ( w );
#ifdef __LP64__
    uint64_t filetype = ( uint64_t ) client;
#else
    uint32_t filetype = ( uint32_t ) client;
#endif
    Widget dialog;
    G3DUI *gui;

    XtVaGetValues ( parent, XmNuserData, &gui, NULL );

    dialog = XmCreateFileSelectionDialog ( parent, "Open File", 
                                           NULL, 0x00 );

    if ( filetype == 0x00 ) {
        XtAddCallback ( dialog, XmNokCallback, exportobjokcbk, gui );
    }

    XtAddCallback ( dialog, XmNcancelCallback, exportfilecancelcbk, gui );

    XtManageChild ( dialog );

    redrawGLViews ( gui ); 
    updateAllCurrentEdit ( gui );
}

/******************************************************************************/
void mirrorWeightgroupcbk ( Widget w, XtPointer client, XtPointer call ) {
    /*** Moment moment moment ... This function must EXCLUSIVELY be used ***/
    /*** with XmVaCreateSimplePulldownMenu ( ) because the G3DUI struct  ***/
    /*** is retrieved through the parent widget of the clicked button    ***/
    Widget parent = XtParent ( w );
#ifdef __LP64__
    uint64_t orientation = ( uint64_t ) client;
#else
    uint32_t orientation = ( uint32_t ) client;
#endif
    G3DURMANAGER *urm;
    G3DOBJECT *obj;
    G3DSCENE *sce;
    G3DUI *gui;

    XtVaGetValues ( parent, XmNuserData, &gui, NULL );

    sce = gui->sce;
    urm = gui->urm;
    obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        G3DWEIGHTGROUP *grp = mes->curgrp, *newgrp;

        if ( grp ) {
            newgrp = g3dweightgroup_mirror ( grp, orientation );

            g3dmesh_addWeightGroup ( mes, newgrp );

            redrawGLViews ( gui );
            updateAllCurrentEdit ( gui );
        }
    }
}

/******************************************************************************/
void weldVerticescbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    G3DURMANAGER *urm = gui->urm;

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3durm_mesh_weldSelectedVertices ( urm, mes, 0x01, gui->flags, REDRAWVIEW );
    }
}

/******************************************************************************/
void untriangulatecbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    G3DURMANAGER *urm = gui->urm;

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3durm_mesh_untriangulate ( urm, mes, gui->flags, REDRAWVIEW );
    }
}

/******************************************************************************/
void triangulatecbk ( Widget w, XtPointer client, XtPointer call ) {
    /*** Moment moment moment ... This function must EXCLUSIVELY be used ***/
    /*** with XmVaCreateSimplePulldownMenu ( ) because the G3DUI struct  ***/
    /*** is retrieved through the parent widget of the clicked button    ***/
    Widget parent = XtParent ( w );
#ifdef __LP64__
    uint64_t clockwise = ( uint64_t ) client;
#else
    uint32_t clockwise = ( uint32_t ) client;
#endif
    G3DURMANAGER *urm;
    G3DOBJECT *obj;
    G3DSCENE *sce;
    G3DUI *gui;

    XtVaGetValues ( parent, XmNuserData, &gui, NULL );

    sce = gui->sce;
    urm = gui->urm;
    obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3durm_mesh_triangulate ( urm, mes, ( int ) clockwise, gui->flags, REDRAWVIEW );
    }
}

/******************************************************************************/
void invertNormalcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DURMANAGER *urm;
    G3DOBJECT *obj;
    G3DSCENE *sce;

    sce = gui->sce;
    urm = gui->urm;
    obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3durm_mesh_invertNormal ( urm, mes, gui->flags, REDRAWVIEW );
    }
}

/******************************************************************************/
void alignNormalscbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj->type == G3DMESHTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3dmesh_alignFaces ( mes );

        /*** Update subdivision if we are in buffered mode. ***/
        g3dmesh_update ( mes, NULL,
                              NULL,
                              NULL,
                              NULL,
                              COMPUTESUBDIVISION, gui->flags );

        redrawGLViews ( gui );
    }
}

/******************************************************************************/
static void fixbonecbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;

    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type == G3DBONETYPE ) ) {
        G3DBONE *bon = ( G3DBONE * ) obj;

        g3dbone_fix ( bon );
    }
}

/******************************************************************************/
static void fixbonetreecbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;

    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type == G3DBONETYPE ) ) {
        G3DBONE *bon = ( G3DBONE * ) obj;

        g3dbone_fix_r ( bon );
    }
}

/******************************************************************************/
static void resetbonecbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;

    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type == G3DBONETYPE ) ) {
        G3DBONE *bon = ( G3DBONE * ) obj;

        g3dbone_reset ( bon );
    }
}

/******************************************************************************/
static void resetbonetreecbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;

    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type == G3DBONETYPE ) ) {
        G3DBONE *bon = ( G3DBONE * ) obj;

        g3dbone_reset_r ( bon );
    }
}

/******************************************************************************/
static void adduvwmapcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;

    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj ){ 
        if ( ( obj->type == G3DMESHTYPE ) ||
           ( ( obj->type &  G3DPRIMITIVETYPE ) == G3DPRIMITIVETYPE ) ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DUVMAP *map = g3duvmap_new ( "UVMap", 0x00, UVMAPFLAT );

            g3dmesh_addUVMap ( mes, map, gui->flags );

            redrawObjectList ( gui );
        }
    }

    redrawGLViews ( gui );
}

/******************************************************************************/
static void addbonecbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    uint32_t oid = g3dscene_getNextObjectID ( sce );
    G3DBONE *bon = g3dbone_new ( oid, "Bone", 1.0f );

    g3durm_object_addChild ( urm, sce, gui->flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) bon );

    g3dscene_unselectAllObjects ( sce, gui->flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) bon, gui->flags );

    redrawObjectList ( gui );
    redrawGLViews ( gui );
}

/******************************************************************************/
static void addsymmetrycbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    uint32_t oid = g3dscene_getNextObjectID ( sce );
    G3DSYMMETRY *sym = g3dsymmetry_new ( oid, "Symmetry" );

    g3durm_object_addChild ( urm, sce, gui->flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) sym );

    g3dscene_unselectAllObjects ( sce, gui->flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) sym, gui->flags );

    redrawObjectList ( gui );
}

/******************************************************************************/
void setmaterialcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
    G3DMATERIAL *mat = gui->selmat;

    if ( obj && mat ) {
        if ( ( obj->type == G3DMESHTYPE ) ||
           ( ( obj->type &  G3DPRIMITIVETYPE ) == G3DPRIMITIVETYPE ) ) {
            G3DMESH  *mes = ( G3DMESH * ) obj;
            G3DUVMAP *map = g3dmesh_getLastUVMap ( mes );

            /*** A textured mesh should ALWAYS have a UVMAP.     ***/
            /*** Likely design issue - to FIX when I have time ***/
            if ( map == NULL ) {
                map = g3duvmap_new ( "UVMap", 0x00, UVMAPFLAT );

                g3dmesh_addUVMap ( mes, map, gui->flags );
            }

            g3dmesh_addMaterial ( ( G3DMESH * ) mes, mat, NULL, map );
        } else {
            if ( obj->type == G3DUVMAPTYPE ){
                G3DMESH  *mes = ( G3DMESH * ) obj->parent;
                G3DUVMAP *map = ( G3DUVMAP* ) obj;

                g3dmesh_addMaterial ( ( G3DMESH * ) mes, mat, NULL, map );
            } else {
                fprintf ( stderr, "No material selected\n" );
            }
        }
    }

    redrawGLViews ( gui );
}

/******************************************************************************/
void addmaterialcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DMATERIAL *mat = g3dmaterial_new ( "Material" );
    LIST *ltmp = gui->lmatlist;

    g3dscene_addMaterial ( gui->sce, mat );

    while ( ltmp ) {
        Widget w = ( Widget ) ltmp->data;

        XmMaterialListAddMaterial ( w, mat->name, mat );

        ltmp = ltmp->next;
    }

    gui->selmat = mat;

    redrawMaterialLists ( gui );
}

/******************************************************************************/
void delmaterialcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DMATERIAL *mat = gui->selmat;
    LIST *ltmp = gui->lmatlist;

    g3dscene_delMaterial ( gui->sce, mat );

    /*while ( ltmp ) {
        Widget w = ( Widget ) ltmp->data;

        delPreview ( w, mat );

        ltmp = ltmp->next;
    }*/

    gui->selmat = NULL;

    redrawMaterialLists ( gui );
}

/******************************************************************************/
static void startnetservercbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    Display *dis = ( gui->curogl ) ? XtDisplay ( gui->curogl ) : NULL;
    Window   win = ( gui->curogl ) ? XtWindow  ( gui->curogl ) : 0x00;
    G3DSCENE *sce = gui->sce;

    if ( win ) {
        Dimension width, height;
        Widget glpar = XtParent ( gui->curogl );
        G3DCAMERA *cam;
        pthread_attr_t attr;

        XtVaGetValues ( gui->curogl, XmNwidth, &width,
                                     XmNheight, &height,
                                     NULL );

        XtVaGetValues ( glpar, XtNcamera, &cam, NULL );

        gui->srv = r3dnetserver_new ( 0x80, 0x1980, sce, 
                                                    cam, width, height, 0x20,"/tmp" );

        if ( gui->srv ) {
            r3dnetserver_startCommunicationThread ( gui->srv );

            /*pthread_join ( srv->tid, NULL );

            r3dnetserver_free ( srv );*/
        }
    }
}

/******************************************************************************/
static void startnetclientcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    Display *dis = ( gui->curogl ) ? XtDisplay ( gui->curogl ) : NULL;
    Window   win = ( gui->curogl ) ? XtWindow  ( gui->curogl ) : 0x00;
    G3DSCENE *sce = gui->sce;

    if ( win ) {
        gui->clt = r3dnetclient_new ( "localhost", 0x1980, gui, dis, win );

        if ( gui->clt ) {
            r3dnetclient_startCommunicationThread ( gui->clt );


            /*pthread_join ( srv->tid, NULL );

            r3dnetserver_free ( srv );*/
        }
    }
}

/******************************************************************************/
static void renderscenecbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    Display *dis = XtDisplay ( w );
    Window root = XDefaultRootWindow ( dis );
    Pixel background, foreground;
    Dimension width, height;
    XWindowAttributes wat;
    Widget shell, edit;
    /*** double-click on the same tile as before ***/

    XtVaGetValues ( w, XmNbackground, &background, 
                       XmNforeground, &foreground, 
                       XmNwidth , &width,
                       XmNheight, &height, NULL );

    XGetWindowAttributes ( dis, root, &wat );

    shell = XtVaAppCreateShell ( NULL, "Class",
                                 topLevelShellWidgetClass,
                                 dis, 
                                 XtNtitle,"Render Settings",
                                 XtNx, ( wat.width/2) -200,
                                 XtNy, ( wat.height/2) -200,
                                 XtNwidth, 400,
                                 XtNheight, 400,
                                 XmNbackground, background, 
                                 XmNforeground, foreground,
                                 NULL );

    edit = createRenderSettings ( shell, gui, "Render Settings", 0x00, 0x00,
                                                                 400 , 400 );

    XtManageChild ( edit );

    XtRealizeWidget ( shell );
}

/******************************************************************************/
static void endnetrendercbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    Display *dis = ( gui->curogl ) ? XtDisplay ( gui->curogl ) : NULL;
    Window   win = ( gui->curogl ) ? XtWindow  ( gui->curogl ) : 0x00;
    G3DSCENE *sce = gui->sce;

    if ( gui->srv ) {
        r3dnetserver_end ( gui->srv );

        r3dnetserver_free ( gui->srv );
    }
}

/******************************************************************************/
static void runnetrendercbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    Display *dis = ( gui->curogl ) ? XtDisplay ( gui->curogl ) : NULL;
    Window   win = ( gui->curogl ) ? XtWindow  ( gui->curogl ) : 0x00;
    G3DSCENE *sce = gui->sce;

    if ( gui->srv ) {
        R3DNETFRAME *frm = r3dnetframe_new ( dis, gui->srv->resx,
                                                  gui->srv->resy,
                                                  gui->srv->depth, 0x00 );
        /*r3dnetserver_run ( gui->srv );*/

        r3dnetserver_assignframe ( gui->srv, frm );
        r3dnetserver_renderframe ( gui->srv, frm );
    }
}

/******************************************************************************/
static void addlightcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    uint32_t oid = g3dscene_getNextObjectID ( sce );
    G3DLIGHT *lig = g3dlight_new ( oid, "Light" );

    g3durm_object_addChild ( urm, sce, gui->flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) lig );

    /*** Below functions are called in g3durm_object_addChild ... ***/
    /*g3dscene_unselectAllObjects ( sce );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) lig );*/

    redrawGLViews        ( gui );
    updateCoords         ( gui );
    redrawObjectList     ( gui );
    updateAllCurrentEdit ( gui );
}

/******************************************************************************/
static void addcameracbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DCAMERA *curcam = gui->curcam;
    G3DURMANAGER *urm = gui->urm;
    uint32_t oid = g3dscene_getNextObjectID ( sce );
    G3DCAMERA *cam = g3dcamera_new ( oid, "Camera", curcam->focal,
                                                    curcam->ratio,
                                                    curcam->znear,
                                                    curcam->zfar );

    g3dcamera_setGrid ( cam, g3dcamera_grid3D );

    g3dobject_importTransformations ( ( G3DOBJECT * ) cam, 
                                      ( G3DOBJECT * ) gui->curcam );

    g3durm_object_addChild ( urm, sce, gui->flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) cam );

    memcpy ( &cam->pivot, &curcam->pivot, sizeof ( G3DVECTOR ) );

    /*g3dobject_updateMatrix_r ( ( G3DOBJECT * ) cam );*/

    /*** Below functions are called in g3durm_object_addChild ... ***/
    /*g3dscene_unselectAllObjects ( sce );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) lig );*/

    redrawGLViews        ( gui );
    updateCoords         ( gui );
    redrawObjectList     ( gui );
    updateAllCurrentEdit ( gui );
}

/******************************************************************************/
static void addspherecbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    uint32_t pid = g3dscene_getNextObjectID ( sce );
    G3DPRIMITIVE *pri = g3dsphere_new ( pid, "Sphere", 0x20, 0x10, 1.0f );

    g3durm_object_addChild ( urm, sce, gui->flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) pri );
    g3dscene_unselectAllObjects ( sce, gui->flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) pri, gui->flags );

    redrawGLViews        ( gui );
    updateCoords         ( gui );
    redrawObjectList     ( gui );
    updateAllCurrentEdit ( gui );
}

/******************************************************************************/
static void addplanecbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    uint32_t pid = g3dscene_getNextObjectID ( sce );
    G3DPRIMITIVE *pri = g3dplane_new ( pid, "Plane", 0x00, 
                                                     0x01, 0x01, 1.0f, 1.0f );

    g3durm_object_addChild ( urm, sce, gui->flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) pri );
    g3dscene_unselectAllObjects ( sce, gui->flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) pri, gui->flags );

    redrawGLViews ( gui );
    updateCoords ( gui );
    redrawObjectList ( gui );
    updateAllCurrentEdit ( gui );
}

/******************************************************************************/
static void addtoruscbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    uint32_t pid = g3dscene_getNextObjectID ( sce );
    G3DPRIMITIVE *pri = g3dtorus_new ( pid, "Torus", 0x00, 
                                                     0x18, 0x0C, 0.75f, 0.25f );

    g3durm_object_addChild ( urm, sce, gui->flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) pri );
    g3dscene_unselectAllObjects ( sce, gui->flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) pri, gui->flags );

    redrawGLViews ( gui );
    updateCoords ( gui );
    redrawObjectList ( gui );
    updateAllCurrentEdit ( gui );
}

/******************************************************************************/
static void addcubecbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    uint32_t pid = g3dscene_getNextObjectID ( sce );
    G3DPRIMITIVE *pri = g3dcube_new ( pid, "Cube", 0x01, 0x01, 0x01, 1.0f );

    g3durm_object_addChild ( urm, sce, gui->flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) pri );
    g3dscene_unselectAllObjects ( sce, gui->flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) pri, gui->flags );

    redrawGLViews ( gui );
    updateCoords ( gui );
    redrawObjectList ( gui );
    updateAllCurrentEdit ( gui );
}

/******************************************************************************/
static void addemptymeshcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    uint32_t pid = g3dscene_getNextObjectID ( sce );
    G3DMESH *mes = g3dmesh_new ( pid, "Mesh", gui->flags );

    g3durm_object_addChild ( urm, sce, gui->flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) mes );

    g3dscene_unselectAllObjects ( sce, gui->flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) mes, gui->flags );

    redrawGLViews ( gui );
    updateCoords ( gui );
    redrawObjectList ( gui );
    updateAllCurrentEdit ( gui );
}

/******************************************************************************/
static void addcylindercbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    uint32_t pid = g3dscene_getNextObjectID ( sce );
    G3DPRIMITIVE *pri = g3dcylinder_new ( pid, "Cylinder", 0x18,
                                                           0x01,
                                                           0x01, 1.0f, 1.0f );

    g3durm_object_addChild ( urm, sce, gui->flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) pri );

    g3dscene_unselectAllObjects ( sce, gui->flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) pri, gui->flags );

    redrawGLViews ( gui );
    updateCoords ( gui );
    redrawObjectList ( gui );
    updateAllCurrentEdit ( gui );
}

/******************************************************************************/
static void convSymmetrycbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type == G3DSYMMETRYTYPE ) ) {
        G3DSYMMETRY *sym = ( G3DSYMMETRY * ) obj;
        LIST *loldobj = NULL;
        G3DMESH *mes;

        g3dui_setHourGlass ( gui );

        mes = g3dsymmetry_convert ( sym, &loldobj, gui->flags );

        g3durm_object_addChild ( urm, sce, gui->flags, 
                                           ( REDRAWVIEW |
                                             REDRAWLIST | REDRAWCURRENTOBJECT ),
                                           ( G3DOBJECT * ) NULL,
                                           ( G3DOBJECT * ) sce,
                                           ( G3DOBJECT * ) mes );

        g3dui_unsetHourGlass ( gui );
        redrawGLViews ( gui );
        updateCoords ( gui );
        redrawObjectList ( gui );
        updateAllCurrentEdit ( gui );
    }
}

/******************************************************************************/
static void removeUnusedcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        if ( gui->flags & VIEWVERTEX ) {
            g3dmesh_removeUnusedVertices ( mes, NULL );
        }

        redrawGLViews ( gui );
        updateCoords ( gui );
        redrawObjectList ( gui );
        updateAllCurrentEdit ( gui );
    }
}

/******************************************************************************/
static void addffdboxcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
    uint32_t pid = g3dscene_getNextObjectID ( sce );
    G3DFFD *ffd = g3dffd_new ( pid, "FFD Box" );

    /*** let's create a default shape ***/
    g3dffd_shape ( ffd, 0x04, 0x04, 0x04, 3.0f, 3.0f, 3.0f );

    if ( obj ) {
        g3durm_object_addChild ( urm, sce, gui->flags, 
                                           ( REDRAWVIEW |
                                             REDRAWLIST | REDRAWCURRENTOBJECT ),
                                           ( G3DOBJECT * ) NULL,
                                           ( G3DOBJECT * ) obj,
                                           ( G3DOBJECT * ) ffd );
    } else { 
        g3durm_object_addChild ( urm, sce, gui->flags, 
                                           ( REDRAWVIEW |
                                             REDRAWLIST | REDRAWCURRENTOBJECT ),
                                           ( G3DOBJECT * ) NULL,
                                           ( G3DOBJECT * ) sce,
                                           ( G3DOBJECT * ) ffd );
    }

    redrawGLViews ( gui );
    updateCoords ( gui );
    redrawObjectList ( gui );
    updateAllCurrentEdit ( gui );
}

/******************************************************************************/
static void assignffdcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj ) {
        if ( obj->type == G3DFFDTYPE ) {
            G3DFFD *ffd = ( G3DFFD * ) obj;
            G3DOBJECT *parent = obj->parent;

            if ( parent->type == G3DMESHTYPE ) {
                G3DMESH *mes = ( G3DMESH * ) parent;

                g3dffd_assign ( ffd, mes );
            }
        }
    }
}

/******************************************************************************/
static void resetffdcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj ) {
        if ( obj->type == G3DFFDTYPE ) {
            G3DFFD *ffd = ( G3DFFD * ) obj;
            G3DOBJECT *parent = obj->parent;

            if ( parent->type == G3DMESHTYPE ) {
                G3DMESH *mes = ( G3DMESH * ) parent;

                g3dffd_unassign ( ffd );

                redrawGLViews ( gui );
            }
        }
    }
}

/******************************************************************************/
static void invertselcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;

            if ( gui->flags & VIEWFACE ) {
                g3dmesh_invertFaceSelection ( mes, gui->flags );
            }

            redrawGLViews ( gui );
        }
    }
}

/******************************************************************************/
Widget createMenuBarButton ( G3DUI *gui, Widget wmenu, char *name,
                             Dimension width,
                             void (*actioncbk) ( Widget, XtPointer, 
                                                         XtPointer ) ) {
    Pixel background, foreground;
    Widget wbtn;

    XtVaGetValues ( wmenu, XmNbackground, &background,
                           XmNforeground, &foreground, NULL );

    /************************************************/
    /*** XmCreateFileSelectionDialog() called by openfilecbk() crashes ***/
    /*** if wopen is a PushButtonGadget. So I replace it by a regular  ***/
    /*** PushButton ***/
    wbtn = XmVaCreatePushButton ( wmenu, name,
                                         XmNwidth, width,
                                         XmNrecomputeSize, False,
                                         XmNfontList, gui->fontlist,
                                         XmNhighlightThickness, 0x00,
                                         XmNshadowThickness, 0x00,
                                         XmNbackground, background,
                                         XmNforeground, 0x00,
                                         NULL );

    /*** I had some strange behavior using XmNarmCallback.  ***/
    /*** I switched to XmNactivateCallback. It works better ***/
    if ( actioncbk ) XtAddCallback ( wbtn, XmNactivateCallback, actioncbk, gui );

    XtManageChild ( wbtn );


    return wbtn;
}

/******************************************************************************/
Widget createMenuBar ( Widget parent, G3DUI *gui, char *name,
                       Dimension x    , Dimension y,
                       Dimension width, Dimension height ) {
    Pixel background, foreground;
    XmFontList fontlist;
    Widget wsym, wundo, wredo, wsaas, wtemp;
    Widget wsphere, wcylinder, wcube, wtorus, wmesh, wplane;
    Widget wfile, cfile;
    Widget wedit, cedit;
    Widget wprim, cprim; /*** Add primitives ***/
    Widget wmodi, cmodi;   /*** Modifiers ***/
    Widget wfunc, cfunc;
    Widget wffdb;
    Widget wrend, crend; /*** Net Render ***/
    Widget wnetr; /*** Net Render ***/
    Widget menubar;
    Widget wmirr, cmirr;
    Widget wimpo, cimpo, wexpo, cexpo;
    Widget wtree, ctree;
    XmString strzx, strxy, stryz, str3ds, strdae, strc4d, strobj, fulltree, sametree,
             clockwise, counterclockwise;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    menubar = XmVaCreateSimpleMenuBar ( parent, MENUBARNAME,
                                        XmNfontList, gui->fontlist,
                                        XmNheight, 0x20,
                                        XmNwidth, width,
                                        XmNhighlightThickness, 0x01,
                                        XmNshadowThickness, 0x01,
                                        XmNbackground, background,
                                        XmNforeground, foreground,
                                        NULL );

    /************************************************/
    wfile = XmCreatePulldownMenu ( menubar, "FilePullDown", NULL, 0x00 );
    XtVaSetValues ( wfile, XmNhighlightThickness, 0x00,
                           XmNshadowThickness, 0x01, NULL );

    cfile = XmVaCreateCascadeButton ( menubar, "File",
                                      XmNwidth, 0x60,
                                      XmNrecomputeSize, False,
                                      XmNfontList, gui->fontlist,
                                      XmNhighlightThickness, 0x01,
                                      XmNshadowThickness, 0x01,
                                      XmNbackground, background,
                                      XmNforeground, 0x00,
                                      XmNsubMenuId, wfile,
                                      NULL );

    createMenuBarButton ( gui, wfile, "New"    , 0x60, newscenecbk );
    createMenuBarButton ( gui, wfile, "Open"   , 0x60, openfilecbk );

    str3ds = XmStringCreateLocalized ( "3D Studio .3ds" );
    strobj = XmStringCreateLocalized ( "Wavefront .obj" );
#ifdef HAVE_EXPAT_H
    strdae = XmStringCreateLocalized ( "Collada .dae"   );
#endif
#ifdef HAVE_C4D_H
    strc4d = XmStringCreateLocalized ( "Cinema4D .c4d"  );
#endif
    wimpo = XmVaCreateSimplePulldownMenu ( wfile, "Import",
                                           0x00, importfilecbk,
                                           XmVaPUSHBUTTON, str3ds, 0, NULL, NULL,
                                           XmVaPUSHBUTTON, strobj, 0, NULL, NULL,
#ifdef HAVE_EXPAT_H
                                           XmVaPUSHBUTTON, strdae, 0, NULL, NULL,
#endif
#ifdef HAVE_C4D_H
                                           XmVaPUSHBUTTON, strc4d, 0, NULL, NULL,
#endif
                                           XmNuserData, gui,
                                           NULL );

    cimpo = XmVaCreateCascadeButton ( wfile, "Import File",
                                      XmNwidth, 0x80,
                                      XmNrecomputeSize, False,
                                      XmNfontList, gui->fontlist,
                                      XmNhighlightThickness, 0x01,
                                      XmNshadowThickness, 0x01,
                                      XmNbackground, background,
                                      XmNforeground, 0x00,
                                      XmNsubMenuId, wimpo,
                                      NULL );

    XmStringFree ( str3ds );
#ifdef HAVE_EXPAT_H
    XmStringFree ( strdae );
#endif
#ifdef HAVE_C4D_H
    XmStringFree ( strc4d );
#endif
    XtManageChild ( cimpo );

    createMenuBarButton ( gui, wfile, "Save"   , 0x60, savefilecbk );
    createMenuBarButton ( gui, wfile, "Save As", 0x60, saveascbk   );

    /*** EXPORT ***************************************************************/
    strobj = XmStringCreateLocalized ( "Wavefront .obj" );

    wexpo = XmVaCreateSimplePulldownMenu ( wfile, "Export",
                                           0x00, exportfilecbk,
                                           XmVaPUSHBUTTON, strobj, 0, NULL, NULL,
                                           XmNuserData, gui,
                                           NULL );

    cexpo = XmVaCreateCascadeButton ( wfile, "Export File",
                                      XmNwidth, 0x80,
                                      XmNrecomputeSize, False,
                                      XmNfontList, gui->fontlist,
                                      XmNhighlightThickness, 0x01,
                                      XmNshadowThickness, 0x01,
                                      XmNbackground, background,
                                      XmNforeground, 0x00,
                                      XmNsubMenuId, wexpo,
                                      NULL );

    XmStringFree ( strobj );

    XtManageChild ( cexpo );
    /**************************************************************************/

    XtManageChild ( XmCreateSeparatorGadget  ( wfile, "SEPA", NULL, 0x00 ) );

    createMenuBarButton ( gui, wfile, "Exit"   , 0x60, exitcbk        );

    XtManageChild ( cfile );

    /************************************************/
    wedit = XmCreatePulldownMenu ( menubar, "EditPullDown", NULL, 0x00 );
    XtVaSetValues ( wedit, XmNhighlightThickness, 0x00,
                           XmNshadowThickness, 0x01, NULL );

    cedit = XmVaCreateCascadeButton ( menubar, "Edit",
                                      XmNwidth, 0x60,
                                      XmNrecomputeSize, False,
                                      XmNfontList, gui->fontlist,
                                      XmNhighlightThickness, 0x01,
                                      XmNshadowThickness, 0x01,
                                      XmNbackground, background,
                                      XmNforeground, 0x00,
                                      XmNsubMenuId, wedit,
                                      NULL );

    createMenuBarButton ( gui, wedit, "Undo", 0x60, undocbk );
    createMenuBarButton ( gui, wedit, "Redo", 0x60, undocbk );

    XtManageChild ( XmCreateSeparatorGadget  ( wedit, "SEPA", NULL, 0x00 ) );

    createMenuBarButton ( gui, wedit, "Invert Normal"   , 0x60, invertNormalcbk  );
    createMenuBarButton ( gui, wedit, "Align Faces"     , 0x60, alignNormalscbk  );

    clockwise        = XmStringCreateLocalized ( "Clockwise" );
    counterclockwise = XmStringCreateLocalized ( "Counter clockwise" );

    wtree = XmVaCreateSimplePulldownMenu ( wedit, "Triangulate",
                                           0x00, triangulatecbk,
                                           XmVaPUSHBUTTON, clockwise       , 0, NULL, NULL,
                                           XmVaPUSHBUTTON, counterclockwise, 0, NULL, NULL,
                                           XmNhighlightThickness, 0x00,
                                           XmNshadowThickness, 0x01,
                                           XmNfontList, gui->fontlist,
                                           XmNrenderTable, gui->renTable,
                                           XmNuserData, gui,
                                           NULL );

    ctree = XmVaCreateCascadeButton ( wedit, "Triangulate",
                                      XmNwidth, 0x80,
                                      XmNrecomputeSize, False,
                                      XmNfontList, gui->fontlist,
                                      XmNhighlightThickness, 0x00,
                                      XmNshadowThickness, 0x01,
                                      XmNbackground, background,
                                      XmNforeground, 0x00,
                                      XmNsubMenuId, wtree,
                                      NULL );

    XmStringFree ( clockwise        );
    XmStringFree ( counterclockwise );

    XtManageChild ( ctree );




    createMenuBarButton ( gui, wedit, "Untriangulate"   , 0x60, untriangulatecbk );
    createMenuBarButton ( gui, wedit, "Weld Vertices"   , 0x60, weldVerticescbk  );
    createMenuBarButton ( gui, wedit, "Delete lone vertices", 0x60, removeUnusedcbk  );
    createMenuBarButton ( gui, wedit, "Convert Symmetry", 0x60, convSymmetrycbk  );

    fulltree = XmStringCreateLocalized ( "All types" );
    sametree = XmStringCreateLocalized ( "Same type" );

    wtree = XmVaCreateSimplePulldownMenu ( wedit, "Select tree",
                                           0x00, selecttreecbk,
                                           XmVaPUSHBUTTON, fulltree, 0, NULL, NULL,
                                           XmVaPUSHBUTTON, sametree, 0, NULL, NULL,
                                           XmNhighlightThickness, 0x00,
                                           XmNshadowThickness, 0x01,
                                           XmNfontList, gui->fontlist,
                                           XmNrenderTable, gui->renTable,
                                           XmNuserData, gui,
                                           NULL );

    ctree = XmVaCreateCascadeButton ( wedit, "Select tree",
                                      XmNwidth, 0x80,
                                      XmNrecomputeSize, False,
                                      XmNhighlightThickness, 0x00,
                                      XmNshadowThickness, 0x01,
                                      XmNfontList, gui->fontlist,
                                      XmNrenderTable, gui->renTable,
                                      XmNbackground, background,
                                      XmNforeground, 0x00,
                                      XmNsubMenuId, wtree,
                                      NULL );

    XmStringFree ( fulltree );
    XmStringFree ( sametree );

    XtManageChild ( ctree );

    createMenuBarButton ( gui, wedit, "Invert Selection", 0x60, invertselcbk  );

    XtManageChild ( cedit );

    /************************************************/
    wprim = XmCreatePulldownMenu ( menubar, "PrimitivePullDown", NULL, 0x00 );
    XtVaSetValues ( wprim, XmNhighlightThickness, 0x00,
                           XmNshadowThickness, 0x01, NULL );

    cprim = XmVaCreateCascadeButton ( menubar, "Objects",
                                      XmNwidth, 0x60,
                                      XmNrecomputeSize, False,
                                      XmNfontList, gui->fontlist,
                                      XmNhighlightThickness, 0x01,
                                      XmNshadowThickness, 0x01,
                                      XmNbackground, background,
                                      XmNforeground, 0x00,
                                      XmNsubMenuId, wprim,
                                      NULL );

    createMenuBarButton ( gui, wprim, "Add plane"   , 0x60, addplanecbk    );
    createMenuBarButton ( gui, wprim, "Add cube"    , 0x60, addcubecbk     );
    createMenuBarButton ( gui, wprim, "Add sphere"  , 0x60, addspherecbk   );
    createMenuBarButton ( gui, wprim, "Add torus"   , 0x60, addtoruscbk    );
    createMenuBarButton ( gui, wprim, "Add cylinder", 0x60, addcylindercbk );

    XtManageChild ( XmCreateSeparatorGadget  ( wprim, "SEPA", NULL, 0x00 ) );

    createMenuBarButton ( gui, wprim, "Add light"   , 0x60, addlightcbk     );

    XtManageChild ( XmCreateSeparatorGadget  ( wprim, "SEPA", NULL, 0x00 ) );

    /*createMenuBarButton ( gui, wprim, "Add camera"  , 0x60, addcameracbk   );

    XtManageChild ( XmCreateSeparatorGadget  ( wprim, "SEPA", NULL, 0x00 ) );*/

    createMenuBarButton ( gui, wprim, "Add empty mesh", 0x60, addemptymeshcbk );


    XtManageChild ( cprim );

    /************************************************/
    wmodi = XmCreatePulldownMenu ( menubar, "ModifierPullDown", NULL, 0x00 );
    XtVaSetValues ( wmodi, XmNhighlightThickness, 0x00,
                           XmNshadowThickness, 0x01, NULL );

    cmodi = XmVaCreateCascadeButton ( menubar, "Modifiers",
                                      XmNwidth, 0x60,
                                      XmNrecomputeSize, False,
                                      XmNfontList, gui->fontlist,
                                      XmNhighlightThickness, 0x01,
                                      XmNshadowThickness, 0x01,
                                      XmNbackground, background,
                                      XmNforeground, 0x00,
                                      XmNsubMenuId, wmodi,
                                      NULL );

    createMenuBarButton ( gui, wmodi, "Add Symmetry"   , 0x60, addsymmetrycbk );
    createMenuBarButton ( gui, wmodi, "Add FFD Box"    , 0x60, addffdboxcbk   );
    createMenuBarButton ( gui, wmodi, "Assign FFD"     , 0x60, assignffdcbk   );
    createMenuBarButton ( gui, wmodi, "Reset FFD"      , 0x60, resetffdcbk    );
    createMenuBarButton ( gui, wmodi, "Add UVW Map"    , 0x60, adduvwmapcbk   );
    createMenuBarButton ( gui, wmodi, "Add Bone"       , 0x60, addbonecbk     );
    createMenuBarButton ( gui, wmodi, "Fix Bone"       , 0x60, fixbonecbk     );
    createMenuBarButton ( gui, wmodi, "Reset Bone"     , 0x60, resetbonecbk   );
    createMenuBarButton ( gui, wmodi, "Fix Bone Tree"  , 0x60, fixbonetreecbk   );
    createMenuBarButton ( gui, wmodi, "Reset Bone Tree", 0x60, resetbonetreecbk );

    XtManageChild ( cmodi );

    /************************************************/
    wfunc = XmCreatePulldownMenu ( menubar, "ModifierPullDown", NULL, 0x00 );
    XtVaSetValues ( wfunc, XmNhighlightThickness, 0x00,
                           XmNshadowThickness, 0x01, NULL );

    cfunc = XmVaCreateCascadeButton ( menubar, "Functions",
                                      XmNwidth, 0x80,
                                      XmNrecomputeSize, False,
                                      XmNfontList, gui->fontlist,
                                      XmNhighlightThickness, 0x01,
                                      XmNshadowThickness, 0x01,
                                      XmNbackground, background,
                                      XmNforeground, 0x00,
                                      XmNsubMenuId, wfunc,
                                      NULL );
/*
    createMenuBarButton ( gui, wfunc, "Mirror Weightgroup", 0x80, NULL );
*/
    strzx = XmStringCreateLocalized ( "Along ZX plane" );
    strxy = XmStringCreateLocalized ( "Along XY plane" );
    stryz = XmStringCreateLocalized ( "Along YZ plane" );

    wmirr = XmVaCreateSimplePulldownMenu ( wfunc, "Mirror",
                                           0x00, mirrorWeightgroupcbk,
                                           XmVaPUSHBUTTON, strzx, 0, NULL, NULL,
                                           XmVaPUSHBUTTON, strxy, 0, NULL, NULL,
                                           XmVaPUSHBUTTON, stryz, 0, NULL, NULL,
                                           XmNhighlightThickness, 0x00,
                                           XmNshadowThickness, 0x01,
                                           XmNuserData, gui,
                                           NULL );



    cmirr = XmVaCreateCascadeButton ( wfunc, "Mirror Weightgroup",
                                      XmNwidth, 0x80,
                                      XmNrecomputeSize, False,
                                      XmNfontList, gui->fontlist,
                                      XmNhighlightThickness, 0x00,
                                      XmNshadowThickness, 0x01,
                                      XmNbackground, background,
                                      XmNforeground, 0x00,
                                      XmNsubMenuId, wmirr,
                                      NULL );

    XtManageChild ( cmirr );

    /*wmirr = XmVaCreateSimplePulldownMenu ( wfunc, "Mirror",
                                           0x00, mirrorSkeletoncbk,
                                           XmVaPUSHBUTTON, strzx, 0, NULL, NULL,
                                           XmVaPUSHBUTTON, strxy, 0, NULL, NULL,
                                           XmVaPUSHBUTTON, stryz, 0, NULL, NULL,
                                           XmNuserData, gui,
                                           NULL );



    cmirr = XmVaCreateCascadeButton ( wfunc, "Mirror Bone",
                                      XmNwidth, 0x80,
                                      XmNrecomputeSize, False,
                                      XmNfontList, gui->fontlist,
                                      XmNhighlightThickness, 0x01,
                                      XmNshadowThickness, 0x01,
                                      XmNbackground, background,
                                      XmNforeground, 0x00,
                                      XmNsubMenuId, wmirr,
                                      NULL );

    XtManageChild ( cmirr );*/

    XmStringFree ( strzx );
    XmStringFree ( strxy );
    XmStringFree ( stryz );

    XtManageChild ( cfunc );

    /************************************************/
#ifdef LATER
    wsym = XmVaCreatePushButtonGadget ( wedit, "Add Material",
                                        XmNwidth, 0x60,
                                        XmNrecomputeSize, False,
                                        XmNfontList, gui->fontlist,
                                        XmNhighlightThickness, 0x00,
                                        XmNshadowThickness, 0x00,
                                        XmNbackground, background,
                                        XmNforeground, 0x00,
                                        NULL );
    XtAddCallback ( wsym, XmNarmCallback, addmaterialcbk, gui );
    XtManageChild ( wsym );

    wsym = XmVaCreatePushButtonGadget ( wedit, "Set Material",
                                        XmNwidth, 0x60,
                                        XmNrecomputeSize, False,
                                        XmNfontList, gui->fontlist,
                                        XmNhighlightThickness, 0x00,
                                        XmNshadowThickness, 0x00,
                                        XmNbackground, background,
                                        XmNforeground, 0x00,
                                        NULL );
    XtAddCallback ( wsym, XmNarmCallback, setmaterialcbk, gui );
    XtManageChild ( wsym );

    /**************************************************************************/
    /**************************************************************************/
    /**************************************************************************/
#endif

    wrend = XmCreatePulldownMenu ( menubar, "FilePullDown", NULL, 0x00 );
    XtVaSetValues ( wrend, XmNhighlightThickness, 0x00,
                           XmNshadowThickness, 0x01, NULL );

    crend = XmVaCreateCascadeButton ( menubar, "Render",
                                      XmNwidth, 0x60,
                                      XmNrecomputeSize, False,
                                      XmNfontList, gui->fontlist,
                                      XmNhighlightThickness, 0x01,
                                      XmNshadowThickness, 0x01,
                                      XmNbackground, background,
                                      XmNforeground, 0x00,
                                      XmNsubMenuId, wrend,
                                      NULL );

    /************************************************/
    /*wnetr = XmVaCreatePushButtonGadget ( wrend, "Net Server", 
                                         XmNwidth, 0x60,
                                         XmNrecomputeSize, False,
                                         XmNfontList, gui->fontlist,
                                         XmNhighlightThickness, 0x00,
                                         XmNshadowThickness, 0x00,
                                         XmNbackground, background,
                                         XmNforeground, 0x00,
                                         NULL );
    XtAddCallback ( wnetr, XmNactivateCallback, startnetservercbk, gui );
    XtManageChild ( wnetr );*/


    /************************************************/
    /*wnetr = XmVaCreatePushButtonGadget ( wrend, "Net Client", 
                                         XmNwidth, 0x60,
                                         XmNrecomputeSize, False,
                                         XmNfontList, gui->fontlist,
                                         XmNhighlightThickness, 0x00,
                                         XmNshadowThickness, 0x00,
                                         XmNbackground, background,
                                         XmNforeground, 0x00,
                                         NULL );

    XtAddCallback ( wnetr, XmNactivateCallback, startnetclientcbk, gui );
    XtManageChild ( wnetr );*/

    /************************************************/
    /*wnetr = XmVaCreatePushButtonGadget ( wrend, "Run", 
                                         XmNwidth, 0x60,
                                         XmNrecomputeSize, False,
                                         XmNfontList, gui->fontlist,
                                         XmNhighlightThickness, 0x00,
                                         XmNshadowThickness, 0x00,
                                         XmNbackground, background,
                                         XmNforeground, 0x00,
                                         NULL );

    XtAddCallback ( wnetr, XmNactivateCallback, runnetrendercbk, gui );
    XtManageChild ( wnetr );*/

    /************************************************/
    /*wnetr = XmVaCreatePushButtonGadget ( wrend, "Stop", 
                                         XmNwidth, 0x60,
                                         XmNrecomputeSize, False,
                                         XmNfontList, gui->fontlist,
                                         XmNhighlightThickness, 0x00,
                                         XmNshadowThickness, 0x00,
                                         XmNbackground, background,
                                         XmNforeground, 0x00,
                                         NULL );

    XtAddCallback ( wnetr, XmNactivateCallback, runnetrendercbk, gui );
    XtManageChild ( wnetr );*/

    /************************************************/
    wnetr = XmVaCreatePushButtonGadget ( wrend, "Render scene", 
                                         XmNwidth, 0x60,
                                         XmNrecomputeSize, False,
                                         XmNfontList, gui->fontlist,
                                         XmNhighlightThickness, 0x00,
                                         XmNshadowThickness, 0x00,
                                         XmNbackground, background,
                                         XmNforeground, 0x00,
                                         NULL );

    XtAddCallback ( wnetr, XmNactivateCallback, renderscenecbk, gui );
    XtManageChild ( wnetr );

    XtManageChild ( crend );
    /*XtManageChild ( wrend );*/



    XtManageChild ( menubar );

    return menubar;
}
