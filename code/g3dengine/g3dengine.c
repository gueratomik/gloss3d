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

static const char triangleVertexShaderSource[] = {
#include "glsl/triangleVertexShader.xxd"
, 0x00 };

/** the xxd file is generated with the following command :
  * xxd -i < triangleFragmentShader.glsl > triangleFragmentShader.xxd
  */
static const char triangleFragmentShaderSource[] = {
#include "glsl/triangleFragmentShader.xxd"
, 0x00 };

/******************************************************************************/
static void g3dengine_initTriangleVertexShader( G3DENGINE *engine ) {
    const char *sourceArray[] = { triangleVertexShaderSource };
    int compileStatus;

    engine->triangleVertexShader = glCreateShader( GL_VERTEX_SHADER );

    glShaderSource( engine->triangleVertexShader,
                    0x01,
                    sourceArray,
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
    const char *sourceArray[] = { triangleFragmentShaderSource };
    int compileStatus;

    engine->triangleFragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

    glShaderSource( engine->triangleFragmentShader,
                    0x01,
                    sourceArray,
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
void g3dengine_drawTriangle ( G3DENGINE *engine,
                              G3DOBJECT *object,
                              G3DFACE *triangle,
                              float    gouraudScalarLimit,
                              LIST    *ltex, 
                              uint32_t object_flags,
                              uint64_t engine_flags ) {
    G3DVERTEX **ver = triangle->ver;
/*
    float vertices[] = { ver[0x00]->pos.x, ver[0x00]->pos.y, ver[0x00]->pos.z,
                         ver[0x01]->pos.x, ver[0x01]->pos.y, ver[0x01]->pos.z,
                         ver[0x02]->pos.x, ver[0x02]->pos.y, ver[0x02]->pos.z };
*/
    int linkingStatus;

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    if( ( engine->flags & ENGINE_TRIANGLE_VERTEX_SHADER_INITIALIZED ) == 0x00 ) {
        g3dengine_initTriangleVertexShader( engine );
    }

    if( ( engine->flags & ENGINE_TRIANGLE_FRAGMENT_SHADER_INITIALIZED ) == 0x00 ) {
        g3dengine_initTriangleFragmentShader( engine );
    }

    if( ( engine->flags & ENGINE_TRIANGLE_SHADER_PROGRAM_INITIALIZED ) == 0x00 ) {
        engine->triangleShaderProgram = glCreateProgram();

        if( engine->flags & ENGINE_TRIANGLE_VERTEX_SHADER_INITIALIZED ) {
            glAttachShader( engine->triangleShaderProgram,
                            engine->triangleVertexShader );
        }

        if( engine->flags & ENGINE_TRIANGLE_FRAGMENT_SHADER_INITIALIZED ) {
            glAttachShader( engine->triangleShaderProgram,
                            engine->triangleFragmentShader );
        }

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

    if( ( engine->flags & ENGINE_TRIANGLE_BUFFER_INITIALIZED ) == 0x00 ) {
        glGenBuffers( 1, &engine->triangleBuffer );
        glBindBuffer( GL_ARRAY_BUFFER, engine->triangleBuffer ); 

        glVertexAttribPointer( 0x00, /* attrib 0 is vertex position as vec3 */
                               0x03,
                               GL_FLOAT,
                               GL_FALSE,
                               0x03 * sizeof( float ),
                               (void*) 0x00 );
        glEnableVertexAttribArray( 0x00 );

        engine->flags |= ENGINE_TRIANGLE_BUFFER_INITIALIZED;
    }

    glBindBuffer( GL_ARRAY_BUFFER, engine->triangleBuffer ); 
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);  
    glBufferData( GL_ARRAY_BUFFER,
                  sizeof( vertices ),
                  vertices,
                  GL_STATIC_DRAW );

    glDrawArrays( GL_TRIANGLES, 0x00, 0x03 );
}
