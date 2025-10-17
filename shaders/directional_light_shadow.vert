#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec2 inUV;


layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view_proj;
} ubo;



void main() {

    vec4 worldPos = ubo.model * vec4(inPos, 1.0);
    vec4 position = ubo.view_proj * worldPos;

    gl_Position =  position;

}
