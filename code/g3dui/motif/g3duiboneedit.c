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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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

/******************************************************************************/
static void updateWeightGroupList     ( Widget, G3DOBJECT *, G3DUI *gui );
static void updateWeightGroupComboBox ( Widget, G3DOBJECT *, G3DUI *gui );

/******************************************************************************/
void updateBoneEdit ( Widget w ) {
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

    if ( obj && ( obj->type == G3DBONETYPE ) ) {
        G3DBONE *bon = ( G3DBONE * ) obj;
        Cardinal nc;
        int i;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
            char *name = XtName ( child );

            if ( XtClass ( child ) == xmComboBoxWidgetClass ) {
                if ( strcmp ( name, EDITBONEVERTEXGROUP   ) == 0x00 ) {
                    updateWeightGroupComboBox ( child, obj, gui );
                }
            }

            if ( XtClass ( child ) == xmListWidgetClass ) {
                if ( strcmp ( name, EDITBONEVERTEXGROUP   ) == 0x00 ) {
                    updateWeightGroupList ( child, obj, gui );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
Widget boneEdit_getVertexGroupComboBox ( Widget w ) {
    WidgetList children;
    Cardinal nc;
    int i;

    XtVaGetValues ( w, XmNchildren   , &children, 
                       XmNnumChildren, &nc, 
                       NULL );

    for ( i = 0x00; i < nc; i ++ ) {
        Widget child = children[i];

        if (        ( XtClass ( child ) == xmComboBoxWidgetClass ) && 
             strcmp ( XtName  ( child ), EDITBONEVERTEXGROUP ) == 0x00 ) {

            return child;
        }
    }
}

/******************************************************************************/
Widget boneEdit_getVertexGroupList ( Widget w ) {
    WidgetList children;
    Cardinal nc;
    int i;

    XtVaGetValues ( w, XmNchildren   , &children, 
                       XmNnumChildren, &nc, 
                       NULL );

    for ( i = 0x00; i < nc; i ++ ) {
        Widget child = children[i];

        if (        ( XtClass ( child ) == xmListWidgetClass ) && 
             strcmp ( XtName  ( child ), EDITBONEVERTEXGROUP ) == 0x00 ) {

            return child;
        }
    }
}

/******************************************************************************/
static void updateWeightGroupComboBox ( Widget w, G3DOBJECT *obj, G3DUI *gui ) {
    G3DBONE *bon = ( G3DBONE * ) obj;
    LIST *lgrp = g3dbone_seekWeightGroups ( bon );
    LIST *ltmp = lgrp;
    uint32_t nb = 0x00;
    Widget lst, txt;

    XtVaGetValues ( w, XmNlist     , &lst,
                       XmNtextField, &txt, NULL );

    XmListDeleteAllItems ( lst );

    /*** clear the text field ***/
    XtVaSetValues ( txt, XmNvalue, "", NULL );

    while ( ltmp ) {
        G3DWEIGHTGROUP *grp = ( G3DWEIGHTGROUP * ) ltmp->data;
        char buf[0x100];
        XmString str;

        snprintf ( buf, 0x100, "%s/%s", ((G3DOBJECT*)grp->mes)->name, grp->name );

        str = XmStringCreate ( buf, XmFONTLIST_DEFAULT_TAG );

        XmListAddItem ( lst, str, 0x00 );

        XmStringFree ( str );

        if ( nb++ < 0x08 ) {
            XtVaSetValues ( w, XmNvisibleItemCount, nb, NULL );
        }

        ltmp = ltmp->next;
    }

    list_free ( &lgrp, NULL );
}

/******************************************************************************/
static void updateWeightGroupList ( Widget w, G3DOBJECT *obj, G3DUI *gui ) {
    if ( obj && ( obj->type == G3DBONETYPE ) ) {
        G3DBONE *bon = ( G3DBONE * ) obj;
        LIST *ltmp = bon->lrig;

        XmListDeleteAllItems ( w );

        while ( ltmp ) {
            G3DRIG *rig = ( G3DRIG * ) ltmp->data;
            G3DWEIGHTGROUP *grp = rig->grp;
            char buf[0x100];
            XmString str;

            snprintf ( buf, 0x100, "%s/%s", ((G3DOBJECT*)grp->mes)->name, grp->name );

            str = XmStringCreate ( buf, XmFONTLIST_DEFAULT_TAG );

            XmListAddItem ( w, str, 0x00 );

            XmStringFree ( str );

            ltmp = ltmp->next;
        }
    }
}

/******************************************************************************/
static void addvergrpcbk ( Widget w, XtPointer client, XtPointer call ) {
    Widget parent = XtParent ( w );
    Widget grplst, cmbbox;
    WidgetList children;
    G3DOBJECT *obj;
    G3DSCENE *sce;
    G3DUI *gui;
    int grpid;

    XtVaGetValues ( parent, XmNuserData, &gui, NULL );

    sce = gui->sce;
    obj = g3dscene_getSelectedObject ( sce );

    grplst = boneEdit_getVertexGroupList     ( parent );
    cmbbox = boneEdit_getVertexGroupComboBox ( parent );

    XtVaGetValues ( cmbbox, XmNselectedPosition, &grpid, NULL );

    /*XmString item;
    Widget lst;

    lst = meshEdit_getVertexGroupList ( parent );

    item = XmStringCreate ( "Test", XmFONTLIST_DEFAULT_TAG );

    XmListAddItem ( lst, item, 0x00 );

    XmStringFree ( item );*/

    if ( obj && ( obj->type == G3DBONETYPE ) ) {
        G3DBONE *bon = ( G3DBONE * ) obj;
        G3DWEIGHTGROUP *grp = g3dbone_seekWeightGroupByID ( bon, grpid );

        /*** actually grp could be NULL because grpid equals 0 when there ***/
        /*** is no item in the list AND where the item #0 is selected ***/
        if ( grp ) {
            g3dbone_addWeightGroup ( bon, grp );

            updateWeightGroupComboBox ( cmbbox, obj, gui );
            updateWeightGroupList     ( grplst, obj, gui );
        }
    }
}

/******************************************************************************/
static void delvergrpcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( w );
    Widget grplst, cmbbox;
    G3DOBJECT *obj;
    G3DSCENE *sce;

    sce = gui->sce;
    obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type == G3DBONETYPE ) ) {
        G3DBONE *bon = ( G3DBONE * ) obj;
        int *posarray = NULL;
        int  poscount = 0x00;
        int i;

        grplst = boneEdit_getVertexGroupList ( parent );
        cmbbox = boneEdit_getVertexGroupComboBox ( parent );

        XmListGetSelectedPos ( grplst, &posarray, &poscount );

        for ( i = 0x00; i < poscount; i++ ) {
            int position = posarray[i] - 0x01;
            G3DRIG *rig = list_seekbyrank ( bon->lrig, position );

            g3dbone_removeWeightGroup ( bon, rig->grp );
        }

        if ( posarray ) free ( posarray );

        updateWeightGroupComboBox ( cmbbox, obj, gui );
        updateWeightGroupList     ( grplst, obj, gui );
        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
static void selvergrpcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmListCallbackStruct *lcs = ( XmListCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    G3DOBJECT *obj;
    G3DSCENE *sce;

    sce = gui->sce;
    obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type == G3DBONETYPE ) ) {
        G3DBONE *bon = ( G3DBONE * ) obj;

        if ( lcs->reason == XmCR_SINGLE_SELECT ) {
            int position = ( lcs->item_position - 0x01 );
            G3DRIG *rig = list_seekbyrank ( bon->lrig, position );
            G3DWEIGHTGROUP *grp = rig->grp;
            G3DMESH *mes = grp->mes;

            g3dmesh_selectWeightGroup ( mes, grp );

            g3dui_redrawGLViews ( gui );
        }
    }
}

/******************************************************************************/
Widget createBoneEdit ( Widget parent  , G3DUI *gui, char *name,
                        Dimension x    , Dimension y,
                        Dimension width, Dimension height ) {
    Pixel white = XWhitePixel ( XtDisplay ( parent ), 0x00 );
    Pixel background, foreground;
    Widget frm, grplst, grpadd, grpdel, cmbbox;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

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

    createSimpleLabel ( frm, EDITBONEVERTEXGROUP, 0x00, 0x00, 0x100 );

    cmbbox = XmVaCreateManagedComboBox ( frm, EDITBONEVERTEXGROUP, 
                                         XmNx, 0x00,
                                         XmNy, 0x14,
                                         XmNwidth , 0x100,
                                         XmNhighlightThickness, 0x00,
                                         XmNshadowThickness, 0x01,
                                         XmNmarginHeight, 0x00,
                                         XmNmarginWidth, 0x00,
                                         XmNitemCount, 0x00,
                                         XmNfontList, gmt->fontlist,
                                         XmNarrowSize, 0x0C,
                                         XmNvisibleItemCount, 0x01,
                                         XmNcomboBoxType, XmDROP_DOWN_LIST,
                                         XmNrenderTable, gmt->renTable,
                                         XmNtraversalOn, False,
                                         XtNbackground, background,
                                         XtNforeground, foreground,
                                         NULL );

    /*** OpenMotif prior to 2.3.4 crashes if this is set before managing ***/
    XtVaSetValues ( cmbbox, XmNheight, 0x12, NULL );

    grplst = XmVaCreateManagedList ( frm, EDITBONEVERTEXGROUP, 
                                     XmNx, 0x00,
                                     XmNy, 0x2A,
                                     XmNwidth , 0x100,
                                     /*XmNheight,  0x60,*/
                                     /*XmNhighlightThickness, 0x00,
                                     XmNshadowThickness, 0x01,*/
                                     XmNfontList, gmt->fontlist,
                                     XmNtraversalOn, False, /* crashes if absent */
                                     XtNbackground, white,
                                     XtNforeground, foreground,
                                     XmNlistSizePolicy, XmCONSTANT,
                                     XmNscrollBarDisplayPolicy, XmSTATIC,
                                     XmNselectionPolicy, XmSINGLE_SELECT,
                                     XmNvisibleItemCount, 0x06,
                                     NULL );

    XtAddCallback ( grplst, XmNsingleSelectionCallback, selvergrpcbk, gui );

    createSimplePushButton ( frm, "+", 0x104, 0x40, 0x20, 0x20, addvergrpcbk );
    createSimplePushButton ( frm, "-", 0x104, 0x64, 0x20, 0x20, delvergrpcbk );


    XtManageChild ( frm );

    return frm;
}
