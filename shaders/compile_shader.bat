@echo off
C:/VulkanSDK/1.4.313.2/Bin/glslc.exe 01_triangle.vert -o 01_triangle_vert.spv
C:/VulkanSDK/1.4.313.2/Bin/glslc.exe 01_triangle.frag -o 01_triangle_frag.spv
C:/VulkanSDK/1.4.313.2/Bin/glslc.exe directional_light_shadow.vert -o directional_light_shadow_vert.spv
C:/VulkanSDK/1.4.313.2/Bin/glslc.exe directional_light_shadow.frag -o directional_light_shadow_frag.spv

pause
