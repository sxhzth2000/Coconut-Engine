#version 450

layout(push_constant) uniform PushData {
    uint texIndex; //
} pc;
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler[2];

void main() {
    vec4 texColor = texture(texSampler[pc.texIndex], fragTexCoord);
    outColor = texColor * vec4(fragColor, 1.0);
}
