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
#include <g3dengine/g3dengine.h>

/******************************************************************************/
void g3dsplinerevolver_setParent ( G3DSPLINEREVOLVER *srv,
                                   G3DOBJECT         *parent,
                                   uint32_t           engine_flags ) {
    if ( g3dobject_isActive ( (G3DOBJECT*) srv ) ) {
        g3dsplinerevolver_activate ( srv, engine_flags );
    }
}

/******************************************************************************/
G3DSPLINEREVOLVER *g3dsplinerevolver_copy ( G3DSPLINEREVOLVER *srv,
                                            uint32_t           engine_flags ) {
    G3DOBJECT *objsrv = ( G3DOBJECT * ) srv;

    return g3dsplinerevolver_new ( objsrv->id, objsrv->name );
}

/******************************************************************************/
void g3dsplinerevolver_startUpdate ( G3DSPLINEREVOLVER *srv, 
                                     uint32_t           engine_flags ) {
    G3DMODIFIER *mod = ( G3DMODIFIER * ) srv;
    G3DOBJECT   *obj = ( G3DOBJECT   * ) srv;
    G3DOBJECT   *parent = g3dobject_getActiveParentByType ( obj, SPLINE );

    if ( parent ) {
    }
}

/******************************************************************************/
void g3dsplinerevolver_endUpdate ( G3DSPLINEREVOLVER *srv,
                                   uint32_t           engine_flags ) {

}

/******************************************************************************/
void g3dsplinerevolver_update ( G3DSPLINEREVOLVER *srv, 
                                uint32_t           engine_flags ) {
    G3DOBJECT *obj    = ( G3DOBJECT * ) srv;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, SPLINE );

    if ( parent ) {
        G3DSPLINE *spl = ( G3DSPLINE * ) parent;

    }
}

