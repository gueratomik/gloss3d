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
static void g3dtext_vertex3dv ( double vertex_data[3], void *object_data ) {
    G3DTEXT *txt    = ( G3DTEXT * ) object_data;
    G3DMESH *txtmes = ( G3DMESH * ) txt;
    G3DVERTEX *ver = g3dmesh_seekVertexByPosition ( txtmes, vertex_data[0], 
                                                            vertex_data[1], 
                                                            vertex_data[2] );

    if ( ( ver == NULL ) ) {
        ver = g3dvertex_new ( vertex_data[0],
                              vertex_data[1],
                              vertex_data[2] );

        g3dmesh_addVertex ( txtmes, ver );
    }

    txt->verTab[txt->vertexCount++] = ver;

    if ( txt->vertexCount == 0x03 ) {
        G3DFACE *fac = g3dtriangle_new ( txt->verTab[0],
                                         txt->verTab[1],
                                         txt->verTab[2] );

        /*** Make sure all faces are oriented the same side ***/
        g3dface_normal ( fac );
        if ( fac->nor.z < 0.0f ) {
            fac->ver[0] = txt->verTab[2];
            fac->ver[1] = txt->verTab[1];
            fac->ver[2] = txt->verTab[0];
        }

        g3dmesh_addFace ( txtmes, fac );

        if ( txt->triangleTesselationType == GL_TRIANGLE_FAN ) {
            txt->verTab[0] = txt->verTab[0];
            txt->verTab[1] = txt->verTab[2];
            txt->verTab[2] = NULL;

            txt->vertexCount = 0x02;
        }

        if ( txt->triangleTesselationType == GL_TRIANGLE_STRIP ) {
            txt->verTab[0] = txt->verTab[1];
            txt->verTab[1] = txt->verTab[2];
            txt->verTab[2] = NULL;

            txt->vertexCount = 0x02;
        }

        if ( txt->triangleTesselationType == GL_TRIANGLES ) {
            txt->verTab[0] = txt->verTab[1] = txt->verTab[2] = NULL;

            txt->vertexCount = 0x00;
        }
    }
}

/******************************************************************************/
static void g3dtext_begin( GLenum type, void *object_data ) {
    G3DTEXT *txt    = ( G3DTEXT * ) object_data;
    G3DMESH *txtmes = ( G3DMESH * ) txt;

    txt->triangleTesselationType = type;

    txt->verTab[0] = txt->verTab[1] = txt->verTab[2] = NULL;

    txt->vertexCount = 0x00;
}

/******************************************************************************/
static void g3dtext_end( void *object_data ) {
    G3DTEXT *txt    = ( G3DTEXT * ) object_data;
    G3DMESH *txtmes = ( G3DMESH * ) txt;

    txt->verTab[0] = txt->verTab[1] = txt->verTab[2] = NULL;

    txt->vertexCount = 0x00;
}

