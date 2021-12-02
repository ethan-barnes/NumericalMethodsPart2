#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 vertexUV;

out vec3 FragPos;
out vec2 UV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 transform;

void main()
{
    gl_Position = transform * (projection * view * model * vec4(position, 1.0f));
    FragPos = vec3(model * vec4(position, 1.0f));

    UV = vertexUV;
} 