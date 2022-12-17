#version 460

in vec3 v_normal;
in vec2 v_uv;
in float v_glow;
in vec3 v_fragment_position;
out vec4 frag_color;

uniform vec3 light_position;

vec3 object_color = vec3(1.0, 0.5, 0.31);
vec3 light_color = vec3(1.0, 1.0, 1.0);
float ambient_strength = 0.1F;

uniform vec3 camera_position;

float specular_strength = 0.7;

void main()
{
    vec3 ambient = ambient_strength * light_color;

    vec3 norm = normalize(v_normal);

    vec3 light_dir = normalize(light_position - v_fragment_position);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    vec3 view_dir = normalize(camera_position - v_fragment_position);
    vec3 reflect_dir = reflect(-light_dir, norm);

    float spec = pow(max(dot(view_dir, reflect_dir), 0.0F), 64.0F);
    vec3 specular = specular_strength * spec * light_color;

    vec3 res = (ambient + diffuse + specular) * object_color;

    frag_color = vec4(res, 1.0) + v_glow;
}