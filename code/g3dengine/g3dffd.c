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
#include <g3dengine/g3dengine.h>

/******************************************************************************/
static int fact ( int n ) {
    if ( n > 0x01 )
        return n * fact ( n - 0x01 );
    else
        return 0x01; 

    /*return exp ( lgamma ( n + 1.0 ) ) ;*/
}

/******************************************************************************/
static double binomialcoeff ( int n, int k ) {
    int div = fact ( k ) * fact ( n - k );

    if ( div == 0x00 ) return 0.0f;

    return ( double ) fact ( n ) / div;
}

/******************************************************************************/
static G3DOBJECT *g3dffd_commit ( G3DFFD        *ffd,
                                  uint32_t       id,
                                  unsigned char *name,
                                  uint64_t       engine_flags ) {
    G3DOBJECT *obj    = ( G3DOBJECT * ) ffd;

    if ( g3dobject_isActive ( ffd ) ) {
        if ( ffd->mod.oriobj ) {
            G3DMESH *parmes = ( G3DMESH * ) ffd->mod.oriobj;
            G3DMESH *cpymes = g3dmesh_new ( ((G3DOBJECT*)parmes)->id,
                                            ((G3DOBJECT*)parmes)->name, 
                                            engine_flags );
            G3DVECTOR origin = { 0.0f, 0.0f, 0.0f, 1.0f }, wmespos, lmespos;
            uint32_t i;

            /*** readjust point position to keep the same matrix as the ***/
            /*** modified mesh ***/
            g3dvector_matrix ( &origin, ffd->mod.oriobj->wmatrix, &wmespos );
            g3dvector_matrix ( &wmespos, ffd->mod.mes.obj.iwmatrix, &lmespos );

            for ( i = 0x00; i < parmes->nbver; i++ ) {
                ffd->mod.verpos[i].x -= lmespos.x;
                ffd->mod.verpos[i].y -= lmespos.y;
                ffd->mod.verpos[i].z -= lmespos.z;
            }

            g3dmesh_clone ( parmes, 
                            ffd->mod.verpos,
                            cpymes,
                            engine_flags );

            /*** adjust back :-/ ***/
            for ( i = 0x00; i < parmes->nbver; i++ ) {
                ffd->mod.verpos[i].x += lmespos.x;
                ffd->mod.verpos[i].y += lmespos.y;
                ffd->mod.verpos[i].z += lmespos.z;
            }

            g3dobject_importTransformations ( cpymes, parmes );

            return cpymes;
        }
    }

    return NULL;
}

/******************************************************************************/
static void g3dffd_getDeformedPosition ( G3DFFD    *ffd,
                                         G3DVECTOR *uvw,
                                         G3DVECTOR *oripos,
                                         G3DVECTOR *altpos ) {
    float xaxis = ( ffd->locmax.x - ffd->locmin.x ),
          yaxis = ( ffd->locmax.y - ffd->locmin.y ),
          zaxis = ( ffd->locmax.z - ffd->locmin.z );
    float difx = ( 1.0f / ffd->nbx ),
          dify = ( 1.0f / ffd->nby ),
          difz = ( 1.0f / ffd->nbz );
    G3DVECTOR vi = { .x = 0.0f, 
                     .y = 0.0f, 
                     .z = 0.0f, 
                     .w = 1.0f };
    uint32_t n = 0x00, i, j, k;

    /*** altered points within the FFD ***/
    altpos->x = ffd->parmin.x;
    altpos->y = ffd->parmin.y;
    altpos->z = ffd->parmin.z;

    for ( i = 0x00; i <= ffd->nbx; i++ ) {
        G3DVECTOR vj = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };
        double bui = binomialcoeff ( ffd->nbx, i );

        for ( j = 0x00; j <= ffd->nby; j++ ) {
            G3DVECTOR vk = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };
            double buj = binomialcoeff ( ffd->nby, j );

            for ( k = 0x00; k <= ffd->nbz; k++ ) {
                double buk = binomialcoeff ( ffd->nbz, k );
                G3DVERTEX *pnt = &ffd->pnt[n++];

                vk.x += ( buk * pow ( ( 1 - uvw->z ), (ffd->nbz) - k ) * pow ( uvw->z, k ) * pnt->nor.x );
                vk.y += ( buk * pow ( ( 1 - uvw->z ), (ffd->nbz) - k ) * pow ( uvw->z, k ) * pnt->nor.y );
                vk.z += ( buk * pow ( ( 1 - uvw->z ), (ffd->nbz) - k ) * pow ( uvw->z, k ) * pnt->nor.z );
            }

            vj.x += ( buj * pow ( ( 1 - uvw->y ), (ffd->nby) - j ) * pow ( uvw->y, j ) * vk.x );
            vj.y += ( buj * pow ( ( 1 - uvw->y ), (ffd->nby) - j ) * pow ( uvw->y, j ) * vk.y );
            vj.z += ( buj * pow ( ( 1 - uvw->y ), (ffd->nby) - j ) * pow ( uvw->y, j ) * vk.z );
        }

        vi.x += ( bui * pow ( ( 1 - uvw->x ), (ffd->nbx) - i ) * pow ( uvw->x, i ) * vj.x );
        vi.y += ( bui * pow ( ( 1 - uvw->x ), (ffd->nbx) - i ) * pow ( uvw->x, i ) * vj.y );
        vi.z += ( bui * pow ( ( 1 - uvw->x ), (ffd->nbx) - i ) * pow ( uvw->x, i ) * vj.z );
    }

    /*** set and adjust to FFD size ***/
    altpos->x += ( vi.x * xaxis );
    altpos->y += ( vi.y * yaxis );
    altpos->z += ( vi.z * zaxis );
}

