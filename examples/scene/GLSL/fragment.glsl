#version 460

in vec3 v_normal;
in vec2 v_uv;
in vec3 v_fragment_position;
out vec4 frag_color;

uniform sampler2D diffuse_texture;
uniform sampler2D specular_texture;

uniform vec3 light_position;
uniform vec3 camera_position;

uniform struct {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
} light = {
{ 1.0, 1.0, 1.0 },
{ 1.0, 1.0, 1.0 },
{ 1.0, 1.0, 1.0 }
};


uniform struct {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
} material;

void main()
{
    // ambient
    vec3 ambient = light.ambient * material.ambient;

    // diffuse
    vec3 norm = normalize(v_normal);
    vec3 light_dir = normalize(light_position - v_fragment_position);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse * texture(diffuse_texture, v_uv).rgb);

    // specular
    vec3 view_dir = normalize(camera_position - v_fragment_position);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0F), material.shininess);
    vec3 specular = light.specular * (spec * material.specular * vec3(texture(specular_texture, v_uv).r));

    vec3 res = ambient + diffuse + specular;

    frag_color = vec4(res, 1.0);
}