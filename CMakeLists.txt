cmake_minimum_required(VERSION 3.28.6)


set(ROJECT_NAME "Coconut")


# 设置所有可执行文件输出到项目根目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})


project(${ROJECT_NAME}

        DESCRIPTION "Learn Vulakm"
)


set(CMAKE_PREFIX_PATH
        "C:/3rdparty"
)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20")
endif (MSVC)
########################################################
# MSVC
########################################################
if (MSVC)
    # msvc utf-8
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")


endif ()


add_subdirectory(src)
