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

/******************************************************************************/
static void zscacbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DURMANAGER *urm = NULL;
    G3DSCENE     *sce = NULL;
    G3DOBJECT    *obj = NULL;
    G3DUI        *gui = NULL;

    /*** retrieve the global GUI structure ***/
    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    /*** prevents a loop when refreshing the widget ***/
    if ( gui->lock ) return;

    sce = gui->sce;
    urm = gui->urm;
    obj = g3dscene_getLastSelected ( sce );

    if ( obj ) {
        char *value = XmTextGetString ( w );
        float zsca = strtof ( value, NULL );

        g3dui_setHourGlass ( gui );

        if ( gui->flags & VIEWOBJECT ) {
            G3DVECTOR oldpos, oldrot, oldsca;

            memcpy ( &oldpos, &obj->pos, sizeof ( G3DVECTOR ) );
            memcpy ( &oldrot, &obj->rot, sizeof ( G3DVECTOR ) );
            memcpy ( &oldsca, &obj->sca, sizeof ( G3DVECTOR ) );

            obj->sca.z = zsca;

            g3dobject_updateMatrix_r ( obj, gui->flags );

            g3durm_object_move ( urm, obj, &oldpos,  
                                           &oldrot, 
                                           &oldsca, REDRAWVIEW );
        }

        g3dui_unsetHourGlass ( gui );

        XtFree ( value );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
static void yscacbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DURMANAGER *urm = NULL;
    G3DSCENE     *sce = NULL;
    G3DOBJECT    *obj = NULL;
    G3DUI        *gui = NULL;

    /*** retrieve the global GUI structure ***/
    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    /*** prevents a loop when refreshing the widget ***/
    if ( gui->lock ) return;

    sce = gui->sce;
    urm = gui->urm;
    obj = g3dscene_getLastSelected ( sce );

    if ( obj ) {
        char *value = XmTextGetString ( w );
        float ysca = strtof ( value, NULL );

        g3dui_setHourGlass ( gui );

        if ( gui->flags & VIEWOBJECT ) {
            G3DVECTOR oldpos, oldrot, oldsca;

            memcpy ( &oldpos, &obj->pos, sizeof ( G3DVECTOR ) );
            memcpy ( &oldrot, &obj->rot, sizeof ( G3DVECTOR ) );
            memcpy ( &oldsca, &obj->sca, sizeof ( G3DVECTOR ) );

            obj->sca.y = ysca;

            g3dobject_updateMatrix_r ( obj, gui->flags );

            g3durm_object_move ( urm, obj, &oldpos,  
                                           &oldrot, 
                                           &oldsca, REDRAWVIEW );
        }

        g3dui_unsetHourGlass ( gui );

        XtFree ( value );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
static void xscacbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DURMANAGER *urm = NULL;
    G3DSCENE     *sce = NULL;
    G3DOBJECT    *obj = NULL;
    G3DUI        *gui = NULL;

    /*** retrieve the global GUI structure ***/
    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    /*** prevents a loop when refreshing the widget ***/
    if ( gui->lock ) return;

    sce = gui->sce;
    urm = gui->urm;
    obj = g3dscene_getLastSelected ( sce );

    if ( obj ) {
        char *value = XmTextGetString ( w );
        float xsca = strtof ( value, NULL );

        g3dui_setHourGlass ( gui );

        if ( gui->flags & VIEWOBJECT ) {
            G3DVECTOR oldpos, oldrot, oldsca;

            memcpy ( &oldpos, &obj->pos, sizeof ( G3DVECTOR ) );
            memcpy ( &oldrot, &obj->rot, sizeof ( G3DVECTOR ) );
            memcpy ( &oldsca, &obj->sca, sizeof ( G3DVECTOR ) );

            obj->sca.x = xsca;

            g3dobject_updateMatrix_r ( obj, gui->flags );

            g3durm_object_move ( urm, obj, &oldpos,  
                                           &oldrot, 
                                           &oldsca, REDRAWVIEW );
        }

        g3dui_unsetHourGlass ( gui );

        XtFree ( value );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
static void zrotcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DURMANAGER *urm = NULL;
    G3DSCENE     *sce = NULL;
    G3DOBJECT    *obj = NULL;
    G3DUI        *gui = NULL;

    /*** retrieve the global GUI structure ***/
    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    /*** prevents a loop when refreshing the widget ***/
    if ( gui->lock ) return;

    sce = gui->sce;
    urm = gui->urm;
    obj = g3dscene_getLastSelected ( sce );

    if ( obj ) {
        char *value = XmTextGetString ( w );
        float zrot = strtof ( value, NULL );

        g3dui_setHourGlass ( gui );

        if ( gui->flags & VIEWOBJECT ) {
            G3DVECTOR oldpos, oldrot, oldsca;

            memcpy ( &oldpos, &obj->pos, sizeof ( G3DVECTOR ) );
            memcpy ( &oldrot, &obj->rot, sizeof ( G3DVECTOR ) );
            memcpy ( &oldsca, &obj->sca, sizeof ( G3DVECTOR ) );

            obj->rot.z = zrot;

            g3dobject_updateMatrix_r ( obj, gui->flags );

            g3durm_object_move ( urm, obj, &oldpos,  
                                           &oldrot, 
                                           &oldsca, REDRAWVIEW );
        }

        g3dui_unsetHourGlass ( gui );

        XtFree ( value );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
static void yrotcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DURMANAGER *urm = NULL;
    G3DSCENE     *sce = NULL;
    G3DOBJECT    *obj = NULL;
    G3DUI        *gui = NULL;

    /*** retrieve the global GUI structure ***/
    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    /*** prevents a loop when refreshing the widget ***/
    if ( gui->lock ) return;

    sce = gui->sce;
    urm = gui->urm;
    obj = g3dscene_getLastSelected ( sce );

    if ( obj ) {
        char *value = XmTextGetString ( w );
        float yrot = strtof ( value, NULL );

        g3dui_setHourGlass ( gui );

        if ( gui->flags & VIEWOBJECT ) {
            G3DVECTOR oldpos, oldrot, oldsca;

            memcpy ( &oldpos, &obj->pos, sizeof ( G3DVECTOR ) );
            memcpy ( &oldrot, &obj->rot, sizeof ( G3DVECTOR ) );
            memcpy ( &oldsca, &obj->sca, sizeof ( G3DVECTOR ) );

            obj->rot.y = yrot;

            g3dobject_updateMatrix_r ( obj, gui->flags );

            g3durm_object_move ( urm, obj, &oldpos,  
                                           &oldrot, 
                                           &oldsca, REDRAWVIEW );
        }

        g3dui_unsetHourGlass ( gui );

        XtFree ( value );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
static void xrotcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DURMANAGER *urm = NULL;
    G3DSCENE     *sce = NULL;
    G3DOBJECT    *obj = NULL;
    G3DUI        *gui = NULL;

    /*** retrieve the global GUI structure ***/
    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    /*** prevents a loop when refreshing the widget ***/
    if ( gui->lock ) return;

    sce = gui->sce;
    urm = gui->urm;
    obj = g3dscene_getLastSelected ( sce );

    if ( obj ) {
        char *value = XmTextGetString ( w );
        float xrot = strtof ( value, NULL );

        g3dui_setHourGlass ( gui );

        if ( gui->flags & VIEWOBJECT ) {
            G3DVECTOR oldpos, oldrot, oldsca;

            memcpy ( &oldpos, &obj->pos, sizeof ( G3DVECTOR ) );
            memcpy ( &oldrot, &obj->rot, sizeof ( G3DVECTOR ) );
            memcpy ( &oldsca, &obj->sca, sizeof ( G3DVECTOR ) );

            obj->rot.x = xrot;

            g3dobject_updateMatrix_r ( obj, gui->flags );

            g3durm_object_move ( urm, obj, &oldpos,  
                                           &oldrot, 
                                           &oldsca, REDRAWVIEW );
        }

        g3dui_unsetHourGlass ( gui );

        XtFree ( value );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
static void zposcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DURMANAGER *urm = NULL;
    G3DSCENE     *sce = NULL;
    G3DOBJECT    *obj = NULL;
    G3DUI        *gui = NULL;

    /*** retrieve the global GUI structure ***/
    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    /*** prevents a loop when refreshing the widget ***/
    if ( gui->lock ) return;

    sce = gui->sce;
    urm = gui->urm;
    obj = g3dscene_getLastSelected ( sce );

    if ( obj ) {
        char *value = XmTextGetString ( w );
        float zpos = strtof ( value, NULL );

        g3dui_setHourGlass ( gui );

        if ( gui->flags & VIEWOBJECT ) {
            G3DVECTOR oldpos, oldrot, oldsca;

            memcpy ( &oldpos, &obj->pos, sizeof ( G3DVECTOR ) );
            memcpy ( &oldrot, &obj->rot, sizeof ( G3DVECTOR ) );
            memcpy ( &oldsca, &obj->sca, sizeof ( G3DVECTOR ) );

            obj->pos.z = zpos;

            g3dobject_updateMatrix_r ( obj, gui->flags );

            g3durm_object_move ( urm, obj, &oldpos,  
                                           &oldrot, 
                                           &oldsca, REDRAWVIEW );
        }

        g3dui_unsetHourGlass ( gui );

        XtFree ( value );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
static void yposcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DURMANAGER *urm = NULL;
    G3DSCENE     *sce = NULL;
    G3DOBJECT    *obj = NULL;
    G3DUI        *gui = NULL;

    /*** retrieve the global GUI structure ***/
    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    /*** prevents a loop when refreshing the widget ***/
    if ( gui->lock ) return;

    sce = gui->sce;
    urm = gui->urm;
    obj = g3dscene_getLastSelected ( sce );

    if ( obj ) {
        char *value = XmTextGetString ( w );
        float ypos = strtof ( value, NULL );

        g3dui_setHourGlass ( gui );

        if ( gui->flags & VIEWOBJECT ) {
            G3DVECTOR oldpos, oldrot, oldsca;

            memcpy ( &oldpos, &obj->pos, sizeof ( G3DVECTOR ) );
            memcpy ( &oldrot, &obj->rot, sizeof ( G3DVECTOR ) );
            memcpy ( &oldsca, &obj->sca, sizeof ( G3DVECTOR ) );

            obj->pos.y = ypos;

            g3dobject_updateMatrix_r ( obj, gui->flags );

            g3durm_object_move ( urm, obj, &oldpos,  
                                           &oldrot, 
                                           &oldsca, REDRAWVIEW );
        }

        g3dui_unsetHourGlass ( gui );

        XtFree ( value );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
static void xposcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DURMANAGER *urm = NULL;
    G3DSCENE     *sce = NULL;
    G3DOBJECT    *obj = NULL;
    G3DUI        *gui = NULL;

    /*** retrieve the global GUI structure ***/
    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    /*** prevents a loop when refreshing the widget ***/
    if ( gui->lock ) return;

    sce = gui->sce;
    urm = gui->urm;
    obj = g3dscene_getLastSelected ( sce );

    if ( obj ) {
        char *value = XmTextGetString ( w );
        float xpos = strtof ( value, NULL );

        g3dui_setHourGlass ( gui );

        if ( gui->flags & VIEWOBJECT ) {
            G3DVECTOR oldpos, oldrot, oldsca;

            memcpy ( &oldpos, &obj->pos, sizeof ( G3DVECTOR ) );
            memcpy ( &oldrot, &obj->rot, sizeof ( G3DVECTOR ) );
            memcpy ( &oldsca, &obj->sca, sizeof ( G3DVECTOR ) );

            obj->pos.x = xpos;

            g3dobject_updateMatrix_r ( obj, gui->flags );

            g3durm_object_move ( urm, obj, &oldpos,  
                                           &oldrot, 
                                           &oldsca, REDRAWVIEW );
        }

        /*if ( ( gui->flags & VIEWVERTEX ) ||
             ( gui->flags & VIEWEDGE   ) ||
             ( gui->flags & VIEWFACE   ) ) {
            if ( obj->type == G3DMESHTYPE ) {
                G3DMESH *mes = ( G3DMESH * ) obj;
                LIST *ltmpver, *lver = NULL;

                if ( gui->flags & VIEWVERTEX ) lver = list_copy                   ( mes->lselver );
                if ( gui->flags & VIEWEDGE   ) lver = g3dedge_getVerticesFromList ( mes->lselfac );
                if ( gui->flags & VIEWFACE   ) lver = g3dface_getVerticesFromList ( mes->lseledg );

                ltmpver = lver;

                while ( ltmpver ) {
                    G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

                    ver->pos.x = xpos;

                    if ( obj->parent && obj->parent->childvertexchange ) {
                        obj->parent->childvertexchange ( obj->parent, obj, ver );
                    }

                    ltmpver = ltmpver->next;
                }

                list_free ( &lver, NULL );
            }
        }*/

        g3dui_unsetHourGlass ( gui );

        XtFree ( value );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void updateCoordinatesEdit ( Widget w ) {
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

    if ( obj ) {
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
                if ( strcmp ( name, EDITXPOSITION   ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%.3f", obj->pos.x );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITYPOSITION   ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%.3f", obj->pos.y );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITZPOSITION   ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%.3f", obj->pos.z );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITXROTATION   ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%.3f", obj->rot.x );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITYROTATION   ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%.3f", obj->rot.y );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITZROTATION   ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%.3f", obj->rot.z );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITXSCALING   ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%.3f", obj->sca.x );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITYSCALING   ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%.3f", obj->sca.y );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITZSCALING   ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%.3f", obj->sca.z );

                    XmTextSetString ( child, buf );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
Widget createCoordinatesEdit ( Widget parent, G3DUI *gui, char *name,
                               Dimension x    , Dimension y,
                               Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget frm, rwc, coords;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    frm = XmVaCreateManagedForm ( parent, name,
                                  XmNx, x,
                                  XmNy, y,
                                  XmNwidth , width,
                                  XmNheight, height,
                                  XmNforeground, foreground,
                                  XmNbackground, background,
                                  XmNuserData, gui,
                                  NULL );

    rwc = XmVaCreateRowColumn ( frm, name,
                                XmNx, x,
                                XmNy, y,
                                XmNwidth , width,
                                XmNheight, height,
                                XmNnumColumns, 0x03,
                                /*XmNorientation, XmHORIZONTAL,*/
                                XmNpacking, XmPACK_COLUMN,
                                XmNspacing, 0x01,
                                XmNforeground, foreground,
                                XmNbackground, background,
                                XmNuserData, gui,
                                NULL );

     createSimpleLabel ( rwc, "Position"   , 0x00, 0x00, 0x60 );
     createFloatText   ( rwc, EDITXPOSITION, 0x00, 0x00, 0x00, 0x60, xposcbk );
     createFloatText   ( rwc, EDITYPOSITION, 0x00, 0x00, 0x00, 0x60, yposcbk );
     createFloatText   ( rwc, EDITZPOSITION, 0x00, 0x00, 0x00, 0x60, zposcbk );

     createSimpleLabel ( rwc, "Rotation"   , 0x00, 0x00, 0x60 );
     createFloatText   ( rwc, EDITXROTATION, 0x00, 0x00, 0x00, 0x60, xrotcbk );
     createFloatText   ( rwc, EDITYROTATION, 0x00, 0x00, 0x00, 0x60, yrotcbk );
     createFloatText   ( rwc, EDITZROTATION, 0x00, 0x00, 0x00, 0x60, zrotcbk );

     createSimpleLabel ( rwc, "Scaling"   , 0x00, 0x00, 0x60 );
     createFloatText   ( rwc, EDITXSCALING, 0x00, 0x00, 0x00, 0x60, xscacbk );
     createFloatText   ( rwc, EDITYSCALING, 0x00, 0x00, 0x00, 0x60, yscacbk );
     createFloatText   ( rwc, EDITZSCALING, 0x00, 0x00, 0x00, 0x60, zscacbk );

    /*coords = createObjectCoordinates ( frm, gui, "COORDINATES",
                                       x, y, width, height );*/

    XtManageChild ( rwc );

    list_insert ( &gui->lcoord, rwc );


    return frm;
}