/******************************************************************************/
static uint32_t g3dffd_modify ( G3DFFD     *ffd,
                                G3DMODIFYOP op,
                                uint64_t    engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) ffd;

    if ( ffd->mod.oriobj ) {
        if ( ffd->mod.oriobj->type & MESH ) {
            G3DMESH *orimes = ( G3DMESH * ) ffd->mod.oriobj;
            G3DVECTOR origin = { 0.0f, 0.0f, 0.0f, 1.0f }, wmespos, lmespos;

            g3dvector_matrix ( &origin, ffd->mod.oriobj->wmatrix, &wmespos );
            g3dvector_matrix ( &wmespos, ffd->mod.mes.obj.iwmatrix, &lmespos );

            if ( orimes->nbver ) {
                uint32_t i, j, k;
                float xaxis = ( ffd->locmax.x - ffd->locmin.x ),
                      yaxis = ( ffd->locmax.y - ffd->locmin.y ),
                      zaxis = ( ffd->locmax.z - ffd->locmin.z );
                float difx = ( 1.0f / ffd->nbx ),
                      dify = ( 1.0f / ffd->nby ),
                      difz = ( 1.0f / ffd->nbz );
                float x, y, z;
                uint32_t n = 0x00;
                LIST *ltmpver = orimes->lver;

                if ( op == G3DMODIFYOP_MODIFY     ) ltmpver = orimes->lver;
                /*** when parent mesh's verticces are moved ***/
                if ( op == G3DMODIFYOP_UPDATE     ) ltmpver = orimes->lselver;
                /*** when FFD's vertices are moved ***/
                if ( op == G3DMODIFYOP_UPDATESELF ) ltmpver = orimes->lver;

                if ( op == G3DMODIFYOP_MODIFY ) {
                    ffd->mod.verpos = ( G3DVECTOR * ) realloc ( ffd->mod.verpos, 
                                                                orimes->nbver * 
                                                                sizeof ( G3DVECTOR ) );
                    ffd->mod.vernor = ( G3DVECTOR * ) realloc ( ffd->mod.vernor, 
                                                                orimes->nbver *  
                                                                sizeof ( G3DVECTOR ) );
                    ffd->uvw        = ( G3DVECTOR * ) realloc ( ffd->uvw, 
                                                                orimes->nbver *
                                                                sizeof ( G3DVECTOR ) );
                }

                x = 0.0f;
                for ( i = 0x00; i <= ffd->nbx; i++, x += difx ) {
                    y = 0.0f;
                    for ( j = 0x00; j <= ffd->nby; j++, y += dify ) {
                        z = 0.0f;
                        for ( k = 0x00; k <= ffd->nbz; k++, z += difz ) {
                            G3DVERTEX *pnt = &ffd->pnt[n++];

                            pnt->nor.x = (( pnt->pos.x - ffd->locmin.x ) / xaxis);
                            pnt->nor.y = (( pnt->pos.y - ffd->locmin.y ) / yaxis);
                            pnt->nor.z = (( pnt->pos.z - ffd->locmin.z ) / zaxis);
                        }
                    }
                }

                while ( ltmpver ) {
                    G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                    G3DVECTOR *uvw = &ffd->uvw[ver->id];
                    G3DVECTOR *verpos = ( ffd->mod.stkpos ) ? &ffd->mod.stkpos[ver->id] : &ver->pos;
                    G3DVECTOR *altpos = &ffd->mod.verpos[ver->id];

                    if ( ( verpos->x >= ffd->parmin.x ) && ( verpos->x <= ffd->parmax.x ) &&
                         ( verpos->y >= ffd->parmin.y ) && ( verpos->y <= ffd->parmax.y ) &&
                         ( verpos->z >= ffd->parmin.z ) && ( verpos->z <= ffd->parmax.z ) ) {

                        ffd->uvw[ver->id].x = ( verpos->x - ffd->parmin.x ) / xaxis;
                        ffd->uvw[ver->id].y = ( verpos->y - ffd->parmin.y ) / yaxis;
                        ffd->uvw[ver->id].z = ( verpos->z - ffd->parmin.z ) / zaxis;
                        ffd->uvw[ver->id].w = 1.0f;

                        g3dffd_getDeformedPosition ( ffd,
                                                     uvw,
                                                     verpos,
                                                     altpos );
                    } else {
                        ffd->uvw[ver->id].w = 0.0f;

                        memcpy ( altpos, verpos, sizeof ( G3DVECTOR ) );
                    }

                    altpos->x += lmespos.x;
                    altpos->y += lmespos.y;
                    altpos->z += lmespos.z;

                    ltmpver = ltmpver->next;
                }
            }

            return MODIFIERCHANGESCOORDS | MODIFIERTAKESOVER;
        }
    }

    return 0x00;
}

