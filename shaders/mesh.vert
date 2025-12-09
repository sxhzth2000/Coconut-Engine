#version 450










layout(location = 0) in vec3 in_position; // for some types as dvec3 takes 2 locations
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_tangent;
layout(location = 3) in vec2 in_texcoord;



layout(set = 0 , binding = 1 ) readonly buffer _unused_name_perframe
{
    mat4 proj_view_matrix;
    vec3 camera_position;
};
