#version 330 core

// generate C char with 
// xxd -i < triangleVertexShader.glsl > triangleVertexShader.xxd

// thanks to https://www.learnopengles.com/tag/gouraud-shading/

layout (location = 0) in vec3 vertexLocalPosition;
layout (location = 1) in vec3 vertexLocalNormal;
layout (location = 2) in vec3 vertexBasicColor;

uniform mat4 posMatrix;
uniform mat4 norMatrix;

out vec3 vertexShadedColor;

void main() {
    vec3 lightWorldPosition  = vec3( vec4 ( -100.0f, -100.0f, -100.0f, 1.0 ) );

    vec3 vertexWorldPosition = vec3( posMatrix * vec4( vertexLocalPosition, 1.0 ) );
    vec3 vertexWorldNormal   = vec3( norMatrix * vec4( vertexLocalNormal  , 1.0 ) );

    // Get a lighting direction vector from the light to the vertex.
    vec3 lightVector = normalize( lightWorldPosition - vertexWorldPosition );

    // Calculate the dot product of the light vector and vertex normal. If the normal and light vector are
    // pointing in the same direction then it will get max illumination.
    float diffuse = max( dot ( vertexLocalPosition, lightVector ), 0.0f );

    vertexShadedColor = vertexBasicColor * diffuse;

    gl_Position = vec4( vertexWorldPosition, 1.0f );
}