/******************************************************************************/
static void g3dffd_onGeometryMove ( G3DFFD     *ffd,
                                    LIST       *lver, 
                                    LIST       *ledg,
                                    LIST       *lfac,
                                    G3DMODIFYOP op,
                                    uint64_t    engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) ffd;

    if ( g3dobject_isActive ( obj ) ) {
        if ( ffd->mod.oriobj ) {
            G3DMESH *parmes = ( G3DMESH * ) ffd->mod.oriobj;

            g3dffd_modify ( ffd,
                            G3DMODIFYOP_UPDATESELF,
                            engine_flags );

            g3dmodifier_modifyChildren ( ffd,
                                         op,
                                         engine_flags );
        }
    }
}

/******************************************************************************/
void g3dffd_activate ( G3DFFD *ffd, uint64_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) ffd;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );

    if ( parent ) {
        G3DMESH *parmes = ( G3DMESH * ) parent;

        g3dvector_matrix ( &ffd->locmin, obj->lmatrix, &ffd->parmin );
        g3dvector_matrix ( &ffd->locmax, obj->lmatrix, &ffd->parmax );

        g3dmesh_modify ( parmes, 
                         G3DMODIFYOP_MODIFY, 
                         engine_flags );
    }
}

/******************************************************************************/
void g3dffd_deactivate ( G3DFFD *ffd, uint64_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) ffd;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );

    if ( parent ) {
        G3DMESH *parmes = ( G3DMESH * ) parent;

        g3dmesh_modify ( parmes, 
                         G3DMODIFYOP_MODIFY, 
                         engine_flags );
    }

    if ( ffd->uvw        ) free ( ffd->uvw        );
    if ( ffd->mod.verpos ) free ( ffd->mod.verpos );
    if ( ffd->mod.vernor ) free ( ffd->mod.vernor );

    ffd->uvw = NULL;
    ffd->mod.verpos = NULL;
    ffd->mod.vernor = NULL;

    /*** TODO: do this in some generic caller function for modifiers ***/
    ffd->mod.oriobj = NULL;
    ffd->mod.stkpos = NULL;
    ffd->mod.stknor = NULL;
}