/******************************************************************************/
uint32_t g3dsplinerevolver_modify ( G3DSPLINEREVOLVER *srv, 
                                    uint32_t           engine_flags ) {
    G3DOBJECT *obj    = ( G3DOBJECT * ) srv;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, SPLINE );

    /*g3dmesh_clearGeometry ( (G3DMESH*) wir );*/
    srv->nbsteps = 12;
    srv->nbdivis = 1;

    if ( parent ) {
        G3DSPLINE *spl    = ( G3DSPLINE * ) parent;
        G3DMESH   *splmes = ( G3DMESH   * ) spl;
        G3DOBJECT *splobj = ( G3DOBJECT * ) spl;
        G3DMESH   *srvmes = ( G3DMESH   * ) srv;
        G3DOBJECT *srvobj = ( G3DOBJECT * ) srv;
        uint32_t   nbSplineVertices = splmes->nbver - ( spl->nbseg * 0x02 );
        uint32_t   nbRevolvedVertices = ( nbSplineVertices * srv->nbsteps ) + ( ( srv->nbdivis - 1 ) * srv->nbsteps );
        uint32_t   nbRevolvedFaces    = ( nbSplineVertices - 1 ) * srv->nbdivis * ( srv->nbsteps - 1 );
        LIST *ltmpver = splmes->lver;
        G3DSUBVERTEX *srvVertices;
        G3DSUBEDGE   *srvEdges;
        G3DSUBFACE   *srvFaces;
        uint32_t i, j, k = 0x00, n = 0x00;

        srvVertices = srvmes->lver = realloc ( srvmes->lver, nbRevolvedVertices * sizeof ( G3DSUBVERTEX ) );
        /*srvmes->ledg = realloc ( mes->ledg, mes->nbedg * sizeof ( G3DSUBVERTEX ) );*/
        srvFaces    = srvmes->lfac = realloc ( srvmes->lfac, nbRevolvedFaces    * sizeof ( G3DSUBFACE   ) );

        g3dmesh_setGeometryInArrays ( srvmes, srvmes->lver, 
                                              nbRevolvedVertices,
                                              NULL,
                                              0,
                                              srvmes->lfac,
                                              nbRevolvedFaces );

        memset ( srvmes->lver, 0x00, nbRevolvedVertices * sizeof ( G3DSUBVERTEX ) );
        /*memset ( wir->modedg, 0x00, wir->nbmodedg * sizeof ( G3DSUBEDGE   ) );*/
        memset ( srvmes->lfac, 0x00, nbRevolvedFaces    * sizeof ( G3DSUBFACE   ) );

        /*** copy original vertices to the begining of the array ***/
        LIST *ltmpver = splmes->lver;
        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

            if ( ver->flags & VERTEXHANDLE ) == 0x00 ) {
                memcpy ( &srvVertices[n++].ver.pos, &ver->pos, sizeof ( G3DVECTOR ) );
            }

            ltmpver = ltmpver->next;
        }

        LIST *ltmpseg = spl->lseg;
        while ( ltmpseg ) {
            G3DSPLINESEGMENT *seg = ( G3DSPLINESEGMENT * ) ltmpseg->data;
            G3DVECTOR  verpos;
            G3DVECTOR  vertexLocalPos;
            float incrementFactor = 1.0f / srv->nbdivis;
            float factor = 0.0f;

            for ( i = 0x00; i < srv->nbdivis; i++ ) {
                g3dsplinesegment_getPoint ( seg, factor, &verpos );
                factor += incrementFactor;

                g3dvector_matrix ( &verpos, srvobj->lmatrix, &vertexLocalPos );

                for ( j = 0x00; j < srv->nbsteps; j++ ) {
                    G3DVECTOR vertexPositionAfterRotation;
                    double RMX[0x10];

                    glPushMatrix ( );
                    glLoadIdentity ( );
                    glRotatef ( (float ) 360 / srv->nbsteps * j, 0.0f, 1.0f, 0.0f );
                    /*glRotatef ( rot.y, 0.0f, 1.0f, 0.0f );
                    glRotatef ( rot.z, 0.0f, 0.0f, 1.0f );*/
                    glGetDoublev ( GL_MODELVIEW_MATRIX, RMX );
                    glPopMatrix ( );

                    g3dvector_matrix ( &vertexLocalPos, RMX, &srvVertices[(k*srv->nbdivis*srv->nbsteps)+(i*srv->nbsteps)+j].ver.pos );
                }
            }

            k++;

            ltmpseg = ltmpseg->next;
        }

        k = n = 0x00;

        ltmpseg = spl->lseg;
        while ( ltmpseg ) {
            for ( i = 0x00; i < srv->nbdivis; i++ ) {
                for ( j = 0x00; j < srv->nbsteps - 1; j++ ) {
                    G3DSUBVERTEX *subver = ( G3DSUBVERTEX * ) srvmes->lver;
                    uint32_t id[0x04] = { ( k * srv->nbdivis * srv->nbsteps ) + ( i * srv->nbsteps ) + j,
                                          ( k * srv->nbdivis * srv->nbsteps ) + ( i * srv->nbsteps ) + j + 1,
                                          ( k * srv->nbdivis * srv->nbsteps ) + ( i * srv->nbsteps ) + j + srv->nbsteps + 1, 
                                          ( k * srv->nbdivis * srv->nbsteps ) + ( i * srv->nbsteps ) + j + srv->nbsteps };
                    srvFaces[n].fac.ver[0x00] = &srvVertices[id[0x00]].ver;
                    srvFaces[n].fac.ver[0x01] = &srvVertices[id[0x01]].ver;
                    srvFaces[n].fac.ver[0x02] = &srvVertices[id[0x02]].ver;
                    srvFaces[n].fac.ver[0x03] = &srvVertices[id[0x03]].ver;

                    g3dsubvertex_addFace ( srvFaces[n].fac.ver[0x00], &srvFaces[n].fac );
                    g3dsubvertex_addFace ( srvFaces[n].fac.ver[0x01], &srvFaces[n].fac );
                    g3dsubvertex_addFace ( srvFaces[n].fac.ver[0x02], &srvFaces[n].fac );
                    g3dsubvertex_addFace ( srvFaces[n].fac.ver[0x03], &srvFaces[n].fac );

                    srvFaces[n].fac.nbver = 0x04;

                    g3dface_normal ( &srvFaces[n].fac );

                    n++;
                }
            }

            k++;

            ltmpseg = ltmpseg->next;
        }

        for ( i = 0x00; i < srvmes->nbver; i++ ) {
            g3dvertex_normal ( &srvVertices[i].ver, 0x00 );
        }
    }
}

/******************************************************************************/
void g3dsplinerevolver_activate ( G3DSPLINEREVOLVER *srv, 
                                  uint32_t           engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) srv;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, SPLINE );

    if ( parent ) {
        g3dmesh_modify_r ( (G3DMESH*)parent, engine_flags );
    }
}