/******************************************************************************/
void g3dtext_generate ( G3DOBJECT *obj, uint32_t   engine_flags ) {
    G3DTEXT *text = ( G3DTEXT * ) obj;
    FT_UInt  glyph_index;
    uint32_t i, j;
    short *contours;
    G3DVECTOR pos = { .x = 0.0f, .y = 0.0f, .z = 0.0f };
    double (*tessData)[0x03] = NULL;

    g3dmesh_clearGeometry ( text );

    if ( text->text ) {
        GLUtesselator *tobj = gluNewTess();

        gluTessProperty ( tobj, GLU_TESS_BOUNDARY_ONLY, GL_FALSE );
        gluTessCallback ( tobj, GLU_TESS_BEGIN_DATA , g3dtext_begin     );
        gluTessCallback ( tobj, GLU_TESS_VERTEX_DATA, g3dtext_vertex3dv );
        gluTessCallback ( tobj, GLU_TESS_END_DATA   , g3dtext_end       );

        /*gluTessCallback( tobj, GLenum(GLU_ERROR), (glu_callback) gltt_polygonizer_error );*/

        for ( i = 0x00; i < strlen ( text->text ); i++ ) {
            uint32_t e = 0x00;

            switch ( text->text[i] ) {
                case '\n' : {
                    pos.x  = 0.0f;
                    pos.y -= ( float ) text->face->glyph->metrics.width / 1000;
                } break;

                default : {
                    glyph_index = FT_Get_Char_Index ( text->face, text->text[i] );

                    FT_Load_Glyph ( text->face, glyph_index, FT_LOAD_DEFAULT );

                    contours = text->face->glyph->outline.contours;
                    FT_Vector *points = text->face->glyph->outline.points;

                    tessData = realloc ( tessData, sizeof ( double ) * 3 * text->face->glyph->outline.n_points );

                    gluTessBeginPolygon(tobj, text);
                    /*gluNextContour( tobj, GLU_EXTERIOR );*/
                    gluTessBeginContour(tobj);

                    /*glBegin ( GL_LINES );*/
                    for ( j = 0x00; j < text->face->glyph->outline.n_points; j++ ) {
                        uint32_t n = ( j + 0x01 ) % text->face->glyph->outline.n_points;

                        tessData[j][0x00] = ( double ) points[j].x / 1000 + pos.x;
                        tessData[j][0x01] = ( double ) points[j].y / 1000 + pos.y;
                        tessData[j][0x02] = 0.0f;
                        gluTessVertex ( tobj, tessData[j], tessData[j]);

                        /*printf ( "%d %d\n", (int32_t)points[j].x, (int32_t)points[j].y);*/
                        if ( j == *contours ) {
                            gluTessEndContour(tobj);
                            /*break;*/
                            gluTessBeginContour(tobj);
                            n = e; e = j + 0x01;
                            /*gluNextContour(tobj, GLU_INTERIOR);*/
                            contours++;
                        }

                        /*glVertex3dv ( tessData[j] );
                        glVertex3dv ( tessData[n] );*/
                    }
                    /*glEnd ( );
                    printf("%d\n", j);*/
                    gluTessEndPolygon(tobj);

                    pos.x += ( float ) text->face->glyph->metrics.width / 1000;
                } break;
            }
        }
        if ( tessData ) free ( tessData );
        gluDeleteTess(tobj);
    }

    g3dmesh_update ( text, NULL,
                             NULL,
                             NULL,
                             UPDATEFACEPOSITION |
                             UPDATEFACENORMAL   |
                             UPDATEVERTEXNORMAL, engine_flags );

    /*FT_Render_Glyph ( text->face->glyph, FT_RENDER_MODE_NORMAL );*/
}

/******************************************************************************/
void g3dtext_free ( G3DOBJECT *obj ) {
    G3DTEXT *text = ( G3DTEXT * ) obj;

    /*g3dmesh_free ( mes );*/
}

/******************************************************************************/
void g3dtext_init ( G3DTEXT *text, uint32_t id, 
                                   char    *name,
                                   uint32_t engine_flags ) {
    G3DSYSINFO *sysinfo = g3dsysinfo_get();
    G3DOBJECT *obj = ( G3DOBJECT * ) text;

    if ( text->face == NULL ) {
        if ( FT_New_Face( sysinfo->ftlib,
                          "/usr/share/fonts/X11/TTF/arial.ttf",
                          0,
                         &text->face ) ) {
            fprintf ( stderr, "FreeType font loading failed\n" );
        }
    }

    FT_Set_Char_Size( text->face, /* handle to face object           */
                      0,          /* char_width in 1/64th of points  */
                      16*64,      /* char_height in 1/64th of points */
                      300,        /* horizontal device resolution    */
                      300 );

    g3dobject_init ( obj, G3DTEXTTYPE, id, name, DRAWBEFORECHILDREN,
                                     DRAW_CALLBACK(g3dmesh_draw),
                                     FREE_CALLBACK(g3dtext_free),
                                                   NULL,
                                                   NULL,
                                     COPY_CALLBACK(NULL),
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                 ADDCHILD_CALLBACK(NULL),
                                                   NULL );

    ((G3DMESH*)text)->dump = g3dmesh_default_dump;

    text->text = "Gloss3D\nIs Nice";

    g3dtext_generate ( text, engine_flags );

}

/******************************************************************************/
G3DTEXT *g3dtext_new ( uint32_t id, 
                       char    *name,
                       uint32_t engine_flags ) {
    G3DSYSINFO *sysinfo = g3dsysinfo_get();
    G3DTEXT *text = ( G3DTEXT * ) calloc ( 0x01, sizeof ( G3DTEXT ) );

    if ( text == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    if ( sysinfo->ftlib == NULL ) {
        if ( FT_Init_FreeType( &sysinfo->ftlib ) ) {
            fprintf ( stderr, "FreeType initialization failed\n" );
        }
    }

    /** type is CUBIC or QUADRATIC **/
    g3dtext_init ( text, id, name, engine_flags );

    return text;
}