/******************************************************************************/
void g3dffd_shape ( G3DFFD *ffd, uint32_t nbx,
                                 uint32_t nby,
                                 uint32_t nbz,
                                 float radx,
                                 float rady,
                                 float radz ) {
    float xaxis = radx * 2.0f,
          yaxis = rady * 2.0f,
          zaxis = radz * 2.0f;
    float difxaxis = xaxis / ( float ) nbx;
    float difyaxis = yaxis / ( float ) nby;
    float difzaxis = zaxis / ( float ) nbz;
    uint32_t structsize = sizeof ( G3DVERTEX );
    float difx = 1.0f / ( float ) nbx,
          dify = 1.0f / ( float ) nby,
          difz = 1.0f / ( float ) nbz;
    uint32_t n = 0x00;
    uint32_t i, j, k;
    float xuvw, yuvw, zuvw;
    float xloc, yloc, zloc;
    uint32_t size = ( nbx + 0x01 ) *
                    ( nby + 0x01 ) *
                    ( nbz + 0x01 ) * structsize;

    list_free ( &((G3DMESH*)ffd)->lver, NULL );
    list_free ( &((G3DMESH*)ffd)->lselver, NULL );
    ((G3DMESH*)ffd)->nbver = 0x00;

    ffd->pnt = ( G3DVERTEX * ) realloc ( ffd->pnt, size );

    memset ( ffd->pnt, 0x00, size );

    ffd->nbx = nbx;
    ffd->nby = nby;
    ffd->nbz = nbz;

    ffd->radx = radx;
    ffd->rady = rady;
    ffd->radz = radz;

    ffd->locmin.x = -radx;
    ffd->locmin.y = -rady;
    ffd->locmin.z = -radz;

    ffd->locmax.x =  radx;
    ffd->locmax.y =  rady;
    ffd->locmax.z =  radz;

    if ( ffd->pnt == NULL ) {
        fprintf ( stderr, "g3dffd_shape: memory allocation failed\n");
    }

    xuvw = 0.0f;
    xloc = ffd->locmin.x;

    for ( i = 0x00; i <= ffd->nbx; i++, xuvw += difx, xloc += difxaxis ) {
        yuvw = 0.0f;
        yloc = ffd->locmin.y;

        for ( j = 0x00; j <= ffd->nby; j++, yuvw += dify, yloc += difyaxis ) {
            zuvw = 0.0f;
            zloc = ffd->locmin.z;

            for ( k = 0x00; k <= ffd->nbz; k++, zuvw += difz, zloc += difzaxis ) {
                G3DVERTEX *pnt = &ffd->pnt[n];

                pnt->id    = n++;
                /*** we use the normal vector to store the UVW coords ***/
                pnt->nor.x = xuvw;
                pnt->nor.y = yuvw;
                pnt->nor.z = zuvw;

                pnt->pos.x = xloc;
                pnt->pos.y = yloc;
                pnt->pos.z = zloc;

                g3dmesh_addVertex ( ( G3DMESH * ) ffd, pnt );
            }
        }
    }
}

