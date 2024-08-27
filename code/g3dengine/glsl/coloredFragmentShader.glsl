#version 330 core     

// generate C char with 
// xxd -i < coloredFragmentShader.glsl > coloredFragmentShader.xxd

out vec4 FragColor;

in vec3 vertexShadedColor;

void main()
{
   FragColor = vec4( vertexShadedColor, 1.0f );
}
