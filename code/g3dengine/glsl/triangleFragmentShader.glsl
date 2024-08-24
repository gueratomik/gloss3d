#version 330 core     

// generate C char with 
// xxd -i < triangleFragmentShader.glsl > triangleFragmentShader.xxd

out vec4 FragColor;

void main()
{
   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