/******************************************************************************/
static uint32_t g3dffd_draw ( G3DFFD    *ffd,
                              G3DCAMERA *cam, 
                              uint64_t   engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) ffd;

    if (   ( obj->flags & OBJECTSELECTED ) ||
         ( ( obj->flags & OBJECTSELECTED ) &&
           ( engine_flags & VIEWVERTEX   ) ) ) {
        if ( ( engine_flags & SYMMETRYVIEW ) == 0x00 ) {
            uint32_t i, j, k;
            uint32_t n = 0x00;
            uint32_t nbpnt = ( ffd->nbx + 0x01 ) *
                             ( ffd->nby + 0x01 ) *
                             ( ffd->nbz + 0x01 );

            glPushAttrib ( GL_ALL_ATTRIB_BITS );
            glDisable ( GL_LIGHTING );

            glPointSize ( 3.0f );

            for ( i = 0x00; i <= ffd->nbx; i++ ) {
                for ( j = 0x00; j <= ffd->nby; j++ ) {
                    for ( k = 0x00; k <= ffd->nbz; k++ ) {
                        G3DVERTEX *pnt = &ffd->pnt[n++];

                        if ( pnt->flags & VERTEXSELECTED ) {
                            glColor3ub ( 0xFF, 0x00, 0x00 );
                        } else {
                            glColor3ub ( 0x00, 0x00, 0x00 );
                        }

                        glBegin ( GL_POINTS );
                        glVertex3fv ( ( const GLfloat * ) &pnt->pos );
                        glEnd ( );
                    }
                }
            }

            if ( ( engine_flags & SELECTMODE ) == 0x00 ) {
                uint32_t nbx = ffd->nbx + 0x01;
                uint32_t nby = ffd->nby + 0x01;
                uint32_t nbz = ffd->nbz + 0x01;
                uint32_t n = 0x00;

                glColor3ub ( 0xFF, 0x7F, 0x00 );
                glBegin ( GL_LINES );

                for ( n = 0x00; n < nbpnt; n++ ) {
                    G3DVERTEX *pnt0 = &ffd->pnt[n];
                    uint32_t   nxtz = ( n + 0x01 );
                    uint32_t   nxty = ( n + nbz  );
                    uint32_t   nxtx = ( n + ( nby * nbz ) );
                    G3DVERTEX *pntz = NULL;
                    G3DVERTEX *pnty = NULL;
                    G3DVERTEX *pntx = NULL;

                    if ( ( nxtz < nbpnt ) && ( nxtz % nbz ) ) {
                        pntz = &ffd->pnt[nxtz];

                        glVertex3fv ( ( const GLfloat * ) &pnt0->pos );
                        glVertex3fv ( ( const GLfloat * ) &pntz->pos );
                    }

                    if ( ( nxty < nbpnt ) && ( ( nxty % ( nbz * nby ) ) >= nbz ) ) {
                        pnty = &ffd->pnt[nxty];

                        glVertex3fv ( ( const GLfloat * ) &pnt0->pos );
                        glVertex3fv ( ( const GLfloat * ) &pnty->pos );
                    }

                    if ( nxtx < nbpnt ) {
                        pntx = &ffd->pnt[nxtx];

                        glVertex3fv ( ( const GLfloat * ) &pnt0->pos );
                        glVertex3fv ( ( const GLfloat * ) &pntx->pos );
                    }
                }

                glEnd ( );
            }

            glPopAttrib ( );
        }
    }

    return 0x00;
}

/******************************************************************************/
static uint32_t g3dffd_moddraw ( G3DOBJECT *obj,
                                 G3DCAMERA *cam, 
                                 uint64_t   engine_flags ) {
    G3DFFD *ffd = ( G3DFFD * ) obj;

    if ( g3dobject_isActive ( obj ) ) {
        g3dmesh_drawModified ( ffd->mod.oriobj,
                               cam,
                               ffd->mod.verpos,
                               ffd->mod.vernor,
                               engine_flags );

        return 0x00;
    }

    return 0x00;
}

/******************************************************************************/
void g3dffd_free ( G3DOBJECT *obj ) {
    G3DFFD *ffd = ( G3DFFD * ) obj;

    if ( ffd->pnt ) free ( ffd->pnt );

    list_free ( &((G3DMESH*)ffd)->lver, NULL );
    list_free ( &((G3DMESH*)ffd)->lselver, NULL );


    if ( ffd->uvw        ) free ( ffd->uvw        );
    if ( ffd->mod.verpos ) free ( ffd->mod.verpos );
    if ( ffd->mod.vernor ) free ( ffd->mod.vernor );

}

/******************************************************************************/
G3DFFD *g3dffd_new ( uint32_t id, char *name ) {
    G3DFFD *ffd = ( G3DFFD * ) calloc ( 0x01, sizeof ( G3DFFD ) );
    G3DMODIFIER *mod = ( G3DMODIFIER * ) ffd;

    if ( ffd == NULL ) {
        fprintf ( stderr, "g3dffd_new: calloc failed\n" );

        return NULL;
    }

    g3dmodifier_init ( mod, G3DFFDTYPE, id, name, OBJECTNOROTATION |
                                                  OBJECTNOSCALING  | 
                                                  MODIFIERNEEDSNORMALUPDATE,
                                    DRAW_CALLBACK(g3dffd_draw),
                                    FREE_CALLBACK(g3dffd_free),
                                    PICK_CALLBACK(g3dmesh_pick),
                                                  NULL,
                                                  NULL,
                                ACTIVATE_CALLBACK(g3dffd_activate),
                              DEACTIVATE_CALLBACK(g3dffd_deactivate),
                                  COMMIT_CALLBACK(g3dffd_commit),
                                                  NULL,
                                                  NULL,
                                  MODIFY_CALLBACK(g3dffd_modify) );

    ((G3DMESH*)mod)->onGeometryMove = g3dffd_onGeometryMove;

    /*obj->flags |= OBJECTNOSYMMETRY;*/

    mod->moddraw = g3dffd_moddraw;

    return ffd;
}

