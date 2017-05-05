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
void g3dtext_generate_draw ( G3DOBJECT *obj, 
                             G3DCAMERA *curcma,
                             uint32_t engine_flags ) {
    g3dtext_generate ( obj, engine_flags );
}

/******************************************************************************/
void g3dtext_generate ( G3DOBJECT *obj, 
                        uint32_t engine_flags ) {
    G3DTEXT *text = ( G3DTEXT * ) obj;
    FT_UInt  glyph_index;
    uint32_t i, j;
    G3DVECTOR pos = { .x = 0.0f, .y = 0.0f, .z = 0.0f };
    double (*tessData)[0x03] = NULL;
#define COORD_ARRAY_COUNT 0x200
    static double coords[COORD_ARRAY_COUNT][0x03] = { 0 };

    g3dmesh_clearGeometry ( text );

    if ( text->text ) {
        GLUtesselator *tobj = gluNewTess();

        gluTessProperty ( tobj, GLU_TESS_BOUNDARY_ONLY, GL_FALSE );
        gluTessCallback ( tobj, GLU_TESS_BEGIN_DATA , g3dtext_begin     );
        gluTessCallback ( tobj, GLU_TESS_VERTEX_DATA, g3dtext_vertex3dv );
        gluTessCallback ( tobj, GLU_TESS_END_DATA   , g3dtext_end       );

        /*gluTessCallback( tobj, GLenum(GLU_ERROR), (glu_callback) gltt_polygonizer_error );*/

        for ( i = 0x00; i < strlen ( text->text ); i++ ) {
            uint32_t maxSteps = COORD_ARRAY_COUNT;
            uint32_t nbStepsPerSegment = 6;
            uint32_t handleID = 0x00;

            switch ( text->text[i] ) {
                case '\n' : {
                    pos.x  = 0.0f;
                    pos.y -= ( float ) text->face->glyph->metrics.width / 1000;
                } break;

                default : {
                    uint32_t verDataSize = sizeof ( double ) * 0x03;
                    uint32_t n_points;
                    FT_Vector *points;
                    short *contours;
                    uint32_t firstPoint = 0x01;
                    uint32_t firstPointID = 0x00;

                    glyph_index = FT_Get_Char_Index ( text->face, text->text[i] );

                    FT_Load_Glyph ( text->face, glyph_index, FT_LOAD_DEFAULT );

                    contours = text->face->glyph->outline.contours;
                    n_points = text->face->glyph->outline.n_points;
                    points   = text->face->glyph->outline.points;

                    tessData = realloc ( tessData, verDataSize * n_points );

                    gluTessBeginPolygon ( tobj, text );
                    gluTessBeginContour ( tobj ); /*glBegin ( GL_LINE_LOOP );*/

                    for ( j = 0x00; j < text->face->glyph->outline.n_points; j++ ) {
                        FT_Outline *outline = &text->face->glyph->outline;
                        uint32_t h = ( ( j - 0x01 + n_points ) % n_points );
                        uint32_t n = ( ( j + 0x01 ) % n_points );
                        char htag = FT_CURVE_TAG ( outline->tags[h] ),
                             jtag = FT_CURVE_TAG ( outline->tags[j] ),
                             ntag = FT_CURVE_TAG ( outline->tags[n] );
                        G3DQUADRATICSEGMENT qsg;
                        G3DSPLINEPOINT pt0, pt1;

                        /*if ( h == 0x00 ) h = firstPoint;
                        if ( n == 0x00 ) n = firstPoint;*/

                        tessData[j][0x00] = ( double ) points[j].x;
                        tessData[j][0x01] = ( double ) points[j].y;
                        tessData[j][0x02] = 0.0f;

                        switch ( jtag ) {
                            case FT_CURVE_TAG_CONIC :
printf("FT_CURVE_TAG_CONIC: %d %d\n", h, n);
                                if ( htag == FT_CURVE_TAG_ON ) {
                                    pt0.ver.pos.x = ( double ) points[h].x;
                                    pt0.ver.pos.y = ( double ) points[h].y;
                                    pt0.ver.pos.z = 0.0f;
                                }

                                if ( ntag == FT_CURVE_TAG_ON ) {
                                    pt1.ver.pos.x = ( double ) points[n].x;
                                    pt1.ver.pos.y = ( double ) points[n].y;
                                    pt1.ver.pos.z = 0.0f;
                                }

                                if ( htag == FT_CURVE_TAG_CONIC ) {
                                    pt0.ver.pos.x = ( ( double ) points[h].x + tessData[j][0x00] ) * 0.5f;
                                    pt0.ver.pos.y = ( ( double ) points[h].y + tessData[j][0x01] ) * 0.5f;
                                    pt0.ver.pos.z = 0.0f;
                                }

                                if ( ntag == FT_CURVE_TAG_CONIC ) {
                                    pt1.ver.pos.x = ( ( double ) points[n].x + tessData[j][0x00] ) * 0.5f;
                                    pt1.ver.pos.y = ( ( double ) points[n].y + tessData[j][0x01] ) * 0.5f;
                                    pt1.ver.pos.z = 0.0f;
                                }

                                /*** if it's the last point of the curve ***/
                                if ( j == *contours ) {
printf("using firstPointID %d\n", firstPointID);
                                    pt1.ver.pos.x = ( ( double ) points[firstPointID].x + tessData[j][0x00] ) * 0.5f;
                                    pt1.ver.pos.y = ( ( double ) points[firstPointID].y + tessData[j][0x01] ) * 0.5f;
                                    pt1.ver.pos.z = 0.0f;
                                }

                                pt0.ver.pos.x = ( pt0.ver.pos.x / 1000 ) + pos.x;
                                pt0.ver.pos.y = ( pt0.ver.pos.y / 1000 ) + pos.y;
                                pt0.ver.pos.z = 0.0f;

                                pt1.ver.pos.x = ( pt1.ver.pos.x / 1000 ) + pos.x;
                                pt1.ver.pos.y = ( pt1.ver.pos.y / 1000 ) + pos.y;
                                pt1.ver.pos.z = 0.0f;

                                tessData[j][0] = ( tessData[j][0] / 1000 ) + pos.x;
                                tessData[j][1] = ( tessData[j][1] / 1000 ) + pos.y;
                                tessData[j][2] = 0.0f;

/*g3dvector_print ( &pt0.ver.pos );
g3dvector_print ( &pt1.ver.pos );*/

                                g3dquadraticsegment_init ( &qsg, 
                                                           &pt0,
                                                           &pt1,
                                                            tessData[j][0],
                                                            tessData[j][1],
                                                            tessData[j][2] );

                                g3dsplinesegment_draw ( &qsg, 
                                                         0.0f, 
                                                         1.0f, 
                                                         1,
                                                         tobj,
                                                         coords[nbStepsPerSegment*handleID++],
                                                         DRAW_FOR_TESSELLATION,
                                                         engine_flags );

                                if ( maxSteps > nbStepsPerSegment ) {
                                    maxSteps -= nbStepsPerSegment;
                                } else {
                                    maxSteps = 0x00;
                                }
                            break;

                            case FT_CURVE_TAG_CUBIC :
                                fprintf ( stderr, "cubic fonts are unsupported!\n");
                            break;

                            case FT_CURVE_TAG_ON : {
                                if ( ( firstPoint              ) ||
                                     ( htag == FT_CURVE_TAG_ON ) ||
                                     ( ntag == FT_CURVE_TAG_ON ) ) {
                                    tessData[j][0] = ( tessData[j][0] / 1000 ) + pos.x;
                                    tessData[j][1] = ( tessData[j][1] / 1000 ) + pos.y;
                                    tessData[j][2] = 0.0f;
printf("FT_CURVE_TAG_ON: %d %d\n", h, n);
                                    gluTessVertex ( tobj, tessData[j], 
                                                          tessData[j] );
                                    /*glVertex3dv ( tessData[j] );*/
                                }
                            } break;

                            default :
                            break;
                        }

                        if ( j == *contours ) {
                            firstPoint = 0x01;
                            firstPointID = j + 1;
printf("firstPoint:%d\n", firstPoint );
                            gluTessEndContour ( tobj ); break; /*glEnd ();*/
                            gluTessBeginContour ( tobj ); /*glBegin ( GL_LINE_LOOP );*/
                            contours++;
                        } else {
                            firstPoint = 0x00;
                        }
                    }
                    gluTessEndPolygon ( tobj ); /*glEnd ( );*/

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

    text->text = "AeA" /*"ABCDEFGHIJKLMNOPQRSTUVWXYZ\n"
                 "abcdefghijklmnopqrstuvwxyz"*/;

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

