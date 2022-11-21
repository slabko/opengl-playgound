#version 460

in vec3 position;
in vec3 normal;
in vec2 uv;
in float glow;

uniform mat4   model;
uniform mat4   view;
uniform mat4   proj;

uniform vec3   light_position;

out vec3 v_normal;
out vec2 v_uv;
out vec3 v_fragment_position;
out float v_glow;

void main()
{
    v_uv = uv;
    v_glow = glow;
    v_normal = normal;

    gl_Position = proj * view * model * vec4(position, 1.0);
    v_fragment_position = vec3(model * vec4(position, 1.0));
}
