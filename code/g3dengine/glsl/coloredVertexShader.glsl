#version 330 core

// generate C char with 
// xxd -i < coloredVertexShader.glsl > coloredVertexShader.xxd

// thanks to https://www.learnopengles.com/tag/gouraud-shading/

layout (location = 0) in vec3 vertexLocalPosition;
layout (location = 1) in vec3 vertexLocalNormal;
layout (location = 2) in vec3 vertexBasicColor;

uniform mat4 posMatrix;

out vec3 vertexShadedColor;

void main() {
    vec4 vertexWorldPosition = posMatrix * vec4( vertexLocalPosition, 1.0 );

    vertexShadedColor = vertexBasicColor;

    gl_Position = vertexWorldPosition;
}
