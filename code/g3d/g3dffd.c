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
#include <g3d.h>

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
void g3dffd_modify ( G3DFFD *ffd ) {
    G3DVECTOR *uvw = ffd->uvw;
    G3DVECTOR *pos = ffd->pos;
    LIST *ltmpver = ffd->lver;
    uint32_t i, j, k;
    float xaxis = ( ffd->locmax.x - ffd->locmin.x ),
          yaxis = ( ffd->locmax.y - ffd->locmin.y ),
          zaxis = ( ffd->locmax.z - ffd->locmin.z );
    float difx = ( 1.0f / ffd->nbx ),
          dify = ( 1.0f / ffd->nby ),
          difz = ( 1.0f / ffd->nbz );
    float x, y, z;
    uint32_t n = 0x00;
    G3DOBJECT *parobj = ((G3DOBJECT*)ffd)->parent;

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
        G3DVECTOR vi = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        n = 0x00;

        /*memcpy ( &ver->pos, pos, sizeof ( G3DVECTOR ) );*/
        ver->pos.x = ffd->parmin.x;
        ver->pos.y = ffd->parmin.y;
        ver->pos.z = ffd->parmin.z;

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
        ver->pos.x += ( vi.x * xaxis );
        ver->pos.y += ( vi.y * yaxis );
        ver->pos.z += ( vi.z * zaxis );

        if ( parobj->type == G3DMESHTYPE ) {
            G3DOBJECT *grdparobj = parobj->parent;

            if ( grdparobj && grdparobj->childvertexchange ) {
                grdparobj->childvertexchange ( grdparobj, parobj, ver );
            }
        }

        uvw++;
        pos++;

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
/*** used when opening g3d files ***/
void g3dffd_appendVertex ( G3DFFD *ffd, G3DVERTEX *ver ) {
    list_append ( &ffd->lver, ver );
 
    ffd->nbver++;
}

/******************************************************************************/
void g3dffd_addVertex ( G3DFFD *ffd, G3DVERTEX *ver ) {
    list_insert ( &ffd->lver, ver );
 
    ffd->nbver++;
}

/******************************************************************************/
void g3dffd_generateuvw ( G3DFFD *ffd ) {
    LIST *ltmp = ffd->lver;
    float xaxis = ( ffd->parmax.x - ffd->parmin.x ),
          yaxis = ( ffd->parmax.y - ffd->parmin.y ),
          zaxis = ( ffd->parmax.z - ffd->parmin.z );
    uint32_t n = 0x00;

    if ( ffd->uvw ) { free ( ffd->uvw ); ffd->uvw = NULL; }

    ffd->uvw = ( G3DVECTOR * ) calloc ( ffd->nbver, sizeof ( G3DVECTOR ) );

    if ( ffd->uvw == NULL ) {
        fprintf ( stderr, "g3dffd_genuvw: memory allocation failed\n" );

        return;
    }

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        ffd->uvw[n].x = ( ver->pos.x - ffd->parmin.x ) / xaxis;
        ffd->uvw[n].y = ( ver->pos.y - ffd->parmin.y ) / yaxis;
        ffd->uvw[n].z = ( ver->pos.z - ffd->parmin.z ) / zaxis;

        n++;

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dffd_assign ( G3DFFD *ffd, G3DMESH *mes ) {
    G3DOBJECT *mesobj = ( G3DOBJECT * ) mes;
    G3DOBJECT *ffdobj = ( G3DOBJECT * ) ffd;
    LIST *ltmp = mes->lver;

    if ( ffd->pos ) { free ( ffd->pos ); ffd->pos = NULL; }

    /*list_free ( &ffd->lver, NULL );
    list_free ( &ffd->lfac, NULL );
    list_free ( &ffd->ledg, NULL );*/

    ffd->nbver = 0x00;

    g3dvector_matrix ( &ffd->locmin, ffdobj->lmatrix, &ffd->parmin );
    g3dvector_matrix ( &ffd->locmax, ffdobj->lmatrix, &ffd->parmax );

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        if ( ( ver->pos.x >= ffd->parmin.x ) && ( ver->pos.x <= ffd->parmax.x ) &&
             ( ver->pos.y >= ffd->parmin.y ) && ( ver->pos.y <= ffd->parmax.y ) &&
             ( ver->pos.z >= ffd->parmin.z ) && ( ver->pos.z <= ffd->parmax.z ) ) {

            g3dffd_addVertex ( ffd, ver );
        }

        ltmp = ltmp->next;
    }

    ffd->lfac = g3dvertex_getFacesFromList ( ffd->lver );
    ffd->ledg = g3dface_getEdgesFromList   ( ffd->lfac );

    g3dvertex_copyPositionFromList ( ffd->lver, &ffd->pos );

    g3dffd_generateuvw ( ffd );
}

/******************************************************************************/
void g3dffd_unassign ( G3DFFD *ffd ) {
    LIST *ltmpver = ffd->lver;
    uint32_t i = 0x00;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DVECTOR *pos = &ffd->pos[i];
        
        ver->pos.x = pos->x;
        ver->pos.y = pos->y;
        ver->pos.z = pos->z;

        i++;

        ltmpver = ltmpver->next;
    }

    list_exec ( ffd->lfac, (void (*)(void *)) g3dface_update );

    ltmpver = ffd->lver;
    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        g3dvertex_normal ( ver, COMPUTEFACEPOINT | COMPUTEEDGEPOINT );

        ltmpver = ltmpver->next;
    }

    list_free ( &ffd->lver, NULL );
    list_free ( &ffd->ledg, NULL );
    list_free ( &ffd->lfac, NULL );

    if ( ffd->pos ) { free ( ffd->pos ); ffd->pos = NULL; };
    if ( ffd->uvw ) { free ( ffd->uvw ); ffd->uvw = NULL; };

    ffd->nbver = 0x00;

    /*** revert the FFD to its original form ***/
    g3dffd_shape ( ffd, ffd->nbx , ffd->nby , ffd->nbz,
                        ffd->radx, ffd->rady, ffd->radz );
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

    /*list_free ( &((G3DMESH*)ffd)->lver, NULL );
    ((G3DMESH*)ffd)->nbver = 0x00;*/

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

                /*g3dmesh_addVertex ( ( G3DMESH * ) ffd, pnt );*/
            }
        }
    }
}

