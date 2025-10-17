#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragWorldPos;
layout(location = 3) in vec3 fragNormal;
layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

//而 sampler2DShadow 的采样坐标必须在 [0,1]
layout(binding = 2) uniform sampler2DShadow shadow_map_Sampler;

//光源投影矩阵
layout(binding = 3) uniform UniformBufferObject_shadow {
    mat4 model;
    mat4 view_proj;
} ubo;






void main() {
    // 光源空间  用的
    vec4 light_space_pos = ubo.view_proj * vec4 (fragWorldPos,1.0);
    // NDC 转换  [-1,1]
    light_space_pos /= light_space_pos.w;

    ///
    vec3 shadowCoord = light_space_pos.xyz ;

    // 检查是否在光源视锥体内（防止超出范围采样）
    bool inside_light_frustum =
    light_space_pos.x >= -1.0 && light_space_pos.x <= 1.0 &&
    light_space_pos.y >= -1.0 && light_space_pos.y <= 1.0 &&
    light_space_pos.z >= 0.0  && light_space_pos.z <= 1.0; // Vulkan NDC z


    vec2 uv = shadowCoord.xy * 0.5 + 0.5;   //xy 映射  [-1,1]->[0,1]
    float depth = shadowCoord.z;  //保持原来深度
    //
    float not_in_shadow=1;





    if (inside_light_frustum) {

        //片元深度是否小于 阴影贴图深度， 1 为是，0 为否
        // 返回一代表深度 较小，说明不再阴影中，光照贡献就大

        // 深度比较采样，自动判断是否在阴影中
        // 深度偏移可以防止 self-shadow
        not_in_shadow = texture(shadow_map_Sampler, vec3(uv, depth - 0.015));
    }

    // 取出物体纹理颜色
    vec4 texColor = texture(texSampler, fragTexCoord);

    // 混合阴影与纹理颜色
    // 简单做法：阴影区调暗
    vec3 lighting = vec3(0.3) + vec3(0.7) * not_in_shadow;  // 阴影中更暗
    outColor = vec4(texColor.rgb * lighting, 1.0);

}