/******************************************************************************/
void g3dsplinerevolver_deactivate ( G3DSPLINEREVOLVER *srv, 
                                    uint32_t           engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) srv;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, SPLINE );

    if ( parent ) {
        g3dmesh_modify_r ( parent, engine_flags );
    }
}

/******************************************************************************/
uint32_t g3dsplinerevolver_draw ( G3DSPLINEREVOLVER *srv, 
                                  G3DCAMERA         *cam, 
                                  uint32_t           engine_flags ) {
    G3DMESH *srvmes = ( G3DMESH * ) srv;
    uint32_t i;

    if ( ( engine_flags & SELECTMODE ) == 0x00 ) {
        glBegin ( GL_QUADS );
        for ( i = 0x00; i < srvmes->nbfac; i++ ) {
            G3DSUBFACE *subfac = ( G3DSUBFACE * ) srvmes->lfac;
            glNormal3f ( subfac[i].fac.ver[0x00]->nor.x, 
                         subfac[i].fac.ver[0x00]->nor.y, 
                         subfac[i].fac.ver[0x00]->nor.z );
            glVertex3f ( subfac[i].fac.ver[0x00]->pos.x, 
                         subfac[i].fac.ver[0x00]->pos.y, 
                         subfac[i].fac.ver[0x00]->pos.z );

            glNormal3f ( subfac[i].fac.ver[0x01]->nor.x, 
                         subfac[i].fac.ver[0x01]->nor.y, 
                         subfac[i].fac.ver[0x01]->nor.z );
            glVertex3f ( subfac[i].fac.ver[0x01]->pos.x, 
                         subfac[i].fac.ver[0x01]->pos.y, 
                         subfac[i].fac.ver[0x01]->pos.z );

            glNormal3f ( subfac[i].fac.ver[0x02]->nor.x, 
                         subfac[i].fac.ver[0x02]->nor.y, 
                         subfac[i].fac.ver[0x02]->nor.z );
            glVertex3f ( subfac[i].fac.ver[0x02]->pos.x, 
                         subfac[i].fac.ver[0x02]->pos.y, 
                         subfac[i].fac.ver[0x02]->pos.z );

            glNormal3f ( subfac[i].fac.ver[0x03]->nor.x, 
                         subfac[i].fac.ver[0x03]->nor.y, 
                         subfac[i].fac.ver[0x03]->nor.z );
            glVertex3f ( subfac[i].fac.ver[0x03]->pos.x, 
                         subfac[i].fac.ver[0x03]->pos.y, 
                         subfac[i].fac.ver[0x03]->pos.z );
        }
        glEnd();
    }

    return MODIFIERTAKESOVER;
}

/******************************************************************************/
void g3dsplinerevolver_free ( G3DSPLINEREVOLVER *srv ) {

}

/******************************************************************************/
G3DSPLINEREVOLVER *g3dsplinerevolver_new ( uint32_t id, char *name ) {
    uint32_t structSize = sizeof ( G3DSPLINEREVOLVER );
    G3DSPLINEREVOLVER *srv = ( G3DSPLINEREVOLVER * ) calloc ( 0x01, structSize );
    G3DMODIFIER *mod = ( G3DMODIFIER * ) srv;

    if ( srv == NULL ) {
        fprintf ( stderr, "g3dsplinerevolver_new: calloc failed\n" );

        return NULL;
    }

    g3dmodifier_init ( mod, G3DSPLINEREVOLVERTYPE, id, name, DRAWBEFORECHILDREN,
                                                             g3dsplinerevolver_draw,
                                                             g3dsplinerevolver_free,
                                                             NULL,
                                                             NULL,
                                                             g3dsplinerevolver_copy,
                                                             g3dsplinerevolver_activate,
                                                             g3dsplinerevolver_deactivate,
                                                             g3dmesh_copy,
                                                             NULL,
                                                             g3dsplinerevolver_setParent,
                                                             g3dsplinerevolver_modify,
                                                             g3dsplinerevolver_startUpdate,
                                                             g3dsplinerevolver_update,
                                                             g3dsplinerevolver_endUpdate );


    ((G3DMESH*)srv)->dump = g3dmesh_default_dump;

    return srv;
}
