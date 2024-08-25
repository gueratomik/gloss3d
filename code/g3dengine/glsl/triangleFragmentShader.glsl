#version 330 core     

// generate C char with 
// xxd -i < triangleFragmentShader.glsl > triangleFragmentShader.xxd

out vec4 FragColor;

in vec3 vertexShadedColor;

void main()
{
   FragColor = vec4( vertexShadedColor, 1.0f );
   //FragColor = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
}
