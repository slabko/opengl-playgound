#version 460

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 v_normal;
out vec2 v_uv;
out vec3 v_fragment_position;

void main()
{
    v_normal = normal;

    gl_Position = proj * view * model * vec4(position, 1.0);
    v_fragment_position = vec3(model * vec4(position, 1.0));
}
