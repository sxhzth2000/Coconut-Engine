#version 450

layout(location = 0) in vec3 inPos;


layout(binding = 0) uniform UniformBufferObject {
    vec3 color;

} ubo;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position =  vec4(inPos, 1.0);
    fragColor = ubo.color ;
}
