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
#include <g3dengine/vtable/g3dinstancevtable.h>

/******************************************************************************/
static G3DINSTANCEVTABLE _vtable = { G3DINSTANCEVTABLE_DEFAULT };

/******************************************************************************/
void g3dinstance_setReference ( G3DINSTANCE *ins, 
                                G3DOBJECT   *ref ) {
    ins->ref = ref;

    if ( ref ) {
        g3dscene_addReferredObject ( ins->sce, ref );
    }
}

/******************************************************************************/
void g3dinstance_setOrientation ( G3DINSTANCE *ins, 
                                  uint32_t     orientation ) {
    ins->orientation = orientation;

    g3dcore_symmetryMatrixf ( ins->smatrix, orientation );
}

/******************************************************************************/
void g3dinstance_setMirrored ( G3DINSTANCE *ins ) {
    ((G3DOBJECT*)ins)->flags |= INSTANCEMIRRORED;
}

/******************************************************************************/
void g3dinstance_unsetMirrored ( G3DINSTANCE *ins ) {
    ((G3DOBJECT*)ins)->flags &= (~INSTANCEMIRRORED);
}

/******************************************************************************/
G3DINSTANCE *g3dinstance_default_copy ( G3DINSTANCE   *ins, 
                                        uint32_t       id, 
                                        unsigned char *name,
                                        uint64_t       engine_flags ) {
    G3DINSTANCE *cpyins = g3dinstance_new ( ((G3DOBJECT*)ins)->id,
                                            ((G3DOBJECT*)ins)->name, ins->sce );

    cpyins->ref = ins->ref;
    cpyins->orientation = ins->orientation;

    memcpy ( cpyins->smatrix, ins->smatrix, sizeof ( double ) * 0x10 );


    return cpyins;
}

/******************************************************************************/
uint32_t g3dinstance_default_draw ( G3DINSTANCE *ins, 
                                    G3DCAMERA   *curcam,
                                    G3DENGINE   *engine,
                                    uint64_t     engine_flags ) {

    glPushAttrib( GL_ALL_ATTRIB_BITS );

    if ( ins->ref ) {
        if ( g3dscene_isObjectReferred ( ins->sce, ins->ref ) ) {
            if ( ins->ref->vtable->draw ) {
                if ( ((G3DOBJECT*)ins)->flags & INSTANCEMIRRORED ) {
                    
                    g3dengine_multModelMatrixf ( engine, ins->smatrix );

        glEnable ( GL_RESCALE_NORMAL );
                    glFrontFace(  GL_CW );
                }

                ins->ref->vtable->draw ( ins->ref,
                                         curcam,
                                         engine,
                                         ( engine_flags & (~VIEWDETAILS) ) | VIEWOBJECT );

                if ( ((G3DOBJECT*)ins)->flags & INSTANCEMIRRORED ) {
        glDisable ( GL_RESCALE_NORMAL );
                    glFrontFace(  GL_CCW );
                }
            }
        }
    }

    glPopAttrib ( );

    return 0x00;
}

/******************************************************************************/
uint32_t g3dinstance_default_pick ( G3DINSTANCE *ins, 
                                    G3DCAMERA   *curcam, 
                                    uint64_t     engine_flags ) {

    if ( ins->ref ) {
        if ( g3dscene_isObjectReferred ( ins->sce, ins->ref ) ) {
            if ( ins->ref->vtable->pick ) {
                if ( engine_flags & VIEWOBJECT ) {
                    if ( ((G3DOBJECT*)ins)->flags & INSTANCEMIRRORED ) {
                        g3dpick_multModelviewMatrixf ( ins->smatrix );
                    }

                    ins->ref->vtable->pick ( ins->ref, 
                                             curcam, 
                                             engine_flags );
                }
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
void g3dinstance_init ( G3DINSTANCE       *ins, 
                        uint32_t          id, 
                        char              *name,
                        G3DSCENE          *sce,
                        G3DINSTANCEVTABLE *vtable ) {

    g3dobject_init ( G3DOBJECTCAST(ins), 
                     G3DINSTANCETYPE,
                     id, 
                     name, 
                     0x00,
                     vtable ? G3DOBJECTVTABLECAST(vtable)
                            : G3DOBJECTVTABLECAST(&_vtable) );

    ins->orientation = INSTANCEYZ;
    ins->sce = sce;
}

/******************************************************************************/
G3DINSTANCE *g3dinstance_new ( uint32_t id, 
                               char     *name,
                               G3DSCENE *sce ) {
    G3DINSTANCE *ins = ( G3DINSTANCE * ) calloc ( 0x01, sizeof ( G3DINSTANCE ) );

    if ( ins == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    g3dinstance_init ( ins, id, name, sce, NULL );


    return ins;
}
