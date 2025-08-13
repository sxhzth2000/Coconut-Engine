
#VULKAN
find_package(Vulkan REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glslang CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

set(CCN_SOURCE_DIR

        framework/Application.cpp
        framework/Application.h
        framework/common.h
        framework/struct.h
        utils/geometries.hpp
        utils/math.cpp
        utils/math.hpp
        utils/shaders.cpp
        utils/shaders.hpp
        utils/utils.hpp
        utils/utils.cpp
        utils/stb_image.h
        framework/Texture.cpp
        framework/Texture.h
        framework/vulkan/VulkanGraphicsPipeline.cpp
        framework/vulkan/VulkanGraphicsPipeline.h
        framework/Shader.cpp
        framework/Shader.h
        framework/vulkan/VulkanDescriptor.cpp
        framework/vulkan/VulkanDescriptor.h
        framework/Buffer.cpp
        framework/Buffer.h
        framework/Camera.h
        framework/Camera.cpp
        framework/ModelLoader.cpp
        framework/ModelLoader.h
        framework/Model.cpp
        framework/Model.h
        framework/Object.cpp
        framework/Object.h
        framework/Mesh.cpp
        framework/Mesh.h
        framework/Reference.h
        framework/vulkan/VulkanContext.cpp
        framework/vulkan/VulkanContext.h
        framework/vulkan/VulkanSwapChain.cpp
        framework/vulkan/VulkanSwapChain.h
        framework/vulkan/VulkanRenderPass.cpp
        framework/vulkan/VulkanRenderPass.h
        framework/common.cpp
        3dpart/imgui/imconfig.h
        3dpart/imgui/imgui.cpp
        3dpart/imgui/imgui.h
        3dpart/imgui/imgui_demo.cpp
        3dpart/imgui/imgui_draw.cpp
        3dpart/imgui/imgui_internal.h
        3dpart/imgui/imgui_tables.cpp
        3dpart/imgui/imgui_widgets.cpp
        3dpart/imgui/imstb_rectpack.h
        3dpart/imgui/imstb_textedit.h
        3dpart/imgui/imstb_truetype.h
        3dpart/imgui/backends/imgui_impl_vulkan.cpp
        3dpart/imgui/backends/imgui_impl_vulkan.h
        3dpart/imgui/backends/imgui_impl_glfw.cpp
        3dpart/imgui/backends/imgui_impl_glfw.h

)

set(CCN_INCLUDE_DIR
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/3dpart/imgui

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
        assimp::assimp
)
add_subdirectory(example)

