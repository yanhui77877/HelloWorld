cmake_minimum_required(VERSION 3.24)
project(hello_window LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 所有第三方源码均随仓库提供；配置阶段不联网，也不依赖仓库外目录。
set(GLFW_LIBRARY_TYPE STATIC CACHE STRING "Build bundled GLFW statically" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/glfw)

# glad: 去 https://glad.dav1d.de 生成（GL 4.6 / Core / C-C++），
# 把 zip 里的 include/ 和 src/glad.c 放到 third_party/glad/ 下
add_library(glad STATIC third_party/glad/src/glad.c)
target_include_directories(glad PUBLIC third_party/glad/include)

add_library(stb_image STATIC third_party/stb/stb_image.cpp)
target_include_directories(stb_image SYSTEM PUBLIC third_party/stb)

find_package(OpenGL REQUIRED)

add_executable(dickman
  "src/main.cpp"
  "src/camera/Camera.cpp"
  "src/camera/Camera.h"
  "src/buffer/Buffer.cpp"
  "src/buffer/Buffer.h"
  "src/vertex_array/VertexArray.cpp"
  "src/vertex_array/VertexArray.h"
  "src/shader/Shader.cpp"
  "src/shader/Shader.h"
  "src/texture/Texture2D.cpp"
  "src/texture/Texture2D.h"
  "src/math/GraphicsMath.h"
  "src/light/Light.h"
  "src/light/Light.cpp"
)
target_link_libraries(dickman PRIVATE glfw glad stb_image OpenGL::GL)

target_compile_features(dickman PRIVATE cxx_std_17)
target_compile_definitions(dickman PRIVATE
  HELLO_SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders"
  HELLO_TEXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/textures"
)
file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert"
  "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag"
  "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.geom"
)
set_source_files_properties(${SHADER_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
target_sources(dickman PRIVATE ${SHADER_FILES})

# 为 src 下的每个头文件目录添加搜索路径，支持 #include <Camera.h>。
file(GLOB_RECURSE PROJECT_HEADERS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)
set(PROJECT_HEADER_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(PROJECT_HEADER_NAMES)
foreach(PROJECT_HEADER IN LISTS PROJECT_HEADERS)
  get_filename_component(PROJECT_HEADER_DIR "${PROJECT_HEADER}" DIRECTORY)
  get_filename_component(PROJECT_HEADER_NAME "${PROJECT_HEADER}" NAME)
  string(TOLOWER "${PROJECT_HEADER_NAME}" PROJECT_HEADER_KEY)
  if(PROJECT_HEADER_KEY IN_LIST PROJECT_HEADER_NAMES)
    message(FATAL_ERROR "Duplicate project header name: ${PROJECT_HEADER_NAME}. Flat includes require unique names.")
  endif()
  list(APPEND PROJECT_HEADER_NAMES "${PROJECT_HEADER_KEY}")
  list(APPEND PROJECT_HEADER_DIRS "${PROJECT_HEADER_DIR}")
endforeach()
list(REMOVE_DUPLICATES PROJECT_HEADER_DIRS)
target_include_directories(dickman PRIVATE ${PROJECT_HEADER_DIRS})

# Windows 下默认输出到 build/Debug，DLL 版 glfw 会自动被拷贝
if (MSVC)
  target_compile_options(dickman PRIVATE /W4 /utf-8)
endif()

## ── ① 契约与身份 ──
#cmake_minimum_required(VERSION 3.24)
#project(MyApp VERSION 1.0.0 LANGUAGES CXX)
#
## ── ② 全局默认（能不用就不用，见后文警告）──
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)        # 关掉 gnu++17，锁死标准
#
## ── ③ 依赖 ──
#find_package(Threads REQUIRED)       # 系统库
#add_subdirectory(third_party/foo)    # 源码内嵌
## FetchContent / find_package(CONFIG) 按需
#
## ── ④ 目标 ──
#add_library(mylib STATIC src/a.cpp src/b.cpp)
#target_include_directories(mylib PUBLIC include)
#target_link_libraries(mylib PRIVATE Threads::Threads)
#
#add_executable(app src/main.cpp)
#target_link_libraries(app PRIVATE mylib)
#
## ── ⑤ 编译选项 ──
#if (MSVC)
#  target_compile_options(mylib PRIVATE /W4 /permissive-)
#else()
#  target_compile_options(mylib PRIVATE -Wall -Wextra)
#endif()
#
## ── ⑥ 可选增强 ──
#include(GNUInstallDirs)
#install(TARGETS app mylib RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
