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
/*** I cannot include setjmp in g3dengine/g3dengine.h because png.h complains about it ***/
#include <setjmp.h>
#include <g3dengine/g3dengine.h>



/******************************************************************************/
G3DENGINE* g3dengine_new ( ) {
    G3DENGINE *engine = calloc( 0x01, sizeof( G3DENGINE ) );

    if ( engine == NULL ) {
        fprintf( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    g3dengine_initShaders( engine );

    return engine;
}

/******************************************************************************/
void g3dengine_free( G3DENGINE *engine ) {
    free( engine );
}

/******************************************************************************/

/** the xxd file is generated with the following command :
  * xxd -i < triangleVertexShader.glsl > triangleVertexShader.xxd
  */

static char *coloredVertexShaderSource = & ( char[] ) {
#include "glsl/coloredVertexShader.xxd"
, 0x00 };

/** the xxd file is generated with the following command :
  * xxd -i < triangleFragmentShader.glsl > triangleFragmentShader.xxd
  */
static char *coloredFragmentShaderSource = & ( char[] ) {
#include "glsl/coloredFragmentShader.xxd"
, 0x00 };

static char *triangleVertexShaderSource = & ( char[] ) {
#include "glsl/shadedVertexShader.xxd"
, 0x00 };

/** the xxd file is generated with the following command :
  * xxd -i < triangleFragmentShader.glsl > triangleFragmentShader.xxd
  */
static char *triangleFragmentShaderSource = & ( char[] ) {
#include "glsl/shadedFragmentShader.xxd"
, 0x00 };

/******************************************************************************/
static void g3dengine_initColoredVertexShader( G3DENGINE *engine ) {
    int compileStatus;

    engine->coloredVertexShader = glCreateShader( GL_VERTEX_SHADER );

    glShaderSource( engine->coloredVertexShader,
                    0x01,
                   &coloredVertexShaderSource,
                    NULL );

    glCompileShader( engine->coloredVertexShader );

    glGetShaderiv( engine->coloredVertexShader,
                   GL_COMPILE_STATUS,
                   &compileStatus );

    if ( compileStatus == GL_FALSE ) {
        glGetShaderInfoLog( engine->coloredVertexShader,
                            sizeof( engine->log ),
                            NULL,
                            engine->log );

        fprintf( stderr, "GLSL compilation failed. Error is:\n%s", engine->log );
    } else {
        engine->flags |= ENGINE_COLORED_VERTEX_SHADER_INITIALIZED;
    }
}

/******************************************************************************/
static void g3dengine_initColoredFragmentShader( G3DENGINE *engine ) {
    int compileStatus;

    engine->coloredFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

    glShaderSource( engine->coloredFragmentShader,
                    0x01,
                   &coloredFragmentShaderSource,
                    NULL );

    glCompileShader( engine->coloredFragmentShader );

    glGetShaderiv( engine->coloredFragmentShader,
                   GL_COMPILE_STATUS,
                   &compileStatus );

    if ( compileStatus == GL_FALSE ) {
        glGetShaderInfoLog( engine->coloredFragmentShader,
                            sizeof( engine->log ),
                            NULL,
                            engine->log );

        fprintf( stderr, "GLSL compilation failed. Error is:\n%s", engine->log );
    } else {
        engine->flags |= ENGINE_COLORED_FRAGMENT_SHADER_INITIALIZED;
    }
}

/******************************************************************************/
static void g3dengine_initTriangleVertexShader( G3DENGINE *engine ) {
    int compileStatus;

    engine->triangleVertexShader = glCreateShader( GL_VERTEX_SHADER );

    glShaderSource( engine->triangleVertexShader,
                    0x01,
                   &triangleVertexShaderSource,
                    NULL );

    glCompileShader( engine->triangleVertexShader );

    glGetShaderiv( engine->triangleVertexShader,
                   GL_COMPILE_STATUS,
                   &compileStatus );

    if ( compileStatus == GL_FALSE ) {
        glGetShaderInfoLog( engine->triangleVertexShader,
                            sizeof( engine->log ),
                            NULL,
                            engine->log );

        fprintf( stderr, "GLSL compilation failed. Error is:\n%s", engine->log );
    } else {
        engine->flags |= ENGINE_TRIANGLE_VERTEX_SHADER_INITIALIZED;
    }
}

/******************************************************************************/
static void g3dengine_initTriangleFragmentShader( G3DENGINE *engine ) {
    int compileStatus;

    engine->triangleFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

    glShaderSource( engine->triangleFragmentShader,
                    0x01,
                   &triangleFragmentShaderSource,
                    NULL );

    glCompileShader( engine->triangleFragmentShader );

    glGetShaderiv( engine->triangleFragmentShader,
                   GL_COMPILE_STATUS,
                   &compileStatus );

    if ( compileStatus == GL_FALSE ) {
        glGetShaderInfoLog( engine->triangleFragmentShader,
                            sizeof( engine->log ),
                            NULL,
                            engine->log );

        fprintf( stderr, "GLSL compilation failed. Error is:\n%s", engine->log );
    } else {
        engine->flags |= ENGINE_TRIANGLE_FRAGMENT_SHADER_INITIALIZED;
    }
}

/******************************************************************************/
static void g3dengine_initTriangleShaderProgram ( G3DENGINE *engine ) {
    int linkingStatus;

    g3dengine_initTriangleVertexShader( engine );
    g3dengine_initTriangleFragmentShader( engine );

    engine->triangleShaderProgram = glCreateProgram();

    glAttachShader( engine->triangleShaderProgram,
                    engine->triangleVertexShader );

    glAttachShader( engine->triangleShaderProgram,
                    engine->triangleFragmentShader );

    glLinkProgram( engine->triangleShaderProgram );

    /* check for linking errors */
    glGetProgramiv( engine->triangleShaderProgram,
                    GL_LINK_STATUS,
                   &linkingStatus );

    if ( linkingStatus == GL_FALSE ) {
        glGetProgramInfoLog( engine->triangleShaderProgram,
                             512,
                             NULL,
                             engine->log );

        fprintf( stderr, "GLSL linking failed. Error is:\n%s", engine->log );
    } else {
        if( engine->flags & ENGINE_TRIANGLE_VERTEX_SHADER_INITIALIZED ) {
            glDeleteShader( engine->triangleVertexShader );
        }

        if( engine->flags & ENGINE_TRIANGLE_FRAGMENT_SHADER_INITIALIZED ) {
            glDeleteShader( engine->triangleFragmentShader );
        }

        engine->flags |= ENGINE_TRIANGLE_SHADER_PROGRAM_INITIALIZED;
    }
}

/******************************************************************************/
static void g3dengine_initColoredShaderProgram ( G3DENGINE *engine ) {
    int linkingStatus;

    g3dengine_initColoredVertexShader( engine );
    g3dengine_initColoredFragmentShader( engine );

    engine->coloredShaderProgram = glCreateProgram();

    glAttachShader( engine->coloredShaderProgram,
                    engine->coloredVertexShader );

    glAttachShader( engine->coloredShaderProgram,
                    engine->coloredFragmentShader );

    glLinkProgram( engine->coloredShaderProgram );

    /* check for linking errors */
    glGetProgramiv( engine->coloredShaderProgram,
                    GL_LINK_STATUS,
                   &linkingStatus );

    if ( linkingStatus == GL_FALSE ) {
        glGetProgramInfoLog( engine->coloredShaderProgram,
                             512,
                             NULL,
                             engine->log );

        fprintf( stderr, "GLSL linking failed. Error is:\n%s", engine->log );
    } else {
        if( engine->flags & ENGINE_COLORED_VERTEX_SHADER_INITIALIZED ) {
            glDeleteShader( engine->coloredVertexShader );
        }

        if( engine->flags & ENGINE_COLORED_FRAGMENT_SHADER_INITIALIZED ) {
            glDeleteShader( engine->coloredFragmentShader );
        }

        engine->flags |= ENGINE_COLORED_SHADER_PROGRAM_INITIALIZED;
    }
}

/******************************************************************************/
void g3dengine_initShaders ( G3DENGINE *engine ) {
    g3dengine_initTriangleShaderProgram( engine );
    g3dengine_initColoredShaderProgram( engine );

    glGenVertexArrays ( 1, &engine->vertexArray );
    glBindVertexArray ( engine->vertexArray );

    glGenBuffers( 1, &engine->vertexBuffer );
    glBindBuffer( GL_ARRAY_BUFFER, engine->vertexBuffer ); 
    glBufferData( GL_ARRAY_BUFFER,
                  sizeof( float ) * 36, /* enough for quads as well */
                  NULL,
                  GL_DYNAMIC_DRAW );


    glVertexAttribPointer( 0x00, /* attrib 0 is vertex position as vec3 */
                           0x03,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof( SHADERVERTEX ),
                           (void*) 0 );
    glEnableVertexAttribArray( 0x00 );

    glVertexAttribPointer( 0x01,
                           0x03,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof( SHADERVERTEX ),
                           (void*) 12 );
    glEnableVertexAttribArray( 0x01 );

    glVertexAttribPointer( 0x02,
                           0x03,
                           GL_FLOAT,
                           GL_FALSE,
                           sizeof( SHADERVERTEX ),
                           (void*) 24 );
    glEnableVertexAttribArray( 0x02 );

    glBindVertexArray(0);

    engine->flags |= ENGINE_VERTEX_BUFFER_INITIALIZED;
}

/******************************************************************************/
void g3dengine_drawTriangle ( G3DENGINE    *engine,
                              G3DOBJECT    *object,
                              SHADERVERTEX *vertices,
                              float         gouraudScalarLimit,
                              LIST         *ltex, 
                              uint32_t      object_flags,
                              uint64_t      engine_flags ) {
    glBindVertexArray( engine->vertexArray );
    glBindBuffer( GL_ARRAY_BUFFER, engine->vertexBuffer ); 
    glBufferSubData( GL_ARRAY_BUFFER,
                     0, sizeof( SHADERVERTEX ) * 3, vertices);

    glDrawArrays( GL_TRIANGLES, 0x00, 0x03 );
}

/******************************************************************************/
void g3dengine_drawQuad ( G3DENGINE    *engine,
                          G3DOBJECT    *object,
                          SHADERVERTEX *vertices,
                          float         gouraudScalarLimit,
                          LIST         *ltex, 
                          uint32_t      object_flags,
                          uint64_t      engine_flags ) {
    glBindVertexArray( engine->vertexArray );
    glBindBuffer( GL_ARRAY_BUFFER, engine->vertexBuffer ); 
    glBufferSubData( GL_ARRAY_BUFFER,
                     0, sizeof( SHADERVERTEX ) * 4, vertices);

    glDrawArrays( GL_TRIANGLE_FAN, 0x00, 0x04 );
}


/******************************************************************************/
void g3dengine_drawLine ( G3DENGINE    *engine,
                          G3DOBJECT    *object,
                          SHADERVERTEX *vertices,
                          uint32_t      object_flags,
                          uint64_t      engine_flags ) {
    glBindVertexArray( engine->vertexArray );
    glBindBuffer( GL_ARRAY_BUFFER, engine->vertexBuffer ); 
    glBufferSubData( GL_ARRAY_BUFFER,
                     0, sizeof( SHADERVERTEX ) * 2, vertices);

    glDrawArrays( GL_LINES, 0x00, 0x02 );
}
