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
#include <g3dui.h>

#ifdef unused

/******************************************************************************/
static uint64_t setsca ( G3DUIWIDGET    *wid, 
                         G3DUIWIDGETARG *arg ) {
    G3DUI *gui = wid->gui;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
    uint32_t absolute = 0x01;
    float val = arg->f;

    if ( obj ) {
        g3dui_setHourGlass ( gui );

        if ( gui->engine_flags & VIEWOBJECT ) {
            URMTRANSFORMOBJECT *uto = g3durm_object_transform ( urm,
                                                                sce,
                                                                sce->lsel,
                                                                REDRAWVIEW );

            if ( strcmp ( wid->name, EDITXSCALING ) == 0x00 ) obj->sca.x = val;
            if ( strcmp ( wid->name, EDITYSCALING ) == 0x00 ) obj->sca.y = val;
            if ( strcmp ( wid->name, EDITZSCALING ) == 0x00 ) obj->sca.z = val;

            g3dobject_updateMatrix_r ( obj, gui->engine_flags );

            urmtransform_saveState ( uto, UTOSAVESTATEAFTER );
        }
    }

    g3dscene_updatePivot ( sce, gui->engine_flags );

    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t setrot ( G3DUIWIDGET    *wid, 
                         G3DUIWIDGETARG *arg ) {
    G3DUI *gui = wid->gui;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
    uint32_t absolute = 0x01;
    float val = arg->f;

    if ( obj ) {
        g3dui_setHourGlass ( gui );

        if ( gui->engine_flags & VIEWOBJECT ) {
            URMTRANSFORMOBJECT *uto = g3durm_object_transform ( urm,
                                                                sce,
                                                                sce->lsel,
                                                                REDRAWVIEW );

            urmtransform_saveState ( uto, UTOSAVESTATEBEFORE );

            if ( strcmp ( wid->name, EDITXROTATION ) == 0x00 ) obj->rot.x = val;
            if ( strcmp ( wid->name, EDITYROTATION ) == 0x00 ) obj->rot.y = val;
            if ( strcmp ( wid->name, EDITZROTATION ) == 0x00 ) obj->rot.z = val;

            g3dobject_updateMatrix_r ( obj, gui->engine_flags );

            urmtransform_saveState ( uto, UTOSAVESTATEAFTER );
        }
    }

    g3dscene_updatePivot ( sce, gui->engine_flags );

    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t setpos ( G3DUIWIDGET *wid, 
                         G3DUIWIDGETARG *arg ) {
    G3DUI *gui = wid->gui;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
    uint32_t absolute = 0x01;
    float val = arg->f;

    if ( obj ) {
        if ( gui->engine_flags & VIEWOBJECT ) {
            URMTRANSFORMOBJECT *uto = g3durm_object_transform ( urm,
                                                                sce,
                                                                sce->lsel,
                                                                REDRAWVIEW );

            urmtransform_saveState ( uto, UTOSAVESTATEBEFORE );

            if ( strcmp ( wid->name, EDITXPOSITION ) == 0x00 ) obj->pos.x = val;
            if ( strcmp ( wid->name, EDITYPOSITION ) == 0x00 ) obj->pos.y = val;
            if ( strcmp ( wid->name, EDITZPOSITION ) == 0x00 ) obj->pos.z = val;

            g3dobject_updateMatrix_r ( obj, gui->engine_flags );

            urmtransform_saveState ( uto, UTOSAVESTATEAFTER );
        }

        if ( gui->engine_flags & VIEWVERTEX ) {
            if ( obj->type == G3DMESHTYPE ) {
                G3DMESH *mes = ( G3DMESH * ) obj;
                uint32_t axis_flags = 0x00;
                G3DVECTOR *oldpos = NULL;
                G3DVECTOR *newpos = NULL;
                G3DVECTOR avg;
                G3DVECTOR to;

                g3dvertex_getAveragePositionFromList ( mes->lselver, &avg );

                if ( strcmp ( wid->name, EDITXPOSITION ) == 0x00 ) {
                    to.x = val;

                    axis_flags |= XAXIS;
                }

                if ( strcmp ( wid->name, EDITYPOSITION ) == 0x00 ) {
                    to.y = val;

                    axis_flags |= YAXIS;
                }

                if ( strcmp ( wid->name, EDITZPOSITION ) == 0x00 ) {
                    to.z = val;

                    axis_flags |= ZAXIS;
                }

                g3dvertex_copyPositionFromList ( mes->lselver, &oldpos );

                g3dmesh_moveVerticesTo ( mes, mes->lselver, &avg, &to, absolute, axis_flags, gui->engine_flags );

                g3dvertex_copyPositionFromList ( mes->lselver, &newpos );

                g3durm_mesh_moveVertexList ( gui->urm, mes, mes->lselver, NULL, NULL, NULL,
                                             oldpos, 
                                             newpos, 
                                             REDRAWVIEW );
            }
        }
    }

    g3dscene_updatePivot ( sce, gui->engine_flags );

    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t getpos ( G3DUIWIDGET    *wid, 
                         G3DUIWIDGETARG *arg ) {

    if ( wid->gui->engine_flags & VIEWOBJECT ) {
        G3DSCENE *sce = wid->gui->sce;
        G3DURMANAGER *urm = wid->gui->urm;
        G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

        if ( strcmp ( wid->name, EDITXPOSITION ) == 0x00 ) {
            arg->f = obj->pos.x;
        }

        if ( strcmp ( wid->name, EDITYPOSITION ) == 0x00 ) {
            arg->f = obj->pos.y;
        }

        if ( strcmp ( wid->name, EDITZPOSITION ) == 0x00 ) {
            arg->f = obj->pos.z;
        }
    }
}

/******************************************************************************/
static G3DUINUMERICENTRYWIDGET xposwid = { .ent = { .wid = { .gui    = NULL ,
                                                             .type   = G3DUIWIDGET_FLOATENTRY,
                                                             .label  = EDITXPOSITION,
                                                             .x      = 0,
                                                             .y      = 24,
                                                             .width  = 48,
                                                             .height = 24,
                                                             .parent = &general },
                                                    .set  = setpos,
                                                    .get  = getpos,
                                                    .labx = 0,
                                                    .laby = 0,
                                                    .labw = 0,
                                                    .labh = 0 } };

static G3DUINUMERICENTRYWIDGET yposwid = { .ent = { .wid = { .gui    = NULL ,
                                                             .type   = G3DUIWIDGET_FLOATENTRY,
                                                             .label  = EDITXPOSITION,
                                                             .x      = 0,
                                                             .y      = 48,
                                                             .width  = 48,
                                                             .height = 24,
                                                             .parent = &general },
                                                    .set  = setpos,
                                                    .get  = getpos,
                                                    .labx = 0,
                                                    .laby = 0,
                                                    .labw = 0,
                                                    .labh = 0 } };

static G3DUINUMERICENTRYWIDGET zposwid = { .ent = { .wid = { .gui    = NULL ,
                                                             .type   = G3DUIWIDGET_FLOATENTRY,
                                                             .label  = EDITXPOSITION,
                                                             .x      = 0,
                                                             .y      = 72,
                                                             .width  = 48,
                                                             .height = 24,
                                                             .parent = &general },
                                                    .set  = setpos,
                                                    .get  = getpos,
                                                    .labx = 0,
                                                    .laby = 0,
                                                    .labw = 0,
                                                    .labh = 0 } };

/******************************************************************************/
static G3DUINUMERICENTRYWIDGET xrotwid = { .ent = { .wid = { .gui    = NULL ,
                                                             .type   = G3DUIWIDGET_FLOATENTRY,
                                                             .label  = EDITXROTATION,
                                                             .x      = 96,
                                                             .y      = 24,
                                                             .width  = 48,
                                                             .height = 24,
                                                             .parent = &general },
                                                    .set  = setpos,
                                                    .get  = getpos,
                                                    .labx = 0,
                                                    .laby = 0,
                                                    .labw = 0,
                                                    .labh = 0 } };

static G3DUINUMERICENTRYWIDGET yrotwid = { .ent = { .wid = { .gui    = NULL ,
                                                             .type   = G3DUIWIDGET_FLOATENTRY,
                                                             .label  = EDITYROTATION,
                                                             .x      = 96,
                                                             .y      = 48,
                                                             .width  = 48,
                                                             .height = 24,
                                                             .parent = &general },
                                                    .set  = setpos,
                                                    .get  = getpos,
                                                    .labx = 0,
                                                    .laby = 0,
                                                    .labw = 0,
                                                    .labh = 0 } };

static G3DUINUMERICENTRYWIDGET zrotwid = { .ent = { .wid = { .gui    = NULL ,
                                                             .type   = G3DUIWIDGET_FLOATENTRY,
                                                             .label  = EDITZROTATION,
                                                             .x      = 96,
                                                             .y      = 72,
                                                             .width  = 48,
                                                             .height = 24,
                                                             .parent = &general },
                                                    .set  = setpos,
                                                    .get  = getpos,
                                                    .labx = 0,
                                                    .laby = 0,
                                                    .labw = 0,
                                                    .labh = 0 } };

/******************************************************************************/
static G3DUINUMERICENTRYWIDGET xscawid = { .ent = { .wid = { .gui    = NULL ,
                                                             .type   = G3DUIWIDGET_FLOATENTRY,
                                                             .label  = EDITXSCALING,
                                                             .x      = 192,
                                                             .y      = 24,
                                                             .width  = 48,
                                                             .height = 24,
                                                             .parent = &general },
                                                    .set  = setpos,
                                                    .get  = getpos,
                                                    .labx = 0,
                                                    .laby = 0,
                                                    .labw = 0,
                                                    .labh = 0 } };

static G3DUINUMERICENTRYWIDGET yscawid = { .ent = { .wid = { .gui    = NULL ,
                                                             .type   = G3DUIWIDGET_FLOATENTRY,
                                                             .label  = EDITYSCALING,
                                                             .x      = 192,
                                                             .y      = 48,
                                                             .width  = 48,
                                                             .height = 24,
                                                             .parent = &general },
                                                    .set  = setpos,
                                                    .get  = getpos,
                                                    .labx = 0,
                                                    .laby = 0,
                                                    .labw = 0,
                                                    .labh = 0 } };

static G3DUINUMERICENTRYWIDGET zscawid = { .ent = { .wid = { .gui    = NULL ,
                                                             .type   = G3DUIWIDGET_FLOATENTRY,
                                                             .label  = EDITZSCALING,
                                                             .x      = 192,
                                                             .y      = 72,
                                                             .width  = 48,
                                                             .height = 24,
                                                             .parent = &general },
                                                    .set  = setpos,
                                                    .get  = getpos,
                                                    .labx = 0,
                                                    .laby = 0,
                                                    .labw = 0,
                                                    .labh = 0 } };

/******************************************************************************/
static G3DUIWIDGET poslab = { .gui    = NULL ,
                              .type   = G3DUIWIDGET_LABEL,
                              .label  = "Translation",
                              .x      = 0,
                              .y      = 0,
                              .width  = 48,
                              .heigth = 24,
                              .parent = &general };

static G3DUIWIDGET rotlab = { .gui    = NULL ,
                              .type   = G3DUIWIDGET_LABEL,
                              .label  = "Rotation",
                              .x      = 96,
                              .y      = 0,
                              .width  = 48,
                              .heigth = 24,
                              .parent = &general };

static G3DUIWIDGET scalab = { .gui    = NULL ,
                              .type   = G3DUIWIDGET_LABEL,
                              .label  = "Scaling",
                              .x      = 192,
                              .y      = 0,
                              .width  = 48,
                              .heigth = 24,
                              .parent = &general };

/******************************************************************************/
static G3DUIWIDGET general = { .gui    = NULL ,
                               .type   = G3DUIWIDGET_CONTAINER,
                               .label  = "Coordinates",
                               .x      = 0,
                               .y      = 0,
                               .width  = 310,
                               .heigth = 192,
                               .children = { &poslab, 
                                             &xposwid,
                                             &yposwid,
                                             &zposwid,
                                             &rotlab,
                                             &xrotwid,
                                             &yrotwid,
                                             &zrotwid,
                                             &scalab,
                                             &xscawid,
                                             &yscawid,
                                             &zscawid,
                                              NULL } };

#endif


/******************************************************************************/
void common_g3duicoordinatesedit_posCbk ( G3DUI *gui, G3DUIAXIS axis, 
                                                      uint32_t absolute,
                                                      float val ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    /*** prevent a loop ***/
    if ( gui->lock ) return;

    if ( obj ) {
        g3dui_setHourGlass ( gui );

        if ( gui->engine_flags & VIEWOBJECT ) {
            URMTRANSFORMOBJECT *uto = g3durm_object_transform ( urm,
                                                                sce,
                                                                sce->lsel,
                                                                REDRAWVIEW );

            urmtransform_saveState ( uto, UTOSAVESTATEBEFORE );

            if ( axis == G3DUIXAXIS ) obj->pos.x = val;
            if ( axis == G3DUIYAXIS ) obj->pos.y = val;
            if ( axis == G3DUIZAXIS ) obj->pos.z = val;

            g3dobject_updateMatrix_r ( obj, gui->engine_flags );

            urmtransform_saveState ( uto, UTOSAVESTATEAFTER );
        }

        if ( gui->engine_flags & VIEWVERTEX ) {
            if ( obj->type == G3DMESHTYPE ) {
                G3DMESH *mes = ( G3DMESH * ) obj;
                uint32_t axis_flags = 0x00;
                G3DVECTOR *oldpos = NULL;
                G3DVECTOR *newpos = NULL;
                G3DVECTOR avg;
                G3DVECTOR to;

                g3dvertex_getAveragePositionFromList ( mes->lselver, &avg );

                if ( axis == G3DUIXAXIS ) { to.x = val; axis_flags |= XAXIS; }
                if ( axis == G3DUIYAXIS ) { to.y = val; axis_flags |= YAXIS; }
                if ( axis == G3DUIZAXIS ) { to.z = val; axis_flags |= ZAXIS; }

                g3dvertex_copyPositionFromList ( mes->lselver, &oldpos );

                g3dmesh_moveVerticesTo ( mes, mes->lselver, &avg, &to, absolute, axis_flags, gui->engine_flags );

                g3dvertex_copyPositionFromList ( mes->lselver, &newpos );

                g3durm_mesh_moveVertexList ( gui->urm, mes, mes->lselver, NULL, NULL, NULL,
                                             oldpos, 
                                             newpos, 
                                             REDRAWVIEW );
            }
        }

        g3dui_unsetHourGlass ( gui );

        g3dui_redrawGLViews ( gui );
    }

    g3dscene_updatePivot ( sce, gui->engine_flags );
}

/******************************************************************************/
void common_g3duicoordinatesedit_rotCbk ( G3DUI *gui, G3DUIAXIS axis, 
                                                      float val ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    /*** prevent a loop ***/
    if ( gui->lock ) return;

    if ( obj ) {
        g3dui_setHourGlass ( gui );

        if ( gui->engine_flags & VIEWOBJECT ) {
            URMTRANSFORMOBJECT *uto = g3durm_object_transform ( urm,
                                                                sce,
                                                                sce->lsel,
                                                                REDRAWVIEW );

            urmtransform_saveState ( uto, UTOSAVESTATEBEFORE );

            if ( axis == G3DUIXAXIS ) obj->rot.x = val;
            if ( axis == G3DUIYAXIS ) obj->rot.y = val;
            if ( axis == G3DUIZAXIS ) obj->rot.z = val;

            g3dobject_updateMatrix_r ( obj, gui->engine_flags );

            urmtransform_saveState ( uto, UTOSAVESTATEAFTER );
        }

        g3dui_unsetHourGlass ( gui );

        g3dui_redrawGLViews ( gui );
    }

    g3dscene_updatePivot ( sce, gui->engine_flags );
}

/******************************************************************************/
void common_g3duicoordinatesedit_scaCbk ( G3DUI *gui, G3DUIAXIS axis, 
                                                      float val ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    /*** prevent a loop ***/
    if ( gui->lock ) return;

    if ( obj ) {
        g3dui_setHourGlass ( gui );

        if ( gui->engine_flags & VIEWOBJECT ) {
            URMTRANSFORMOBJECT *uto = g3durm_object_transform ( urm,
                                                                sce,
                                                                sce->lsel,
                                                                REDRAWVIEW );

            if ( axis == G3DUIXAXIS ) obj->sca.x = val;
            if ( axis == G3DUIYAXIS ) obj->sca.y = val;
            if ( axis == G3DUIZAXIS ) obj->sca.z = val;

            g3dobject_updateMatrix_r ( obj, gui->engine_flags );

            urmtransform_saveState ( uto, UTOSAVESTATEAFTER );
        }

        g3dui_unsetHourGlass ( gui );

        g3dui_redrawGLViews ( gui );
    }

    g3dscene_updatePivot ( sce, gui->engine_flags );
}

