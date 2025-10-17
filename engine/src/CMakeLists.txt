
#VULKAN
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glslang CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

set(CCN_SOURCE_DIR
        runtime/function/render/window_system.cpp
        runtime/function/render/window_system.h
        runtime/function/global/global_context.cpp
        runtime/function/global/global_context.h
        runtime/engine.cpp
        runtime/engine.h
        runtime/function/render/interface/rhi.h
        runtime/function/render/interface/vulkan/vulkan_rhi.h
        runtime/function/render/interface/vulkan/vulkan_rhi.cpp
        runtime/function/render/render_system.cpp
        runtime/function/render/render_system.h
        runtime/core/log/log_system.h
        runtime/core/log/log_system.h
        runtime/core/log/log_system.cpp
        runtime/core/base/macro.h
        runtime/function/render/render_type.h
        runtime/function/render/interface/rhi_struct.h
        runtime/core/base/hash.h
        runtime/function/render/render_pipeline_base.cpp
        runtime/function/render/render_pipeline_base.h

        runtime/function/render/pass/main_camera_pass.h
        runtime/function/render/pass/main_camera_pass.cpp
        runtime/function/render/render_pass_base.h
        runtime/function/render/render_pass_base.cpp
        runtime/function/render/render_pass.h
        runtime/function/render/render_pass.cpp
        runtime/function/render/interface/vulkan/vulkan_rhi_struct.h
        runtime/function/render/interface/vulkan/vulkan_util.h
        runtime/function/render/render_pipeline.cpp
        runtime/function/render/render_pipeline.h
        runtime/function/render/render_mesh.h
        runtime/function/render/interface/vulkan/vulkan_util.cpp
        runtime/function/render/render_common.h
        runtime/function/render/render_resource_base.cpp
        runtime/function/render/render_resource_base.h
        runtime/function/render/render_resource.h
        runtime/function/render/render_resource.cpp
        runtime/function/render/pass/directional_light_pass.cpp
        runtime/function/render/pass/directional_light_pass.h


)

set(CCN_INCLUDE_DIR
        ${CMAKE_CURRENT_LIST_DIR}/
        ${CMAKE_SOURCE_DIR}/engine/3dpart/
)

add_library(CCN STATIC
        ${CCN_SOURCE_DIR}
)

target_include_directories(CCN PUBLIC
        ${CCN_INCLUDE_DIR}
)

target_link_libraries(CCN PUBLIC
        Vulkan::Vulkan
        glslang::glslang
        glfw
        glm::glm
       # assimp::assimp
)
add_subdirectory(editor)

