#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec2 inUV;


layout(binding = 0) uniform UniformBufferObject_main {
    mat4 model;
    mat4 view_proj;
} ubo;


layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragWorldPos;
layout(location = 3) out vec3 fragNormal;


void main() {
    // 计算世界坐标
    vec4 worldPos = ubo.model * vec4(inPos, 1.0);
    fragWorldPos = worldPos.xyz;


    // 计算裁剪空间位置
    gl_Position =  ubo.view_proj * worldPos;
    fragTexCoord = inUV;
    fragNormal= inNormal;

}