/******************************************************************************/
uint32_t g3dffd_draw ( G3DOBJECT *obj, G3DCAMERA *cam, uint32_t flags ) {
    G3DFFD *ffd = ( G3DFFD * ) obj;
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

                if ( flags & SELECTMODE ) glLoadName ( ( GLint ) pnt->id );

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

    if ( ( flags & SELECTMODE ) == 0x00 ) {
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

    return 0x00;
}

/******************************************************************************/
void g3dffd_free ( G3DOBJECT *obj ) {
    G3DFFD *ffd = ( G3DFFD * ) obj;

}

/******************************************************************************/
G3DFFD *g3dffd_new ( uint32_t id, char *name ) {
    G3DFFD *ffd = ( G3DFFD * ) calloc ( 0x01, sizeof ( G3DFFD ) );
    G3DOBJECT *obj = ( G3DOBJECT * ) ffd;

    if ( ffd == NULL ) {
        fprintf ( stderr, "g3dffd_new: calloc failed\n" );

        return NULL;
    }

    g3dobject_init ( obj, G3DFFDTYPE, id, name, DRAWBEFORECHILDREN,
                                                g3dffd_draw,
                                                g3dffd_free,
                                                NULL,
                                                NULL,
                                                NULL,
                                                NULL,
                                                NULL,
                                                NULL,
                                                NULL,
                                                NULL );

    obj->flags |= OBJECTNOSYMMETRY;


    return ffd;
}

