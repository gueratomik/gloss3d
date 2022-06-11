
/******************************************************************************/
void restrictcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DMOUSETOOL *mou = common_g3dui_getMouseTool ( gui, CUTMESHTOOL );
    G3DCUTMESH   *cm = mou->data;
    XmToggleButtonCallbackStruct *tbs = ( XmToggleButtonCallbackStruct * ) call;

    if ( gui->lock ) return;

    if ( tbs->set ) cm->restrict_to_selection = 0x00;
    else            cm->restrict_to_selection = 0x01;
}

/******************************************************************************/
void updateCutMeshToolEdit ( Widget w ) {
    WidgetList children;
    G3DMOUSETOOL *mou;
    Cardinal nc;
    G3DUI *gui;
    int i;

    XtVaGetValues ( w, XmNuserData, &gui,
                       XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    mou = common_g3dui_getMouseTool ( gui, CUTMESHTOOL );

    gui->lock = 0x01;

    if ( mou ) {
        G3DCUTMESH *cm = mou->data;

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
 
            if ( XtClass ( child ) == xmToggleButtonWidgetClass ) {
                if ( strcmp ( XtName ( child ), "Restrict to selection" ) == 0x00 ) {
                    if ( cm->restrict_to_selection ) {
                        XtVaSetValues ( child, XmNset, True, NULL );
                    } else {
                        XtVaSetValues ( child, XmNset, False, NULL );
                    }
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
Widget createKnifeToolEdit ( Widget parent, G3DUI *gui, char *name,
                             Dimension x    , Dimension y,
                             Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget lab, frm;


    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    frm = XmVaCreateManagedForm ( parent, name,
                                  XtNx, 0x00,
                                  XtNy, 0x00,
                                  XmNwidth , 0x140,
                                  XmNheight, 0x140,
                                  XmNforeground, foreground,
                                  XmNbackground, background,
                                  XmNborderWidth, 0x00,
                                  XmNuserData, gui,
                                  NULL );

    lab = XmVaCreateManagedToggleButton ( frm, "Restrict to selection",
                                          XtNx, 0x00,
                                          XtNy, 0x00,
                                          XmNwidth , 0xC0,
                                          XmNheight, 0x20,
                                          XmNforeground, foreground,
                                          XmNbackground, background,
                                          XmNborderWidth, 0x00,
                                          XmNuserData, gui,
                                          NULL );

    XtAddCallback ( lab, XmNarmCallback, restrictcbk, gui );


    return frm;
}
