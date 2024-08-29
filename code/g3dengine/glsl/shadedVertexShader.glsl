#version 330 core

// generate C char with 
// xxd -i < triangleVertexShader.glsl > triangleVertexShader.xxd

// thanks to https://www.learnopengles.com/tag/gouraud-shading/

layout (location = 0) in vec3 vertexLocalPosition;
layout (location = 1) in vec3 vertexLocalNormal;
layout (location = 2) in vec3 vertexBasicColor;

uniform mat4 mvpMatrix;
uniform mat4 mvwMatrix;
uniform mat4 norMatrix;

out vec3 vertexShadedColor;

void main() {
    vec4 light0WorldPosition  = mvwMatrix * vec4( -100.0f, -100.0f, -100.0f, 1.0f );
    vec4 light1WorldPosition  = mvwMatrix * vec4(  100.0f,  100.0f,  100.0f, 1.0f );

    vec4 vertexWorldPosition = mvwMatrix * vec4( vertexLocalPosition, 1.0 );
 // other option :
 // vec4 vertexWorldNormal   = mvwMatrix * vec4( vertexLocalNormal  , 0.0 );
 // see : https://stackoverflow.com/questions/13654401/why-transform-normals-with-the-transpose-of-the-inverse-of-the-modelview-matrix
    vec4 vertexWorldNormal   = norMatrix * vec4( vertexLocalNormal  , 1.0 );

    // Get a lighting direction vector from the light to the vertex.
    vec3 light0Vector = normalize( light0WorldPosition.xyz - vertexWorldPosition.xyz );
    vec3 light1Vector = normalize( light1WorldPosition.xyz - vertexWorldPosition.xyz );

    // Calculate the dot product of the light vector and vertex normal. If the normal and light vector are
    // pointing in the same direction then it will get max illumination.
    float diffuse0 = max( dot ( vertexWorldNormal.xyz, light0Vector ), 0.0f );
    float diffuse1 = max( dot ( vertexWorldNormal.xyz, light1Vector ), 0.0f );

    vertexShadedColor = vec3( 0.0, 0.0, 0.0 );

    vertexShadedColor += vec3( min( ( vertexBasicColor.x * diffuse0 ), 1.0 ),
                               min( ( vertexBasicColor.y * diffuse0 ), 1.0 ),
                               min( ( vertexBasicColor.z * diffuse0 ), 1.0 ) );

    vertexShadedColor += vec3( min( ( vertexBasicColor.x * diffuse1 ), 1.0 ),
                               min( ( vertexBasicColor.y * diffuse1 ), 1.0 ),
                               min( ( vertexBasicColor.z * diffuse1 ), 1.0 ) );

    gl_Position = mvpMatrix * vec4( vertexLocalPosition, 1.0 );
}
